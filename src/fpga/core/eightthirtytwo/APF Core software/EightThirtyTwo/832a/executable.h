#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "objectfile.h"
#include "sectionmap.h"
#include "832util.h"

struct executable
{
	struct objectfile *objects;
	struct objectfile *lastobject;
	struct sectionmap *map;
	int baseaddress;
	int bssaddress;
};


struct executable *executable_new();
void executable_delete(struct executable *exe);

void executable_loadobject(struct executable *exe,const char *fn);
struct symbol *executable_findsymbol(struct executable *sect,const char *symname,struct section *excludesection);

void executable_setbaseaddress(struct executable *exe,int baseaddress);
void executable_link(struct executable *exe, int reloc);
int executable_save(struct executable *exe,const char *fn,enum eightthirtytwo_endian,int reloc);
void executable_setbssaddress(struct executable *exe,int bssaddress);
void executable_dump(struct executable *exe,int untouched);

#endif
