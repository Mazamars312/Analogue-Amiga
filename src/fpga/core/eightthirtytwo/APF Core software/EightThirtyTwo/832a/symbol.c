/*
	symbol.c

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
#include <string.h>

#include "832util.h"
#include "symbol.h"

struct symbol *symbol_new(const char *id,int cursor,int flags)
{
	struct symbol *result;
	if(result=(struct symbol *)malloc(sizeof(struct symbol)))
	{
		result->next=0;
		result->identifier=strdup(id);
		result->cursor=cursor;
		result->flags=flags;
		result->offset=0;
		result->sect=0;
		result->resolve=0;
		result->address=0;
		result->size=0;
	}
	return(result);
}

void symbol_delete(struct symbol *sym)
{
	if(sym)
	{
		if(sym->identifier)
			free(sym->identifier);
		free(sym);
	}
}


struct symbol *symbol_nextref(struct symbol *sym)
{
	while(sym)
	{
		sym=sym->next;
		if(sym && SYMBOL_ISREF(sym))
			return(sym);
	}
	return(0);
}


struct symbol *symbol_nextsymbol(struct symbol *sym)
{
	while(sym)
	{
		sym=sym->next;
		if(sym && !SYMBOL_ISREF(sym))
			return(sym);
	}
	return(0);
}


int symbol_matchname(struct symbol *sym,const char *name)
{
	if(sym && name)
		return(strcmp(sym->identifier,name)==0);
	return(0);
}


/* Calculate the worst-case size for a reference.
   The only variable-size reference types are LDABS, LDPCREL and ALIGN
 */

static int count_pcrelchunks(unsigned int a1,unsigned int a2)
{
	int i;
	debug(1,"Counting displacement from %x to %x\n",a1,a2);
	for(i=1;i<6;++i)
	{
		unsigned int d=a2-(a1+i);
		d&=0xffffffff;
		if(i>=count_constantchunks(d))
			return(i);
	}
	return(6);
}

/* Calculate the size of a reference, and compare against the previously-defined size.
   References can only grow, not shrink.
   If the reference has grown, returns 1, otherwise returns 0 */
int reference_size(struct symbol *sym)
{
	int result=0;
	if(sym)
	{
		if(sym->flags&SYMBOLFLAG_ALIGN)
		{
			/* Size will be determined when assigning addresses */
		}
		else if(sym->flags&SYMBOLFLAG_REFERENCE)
		{
			/* simple references are always four bytes */
			sym->size=4;
		}
		else if (sym->size==0)
		{
			sym->size=1; /* Apart from aligns, references are always at least 1 byte. */
			result=1;
		}
		else if (sym->flags&SYMBOLFLAG_LDABS)
		{
			if(sym->resolve)
			{
				int size;
				/* Compute sizes based on the absolute address of the target. */
				if(sym->resolve->flags&SYMBOLFLAG_CONSTANT)
					size=count_constantchunks(sym->resolve->cursor);
				else
					size=count_constantchunks(sym->resolve->address+sym->offset);
				if(size>sym->size)
				{
					result=1;
					sym->size=size;
				}
			}
		}
		else if (sym->flags&SYMBOLFLAG_LDPCREL)
		{
			if(sym->resolve)
			{
				int i;
				int size;
				int addr=sym->sect->address+sym->cursor+sym->sect->offset+1;
				/* Compute worst-case sizes based on the distance to the target. */

				if(sym->resolve->flags&SYMBOLFLAG_CONSTANT)
					size=count_pcrelchunks(addr,sym->resolve->cursor);
				else
				{
					debug(1,"Reference %s, cursor %x, address %x\n",sym->identifier,sym->cursor,addr);
					size=count_pcrelchunks(addr,sym->resolve->address+sym->offset);
				}
				if(size>sym->size)
				{
					result=1;
					sym->size=size;
				}
			}
		}
	}
	return(result);
}


void symbol_output(struct symbol *sym,FILE *f)
{
	if(sym)
	{
		write_short_le(sym->flags,f);
		write_int_le(sym->offset,f);
		write_int_le(sym->cursor,f);
		write_lstr(sym->identifier,f);
	}
}


void symbol_dump(struct symbol *sym)
{
	if(sym)
	{
		debug(1,"%s, %s, cursor: %d, flags: %x, offset: %d\n",SYMBOL_ISREF(sym) ? "REF: " : "SYM: ",sym->identifier, sym->cursor,sym->flags,sym->offset);
		debug(1,"size %d, address %x\n",sym->size,sym->address);
	}
}

