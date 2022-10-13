namespace fibers
{
    struct context;
}


//Defines platform as executable
#pragma section(".text")
__declspec(allocate(".text"))
static unsigned char set_context_code[] = {
    0x4c ,0x8b ,0x01 ,                                  // mov    (%rcx),%r8       
    0x48 ,0x8b ,0x61 ,0x08 ,                            // mov    0x8(%rcx),%rsp   
    0x48 ,0x8b ,0x59 ,0x10 ,                            // mov    0x10(%rcx),%rbx  
    0x48 ,0x8b ,0x69 ,0x18 ,                            // mov    0x18(%rcx),%rbp  
    0x4c ,0x8b ,0x61 ,0x20 ,                            // mov    0x20(%rcx),%r12  
    0x4c ,0x8b ,0x69 ,0x28 ,                            // mov    0x28(%rcx),%r13  
    0x4c ,0x8b ,0x71 ,0x30 ,                            // mov    0x30(%rcx),%r14  
    0x4c ,0x8b ,0x79 ,0x38 ,                            // mov    0x38(%rcx),%r15  
    0x48 ,0x8b ,0x79 ,0x40 ,                            // mov    0x40(%rcx),%rdi  
    0x48 ,0x8b ,0x71 ,0x48 ,                            // mov    0x48(%rcx),%rsi  
    0x0f ,0x10 ,0x71 ,0x50 ,                            // movups 0x50(%rcx),%xmm6 
    0x0f ,0x10 ,0x79 ,0x60 ,                            // movups 0x60(%rcx),%xmm7 
    0x44 ,0x0f ,0x10 ,0x41 ,0x70 ,                      // movups 0x70(%rcx),%xmm8 
    0x44 ,0x0f ,0x10 ,0x89 ,0x80 ,0x00 ,0x00 ,          // movups 0x80(%rcx),%xmm9 
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0x91 ,0x90 ,0x00 ,0x00 ,          // movups 0x90(%rcx),%xmm10
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0x99 ,0xa0 ,0x00 ,0x00 ,          // movups 0xa0(%rcx),%xmm11
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0xa1 ,0xb0 ,0x00 ,0x00 ,          // movups 0xb0(%rcx),%xmm12
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0xa9 ,0xc0 ,0x00 ,0x00 ,          // movups 0xc0(%rcx),%xmm13
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0xb1 ,0xd0 ,0x00 ,0x00 ,          // movups 0xd0(%rcx),%xmm14
    0x00 ,                                              //                         
    0x44 ,0x0f ,0x10 ,0xb9 ,0xe0 ,0x00 ,0x00 ,          // movups 0xe0(%rcx),%xmm15
    0x00 ,                                              //                         
    0x41 ,0x50 ,                                        // push   %r8              
    0x31 ,0xc0 ,                                        // xor    %eax,%eax        
    0xc3 ,                                              // retq                    
};

//Function pointer for platform get_context code
static void (*set_context_internal)(fibers::context*) = (void (*)(fibers::context*))&set_context_code;