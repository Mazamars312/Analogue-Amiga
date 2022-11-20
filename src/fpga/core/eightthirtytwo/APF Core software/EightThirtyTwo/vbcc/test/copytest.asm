	//registers used:
		//r1: yes
		//r2: yes
		//r3: yes
		//r4: no
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.2
	.global	_func
_func:
	stdec	r6
	mt	r3
	stdec	r6
						// allocreg r3
						// allocreg r1
						// Q1 disposable
		// Offsets 0, 0
		// Have am? yes, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r3 - no need to prep
						// (obj to tmp) flags 40 type a
						// reg r1 - only match against tmp
	mt	r1
						// (save temp)isreg
	mr	r3
						//save_temp done
						// freereg r1
						// allocreg r2

						//copytest.c, line 12
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r2 - no need to prep
						// (obj to tmp) flags 42 type a
						// matchobj comparing flags 66 with 64
						// reg r3 - only match against tmp
	mt	r3
						// (save temp)isreg
	mr	r2
						//save_temp done
						// allocreg r1

						//copytest.c, line 12
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l6,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//copytest.c, line 12
						//call
						//pcreltotemp
	.lipcrel	___strcmp // extern
	add	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r2
						// freereg r1
						// allocreg r1

						//copytest.c, line 12
						// (test)
						// (obj to tmp) flags 4a type 3
						// reg r0 - only match against tmp
	mt	r0
				// flags 4a
	and	r0
						// freereg r1

						//copytest.c, line 12
	cond	NEQ
						//conditional branch regular
						//pcreltotemp
	.lipcrel	l5
		add	r7
						// allocreg r2
						// allocreg r1

						//copytest.c, line 13
						// (bitwise/arithmetic) 	//ops: 0, 0, 1
						// (obj to r0) flags 2 type 3
						// matchobj comparing flags 2 with 74
						//static not varadr
						//statictotemp (FIXME - make PC-relative?)
	.liabs	l1,0
						//static deref
						//sizemod based on type 0x3
	ldt
	mr	r0
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 2
						// matchobj comparing flags 1 with 2
						// const
						// matchobj comparing flags 1 with 2
						// matchobj comparing flags 1 with 2
	.liconst	1
	add	r0
						// (save result) // not reg
						// Store_reg to type 0x3, flags 0x2
						// (prepobj tmp)
 						// static
	.liabs	l1,4
						// static pe not varadr
	stmpdec	r0
 						// WARNING - check that 4 has been added.
l5: # 

						//copytest.c, line 14
						//setreturn
						// (obj to r0) flags 1 type 3
						// const
	.liconst	0
	mr	r0
						// freereg r1
						// freereg r2
						// freereg r3
	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: yes
		//r2: yes
		//r3: no
		//r4: no
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.3
	.global	_main
_main:
	stdec	r6
	.liconst	-16
	add	r6
						// allocreg r2

						//copytest.c, line 31
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// (obj to r0) flags 2 type c
						// extern
	.liabs	_mystruct
	mr	r0
						// (prepobj r1)
 						// matchobj comparing flags 130 with 2
						// matchobj comparing flags 130 with 2
						// var, auto|reg
	mt	r6
	mr	r1

					// Copying 4 words and 0 bytes to localcopy
					// Copying 4 words to localcopy
	ldinc	r0
	stinc	r1
	ldinc	r0
	stinc	r1
	ldinc	r0
	stinc	r1
	ldinc	r0
	stinc	r1
						// allocreg r1

						//copytest.c, line 32
		// Offsets 8, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 2 type a
						// var, auto|reg
	.liconst	8
						//sizemod based on type 0xa
	ldidx	r6
						// (save temp)isreg
	mr	r1
						//save_temp done

						//copytest.c, line 32
						//call
						// (obj to tmp) flags 22 type f
						// matchobj comparing flags 34 with 2
						// deref 
						// (prepobj tmp)
 						// matchobj comparing flags 162 with 2
						// deref
						// var FIXME - deref?
						// reg - auto
						// matchobj comparing flags 1 with 2
	.liconst	12
	ldidx	r6
	exg	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r1
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r2 - no need to prep
						// (obj to tmp) flags 2 type 3
						//static not varadr
						//statictotemp (FIXME - make PC-relative?)
	.liabs	l1,0
						//static deref
						//sizemod based on type 0x3
	ldt
						// (save temp)isreg
	mr	r2
						//save_temp done

						//copytest.c, line 33
						// (compare) (q1 signed) (q2 signed)
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 2
						// const
						// matchobj comparing flags 1 with 2
	.liconst	1
	cmp	r2

						//copytest.c, line 33
	cond	NEQ
						//conditional branch regular
						//pcreltotemp
	.lipcrel	l11
		add	r7
						// allocreg r1

						//copytest.c, line 34
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l12,0
						// static pe is varadr
	stdec	r6

						//copytest.c, line 34
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//copytest.c, line 36
						//pcreltotemp
	.lipcrel	l13
	add	r7
l11: # 

						//copytest.c, line 36
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// reg r2 - only match against tmp
	mt	r2
	stdec	r6

						//copytest.c, line 36
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l14,0
						// static pe is varadr
	stdec	r6

						//copytest.c, line 36
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 8 + 0 bytes
	.liconst	8
	add	r6
l13: # 

						//copytest.c, line 38
						//setreturn
						// (obj to r0) flags 1 type 3
						// const
	.liconst	0
	mr	r0
						// freereg r1
						// freereg r2
						// matchobj comparing flags 1 with 1
						// matchobj comparing flags 1 with 1
	.liconst	-16
	sub	r6
	ldinc	r6
	mr	r7

	.section	.data.4
	.align	4
	.global	_mystruct
_mystruct:
	.int	42
	.int	255
						// Declaring from tree
						// static
	.ref	l7
						// Declaring from tree
						// extern (offset 0)
	.ref	_func
	.section	.rodata.5
	.align	4
l7:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	44
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.data.6
	.align	4
l1:
	.int	0
	.section	.rodata.7
	.align	4
l6:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	44
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.8
	.align	4
l12:
	.byte	27
	.byte	91
	.byte	51
	.byte	50
	.byte	109
	.byte	80
	.byte	97
	.byte	115
	.byte	115
	.byte	101
	.byte	100
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	10
	.byte	0
	.section	.rodata.9
	.align	4
l14:
	.byte	27
	.byte	91
	.byte	51
	.byte	49
	.byte	109
	.byte	70
	.byte	97
	.byte	105
	.byte	108
	.byte	101
	.byte	100
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	45
	.byte	32
	.byte	103
	.byte	111
	.byte	116
	.byte	32
	.byte	37
	.byte	100
	.byte	0
