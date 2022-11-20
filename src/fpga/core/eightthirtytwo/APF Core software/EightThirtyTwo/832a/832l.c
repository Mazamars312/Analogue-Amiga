/*
	832l.c - EightThirtyTwo linker

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

#include "832util.h"
#include "executable.h"
#include "objectfile.h"
#include "mapfile.h"


int main(int argc,char **argv)
{
	if(argc==1)
	{
		fprintf(stderr,"Usage: %s [options] obj1.o <obj2.o> ... <-o output.bin>\n",argv[0]);
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t-e big|little\t- specify bit or little endian configuration\n");
		fprintf(stderr,"\t-o <file>\t- specify output file\n");
		fprintf(stderr,"\t-b <number>\t- specify base address\n");
		fprintf(stderr,"\t-m <mapfile>\t- write a map file\n");
		fprintf(stderr,"\t-M <mapfile>\t- write a map file including static / local symbols\n");
		fprintf(stderr,"\t-s <symbol>=<number>\t- define symbol (such as stack size)\n");
		fprintf(stderr,"\t-r\t\t- emit a relocation table within the executable\n");
		fprintf(stderr,"\t-d\t\t- enable debug messages\n");
	}
	else
	{
		int i;
		enum eightthirtytwo_endian endian=EIGHTTHIRTYTWO_LITTLEENDIAN;
		int locals=0;
		int nextfn=0;
		int nextbase=0;
		int nextsym=0;
		int nextmap=0;
		int nextendian=0;
		int reloc=0;
		char *outfn="a.out";
		char *mapfn=0;
		struct executable *exe=executable_new();
		if(exe)
		{
			for(i=1;i<argc;++i)
			{
				if(strncmp(argv[i],"-m",2)==0)
					nextmap=1;
				else if(strncmp(argv[i],"-M",2)==0)
					nextmap=locals=1;
				else if(strncmp(argv[i],"-e",2)==0)
					nextendian=1;
				else if(strncmp(argv[i],"-o",2)==0)
					nextfn=1;
				else if(strncmp(argv[i],"-d",2)==0)
					setdebuglevel(1);
				else if(strncmp(argv[i],"-r",2)==0)
					reloc=1;
				else if(strncmp(argv[i],"-b",2)==0)
					nextbase=1;
				else if(!nextsym && strncmp(argv[i],"-s",2)==0)
					nextsym=1;
				else if(nextsym)
				{
					/* Deferred to a second pass so there's a section to add the symbol to. */
					nextsym=0;
				}
				else if(nextbase)
				{
					char *tmp;
					unsigned long addr=strtoul(argv[i],&tmp,0);
					if(tmp==argv[i] && addr==0)
						fprintf(stderr,"Bad base address - using 0\n");
					printf("Setting base address to 0x%lx\n",addr);
					executable_setbaseaddress(exe,addr);
					if(*tmp==',')
					{
						++tmp;
						addr=0;
						if(!*tmp)
							tmp=argv[++i];
						addr=strtoul(tmp,&tmp,0);
						if(tmp==argv[i] && addr==0)
							fprintf(stderr,"Bad BSS base address - ignoring\n");
						else
						{
							printf("Setting bss base address to 0x%lx\n",addr);
							executable_setbssaddress(exe,addr);
						}
					}
					nextbase=0;
				}
				else if(nextfn)
				{
					outfn=argv[i];
					nextfn=0;
				}
				else if(nextmap)
				{
					mapfn=argv[i];
					nextmap=0;
				}
				else if(nextendian)
				{
					if(*argv[i]=='l')
						endian=EIGHTTHIRTYTWO_LITTLEENDIAN;
					else if(*argv[i]=='b')
						endian=EIGHTTHIRTYTWO_BIGENDIAN;
					else
						linkerror("Endian flag must be \"little\" or \"big\"\n");
					nextendian=0;
				}
				else
				{
					error_setfile(argv[i]);
					executable_loadobject(exe,argv[i]);
				}

				/* Dirty trick for when we have an option with no space before the parameter. */
				if((*argv[i]=='-') && (strlen(argv[i])>2))
				{
					if(nextsym)
						nextsym=0;
					else
					{
						argv[i]+=2;
						if(*argv[i]=='=')
							++argv[i];
						--i;
					}
				}
			}

			/* Perform a second pass of the command line arguments to create symbols: */

			for(i=1;i<argc;++i)
			{
				if(strncmp(argv[i],"-s",2)==0)
					nextsym=1;
				else if(nextsym)
				{
					char *tok=strtok(argv[i],"= ");
					char *tok2=strtok(0,"= ");
					if(tok && tok2)
					{
						struct section *sect;
						char *endptr;
						unsigned long v=strtoul(tok2,&endptr,0);
						printf("Setting %s to %s\n",tok,tok2);
						if(!v && endptr==tok2)
							asmerror("Command-line symbol definition must be a number");
						/* Declare a symbol with constant value from the command line. */
						if(exe->objects)
							section_declareconstant(exe->objects->sections,tok,v,1);
					}
					nextsym=0;
				}

				/* Dirty trick for when we have an option with no space before the parameter. */
				if((*argv[i]=='-') && (strlen(argv[i])>2))
				{
					argv[i]+=2;
					if(*argv[i]=='=')
						++argv[i];
					--i;
				}
			}

			printf("Linking...\n");
			executable_link(exe,reloc);
			printf("Saving with %s endian configuration to %s\n",endian==EIGHTTHIRTYTWO_LITTLEENDIAN ? "little" : "big",outfn);
			if(!executable_save(exe,outfn,endian,reloc))
				exit(1);

			if(mapfn)
			{
				printf("Writing map file %s\n",mapfn);
				mapfile_write(exe,mapfn,locals);
			}

			executable_delete(exe);
		}
	}
	return(0);
}

