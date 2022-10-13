#pragma once

#ifndef FIBERS_HARDWARE_THREAD_H
#define FIBERS_HARDWARE_THREAD_H

#include <thread>

namespace fibers
{
    class hardware_thread {

    public:
        explicit hardware_thread(size_t affinity);

        [[nodiscard]] inline size_t get_affinity() const { return thread_affinity;}

        template<class Function, class... Args>
        void start(Function&& f, Args&&... args);

        std::thread os_thread;
    private:
        void set_affinity(size_t affinity);
        size_t thread_affinity{};
    };



}



#endif //FIBERS_HARDWARE_THREAD_H
