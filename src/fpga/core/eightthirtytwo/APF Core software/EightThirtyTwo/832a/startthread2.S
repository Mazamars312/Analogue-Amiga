
	.section	.text.premain2
	.weak	_premain2
_premain2:
	ldinc	r7
	.liabs	STACKTOP2
	mr	r6

	// Clear BSS here
	ldinc	r7
	.reloc	_thread2main
	exg	r7
.endloop2:
	cond NEX	// Pause until an interrupt is received
	li	IMW0(PCREL(.endloop2))
	add	r7

	.weak	_thread2main
_thread2main:
	mr	r7	// Return immediately


