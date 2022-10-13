#include "../include/thread_map.h"


namespace fibers
{

    // Here we are using an atomic bool to mimic the behavior of a spinlock.
    // Writing to the map will prevent reads, without putting the thread to sleep.
    // However, reading from the map is not mutually exclusive, since reading from a std::map
    // should be thread safe. Reading will only be prevented if another thread is writing to the map,
    // which *should* never happen, since we strictly control when this map is populated
    // in the initialization step in the fiber_manager.



    void thread_map::write(size_t id, fiber_thread* thread) noexcept {
        for (;;) {
            if (!lock_.exchange(true, std::memory_order_acquire)) {
                break;
            }

            auto result = lock_.load(std::memory_order_relaxed);
            while (result) {
                result = lock_.load(std::memory_order_relaxed);
            }
        }
        map_[id] = thread;
        lock_.store(false,  std::memory_order_acquire);
    }

    bool thread_map::try_write(size_t id, fiber_thread* thread) noexcept {
        if (lock_.load(std::memory_order_relaxed) &&
            !lock_.exchange(true, std::memory_order_acquire))
        {
            map_[id] = thread;
            lock_.store(false,  std::memory_order_acquire);
        }
    }

    bool thread_map::contains(size_t id) noexcept {
        for (;;)
        {
            if (lock_.load(std::memory_order_relaxed)) {
                continue;
            }
            return map_.find(id) != map_.end();
        }
    }

    fiber_thread* thread_map::get(size_t id) noexcept {
        for (;;)
        {
            if (lock_.load(std::memory_order_relaxed)) {
                continue;
            }
            if (map_.find(id) == map_.end())
                return nullptr;
            // We use find() and at() here since the indexing operator[] is considered a write.
            return map_.at(id);
        }
    }
}