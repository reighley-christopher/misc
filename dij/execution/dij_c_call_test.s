	.file	"dij_c_call_test.c"
	.section	.rodata
	.align 8
.LC0:
	.string	"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n"
	.text
	.globl	test_func
	.type	test_func, @function
test_func:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movl	$.LC0, %eax
	movq	-40(%rbp), %r9
	movq	-32(%rbp), %r8
	movq	-24(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rsi
	movq	48(%rbp), %rdi
	movq	%rdi, 40(%rsp)
	movq	40(%rbp), %rdi
	movq	%rdi, 32(%rsp)
	movq	32(%rbp), %rdi
	movq	%rdi, 24(%rsp)
	movq	24(%rbp), %rdi
	movq	%rdi, 16(%rsp)
	movq	16(%rbp), %rdi
	movq	%rdi, 8(%rsp)
	movq	-48(%rbp), %rdi
	movq	%rdi, (%rsp)
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$100, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	test_func, .-test_func
	.section	.rodata
.LC1:
	.string	"%ld %ld"
	.text
	.globl	test_func_short
	.type	test_func_short, @function
test_func_short:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$.LC1, %eax
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	$-1, %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	test_func_short, .-test_func_short
	.section	.rodata
.LC2:
	.string	"%lx"
	.text
	.globl	test_func_var
	.type	test_func_var, @function
test_func_var:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$224, %rsp
	movq	%rsi, -168(%rbp)
	movq	%rdx, -160(%rbp)
	movq	%rcx, -152(%rbp)
	movq	%r8, -144(%rbp)
	movq	%r9, -136(%rbp)
	testb	%al, %al
	je	.L4
	movaps	%xmm0, -128(%rbp)
	movaps	%xmm1, -112(%rbp)
	movaps	%xmm2, -96(%rbp)
	movaps	%xmm3, -80(%rbp)
	movaps	%xmm4, -64(%rbp)
	movaps	%xmm5, -48(%rbp)
	movaps	%xmm6, -32(%rbp)
	movaps	%xmm7, -16(%rbp)
.L4:
	movq	%rdi, -216(%rbp)
	movl	$8, -208(%rbp)
	movl	$48, -204(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -200(%rbp)
	leaq	-176(%rbp), %rax
	movq	%rax, -192(%rbp)
	movl	$0, -180(%rbp)
	jmp	.L5
.L8:
	movl	-208(%rbp), %eax
	cmpl	$48, %eax
	jae	.L6
	movq	-192(%rbp), %rdx
	movl	-208(%rbp), %eax
	movl	%eax, %eax
	addq	%rdx, %rax
	movl	-208(%rbp), %edx
	addl	$8, %edx
	movl	%edx, -208(%rbp)
	jmp	.L7
.L6:
	movq	-200(%rbp), %rdx
	movq	%rdx, %rax
	addq	$8, %rdx
	movq	%rdx, -200(%rbp)
.L7:
	movq	(%rax), %rdx
	movl	$.LC2, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	addl	$1, -180(%rbp)
.L5:
	movl	-180(%rbp), %eax
	cltq
	cmpq	-216(%rbp), %rax
	jl	.L8
	movl	$10, %edi
	call	putchar
	movq	-216(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	test_func_var, .-test_func_var
	.section	.rodata
.LC3:
	.string	"Hello %s"
.LC4:
	.string	"World"
.LC5:
	.string	"hello %ld\n"
.LC6:
	.string	"goodbye %x\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$208, %rsp
	movl	%edi, -196(%rbp)
	movq	%rsi, -208(%rbp)
	movq	$1, -192(%rbp)
	movq	$2, -184(%rbp)
	movq	$3, -176(%rbp)
	movq	$4, -168(%rbp)
	movq	$5, -160(%rbp)
	movq	$6, -152(%rbp)
	movq	$7, -144(%rbp)
	movq	$8, -136(%rbp)
	movq	$9, -128(%rbp)
	movq	$10, -120(%rbp)
	movq	$11, -112(%rbp)
	movq	$12, -48(%rbp)
	movq	$13, -40(%rbp)
	movq	$5, -96(%rbp)
	movq	$4, -88(%rbp)
	movq	$3, -80(%rbp)
	movq	$2, -72(%rbp)
	movq	$1, -64(%rbp)
	movl	$.LC3, %eax
	movq	%rax, -32(%rbp)
	movl	$.LC4, %eax
	movq	%rax, -24(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, %rdx
	movl	$88, %esi
	movl	$test_func, %edi
	call	dij_c_call
	movq	%rax, -8(%rbp)
	movl	$.LC5, %eax
	movq	-8(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	leaq	-48(%rbp), %rax
	movq	%rax, %rdx
	movl	$16, %esi
	movl	$test_func_short, %edi
	call	dij_c_call
	movq	%rax, -8(%rbp)
	movl	$.LC6, %eax
	movq	-8(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$0, %r9d
	movl	$1, %r8d
	movl	$2, %ecx
	movl	$3, %edx
	movl	$4, %esi
	movl	$5, %edi
	movl	$0, %eax
	call	test_func_var
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$48, %esi
	movl	$test_func_var, %edi
	call	dij_c_call
	movq	%rax, -8(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, %rdx
	movl	$16, %esi
	movl	$printf, %edi
	call	dij_c_call
	movq	%rax, -8(%rbp)
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
