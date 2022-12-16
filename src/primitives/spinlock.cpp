#include "../../include/primitives/spinlock.h"

// Based on the implementation from https://rigtorp.se/spinlock/

namespace fibers
{
    void spinlock::lock() noexcept {
        for (;;) {
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            while (lock_.load(std::memory_order_relaxed)) {
                // normally you would execute a PAUSE instruction in a normal spin-lock
                // however, since this will be used with fibers, we want to avoid PAUSE
                // as it will increase latency resulting in performance loss.
                ; // spin
            }
        }
    }

    bool spinlock::try_lock() noexcept {
        return !lock_.load(std::memory_order_relaxed) &&
                !lock_.exchange(true, std::memory_order_acquire);
    }

    void spinlock::unlock() noexcept {
        lock_.store(false,  std::memory_order_release);
    }
}

