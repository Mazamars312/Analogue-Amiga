	// setjmp() / longjmp() implementation for 832
	
	// int setjmp(jmp_buf env) - environment pointer in r1, result in r0

	// jumpbuf structure:
	// 0: return address
	// 4: r3
	// 8: r4
	// 12: r5
	// 16: r6

	.section .text.setjmp
	.global _setjmp
_setjmp:
	mr	r0	// Save return address to r0
	st	r1	// And also the jmpbuf structure, then save non-scratch registers to jmpbuf
	li	20
	add	r1
	exg	r1
	stmpdec	r6
	stmpdec	r5
	stmpdec	r4
	stmpdec	r3

	li	0
	exg	r0
	mr r7	// Return with 0 in r0
	

	// int longjmp(jmp_buf env, int val) - environment pointer in r1, val in (r6), result in r0

	.global _longjmp
_longjmp:
	li	0
	ldidx	r6
	mr	r0	// Place return value straight into r0
	ldinc	r1
	mr	r2		// Save return address temporarily to r2
	ldinc	r1
	mr	r3
	ldinc	r1
	mr	r4
	ldinc	r1
	mr	r5
	ldinc	r1
	mr	r6
	mt	r2	// Move return address
	mr	r7
	
