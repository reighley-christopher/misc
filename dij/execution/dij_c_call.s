dij_c_call:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	movq	%rdi, %r10 /*the function to be called*/
	movq	%rsi, %rbx /*the size in bytes of the parameter array*/
       /* movq    %rdx, %rbx /*the address of the parameter array*/
 
	addq	%rdx, %rbx /*the far end of the parameter array*/
        addq    $48, %rdx  /*the part of the address array that is going on the stack*/
       
stack_loop: 
        cmpq    %rdx, %rbx 
        jle     pass_registers    
	subq	$8, %rbx
        pushq   (%rbx)
        jmp    stack_loop   

pass_registers:
        movq    %rdx, %rbx	
	subq	$8, %rbx
	movq	(%rbx), %r9
	subq	$8, %rbx
	movq	(%rbx), %r8
	subq	$8, %rbx
	movq	(%rbx), %rcx
	subq	$8, %rbx
	movq	(%rbx), %rdx
	subq	$8, %rbx
	movq	(%rbx), %rsi
	subq	$8, %rbx
	movq	(%rbx), %rdi
        /*I do not know why clearing eax is important but it is*/
        /*gcc does it religiously, it carries some information of use*/
        /*to the varargs macro*/
        movl    $0, %eax
	call	*%r10
        movq	%rbp, %rsp
	subq	$8, %rsp
	popq	%rbx
	popq	%rbp
	ret

.globl dij_c_call
