f:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$f.size, %eax
	subq	%rax, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
#Assignment::generate
#Add::generate
#Multiply::generate
here
	movl	-8(%rbp), %eax
	imull	-12(%rbp), %eax
here
	movl	-4(%rbp), %edi
	addl	%eax, %edi
here
	movl	%edi, %eax
	movl	%eax, x

f.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	f.size, 16
	.globl	f

	.comm	x, 4
	.data
