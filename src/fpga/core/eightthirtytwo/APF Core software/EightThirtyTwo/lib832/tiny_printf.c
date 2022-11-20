#include "uart.h"
#include "stdarg.h"

__weak int printf(const char *fmt, ...)
{
    va_list ap;
    int ret=0;
	unsigned int c;
	int nextfmt=0;
	int length;

    va_start(ap, fmt);

	while((c=*fmt++))
	{
		if(nextfmt)
		{
			unsigned int c2;
			int i;
			int nz;
			int val;
			nextfmt=0;
			length=0;
	        // Process output
	        switch (c) {
			    case 'd':
			    case 'u':
			    case 'x':
				case 'p':
					if(val=va_arg(ap,int))
					{
						nz=0;
						for(i=0;i<8;++i)
						{
							c2=(val>>28)&0xf;
							val<<=4;
							if(c2)
								nz=1;	// Non-zero?  Start printing then.
							if(c2>9)
								c2+='A'-10;
							else
								c2+='0';
							if(nz)	// If we've encountered only zeroes so far we don't print.
							{
								putchar(c2);
								++ret;
							}
						}
					}
					else
					{
						putchar('0');
						++ret;
					}
				    break;
			    case 's':
			        ret+=puts(va_arg(ap, char *));
			        break;
				case 'l':
				case '0':
					nextfmt=1;
					break;
			    case 'c':
			        putchar(va_arg(ap, int /*char*/));
					ret++;
			        continue;
			    default:
			        putchar(c);
			        continue;
	        }
		}
		else
		{
			if(c=='%')
				nextfmt=1;
			else
				putchar(c);
		}
	}
	va_end(ap);
    return (ret);
}

