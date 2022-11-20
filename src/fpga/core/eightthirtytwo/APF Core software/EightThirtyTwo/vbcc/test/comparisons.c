/* Simulation time:
--stop-time=500us
*/

#include <stdio.h>
#include <string.h>

struct mystruct
{
	int v1,v2;	// 0 - 7
	unsigned int v3,v4;	// 8 - 15
	char	c1,c2;	// 16-17 (padded?)
	unsigned char	c3,c4;	// 16-17 (padded?)
	short	s1,s2;	// 18-21
	unsigned short s3,s4;	// 22-25
};

struct mystruct st=
{
	0xc0000000,0x1000,
	0xffff0000,0x0100,
	0x9c,0x49,
	0x9c,0x49,
	0x1234,0xfedc,
	0x1234,0xfedc
};

char resultstring[16];
void addchar(char c)
{
	int i=strlen(resultstring);
	resultstring[i++]=c;
	resultstring[i]=0;
}

int main(int argc,char **argv)
{
	resultstring[0]=0;
	char *p=resultstring;
	if(st.v1<st.v2) addchar('0'); else addchar('A');
	if(st.v3<st.v4)	addchar('1'); else addchar('B');
	if(st.v1<st.v3)	addchar('2'); else addchar('C');
	if(st.v1<st.c1)	addchar('3'); else addchar('D');
	if(st.c1<st.c2) addchar('4'); else addchar('E');
	if(st.c3<st.c4) addchar('5'); else addchar('F');
	if(st.s1<st.s2) addchar('6'); else addchar('G');
	if(st.s3<st.s4) addchar('7'); else addchar('H');

	st.c1=0x1c;
	if(st.c1<st.c2) addchar('8'); else addchar('I');

	if(strcmp(resultstring,"0B234FG78")==0)
		printf("\033[32mPassed (signed char)\033[0m\n");
	else if(strcmp(resultstring,"0B23EFG78")==0) 
		printf("\033[32mPassed (unsigned char)\033[0m\n");
	else
		printf("\033[31mFailed\033[0m - got %s",resultstring);

//	puts("(Should be: 0B234FG78 if built without -unsigned-char flag,\n0B23EFG78 otherwise.)\n");
	
	return(0);
}

