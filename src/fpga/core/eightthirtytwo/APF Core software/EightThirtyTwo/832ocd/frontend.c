/*
	frontend.c

	Copyright (c) 2020 by Alastair M. Robinson

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
#include <string.h>
#include <ncurses.h>
#include "frontend.h"


int frontend_confirm()
{
	curs_set(2);
	while(1)
	{
		char ch = getch();
		switch(ch)
		{
			case 'y':
			case 'Y':
				curs_set(0);
				return(1);
				break;
			case 'n':
			case 'N':
			case 27:
				curs_set(0);
				return(0);
				break;
			default:
				break;
		}
	}
}


char validatechar(char in,const char *options)
{
	int l;
	int i;
	if(!options)
		return(in);
	l=strlen(options);

	for(i=0;i<l;++i)
	{
		if(options[i]==in)
			return(in);
	}
	return(0);
}


#define STRINGBUFSIZE 29
static char stringbuf[STRINGBUFSIZE];

char *frontend_getinput(WINDOW *win,const char *prompt,int base)
{
	char *validkeys;
	int cursor;
	int promptlen=strlen(prompt);
	int prefixbytes=0;
	werase(win);
	mvwprintw(win,0,0,prompt);
	stringbuf[0]=' ';

	if(base==16)
	{
		stringbuf[1]='0';
		stringbuf[2]='x';
		stringbuf[3]=0;
		validkeys="abcdefABCDEF0123456789";
	}
	else if(base==10)
	{
		stringbuf[1]=0;
		validkeys="0123456789";
	}
	else
	{
		stringbuf[0]=0;
		validkeys=0;
	}
	
	cursor=prefixbytes=strlen(stringbuf);
	curs_set(2);

	while(1)
	{
		int ch;
		mvwprintw(win,0,promptlen,"%s ",stringbuf);
		wmove(win,0,promptlen+cursor);
		wrefresh(win);
		ch = getch();
		switch(ch)
		{
			case '-':
				stringbuf[0]=stringbuf[0]==' ' ? '-' : ' ';
				break;
			case '+':
				stringbuf[0]=' ';
				break;
			case KEY_BACKSPACE:
				if(cursor>prefixbytes)
					stringbuf[--cursor]=0;
				else
					stringbuf[0]=' ';
				break;
			case 27:
				stringbuf[0]=0;
				curs_set(0);
				return(stringbuf);
				break;
			case 10:
				curs_set(0);
				return(stringbuf);
				break;
			default:
				if(validatechar(ch,validkeys))
				{
					stringbuf[cursor++]=ch;
					stringbuf[cursor]=0;
				}
				break;
		}
		if(cursor>=(STRINGBUFSIZE-1))
			cursor=STRINGBUFSIZE-2;
	}
}


int frontend_choice(WINDOW *w,const char *prompt,const char *options,char def)
{
	int promptlen=strlen(prompt);
	int optbytes=strlen(options);
	werase(w);
	mvwprintw(w,0,0,prompt);
	stringbuf[0]=def;
	stringbuf[1]=0;
	curs_set(2);
	while(1)
	{
		int ch;
		mvwprintw(w,0,promptlen,"%s ",stringbuf);
		wmove(w,0,promptlen);
		wrefresh(w);

		ch=getch();
		if(ch==10 || ch==27)
			return(stringbuf[0]);
		else if(validatechar(ch,options))
		{
			stringbuf[0]=ch;
			return(stringbuf[0]);
		}
	}
}


