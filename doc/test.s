	.text
	.file	"test.ll"
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// BB#0:                                // %main
	mov	 w0, wzr
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
