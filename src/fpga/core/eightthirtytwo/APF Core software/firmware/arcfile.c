#include <stdio.h>
#include <string.h>

#include "statusword.h"
#include "arcfile.h"

static char *matchstrings[]={"CONF=","MOD","NAME","DEFAULT=0"};

struct arcfilestate arcstate;

void arcfile_init()
{
	arcstate.file.size=0;
}


int arcfile_open(const char *filename)
{
	int result=0;
	char buffer[4];
	memset(&arcstate,0,sizeof(arcstate));
	result=FileOpen(&arcstate.file,filename);
	if(result && FileRead(&arcstate.file,buffer,4))
	{
		if(strncmp(buffer,"[ARC",4))
			return(0);
	}
	else
		return(0);

	/* Iterate through the file to fetch both the mod value and the ROM filename */
	arcfile_begin();
	while(arcfile_next())
		;

	if(arcstate.def)
	{
		statusword=arcstate.def;
		sendstatus();
	}

	return(1);
}


void arcfile_begin()
{
	FileSeek(&arcstate.file,0);
	arcstate.param=ARC_PREAMBLE;
}


char arcfile_next()
{
	int result=0;
	while(!result)
	{
		int c;

		/* Begin by returning two semicolons to avoid having to special-case the menu code */		
		if(arcstate.param < ARC_NONE)
		{
			++arcstate.param;
			return(';');
		}
		
		c=FileGetCh(&arcstate.file);
		if(c<0)
			return(0);
		switch(arcstate.param)
		{
			int c2;
			int i;

			/* Detect a match with either "CONF=", "MOD" or "NAME".
			   In each case one extra character will be consumed since a comparison with
			   the trailing null byte is what indicates that comparison has succeeded. */
			
			case ARC_NONE:
				for(i=0;i<4;++i)
				{
					c2=matchstrings[i][arcstate.matchidx[i]++];
					if(c!=c2)
						arcstate.matchidx[i]=0;
					if(!c2)
						arcstate.param=ARC_CONF+i;
				}
				break;

			/* Extract the ROM filename from the ARC file */
			case ARC_NAME:
				if((arcstate.nameidx>8) || c==10 || c==13)
				{
					int i;
					for(i=arcstate.nameidx;i<8;++i)
						arcstate.name[i]=' ';
					arcstate.name[8]='R';
					arcstate.name[9]='O';
					arcstate.name[10]='M';
					arcstate.name[11]=0;
					arcstate.param=ARC_NONE;
				}
				else
					arcstate.name[arcstate.nameidx++]=c;
				break;
				
			/* Mod is just a number, probably in base 10.  Note, it could be in base 16, with 0x prefix - should handle this case */
			case ARC_MOD:
				if(c>='0' && c<='9')
					arcstate.mod=arcstate.mod*10+(c-'0');
				if(c==10 || c==13)
					arcstate.param=ARC_NONE;
				break;
				
			/* Janky hex string decoder - this should be made more robust, but should cope with mra-tools-c's output. */
			case ARC_DEFAULT:
				if(c>='0' && c<='9')
					arcstate.def=arcstate.def*16+(c-'0');
				if(c>='A' && c<='F')
					arcstate.def=arcstate.def*16+(10+c-'A');
				if(c==10 || c==13)
					arcstate.param=ARC_NONE;
				break;
			
			/* Return successive characters from the config string, terminating each line with ';' */
			case ARC_CONF:
				if(c=='\"')
					result=';';
				else if(c==10 || c==13)
					arcstate.param=ARC_NONE;
				else
					result=c;
				break;
		}

	}
	return(result);
}

