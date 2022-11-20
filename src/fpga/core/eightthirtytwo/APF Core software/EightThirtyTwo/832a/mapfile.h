#ifndef MAPFILE_H

#include "section.h"
#include "executable.h"

struct section *mapfile_read(const char *filename);
void mapfile_write(struct executable *exe,const char *fn,int locals);

#endif

