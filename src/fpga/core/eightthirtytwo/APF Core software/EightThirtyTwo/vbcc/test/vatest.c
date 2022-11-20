/* Simulation time:
--stop-time=500us
*/

#include "uart.h"
#include "stdarg.h"
#include "small_printf.h"

static char buf[16];

int main(int argc,char **argv)
{
	int a=512;
	int	b=10;
	printf("A: %d\n",42);
	printf("T: %s, %c, %d, %d\n","Testing",65,a/b,a%b);
	printf("X: %x\n",0x42);
	return(0);
}

