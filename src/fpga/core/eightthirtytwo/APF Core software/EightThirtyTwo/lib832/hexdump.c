
#include "uart.h"

void hexdump(unsigned char *p,unsigned int l)
{
	unsigned int *p2=(unsigned int *)p;
	l=(l/4);
	while(l--)
	{
		int i;
		unsigned int t=*p2++;
		for(i=0;i<8;++i)
		{
			unsigned int t2=(t>>28);
			t2+='0';
			if(t2>'9')
				t2+='@'-'9';
			putchar(t2);
			t<<=4;
		}
		putchar(' ');
		if((l&3)==0)
			putchar('\n');
	}
	putchar('\n');
}


