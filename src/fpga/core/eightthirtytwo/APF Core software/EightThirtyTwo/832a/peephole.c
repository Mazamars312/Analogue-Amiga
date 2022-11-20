/*
	Peephole optimisations for 832 assembler

	Detect and remove unnecessary opcode combinations
	such as "mr r3, mt r3".
	Potentially st r6, ld r6 too, but must be possible to disable this
	in case it causes issues with volatile registers


	Copyright (c) 2021 by Alastair M. Robinson

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
#include "peephole.h"
#include "832util.h"

int peephole_test(struct peepholecontext *pc,int opcode)
{
	int result=1;
	int operand=opcode&7;
	opcode&=~7;
	if(pc)
	{
		debug(1,"Peephole comparing %x, %x with %x, %x\n",opcode,operand,pc->opcode,pc->operand);
		if(pc->operand==operand)
		{
			debug(1,"Operands match\n");
			/* MT followed by MR to the same register */
			if(pc->opcode==opc_mt && opcode==opc_mr)
				result=0;
			/* MR followed by MT to the same register */
			if(pc->opcode==opc_mr && opcode==opc_mt)
				result=0;
		}
		pc->opcode=opcode;
		pc->operand=operand;
	}
	return(result);
}


void peephole_clear(struct peepholecontext *pc)
{
	if(pc)
	{
		pc->opcode=0;
		pc->operand=0;
	}
}

