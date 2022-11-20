/*
	objectfile.c

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

#include "832a.h"
#include "objectfile.h"

static unsigned char tmp[256];

struct objectfile *objectfile_new()
{
	struct objectfile *obj;
	obj=(struct objectfile *)malloc(sizeof(struct objectfile));
	if(obj)
	{
		obj->filename=0;
		obj->next=0;
		obj->sections=0;
		obj->lastsection=0;
		obj->currentsection=0;
		obj->equates=0;
		obj->lastequate=0;
	}
	return(obj);
}

/*
	Loading an object file is fairly straightforward, the only complication
    is loading the binary section to codebuffers.
	Need to avoid splitting a reference between buffers, otherwise resolving
	and relaxing references will get very complicated.
	When loading chunks, check symbols that fall within this buffer for alignment,
	and add headroom for expansion.
	Check references to make sure there isn't one beginning in the last
	(6+alignment headroom) bytes, and if so, reduce the chunk size accordingly.

	UPDATE:
	Sidestepped this issue entirely by leaving out the placeholder bytes, so the
	assembler now just emits the reference table and leaves the linker to insert
	the correct number of bytes.
*/

void objectfile_load(struct objectfile *obj,const char *fn)
{
	FILE *f;
	struct section *sect=0;
	struct symbol *sym;
	obj->filename=strdup(fn);
	f=fopen(fn,"rb");
	if(!f)
		linkerror("Can't open file");
	fread(tmp,4,1,f);
	if(strncmp(tmp,"832\x02",4)!=0)
		linkerror("Not an 832 rev2 object file");

	while(fread(tmp,4,1,f))
	{
		int l;
		debug(1,"Chunk header: %s\n",tmp);
		if(strncmp(tmp,"832\x02",4)==0)	/* Another header - probably means objects have been concatenated. */
		{
			struct objectfile *new=objectfile_new();
			if(new)
			{
				obj->next=new;
				obj->filename=strdup(fn);
				obj=new;
				sect=0;
			}
		}
		else if(strncmp(tmp,"SECT",4)==0)
		{
			read_lstr(f,tmp);
			debug(1,"Section %s :\n",tmp);
			sect=objectfile_addsection(obj,tmp);
			sect->flags=read_int_le(f);
		}
		else if(strncmp(tmp,"BSS ",4)==0)
		{
			l=read_int_le(f);
			debug(1,"%d bytes of binary\n",l);
			sect->cursor=l;
		}
		else if(strncmp(tmp,"BNRY",4)==0)
		{
			l=read_int_le(f);
			debug(1,"%d bytes of binary\n",l);
			while(l>0)
			{
				section_loadchunk(sect,l>CODEBUFFERSIZE ? CODEBUFFERSIZE : l,f);
				l-=CODEBUFFERSIZE;
			}
		}
		else if(strncmp(tmp,"SYMB",4)==0)
		{
			fread(tmp,1,1,f);
			while(tmp[0]!=0xff)
			{
				int offset;
				int flags=tmp[0];
				int cursor;
				fread(tmp,1,1,f);	/* read second byte of flags */
				flags|=tmp[0]<<8;
				offset=read_int_le(f);
				cursor=read_int_le(f);
				read_lstr(f,tmp);
				debug(1,"Symbol: %s, cursor %d, flags %x, offset %d\n",tmp,cursor,flags,offset);
				sym=symbol_new(tmp,cursor,flags);
				if(sect && sym)
				{
					sym->offset=offset;
					section_addsymbol(sect,sym);
				}
				fread(tmp,1,1,f);
			}
		}
		else
			linkerror("Encountered bad chunk");
	}
	fclose(f);
}


struct section *objectfile_findsection(struct objectfile *obj,const char *sectionname)
{
	struct section *sect;
	if(!obj)
		return(0);
	sect=obj->sections;
	while(sect)
	{
		if(section_matchname(sect,sectionname))
			return(sect);
		sect=sect->next;
	}
	return(0);
}


struct section *objectfile_addsection(struct objectfile *obj, const char *sectionname)
{
	struct section *sect=section_new(obj,sectionname);
	if(sect)
	{
		sect->obj=obj;
		if(obj->lastsection)
			obj->lastsection->next=sect;
		else
			obj->sections=sect;
		obj->lastsection=sect;
	}
	return(sect);
}


struct section *objectfile_setsection(struct objectfile *obj, const char *sectionname)
{
	struct section *sect=objectfile_findsection(obj,sectionname);
	if(sect)
		obj->currentsection=sect;
	else
		obj->currentsection=objectfile_addsection(obj,sectionname);	
	return(obj->currentsection);
}


/* Return the current section.  If none has yet been defined, create one called ".text". */
struct section *objectfile_getsection(struct objectfile *obj)
{
	if(!obj->currentsection)
		objectfile_setsection(obj,".text");
	return(obj->currentsection);
}


struct symbol *objectfile_findsymbol(struct objectfile *obj, const char *symname)
{
	struct symbol *result;
	struct section *sect;
	if(!obj)
		return(0);
	sect=obj->sections;
	while(sect)
	{
		if(result=section_findsymbol(sect,symname))
			return(result);
		sect=sect->next;
	}
	return(0);
}


struct equate *objectfile_findequate(struct objectfile *obj,const char *equname)
{
	struct equate *result=0;
	if(obj)
	{
		result=obj->equates;
		while(result)
		{
			if(strcmp(result->identifier,equname)==0)
				return(result);
			result=result->next;
		}
	}
	return(result);
}


void objectfile_addequate(struct objectfile *obj,const char *equname,int value)
{
	if(obj)
	{
		struct equate *equ=equate_new(equname,value);
		if(equ)
		{
			if(obj->lastequate)
				obj->lastequate->next=equ;
			else
				obj->equates=equ;
			obj->lastequate=equ;
		}
	}
}


void objectfile_emitbyte(struct objectfile *obj,unsigned char byte)
{
	if(obj)
		section_emitbyte(objectfile_getsection(obj),byte);
}


void objectfile_delete(struct objectfile *obj)
{
	struct section *sect,*next;
	struct equate *equ;
	if(obj)
	{
		if(obj->filename)
			free(obj->filename);
		next=obj->sections;
		while(next)
		{
			sect=next;
			next=next->next;
			section_delete(sect);
		}
		equ=obj->equates;
		while(equ)
		{
			struct equate *neq;
			neq=equ->next;
			equate_delete(equ);
			equ=neq;
		}
		free(obj);
	}	
}

void objectfile_dump(struct objectfile *obj,int untouched)
{
	struct section *sect;
	debug(1,"\nObjectfile: %s\n",obj->filename ? obj->filename : "not yet defined");
	sect=obj->sections;
	while(sect)
	{
		section_dump(sect,untouched);
		sect=sect->next;
	}
}


int objectfile_output(struct objectfile *obj,const char *filename)
{
	char *err="Object filename missing";
	if(obj)
	{
		struct section *sect=obj->sections;

		err="Can't open object file for writing";
		FILE *f=fopen(filename,"wb");
		if(f)
		{
			fwrite("832",3,1,f);
			fputc(0x02,f); /* Revision 2 file */

			while(sect)
			{
				section_outputobj(sect,f);
				sect=sect->next;
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


void objectfile_writemap(struct objectfile *obj,FILE *f,int locals)
{
	struct section *sect;
	if(obj)
	{
		sect=obj->sections;
		while(sect)
		{
			section_writemap(sect,f,locals);
			sect=sect->next;
		}
	}
}


int objectfile_containstouchedsection(struct objectfile *obj)
{
	int result=0;
	struct section *sect;
	if(obj)
	{
		sect=obj->sections;
		while(sect)
		{
			if(sect->flags&SECTIONFLAG_TOUCHED)
				result=1;
			sect=sect->next;
		}
	}
	return(result);
}

