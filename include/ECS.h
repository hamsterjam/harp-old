#ifndef __HARP_ECS_H
#define __HARP_ECS_H
#include <cstddef>
#include <initializer_list>
#include <vector>
#include <queue>

#include <DynamicPoolAllocator.h>

typedef unsigned int Entity;
typedef unsigned int Component;

class ECS {
    private:
        struct ChangeRequest {
            Entity ent;
            Component comp;
            void* val;
            bool remove;
        };

        Entity    nextEnt;
        Component nextComp;

        unsigned int entVecLength;
        unsigned int compVecLength;

        std::queue<Entity> entRecycleQueue;
        std::queue<ChangeRequest> changeQueue;

        DynamicPoolAllocator changePool;

        std::size_t* compSize;
        bool** hasComp;
        void** data;

    public:
        class EntityIterator {
            private:
                friend class ECS;

                std::vector<Component> comps;
                Entity ent;
                ECS* par;
            public:
                EntityIterator(std::initializer_list<Component> compList, Entity ent, ECS* par);
                bool operator==(EntityIterator rhs);
                bool operator!=(EntityIterator rhs) {return !((*this)==rhs);}
                EntityIterator operator++();    // Prefix
                EntityIterator operator++(int); // Postfix
                Entity operator*() {return ent;}
        };

        ECS(unsigned int entVecLength, unsigned int compVecLength, std::size_t poolSize);
        ~ECS();

        Entity createEntity();
        void deleteEntity(Entity ent);

        Component createComponentType(std::size_t size);

        void  setComponent(Entity ent, Component comp, void* val);
        void* getComponent(Entity ent, Component comp);
        void  removeComponent(Entity ent, Component comp);

        // Zero sized components
        //
        // Technically these work by passing in NULL for the value and 0 for the size, but
        // adding in some wrappers makes it more... nice? I guess?

        Component createFlagComponentType();
        void setFlagComponent(Entity ent, Component flag, bool val);
        bool getFlagComponent(Entity ent, Component flag);

        void  updateComponents();

        EntityIterator begin(std::initializer_list<Component> comps);
        EntityIterator end();
};

#endif
