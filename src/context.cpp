#include "../include/context.h"

#include "get_context.cpp"
#include "set_context.cpp"
#include "swap_context.cpp"

#if __has_include(<valgrind/valgrind.h>)
#include <valgrind/valgrind.h>
#define HAS_VALGRIND
#endif



/**
// These macros are defined by GCC and/or clang
#if defined(__SANITIZE_ADDRESS__) || __has_feature(address_sanitizer)
extern "C" {
    // Check out sanitizer/asan-interface.h in compiler-rt for documentation.
    void __sanitizer_start_switch_fiber(void** fake_stack_save, const void* bottom, size_t size);
    void __sanitizer_finish_switch_fiber(void* fake_stack_save, const void** bottom_old, size_t* size_old);
}
#define HAS_ASAN
#endif

// These macros are defined by GCC and/or clang
#if defined(__SANITIZER_THREAD__) || __has_feature(thread_sanitizer)
extern "C" {
    // Check out sanitizer/tsan_interface.h in compiler-rt for documentation.
    void* __tsan_get_current_fiber(void);
    void* __tsan_create_fiber(unsigned flags);
    void __tsan_destroy_fiber(void* fiber);
    void __tsan_switch_to_fiber(void* fiber, unsigned flags);
}
#define HAS_TSAN
#endif
 */


namespace fibers
{
    void set_context(context* context)
    {
        set_context_internal(context);
    }

    void get_context(context* context)
    {
        get_context_internal(context);
    }

    void swap_context(context* old_context, context* new_context)
    {


        #if defined(HAS_ASAN)
            // This pointer will live on this stack frame.
            void* fake_stack_save = nullptr;
            __sanitizer_start_switch_fiber(
                &fake_stack_save,
                stack_data,
                stack_size);
        #endif


            // This needs to be stored somewhere that the context we switch to will have
            // access to it so it can switch back to it. This means it cannot be on this
            // stack frame. We could store it in the stack frame of the context we switch
            // to for example. Or we could just store it on the heap.
        #if defined(HAS_TSAN)
            void* this_fiber = __tsan_get_current_fiber();

            // Here we just make a new fiber and immediately switch to it.
            void* next_fiber = __tsan_create_fiber(0);
            __tsan_switch_to_fiber(next_fiber, nullptr);
        #endif

            // Before context switch, register our stack with Valgrind.
        #if defined(HAS_VALGRIND)
            unsigned stack_id = VALGRIND_STACK_REGISTER(stack_data, stack_data + stack_size);
        #endif


            // The context switch happens here in the middle.
            //
            // It's important at this point that |old_context| and |new_context| are not
            // on the stack, otherwise TSAN will complain since we just told it we've
            // switched stacks.
            //
            // We CANNOT call |__tsan_switch_to_fiber| as the first thing in the context we
            // switch to. The documentation explicitly forbids this.
            swap_context_internal(old_context, new_context);

            // We've returned from the context switch, we can now throw that stack out.
        #if defined(HAS_VALGRIND)
            VALGRIND_STACK_DEREGISTER(stack_id);
        #endif

            // Somewhere in the context we just switch to, when switching back to this
            // context we need to restore the fiber stored in |this_fiber|.
            //
            // So just before the |swap_context| we just call |__tsan_switch_to_fiber| again.
            // this time restoring it.
            //
            // __tsan_switch_to_fiber(this_fiber, nullptr);

            // We've returned from the context switch, we can now throw |this_fiber| out.
        #if defined(HAS_TSAN)
            __tsan_destroy_fiber(this_fiber);
        #endif

        #if defined(HAS_ASAN)
            // When we return from the context switch we indicate that to ASAN.
            void* bottom_old = nullptr;
            size_t size_old = 0;
            __sanitizer_finish_switch_fiber(
                fake_stack_save,
                &bottom_old,
                &size_old);
        #endif

            // Here you can assert |bottom_old| and |size_old| are what you expect when
            // returning to this context.
            //
            // You can also pass NULL for the last two arguments if you don't care.
    }
}



