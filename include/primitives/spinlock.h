#pragma once
#include <atomic>

namespace fibers
{
    struct spinlock
    {
        void lock() noexcept;
        bool try_lock() noexcept;
        void unlock() noexcept;
    private:
        std::atomic<bool> lock_ {false};
    };
}


