/* Standard library string functions implemented in C */

#include <stdio.h>
#include "string.h"


size_t strspn(const char *str, const char *inc)
{
	size_t result=0;
	char c;
	while(1)
	{
		const char *s2=inc;
		char d;
		int	match=0;
		if(!(c=*str++))
			return(result);
		d=*s2++;
		while(d)
		{
			if(c==d)
			{
				match=1;
				d=0;
			}
			else
				d=*s2++;
		}
		if(match)
			++result;
		else
			return(result);
	}
}


size_t strcspn(const char *str, const char *exc)
{
	size_t result=0;
	char c;
	while(1)
	{
		const char *s2=exc;
		char d;
		int	match=0;
		if(!(c=*str++))
			return(result);
		d=*s2++;
		while(d)
		{
			if(c==d)
			{
				match=1;
				d=0;
			}
			else
				d=*s2++;
		}
		if(!match)
			++result;
		else
			return(result);
	}
}


char *strtok_r(char *str,const char *delim, char **saveptr)
{
	size_t c;
	if(str)
		*saveptr=str;
	if(!*saveptr)
		return(0);
	str=(*saveptr)+strspn(*saveptr,delim);
	c=strcspn(str,delim);
	if(c)
	{
		if(str[c])
		{
			str[c]=0;
			*saveptr=str+c+1;
		}
		else
			*saveptr=0;
		return(str);
	}
	else
		return(0);
}

char *strtok(char *str,const char *delim)
{
	static char *saveptr=0;
	return(strtok_r(str,delim,&saveptr));
}

/* returns a negative number if the digit is invalid, else the digit's value */
int todigit(int c,int base)
{
//	c&=~32;	/* Convert any hex digits to lower case */
	if(c>='A' && c<='F' && base==16)
		return(10+c-'A');
	if(c>='a' && c<='f' && base==16)
		return(10+c-'a');
	c=c-48;
	if(c>9)
		c=-1;
	return(c);
}

int isdigit(int c)
{
	return(c>='0' && c<='9');
}

long atoi(const char *nptr)
{
	int neg=0;
	int result=0;
	const char *p=nptr;
	if(*p=='-')
	{
		neg=1;
		++p;
	}
	while(1)
	{
		int digit;
		digit=todigit(*p++,10);
		if(digit<0)
			return(neg ? -result : result);
		result*=10;
		result+=digit;
	}
}

long strtol(const char *in,char **endptr,int base)
{
	int neg=0;
	int result=0;
	int digit;
	const char *p=in;
	p+=strspn(p," ");
	if(*p=='-')
	{
		neg=1;
		++p;
	}
	while(1)
	{
		if(endptr)
			*endptr=(char *)p;
		if(!*p)
			return(neg ? -result : result);
		digit=todigit(*p++,base);
		if(digit<0)
			return(neg ? -result : result);
		result*=base;
		result+=digit;
	}
}

unsigned long strtoul(const char *in,char **endptr,int base)
{
	int neg=0;
	int result=0;
	int digit;
	const char *p=in;
	p+=strspn(p," ");
	while(1)
	{
		if(endptr)
			*endptr=(char *)p;
		if(!*p)
			return(result);
		digit=todigit(*p++,base);
		if(digit<0)
			return(result);
		result*=base;
		result+=digit;
	}
}

const char *strchr(const char *str,char c)
{
	char t;
	do{
		t=*str;
		if(t==c)
			return str;
		++str;
	} while(t);
	return(0);
}

