#ifndef SECTIONMAP_H
#define SECTIONMAP_H

#include "executable.h"
#include "section.h"

struct sectionmap_entry
{
	struct section *sect;
	int address;
};

#define BUILTIN_CTORS_START "__ctors_start__"
#define BUILTIN_CTORS_END "__ctors_end__"
#define BUILTIN_DTORS_START "__dtors_start__"
#define BUILTIN_DTORS_END "__dtors_end__"
#define BUILTIN_BSS_START "__bss_start__"
#define BUILTIN_BSS_END "__bss_end__"
#define BUILTIN_RELOCTABLE "__reloctable__"

struct sectionmap
{
	int entrycount;
	struct sectionmap_entry *entries;	/* Array, for speed and ease of sorting */
	struct section *builtins;	/* start and end markers for ctor/dtor/bss */
	struct section *lastbuiltin;
};

struct executable;
struct sectionmap *sectionmap_new(int reloc);
struct section *sectionmap_getbuiltin(struct sectionmap *map,const char *builtin);
int sectionmap_populate(struct executable *exe);

void sectionmap_delete(struct sectionmap *map);

void sectionmap_dump(struct sectionmap *map);

#endif

