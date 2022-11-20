	//registers used:
		//r1: yes
		//r2: yes
		//r3: yes
		//r4: no
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.3
	.global	_f1
_f1:
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

						//fptrtest.c, line 22
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

						//fptrtest.c, line 22
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
	.liabs	l9,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//fptrtest.c, line 22
						//call
						//pcreltotemp
	.lipcrel	___strcmp // extern
	add	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r2
						// freereg r1
						// allocreg r1

						//fptrtest.c, line 22
						// (test)
						// (obj to tmp) flags 4a type 3
						// reg r0 - only match against tmp
	mt	r0
				// flags 4a
	and	r0
						// freereg r1

						//fptrtest.c, line 22
	cond	NEQ
						//conditional branch regular
						//pcreltotemp
	.lipcrel	l8
		add	r7
						// allocreg r2
						// allocreg r1

						//fptrtest.c, line 23
						// (bitwise/arithmetic) 	//ops: 0, 0, 1
						// (obj to r0) flags 2 type 3
						// matchobj comparing flags 2 with 74
						//static not varadr
						//statictotemp (FIXME - make PC-relative?)
	.liabs	l3,0
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
	.liabs	l3,4
						// static pe not varadr
	stmpdec	r0
 						// WARNING - check that 4 has been added.
l8: # 

						//fptrtest.c, line 24
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
		//r2: no
		//r3: yes
		//r4: no
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.4
	.global	_call
_call:
	stdec	r6
	mt	r3
	stdec	r6
						// allocreg r1

						//fptrtest.c, line 29
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l12,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//fptrtest.c, line 29
						//call
						//pcreltotemp
	.lipcrel	l1 // Static 
	addt	r7	//Deref function pointer
	ldt
	exg	r7
						// Deferred popping of 0 bytes (0 in total)
						// freereg r1
						// allocreg r1

						//fptrtest.c, line 30
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l13,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done
						// allocreg r3

						//fptrtest.c, line 30
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r3 - no need to prep
						// (obj to tmp) flags 22 type a
						// matchobj comparing flags 34 with 130
						// deref 
						// (prepobj tmp)
 						// matchobj comparing flags 162 with 130
						// deref
						// var FIXME - deref?
						// static
	ldinc	r7
	.ref	l2,0
	ldt
						//sizemod based on type 0xa
	ldt
						// (save temp)isreg
	mr	r3
						//save_temp done

						//fptrtest.c, line 30
						// Q1 disposable
						//call
						// (obj to tmp) flags 6a type f
						// matchobj comparing flags 106 with 34
						// deref 
	//mt
	exg	r7
						// Deferred popping of 0 bytes (0 in total)
						// freereg r3
						// freereg r1

						//fptrtest.c, line 31
						//setreturn
						// (obj to r0) flags 1 type 3
						// const
	.liconst	0
	mr	r0
	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: yes
		//r2: yes
		//r3: yes
		//r4: no
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.5
	.global	_main
_main:
	stdec	r6
	mt	r3
	stdec	r6
						// allocreg r2

						//fptrtest.c, line 37
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// static
	.liabs	l1,0
						// static pe not varadr
	mr	r0
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// extern (offset 0)
	.liabs	_f1
						// extern pe is varadr
						// (save temp)store type a
	st	r0
						//save_temp done

						//fptrtest.c, line 38
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// static
	.liabs	l2,0
						// static pe not varadr
	mr	r0
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// static
	.liabs	l1,0
						// static pe is varadr
						// (save temp)store type a
	st	r0
						//save_temp done
						// allocreg r3

						//fptrtest.c, line 39
						//FIXME convert
						// (convert - reducing type a to 3
						// (prepobj r0)
 						// reg r3 - no need to prep
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// extern (offset 0)
	.liabs	_f1
						// extern pe is varadr
						//Saving to reg r3
						// (save temp)isreg
	mr	r3
						//save_temp done
						//No need to mask - same size

						//fptrtest.c, line 40
		// Offsets 0, 12
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// static
	.liabs	l4,12
						// static pe not varadr
	mr	r0
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// extern (offset 0)
	.liabs	_f1
						// extern pe is varadr
						// (save temp)store type a
	st	r0
						//save_temp done
						// allocreg r1

						//fptrtest.c, line 41
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 130
						// matchobj comparing flags 130 with 130
						// static
	.liabs	l16,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//fptrtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	l4,12 // Static with vmax 
	addt	r7	//Deref function pointer
	ldt
	exg	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r1

						//fptrtest.c, line 42
						//FIXME convert
						// (convert - reducing type 3 to a
						//No need to mask - same size
						// allocreg r1

						//fptrtest.c, line 42
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l17,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//fptrtest.c, line 42
						// Q1 disposable
						//call
						// (obj to tmp) flags 6a type f
						// matchobj comparing flags 106 with 130
						// deref 
	mt	r3
	exg	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r3
						// freereg r1
						// allocreg r1

						//fptrtest.c, line 29
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l12,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done

						//fptrtest.c, line 29
						//call
						//pcreltotemp
	.lipcrel	l1 // Static 
	addt	r7	//Deref function pointer
	ldt
	exg	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r1
						// allocreg r1

						//fptrtest.c, line 30
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r1 - no need to prep
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l13,0
						// static pe is varadr
						// (save temp)isreg
	mr	r1
						//save_temp done
						// allocreg r3

						//fptrtest.c, line 30
		// Offsets 0, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r3 - no need to prep
						// (obj to tmp) flags 22 type a
						// matchobj comparing flags 34 with 130
						// deref 
						// (prepobj tmp)
 						// matchobj comparing flags 162 with 130
						// deref
						// var FIXME - deref?
						// static
	ldinc	r7
	.ref	l2,0
	ldt
						//sizemod based on type 0xa
	ldt
						// (save temp)isreg
	mr	r3
						//save_temp done

						//fptrtest.c, line 30
						// Q1 disposable
						//call
						// (obj to tmp) flags 6a type f
						// matchobj comparing flags 106 with 34
						// deref 
	//mt
	exg	r7
						// Flow control - popping 0 + 0 bytes
						// freereg r3
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
	.liabs	l3,0
						//static deref
						//sizemod based on type 0x3
	ldt
						// (save temp)isreg
	mr	r2
						//save_temp done

						//fptrtest.c, line 44
						// (compare) (q1 signed) (q2 signed)
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 2
						// const
						// matchobj comparing flags 1 with 2
	.liconst	4
	cmp	r2

						//fptrtest.c, line 44
	cond	NEQ
						//conditional branch regular
						//pcreltotemp
	.lipcrel	l19
		add	r7
						// allocreg r1

						//fptrtest.c, line 45
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l20,0
						// static pe is varadr
	stdec	r6

						//fptrtest.c, line 45
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//fptrtest.c, line 47
						//pcreltotemp
	.lipcrel	l21
	add	r7
l19: # 

						//fptrtest.c, line 47
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// reg r2 - only match against tmp
	mt	r2
	stdec	r6

						//fptrtest.c, line 47
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l22,0
						// static pe is varadr
	stdec	r6

						//fptrtest.c, line 47
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 8 + 0 bytes
	.liconst	8
	add	r6
l21: # 

						//fptrtest.c, line 48
						//setreturn
						// (obj to r0) flags 1 type 3
						// const
	.liconst	0
	mr	r0
						// freereg r1
						// freereg r2
	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	.section	.data.6
	.align	4
l3:
	.int	0
	.section	.bss.7
	.align	4
	.lcomm	l1,4
	.section	.bss.8
	.align	4
	.lcomm	l2,4
	.section	.bss.9
	.align	4
	.lcomm	l4,16
	.section	.rodata.a
	.align	4
l9:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.b
	.align	4
l12:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.c
	.align	4
l13:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.d
	.align	4
l16:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.e
	.align	4
l17:
	.byte	72
	.byte	101
	.byte	108
	.byte	108
	.byte	111
	.byte	32
	.byte	119
	.byte	111
	.byte	114
	.byte	108
	.byte	100
	.byte	33
	.byte	10
	.byte	0
	.section	.rodata.f
	.align	4
l20:
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
	.section	.rodata.10
	.align	4
l22:
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
	.byte	114
	.byte	101
	.byte	115
	.byte	117
	.byte	108
	.byte	116
	.byte	58
	.byte	32
	.byte	37
	.byte	100
	.byte	10
	.byte	0
