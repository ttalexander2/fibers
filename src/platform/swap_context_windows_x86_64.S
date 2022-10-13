swap_context:
    # Save the return address.
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
    # Should return to the address set with {get, swap}_context.
    movq 8*0(%rdx), %r8
    # Load new stack pointer.
    movq 8*1(%rdx), %rsp
    # Load preserved registers.
    movq 8*2(%rdx), %rbx
    movq 8*3(%rdx), %rbp
    movq 8*4(%rdx), %r12
    movq 8*5(%rdx), %r13
    movq 8*6(%rdx), %r14
    movq 8*7(%rdx), %r15
    movq 8*8(%rdx), %rdi
    movq 8*9(%rdx), %rsi
    movups 8*10+16*0(%rdx), %xmm6
    movups 8*10+16*1(%rdx), %xmm7
    movups 8*10+16*2(%rdx), %xmm8
    movups 8*10+16*3(%rdx), %xmm9
    movups 8*10+16*4(%rdx), %xmm10
    movups 8*10+16*5(%rdx), %xmm11
    movups 8*10+16*6(%rdx), %xmm12
    movups 8*10+16*7(%rdx), %xmm13
    movups 8*10+16*8(%rdx), %xmm14
    movups 8*10+16*9(%rdx), %xmm15
    # Push RIP to stack for RET.
    pushq %r8
    # Return.
    xorl %eax, %eax
    ret
    