	.section	.text.0
	.global	_putchar
_putchar:
	stdec	r6

	li	-4
	add	r6

	.liconst -64

	mr	r1
l5: # 
	ld	r1
	mr	r0

	.liconst 256
	and	r0

	li	4
	addt	r6
	stmpdec	r0

	ld	r6

	cond	EQ

	.lipcrel l5
		add	r7

	li	8
	ldidx	r6

	st	r1

	mr	r0

	li	-4
	sub	r6
	ldinc	r6
	mr	r7


	.section	.text.0
	.global	_puts
_puts:
	exg	r6
	stmpdec	r6
	stmpdec	r3
	stmpdec	r4
	exg	r6

	li	12
	ldidx	r6

	mr	r3

	li	0

	mr	r4

	ldbinc	r3

	mr	r2

	.liconst 255
	and	r2


	mt	r2
	and	r2


	cond	EQ

	.lipcrel l12
	add	r7
l11: # 
	mt	r2
	mr	r1

	mt	r1
	stdec	r6

	ldinc	r7
	.ref	_putchar
	exg	r7

	li	4
	add	r6

	li	1
	add	r4

	ldbinc	r3

	mr	r2

	.liconst 255
	and	r2

	mt	r2
	and	r2

	cond	NEQ

	.lipcrel l11
	add	r7
l12: # 

	mt	r4
	mr	r0

	ldinc	r6
	mr	r4
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r7

