//	premain.S
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


//  More comprehensive premain implementation.
//
//  Supports auto initialisation from .ctors section and clearing of BSS data.
//
//  STACKSIZE is defined to 4K, but is a weak symbol so can be overridden either in another section
//  or on the linker command line.

	.constant _STACKSIZE,4096
	.global _STACKSIZE
	.weak	_STACKSIZE

	.section	.text.premain
	.global	_premain
_premain:
	// Setup the stack
	.liabs	__bss_end__
	mr	r6
	// Fill the stack with a marker pattern
	.liabs	_STACKSIZE
	mr	r0
	li	2
	shr	r0
	.liconst 0x55aa55aa
	mr	r1
.stackloop:
	mt	r1
	stinc	r6
	li	1
	sub	r0
	cond SGT
		.lipcrel .stackloop
		add	r7

	// Clear BSS data
	.liabs	__bss_end__
	mr	r1
	.liabs	__bss_start__
	mr	r0
	sub	r1
.clrwordloop
	li	16
	sub	r1
	cond	GE
		li	0
		stinc	r0
		stinc	r0
		stinc	r0
		stinc	r0
		.lipcrel	.clrwordloop
		add	r7

	li	16
	add	r1
	cond	NEQ
.clrbyteloop
		li	0
		stbinc	r0
		li	1
		sub	r1
		cond	NEQ	// Can nest conditions as long as they don't differ.
			.lipcrel .clrbyteloop
			add r7


	// Call constructors
	.liabs	__ctors_end__
	mr	r5
	.liabs	__ctors_start__
	mr	r4
.ctorloop:
	mt	r4
	cmp	r5
	cond	EQ
		.lipcrel .ctordone
		add	r7

	ldinc	r4
	exg	r7

	.lipcrel .ctorloop
	add	r7

.ctordone:
	// FIXME - should push argc and argv on the stack before calling main.
	.lipcrel	_main
	add	r7

	// Call destructors
	.liabs	__dtors_start__
	mr	r5
	.liabs	__dtors_end__
	mr	r4
.dtorloop:
	mt	r4
	cmp	r5
	cond	EQ
		.lipcrel .endloop
		add	r7

	li	4
	sub	r4
	ld	r4
	exg	r7

	.lipcrel .dtorloop
	add	r7

.endloop:
	cond NEX	// Pause until an interrupt is received
	and r0	// Nop, first instruction after cond NEX is lost.
	.lipcrel .endloop
	add	r7

