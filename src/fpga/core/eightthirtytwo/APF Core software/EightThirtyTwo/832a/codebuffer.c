/*
	codebuffer.c

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
#include <stdlib.h>
#include <string.h>

#include "832util.h"
#include "codebuffer.h"

struct codebuffer *codebuffer_new()
{
	struct codebuffer *buf=(struct codebuffer *)malloc(sizeof(struct codebuffer));
	if(buf)
	{
		buf->next=0;
		buf->buffer=malloc(CODEBUFFERSIZE);
		buf->cursor=0;
	}
	return(buf);	
}

void codebuffer_delete(struct codebuffer *buf)
{
	if(buf)
	{
		if(buf->buffer)
			free(buf->buffer);
		free(buf);
	}
}

int codebuffer_put(struct codebuffer *buf,int val)
{
	if(!buf)
		return(0);
	if(buf->cursor<CODEBUFFERSIZE)
	{
		buf->buffer[buf->cursor]=val;
		++buf->cursor;
		return(1);
	}
	return(0);
}

int codebuffer_write(struct codebuffer *buf,const char *data,int size)
{
	int i;
	int s=CODEBUFFERSIZE-buf->cursor;
	if(!buf)
		return(0);
	if(size<s)
		s=size;
	for(i=0;i<s;++i)
	{
		buf->buffer[buf->cursor++]=*data++;
	}
	return(s);
}

void codebuffer_loadchunk(struct codebuffer *buf,int bytes,FILE *f)
{
	fread(buf->buffer,bytes,1,f);
	buf->cursor=bytes;
}


void codebuffer_output(struct codebuffer *buf,FILE *f)
{
	fwrite(buf->buffer,buf->cursor,1,f);
}


void codebuffer_dump(struct codebuffer *buf)
{
	if(buf)
	{
		hexdump(1,buf->buffer,buf->cursor);
	}
}
