#include <stdio.h>
#include "statusword.h"
#include "spi.h"
#include "user_io.h"
#include "config.h"

unsigned int statusword; /* Low 32-bits of status word */
#ifdef CONFIG_STATUSWORD_64BIT
unsigned int statusword_high; /* Upper 32-bits of a 64-bit status word */
#endif

__weak void sendstatus()
{
	SPI(0xff);
	SPI_ENABLE(HW_SPI_CONF);
	SPI(UIO_SET_STATUS2);
	SPI(statusword);
	SPI(statusword>>8);
	SPI(statusword>>16);
	SPI(statusword>>24);
#ifdef CONFIG_STATUSWORD_64BIT
	SPI(statusword_high);
	SPI(statusword_high>>8);
	SPI(statusword_high>>16);
	SPI(statusword_high>>24);
#endif
	SPI_DISABLE(HW_SPI_CONF);
}

#ifdef CONFIG_STATUSWORD_64BIT

/* 64bit implementation of status_get() */
int statusword_get(unsigned int shift,unsigned int mask)
{
	unsigned int v;
	v=(statusword>>shift);	// Extract value from existing statusword and prepare insertion masks
	if(shift>=32)
		v=statusword_high>>(shift-32);
	else
		v|=statusword_high<<(32-shift);
	v&=mask; // and mask...
	return(v);
}

/* 64bit implementation of status_cycle() */
void statusword_cycle(unsigned int shift,unsigned int mask,unsigned int limit)
{
	unsigned int v,v1,v2;
	unsigned int m1,m2;
	v=(statusword>>shift);	// Extract value from existing statusword and prepare insertion masks
	if(shift>=32)
	{
		m1=0;
		m2=mask<<(shift-32);
		v=statusword_high>>(shift-32);
	}
	else
	{
		m1=mask<<shift;
		m2=mask>>(32-shift);
		v|=statusword_high<<(32-shift);
	}
	v&=mask; // and mask...
	++v;
	if(v>=limit)
		v=0;
	if(shift>=32) // Shift the new value for re-insertion
	{
		v1=0;
		v2=v<<(shift-32);
	}
	else
	{
		v1=v<<shift;
		v2=v>>(32-shift);
	}
	statusword&=~(m1);  // Mask off old value from status word
	statusword_high&=~(m2); // Mask off old value from status word
	statusword|=v1;     // and insert new value
	statusword_high|=v2;    // and insert new value

	sendstatus();
}

#else

/* 32bit implementation of status_get() */
unsigned int statusword_get(unsigned int shift,unsigned int mask)
{
	unsigned int v;
	v=(statusword>>shift);	// Extract value from existing statusword and prepare insertion masks
	v&=mask; // and mask...
	return(v);
}

/* 32bit implementation of status_cycle() */
__weak void statusword_cycle(int shift,int mask,int limit)
{
	int v;
	v=(statusword>>shift);	// Extract value from existing statusword
	v&=mask;					// and mask...
	++v;
	if(v>=limit)
		v=0;
	statusword&=~(mask<<shift); // Mask off old value from status word
	statusword|=v<<shift;		// and insert new value

	sendstatus();
}

#endif

