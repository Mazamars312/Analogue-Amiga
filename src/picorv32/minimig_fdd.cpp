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
*/

// 2009-11-14   - adapted gap size
// 2009-12-24   - updated sync word list
//              - fixed sector header generation
// 2010-01-09   - support for variable number of tracks

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "minimig_fdd.h"
#include "spi.h"
#include "printf.h"
#include "apf.h"
#include "timer.h"

unsigned char drives = 0; // number of active drives reported by FPGA (may change only during reset)
adfTYPE *pdfx;            // drive select pointer
adfTYPE df[4] = {};    // drive information structure

static uint8_t sector_buffer[512];

unsigned char Error;

#define TRACK_SIZE 12668
#define HEADER_SIZE 0x40
#define DATA_SIZE 0x400
#define SECTOR_SIZE (HEADER_SIZE + DATA_SIZE)
#define SECTOR_COUNT 11
#define LAST_SECTOR (SECTOR_COUNT - 1)
#define GAP_SIZE (TRACK_SIZE - SECTOR_COUNT * SECTOR_SIZE)
#define RAMENDEN(x) *(volatile unsigned int *)(0xFFFFFFF0+(x))

#define B2W(a,b) (((((uint16_t)(a))<<8) & 0xFF00) | ((uint16_t)(b) & 0x00FF))

void SendSector(unsigned char *pData, unsigned char sector, unsigned char track, unsigned char dsksynch, unsigned char dsksyncl)
{
	unsigned char checksum[4];
	unsigned short i;
	unsigned char x,y;
	unsigned char *p;

	// preamble
	spi_w(0xAAAA);
	spi_w(0xAAAA);

	// synchronization
	spi_w(B2W(dsksynch, dsksyncl));
	spi_w(B2W(dsksynch, dsksyncl));

	// odd bits of header
	x = 0x55;
	checksum[0] = x;
	y = (track >> 1) & 0x55;
	checksum[1] = y;
	spi_w(B2W(x,y));

	x = (sector >> 1) & 0x55;
	checksum[2] = x;
	y = ((11 - sector) >> 1) & 0x55;
	checksum[3] = y;
	spi_w(B2W(x, y));

	// even bits of header
	x = 0x55;
	checksum[0] ^= x;
	y = track & 0x55;
	checksum[1] ^= y;
	spi_w(B2W(x, y));

	x = sector & 0x55;
	checksum[2] ^= x;
	y = (11 - sector) & 0x55;
	checksum[3] ^= y;
	spi_w(B2W(x, y));

	// sector label and reserved area (changes nothing to checksum)
	i = 0x10;
	while (i--) spi_w(0xAAAA);

	// send header checksum
	spi_w(0xAAAA);
	spi_w(0xAAAA);
	spi_w(B2W(checksum[0] | 0xAA, checksum[1] | 0xAA));
	spi_w(B2W(checksum[2] | 0xAA, checksum[3] | 0xAA));

	// calculate data checksum
	checksum[0] = 0;
	checksum[1] = 0;
	checksum[2] = 0;
	checksum[3] = 0;
	p = pData;
	i = DATA_SIZE / 2 / 4;
	while (i--)
	{

		x = *p++;
		checksum[0] ^= x ^ x >> 1;
		x = *p++;
		checksum[1] ^= x ^ x >> 1;
		x = *p++;
		checksum[2] ^= x ^ x >> 1;
		x = *p++;
		checksum[3] ^= x ^ x >> 1;
		;
	}

	// send data checksum
	spi_w(0xAAAA);
	spi_w(0xAAAA);
	spi_w(B2W(checksum[0] | 0xAA, checksum[1] | 0xAA));
	spi_w(B2W(checksum[2] | 0xAA, checksum[3] | 0xAA));

	// odd bits of data field
	i = DATA_SIZE / 4;
	p = pData;
	while (i--)
	{
		x = (*p++ >> 1) | 0xAA;
		y = (*p++ >> 1) | 0xAA;
		spi_w(B2W(x, y));
	}

	// even bits of data field
	i = DATA_SIZE / 4;
	p = pData;
	while (i--)
	{
		x = *p++ | 0xAA;
		y = *p++ | 0xAA;
		spi_w(B2W(x, y));
	}
}

void SendGap(void)
{
	unsigned short i = GAP_SIZE/2;
	while (i--) spi_w(0xAAAA);
}

// read a track from disk
void ReadTrack(adfTYPE *drive)
{
	// track number is updated in drive struct before calling this function
  uint32_t foo;
	unsigned char sector;
	unsigned char status;
	unsigned char track;
	unsigned short dsksync;
	uint16_t tmp;
	if (drive->track >= drive->tracks)
	{
		printf("Illegal track read: %d\n", drive->track);
		drive->track = drive->tracks - 1;
	}

	unsigned long lba;

	if (drive->track != drive->track_prev)
	{ // track step or track 0, start at beginning of track
		drive->track_prev = drive->track;
		sector = 0;
		drive->sector_offset = sector;
		lba = drive->track * SECTOR_COUNT;
	}
	else
	{ // same track, start at next sector in track
		sector = drive->sector_offset;
		lba = (drive->track * SECTOR_COUNT) + sector;
	}
  //uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length
	foo = (uint32_t) &sector_buffer;
	if (dataslot_read_lba_set(drive->dataslot, foo, lba))
	{
		return;
	}
	mister_EnableFpga();
	tmp = spi_w(0);
	status = (uint8_t)(tmp>>8); // read request signal
	track = (uint8_t)tmp; // track number (cylinder & head)
	dsksync = spi_w(0); // disk sync
	spi_w(0); // mfm words to transfer
	mister_DisableFpga();

	if (track >= drive->tracks)
		track = drive->tracks - 1;

	while (1)
	{
		foo = (uint32_t) &sector_buffer;
		// RAMENDEN(0) = 1;
		dataslot_read_lba_read(512);
		// RAMENDEN(0) = 0;
		// FileReadSec(&drive->file, sector_buffer);

		mister_EnableFpga();

		// check if FPGA is still asking for data
		tmp = spi_w(0);
		status = (uint8_t)(tmp >> 8); // read request signal
		track = (uint8_t)tmp; // track number (cylinder & head)
		dsksync = spi_w(0); // disk sync
		spi_w(0); // mfm words to transfer

		if (track >= drive->tracks)
			track = drive->tracks - 1;

		// workaround for Copy Lock in Wiz'n'Liz and North&South (might brake other games)
		if (dsksync == 0x0000 || dsksync == 0x8914 || dsksync == 0xA144)
			dsksync = 0x4489;

		// North&South: $A144
		// Wiz'n'Liz (Copy Lock): $8914
		// Prince of Persia: $4891
		// Commando: $A245

		// some loaders stop dma if sector header isn't what they expect
		// because we don't check dma transfer count after sending a word
		// the track can be changed while we are sending the rest of the previous sector
		// in this case let's start transfer from the beginning
		if (track == drive->track)
		{
			// send sector if fpga is still asking for data
			if (status & CMD_RDTRK)
			{
				//GenerateHeader(sector_header, sector_buffer, sector, track, dsksync);
				//SendSector(sector_header, sector_buffer);
				SendSector(sector_buffer, sector, track, (unsigned char)(dsksync >> 8), (unsigned char)dsksync);

				if (sector == LAST_SECTOR)
					SendGap();
			}
		}

		// we are done accessing FPGA
		mister_DisableFpga();

		// track has changed
		if (track != drive->track)
			break;

		// read dma request
		if (!(status & CMD_RDTRK))
			break;

		sector++;
		if (sector >= SECTOR_COUNT)
		{
			// go to the start of current track
			sector = 0;
			lba = drive->track * SECTOR_COUNT;
			foo = (uint32_t) &sector_buffer;
			if (dataslot_read_lba_set(drive->dataslot, foo, lba))
			{
				return;
			}
		}

		// remember current sector
		drive->sector_offset = sector;
	}
}

void UpdateDriveStatus(void)
{
	uint16_t i = 0x1000;
	mister_EnableFpga();
	spi_w(i);
	mister_DisableFpga();
	usleep(300);
	i = 0x1000 | df[0].status | (df[1].status << 1) | (df[2].status << 2) | (df[3].status << 3);
	mister_EnableFpga();
	spi_w(i);
	mister_DisableFpga();

}

void HandleFDD(unsigned char c1, unsigned char c2)
{

	unsigned char sel;
	drives = (c1 >> 4) & 0x03; // number of active floppy drives

	if (c1 & CMD_RDTRK)
	{
		sel = (c1 >> 6) & 0x03;
		df[sel].track = c2;
		printf("selected drive %d\r\n", sel);
		ReadTrack(&df[sel]);
	}
	// else if (c1 & CMD_WRTRK)
	// {
	// 	sel = (c1 >> 6) & 0x03;
	// 	df[sel].track = c2;
	// 	WriteTrack(&df[sel]);
	// }
}

// insert floppy image pointed to to by global <file> into <drive>
// We will change this for the inputerup so this gets updated by the APF interface
void InsertFloppy(adfTYPE *drive, uint32_t fsize, uint32_t drive_dataslot)
{
	int writable = 1;

	unsigned long tracks;
	drive->dataslot = drive_dataslot;
  drive->size = (uint32_t)fsize;
	// calculate number of tracks in the ADF image file
	tracks = drive->size / (512 * 11);
	if (tracks > MAX_TRACKS)
	{
		printf("UNSUPPORTED ADF SIZE!!! Too many tracks: %lu\n", tracks);
		tracks = MAX_TRACKS;
	}
	drive->tracks = (unsigned char)tracks;
	// initialize the rest of drive struct
	drive->status = DSK_INSERTED;
	if (writable) // read-only attribute
	drive->status |= DSK_WRITABLE;
	drive->sector_offset = 0;
	drive->track = 0;
	drive->track_prev = -1;
	printf("file writable: %d\r\n", writable);
	printf("file size: %lu (%lu KB)\r\n", drive->size, drive->size >> 10);
	printf("drive tracks: %u\r\n", drive->tracks);
	printf("drive track_prev: %u\r\n", drive->track_prev);
	printf("drive track_prev: %u\r\n", drive->track_prev);
	printf("drive status: 0x%02X\r\n", drive->status);
	return;
}
