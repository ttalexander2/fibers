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

        inline void* allocate(size_t size, size_t alignment = 16) {
            return allocate_impl(size, 16);
        }

        inline void deallocate(void* address) {
            return deallocate_impl(address);
        }

        inline void reset() {
            return reset_impl();
        }

    protected:

        virtual void* allocate_impl(size_t size, size_t alignment) = 0;
        virtual void deallocate_impl(void* address) = 0;
        virtual void reset_impl() = 0;
    };
}

#endif //FIBERS_FIBER_ALLOCATOR_H
