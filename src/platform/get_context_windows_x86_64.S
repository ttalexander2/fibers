get_context:
   # Save the return address and stack pointer.
    movq (%rsp), %r8
    movq %r8, 8*0(%rcx)
    leaq 8(%rsp), %r8
    movq %r8, 8*1(%rcx)
    # Save preserved registers.
    movq %rbx, 8*2(%rcx)
    movq %rbp, 8*3(%rcx)
    movq %r12, 8*4(%rcx)
    movq %r13, 8*5(%rcx)
    movq %r14, 8*6(%rcx)
    movq %r15, 8*7(%rcx)
    movq %rdi, 8*8(%rcx)
    movq %rsi, 8*9(%rcx)
    movups %xmm6, 8*10+16*0(%rcx)
    movups %xmm7, 8*10+16*1(%rcx)
    movups %xmm8, 8*10+16*2(%rcx)
    movups %xmm9, 8*10+16*3(%rcx)
    movups %xmm10, 8*10+16*4(%rcx)
    movups %xmm11, 8*10+16*5(%rcx)
    movups %xmm12, 8*10+16*6(%rcx)
    movups %xmm13, 8*10+16*7(%rcx)
    movups %xmm14, 8*10+16*8(%rcx)
    movups %xmm15, 8*10+16*9(%rcx)
    # Return.
    xorl %eax, %eax
    ret
    