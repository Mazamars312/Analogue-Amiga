/*	Disassembler for 832

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
#include "832opcodes.h"
#include "832util.h"
#include "section.h"
#include "mapfile.h"


void file_disassemble(const char *filename,struct section *symbolmap,enum eightthirtytwo_endian endian)
{
	FILE *f;
	f=fopen(filename,"rb");
	if(f)
	{
		int a=0;
		int c;
		int imm;
		int signedimm;
		int immstreak=0;
		while((c=fgetc(f))!=EOF)
		{
			int opc=c&0xf8;
			int opr=c&7;
			int j;
			struct symbol *s;
			s=section_findsymbolbycursor(symbolmap,a);
			if(s && s->cursor==a)
				printf("%s:\n",s->identifier);
			if((opc&0xc0)==0xc0)
			{
				if(immstreak)
					imm<<=6;
				else
					imm=c&0x20 ? 0xffffffc0 : 0;
				imm|=c&0x3f;
				if(imm&0x80000000)
				{
					signedimm=~imm;
					signedimm+=1;
					signedimm=-signedimm;
				}
				else
					signedimm=imm;
				printf("  %05x\t%02x\tli\t%x\t(0x%x, %d)\n",a,c,c&0x3f,imm,signedimm);
				immstreak=1;
			}
			else
			{
				int found=0;
				/* Look for overloads first... */
				if((c&7)==7)
				{
					for(j=0;j<sizeof(opcodes)/sizeof(struct opcode);++j)
					{
						if(opcodes[j].opcode==c)
						{
							found=1;
							printf("  %05x\t%02x\t%s\n",a,c,opcodes[j].mnem);
							break;
						}
					}
				}
				if(!found)
				{
					/* If not found, look for base opcodes... */
					for(j=0;j<sizeof(opcodes)/sizeof(struct opcode);++j)
					{
						if(opcodes[j].opcode==opc)
						{
							if((c==(opc_add+7) || c==(opc_addt+7)) && immstreak)
							{
								printf("  %05x\t%02x\t%s\t%s\t(%x)\n",
									a,c,opcodes[j].mnem,operands[(c&7)|((c&0xf8)==0 ? 8 : 0)].mnem,a+1+signedimm);
							}
							else
								printf("  %05x\t%02x\t%s\t%s\n",a,c,opcodes[j].mnem,operands[(c&7)|((c&0xf8)==0 ? 8 : 0)].mnem);
							if(c==(opc_ldinc+7))
							{
								int v=read_int(f,endian);
								printf("\t\t0x%x\n",v);
								a+=4;
							}
							break;
						}
					}
				}
				immstreak=0;
			}
			++a;
		}		
		fclose(f);
	}
	else
		fprintf(stderr,"Can't open %s\n",filename);
}

int main(int argc,char **argv)
{
	if(argc==1)
	{
		fprintf(stderr,"Usage: %s [options] binaryfile1 [binaryfile2 ...]\n",argv[0]);
		fprintf(stderr,"Options:\n");
		fprintf(stderr,"\t-e big|little\t- specify bit or little endian configuration\n");
		fprintf(stderr,"\t-m <mapfile>\t- read a map file\n");
	}
	else
	{
		int nextmap=0;
		int nextendian=0;
		struct section *symbolmap=0;
		enum eightthirtytwo_endian endian=EIGHTTHIRTYTWO_LITTLEENDIAN;
		int i;
		for(i=1;i<argc;++i)
		{
			if(strncmp(argv[i],"-m",2)==0)
				nextmap=1;
			else if(strncmp(argv[i],"-e",2)==0)
				nextendian=1;
			else if(nextmap)
			{
				symbolmap=mapfile_read(argv[i]);
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
				file_disassemble(argv[i],symbolmap,endian);
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
		if(symbolmap)
			section_delete(symbolmap);
	}
	return(0);
}

