
#include <stdio.h>

struct teststruct
{
	int r1;
	int r2;
	char *tmp;
};

int function(char c, short d, register short *e)
{
	d+=c;
	d+=d;
	*e+=d;
	return(d);
}


int main(int argc,char **argv)
{
	int v1=2;
	int v2=3;
	short r=0x5555;
	struct teststruct ts;
	printf("Test\n");
	ts.r1=v1;
	ts.r2=v2;
	ts.tmp="Hello, world\n";
	printf(ts.tmp+ts.r1+ts.r2);
	printf("%d\n",function(64,3,&r));
	printf("%x\n",r);
	return(0);
}

