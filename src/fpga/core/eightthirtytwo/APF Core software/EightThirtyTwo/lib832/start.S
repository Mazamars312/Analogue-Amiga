//	start.S
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


// Startup code for lib832.  

	.section .text.startup

_start:
	cond	SGT	// Z flag and C flag both clear -> Thread 1
		.lipcrel .start1
		add	r7

	cond	SLT	// Z flag clear, C flag set -> Thread 2
		.lipcrel .start2
		add	r7

	// By elimination the Z flag must be set, so this must be an interrupt.
	// (The interrupt is triggered using the xor alu op, so C is not cleared.)

	exg	r6
	stmpdec	r0	// Store r0 first since we need a register for decrementing the return address
	stmpdec	r6	// Now save the return address
	stmpdec	r1	// Save r1
	stmpdec	r2	// R2 is now a scratch register, so save that too.
	exg	r6

	ldinc	r7

	.global _InterruptVector	// This word can be written to install an int handler.  Location 13.
_InterruptVector:
	.ref	_interrupt

	exg	r7

	ldinc	r6
	mr	r2
	ldinc	r6
	mr	r1
	ldinc	r6
	mr	r0		// Return address temporarily into r0
	li	-1
	add	r0		// Decrement return address
	ldinc r6
	exg	r0		// Restore r0
	mr	r7		// Jump to return address - 1.


.start1:
	// Set up the stack here
	ldinc	r7
	.ref	_premain
	mr	r7
.start2:
	// Set up the stack here
	ldinc	r7
	.ref	_premain2
	mr	r7

	.section	.text.interrupt
	.weak	_interrupt
_interrupt:
	mr	r7	// return immediately

	.section	.text.premain2
	.weak	_premain2
_premain2:
.endloop2:
	cond NEX	// Pause until an interrupt is received
	.lipcrel	.endloop2
	add	r7

