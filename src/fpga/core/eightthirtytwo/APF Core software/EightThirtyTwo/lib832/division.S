//	divison.S
//	Copyright 2020 by Alastair M. Robinson

//	This file is part of lib832.

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


//	Integer division routines

//	_div_u32byu32 - numerator in r1, denominator in r2,
//	returns quotient in r0, remainder in r1.
//
//	_div_s32bys32 just evaluates signs, negates and calls unsigned division.


	.section .text._div_32by32
	.global	_div_u32byu32


_div_u32byu32:
	stdec	r6
	mt	r3	// Save r3
	stdec	r6

	mt	r2
	and	r2
	cond	EQ	// Division by zero?
		.lipcrel	.end
		add	r7

	li	0
	mr	r0
	li	1
	mr	r3

	mt	r2
	cmp	r1
	cond	LE
		.lipcrel .next
		add	r7

.start:
	mt	r2
	add	r2
	cond	LE	// Have we shifted off the leftmost bit?
		.lipcrel .realign
		add	r7
	mt	r3
	add	r3

	mt	r2
	cmp	r1
	cond	SGT
		.lipcrel .start
		add	r7
	.lipcrel .next
	add	r7

.realign:
	li	1
	or	r2
	ror	r2
	
.next:
	mt	r2
	cmp	r1
	cond	GE
		sub	r1
		mt	r3
		add	r0
		cond	EX

	li	1
	shr	r2
	shr	r3
	cond	NEQ
		.lipcrel .next
		add	r7

.end:
	ldinc	r6
	mr	r3
	ldinc	r6
	mr	r7



	.global	_div_s32bys32

_div_s32bys32:
	stdec	r6
	mt	r5	// save r5.
	stdec	r6

	li	4	// Bit 2 set to avoid zero flag messing up test for carry later.
	mr	r5	// Keep track of signs here

	mt	r1	// Check sign of numerator.
	sgn
	addt	r1
	cond	SLT	// Did we overflow?
		li	-1
		xor	r1	// Negate if negative
		li	1
		add	r1
		li	3
		xor	r5	// and track the eventual sign bit
	cond EX

	mt	r2	// Check sign of denominator.
	sgn
	addt	r2
	cond	SLT
		li	-1
		xor	r2	// Negate if negative...
		li	1
		add	r2
		li	1
		xor	r5	// If both numerator and denominator are negative then we don't need to negate the result.
	cond EX

	// Now perform an unsigned division.
	.lipcrel _div_u32byu32
	add	r7

	li	1
	shr	r5
	cond	SLT	// Negate the result if necessary.
		li	-1
		xor	r0
		li	1
		add	r0
	cond EX
	shr	r5
	cond	SLT	// Negate the remainder if necessary.
		li	-1
		xor	r1
		li	1
		add	r1
	cond EX

	// FIXME - what about the remainder?

	ldinc	r6
	mr	r5
	ldinc	r6
	mr	r7

