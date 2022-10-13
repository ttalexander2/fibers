#pragma once

#include <emmintrin.h>

namespace fibers
{
    struct context
    {
        void *rip, *rsp;
        void *rbx, *rbp, *r12, *r13, *r14, *r15, *rdi, *rsi;
        __m128i xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
    };


    void set_context(context* context);
    void get_context(context* context);
    void swap_context(context* old_context, context* new_context);
}



