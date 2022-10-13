#include "../../include/primitives/mutex.h"

#include "../../include/fibers.h"

namespace fibers
{
    void mutex::lock() noexcept {
        for (;;) {
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }

            while (lock_.load(std::memory_order_relaxed)) {
                // Our yield function is conditional, it will yield the current fiber if called from a fiber
                // execution thread. If it is called from another thread, it will yield that entire thread.
                // This way if the uses a mutex from a thread not controlled by the fiber system (ie. main thread)
                // it will function as a normal mutex from that context, but will interface with the fiber
                // yield system here.
                fibers::current::yield();
            }
        }
    }

    bool mutex::try_lock() noexcept {
        return !lock_.load(std::memory_order_relaxed) &&
               !lock_.exchange(true, std::memory_order_acquire);
    }

    void mutex::unlock() noexcept {
        lock_.store(false,  std::memory_order_acquire);
    }
}
