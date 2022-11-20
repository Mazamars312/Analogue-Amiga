/*	EightThirtyTwo assembler

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

#include "832a.h"
#include "832util.h"

#include "objectfile.h"
#include "section.h"
#include "equates.h"
#include "expressions.h"
#include "peephole.h"


static char *delims=" \t:\n\r,";

void parsesourcefile(struct objectfile *obj,const char *fn,enum eightthirtytwo_endian endian);


void directive_symbolflags(struct objectfile *obj,char *tok,char *tok2,int key)
{
	struct section *sect=objectfile_getsection(obj);
	struct symbol *sym;
	if(sect)
	{
		sym=section_getsymbol(sect,tok);
		if(sym)
			sym->flags|=key;
	}
}


void directive_section(struct objectfile *obj,char *tok,char *tok2,int key)
{
	objectfile_setsection(obj,tok);
}


void directive_sectionflags(struct objectfile *obj,char *tok,char *tok2,int key)
{
	struct section *sect;
	if(sect=objectfile_setsection(obj,tok))
		sect->flags|=key;
}


/* Emit literal values in little-endian form */
void directive_literal(struct objectfile *obj,char *tok,char *tok2,int key)
{
	char *endptr;
	int v=strtoul(tok,&endptr,0);

	if(!v && endptr==tok)
		v=expression_evaluatestring(tok,obj->equates);

	if(key)
	{
		/* Big endian... */

		if(key<-1)
		{
			objectfile_emitbyte(obj,((v>>24)&255)); /* int */
			objectfile_emitbyte(obj,((v>>16)&255));
		}
		if(key<0)
			objectfile_emitbyte(obj,((v>>8)&255)); /* short */

		/* Single byte */

		objectfile_emitbyte(obj,(v&255));

		/* Little endian */
		if(key>1)
			objectfile_emitbyte(obj,((v>>8)&255)); /* short */
		if(key>2)
		{
			objectfile_emitbyte(obj,((v>>16)&255)); /* int */
			objectfile_emitbyte(obj,((v>>24)&255));
		}
	}
	else	/* .space directive */
	{
		int fill;
		int i;
		if(tok2)
			fill=strtoul(tok2,0,0);	/* If this fails we just get a zero, which is fine. */
		for(i=0;i<v;++i)
			objectfile_emitbyte(obj,fill);
	}
}


/* Emit literal string */
void directive_ascii(struct objectfile *obj,char *tok,char *tok2,int key)
{
	int l;
	if(tok)
	{
		parseescapes(tok);
		l=strlen(tok);
		debug(1,"Ascii string: %s, length: %d\n",tok,l);
		if(l)
		{
			int i;
			for(i=0;i<l;++i)
			{
				objectfile_emitbyte(obj,tok[i]);
			}
		}
		else
			asmerror("Bad ASCII string");
	}
}


void directive_label(struct objectfile *obj,char *tok,char *tok2,int key)
{
	struct section *sect=objectfile_getsection(obj);
	struct section *sect2=obj->sections;
	while(sect2)
	{
		if(sect!=sect2)
		{
			if(section_findsymbol(sect2,tok))
				asmerror("Symbol redefined\n");
		}
		sect2=sect2->next;
	}
	section_declaresymbol(sect,tok,0);
}


/* Add one of several types of reference to the current section.
   The reference can be embedded-absolute, load-absolute or load-PC relative */

void directive_reference(struct objectfile *obj,char *tok,char *tok2,int key)
{
	long offset=0;
	struct section *sect=objectfile_getsection(obj);
	if(tok2)
		offset=strtol(tok2,0,0);
	if(sect)
		section_declarereference(sect,tok,key,offset);
}


void directive_liconst(struct objectfile *obj,char *tok,char *tok2,int key)
{
	char *endptr;
	long v=strtol(tok,&endptr,0);
	int chunk;

	if(!v && endptr==tok)
		v=expression_evaluatestring(tok,obj->equates);

	chunk=count_constantchunks(v);
	debug(1,"%d chunks\n",chunk);
	while(chunk--)
	{
		int c=(v>>(6*chunk))&0x3f;
		c|=0xc0;
		debug(1,"Emitting chunk %d, %x\n",chunk,c);
		objectfile_emitbyte(obj,c);
	}
}


void directive_constant(struct objectfile *obj,char *tok,char *tok2,int key)
{
	unsigned int val;
	struct section *sect;
	if(!tok2)
		asmerror("Missing value for .constant");
	val=strtoul(tok2,0,0);
	sect=objectfile_getsection(obj);
	section_declareconstant(sect,tok,val,0);
}


void directive_align(struct objectfile *obj,char *tok,char *tok2,int key)
{
	int align=atoi(tok);
	struct section *sect=objectfile_getsection(obj);
	section_align(sect,align);
}


void directive_common(struct objectfile *obj,char *tok,char *tok2,int key)
{
	int size=atoi(tok2);
	struct section *sect=objectfile_getsection(obj);
	section_declarecommon(sect,tok,size,key);
}


void directive_include(struct objectfile *obj,char *tok,char *tok2,int key)
{
	if(tok)
	{
		parsesourcefile(obj,tok,(enum eightthirtytwo_endian)key);
	}
}


void directive_incbin(struct objectfile *obj,char *tok,char *tok2,int key)
{
	char buf[512];
	if(tok)
	{
		int s;
		FILE *f;
		struct section *sect=objectfile_getsection(obj);
		printf("Opening file %s\n",tok);
		f=fopen(tok,"rb");
		if(!f)
			asmerror(".incbin - can't open file\n");
		while(s=fread(buf,1,512,f))
		{
			printf("Adding %d bytes to section\n",s);
			section_write(sect,buf,s);
		}
		fclose(f);
	}
}


void directive_equate(struct objectfile *obj,char *tok,char *tok2,int key)
{
	int value;
	char *endptr;
	struct equate *equ;

	if(!tok)
		asmerror("Missing identifier");
	if(!tok2)
		asmerror("Missing value");

	value=strtoul(tok2,&endptr,0);
	if(!value && endptr==tok2)
		value=expression_evaluatestring(tok2,obj->equates);
	
	objectfile_addequate(obj,tok,value);
}


struct directive
{
	char *mnem;
	void (*handler)(struct objectfile *obj,char *token,char *token2,int key);
	int key;
};


struct directive directives[]=
{
	/* In big-endian mode start searching here */
	{".include",directive_include,EIGHTTHIRTYTWO_BIGENDIAN},
	{".int",directive_literal,-2},	/* Big endian variants */
	{".short",directive_literal,-1},
	/* In little-endian mode start the search here */
	{".include",directive_include,EIGHTTHIRTYTWO_LITTLEENDIAN},
	{".int",directive_literal,4},	/* Little endian variants */
	{".short",directive_literal,2},
	{".byte",directive_literal,1},
	{".space",directive_literal,0},
	{".equ",directive_equate,0},
	{".ctor",directive_sectionflags,SECTIONFLAG_CTOR},
	{".dtor",directive_sectionflags,SECTIONFLAG_DTOR},
	{".bss",directive_sectionflags,SECTIONFLAG_BSS},
	{".global",directive_symbolflags,SYMBOLFLAG_GLOBAL},
	{".globl",directive_symbolflags,SYMBOLFLAG_GLOBAL},
	{".weak",directive_symbolflags,SYMBOLFLAG_WEAK},
	{".section",directive_section,0},
	{".constant",directive_constant,0},
	{".align",directive_align,0},
	{".comm",directive_common,1},
	{".lcomm",directive_common,0},
	{".ascii",directive_ascii,0},
	{".ref",directive_reference,SYMBOLFLAG_REFERENCE},
	{".liabs",directive_reference,SYMBOLFLAG_LDABS},
	{".lipcrel",directive_reference,SYMBOLFLAG_LDPCREL},
	{".liconst",directive_liconst,0},
	{".incbin",directive_incbin,0},
	{0,0}
};


void parsesourcefile(struct objectfile *obj,const char *fn,enum eightthirtytwo_endian endian)
{
	FILE *f;
	struct peepholecontext pc;
	printf("Opening file %s\n",fn);
	
	error_setfile(fn);
	if(f=fopen(fn,"r"))
	{
		int line=0;
		char *linebuf=0;
		size_t len;
		int c;
		while(c=getline(&linebuf,&len,f)>0)
		{
			char *tok,*tok2,*tok3;
			++line;
			error_setline(line);
			if(tok=strtok_escaped(linebuf))
			{
				int d;
				tok3=tok2=strtok_escaped(0);
				if(tok2)
					tok3=strtok_escaped(0);
				/* comments */
				if((tok[0]=='/' && tok[1]=='/') || tok[0]==';' || tok[0]=='#')
					continue;
				/* Labels starting at column zero */
				if(linebuf[0]!=' ' && linebuf[0]!='\t' && linebuf[0]!='\n' && linebuf[0]!='\r')
				{
					peephole_clear(&pc);
					directive_label(obj,tok,0,0);
				}
				else
				{
					/* Search the directives table */
					if(endian==EIGHTTHIRTYTWO_LITTLEENDIAN)
						d=3;	/* Skip over the big-endian definitions */
					else
						d=0;
					while(directives[d].mnem)
					{
						if(strcasecmp(tok,directives[d].mnem)==0)
						{
							peephole_clear(&pc);
							directives[d].handler(obj,tok2,tok3,directives[d].key);
							break;
						}
						++d;
					}
					error_setfile(fn); /* This would have been changed by an include directive, so set it back */

					/* Not a directive?  Interpret as an opcode... */
					if(!directives[d].mnem)
					{
						int o=0;
						while(opcodes[o].mnem)
						{
							if(strcasecmp(tok,opcodes[o].mnem)==0)
							{
								int opc=opcodes[o].opcode;
								if(tok2 && opcodes[o].opbits==3) /* 3 bit literals - register or condition code */
								{
									int r=0;
									while(operands[r].mnem)
									{
										if(strcasecmp(tok2,operands[r].mnem)==0)
										{
											opc+=operands[r].opcode;
											break;
										}
										++r;
									}
									if(!operands[r].mnem)
										asmerror("bad register");
								}
								else if(tok2 && opcodes[o].opbits==6) /* 6 bit literal - immediate value */
								{
									char *endptr;
									unsigned int v=strtoul(tok2,&endptr,0);
									if(!v && endptr==tok2)
										asmerror("Invalid constant value");
									if(v>0x3f && v<(0xffffffc0ul))
										asmerror("Constant value out of range - suggest .liconst");										
									v&=0x3f;
									opc|=v;
								}
								debug(1,"%s\t%s -> 0x%x\n",tok, tok2 && opcodes[o].opbits ? tok2 : "", opc);
								if(peephole_test(&pc,opc))
									objectfile_emitbyte(obj,opc);
								break;
							}
							++o;
						}
						if(!opcodes[o].mnem)
							asmerror("syntax error\n");
					}
				}
			}
		}
		if(linebuf)
			free(linebuf);
		fclose(f);
	}
	else
		asmerror("Can't open file\n");
}


/* Attempt to assemble the named file.  Calls exit() on failure. */
void assemble(const char *fn,const char *on,enum eightthirtytwo_endian endian)
{
	int result=0;
	struct objectfile *obj;

	obj=objectfile_new();
	if(!obj)
		return(0);

	parsesourcefile(obj,fn,endian);

	result=objectfile_output(obj,on);
	objectfile_dump(obj,1);
	objectfile_delete(obj);
	printf("Output file: %s\n",on);

	if(!result)
		exit(1);
}

char *objname(const char *srcname)
{
	int l=strlen(srcname);
	int i;
	char *result=malloc(l+3);
	strcpy(result,srcname);
	for(i=l;i>0;--i)
	{
		if(result[i]=='.')
		{
			result[i+1]='o';
			result[i+2]=0;
			return(result);
		}
	}
	result[l]='.';
	result[l+1]='o';
	result[l+2]=0;
	return(result);
}

int main(int argc, char **argv)
{
	int i;
	int result=0;
	if(argc==1)
	{
		fprintf(stderr,"Usage: %s [options] file.asm <file2.asm> ...\n",argv[0]);
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t-o <file>\t- specify output file\n");
		fprintf(stderr,"\t-d - enable debug messages\n");
		result=1;
	}
	else
	{
		enum eightthirtytwo_endian endian=EIGHTTHIRTYTWO_LITTLEENDIAN;
		char *outfn=0;
		int nextfn=0;
		int nextendian=0;
		for(i=1;i<argc;++i)
		{
			if(strcmp(argv[i],"-d")==0)
					setdebuglevel(1);
			else if(strcmp(argv[i],"-o")==0)
					nextfn=1;
			else if(strncmp(argv[i],"-e",2)==0)
				nextendian=1;
			else if(nextfn)
			{
				outfn=argv[i];
				nextfn=0;
			}
			else if(nextendian)
			{
				if(*argv[i]=='l')
					endian=EIGHTTHIRTYTWO_LITTLEENDIAN;
				else if(*argv[i]=='b')
					endian=EIGHTTHIRTYTWO_BIGENDIAN;
				else
					asmerror("Endian flag must be \"little\" or \"big\"\n");
				nextendian=0;
			}
			else
			{
				char *on;
				if(!(on=outfn))
					on=objname(argv[i]);
				printf("Assembling %s with %s endian configuration\n",argv[i],endian==EIGHTTHIRTYTWO_LITTLEENDIAN ? "little" : "big");
				assemble(argv[i],on,endian);
				if(!outfn)
					free(on);
				outfn=0;
			}
			/* Dirty trick for when we have an option with no space before the parameter. */
			if((*argv[i]=='-') && (strlen(argv[i])>2))
			{
				argv[i]+=2;
				--i;
			}
		}
	}
	return(result);
}

