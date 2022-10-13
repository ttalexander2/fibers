#pragma once
#ifndef FIBERS_MUTEX_H
#define FIBERS_MUTEX_H

#include <atomic>

namespace fibers
{
    struct mutex
    {
        void lock() noexcept;
        bool try_lock() noexcept;
        void unlock() noexcept;
    private:
        std::atomic<bool> lock_ {false};
    };
}

#endif //FIBERS_MUTEX_H
