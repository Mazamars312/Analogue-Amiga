#ifndef SYMBOL_H
#define SYMBOL_H

#include "section.h"

#define SYMBOLFLAG_REFERENCE 1
#define SYMBOLFLAG_LDABS 2
#define SYMBOLFLAG_LDPCREL 4
#define SYMBOLFLAG_GLOBAL 8
#define SYMBOLFLAG_LOCAL 16
#define SYMBOLFLAG_WEAK 32
#define SYMBOLFLAG_CONSTANT 64
#define SYMBOLFLAG_ALIGN 128

#define SYMBOL_ISREF(x) (((x)->flags&SYMBOLFLAG_REFERENCE)|((x)->flags&SYMBOLFLAG_LDABS)\
							|((x)->flags&SYMBOLFLAG_LDPCREL)|((x)->flags&SYMBOLFLAG_ALIGN))

struct symbol
{
	struct symbol *next;
	char *identifier;
	int offset;
	int cursor;
	int flags;
	/* Used by linker */
	struct symbol *resolve;
	struct section *sect;
	int address;
	int size;
};

struct symbol *symbol_new(const char *id,int cursor,int flags);
void symbol_delete(struct symbol *sym);

int symbol_matchname(struct symbol *sym,const char *name);

void symbol_output(struct symbol *sym,FILE *f);

void symbol_dump(struct symbol *sym);

struct symbol *symbol_nextref(struct symbol *sym);
struct symbol *symbol_nextsymbol(struct symbol *sym);

int reference_size(struct symbol *sym);

#endif

