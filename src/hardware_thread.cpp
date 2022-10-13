#include "../include/hardware_thread.h"

#include <iostream>
#if __has_include(<Windows.h>)
#include <Windows.h>
#endif

namespace fibers
{

    hardware_thread::hardware_thread(size_t affinity) : thread_affinity(affinity)
    {
        set_affinity(affinity);
    }

    template<class Function, class... Args>
    void hardware_thread::start(Function &&f, Args &&... args) {
        os_thread = std::thread(std::forward<Function>(f), std::forward<Args>(args)...);
        set_affinity(thread_affinity);
    }

    void hardware_thread::set_affinity(size_t affinity) {
        thread_affinity = affinity;
#ifdef _WIN64
        const auto w_affinity = DWORD_PTR(1) << affinity;
        void* handle = reinterpret_cast<void *>(os_thread.native_handle());
        DWORD_PTR result = SetThreadAffinityMask(handle, w_affinity);
        if (result == 0)
        {
            DWORD err = GetLastError();
            std::cerr << "SetThreadAffinityMask failed, GLE=" << err << "\n";
        }
#endif

    }
}