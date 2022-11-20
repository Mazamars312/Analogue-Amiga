	.section	.text
	.global _main:
_main:
	stdec	r6

	.liabs	l2
	stdec	r6

	.lipcrel _puts
	add	r7

	li	4
	add	r6

	li	0
	mr	r0
	ldinc	r6
	mr	r7

	.section	.rodata
l2:
	.ascii	"\033[32mHello world!\033[0m\n"
	.byte 0

