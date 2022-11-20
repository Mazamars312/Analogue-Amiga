/*
--stop-time=2ms
*/

#include <stdio.h>
#include <string.h>

struct mystruct
{
	char *str;
};

struct mystruct st;

char strbuf[20];

int main(int argc,char **argv)
{
	int i=0;
	st.str="HELLO, world!\n";

	for(i=0;i<strlen(st.str);++i)
		st.str[i]=tolower(st.str[i]);

	strcpy(strbuf,"Testing: ");
	strncat(strbuf,st.str,5);
	strcat(strbuf,st.str);
	for(i=0;i<strlen(strbuf);++i)
		strbuf[i]=toupper(strbuf[i]);

	i=strlen(strbuf);
	if(i==28)
		printf("strlen: \033[32mPassed\033[0m\n");
	else
		printf("strlen: \033[31mFailed\033[0m - got %d, should be 28\n",i);

	strncpy(strbuf,"Hello, World\n",7);
	if(strcmp(strbuf,"Hello, : HELLOHELLO, WORLD!\n")==0)
		printf("strcmp: \033[32mPassed\033[0m\n");
	else
		printf("strcmp: \033[31mFailed\033[0m - got %s\n",strbuf);

	if(strcmp("Hello, World     ","Hello, World   "))
		printf("strcmp2: \033[32mPassed\033[0m\n");
	else
		printf("strcmp2: \033[31mFailed\033[0m\n");

	if(strcmp("HELLO, World   ","Hello, WORLD"))
		printf("strcasecmp: \033[32mPassed\033[0m\n");
	else
		printf("strcasecmp: \033[31mFailed\033[0m\n");

	for(i=1;i<15;++i)
	{
		if(strncmp("Hello, World","Hello, WORLD",i))
		{
			if(i==9)
				printf("strncmp: \033[32mPassed\033[0m\n");
			else
				printf("strncmp: \033[31mFailed\033[0m - got %d\n",i);
			i=15;
		}
	}

	for(i=1;i<15;++i)
	{
		if(strncasecmp("HELLO, World!","Hello, WORLD",i))
		{
			if(i==13)
				printf("strncasecmp: \033[32mPassed\033[0m\n");
			else
				printf("strncasecmp: \033[31mFailed\033[0m - got %d\n",i);
			i=15;
		}
	}
	return(0);
}
