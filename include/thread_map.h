//
// Created by Thomas on 10/11/2022.
//

#ifndef FIBERS_THREAD_MAP_H
#define FIBERS_THREAD_MAP_H

#include <map>
#include "primitives/spinlock.h"
#include "fiber_thread.h"

namespace fibers
{
    // This class is meant for internal use in the fiber_manager, and should not be used elsewhere.
    // This class holds a map of hardware threads, indexed by their ID.
    // It allows for *inefficient* thread-safe writes, and simultaneous read operations.
    class thread_map {
        std::atomic_bool lock_{false};
        std::map<size_t, fiber_thread*> map_;

    public:
        void write(size_t id, fiber_thread* thread) noexcept;
        bool try_write(size_t id, fiber_thread* thread) noexcept;
        bool contains(size_t id) noexcept;
        fiber_thread* get(size_t id) noexcept;
    };
}



#endif //FIBERS_THREAD_MAP_H
