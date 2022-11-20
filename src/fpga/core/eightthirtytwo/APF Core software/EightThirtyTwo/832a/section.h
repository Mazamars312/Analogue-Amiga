#ifndef SECTION_H
#define SECTION_H

#include <stdio.h>
#include "codebuffer.h"
#include "symbol.h"
#include "objectfile.h"
#include "832util.h"

/* External section flags */
#define SECTIONFLAG_BSS 1
#define SECTIONFLAG_CTOR 2
#define SECTIONFLAG_DTOR 4
/* Internal flags */
#define SECTIONFLAG_TOUCHED 256

struct section
{
	struct section *next;
	char *identifier;
	int cursor;
	int flags;
	struct codebuffer *codebuffers;
	struct codebuffer *lastcodebuffer;
	struct symbol *symbols;
	struct symbol *lastsymbol;
	struct objectfile *obj;
	/* Used for linking */
	int address;
	int offset;	/* Total adjustment from references, aligns, etc. */
};


struct section *section_new(struct objectfile *obj,const char *name);
void section_clear(struct section *sect);
void section_delete(struct section *sect);

/* Section names are case-sensitive */
int section_matchname(struct section *sect,const char *name);
void section_touch(struct section *sect);

struct symbol *section_findsymbolbycursor(struct section *sect,int cursor);
struct symbol *section_findglobalsymbolbycursor(struct section *sect,int cursor);
struct symbol *section_findsymbol(struct section *sect,const char *symname);
struct symbol *section_getsymbol(struct section *sect, const char *symname);
void section_declaresymbol(struct section *sect, const char *name,int flags);
void section_addsymbol(struct section *sect, struct symbol *sym);

void section_declarereference(struct section *sect, const char *name,int flags,int offset);

void section_declarecommon(struct section *sect,const char *lab,int size,int global);
void section_declareconstant(struct section *sect,const char *lab,int size,int global);
void section_emitbyte(struct section *sect,unsigned char byte);
void section_write(struct section *sect,const char *buf,int size);
void section_align(struct section *sect,int align);

int section_sizereferences(struct section *sect);
int section_assignaddresses(struct section *sect,int base);

void section_loadchunk(struct section *sect,int bytes,FILE *f);
void section_outputobj(struct section *sect,FILE *f);
void section_outputexe(struct section *sect,FILE *f,enum eightthirtytwo_endian);
void section_dump(struct section *sect,int untouched);
void section_writemap(struct section *sect,FILE *f,int locals);
int section_assignaddresses(struct section *sect,int base);
void section_outputrelocs(struct section *sect,FILE *f,enum eightthirtytwo_endian endian);
#endif

