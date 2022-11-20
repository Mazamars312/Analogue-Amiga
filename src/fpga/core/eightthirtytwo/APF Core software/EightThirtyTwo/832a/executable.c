/*
	executable.c

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
#include <string.h>
#include <stdlib.h>

#include "executable.h"
#include "832util.h"


/*	Linked executable.  Linking procedure is as follows:
	* Read object files one by one.
    * Find unresolvable references
	* Start with the first section, take each reference in turn
		* Find target symbols for all references, marking the section containing each reference as "touched".
			* If any references can't be found, throw an error
			* If we encounter a weak symbol at this stage, keep looking for a stronger one.
			* FIXME - If a strong symbol is declared more than once, throw an error.
			* As each section is touched, recursively repeat the resolution process
			* Store pointers to the target symbol for each reference.
	* Build a list of sections to be output, incorporating only sections that have been touched.
	* Sort sections:
		* The first section must remain so
		* ctors and dtors must be collected and sorted
		* BSS sections should come last
		* Need to define virtual symbols at the start and end of the BSS sections, and the ctor/dtor lists
	* Give each reference an initial minimum size.
	* Assign addresses to all sections, symbols and references.
	* Recalculate the size of each reference
	* Repeat the previous two steps until the references stop growing.
	* Save the linked executable
*/

struct executable *executable_new()
{
	struct executable *result=(struct executable *)malloc(sizeof(struct executable));
	if(result)
	{
		result->objects=0;
		result->lastobject=0;
		result->map=0;/*sectionmap_new(reloc);*/
		result->baseaddress=0;
		result->bssaddress=-1;
	}
	return(result);
}


void executable_setbaseaddress(struct executable *exe,int baseaddress)
{
	if(exe)
		exe->baseaddress=baseaddress;
}


void executable_setbssaddress(struct executable *exe,int bssaddress)
{
	if(exe)
		exe->bssaddress=bssaddress;
}


void executable_delete(struct executable *exe)
{
	if(exe)
	{
		struct objectfile *obj,*next;
		next=exe->objects;
		while(next)
		{
			obj=next;
			next=obj->next;
			objectfile_delete(obj);
		}
		if(exe->map)
			sectionmap_delete(exe->map);
		free(exe);
	}
}

void executable_loadobject(struct executable *exe,const char *fn)
{
	if(exe)
	{
		struct objectfile *obj=objectfile_new();
		if(exe->lastobject)
			exe->lastobject->next=obj;
		else
			exe->objects=obj;
		exe->lastobject=obj;

		objectfile_load(obj,fn);

		while(exe->lastobject->next)
			exe->lastobject=exe->lastobject->next;
	}
}

void executable_dump(struct executable *exe,int untouched)
{
	struct objectfile *obj;
	obj=exe->objects;
	while(obj)
	{
		objectfile_dump(obj,untouched);
		obj=obj->next;
	}
}


/* Hunt for a symbol, but exclude a particular section from the search.
   If a weak symbol is found the search continues for a stronger one.
   If no non-weak version is found, the last version declared will be used.
 */
struct symbol *executable_resolvereference(struct executable *exe,struct symbol *ref,struct section *excludesection)
{
	struct symbol *result=0;
	struct section *sect;
	if(exe)
	{
		struct objectfile *obj;
		/* Check builtin sections first */
		sect=exe->map->builtins;
		while(sect)
		{
			struct symbol *sym=section_findsymbol(sect,ref->identifier);
			if(sym)
			{
				ref->resolve=sym;
				return(sym);
			}
			sect=sect->next;
		}

		obj=exe->objects;
		while(obj)
		{
			sect=obj->sections;
			while(sect)
			{
				if(sect!=excludesection)
				{
					struct symbol *sym=section_findsymbol(sect,ref->identifier);
					if(sym)
					{
						debug(1,"%s's flags: %x, %x\n",ref->identifier,sym->flags,sym->flags&SYMBOLFLAG_GLOBAL);
						if(sym->flags&SYMBOLFLAG_WEAK)
						{
							debug(1,"Weak symbol found - keep looking\n");
							ref->resolve=sym;
							result=sym; /* Use this result if nothing better is found */
						}
						else if((sym->flags&SYMBOLFLAG_GLOBAL)==0)
						{
							if(excludesection && excludesection->obj == sect->obj)
							{
								debug(1,"Symbol found without global scope, but within the same object file\n");
								ref->resolve=sym;
								return(sym);
							}
							else
								debug(1,"Symbol found but not globally declared - keep looking\n");
						}
						else
						{
							ref->resolve=sym;
							return(sym);
						}
					}
				}
				sect=sect->next;
			}

			obj=obj->next;
		}
	}
	/* Return either zero or the most recently found weak instance */
	return(result);	
}


/* Resolve each reference within a section, recursively repeating the
   process for each section in which a reference was found.
   Marks as "touched" the supplied section and, through the recursion,
   the sections containing any resolved symbols. */

int executable_resolvereferences(struct executable *exe,struct section *sect)
{
	int result=1;
	struct symbol *ref;
	if(!sect)
		return(0);
	if(sect && (sect->flags&SECTIONFLAG_TOUCHED))
		return(1);
	section_touch(sect);

	ref=sect->symbols;
	if(!SYMBOL_ISREF(ref))
		ref=symbol_nextref(ref);

	while(ref)
	{
		if(!(ref->flags&SYMBOLFLAG_ALIGN)) /* Don't try and resolve an alignment ref */
		{
			/* This needs to search the current object file, not just the current section. */
			struct symbol *sym=objectfile_findsymbol(sect->obj,ref->identifier);
			struct symbol *sym2=0;
			if(sym)
			{
				debug(1,"Found symbol %s within the current section\n",sym->identifier);
			}
			if(!sym || (sym->flags&SYMBOLFLAG_WEAK))
			{
				debug(1,"Symbol %s %s - searching all sections...\n",ref->identifier, sym ? "is weak" : "not found");
				sym2=executable_resolvereference(exe,ref,sect);
			}
			if(sym2)
				sym=sym2;
			if(!sym)
			{
				fprintf(stderr,"\n*** %s - unresolved symbol: %s\n\n",sect->obj->filename,ref->identifier);
				result=0;
			}
			ref->resolve=sym;

			/* Recursively resolve references in the section containing the symbol just found. */
			if(sym)
				result&=executable_resolvereferences(exe,ref->resolve->sect);
		}
		ref=symbol_nextref(ref);
	}
	return(result);
}


int executable_resolvecdtors(struct executable *exe)
{
	int result=1;
	int ctorwarn=0;
	if(exe)
	{
		struct objectfile *obj=exe->objects;
		struct section *sect=sectionmap_getbuiltin(exe->map,BUILTIN_CTORS_START);

		/* Throw a warning if we have unreferenced ctors / dtors */
		if(sect && !(sect->flags & SECTIONFLAG_TOUCHED))
			ctorwarn=1;

		while(obj)
		{
			sect=obj->sections;
			while(sect)
			{
				if(!(sect->flags&SECTIONFLAG_TOUCHED))
				{
					if((sect->flags&SECTIONFLAG_CTOR) || (sect->flags&SECTIONFLAG_DTOR))
					{
						/* Having found a ctor or dtor we need to touch the function it references
						   but only if something else within the same objectfile has been referenced. */
						if(objectfile_containstouchedsection(obj))
						{
							result&=executable_resolvereferences(exe,sect);
							if(ctorwarn)
							{
								fprintf(stderr,"\nWARNING: ctors/dtors found but __ctors_start__ not referenced\n\n");
								ctorwarn=0;
							}
						}
					}
				}
				sect=sect->next;
			}
			obj=obj->next;
		}
	}

	return(result);
}


/* 
	Assign an initial best-case and worst-case address to all symbols.
*/

void executable_assignaddresses(struct executable *exe)
{
	int i;
	if(exe && exe->map)
	{
		struct sectionmap *map=exe->map;
		struct section *sect,*prev;
		struct section *bssstart;
		int j=0;
		int resolve=1;

		bssstart=sectionmap_getbuiltin(map,BUILTIN_BSS_START);

		/* Assign initial sizes to references */
		for(i=0;i<map->entrycount;++i)
		{
			sect=map->entries[i].sect;
			if(sect)
				section_sizereferences(map->entries[i].sect);
		}

		/* Make several passes through the sectionmap.
		   Assign addresses based on initial best-case sizes,
		   then recalculate reference sizes.  Repeat until
		   the references stop getting bigger. */

		while(resolve)
		{
			int sectionbase=exe->baseaddress;
			resolve=0;

			/* Now assign addresses */
			for(i=0;i<map->entrycount;++i)
			{
				sect=map->entries[i].sect;
				if(sect)
				{
					/* If the user has specified a BSS start address, apply it here. */
					if(sect==bssstart && exe->bssaddress>0)
					{
						if(exe->bssaddress<sectionbase && exe->bssaddress>=exe->baseaddress)
							linkerror("Specified BSS address would collide with code!");
						sectionbase=exe->bssaddress;
					}
					sectionbase=section_assignaddresses(sect,sectionbase);
				}
			}

			/* Refine reference sizes based on new addresses */
			for(i=0;i<map->entrycount;++i)
			{
				sect=map->entries[i].sect;
				if(sect)
					resolve|=section_sizereferences(map->entries[i].sect);
			}
			++j;
		}
		debug(0,"Address resolution stabilised after %d passes\n",j);
	}
}


int executable_save(struct executable *exe,const char *fn,enum eightthirtytwo_endian endian,int reloc)
{
	char *err=fn ? "Executable filename missing" : "No executable data to save";
	FILE *f;
	if(fn && exe && exe->map)
	{
		err="Can't open executable for writing";
		f=fopen(fn,"wb");
		if(f)
		{
			int i;
			struct sectionmap *map=exe->map;
			struct section *sect;

			for(i=0;i<map->entrycount;++i)
			{
				sect=map->entries[i].sect;
				if(sect)
					section_outputexe(sect,f,endian);
			}

			if(reloc)
			{
				for(i=0;i<map->entrycount;++i)
				{
					sect=map->entries[i].sect;
					if(sect)
						section_outputrelocs(sect,f,endian);
				}
			}
			fclose(f);
			err=0;
		}
	}
	if(err)
	{
		fprintf(stderr,"Error: %s\n",err);
		return(0);
	}
	return(1);
}


void executable_link(struct executable *exe, int reloc)
{
	/*	FIXME - Catch redefined global symbols */
	int result=1;
	int sectioncount;

	if(!exe)
		debug(0,"Link called with null executable!\n");

	if(!exe->map)
		exe->map=sectionmap_new(reloc);
	if(!exe->map)
		debug(0,"Section map creation failed!\n");

	/* Resolve references starting with the first section */
	if(exe && exe->objects && exe->objects->sections)
		result&=executable_resolvereferences(exe,exe->objects->sections);
	/* Resolve any ctor and dtor sections */
	result&=executable_resolvecdtors(exe);

	if(!result)
	{
		executable_delete(exe);
		exit(1);
	}

	sectionmap_populate(exe);
	sectionmap_dump(exe->map);

	executable_assignaddresses(exe);

	executable_dump(exe,0);
}


