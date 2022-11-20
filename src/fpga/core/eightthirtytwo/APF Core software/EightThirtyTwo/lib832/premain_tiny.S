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


//  Minimal premain implementation - no stack guard or ctors/dtors.
//
//  Does clear BSS data, however.
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
	add	r6

	// Clear BSS data
	// Will overshoot, but since the stack follows BSS, it won't matter.
	.liabs	__bss_end__
	mr	r1
	.liabs	__bss_start__
	mr	r0
	sub	r1
.clrwordloop
	li	0
	stinc	r0
	stinc	r0
	stinc	r0
	stinc	r0
	li	16
	sub	r1
	cond	GE
		.lipcrel	.clrwordloop
		add	r7

	.lipcrel	_main
	add	r7

.endloop:
	cond NEX	// Pause until an interrupt is received
	and r0	// Nop, first instruction after cond NEX is lost.
	.lipcrel .endloop
	add	r7

