#ifndef OBJECTFILE_H
#define OBJECTFILE_H

#include <stdio.h>

#include "section.h"
#include "symbol.h"
#include "equates.h"

struct objectfile
{
	char *filename;
	struct objectfile *next;
	struct section *sections;
	struct section *lastsection;
	struct section *currentsection;
	struct equate *equates;
	struct equate *lastequate;
};

struct objectfile *objectfile_new();
void objectfile_delete(struct objectfile *obj);

void objectfile_load(struct objectfile *obj,const char *fn);

struct section *objectfile_getsection(struct objectfile *obj);
struct section *objectfile_addsection(struct objectfile *obj, const char *sectionname);
struct section *objectfile_findsection(struct objectfile *obj,const char *sectionname);
struct section *objectfile_setsection(struct objectfile *obj, const char *sectionname);
struct symbol *objectfile_findsymbol(struct objectfile *obj, const char *symname);

int objectfile_containstouchedsection(struct objectfile *obj);

void objectfile_addequate(struct objectfile *obj,const char *equname,int value);
struct equate *objectfile_findequate(struct objectfile *obj, const char *equname);

void objectfile_dump(struct objectfile *obj,int untouched);

void objectfile_writemap(struct objectfile *obj,FILE *f,int locals);
void objectfile_emitbyte(struct objectfile *obj,unsigned char byte);
int objectfile_output(struct objectfile *obj,const char *filename);
#endif

