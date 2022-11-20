/*
	832util.c

	Copyright (c) 2019,2020 by Alastair M. Robinson

	This file is part of the EightThirtyTwo CPU project.

	EightThirtyTwo is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	EightThirtyTwo is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with EightThirtyTwo.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "832util.h"

static const char *error_file;
static int error_line;
static int debuglevel=0;

int getdebuglevel()
{
	return(debuglevel);
}

void setdebuglevel(int level)
{
	debuglevel=level;
}

void debug(int level,const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	if(level<=debuglevel)
	{
		vprintf(fmt,ap);
	}
	va_end(ap);
}

void hexdump(int level,char *p,int l)
{
	int i=0;
	if(level<=debuglevel)
	{
		while(l--)
		{
			unsigned int t=*p++;
			unsigned int t2=(t>>4)&15;
			t2+='0'; if(t2>'9') t2+='@'-'9';
			putchar(t2);
			t2=t&15;
			t2+='0'; if(t2>'9') t2+='@'-'9';
			putchar(t2);
			++i;
			if((i&3)==0)
				putchar(' ');
			if((i&15)==0)
				putchar('\n');
		}
		putchar('\n');
	}
}

void error_setfile(const char *fn)
{
	error_file=fn;
}

void error_setline(int line)
{
	error_line=line;
}

void linkerror(const char *err)
{
	fprintf(stderr,"Error in %s - %s\n",error_file,err);
	exit(1);
}

void asmerror(const char *err)
{
	fprintf(stderr,"Error in %s, line %d - %s\n",error_file,error_line,err);
	exit(1);
}

void write_int(int i,FILE *f,enum eightthirtytwo_endian endian)
{
	if(endian==EIGHTTHIRTYTWO_BIGENDIAN)
		write_int_be(i,f);
	else
		write_int_le(i,f);
}

void write_int_le(int i,FILE *f)
{
	fputc(i&255,f); i>>=8;
	fputc(i&255,f); i>>=8;
	fputc(i&255,f); i>>=8;
	fputc(i&255,f);
}

void write_int_be(int i,FILE *f)
{
	fputc((i>>24)&255,f);
	fputc((i>>16)&255,f);
	fputc((i>>8)&255,f);
	fputc(i&255,f);
}

void write_short(int i,FILE *f,enum eightthirtytwo_endian endian)
{
	if(endian==EIGHTTHIRTYTWO_BIGENDIAN)
		write_short_be(i,f);
	else
		write_short_le(i,f);
}

void write_short_le(int i,FILE *f)
{
	fputc(i&255,f); i>>=8;
	fputc(i&255,f);
}

void write_short_be(int i,FILE *f)
{
	fputc((i>>8)&255,f);
	fputc(i&255,f);
}

void write_lstr(const char *str,FILE *f)
{
	int l=strlen(str);
	fputc(l,f);
	fputs(str,f);
}


int read_int(FILE *f,enum eightthirtytwo_endian endian)
{
	if(endian==EIGHTTHIRTYTWO_BIGENDIAN)
		return(read_int_be(f));
	else
		return(read_int_le(f));
}

int read_int_le(FILE *f)
{
	int result;
	unsigned char buf[4];
	fread(buf,4,1,f);
	result=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
	return(result);
}

int read_int_be(FILE *f)
{
	int result;
	unsigned char buf[4];
	fread(buf,4,1,f);
	result=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3];
	return(result);
}

int read_short_le(FILE *f)
{
	int result;
	unsigned char buf[2];
	fread(buf,2,1,f);
	result=(buf[1]<<8)|buf[0];
	return(result);
}

void read_lstr(FILE *f,char *ptr)
{
	int l;
	fread(ptr,1,1,f);
	l=ptr[0];
	fread(ptr,l,1,f);
	ptr[l]=0;
}

int count_constantchunks(long v)
{
	int chunk = 1;
	long v2 = v;
	while (chunk<6 && ((v2 & 0xffffffe0) != 0) && ((v2 & 0xffffffe0) != 0xffffffe0))
	/* Are we looking at a sign-extended 6-bit value yet? */
	{
		v2 >>= 6;
		/* Sign-extend */
		if(v2&0x02000000)
			v2|=0xfc000000;
		++chunk;
	}
	return (chunk);
}


static int tooctal(char *str,int *count)
{
	int result=0;
	int c,i;
	i=0;
	c=*str++;
	while((c>='0')&&(c<='7'))
	{
		result<<=3;
		result+=c-'0';
		c=*str++;
		++i;
	}
	if(count)
		*count=i;
	return(result);
}

void parseescapes(char *str)
{
	int escaped=0;
	int i;
	int trunc;
	int l=strlen(str);
	for(i=0;i<l;++i)
	{
		trunc=0;
		if(escaped)
		{
			switch(str[i])
			{
				case '\\':
					trunc=1;
					break;
				case '?':
					trunc=1;
					str[i-1]='?';
					break;
				case '%':
					trunc=1;
					str[i-1]='%';
					break;
				case '\"':
					trunc=1;
					str[i-1]='\"';
					break;
				case 'n':
					trunc=1;
					str[i-1]='\n';
					break;
				case 'r':
					trunc=1;
					str[i-1]='\r';
					break;
				case 't':
					trunc=1;
					str[i-1]='\t';
					break;
				case '0':
					str[i-1]=(char)tooctal(&str[i],&trunc);
					break;
			}
			escaped=0;
		}
		else if(str[i]=='\\')
			escaped=1;
		if(trunc)
		{
			int j;
			for(j=i;j<(1+l-trunc);++j)
				str[j]=str[j+trunc];
			l-=trunc;
		}
	}
}

static char *delims=" \t:\n\r,";

/* A strtok equivalent with awareness of C-literal style escape sequences */
char *strtok_escaped(char *str)
{
	static char *ptr;
	char *result;
	int dl=strlen(delims);
	int i,j;
	char c,pc;
	int escaped=0;
	int quoted=0;
	if(str)
		ptr=str;
	/* Step over any leading delimiters */
	for(i=0;i<dl;++i)
	{
		if(!*ptr)
			return(0);
		if(*ptr==delims[i])
		{
			++ptr;
			i=-1;
		}
	}
	if(*ptr=='\"')
	{
		quoted=1;
		++ptr;
	}
	result=ptr;
	while(c=*ptr)
	{
		if(c=='"' && !escaped)
		{
			*ptr++=0;
			return(result);
		}
		else if(c=='\\' && pc!='\\')
			escaped=1;
		else
			escaped=0;
		if(!escaped && !quoted)
		{
			for(i=0;i<dl;++i)
			{
				if(c==delims[i])
				{
					*ptr++=0;
					return(result);
				}
			}
		}
		pc=c;
		++ptr;
	}
	return(result);
}

