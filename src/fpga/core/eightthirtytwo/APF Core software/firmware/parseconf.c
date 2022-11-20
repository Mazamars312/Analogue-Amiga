/*
-- Standalone code to parse a MiST config string
-- Copyright (c) 2021 by Alastair M. Robinson

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty
-- of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <string.h>

#include "menu.h"

char *conf="TGFX16;;F,BINPCESGX,Load;P1,Video options;P1O12,Scandoubler Fx,None,CRT 25%,CRT 50%,CRT 75%;P1O3,Overscan,Hidden,Visible;P1O4,Border Color,Original,Black;P2,Controllers;P2O8,Swap Joysticks,No,Yes;P2O9,Turbo Tap,Disabled,Enabled;P2OA,Controller,2 Buttons,6 Buttons;P2OB,Mouse,Disable,Enable;T0,Reset;V,v1.0.210102";

int statusword; /* Support 32-bit status word initially - need to be 64-bit in the long run */

static char *confptr;

char conf_next()
{
	if(confptr)
		return(*confptr++);
	else
		return(0);
}


int conf_nextfield()
{
	int c=*confptr++;
	while(c && c!=';')
		c=*confptr++;
	return(c);
}


int getdigit()
{
	int c=-1;
	c=conf_next();
	if(!c || c==',' || c==';')
		c=-1;
	else
	{
		if(c>'9')
			c=c+10-'A';
		else
			c-='0';
	}
	return(c);	
}

#define LINELENGTH 32
char exts[32];
char menu_entry[16][LINELENGTH];


/* Copy a maximum of limit bytes to the output string, stopping when a comma is reached. */
/* If the copy flag is zero, don't copy, just consume bytes from the input */

int copytocomma(char *buf, int limit,int copy)
{
	int count=0;
	int c;
	c=conf_next();
	while(c && c!=',' && c!=';')
	{
		if(count<limit && copy)
			*buf++=c;
		if(c)
			++count;
		c=conf_next();
	}
	if(copy)
		*buf++=0;
	return(c==';' ? -count : count);
}


static void activate(int row);
static void scrollmenu(int row);

static char romfilenames[7][30];
static struct menu_entry menu[]=
{
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[0],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[1],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[2],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[3],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[4],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[5],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[6],MENU_ACTION(&activate)},
	{MENU_ENTRY_CALLBACK,0,0,0,romfilenames[7],MENU_ACTION(&activate)},
	{MENU_ENTRY_NULL,0,0,0,0,MENU_ACTION(&scrollmenu)}
};


int parseconf(int selpage,struct menu_entry *menu,int first,int limit)
{
	int c;
	int maxpage=0;
	int line=0;
	char *title;
	conf_nextfield(); /* Skip over core name */
	c=conf_nextfield(); /* Skip over core file extensions */
	while(c && line<limit)
	{
		c=conf_next();
		switch(c)
		{
			case 'F':
				{
					int i=0;
					c=conf_next(); /* Step over first comma */
					if(selpage==0)
					{
						while((exts[i++]=conf_next())!=',')
							;
						exts[i-1]=0;
						strcpy(menu[line].label,"Load");
						++line;
					}
//					printf("File selector, extensions %s\n",exts);
					c=conf_nextfield();
				}
				break;
			case 'P':
				{
					int page;
					page=getdigit();

//					printf("Page %d\n",page);

					if(page>maxpage)
						maxpage=page;
					c=getdigit();

					if(c<0)
					{
						/* Is this a submenu declaration? */
						if(selpage==0)
						{
							title=menu[line].label;
							menu[line].val=page;
							c=conf_next();
							while(c && c!=';')
							{
								*title++=c;
								c=conf_next();
							}
							*title++=' ';
							*title++='-';
							*title++='>';
							*title++=0;
							line++;
						}
						else
							c=conf_nextfield();
					}
					else if (page==selpage)
					{
						/* Must be a submenu entry */
						int low,high=0;
						int opt=0;
						int mask;

						/* Parse option */
						low=getdigit();
						high=getdigit();

						if(high<0)
							high=low;
						else
							conf_next();

						mask=(1<<(1+high-low))-1;
						menu[line].shift=low;
						menu[line].val=(statusword>>low)&mask;

						title=menu[line].label;
//						printf("selpage %d, page %d\n",selpage,page);
						if((c=copytocomma(title,LINELENGTH,selpage==page))>0)
						{
							if(c>0)
								title+=c;
							strncpy(title,": ",menu[line].label+LINELENGTH-title);
							title+=2;
							do
							{
								++opt;
							} while(copytocomma(title,menu[line].label+LINELENGTH-title,opt==menu[line].val+1)>0);
						}
//						printf("Decoded %d options\n",opt);
						menu[line].limit=opt;
						++line;
					}
					else
						c=conf_nextfield();
				}
				break;
			default:
				c=conf_nextfield();
				break;
		}
	}
	for(;line<8;++line)
	{
		*menu[line].label=0;
	}
	if(selpage)
	{
		strcpy(menu[7].label,"Back");
	}
//	printf("Maxpage %d\n",maxpage);
	return(maxpage);
}


static void activate(int row)
{


}

static int romindex=0;
static void scrollmenu(int row)
{
	switch(row)
	{
		case ROW_LINEUP:
			if(romindex)
				--romindex;
			break;
		case ROW_PAGEUP:
			romindex-=16;
			if(romindex<0)
				romindex=0;
			break;
		case ROW_LINEDOWN:
			++romindex;
			break;
		case ROW_PAGEDOWN:
			romindex+=16;
			break;
	}
//	listroms();
//	Menu_Draw();
}

void DrawMenu(struct menu_entry *menu)
{
	int i;
	for(i=0;i<8;++i)
	{
		printf("%s\n",menu[i].label);
	}
}


int main(int argc,char **argv)
{
	int i=0;
	int pages;
	statusword=0x18;

	confptr=conf;
	pages=parseconf(0,menu,romindex,8);
	DrawMenu(menu);

	printf("%d pages\n",pages);

	for(i=1;i<=pages;++i)
	{
		confptr=conf;
		parseconf(i,menu,romindex,8);
		DrawMenu(menu);
	}
//	while(parseconf(i))
//		confptr=conf;


	return(0);
}

