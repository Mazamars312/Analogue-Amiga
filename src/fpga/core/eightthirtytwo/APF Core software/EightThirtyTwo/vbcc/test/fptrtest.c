/* Simulation time:
--stop-time=500us
*/

#include <stdio.h>
#include <string.h>

static int (*fptr)(const char *msg);
static int (**fptrptr)(const char *msg);

static int count=0;

static struct {
	int pad;
	int pad2;
	int pad3;
	int(*f)(const char *str);
} object;

int f1(const char *str)
{
	if(strcmp(str,"Hello world!\n")==0)
		count++;
	return(0);
}

int call()
{
	fptr("Hello world!\n");
	(*fptrptr)("Hello world!\n");
	return(0);
}

int main(int argc,char **argv)
{
	int iptr;
	fptr=f1;
	fptrptr=&fptr;
	iptr=(int)fptr;
	object.f=f1;
	object.f("Hello world!\n");
	((int (*)(const char *))iptr)("Hello world!\n");
	call();
	if(count==4)
		printf("\033[32mPassed\033[0m\n");
	else
		printf("\033[31mFailed\033[0m - result: %d\n",count);
	return(0);
}

