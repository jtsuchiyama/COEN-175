main:
#Generating function prologue
pushq	%rbp
movq	%rsp, %rbp
subq	$16, %rsp
#Spilling parameters
#Generating body
movl	$10, -4(%rbp)
movl	-4(%rbp), %edi
call	init_array
movl	-4(%rbp), %edi
call	print_array
#Generating function epilogue
movq	%rbp, %rsp
popq	%rbp
ret
#Outputting global directive
.globl	main
.comm	a, 40
