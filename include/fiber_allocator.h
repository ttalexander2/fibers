//
// Created by Thomas on 10/28/2022.
//

#ifndef FIBERS_FIBER_ALLOCATOR_H
#define FIBERS_FIBER_ALLOCATOR_H

namespace fibers
{
    class fiber_allocator
    {
    public:
        virtual ~fiber_allocator() = default;
        virtual void* allocate(size_t size, size_t alignment) = 0;
        virtual void deallocate(void* address) = 0;
        virtual void reset() = 0;
    };
}

#endif //FIBERS_FIBER_ALLOCATOR_H
