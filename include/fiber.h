#pragma once
#include <functional>
#include <chrono>

#include "context.h"
#include "free_list_allocator.h"
#include "job.h"

namespace fibers
{
    // The fiber class simply represents a stack space and a context.
    // Additionally, this class is used to hold various information including the current job being executed.
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
        volatile bool started = false;
        volatile bool completed = false;
        void* stack = nullptr;
        context return_ctx{};
        job* current_job = nullptr;
    };
}


