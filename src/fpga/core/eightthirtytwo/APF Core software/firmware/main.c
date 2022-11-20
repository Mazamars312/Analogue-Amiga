/*
-- Minimalist MiST-compatibile controller firmware
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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#include "configstring.h"
#include "uart.h"
#include "spi.h"
#include "minfat.h"
#include "interrupts.h"
#include "ps2.h"
#include "mouse.h"
#include "user_io.h"
#include "osd.h"
#include "menu.h"
#include "font.h"
#include "cue_parser.h"
#include "pcecd.h"
#include "spi_sd.h"
#include "diskimg.h"
#include "statusword.h"
#include "settings.h"
#include "arcfile.h"

#define Breadcrumb(x) HW_UART(REG_UART)=x;

#define DIRECTUPLOAD 0x10

#define LINELENGTH 32


int menupage;
unsigned char romtype=0;
unsigned char unit=0;

fileTYPE file;

__weak int rom_minsize=1;

#ifdef CONFIG_SETTINGS
char std_label_exit[]="             Exit            \x81";
char std_label_back[]="\x80            Back";
#else
char std_label_exit[]="\x80 Exit";
char std_label_back[]="\x80 Back";
#endif

#ifndef CONFIG_WITHOUT_FILESYSTEM
#define SPIFPGA(a,b) SPI_ENABLE(HW_SPI_FPGA); *spiptr=(a); *spiptr=(b); SPI_DISABLE(HW_SPI_FPGA);
int LoadROM(const char *fn)
{
	register volatile int *spiptr=&HW_SPI(HW_SPI_DATA);
	int i;
	if(FileOpen(&file,fn))
	{
		int minsize=rom_minsize;
		int sendsize;
		int idx;

		idx=configstring_setindex(fn);

#ifdef CONFIG_ARCFILE
		/* Load Arc file, then load the associated ROM */
		if(idx==CONFIGSTRING_INDEX_ARC)
		{
			int default_dip;
			if((default_dip=arcfile_open(fn)))
			{
				spi_uio_cmd8(UIO_SET_MOD, arcstate.mod);
				romtype=0; /* ROMs loaded alongside .arc files must have index 0 */
				LoadROM(arcstate.name);
			}
			return(1);
		}
#endif

//		if(configstring_coretype&DIRECTUPLOAD)	/* Send a dummy file info */
//		{
//			unsigned int i;
			SPI_ENABLE(HW_SPI_FPGA);
			*spiptr=SPI_FPGA_FILE_INFO;
			for(i=0;i<11;++i)
				*spiptr=fn[i];
			for(i=12;i<32;++i)
				*spiptr=0xff;
			SPI_DISABLE(HW_SPI_FPGA);
//		}
		*spiptr=0xFF;

		SPIFPGA(SPI_FPGA_FILE_TX,1);

		while(minsize>0)
		{
			int imgsize=file.size;
			minsize-=imgsize;
//			printf("Sending %d bytes\n",imgsize);
			while(imgsize)
			{
				char *buf=sector_buffer;
				int result;

				sendsize=512;
				imgsize-=512;
				if(imgsize<0)
				{
					sendsize=imgsize+512;
					imgsize=0;
				}

				if(configstring_coretype&DIRECTUPLOAD)
					result=FileReadSector(&file,0);
				else
				{
					result=FileReadSector(&file,sector_buffer);
					SPI_ENABLE_FAST_INT(HW_SPI_FPGA);
					*spiptr=SPI_FPGA_FILE_TX_DAT;
					do
					{
						*spiptr=*buf++;
					} while(--sendsize);
					SPI_DISABLE(HW_SPI_FPGA);
				}
				if(!result)
					return(0);

				FileNextSector(&file,1);
			}
			if(minsize>0)
				FileFirstSector(&file); // Start from the beginning again.
		}

		SPIFPGA(SPI_FPGA_FILE_TX,0);
#ifdef CONFIG_AUTOCLOSE_OSD
		
#endif
		return(1);
	}
	else
		return(0);
}
#endif


int menuindex;
int moremenu;
int romindex; /* First file to be displayed */
static int listroms();
void selectrom(int row);
// static void scrollroms(int row);
static void scrollmenu(int row);
void buildmenu(int set);
static void submenu(int row);
void parseconf(int selpage,struct menu_entry *menu,unsigned int first,unsigned int limit);

static char romfilenames[7][30];

struct menu_entry menu[]=
{
	{MENU_ACTION(&selectrom),romfilenames[0],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[1],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[2],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[3],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[4],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[5],0,0,0},
	{MENU_ACTION(&selectrom),romfilenames[6],0,0,0},
	{MENU_ACTION(&submenu),0,0,0,0},
	{MENU_ACTION(scrollmenu),0,0,0,0}
};


static DIRENTRY *nthfile(unsigned int n)
{
	unsigned int i,j=0;
	DIRENTRY *p;
	for(i=0;j<=n;++i)
	{
		p=NextDirEntry(i==0,configstring_matchextension);
		++j;
		if(!p)
			j=n;
	}
	return(p);
}

extern char cd_buffer[2352];

void spi32le(int x)
{
	SPI(x&255);
	SPI((x>>8)&255);
	SPI((x>>16)&255);
	SPI((x>>24)&255);
} 

#ifdef CONFIG_CD
int setcuefile(const char *filename)
{
	int cue_valid=0;
	if(!cue_open(filename))
	{
		int i=1;
		do {
			if(cue_parse(i)==0)
				cue_valid=1;
		} while(++i<=toc.last);
	}
	// send mounted image size first then notify about mounting
	EnableIO();
	SPI(UIO_SET_SDINFO);
	// use LE version, so following BYTE(s) may be used for size extension in the future.
	spi32le(cue_valid ? toc.file.size : 0);
	spi32le(cue_valid ? toc.file.size : 0);
	spi32le(0); // reserved for future expansion
	spi32le(0); // reserved for future expansion
	// notify core of possible sd image change
	DisableIO();
	spi_uio_cmd8(UIO_SET_SDSTAT, 1);
	return(cue_valid);
}
#endif

#ifdef CONFIG_WITHOUT_FILESYSTEM

static void fileselector(int row){}
void selectrom(int row) {}
static void listroms() {}
static void scrollroms(int row) {}

#else

__weak int loadimage(char *filename,int unit)
{
	switch(unit)
	{
		case 0:
			return(LoadROM(filename));
			break;
#ifdef CONFIG_DISKIMG
		case '0':
		case '1':
		case '2':
		case '3':
			diskimg_mount(0,unit-'0');				
			return(diskimg_mount(filename,unit-'0'));				
			break;
#endif
#ifdef CONFIG_CD
		case 'C':
//				printf("Opening %s\n",filename ? filename : "(null)");
			return(setcuefile(filename));
			break;
#endif
#ifdef CONFIG_SETTINGS
		case 'S':
			return(loadsettings(filename));
			break;
		case 'T':
			return(savesettings(filename));
			break;
#endif
	}
}

char filename[12];
void selectrom(int row)
{
	if(!romfilenames[row][0])	// Did the user select an empty row?
		return;
	DIRENTRY *p=nthfile(menuindex+row);
//	printf("File %s\n",p->Name);
	if(p)
	{
		strncpy(filename,p->Name,11);	// Store a temporary copy of the filename, since loading it by name
										// will overwrite the sector buffer which currently contains it!
		menu[row].label="Loading...";
		Menu_Draw(row);
		menu[row].label=romfilenames[row];
		loadimage(filename,unit);
	}
	Menu_Draw(row);
	Menu_ShowHide(0);
	menupage=0;
	buildmenu(1);
}


static void selectdir(int row)
{
	DIRENTRY *p=nthfile(menuindex+row);
	if(p)
		ChangeDirectory(p);
	menuindex=0;
	buildmenu(1);
}


static int listroms()
{
	DIRENTRY *p=(DIRENTRY *)sector_buffer; // Just so it's not NULL
	unsigned int i,j;
	j=0;
	for(i=0;(j<menuindex) && p;++i)
	{
		p=NextDirEntry(i==0,configstring_matchextension);
		++j;
	}

	for(j=0;(j<7) && p;++i)
	{
		p=NextDirEntry(i==0,configstring_matchextension);
		if(p)
		{
			// FIXME declare a global long file name buffer.
			if(p->Attributes&ATTR_DIRECTORY)
			{
				menu[j].action=MENU_ACTION(&selectdir);
				menu[j].u.file.index=-1;
				romfilenames[j][0]=FONT_ARROW_RIGHT; // Right arrow
				romfilenames[j][1]=' ';
				if(longfilename[0])
					strncpy(romfilenames[j++]+2,longfilename,27);
				else
				{
					romfilenames[j][13]=0;
					strncpy(romfilenames[j++]+2,p->Name,11);
				}
			}
			else
			{
				menu[j].action=MENU_ACTION(&selectrom);
				if(longfilename[0])
					strncpy(romfilenames[j++],longfilename,29);
				else
				{
					romfilenames[j][11]=0;
					strncpy(romfilenames[j++],p->Name,11);
				}
			}
		}
	}
	return(j);
}

static void fileselector(int row)
{
	romtype=menu[row].u.file.index;
	configstring_index=menu[row].u.file.cfgidx;
	unit=menu[row].u.file.unit;
	loadimage(NULL,unit); /* Dismount existing disk when entering the menu */
	menupage=MENUPAGE_FILE;
	buildmenu(1);
}

#endif

static void submenu(int row)
{
	menupage=menu[row].u.menu.page;
	menuindex=0;
//	putchar(row+'0');
	buildmenu(1);
}

static void MenuHide(int row)
{
	Menu_ShowHide(0);
}


static void menufoot(int sub)
{
	if(sub)
	{
		menu[7].u.menu.page=MENUPAGE_ROOT;
		menu[7].action=MENU_ACTION(&submenu);
		menu[7].label=std_label_back;
	}
	else
	{
		menu[7].label=std_label_exit;
		menu[7].action=MENU_ACTION(&MenuHide);
	}
}


void cycle(int row)
{
	int v;
	struct menu_entry *m=&menu[row];
	statusword_cycle(m->u.opt.shift,m->u.opt.val,m->u.opt.limit);
	buildmenu(0);
	Menu_Draw(row);
}


__weak void toggle(int row)
{
	cycle(row);
	cycle(row);
#ifdef CONFIG_AUTOCLOSE_OSD
	Menu_ShowHide(0);
#endif
}


static void scrollmenu(int row)
{
	switch(row)
	{
		case ROW_RIGHT:
#ifdef CONFIG_SETTINGS	/* If settings are disabled, fall through to ROW_LEFT: */
			menupage=MENUPAGE_SETTINGS;
			buildmenu(1);
			return;
			break;
#endif
		case ROW_LEFT:
			submenu(7);
			return;
			break;

		case ROW_LINEUP:
			if(menuindex)
				--menuindex;
			break;
		case ROW_LINEDOWN:
			if(moremenu)
				++menuindex;
			break;
		case ROW_PAGEUP:
			menuindex-=6;
			if(menuindex<0)
				menuindex=0;
			break;
		case ROW_PAGEDOWN:
			if(moremenu)
				menuindex+=6;
			break;
	}
	buildmenu(0);
	Menu_Draw(row);
}


void dipswitches(int row)
{
	menupage=MENUPAGE_DIPSWITCHES;
	buildmenu(0);
	Menu_Draw(row);
}


void parseconf(int selpage,struct menu_entry *menu,unsigned int first,unsigned int limit)
{
	int c;
	unsigned int maxpage=0;
	unsigned int line=0;
	char *title;
	unsigned int skip=first;
	unsigned int fileindex=1;
	unsigned int configidx=1;
	moremenu=1;

	if(menupage==MENUPAGE_FILE)
	{
		line=listroms();
	}
#ifdef CONFIG_SETTINGS
	else if(menupage==MENUPAGE_SETTINGS)
	{
		strcpy(menu[line].label,"Load Settings \x81");
		menu[line].action=MENU_ACTION(&fileselector);
		menu[line].u.file.cfgidx=CONFIGSTRING_INDEX_CFG;
		menu[line].u.file.unit='S';
		++line;
		strcpy(menu[line].label,"Save Settings \x81");
		menu[line].action=MENU_ACTION(&fileselector);
		menu[line].u.file.cfgidx=CONFIGSTRING_INDEX_CFG;
		menu[line].u.file.unit='T';
		++line;
	}
#endif
	else
	{
		configstring_begin();

		configstring_nextfield(); /* Skip over core name */
		c=configstring_next();
#ifdef CONFIG_ARCFILE
		if(1)
#else
		if(c!=';')
#endif
		{
			if(!selpage) /* Add the load item only for the first menu page */
			{
				strcpy(menu[line].label,"Load *. ");
				menu[line].action=MENU_ACTION(&fileselector);
				menu[line].label[7]=c;
				menu[line].u.file.index=fileindex;
				menu[line].u.file.cfgidx=0;
				menu[line].u.file.unit=0;
#ifdef CONFIG_ARCFILE
				if(c!=';')
#endif
					configstring_copytocomma(&menu[line].label[8],LINELENGTH-8,1);
				if(line>=skip)
					++line;
				else
					--skip;
			}
			else
				configstring_nextfield();
			++fileindex; /* Need to bump the file index whichever page we're on. */
		}
		while(c && line<limit)
		{
			int diskunit=0;
			unsigned int parent=0;
			unsigned int page=0;
			c=configstring_next();

#ifdef CONFIG_ARCFILE
			if(selpage==MENUPAGE_DIPSWITCHES)
				page=selpage;
				
			/* Simplified page handling for Arcade cores */
			while(c=='P')
			{
				c=configstring_next(); /* Skip over the digit */
				c=configstring_next();
				if(c==',')	/* New page - skip over it to flatten options into the toplevel and DIP switch menus */
				{
					c=configstring_nextfield();
				}
				if(c=='P')
					++configidx; /* Keep track of which line from the config string we're reading - for pattern matching. */
			}
#else
			/* Page handling - P either declares a new page, or prefixes an option within a page */
			while(c=='P')
			{
				parent=page;
				page=configstring_getdigit();

				c=configstring_next();
				if(c==',')	/* New page, create a menu item */
				{
					title=menu[line].label;
					menu[line].u.menu.page=page;
					menu[line].action=MENU_ACTION(&submenu);
					c=configstring_next();
					while(c && c!=';')
					{
						*title++=c;
						c=configstring_next();
					}
					*title++=' ';
					*title++=FONT_ARROW_RIGHT;
					*title++=0;
					/* Are we in the menu root? */
					if(selpage==parent)
					{
						if(line>=skip)
							++line;
						else
							--skip;
					}
					c=configstring_next();
				}
				if(c=='P')
					++configidx; /* Keep track of which line from the config string we're reading - for pattern matching. */
			}
#endif

			if(page==selpage)
			{
				unsigned int low,high=0;
				unsigned int opt=0;
				unsigned int val;
				unsigned int nextline=0;

				switch(c)
				{
					case ';':
						break;
#ifdef CONFIG_ARCFILE
					case 'D': /* DIP Switches */
						strcpy(menu[line].label,"DIP Switches");
						menu[line].action=MENU_ACTION(&dipswitches);
						c=configstring_next();
						nextline=1;
						break;
#endif					
					case 'S': /* Disk image select */
						diskunit='0';
						c=configstring_next(); /* Unit no will be ASCII '0', '1', etc - or 'C' for CD images */
						if(c!=',')
							diskunit=c;
						while(c!=',')
							c=configstring_next();
						// Fall through...
					case 'F':
						if(c!=',')
							configstring_next();
						configstring_copytocomma(menu[line].label,10,0); /* Step over the filetypes */
						low=-configstring_copytocomma(menu[line].label,LINELENGTH-2,1);
						if(low>0 && low<(LINELENGTH-3))
						{
							menu[line].label[low]=' ';
							menu[line].label[low+1]=FONT_ARROW_RIGHT;
							menu[line].label[low+2]=0;
						}
						menu[line].action=MENU_ACTION(&fileselector);
						menu[line].u.file.index=fileindex;
						menu[line].u.file.cfgidx=configidx;
						menu[line].u.file.unit=diskunit;
						++fileindex;
						nextline=1;
						break;
					case 'O':
					case 'T':
						/* Parse option */
						low=configstring_getdigit();
						high=configstring_getdigit();

						if(high==',')
							high=low;
						else
							configstring_next();

						menu[line].u.opt.shift=low;
						menu[line].u.opt.val=(1<<(1+high-low))-1;
						#ifdef CONFIG_STATUSWORD_64BIT
						val=statusword_get(low,menu[line].u.opt.val);
						#else
						val=(statusword>>low)&menu[line].u.opt.val;
						#endif
						title=menu[line].label;
						if((c=configstring_copytocomma(title,LINELENGTH,selpage==page))>0)
						{
							title+=c;
							strncpy(title,": ",LINELENGTH-c);
							title+=2;
							do
							{
								++opt;
							} while(configstring_copytocomma(title,LINELENGTH-(c+2),opt==(val+1))>0);
						}

						if(opt)
						{
							menu[line].u.opt.limit=opt;
							menu[line].action=MENU_ACTION(&cycle);
						}
						else
						{
							menu[line].u.opt.limit=2;
							menu[line].action=MENU_ACTION(&toggle);
						}
						nextline=1;
						break;
					default:
						c=configstring_nextfield();
						break;
				}
				if(nextline)
				{
					if(line>=skip)
						++line;
					else
						--skip;
				}				
			}
			else
			{
				if(c=='F')
					++fileindex;
				c=configstring_nextfield();
			}
			++configidx; /* Keep track of which line from the config string we're reading - for pattern matching. */
		}
		configstring_end();
	}
	for(;line<7;++line)
	{
		moremenu=0;
		*menu[line].label=0;
		menu[line].action=0;
	}
}

void buildmenu(int set)
{
	if(set)
		menuindex=0;
	parseconf(menupage,menu,menuindex,7);
	menufoot(menupage);
	if(set)
		Menu_Set(menu);
}

/* Allow the Boot ROM filename to set in config.h instead of requiring an override.
   The override method will still work, however. */
#ifndef ROM_FILENAME
#define ROM_FILENAME "BOOT    ROM"
#endif
__weak const char *bootrom_name=ROM_FILENAME;


#ifdef CONFIG_WITHOUT_FILESYSTEM
__weak char *autoboot()
{
	return(0);
}
#else
__weak char *autoboot()
{
	char *result=0;
#ifdef CONFIG_SETTINGS
	loadsettings(CONFIG_SETTINGS_FILENAME);
#endif
	romtype=0;
#ifdef ROM_REQUIRED
	if(!LoadROM(bootrom_name))
		result="ROM loading failed";
#else
	LoadROM(bootrom_name)
#endif
	return(result);
}
#endif


__weak char *get_rtc()
{
	/* Upload current time via RTC. Use the sector buffer as temp storage. */
	register volatile int *spiptr=&HW_SPI(HW_SPI_DATA);
	char *ptr=sector_buffer;
	if(HAVE_RTC)
	{
		int t;
		EnableRTC();
		*spiptr=0x92;	/* Read, Subaddress 001, start reading at register 0000 */
		*spiptr=0xff; *ptr++=*spiptr; /* Seconds */
		*spiptr=0xff; *ptr++=*spiptr; /* Minutes */
		*spiptr=0xff; *ptr++=*spiptr; /* Hours */
		*spiptr=0xff; *ptr++=*spiptr; /* Day */
		*spiptr=0xff; t=*spiptr; /* Weekday */
		*spiptr=0xff; *ptr++=*spiptr; /* Month */
		*spiptr=0xff; *ptr++=*spiptr; /* Year */
		*ptr++=t;
		DisableRTC();
	}
	else
	{
		*ptr++=0;
		*ptr++=0;
		*ptr++=0;
		*ptr++=0x19;
		*ptr++=0x01;
		*ptr++=0x22;
		*ptr++=0x03;
	}
	*ptr++=0;
	return(sector_buffer);
}


__weak void init()
{
	int havesd;
	int i,c;
	char *err;

	PS2Init();

	buildmenu(1);

#ifdef CONFIG_WITHOUT_FILESYSTEM
	havesd=0;
#else
	if(!(havesd=sd_init() && FindDrive()))
	{
		Menu_Message("SD failed.",0);
		while(1)
			;
	}
#ifdef CONFIG_AUTOSCANDOUBLER
	AutoScandoubler();
#endif

	Menu_Message("Booting...",0);
	Menu_Message(autoboot(),0);

	EnableInterrupts();
}

#endif

__weak void mainloop()
{
#ifdef CONFIG_RTC
	int framecounter;
#endif
	while(1)
	{
#ifdef PS2_MOUSE
		HandlePS2Mouse(0);
#endif

#ifdef CONFIG_CD
		pcecd_poll();
#endif
		Menu_Run();

#ifdef CONFIG_CD
		pcecd_poll();
#endif

#ifdef CONFIG_DISKIMG
		diskimg_poll();
#endif

#ifdef CONFIG_RTC
		if((framecounter++&8191)==0)
			user_io_send_rtc(get_rtc());
#endif
	}
}

__weak int main(int argc,char **argv)
{
	menuindex=0;
	menupage=0;
	init();
	mainloop();
	return(0);
}

