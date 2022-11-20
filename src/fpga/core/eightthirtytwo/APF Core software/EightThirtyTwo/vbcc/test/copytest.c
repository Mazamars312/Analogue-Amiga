/* Simulation time:
--stop-time=500us
*/

#include <stdio.h>
#include <string.h>

static count=0;

int func(const char *str)
{
	if(strcmp(str,"Hello, world!\n")==0)
		++count;
	return(0);
}

struct teststruct
{
	int a,b;
	char *cptr;
	int (*fptr)(const char *msg);
};

struct teststruct mystruct=
{
	42,255,"Hello, world!\n",func
};

int main(int argc,char **argv)
{
	struct teststruct localcopy=mystruct;
	localcopy.fptr(localcopy.cptr);
	if(count==1)
		printf("\033[32mPassed\033[0m\n");
	else
		printf("\033[31mFailed\033[0m - got %d",count);

	return(0);
}

