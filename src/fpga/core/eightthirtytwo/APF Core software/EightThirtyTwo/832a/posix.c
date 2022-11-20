#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "832util.h"

#ifdef SUPPLY_POSIX
 
char *strdup(const char *src)
{
	char *result=malloc(strlen(src)+1);
	if(result)
		strcpy(result,src);
	return(result);
}

ssize_t getdelim (char **lineptr, size_t *n, char delim, FILE *fp)
{
	char *buf=*lineptr;
	size_t bufsize=*n;
	size_t chars_read=0;
	int chr=0;
	
	/* Allocate buffer if necessary */
	if(!buf)
		buf=malloc(bufsize=256);

	do
	{
		chr=fgetc(fp);
		if(chars_read==bufsize-1)
		{
			bufsize*=2;
			buf=realloc(buf,bufsize);
		}
		if(buf && (chr!=EOF))
			buf[chars_read++]=chr;
	} while(buf && (chr!=delim) && (chr!=EOF));
	
	if(buf)
		buf[chars_read]=0;
	else
		chars_read=-1;

	*lineptr=buf;
	*n=bufsize;
	return(chars_read);
}

int strcasecmp(const char *s1, const char *s2)
{
	while (((*s1)&~32) == ((*s2++)&~32))
		if (*s1++ == 0)
			return 0;
	return(((*s1)&~32) - ((*--s2)&~32));
}

#endif

