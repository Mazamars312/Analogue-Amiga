#include <stdio.h>
#include <stdlib.h>

#include "mapfile.h"

struct section *mapfile_read(const char *filename)
{
	struct section *result=0;
	if(filename)
	{
		FILE *f;
		f=fopen(filename,"r");
		if(f)
		{
			if(result=section_new(0,"symboltable"))
			{
				int line=0;
				char *linebuf=0;
				size_t len;
				int c;
				while(c=getline(&linebuf,&len,f)>0)
				{
					char *endptr;
					if(linebuf[0]!=' ') /* Skip over section size entries */
					{
						int v=strtoul(linebuf,&endptr,0);
						if(!v && endptr==linebuf)
							linkerror("Invalid constant value");
						else
						{
							if(endptr[1]==' ')
							{
								struct symbol *sym;
								char *tok=strtok_escaped(endptr);
								if(sym=symbol_new(tok,v,0))
									section_addsymbol(result,sym);
							}
						}
					}
				}
				if(linebuf)
					free(linebuf);
			}
			fclose(f);
		}	
	}

	return(result);
}


void mapfile_write(struct executable *exe,const char *fn,int locals)
{
	FILE *f=fopen(fn,"w");
	if(f)
	{
		struct sectionmap *map=exe->map;
		struct section *sect;
		int i;
		int prevaddr=0;
		for(i=0;i<map->entrycount;++i)
		{
			sect=map->entries[i].sect;
			if(sect)
			{
				fprintf(f,"           (0x%x bytes)\n",sect->address-prevaddr);
				section_writemap(sect,f,locals);
				prevaddr=sect->address;
			}
		}
		fclose(f);
	}
}

