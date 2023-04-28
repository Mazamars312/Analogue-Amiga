/*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski

This file is part of Minimig

Minimig is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Minimig is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

This is the Minimig OSD (on-screen-display) handler.

2012-02-09 - Split character rom out to separate header file, with upper 128 entries
as rotated copies of the first 128 entries.  -- AMR

29-12-2006 - created
30-12-2006 - improved and simplified
-- JB --
2008-10-04 - ARM version
2008-10-26 - added cpu and floppy configuration functions
2008-12-31 - added enable HDD command
2009-02-03 - full keyboard support
2009-06-23 - hires OSD display
2009-08-23 - adapted ConfigIDE() - support for 2 hardfiles
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "osd.h"
#include "riscprintf.h"

#include "spi.h"

#include "charrom.h"


#define OSDLINELEN       256       // single line length in bytes
#define OSD_CMD_WRITE    0x20      // OSD write video data command
#define OSD_CMD_ENABLE   0x41      // OSD enable command
#define OSD_CMD_DISABLE  0x40      // OSD disable command

static int osd_size = 1;
static int arrow;
char framebuffer[256];
static int  osdbufpos = 0;
static int  osdset = 0;

void OsdSetSize(int n)
{
	osd_size = n;
}

int OsdGetSize()
{
	return osd_size;
}

static void rotatechar(unsigned char *in, unsigned char *out)
{
	int a;
	int b;
	int c;
	for (b = 0; b<8; ++b)
	{
		a = 0;
		for (c = 0; c<8; ++c)
		{
			a <<= 1;
			a |= (in[c] >> b) & 1;
		}
		out[b] = a;
	}
}

static void osd_start(int line)
{
	line = line & 0x1F;
	osdset |= 1 << line;
	osdbufpos = line * 256;
}

void OsdSetArrow(int a)
{
	arrow = a;
}
static uint8_t osdbuf[256];
static unsigned char titlebuffer[256];

void OsdWrite(const char *s, unsigned char invert, unsigned char stipple, char usebg, int maxinv, int mininv)
{
	OsdWriteOffset(0, s, invert, stipple, 0, 0, usebg, maxinv, mininv);
}

static void draw_title(const unsigned char *p)
{
	// left white border
	osdbuf[osdbufpos++] = 0xff;
	osdbuf[osdbufpos++] = 0xff;
	osdbuf[osdbufpos++] = 0xff;

	for (int i = 0; i < 8; i++)
	{
		osdbuf[osdbufpos++] = 255 ^ *p;
		osdbuf[osdbufpos++] = 255 ^ *p++;
	}

	// right white border
	osdbuf[osdbufpos++] = 0xff;

	// blue gap
	osdbuf[osdbufpos++] = 0;
	osdbuf[osdbufpos++] = 0;
}

// write a null-terminated string <s> to the OSD buffer starting at line <n>
void OsdWriteOffset(unsigned char n, const char *s, unsigned char invert, unsigned char stipple, char offset, char leftchar, char usebg, int maxinv, int mininv)
{
	//printf("OsdWriteOffset(%d)\n", n);
	unsigned short i;
	unsigned char b;
	const unsigned char *p;
	unsigned char stipplemask = 0xff;
	int linelimit = OSDLINELEN;
	int arrowmask = arrow;
	// if (n == (osd_size-1) && (arrow & OSD_ARROW_RIGHT))
	// 	linelimit -= 22;

	if (n && n < OsdGetSize() - 1) leftchar = 0;

	if (stipple) {
		stipplemask = 0x55;
		stipple = 0xff;
	}
	else
		stipple = 0;

	osd_start(n);

	unsigned char xormask = 0;
	unsigned char xorchar = 0;

	i = 0;
	// send all characters in string to OSD
	while (1)
	{
		if (invert && i / 8 >= mininv) xormask = 255;
		if (invert && i / 8 >= maxinv) xormask = 0;

		// if (i == 0 && (n < osd_size))
		// {	// Render sidestripe
		// 	unsigned char tmp[8];
		//
		// 	if (leftchar)
		// 	{
		// 		unsigned char tmp2[8];
		// 		memcpy(tmp2, charfont[(uint)leftchar], 8);
		// 		rotatechar(tmp2, tmp);
		// 		p = tmp;
		// 	}
		// 	else
		// 	{
		// 		p = &titlebuffer[(osd_size - 1 - n) * 8];
		// 	}
		//
		// 	draw_title(p);
		// 	i += 22;
		// }
		// else
		if (n == (osd_size-1) && (arrowmask & OSD_ARROW_LEFT))
		{	// Draw initial arrow
			unsigned char b;

			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;
			p = &charfont[0x10][0];
			for (b = 0; b<8; b++) osdbuf[osdbufpos++] = (*p++ << offset) ^ xormask;
			p = &charfont[0x14][0];
			for (b = 0; b<8; b++) osdbuf[osdbufpos++] = (*p++ << offset) ^ xormask;
			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;
			osdbuf[osdbufpos++] = xormask;

			i += 24;
			arrowmask &= ~OSD_ARROW_LEFT;
			if (*s++ == 0) break;	// Skip 3 characters, to keep alignent the same.
			if (*s++ == 0) break;
			if (*s++ == 0) break;
		}
		else
		{
			b = *s++;
			if (!b) break;

			if (b == 0xb)
			{
				stipplemask ^= 0xAA;
				stipple ^= 0xff;
			}
			else if (b == 0xc)
			{
				xorchar ^= 0xff;
			}
			else if (b == 0x0d || b == 0x0a)
			{  // cariage return / linefeed, go to next line
			   // increment line counter
				if (++n >= linelimit)
					n = 0;

				// send new line number to OSD
				osd_start(n);
			}
			else if (i<(linelimit - 8))
			{  // normal character
				unsigned char c;
				p = &charfont[b][0];
				for (c = 0; c<8; c++) {
					char bg = usebg ? framebuffer[i+c-22] : 0;
					osdbuf[osdbufpos++] = (((*p++ << offset)&stipplemask) ^ xormask ^ xorchar) | bg;
					stipplemask ^= stipple;
				}
				i += 8;
			}
		}
	}

	for (; i < linelimit; i++) // clear end of line
	{
		char bg = usebg ? framebuffer[i-22] : 0;
		osdbuf[osdbufpos++] = xormask | bg;
	}

	if (n == (osd_size-1) && (arrowmask & OSD_ARROW_RIGHT))
	{	// Draw final arrow if needed
		unsigned char c;
		osdbuf[osdbufpos++] = xormask;
		osdbuf[osdbufpos++] = xormask;
		osdbuf[osdbufpos++] = xormask;
		p = &charfont[0x15][0];
		for (c = 0; c<8; c++) osdbuf[osdbufpos++] = (*p++ << offset) ^ xormask;
		p = &charfont[0x11][0];
		for (c = 0; c<8; c++) osdbuf[osdbufpos++] = (*p++ << offset) ^ xormask;
		osdbuf[osdbufpos++] = xormask;
		osdbuf[osdbufpos++] = xormask;
		osdbuf[osdbufpos++] = xormask;
		i += 22;
	}
}


// clear OSD frame buffer
void OsdClear(void)
{
	for (int i = 0; i < OSDLINELEN; i++)
	{
	osdbuf[i] = 0;
  	asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	}
}

// enable displaying of OSD
void OsdEnable(unsigned char mode)
{
	// user_io_osd_key_enable(mode & DISABLE_KEYBOARD);
	mode &= (DISABLE_KEYBOARD | OSD_MSG);
	HPS_spi_osd_cmd(OSD_CMD_ENABLE | mode);
}

void InfoEnable(int x, int y, int width, int height, int background_color, int text_color)
{
	// user_io_osd_key_enable(0);
	HPS_spi_osd_cmd_cont(OSD_CMD_ENABLE | OSD_INFO);
	spi_w(x);
	spi_w(y);
	spi_w(width);
	spi_w(height);
	spi_w(background_color);
	spi_w(text_color);
	spi_w(text_color);
	spi_w(text_color);
	HPS_DisableOsd();
}


// disable displaying of OSD
void OsdDisable()
{
	// user_io_osd_key_enable(0);
	HPS_spi_osd_cmd(OSD_CMD_DISABLE);
}


void OsdUpdate(int line)
{
	// mainprintf("testering\r\n");
		HPS_spi_osd_cmd_cont(OSD_CMD_WRITE | line);
	  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
		OSD_HPS_spi_write(osdbuf, 256, 0);
	  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
	  asm volatile("nop"); // needed as loops do not work in G++ RISCV compilers
		HPS_DisableOsd();
}
