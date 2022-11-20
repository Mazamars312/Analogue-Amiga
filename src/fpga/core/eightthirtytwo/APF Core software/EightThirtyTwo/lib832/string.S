//	string.S - String functions implemented in assembly language for EightThirtyTwo
//	Copyright 2020 by Alastair M. Robinson

//	This file is part of Lib832.

//	Lib832 is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Lesser General Public License as
//	published by the Free Software Foundation, either version 3 of
//	the License, or (at your option) any later version.

//	Lib832 is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Lesser General Public License for more details.

//	You should have received a copy of the GNU Lesser General Public License
//	along with lib832.  If not, see <https://www.gnu.org/licenses/>.


	// char *strncat(__reg("r2") char *, __reg("r1") const char *, __reg("r3") size_t);

	.section	.text.strncat
	.global	___strncat
___strncat:
	stdec	r6
.finddst:
	ldbinc	r2
	cond	NEQ
		.lipcrel .finddst
		add	r7
	li	1
	sub	r2
.cpy:
	ldbinc	r1
	stbinc	r2
	cond	EQ	// Done?
		ldinc	r6
		mr	r7
	li	1
	sub	r3
	cond	NEQ
		.lipcrel .cpy
		add	r7
	li	0
	stbinc	r2
	ldinc	r6
	mr	r7



	// char *strcat(__reg("r2") char *, __reg("r1") const char *);

	.section	.text.strcat
	.global	___strcat
___strcat:
	stdec	r6
.finddst2:
	ldbinc	r2
	cond	NEQ
		.lipcrel .finddst2
		add	r7
	li	1
	sub	r2
.cpy2:
	ldbinc	r1
	stbinc	r2
	cond	NEQ
		.lipcrel .cpy2
		add	r7
	ldinc	r6
	mr	r7



	// int strcmp(__reg("r2") const char *, __reg("r1") const char *);

	.section	.text.strcmp
	.global	___strcmp

___strcmp:						// Parameters: char *s1, char *s2
	stdec	r6
	mt	r2
	stdec	r6
.loopstrcmp:
	ldbinc	r2	// s2++
	mr	r0
	ldbinc	r1
	cond	EQ					// If null, we're done.  If S2 has also terminated, r0 will be 0; else a positive number.
		.lipcrel .end
		add	r7
	sub	r0						// Otherwise subtract 
	cond	EQ
		.lipcrel .loopstrcmp
		add	r7
.end:
	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r7


	// int strcasecmp(__reg("r2") const char *, __reg("r1") const char *);

	.section	.text.strcasecmp
	.global	___strcasecmp

___strcasecmp:						// Parameters: char *s1, char *s2
	stdec	r6
	mt	r2
	stdec	r6
	mt	r3
	stdec	r6
.loopstrcasecmp:
	ldbinc	r2	// s2++
	mr	r0
	ldbinc	r1
	cond	EQ					// If null, we're done.  If S2 has also terminated, r0 will be 0; else a positive number.
		.lipcrel .endstrcasecmp
		add	r7
	mr	r3
	.liconst	0xdf; // Mask off bit 5 
	and	r0
	and	r3
	mt	r3
	sub	r0						// Otherwise subtract 
	cond	EQ
		.lipcrel .loopstrcasecmp
		add	r7
.endstrcasecmp:
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r7


// int strncmp(__reg("r2") const char *, __reg("r1") const char *, __reg("r3") size_t);

	.section	.text.strncmp
	.global	___strncmp
___strncmp:						// Parameters: char *s1, char *s2, int n
	stdec	r6
	mt	r3
	stdec	r6
	mt	r2
	stdec	r6
.loopstrncmp:
	li	1
	sub	r3
	cond SLT
		.lipcrel .endstrncmp
		add	r7
	ldbinc	r2	// s2++
	mr	r0
	ldbinc	r1
	cond	EQ					// If null, we're done.  If S2 has also terminated, r0 will be 0; else a positive number.
		.lipcrel .endstrncmp
		add	r7
	sub	r0						// Otherwise subtract 
	cond	EQ
		.lipcrel .loopstrncmp
		add	r7
.endstrncmp:
	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r7


// int strncasecmp(__reg("r2") const char *, __reg("r1") const char *, __reg("r3") size_t);

	.section	.text.strncasecmp
	.global	___strncasecmp
___strncasecmp:					// Parameters: char *s1, char *s2, int n
	stdec	r6
	mt	r4
	stdec	r6
	mt	r3
	stdec	r6
	mt	r2
	stdec	r6
.loopstrncasecmp:
	li	1
	sub	r3
	cond SLT
		.lipcrel .endstrncasecmp
		add	r7
	ldbinc	r2					// s2++
	mr	r0
	ldbinc	r1
	cond	EQ					// If null, we're done.  If S2 has also terminated, r0 will be 0; else a positive number.
		.lipcrel .endstrncasecmp
		add	r7
	mr	r4
	.liconst	0xdf					// Mask off bit 5
	and	r0
	and	r4
	mt	r4
	sub	r0						// Otherwise subtract 
	cond	EQ
		.lipcrel .loopstrncasecmp
		add	r7
.endstrncasecmp:
	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r4
	ldinc	r6
	mr	r7



// int memcmp(__reg("r3") const char *, __reg("r2") const char *, __reg("r1") size_t);

	.section	.text.memcmp
	.global	___memcmp
___memcmp:						// Parameters: char *s1, char *s2, int n
	stdec	r6
	mt	r3
	stdec	r6
	mt	r2
	stdec	r6
.loopmemcmp:
	li	1
	sub	r1
	cond SLT
		.lipcrel .endmemcmp
		add	r7
	ldbinc	r3	// s2++
	mr	r0
	ldbinc	r2
	sub	r0						// Perform comparison
	cond	EQ
		.lipcrel .loopmemcmp
		add	r7
.endmemcmp:
	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r7



	// char *strncpy(__reg("r3") char *dst, __reg("r2") const char *src, __reg("r1") size_t size);

	.section	.text.strncpy
	.globl	___strncpy
___strncpy:
	stdec	r6
	mt	r3
	mr	r0
	add	r1
.strnloop:
	mt	r3
	cmp	r1
	cond	EQ
		.lipcrel .done
		add	r7
	ldbinc r2
	stbinc	r3
	cond	NEQ
		.lipcrel .strnloop
		add	r7
.done:
	ldinc	r6
	mr	r7	// 16 bytes



	// char *strcpy(char *dst (in r1),const char *src (on stack));

	.section	.text.strcpy
	.globl	_strcpy
_strcpy:
	stdec	r6
	.liconst 4
	ldidx	r6
	mr	r2
	mt	r1
	mr	r0
.strcpyloop:
	ldbinc r2
	stbinc	r1
	cond	NEQ
		.lipcrel .strcpyloop
		add	r7
	ldinc	r6
	mr	r7	// 13 bytes



	// size_t strlen(__reg("r1" const char *);

	.section	.text.strlen
	.global	___strlen
___strlen:
	stdec	r6

	mt	r1
	mr	r0
	ldbinc	r1
.loopstrlen:
	ldbinc	r0
	cond	EQ
		mt	r1
		sub	r0
		ldinc	r6
		mr	r7
	.lipcrel .loopstrlen
	add	r7	// 12 bytes



	// int tolower(__reg("r1") int c);

	.section	.text.tolower
	.global	___tolower
___tolower:
	stdec	r6

	mt	r1
	mr	r0

	.liconst 65 // 'A'
	sub	r1
	cond	SLT
		ldinc	r6
		mr	r7
	.liconst	26
	cmp	r1
	cond	SLT
		.liconst 32
		or	r0
		cond EX
	ldinc	r6
	mr	r7



	// int toupper(__reg("r1") int c);

	.section	.text.toupper
	.global	___toupper
___toupper:
	stdec	r6

	mt	r1
	mr	r0

	.liconst 97 ; 'a'
	sub	r1
	cond	SLT
		ldinc	r6
		mr	r7
	.liconst	26
	cmp	r1
	cond	SLT
		.liconst 0xffffffdf ; ~32
		and	r0
		cond EX
	ldinc	r6
	mr	r7



	// void *memset(__reg("r3") void *dst, __reg("r2") int c,__reg("r1") size_t size);

	.section	.text.memset
	.globl	___memset
___memset:
	stdec	r6
	mt	r3
	mr	r0
	add	r1
.loopmemset:
	mt	r3
	cmp	r1
	cond	NEQ
		mt	r2
		stbinc	r3
		.lipcrel .loopmemset
		add	r7
	ldinc	r6
	mr	r7	// 13 bytes

	// char *memcpy(char *dst (in r1),const char *src (on stack),size_t size (on stack));

	.section	.text.memcpy
	.globl	_memcpy
_memcpy:
	stdec	r6
	mt	r3
	stdec	r6

	.liconst 8	// Fetch source pointer
	ldidx	r6
	mr	r2

	.liconst 12 // Fetch size
	ldidx	r6
	mr	r3

	mt	r1		// Dest pointer already in r1, move to r0 for result
	mr	r0

	add	r3
.memcpyloop:
	mt	r1
	cmp	r3
	cond	EQ
		.lipcrel .memcpydone
		add	r7
	ldbinc r2
	stbinc	r1
	.lipcrel .memcpyloop
	add	r7
.memcpydone:
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r7


	// char *memmove(__reg("r3") char *dst, __reg("r2") const char *src, __reg("r1") size_t size);

	.section	.text.memove
	.globl	___memmove
___memmove:
	stdec	r6
	mt	r3
	mr	r0
	add	r1
	cmp	r2
	cond	LE
		.lipcrel	.memcpyloop
		add	r7
	sub	r2
	mt	r1
	add	r2
.memmoveloop:
	mt	r3
	cmp	r1
	cond	EQ
		.lipcrel .memmovedone
		add	r7
	li	1
	sub	r2
	byt
	ld r2
	byt
	stdec r3
	.lipcrel .memmoveloop
	add	r7
.memmovedone:
	ldinc	r6
	mr	r7	// 15 bytes

