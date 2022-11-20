/* Simulation time:
--stop-time=5ms
*/

#ifdef DEBUG
#include <stdio.h>
#else
#include "tiny_printf.h"
#endif

void sdiv(int n,int d,int e)
{
	if(e==n/d)
		printf("\033[32mPassed:\033[0m ");
	else
		printf("\033[31mFailed:\033[0m ");
	printf("%d / %d: %d, expected: %d\n",n,d,n/d,e);
}

void smod(int n,int d, int e)
{
	if(e==n%d)
		printf("\033[32mPassed:\033[0m ");
	else
		printf("\033[31mFailed:\033[0m ");
	printf("%d %% %d: %d, expected: %d\n",n,d,n%d,e);
}

void udiv(unsigned int n,unsigned int d,unsigned int e)
{
	if(e==n/d)
		printf("\033[32mPassed:\033[0m ");
	else
		printf("\033[31mFailed:\033[0m ");
	printf("%u / %u: %u, expected: %u\n",n,d,n/d,e);
}

void umod(unsigned int n,unsigned int d,unsigned int e)
{
	if(e==n%d)
		printf("\033[32mPassed:\033[0m ");
	else
		printf("\033[31mFailed:\033[0m ");
	printf("%u %% %u: %u, expected: %u\n",n,d,n%d,e);
}

int main(int argc,char **argv)
{
	sdiv(0x12345678,-0x1234,-65540);
	smod(0x12345678,-0x1234,3496);
	sdiv(-4321,0x21,-130);
	smod(-4321,0x21,-31);
	udiv(0xffffef1f,0x21,130150393);
	umod(0xffffef1f,0x21,6);
	udiv(0xcdba9876,0x12345,46289);
	umod(0xcdba9876,0x12345,18721);
	udiv(0x87654321,0x1,0x87654321);
	umod(0x87654321,0x1,0);
	udiv(0x12345678,0x1234,65540);
	umod(0x12345678,0x1234,3496);
	udiv(0x87654321,0x1,0x87654321);
	umod(0x87654321,0x1,0);
	sdiv(0x123,0x1234,0);
	smod(0x123,0x1234,0x123);

	return(0);
}
