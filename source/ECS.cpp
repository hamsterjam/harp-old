#include <ECS.h>

#include <cstdlib>
#include <cstring> // for memcpy

#include <DynamicPoolAllocator.h>

using namespace std;

ECS::ECS(unsigned int entVecLength, unsigned int compVecLength, size_t poolSize):
        changePool(poolSize) {
    this->entVecLength  = entVecLength;
    this->compVecLength = compVecLength;

    nextEnt  = 0;
    nextComp = 0;

    // Allocate arrays
    compSize = (size_t*) malloc(sizeof(size_t) * compVecLength);
    hasComp = (bool**) malloc(sizeof(bool*) * compVecLength);
    data = (void**) malloc(sizeof(void*) * compVecLength);

    for (int i=0; i < compVecLength; ++i) {
        hasComp[i] = (bool*) malloc(sizeof(bool) * entVecLength);
    }

    // hasComp will be init'd by createEntity
}

ECS::~ECS() {
    free(compSize);

    for (int i=0; i < nextComp; ++i) {
        free(hasComp[i]);
        free(data[i]);
    }

    free(hasComp);
    free(data);
}

//
// Entity functions
//

Entity ECS::createEntity() {
    // Check our recycle queue first
    if (!entRecycleQueue.empty()) {
        // Don't have to resize or init anything, that's all taken care of when it
        // was originally init'd
        Entity ret = entRecycleQueue.front();
        entRecycleQueue.pop();
        return ret;
    }

    // Resize arrays if we need to
    if (nextEnt >= entVecLength) {
        entVecLength *= 2;
        for(int i=0; i < nextComp; ++i) {
            hasComp[i] = (bool*) realloc(hasComp[i], sizeof(bool) * entVecLength);
            data[i] = realloc(data[i], compSize[i] * entVecLength);
        }
    }

    // init the new ent's entries in hasComp
    for (int i=0; i < nextComp; ++i) {
        hasComp[i][nextEnt] = false;
    }

    return nextEnt++;
}

void ECS::deleteEntity(Entity ent) {
    // Just setting it to having no components should be good enough
    for (int i=0; i < nextComp; ++i) {
        hasComp[i][ent] = false;
    }

    // Add the entity to the recycle queue
    entRecycleQueue.push(ent);
}

//
// Component Functions
//

Component ECS::createComponentType(size_t size) {
    // Resize arrays if we need to
    if (nextComp >= compVecLength) {
        compVecLength *= 2;

        compSize = (size_t*) realloc(compSize, sizeof(size_t) * compVecLength);
        hasComp = (bool**) realloc(hasComp, sizeof(bool*) * compVecLength);
        data = (void**) realloc(data, sizeof(void*) * compVecLength);

        for (int i = nextComp; i < compVecLength; ++i) {
            hasComp[i] = (bool*) malloc(sizeof(bool) * entVecLength);
        }
    }

    // Make sure all entities don't have this
    for (int i=0; i < nextEnt; ++i) {
        hasComp[nextComp][i] = false;
    }

    // Allocate our data yo
    compSize[nextComp] = size;
    data[nextComp] = malloc(size * entVecLength);

    return nextComp++;
}

void ECS::setComponent(Entity ent, Component comp, void* val) {
    // We need to allocate new memory for the data so we have ownership

    size_t size = compSize[comp];
    void* valCopy = changePool.alloc(size);
    memcpy((char*) valCopy, val, size);

    // Create the change request and add it to the queue
    ChangeRequest req;
    req.ent = ent;
    req.comp = comp;
    req.val = valCopy;
    req.remove = false;

    changeQueue.push(req);
}

void* ECS::getComponent(Entity ent, Component comp) {
    // First thing to do is to check to see if it actually HAS the comp
    if (!hasComp[comp][ent]) {
        return NULL;
    }
    size_t size = compSize[comp];

    // Cast to char to do clean pointer arithmetic
    return ((char*) data[comp]) + size * ent;
}

void ECS::removeComponent(Entity ent, Component comp) {
    // Just set the hasComp to false, don't bother with the data

    ChangeRequest req;
    req.ent = ent;
    req.comp = comp;
    req.remove = true;

    changeQueue.push(req);
}

void ECS::updateComponents() {
    while(!changeQueue.empty()) {
        ChangeRequest req = changeQueue.front();

        if (req.remove) {
            hasComp[req.comp][req.ent] = false;
        }
        else {
            hasComp[req.comp][req.ent] = true;
            size_t size = compSize[req.comp];
            memcpy(((char*) data[req.comp]) + size * req.ent, req.val, size);
        }

        changeQueue.pop();
    }

    changePool.freeAll();
}

// Zero sized components

Component ECS::createFlagComponentType() {
    return createComponentType(0);
}

void ECS::setFlagComponent(Entity ent, Component flag, bool val) {
    if (val) {
        setComponent(ent, flag, NULL);
    }
    else {
        removeComponent(ent, flag);
    }
}

bool ECS::getFlagComponent(Entity ent, Component flag) {
    return getComponent(ent, flag);
}

//
// System functions
//

ECS::EntityIterator ECS::begin(initializer_list<Component> comps) {
    EntityIterator ret(comps, nextEnt, this);

    // Iterate through till you find an entity with the right components
    for (Entity e = 0; e < nextEnt; ++e) {

        bool exit = true;

        for (Component c : ret.comps) {
            if (!hasComp[c][e]) {
                exit = false;
                break;
            }
        }

        if (exit) {
            ret.ent = e;
            break;
        }
    }

    return ret;
}

ECS::EntityIterator ECS::end() {
    EntityIterator ret({}, nextEnt, this);
    return ret;
}

//
// EntityIterator
//

ECS::EntityIterator::EntityIterator(initializer_list<Component> compList, Entity ent, ECS* par):
    comps(compList)
{
    this->ent = ent;
    this->par = par;
}

bool ECS::EntityIterator::operator==(EntityIterator rhs) {
    EntityIterator lhs = *this;
    return lhs.ent == rhs.ent;
}

ECS::EntityIterator ECS::EntityIterator::operator++() {
    // Just iterate through to find the next one
    // I know I'm reusing code here, but fuck it

    Entity currEnt = this->ent;
    this->ent = par->nextEnt;

    for (Entity e = currEnt + 1; e < par->nextEnt; ++e) {

        bool exit = true;

        for (Component c: this->comps) {
            if (!par->hasComp[c][e]) {
                exit = false;
                break;
            }
        }

        if (exit) {
            this->ent = e;
            break;
        }
    }

    return *this;
}

ECS::EntityIterator ECS::EntityIterator::operator++(int) {
    EntityIterator ret = *this;
    ++(*this);
    return ret;
}
