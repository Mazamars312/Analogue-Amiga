#include <stdio.h>
#include "config.h"
#include "spi.h"
#include "configstring.h"
#include "arcfile.h"
#include "menu.h"

unsigned char configstring_coretype;
// #define configstring_next() SPI(0xff)

__weak unsigned char configstring_index=0;


/* If we're built with ARC file support then we need to be able to take config string data
   from the ARC file when showing the DIP switches page */

#ifdef CONFIG_ARCFILE
extern int menupage;
#define configstring_from_arc (menupage==MENUPAGE_DIPSWITCHES)
#endif

__weak int configstring_next()
{
	int result;
#ifdef CONFIG_ARCFILE
	if(configstring_from_arc)
		result=arcfile_next();
	else
#endif
		result=SPI(0xff);
	return(result);
}

__weak void configstring_begin()
{
#ifdef CONFIG_ARCFILE
	if(configstring_from_arc)
	{
		arcfile_begin();
		return;
	}
#endif
	SPI(0xff);
	SPI_ENABLE(HW_SPI_CONF);
	configstring_coretype=SPI(SPI_CONF_READ); /* Read conf string command */
}


__weak void configstring_end()
{
	SPI_DISABLE(HW_SPI_CONF);
}


__weak int configstring_nextfield()
{
	int c;
	do
		c=configstring_next();
	while(c && c!=';');
	return(c);
}

/* Copy a maximum of limit bytes to the output string, stopping when a comma is reached. */
/* If the copy flag is zero, don't copy, just consume bytes from the input */

__weak int configstring_copytocomma(char *buf, int limit,int copy)
{
	int count=0;
	int c;
	c=configstring_next();
	while(c && c!=',' && c!=';')
	{
		if(count<limit && copy)
			*buf++=c;
		if(c)
			++count;
		c=configstring_next();
	}
	if(copy)
		*buf++=0;
	return(c==',' ? count : -count);
}


__weak int configstring_getdigit()
{
	unsigned int c=configstring_next();
//	printf("Getdigit %c\n",c);
	if(c>='0' && c<='9')
		c-='0';
	if(c>='A' && c<='Z')
		c-='A'-10;
#ifdef CONFIG_STATUSWORD_64BIT
	if(c>='a' && c<='z')
		c-='a'-36;
#endif
	return(c);	
}


extern unsigned char romtype;
#define SPIFPGA(a,b) SPI_ENABLE(HW_SPI_FPGA); *spiptr=(a); *spiptr=(b); SPI_DISABLE(HW_SPI_FPGA);
__weak int configstring_setindex(const char *fn)
{
	register volatile int *spiptr=&HW_SPI(HW_SPI_DATA);
	/* Figure out which extension configstring_matches, and thus which index we need to use */
	int extindex=configstring_matchextension(fn);

	if(extindex)
		--extindex;

//	printf("Setting index to %x (%x, %x, %x)\n",0xff&(romtype|(extindex<<6)),extindex,romtype,configstring_index);

	SPIFPGA(SPI_FPGA_FILE_INDEX,romtype|(extindex<<6));
	return(extindex);
}


__weak int configstring_matchextension(const char *ext)
{
	int done=0;
	unsigned int i=0;
	int c=1;
	int c1,c2,c3;

#ifdef CONFIG_SETTINGS
	if(configstring_index==CONFIGSTRING_INDEX_CFG)
	{
		if(ext[8]=='C' && ext[9]=='F' && ext[10]=='G')
			return(1);
		return(0);
	}

#endif

#ifdef CONFIG_ARCFILE
	if(configstring_index==0)
	{
		if(ext[8]=='A' && ext[9]=='R' && ext[10]=='C')
			return(CONFIGSTRING_INDEX_ARC+1); /* Hack, adding one here, since setindex() reduces it by 1 */
		return(0);
	}

#endif

	configstring_begin();

/*  The first config entry has the corename, a semicolon, then optional match extensions.
    Subsequent configs have a type, a comma, then the extensions.
    We can find the correct entry by stepping over semicolons.  */

//	putchar('0'+configstring_index);

//	printf("Config index %d\n",configstring_index);

	for(i=0;i<=configstring_index;++i)
		configstring_nextfield();

/*	Having found the correct entry, we need to step over the descriptor if there is one. */

	if(configstring_index) /* No descriptor for the first entry. */
	{
		do
		{
			c=configstring_next();
//			putchar(c);
		} while(c && c!=',');
	}

	i=0;
//	putchar('\n');
	while(!done)
	{
		c1=configstring_next();
		c2=configstring_next();
		c3=configstring_next();
//		printf("%d, %d, %d, %d, %d, %d\n",c1,c2,c3,ext[8],ext[9],ext[10]);
		++i;
		if((c1==ext[8] || c1=='?') && (c2==ext[9] || c2=='?') && (c3==ext[10] || c3=='?')
			done=1;
		else if(c1==',' || c1==';')
		{
			i=0;
			done=1;
		}
	}
	configstring_end();
//	printf("Second match result %d\n",i);
	return(i);
}


