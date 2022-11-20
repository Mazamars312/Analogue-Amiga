/*
	sectionmap.c

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

#include "sectionmap.h"
#include "symbol.h"

static struct section *sectionmap_addbuiltin(struct sectionmap *map,const char *id,int flags,int align)
{
	struct section *sect=section_new(0,id);
	if(sect)
	{
		sect->flags|=flags;
		if(align)
			section_align(sect,align);
		section_declaresymbol(sect,id,SYMBOLFLAG_GLOBAL);
		if(map->lastbuiltin)
			map->lastbuiltin->next=sect;
		else
			map->builtins=sect;
		map->lastbuiltin=sect;
	}
	return(sect);
}

struct sectionmap *sectionmap_new(int reloc)
{
	struct sectionmap *result;
	result=(struct sectionmap *)malloc(sizeof(struct sectionmap));
	if(result)
	{
		result->entrycount=0;
		result->entries=0;
		result->builtins=0;
		result->lastbuiltin=0;
		sectionmap_addbuiltin(result,BUILTIN_CTORS_START,SECTIONFLAG_CTOR,0);
		sectionmap_addbuiltin(result,BUILTIN_CTORS_END,SECTIONFLAG_CTOR,0);
		sectionmap_addbuiltin(result,BUILTIN_DTORS_START,SECTIONFLAG_DTOR,0);
		sectionmap_addbuiltin(result,BUILTIN_DTORS_END,SECTIONFLAG_DTOR,0);
		if(reloc)
			sectionmap_addbuiltin(result,BUILTIN_RELOCTABLE,SECTIONFLAG_BSS,0); /* Overlaps BSS - only needed at startup */
		sectionmap_addbuiltin(result,BUILTIN_BSS_START,SECTIONFLAG_BSS,4);
		sectionmap_addbuiltin(result,BUILTIN_BSS_END,SECTIONFLAG_BSS,4);
	}
	return(result);
}

void sectionmap_delete(struct sectionmap *map)
{
	if(map)
	{
		struct section *sect=map->builtins;
		while(sect)
		{
			struct section *next=sect->next;
			section_delete(sect);
			sect=next;
		}
		if(map->entries)
			free(map->entries);
		free(map);
	}
}


/* Return a count of the number of sections that have been touched while resolving references. */
static int countsections(struct executable *exe)
{
	struct section *sect=0;
	int result=0;
	if(exe)
	{
		struct objectfile *obj=exe->objects;
		sect=exe->map->builtins;
		while(sect)
		{
/*			if(sect->flags&SECTIONFLAG_TOUCHED) */
				++result;
			sect=sect->next;
		}

		while(obj)
		{
			sect=obj->sections;
			while(sect)
			{
				if(sect->flags&SECTIONFLAG_TOUCHED)
					++result;
				sect=sect->next;
			}
			obj=obj->next;
		}
	}
	return(result);	
}


struct section *sectionmap_getbuiltin(struct sectionmap *map,const char *builtin)
{
	struct section *sect=map->builtins;
	while(sect && !section_matchname(sect,builtin))
		sect=sect->next;
	return(sect);
}

/* Sort a subsection of the entry map */

static int sectionmap_sortcompare(const void *p1, const void *p2)
{
	struct sectionmap_entry *e1,*e2;
	e1=(struct sectionmap_entry *)p1;
	e2=(struct sectionmap_entry *)p2;
	if(!e1 || !e1->sect)
		return(1);
	if(!e2 || !e2->sect)
		return(-1);
	return(strcmp(e1->sect->identifier,e2->sect->identifier));
}


/* Scan the executable for sections matching a particular set of flags
   (plain code, ctors/dtors, bss, etc) and add them to the sectionmap. */

static int sectionmap_populate_inner(struct executable *exe,int idx,int flags)
{
	struct sectionmap *map=exe->map;
	struct objectfile *obj=exe->objects;

	/* Collect together the non-ctor/dtor and non-bss sections first */
	while(obj)
	{
		struct section *sect=obj->sections;
		while(sect)
		{
			if((sect->flags&SECTIONFLAG_TOUCHED) &&
							((sect->flags&(SECTIONFLAG_CTOR|SECTIONFLAG_DTOR|SECTIONFLAG_BSS))==flags))
				map->entries[idx++].sect=sect;
			sect=sect->next;
		}
		obj=obj->next;
	}
	return(idx);
}


/* Populate the section map and sort any ctors / dtors */

int sectionmap_populate(struct executable *exe)
{
	int i;
	struct section *sect;
	struct sectionmap *map=exe->map;
	map->entrycount=countsections(exe);
	debug(1,"%d sections touched\n",map->entrycount);

	if(map->entries=malloc(sizeof(struct sectionmap_entry)*map->entrycount))
	{
		struct objectfile *obj=exe->objects;
		int idxstart=0;
		int idx=0;

		memset(map->entries,0,sizeof(struct sectionmap_entry)*map->entrycount);

		/* Collect together the non-ctor/dtor and non-bss sections first */
		idx=sectionmap_populate_inner(exe,idx,0);

		sect=sectionmap_getbuiltin(map,BUILTIN_CTORS_START);
		if(sect && sect->flags&SECTIONFLAG_TOUCHED)
		{
			map->entries[idx++].sect=sect;
			/* Now collect ctors...*/
			idxstart=idx;
			idx=sectionmap_populate_inner(exe,idx,SECTIONFLAG_CTOR);

			/* Sort ctors */
			debug(1,"Sorting constructors %d to %d\n",idxstart,idx-1);
			if(idx>idxstart)
				qsort(map->entries+idxstart,idx-idxstart,sizeof(struct sectionmap_entry),sectionmap_sortcompare);

			sect=sectionmap_getbuiltin(map,BUILTIN_CTORS_END);
			map->entries[idx++].sect=sect;				
			sect=sectionmap_getbuiltin(map,BUILTIN_DTORS_START);
			map->entries[idx++].sect=sect;

			idxstart=idx;
			idx=sectionmap_populate_inner(exe,idx,SECTIONFLAG_DTOR);

			/* Sort dtors */
			debug(1,"Sorting destructors %d to %d\n",idxstart,idx-1);
			if(idx>idxstart)
				qsort(map->entries+idxstart,idx-idxstart,sizeof(struct sectionmap_entry),sectionmap_sortcompare);

			sect=sectionmap_getbuiltin(map,BUILTIN_DTORS_END);
			map->entries[idx++].sect=sect;					

		}
		/* Add reloc table if present */
		sect=sectionmap_getbuiltin(map,BUILTIN_RELOCTABLE);
		if(sect)
			map->entries[idx++].sect=sect;

		/* Collect BSS */
		sect=sectionmap_getbuiltin(map,BUILTIN_BSS_START);
		map->entries[idx++].sect=sect;

		idxstart=idx;
		idx=sectionmap_populate_inner(exe,idx,SECTIONFLAG_BSS);

		sect=sectionmap_getbuiltin(map,BUILTIN_BSS_END);
		map->entries[idx++].sect=sect;					

		return(1);
	}
	return(0);
}


void sectionmap_dump(struct sectionmap *map)
{
	if(map->entries)
	{
		int i;
		for(i=0;i<map->entrycount;++i)
			debug(1,"Section %s at offset %d\n",map->entries[i].sect ? map->entries[i].sect->identifier : "(none)",
						map->entries[i].address);
	}
	else
		debug(1,"Section map is empty\n");
}


