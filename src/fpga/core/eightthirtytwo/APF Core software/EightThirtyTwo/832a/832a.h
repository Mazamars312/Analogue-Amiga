#ifndef EIGHTTHIRTYTWOASSEMBLER_H
#define EIGHTTHIRTYTWOASSEMBLER_H

struct opcode
{
	const char *mnem;
	int opcode;
	int opbits;
};

extern struct opcode operands[17];
extern struct opcode opcodes[31];

#endif

