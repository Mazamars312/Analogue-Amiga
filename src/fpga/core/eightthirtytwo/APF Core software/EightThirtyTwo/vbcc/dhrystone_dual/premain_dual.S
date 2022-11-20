//
//  More comprehensive premain implementation.
//  The rudimentary premain in start.S has weak linkage, so this one will override it if linked.
//
//  Supports auto initialisation from .ctors section and clearing of BSS data.
//
//  STACKSIZE is defined to 1K, but is a weak symbol so can be overridden either in another section
//  or on the linker command line.


	.section	.text.premain
	.global	_premain
	.constant _STACKSIZE,1024
	.global _STACKSIZE
	.weak	_STACKSIZE
_premain:
	// Setup the stack
	.liabs	__bss_end__
	mr	r6
	.liabs	_STACKSIZE
	add	r6

	// FIXME - should push argc and argv on the stack before calling main.
	.lipcrel	_main
	add	r7

.endloop:
	cond NEX	// Pause until an interrupt is received
	.lipcrel .endloop
	add	r7


	.section	.text.premain2
	.global	_premain2
_premain2:
	// Setup the stack
	.liabs	__bss_end__
	mr	r6
	.liabs	_STACKSIZE
	add	r6
	add r6	// 2nd thread's stack must be beyond the first one

	// FIXME - should wait for the first thread to finish clearing BSS.

	// FIXME - should push argc and argv on the stack before calling main.
	.lipcrel	_thread2main
	add	r7

.endloop2:
	cond NEX	// Pause until an interrupt is received
	.lipcrel .endloop2
	add	r7

