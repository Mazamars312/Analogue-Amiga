	//registers used:
		//r1: yes
		//r2: no
		//r3: yes
		//r4: yes
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.5
	.global	_sdiv
_sdiv:
	stdec	r6
	mt	r3
	stdec	r6
	mt	r4
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
						// (obj to tmp) flags 40 type 3
						// reg r1 - only match against tmp
	mt	r1
						// (save temp)isreg
	mr	r3
						//save_temp done
						// freereg r1
						// allocreg r4

						//divtest.c, line 13
						//Call division routine
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 64
						// reg r3 - only match against tmp
	//mt
	mr	r1
						// (obj to tmp) flags 2 type 3
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	12
	ldidx	r6
	mr	r2
	.lipcrel	_div_s32bys32
	add	r7
	mt	r0
	mr	r4
						// (save result) // isreg

						//divtest.c, line 13
						// (compare) (q1 signed) (q2 signed)
						// (obj to tmp) flags 2 type 3
						// var, auto|reg
	.liconst	16
	ldidx	r6
	cmp	r4

						//divtest.c, line 13
	cond	NEQ
						//conditional branch reversed
						//pcreltotemp
	.lipcrel	l4
		add	r7

						//divtest.c, line 14
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 2
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 2
						// static
	.liabs	l5,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 14
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//divtest.c, line 16
						//pcreltotemp
	.lipcrel	l6
	add	r7
l4: # 

						//divtest.c, line 16
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l7,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 16
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6
l6: # 

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 2 type 3
						// var, auto|reg
	.liconst	16
	ldidx	r6
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 2
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 2 type 3
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	20
	ldidx	r6
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 2
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l8,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 17
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (20 in total)
						// freereg r3
						// freereg r4
	.liconst	-20
	sub	r6
	ldinc	r6
	mr	r4

	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: yes
		//r2: no
		//r3: yes
		//r4: yes
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.6
	.global	_smod
_smod:
	stdec	r6
	mt	r3
	stdec	r6
	mt	r4
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
						// (obj to tmp) flags 40 type 3
						// reg r1 - only match against tmp
	mt	r1
						// (save temp)isreg
	mr	r3
						//save_temp done
						// freereg r1
						// allocreg r4

						//divtest.c, line 22
						//Call division routine
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 64
						// reg r3 - only match against tmp
	//mt
	mr	r1
						// (obj to tmp) flags 2 type 3
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	12
	ldidx	r6
	mr	r2
	.lipcrel	_div_s32bys32
	add	r7
	mt	r1
	mr	r4
						// (save result) // isreg

						//divtest.c, line 22
						// (compare) (q1 signed) (q2 signed)
						// (obj to tmp) flags 2 type 3
						// var, auto|reg
	.liconst	16
	ldidx	r6
	cmp	r4

						//divtest.c, line 22
	cond	NEQ
						//conditional branch reversed
						//pcreltotemp
	.lipcrel	l12
		add	r7

						//divtest.c, line 23
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 2
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 2
						// static
	.liabs	l13,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 23
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//divtest.c, line 25
						//pcreltotemp
	.lipcrel	l14
	add	r7
l12: # 

						//divtest.c, line 25
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l15,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 25
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6
l14: # 

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 2 type 3
						// var, auto|reg
	.liconst	16
	ldidx	r6
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 2
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 2 type 3
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	20
	ldidx	r6
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 42 type 3
						// matchobj comparing flags 66 with 2
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l16,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 26
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (20 in total)
						// freereg r3
						// freereg r4
	.liconst	-20
	sub	r6
	ldinc	r6
	mr	r4

	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: yes
		//r2: no
		//r3: yes
		//r4: yes
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.7
	.global	_udiv
_udiv:
	stdec	r6
	mt	r3
	stdec	r6
	mt	r4
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
						// (obj to tmp) flags 40 type 103
						// reg r1 - only match against tmp
	mt	r1
						// (save temp)isreg
	mr	r3
						//save_temp done
						// freereg r1
						// allocreg r4

						//divtest.c, line 31
						//Call division routine
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 64
						// reg r3 - only match against tmp
	//mt
	mr	r1
						// (obj to tmp) flags 2 type 103
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	12
	ldidx	r6
	mr	r2
	.lipcrel	_div_u32byu32
	add	r7
	mt	r0
	mr	r4
						// (save result) // isreg

						//divtest.c, line 31
						// (compare) (q1 unsigned) (q2 unsigned)
						// (obj to tmp) flags 2 type 103
						// var, auto|reg
	.liconst	16
	ldidx	r6
	cmp	r4

						//divtest.c, line 31
	cond	NEQ
						//conditional branch reversed
						//pcreltotemp
	.lipcrel	l20
		add	r7

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 2
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 2
						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//divtest.c, line 34
						//pcreltotemp
	.lipcrel	l22
	add	r7
l20: # 

						//divtest.c, line 34
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l23,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 34
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6
l22: # 

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 2 type 103
						// var, auto|reg
	.liconst	16
	ldidx	r6
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 2
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 2 type 103
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	20
	ldidx	r6
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 2
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (20 in total)
						// freereg r3
						// freereg r4
	.liconst	-20
	sub	r6
	ldinc	r6
	mr	r4

	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: yes
		//r2: no
		//r3: yes
		//r4: yes
		//r5: no
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.8
	.global	_umod
_umod:
	stdec	r6
	mt	r3
	stdec	r6
	mt	r4
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
						// (obj to tmp) flags 40 type 103
						// reg r1 - only match against tmp
	mt	r1
						// (save temp)isreg
	mr	r3
						//save_temp done
						// freereg r1
						// allocreg r4

						//divtest.c, line 40
						//Call division routine
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 64
						// reg r3 - only match against tmp
	//mt
	mr	r1
						// (obj to tmp) flags 2 type 103
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	12
	ldidx	r6
	mr	r2
	.lipcrel	_div_u32byu32
	add	r7
	mt	r1
	mr	r4
						// (save result) // isreg

						//divtest.c, line 40
						// (compare) (q1 unsigned) (q2 unsigned)
						// (obj to tmp) flags 2 type 103
						// var, auto|reg
	.liconst	16
	ldidx	r6
	cmp	r4

						//divtest.c, line 40
	cond	NEQ
						//conditional branch reversed
						//pcreltotemp
	.lipcrel	l28
		add	r7

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 2
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 2
						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6

						//divtest.c, line 43
						//pcreltotemp
	.lipcrel	l30
	add	r7
l28: # 

						//divtest.c, line 43
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l31,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 43
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Flow control - popping 4 + 0 bytes
	.liconst	4
	add	r6
l30: # 

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 2 type 103
						// var, auto|reg
	.liconst	16
	ldidx	r6
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 2
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 2 type 103
						// matchobj comparing flags 2 with 66
						// var, auto|reg
						// matchobj comparing flags 1 with 66
	.liconst	20
	ldidx	r6
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 42 type 103
						// matchobj comparing flags 66 with 2
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 66
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 66
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (20 in total)
						// freereg r3
						// freereg r4
	.liconst	-20
	sub	r6
	ldinc	r6
	mr	r4

	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	//registers used:
		//r1: no
		//r2: no
		//r3: yes
		//r4: yes
		//r5: yes
		//r6: no
		//r7: no
		//tmp: no
	.section	.text.9
	.global	_main
_main:
	exg	r6
	stmpdec	r6
	stmpdec	r3
	stmpdec	r4
	stmpdec	r5
	exg	r6
						// allocreg r5
		// Offsets -843409290, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r5 - no need to prep
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	-843409290
						// (save temp)isreg
	mr	r5
						//save_temp done
						// allocreg r4
		// Offsets 305419896, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r4 - no need to prep
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	305419896
						// (save temp)isreg
	mr	r4
						//save_temp done
						// allocreg r3
		// Offsets -2023406815, 0
		// Have am? no, no
						// (a/p assign)
						// Have an addressing mode...
						// (prepobj r0)
 						// reg r3 - no need to prep
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-2023406815
						// (save temp)isreg
	mr	r3
						//save_temp done

						//divtest.c, line 14
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l5,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 14
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (4 in total)

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	-65540
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4660
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 3
						// matchobj comparing flags 576 with 1
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l8,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 17
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (24 in total)

						//divtest.c, line 23
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l13,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 23
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (28 in total)

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	3496
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4660
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 3
						// matchobj comparing flags 576 with 1
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l16,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 26
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (48 in total)

						//divtest.c, line 14
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l5,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 14
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (52 in total)

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	-130
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	33
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4321
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l8,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 17
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (72 in total)

						//divtest.c, line 23
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l13,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 23
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (76 in total)

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	-31
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	33
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4321
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l16,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 26
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (96 in total)

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (100 in total)

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	130150393
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	33
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4321
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (120 in total)

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (124 in total)

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	6
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	33
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	-4321
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (144 in total)

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (148 in total)

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	46289
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	74565
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r5 - only match against tmp
	mt	r5
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (168 in total)

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (172 in total)

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	18721
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	74565
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r5 - only match against tmp
	mt	r5
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (192 in total)

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (196 in total)

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 576
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 576
						// const
						// matchobj comparing flags 1 with 576
	.liconst	1
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (216 in total)

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (220 in total)

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	0
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	1
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (240 in total)

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (244 in total)

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	65540
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	4660
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (264 in total)

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (268 in total)

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	3496
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	4660
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r4 - only match against tmp
	mt	r4
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (288 in total)

						//divtest.c, line 32
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l21,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 32
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (292 in total)

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 576
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 576
						// const
						// matchobj comparing flags 1 with 576
	.liconst	1
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 35
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l24,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 35
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (312 in total)

						//divtest.c, line 41
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l29,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 41
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (316 in total)

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// const
	.liconst	0
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 103
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	1
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 240 type 103
						// matchobj comparing flags 576 with 1
						// reg r3 - only match against tmp
	mt	r3
	stdec	r6

						//divtest.c, line 44
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 576
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 576
						// static
	.liabs	l32,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 44
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (336 in total)

						//divtest.c, line 14
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l5,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 14
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (340 in total)

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	0
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	4660
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	291
	stdec	r6

						//divtest.c, line 17
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l8,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 17
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (360 in total)

						//divtest.c, line 23
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// (prepobj tmp)
 						// static
	.liabs	l13,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 23
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 4 bytes (364 in total)

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 0, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// const
	.liconst	291
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 4, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1

			// required value found in tmp
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 8, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	4660
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 12, regnames[sp] r6
						// (obj to tmp) flags 1 type 3
						// matchobj comparing flags 1 with 1
						// const
						// matchobj comparing flags 1 with 1
	.liconst	291
	stdec	r6

						//divtest.c, line 26
						// (a/p push)
						// a: pushed 16, regnames[sp] r6
						// (obj to tmp) flags 82 type a
						// matchobj comparing flags 130 with 1
						// (prepobj tmp)
 						// matchobj comparing flags 130 with 1
						// static
	.liabs	l16,0
						// static pe is varadr
	stdec	r6

						//divtest.c, line 26
						//call
						//pcreltotemp
	.lipcrel	_printf // extern
	add	r7
						// Deferred popping of 20 bytes (384 in total)

						//divtest.c, line 66
						//setreturn
						// (obj to r0) flags 1 type 3
						// const
	.liconst	0
	mr	r0
						// freereg r3
						// freereg r4
						// freereg r5
						// matchobj comparing flags 1 with 1
						// matchobj comparing flags 1 with 1
	.liconst	-384
	sub	r6
	ldinc	r6
	mr	r5

	ldinc	r6
	mr	r4

	ldinc	r6
	mr	r3

	ldinc	r6
	mr	r7

	.section	.rodata.a
	.align	4
l5:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.b
	.align	4
l7:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.c
	.align	4
l8:
	.byte	37
	.byte	100
	.byte	32
	.byte	47
	.byte	32
	.byte	37
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	100
	.byte	44
	.byte	32
	.byte	101
	.byte	120
	.byte	112
	.byte	101
	.byte	99
	.byte	116
	.byte	101
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	100
	.byte	10
	.byte	0
	.section	.rodata.d
	.align	4
l13:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.e
	.align	4
l15:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.f
	.align	4
l16:
	.byte	37
	.byte	100
	.byte	32
	.byte	37
	.byte	37
	.byte	32
	.byte	37
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	100
	.byte	44
	.byte	32
	.byte	101
	.byte	120
	.byte	112
	.byte	101
	.byte	99
	.byte	116
	.byte	101
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	100
	.byte	10
	.byte	0
	.section	.rodata.10
	.align	4
l21:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.11
	.align	4
l23:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.12
	.align	4
l24:
	.byte	37
	.byte	117
	.byte	32
	.byte	47
	.byte	32
	.byte	37
	.byte	117
	.byte	58
	.byte	32
	.byte	37
	.byte	117
	.byte	44
	.byte	32
	.byte	101
	.byte	120
	.byte	112
	.byte	101
	.byte	99
	.byte	116
	.byte	101
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	117
	.byte	10
	.byte	0
	.section	.rodata.13
	.align	4
l29:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.14
	.align	4
l31:
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
	.byte	58
	.byte	27
	.byte	91
	.byte	48
	.byte	109
	.byte	32
	.byte	0
	.section	.rodata.15
	.align	4
l32:
	.byte	37
	.byte	117
	.byte	32
	.byte	37
	.byte	37
	.byte	32
	.byte	37
	.byte	117
	.byte	58
	.byte	32
	.byte	37
	.byte	117
	.byte	44
	.byte	32
	.byte	101
	.byte	120
	.byte	112
	.byte	101
	.byte	99
	.byte	116
	.byte	101
	.byte	100
	.byte	58
	.byte	32
	.byte	37
	.byte	117
	.byte	10
	.byte	0
