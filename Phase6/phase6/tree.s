insert:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$insert.size, %eax
	subq	%rax, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
#If::generate
#Expression::test
#Not::generate
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	sete	%al
	movzbl	%al, %rax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L0
#Assignment::generate
#Multiply::generate
	movq	$3, %rax
	imulq	$8, %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	malloc
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq	-16(%rbp), %rdi
	movq	%rax, %rsi
	movq	%rdi, (%rsi)
#Assignment::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq	null, %rdi
	movq	%rax, %rsi
	movq	%rdi, (%rsi)
#Assignment::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq	null, %rdi
	movq	%rax, %rsi
	movq	%rdi, (%rsi)
	jmp	.L1
.L0:
#If::generate
#Expression::test
#LessThan::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rdi
	cmpq	%rax, %rdi
	setl	%al
	movzbl	%al, %eax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L2
#Assignment::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, %rdi
	call	insert
#Add::generate
	movq	-8(%rbp), %rdi
	addq	$8, %rdi
	movq	%rax, %rsi
	movq	%rdi, %rax
	movq	%rsi, (%rax)
	jmp	.L3
.L2:
#If::generate
#Expression::test
#GreaterThan::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rdi
	cmpq	%rax, %rdi
	setg	%al
	movzbl	%al, %eax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L4
#Assignment::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, %rdi
	call	insert
#Add::generate
	movq	-8(%rbp), %rdi
	addq	$16, %rdi
	movq	%rax, %rsi
	movq	%rdi, %rax
	movq	%rsi, (%rax)
	jmp	.L5
.L4:
.L5:
.L3:
.L1:
#Return::generate
	movq	-8(%rbp), %rax
	jmp	insert.exit

insert.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	insert.size, 16
	.globl	insert

search:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$search.size, %eax
	subq	%rax, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
#If::generate
#Expression::test
#Not::generate
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	sete	%al
	movzbl	%al, %rax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L6
#Return::generate
	movl	$0, %eax
	jmp	search.exit
	jmp	.L7
.L6:
.L7:
#If::generate
#Expression::test
#LessThan::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rdi
	cmpq	%rax, %rdi
	setl	%al
	movzbl	%al, %eax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L8
#Return::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, %rdi
	call	search
	jmp	search.exit
	jmp	.L9
.L8:
.L9:
#If::generate
#Expression::test
#GreaterThan::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rdi
	cmpq	%rax, %rdi
	setg	%al
	movzbl	%al, %eax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L10
#Return::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq		(%rax), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, %rdi
	call	search
	jmp	search.exit
	jmp	.L11
.L10:
.L11:
#Return::generate
	movl	$1, %eax
	jmp	search.exit

search.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	search.size, 16
	.globl	search

preorder:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$preorder.size, %eax
	subq	%rax, %rsp
	movq	%rdi, -8(%rbp)
#If::generate
#Expression::test
yourmom
	movq	-8(%rbp), %rax
yourmom1
	cmpq	$0, %rax
	je	.L12
#Assignment::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	movq	%rdi, -16(%rbp)
#Dereference::generate
	movq	-16(%rbp), %rax
	movl		(%rax), %eax
#Address::generate
	leaq	.L15, %rdi
	movl	%eax, %esi
	movl	$0, %eax
	call	printf
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	call	preorder
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	call	preorder
	jmp	.L13
.L12:
.L13:

preorder.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	preorder.size, 16
	.globl	preorder

inorder:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$inorder.size, %eax
	subq	%rax, %rsp
	movq	%rdi, -8(%rbp)
#If::generate
#Expression::test
yourmom
	movq	-8(%rbp), %rax
yourmom1
	cmpq	$0, %rax
	je	.L16
#Assignment::generate
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$0, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	movq	%rdi, -16(%rbp)
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$8, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	call	inorder
#Dereference::generate
	movq	-16(%rbp), %rax
	movl		(%rax), %eax
#Address::generate
	leaq	.L15, %rdi
	movl	%eax, %esi
	movl	$0, %eax
	call	printf
#Dereference::generate
#Add::generate
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq		(%rax), %rax
	movq	%rax, %rdi
	call	inorder
	jmp	.L17
.L16:
.L17:

inorder.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	inorder.size, 16
	.globl	inorder

main:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$main.size, %eax
	subq	%rax, %rsp
#Assignment::generate
	movl	$0, %eax
	movl	%eax, -52(%rbp)
#While::generate
.L19:
#Expression::test
#LessThan::generate
	movl	-52(%rbp), %eax
	cmpl	$8, %eax
	setl	%al
	movzbl	%al, %eax
yourmom
yourmom1
	cmpl	$0, %eax
	je	.L20
#Assignment::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
#Multiply::generate
#Cast::generate
	movl	-52(%rbp), %edi
	movslq	%edi, %rsi
	imulq	$4, %rsi
	addq	%rsi, %rax
	movl	-52(%rbp), %edi
	movq	%rax, %rsi
	movl	%edi, (%rsi)
#Assignment::generate
#Add::generate
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %edi
	movl	%edi, -52(%rbp)
	jmp	.L19
.L20:
#Assignment::generate
	movq	null, %rax
	movq	%rax, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$28, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$4, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$0, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$20, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$8, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$12, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Assignment::generate
#Address::generate
#Add::generate
#Address::generate
	leaq	-48(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	insert
	movq	%rax, %rdi
	movq	%rdi, -8(%rbp)
#Address::generate
	leaq	.L22, %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	-8(%rbp), %rdi
	call	preorder
#Address::generate
	leaq	.L24, %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	-8(%rbp), %rdi
	call	inorder

main.exit:
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.set	main.size, 64
	.globl	main

	.comm	null, 8
	.data
.L15:	.asciz	"%d\012"
.L24:	.asciz	"inorder traversal:\012"
.L22:	.asciz	"preorder traversal:\012"
