#include "uart.h"

/* Declare with weak linkage; the function can then be overridden if your hardware differs. */
int putchar(int c)
{
	volatile unsigned int *uart=&HW_UART(REG_UART);
	do {} while(!((*uart)&(1<<REG_UART_TXREADY)));

	*uart=c;
	return(c);
}


int puts(const char *msg)
{
	unsigned char c;
	int result=0;

	while(c=*msg++)
	{
		putchar(c);
		++result;
	}
	return(result);
}

void SetUART(int sys_clock, int uart_rate)
{
	SET_UART(0) = ((sys_clock * 1000)/uart_rate);
};
