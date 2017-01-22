#ifndef __HARP_DYNAMIC_POOL_ALLOCATOR_H
#define __HARP_DYNAMIC_POOL_ALLOCATOR_H

#include <cstddef>

class DynamicPoolAllocator {
    private:
        void** pools;

        unsigned int numPools;
        std::size_t poolSize;
        std::size_t originalPoolSize;

        unsigned int filledPools;
        std::size_t currentPoolAllocatedSize;

        void increasePools();
        void decreasePools();
        void consolidatePools();
        void splitPools();

    public:
        DynamicPoolAllocator(std::size_t poolSize);
        ~DynamicPoolAllocator();

        void* alloc(std::size_t size);
        void freeAll();

        /*
        void printDebugData();
        */
};

#endif
