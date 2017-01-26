#include <DynamicPoolAllocator.h>
#include <cstdlib>

//#include <iostream> // DEBUG

using namespace std;

//
// Utility functions
//

void DynamicPoolAllocator::increasePools() {
    // Double the number of pools
    pools = (void**) realloc(pools, sizeof(void*) * numPools * 2);
    for (int i = numPools; i < numPools * 2; ++i) {
        pools[i] = malloc(poolSize);
    }
    numPools *= 2;
}

void DynamicPoolAllocator::decreasePools() {
    // Half the number of pools
    if (numPools == 1) return;

    for (int i = numPools/2; i < numPools; ++i) {
        free(pools[i]);
    }
    pools = (void**) realloc(pools, sizeof(void*) * numPools / 2);
    numPools /= 2;
}

void DynamicPoolAllocator::consolidatePools() {
    // Half the number of pools, but double their size
    if (numPools == 1) return;

    decreasePools();

    for (int i = 0; i < numPools; ++i) {
        // free->malloc instead of realloc, we dont care about the contents
        // (this should only be done after a freeAll anyway...)
        free(pools[i]);
        pools[i] = malloc(poolSize * 2);
    }

    poolSize *= 2;
}

void DynamicPoolAllocator::splitPools() {
    // Half the size of pools, don't increase number of pools
    if (poolSize / 2 < originalPoolSize) return;

    // Resize the pools we currently have
    for (int i = 0; i < numPools; ++i) {
        // Again, free->malloc instead of realloc
        free(pools[i]);
        pools[i] = malloc(poolSize / 2);
    }

    poolSize /= 2;
}

//
// Interface
//

DynamicPoolAllocator::DynamicPoolAllocator(size_t poolSize) {
    this->pools = (void**) malloc(sizeof(void*));
    this->pools[0] = malloc(poolSize);

    this->numPools = 1;
    this->poolSize = poolSize;
    this->originalPoolSize = poolSize;

    this->filledPools = 0;
    this->currentPoolAllocatedSize = 0;
}

DynamicPoolAllocator::~DynamicPoolAllocator() {
    for (int i = 0; i < numPools; ++i) {
       free(pools[i]);
    }
    free(pools);
}

void* DynamicPoolAllocator::alloc(size_t size) {
    if (size > poolSize) {
        return NULL;
    }

    // If size is 0 we should return null to be safe(r)
    if (size == 0) {
        return NULL;
    }

    if (currentPoolAllocatedSize + size > poolSize) {
        ++filledPools;
        currentPoolAllocatedSize = 0;
        if (filledPools == numPools) {
            increasePools();
        }
    }

    unsigned int currentPool = filledPools;

    void* ret = (void*)((char*) pools[currentPool] + currentPoolAllocatedSize);
    currentPoolAllocatedSize += size;
    return ret;
}

void DynamicPoolAllocator::freeAll() {
    // This is unreachable
    /*
    if (numPools == 1 && currentPoolAllocatedSize < poolSize/4) {
        // Using < 1/4 available space, free some by splitting the pool
        splitPools();
    }
    */

    if (numPools == 2 && currentPoolAllocatedSize < poolSize/2 && filledPools == 0) {
        // Using < 1/4 available space, free some by splitting the pool (if possible)
        if (poolSize > originalPoolSize) {
            splitPools();
        }
        else {
            decreasePools();
        }
    }
    if (numPools >= 4) {
        // Either want to consolidate down, or drop pools
        if (filledPools + 1 <= numPools / 4) {
            // Using < 1/4 of the available space
            decreasePools();
        }
        else {
            // Have lots of pools, but we are using the space
            consolidatePools();
        }
    }

    if (numPools == 1 && currentPoolAllocatedSize < poolSize/4) {
        // Again, using < 1/4 of available space, split the pool
        splitPools();
    }

    filledPools = 0;
    currentPoolAllocatedSize = 0;
}

/*
void DynamicPoolAllocator::printDebugData() {
    cout << "number of pools:\t" << numPools << endl;
    cout << "pool size:\t\t" << poolSize << endl;
    cout << "filled pools:\t\t" << filledPools << endl;
    cout << "filled space:\t\t" << currentPoolAllocatedSize << endl << endl;
}
*/
