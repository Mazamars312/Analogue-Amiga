//	dualthread.S
//	Copyright 2021 by Alastair M. Robinson

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


//	Thread synchronisation primitives for lib832.


	.section .text.threads

_thread_sleepflag:
	.int	0

// Thread_asleep can be used to tell whether or not the other thread is sleeping.
// Due to concurrency issues the answer is not definitive if negative, since the 
// thread could have completed and slept since the test was made - but it should
// be definitive when positive.

	.global _thread_asleep:
_thread_asleep:
	mr	r0
	.lipcrel _thread_sleepflag
	addt	r7
	ldt
	exg	r0
	mr	r7

// Put the thread to sleep, marking a flag in the process.
// Note, this code has no concept of which thread is sleeping, so you should
// always designate one thread as subservient to the other.

	.global _thread_sleep
_thread_sleep:
	mr	r0
	li	1
	mr	r1
	.lipcrel _thread_sleepflag,4
	addt	r7
	stmpdec	r1
	sig		// Wake up the other thread if it's sleeping, to insure against deadlocks.
	cond NEX
	mt	r0	// Nop, first instruction after sleep is lost
	mt	r0
	mr	r7

// Unpausing the other thread is as simple as issuing a sig instruction.
// If the other thread is asleep we clear the sleep flag.  If it's not
// asleep we leave it alone to avoid the (admittedly extremely unlikely)
// race condition where this thread clears the flag and issues a sig
// in between the other thread setting the flag and actually sleeping.

	.global	_thread_wake
_thread_wake:
	stdec	r6
	.lipcrel	_thread_asleep
	add	r7
	mr	r0
	and	r0
	cond	NEQ	// Is the other thread asleep?
		li	0	// If so, clear the sleep flag before waking it
		mr	r0
		.lipcrel _thread_sleepflag,4
		addt	r7
		stmpdec,r0
		cond EX
	sig
	ldinc	r6
	mr	r7

