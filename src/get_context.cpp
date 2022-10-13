namespace fibers
{
    struct context;
}

//Defines platform as executable
#pragma section(".text")
__declspec(allocate(".text"))
static unsigned char get_context_code[] = {
    0x4c ,0x8b ,0x04 ,0x24 ,                                    // mov    (%rsp),%r8
    0x4c ,0x89 ,0x01 ,                                          // mov    %r8,(%rcx)
    0x4c ,0x8d ,0x44 ,0x24 ,0x08 ,                              // lea   0x8(%rsp),%r8
    0x4c ,0x89 ,0x41 ,0x08 ,                                    // mov    %r8,0x8(%rcx)
    0x48 ,0x89 ,0x59 ,0x10 ,                                    // mov    %rbx,0x10(%rcx)
    0x48 ,0x89 ,0x69 ,0x18 ,                                    // mov    %rbp,0x18(%rcx)
    0x4c ,0x89 ,0x61 ,0x20 ,                                    // mov    %r12,0x20(%rcx)
    0x4c ,0x89 ,0x69 ,0x28 ,                                    // mov    %r13,0x28(%rcx)
    0x4c ,0x89 ,0x71 ,0x30 ,                                    // mov    %r14,0x30(%rcx)
    0x4c ,0x89 ,0x79 ,0x38 ,                                    // mov    %r15,0x38(%rcx)
    0x48 ,0x89 ,0x79 ,0x40 ,                                    // mov    %rdi,0x40(%rcx)
    0x48 ,0x89 ,0x71 ,0x48 ,                                    // mov    %rsi,0x48(%rcx)
    0x0f ,0x11 ,0x71 ,0x50 ,                                    // movups %xmm6,0x50(%rcx)
    0x0f ,0x11 ,0x79 ,0x60 ,                                    // movups %xmm7,0x60(%rcx)
    0x44 ,0x0f ,0x11 ,0x41 ,0x70 ,                              // movups %xmm8,0x70(%rcx)
    0x44 ,0x0f ,0x11 ,0x89 ,0x80 ,0x00 ,0x00 ,                  // movups %xmm9,0x80(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0x91 ,0x90 ,0x00 ,0x00 ,                  // movups %xmm10,0x90(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0x99 ,0xa0 ,0x00 ,0x00 ,                  // movups %xmm11,0xa0(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0xa1 ,0xb0 ,0x00 ,0x00 ,                  // movups %xmm12,0xb0(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0xa9 ,0xc0 ,0x00 ,0x00 ,                  // movups %xmm13,0xc0(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0xb1 ,0xd0 ,0x00 ,0x00 ,                  // movups %xmm14,0xd0(%rcx)
    0x00 ,                                                      // 
    0x44 ,0x0f ,0x11 ,0xb9 ,0xe0 ,0x00 ,0x00 ,                  // movups %xmm15,0xe0(%rcx)
    0x00 ,                                                      // 
    0x31 ,0xc0 ,                                                // xor    %eax,%eax
    0xc3 ,                                                      // retq
};

//Function pointer for platform get_context code
static void (*get_context_internal)(fibers::context*) = (void (*)(fibers::context*))&get_context_code;