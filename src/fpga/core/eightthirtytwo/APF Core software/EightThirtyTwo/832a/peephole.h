#ifndef PEEPHOLE_H
#define PEEPHOLE_H

#include "832opcodes.h"

struct peepholecontext
{
	int opcode;
	int operand;
};

/* Return 0 if the opcode under consideration should be skipped, 1 otherwise */
int peephole_test(struct peepholecontext *pc,int opcode);

void peephole_clear(struct peepholecontext *pc);

#endif

