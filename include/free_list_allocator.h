#pragma once
#include <tuple>
#include <cstdint>
#include "fiber_allocator.h"

using size_t = std::size_t;

namespace fibers
{
    class free_list_allocator : public fiber_allocator
    {
    public:
        struct header
        {
            size_t size;
            size_t alignment;
        };

        static constexpr size_t header_size = sizeof(header);

        struct node
        {
            size_t size;
            node* next;
        };

        explicit free_list_allocator(size_t size);
        ~free_list_allocator() override;

        void* allocate(size_t size, size_t alignment) override;
        void deallocate(void* address) override;
        void reset() override;

        inline void* allocate(size_t size) {
            return allocate(size, 16);
        }

    private:
        void initialize();
        void defragment();
        std::tuple<node*, node*> find_first_fit(size_t size, size_t alignment);
        void insert_after(node* insert, node* previous);
        void remove_after(node* remove, node* previous);

        inline uintptr_t align_address(uintptr_t address, std::size_t alignment);

        template<class T>
        inline T* align_pointer(T* pointer, std::size_t alignment);

    private:
        node* head_;
        size_t size_;
        void* memory_block_;

    
    };
}
