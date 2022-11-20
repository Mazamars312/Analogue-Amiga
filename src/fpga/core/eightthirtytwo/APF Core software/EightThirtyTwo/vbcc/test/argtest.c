#include "uart.h"
#include "stdarg.h"
#include "small_printf.h"


void vafunc(char *fmt,...)
{
	va_list va;
	va_start(va,fmt);	
	putchar(va_arg(va,int));
	putchar(va_arg(va,int));
	va_end(va);
}

int main(int argc,char **argv)
{
	small_printf("%s\n","test");
	return(0);
}

