	.section .text.startup

// Define STACKTOP.  This will be used from within the minimal premain code
// so will be ignored if you link in the more complete premain code which sets
// the stack to a fixed position beyond the BSS section.
// Declared with weak linkage so you can override it if you wish.

	.constant STACKTOP,0x2000
	.weak	STACKTOP

_start:
	cond	SGT	// Z flag and C flag both clear -> Thread 1
		.lipcrel .start1
		add	r7

	cond	SLT	// Z flag clear, C flag set -> Thread 2
		.lipcrel .start2
		add	r7

	// By elimination the Z flag must be set, so this must be an interrupt.
	// (The interrupt is triggered using the xor alu op, so C is not cleared.)

	exg	r6
	stmpdec	r0	// Store r0 first since we need a register for decrementing the return address
	stmpdec	r6	// Now save the return address
	stmpdec	r1	// Save r1
	stmpdec	r2	// R2 is now a scratch register, so save that too.
	exg	r6

	ldinc	r7

	.global _InterruptVector	// This word can be written to install an int handler.  Location 13.
_InterruptVector:
	.ref	_interrupt

	exg	r7

	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r1
	ldinc	r6
	mr	r0		// Return address temporarily into r0
	li	-1
	add	r0		// Decrement return address
	ldinc r6
	exg	r0		// Restore r0
	mr	r7		// Jump to return address - 1.


.start1:
	// Set up the stack here
	ldinc	r7
	.ref	_premain
	mr	r7
.start2:
	// Set up the stack here
	ldinc	r7
	.ref	_premain2
	mr	r7

	.section	.text.premain
	.weak	_premain
	.globl _premain
_premain:
	// Setup the stack

	ldinc	r7
	.ref	STACKTOP
	mr	r6

	// Clear BSS here
	ldinc	r7
	.ref	_main
	exg	r7
.endloop:
	cond NEX	// Pause until an interrupt is received
	.lipcrel	.endloop
	add	r7

	.section	.text.premain2
	.weak	_premain2
_premain2:
.endloop2:
	cond NEX	// Pause until an interrupt is received
	.lipcrel	.endloop2
	add	r7

	.section	.text.interrupt
	.weak	_interrupt
_interrupt:
	mr	r7	// return immediately

