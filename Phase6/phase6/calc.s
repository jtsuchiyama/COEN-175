lexan:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$lexan.size, %eax
	subq	%rax, %rsp
#If::generate
#Expression::test
#Equal::generate
	movl	c, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L0
#Assignment::generate
	movl	$0, %eax
	call	getchar
	movl	%eax, %edi
	movl	%edi, c
.L0:
#While::generate
.L2:
#Expression::test
#LogicalAnd::generate
#Expression::test
	movl	c, %edi
	movl	$0, %eax
	call	isspace
	cmpl	$0, %eax
	je	.L4
#Expression::test
#NotEqual::generate
#Cast::generate
	movl	c, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$10, %rax
	setne	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L4
	movl	$1, %eax
	jmp	.L5
.L4:
	movl	$0, %eax
.L5:
	cmpl	$0, %eax
	je	.L3
#Assignment::generate
	movl	$0, %eax
	call	getchar
	movl	%eax, %edi
	movl	%edi, c
	jmp	.L2
.L3:
#If::generate
#Expression::test
#Not::generate
	movl	c, %edi
	movl	$0, %eax
	call	isdigit
	movl	%eax, %edi
	cmpl	$0, %edi
	sete	%dil
	movzbl	%dil, %edi
	cmpl	$0, %edi
	je	.L6
#Assignment::generate
	movl	c, %eax
	movl	%eax, -8(%rbp)
#Assignment::generate
	movl	$0, %eax
	movl	%eax, c
#Return::generate
	movl	-8(%rbp), %eax
	jmp	lexan.exit
.L6:
#Assignment::generate
	movl	$0, %eax
	movl	%eax, -4(%rbp)
#While::generate
.L8:
#Expression::test
	movl	c, %edi
	movl	$0, %eax
	call	isdigit
	cmpl	$0, %eax
	je	.L9
#Assignment::generate
#Subtract::generate
#Add::generate
#Multiply::generate
	movl	-4(%rbp), %eax
	imull	$10, %eax
	addl	c, %eax
	subl	$48, %eax
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
#Assignment::generate
	movl	c, %edi
	movl	$0, %eax
	call	getchar
	movl	%eax, %edi
	movl	%edi, c
	jmp	.L8
.L9:
#Assignment::generate
	movl	-4(%rbp), %eax
	movl	%eax, lexval
#Return::generate
	movl	NUM, %eax
	jmp	lexan.exit

lexan.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	lexan.size, 16
	.globl	lexan

match:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$match.size, %eax
	subq	%rax, %rsp
	movl	%edi, -4(%rbp)
#If::generate
#Expression::test
#NotEqual::generate
	movl	lookahead, %eax
	cmpl	-4(%rbp), %eax
	setne	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L10
#Address::generate
	leaq	.L13, %rax
	movl	lookahead, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$1, %edi
	movl	$0, %eax
	call	exit
.L10:
#Assignment::generate
	call	lexan
	movl	%eax, %edi
	movl	%edi, lookahead

match.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	match.size, 16
	.globl	match

factor:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$factor.size, %eax
	subq	%rax, %rsp
#If::generate
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$40, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L14
#Cast::generate
	movq	$40, %rax
	movl	%eax, %edi
	call	match
#Assignment::generate
	movl	$0, %eax
	call	expr
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
#Cast::generate
	movq	$41, %rax
	movl	%eax, %edi
	call	match
#Return::generate
	movl	-4(%rbp), %eax
	jmp	factor.exit
.L14:
#Assignment::generate
	movl	lexval, %eax
	movl	%eax, -4(%rbp)
	movl	NUM, %edi
	call	match
#Return::generate
	movl	-4(%rbp), %eax
	jmp	factor.exit

factor.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	factor.size, 16
	.globl	factor

term:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$term.size, %eax
	subq	%rax, %rsp
#Assignment::generate
	call	factor
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
#While::generate
.L16:
#Expression::test
#LogicalOr::generate
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$42, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	jne	.L18
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$47, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	jne	.L18
	movl	$0, %eax
	jmp	.L19
.L18:
	movl	$1, %eax
.L19:
	cmpl	$0, %eax
	je	.L17
#If::generate
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$42, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L20
#Cast::generate
	movq	$42, %rax
	movl	%eax, %edi
	call	match
#Assignment::generate
#Multiply::generate
	call	factor
	movl	-4(%rbp), %edi
	imull	%eax, %edi
	movl	%edi, %eax
	movl	%eax, -4(%rbp)
	jmp	.L21
.L20:
#Cast::generate
	movq	$47, %rax
	movl	%eax, %edi
	call	match
#Assignment::generate
#Divide::generate
	call	factor
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	cltd
	idivl	%ecx
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
.L21:
	jmp	.L16
.L17:
#Return::generate
	movl	-4(%rbp), %eax
	jmp	term.exit

term.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	term.size, 16
	.globl	term

expr:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$expr.size, %eax
	subq	%rax, %rsp
#Assignment::generate
	call	term
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
#While::generate
.L22:
#Expression::test
#LogicalOr::generate
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$43, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	jne	.L24
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$45, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	jne	.L24
	movl	$0, %eax
	jmp	.L25
.L24:
	movl	$1, %eax
.L25:
	cmpl	$0, %eax
	je	.L23
#If::generate
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$43, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L26
#Cast::generate
	movq	$43, %rax
	movl	%eax, %edi
	call	match
#Assignment::generate
#Add::generate
	call	term
	movl	-4(%rbp), %edi
	addl	%eax, %edi
	movl	%edi, %eax
	movl	%eax, -4(%rbp)
	jmp	.L27
.L26:
#Cast::generate
	movq	$45, %rax
	movl	%eax, %edi
	call	match
#Assignment::generate
#Subtract::generate
	call	term
	movl	-4(%rbp), %edi
	subl	%eax, %edi
	movl	%edi, %eax
	movl	%eax, -4(%rbp)
.L27:
	jmp	.L22
.L23:
#Return::generate
	movl	-4(%rbp), %eax
	jmp	expr.exit

expr.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	expr.size, 16
	.globl	expr

main:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$main.size, %eax
	subq	%rax, %rsp
#Assignment::generate
	movl	$256, %eax
	movl	%eax, NUM
#Assignment::generate
	call	lexan
	movl	%eax, %edi
	movl	%edi, lookahead
#While::generate
.L28:
#Expression::test
#NotEqual::generate
#Negate::generate
	movl	$1, %eax
	negl	%eax
	movl	lookahead, %edi
	cmpl	%eax, %edi
	setne	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L29
#Assignment::generate
	call	expr
	movl	%eax, %edi
	movl	%edi, -4(%rbp)
#Address::generate
	leaq	.L31, %rax
	movl	-4(%rbp), %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
#While::generate
.L32:
#Expression::test
#Equal::generate
#Cast::generate
	movl	lookahead, %eax
	movslq	%eax, %rdi
	movq	%rdi, %rax
	cmpq	$10, %rax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L33
#Cast::generate
	movq	$10, %rax
	movl	%eax, %edi
	call	match
	jmp	.L32
.L33:
	jmp	.L28
.L29:

main.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	main.size, 16
	.globl	main

	.comm	NUM, 4
	.comm	lookahead, 4
	.comm	c, 4
	.comm	lexval, 4
	.data
.L31:	.asciz	"%d\012"
.L13:	.asciz	"syntax error at %d\012"
