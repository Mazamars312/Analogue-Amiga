/*
	832defs.c

	Copyright (c) 2019,2020 by Alastair M. Robinson

	This file is part of the EightThirtyTwo CPU project.

	EightThirtyTwo is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	EightThirtyTwo is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with EightThirtyTwo.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>

#include "832a.h"
#include "832opcodes.h"

struct opcode operands[]=
{
	/* Operand definitions - registers first */
	{"r0",0,0},
	{"r1",1,0},
	{"r2",2,0},
	{"r3",3,0},
	{"r4",4,0},
	{"r5",5,0},
	{"r6",6,0},
	{"r7",7,0},

	{"NEX",0,0},	/* Match none. */
	{"SGT",1,0},	/* Zero clear, carry clear */
	{"EQ",2,0},	/* Zero set, carry don't care */
	{"GE",3,0},	/* Zero set or carry clear */
	{"SLT",4,0},	/* Zero clear, carry set */
	{"NEQ",5,0},	/* Zero clear, carry don't care */
	{"LE",6,0},	/* Zero set or carry set */
	{"EX",7,0},	/* Zero don't care, carry don't care */
	{0,0xff,0}	/* Null terminate */
};

struct opcode opcodes[]=
{
	/* Regular opcodes, each taking a 3-bit operand specifying the register number */
	{"cond",opc_cond,3},
	{"exg",opc_exg,3},
	{"ldbinc",opc_ldbinc,3},
	{"stdec",opc_stdec,3},

	{"ldinc",opc_ldinc,3},
	{"shr",opc_shr,3},
	{"shl",opc_shl,3},
	{"ror",opc_ror,3},

	{"stinc",opc_stinc,3},
	{"mr",opc_mr,3},
	{"stbinc",opc_stbinc,3},
	{"stmpdec",opc_stmpdec,3},

	{"ldidx",opc_ldidx,3},
	{"ld",opc_ld,3},
	{"mt",opc_mt,3},
	{"st",opc_st,3},

	{"add",opc_add,3},
	{"sub",opc_sub,3},
	{"mul",opc_mul,3},
	{"and",opc_and,3},

	{"addt",opc_addt,3},
	{"cmp",opc_cmp,3},
	{"or",opc_or,3},
	{"xor",opc_xor,3},

	/* Load immediate takes a six-bit operand */

	{"li",opc_li,6},

	/* Overloaded opcodes. Operands that make no sense when applied to r7, re-used.
	   No operand for obvious reasons. */

	{"sgn",ovl_sgn,0}, /* Overloads or */
	{"ldt",ovl_ldt,0}, /* Overloads xor */
	{"byt",ovl_byt,0}, /* Overloads mul */
	{"hlf",ovl_hlf,0}, /* Overloads and */
	{"sig",ovl_sig,0}, /* Overloads and */
	{0,0xff,0}	/* Null terminate */
};

