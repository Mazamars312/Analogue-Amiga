//	absolutestack.S
//	Copyright 2020 by Alastair M. Robinson

//	Absolute stack pointer.  Can be included just by referencing the seed function _setstack

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

	.section .text.setstack
	.global	_setstack

	.constant _STACKTOP,8192
	.global _STACKTOP
	.weak	_STACKTOP

_setstack:
	mr	r7

__setstack:
	mr	r0
	.liabs	_STACKTOP
	exg	r6
	stdec	r6
	mt	r0
	mr	r7

__restorestack:
	mr	r0
	ldinc	r6
	mr	r6
	mt	r0
	mr	r7

	.ctor	.ctor.000
	.ref	__setstack

	.dtor	.dtor.000
	.ref	__restorestack

