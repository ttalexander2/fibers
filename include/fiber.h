#pragma once
#include <functional>
#include <chrono>

#include "context.h"
#include "free_list_allocator.h"

namespace fibers
{
    struct fiber
    {
        friend class fiber_manager;

        ~fiber() = default;
        fiber(const fiber&) = delete;
        fiber& operator= (const fiber&) = delete;

        [[nodiscard]] inline size_t get_id() const noexcept { return id; }

    private:
        fiber();

        size_t id{};
        context ctx{};
        std::function<void()> function;
        bool started = false;
        bool completed = false;
    };
}


