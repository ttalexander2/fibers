#include <cstdlib>
#include "../include/free_list_allocator.h"


namespace fibers
{
    free_list_allocator::free_list_allocator(size_t size)
        : head_(nullptr), size_(size), memory_block_(nullptr)
    {
        initialize();
    }

    free_list_allocator::~free_list_allocator()
    {
        free(reinterpret_cast<void*>(memory_block_));
    }

    void free_list_allocator::initialize()
    {
        memory_block_ = reinterpret_cast<uint8_t*>(malloc(size_));
        reset_impl();
    }

    void* free_list_allocator::allocate_impl(size_t size, size_t alignment /* = 16*/)
    {
        const auto result = find_first_fit(size, alignment);
        node* current_node = std::get<0>(result);
        node* previous_node = std::get<1>(result);

        const size_t remaining_size = current_node->size - size;
        if (remaining_size > 0)
        {
            node* n = reinterpret_cast<node*>(reinterpret_cast<size_t>(current_node) - size);
            *n = node { remaining_size, nullptr };
            insert_after(n, current_node);
        }

        remove_after(current_node, previous_node);

        const size_t alignment_padding = alignment - header_size;
        const size_t header_address = reinterpret_cast<size_t>(current_node) + alignment_padding;
        header* h = reinterpret_cast<header*>(header_address);
        *h = header{size, alignment_padding};

        return reinterpret_cast<void*>(header_address + header_size);
    }

    void free_list_allocator::deallocate_impl(void* address)
    {
        const std::size_t current_address = reinterpret_cast<std::size_t>(address);
        const std::size_t header_address = current_address - header_size;
	
        header* h = reinterpret_cast<header*>(header_address);
        node* n = reinterpret_cast<node*>(header_address);
        *n = node{ h->size + h->alignment, nullptr };

        node* current = head_;
        node* previous = nullptr;

        if (current == nullptr)
        {
            insert_after(n, head_);
        }
        else
        {
            while (current != nullptr)
            {
                if (current > address)
                {
                    break;
                }

                previous = current;
                current = current->next;
            }

            insert_after(n, previous);
        }

        defragment();
    }

    void free_list_allocator::reset_impl()
    {
        node* n = reinterpret_cast<node*>(memory_block_);
        *n = node{ size_, nullptr };

        head_ = nullptr;
        insert_after(n, nullptr);
    }



    void free_list_allocator::defragment()
    {
        node* current = head_;
        while (current != nullptr && current->next != nullptr)
        {
            if (reinterpret_cast<std::size_t>(current) + current->size == reinterpret_cast<std::size_t>(current->next))
            {
                current->size += current->next->size;
                remove_after(current->next, current);
            }

            current = current->next;
        }
    }

    std::tuple<free_list_allocator::node*, free_list_allocator::node*> free_list_allocator::find_first_fit(size_t size, size_t alignment)
    {
        node* current_node = head_;
        node* previous_node = nullptr;

        size = align_address(size + header_size, alignment);

        while (current_node != nullptr)
        {
            if (current_node->size >= size)
            {
                break;
            }

            previous_node = current_node;
            current_node = current_node->next;
        }

        return std::make_tuple(current_node, previous_node);
    }

    void free_list_allocator::insert_after(node* insert, node* previous)
    {
        if (previous == nullptr)
        {
            insert->next = head_;
            head_ = insert;
        }
        else
        {
            insert->next = previous->next;
            previous->next = insert;

        }
    }

    void free_list_allocator::remove_after(node* remove, node* previous)
    {
        if (previous == nullptr)
        {
            head_ = remove->next;
        }
        else
        {
            previous->next = remove->next;
        }
    }

    uintptr_t free_list_allocator::align_address(uintptr_t address, std::size_t alignment)
    {
        const size_t mask = alignment - 1;
        return (address + mask) & ~mask;
    }

    template <class T>
    T* free_list_allocator::align_pointer(T* pointer, std::size_t alignment)
    {
        const uintptr_t address = reinterpret_cast<uintptr_t>(pointer);
        const uintptr_t aligned_address = align_address(address, alignment);
        return reinterpret_cast<T*>(aligned_address);
    }
}
