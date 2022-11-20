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
	.constant STACKSIZE,1024
	.weak	STACKSIZE
_premain:
	// Setup the stack
	.liabs	__bss_end__
	mr	r6
	.liabs	STACKSIZE
	add	r6

	// Clear BSS data
	ldinc	r7
	.ref	__bss_end__
	mr	r1
	ldinc	r7
	.ref	__bss_start__
	mr	r0
.clrloop:
	mt	r0
	cmp	r1
	cond	NEQ
		li	0
		stbinc	r0	// FIXME - faster to clear by word, but need to clear any remainder bytewise.
		.lipcrel .clrloop
		add	r7

	// Call constructors
	ldinc	r7
	.ref	__ctors_end__
	mr	r4
	ldinc	r7
	.ref	__ctors_start__
	mr	r5
.ctorloop:	// Call in reverse order
	mt	r4
	cmp	r5
	cond	EQ
		.lipcrel .ctordone
		add	r7

	li	4
	sub	r4
	ld	r4
	exg	r7

	.lipcrel .ctorloop
	add	r7

.ctordone:
	// FIXME - should push argc and argv on the stack before calling main.
	.lipcrel	_main
	add	r7

	// Call destructors
	ldinc	r7
	.ref	__dtors_start__
	mr	r4
	ldinc	r7
	.ref	__dtors_end__
	mr	r5
.dtorloop:
	mt	r4
	cmp	r5
	cond	EQ
		.lipcrel .endloop
		add	r7

	ldinc	r4
	exg	r7

	.lipcrel .dtorloop
	add	r7

.endloop:
	cond NEX	// Pause until an interrupt is received
	.lipcrel .endloop
	add	r7

