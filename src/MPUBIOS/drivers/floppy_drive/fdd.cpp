/*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski
Copyright 2022 Murray Aickin

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
// 2022-11-25   - Added Pocket APF access


#include "fdd.h"

unsigned char drives = 0; // number of active drives reported by FPGA (may change only during reset)
adfTYPE df[4] = {};    // drive information structure
uint8_t sector_bufferfdd1[5632];
// uint8_t *sector_buffer1 __attribute__((section("SRAM")));
uint8_t sector_bufferfdd0[512];
unsigned char Error;

#define TRACK_SIZE 12668
#define HEADER_SIZE 0x40
#define DATA_SIZE 0x400
#define SECTOR_SIZE (HEADER_SIZE + DATA_SIZE)
#define SECTOR_COUNT 11
#define LAST_SECTOR (SECTOR_COUNT - 1)
#define GAP_SIZE (TRACK_SIZE - SECTOR_COUNT * SECTOR_SIZE)

#define B2W(a,b) (((((uint16_t)(a))<<8) & 0xFF00) | ((uint16_t)(b) & 0x00FF))

unsigned char GetData(void)
{
	unsigned char c, c1, c2, c3, c4;
	unsigned char i;
	unsigned char *p;
	unsigned short n;
	unsigned char checksum[4];
	uint16_t tmp;

	Error = 0;
	while (1)
	{
		HPS_EnableFpga();
		c1 = (uint8_t)(spi_w(0) >> 8); // write request signal, track number (cylinder & head)
		if (!(c1 & CMD_WRTRK))
			break;
		spi_w(0);
		tmp = spi_w(0); // mfm words to transfer

		n = tmp & 0x3FFF;

		if (n >= 0x204)
		{
			tmp = (spi_w(0) & 0x5555) << 1;
			c1 = (uint8_t)(tmp >> 8);
			c2 = (uint8_t)tmp & 0x55;
			tmp = (spi_w(0) & 0x5555) << 1;
			c3 = (uint8_t)(tmp >> 8);
			c4 = (uint8_t)tmp;

			tmp = spi_w(0) & 0x5555;
			c1 |= (uint8_t)(tmp >> 8);
			c2 |= (uint8_t)tmp;
			tmp = spi_w(0) & 0x5555;
			c3 |= (uint8_t)(tmp >> 8);
			c4 |= (uint8_t)tmp;

			checksum[0] = 0;
			checksum[1] = 0;
			checksum[2] = 0;
			checksum[3] = 0;

			// odd bits of data field
			i = 128;
			p = sector_bufferfdd0;
			do
			{
				tmp = spi_w(0);
				c = (uint8_t)(tmp >> 8);
				checksum[0] ^= c;
				*p++ = (c & 0x55) << 1;
				c = (uint8_t)tmp;
				checksum[1] ^= c;
				*p++ = (c & 0x55) << 1;
				tmp = spi_w(0);
				c = (uint8_t)(tmp >> 8);
				checksum[2] ^= c;
				*p++ = (c & 0x55) << 1;
				c = (uint8_t)tmp;
				checksum[3] ^= c;
				*p++ = (c & 0x55) << 1;
			} while (--i);

			// even bits of data field
			i = 128;
			p = sector_bufferfdd0;
			do
			{
				tmp = spi_w(0);
				c = (uint8_t)(tmp >> 8);
				checksum[0] ^= c;
				*p++ |= c & 0x55;
				c = (uint8_t)tmp;
				checksum[1] ^= c;
				*p++ |= c & 0x55;
				tmp = spi_w(0);
				c = (uint8_t)(tmp >> 8);
				checksum[2] ^= c;
				*p++ |= c & 0x55;
				c = (uint8_t)tmp;
				checksum[3] ^= c;
				*p++ |= c & 0x55;
			} while (--i);

			checksum[0] &= 0x55;
			checksum[1] &= 0x55;
			checksum[2] &= 0x55;
			checksum[3] &= 0x55;

			if (c1 != checksum[0] || c2 != checksum[1] || c3 != checksum[2] || c4 != checksum[3])
			{
				Error = 29;
				break;
			}

			HPS_DisableFpga();
			return 1;
		}
		else if ((tmp & 0x8000) == 0) // not enough data in fifo and write dma is not active
		{
			Error = 28;
			break;
		}

		HPS_DisableFpga();
	}
	HPS_DisableFpga();
	return 0;
}



unsigned char FindSync(adfTYPE *drive)
// reads data from fifo till it finds sync word or fifo is empty and dma inactive (so no more data is expected)
{
	unsigned char  c1, c2;
	unsigned short n;
	uint16_t tmp;

	while (1)
	{
		HPS_EnableFpga();
		tmp = spi_w(0);
		c1 = (uint8_t)(tmp >> 8); // write request signal
		c2 = (uint8_t)tmp; // track number (cylinder & head)
		if (!(c1 & CMD_WRTRK))
			break;
		if (c2 != drive->track)
			break;
		spi_w(0); //disk sync word

		n = spi_w(0) & 0xBFFF; // mfm words to transfer

		if (n == 0)
			break;

		n &= 0x3FFF;

		while (n--)
		{
			if (spi_w(0) == 0x4489)
			{
				HPS_DisableFpga();
				return 1;
			}
		}
		HPS_DisableFpga();
	}
	HPS_DisableFpga();
	return 0;
}

unsigned char GetHeader(unsigned char *pTrack, unsigned char *pSector)
// this function reads data from fifo till it finds sync word or dma is inactive
{
	unsigned char c, c1, c2, c3, c4;
	unsigned char i;
	unsigned char checksum[4];
	uint16_t tmp;

	Error = 0;
	while (1)
	{
		HPS_EnableFpga();
		c1 = (uint8_t)(spi_w(0)>>8); // write request signal, track number (cylinder & head)
		if (!(c1 & CMD_WRTRK))
			break;
		spi_w(0); //disk sync word
		tmp = spi_w(0); // mfm words to transfer

		if ((tmp & 0x3F00) != 0 || (tmp & 0xFF) > 24)// remaining header data is 25 mfm words
		{
			tmp = spi_w(0); // second sync
			if (tmp != 0x4489)
			{
				Error = 21;
				mainprintf("\nSecond sync word missing...\n");
				break;
			}

			tmp = spi_w(0);
			c = (uint8_t)(tmp >> 8);
			checksum[0] = c;
			c1 = (c & 0x55) << 1;
			c = (uint8_t)tmp;
			checksum[1] = c;
			c2 = (c & 0x55) << 1;

			tmp = spi_w(0);
			c = (uint8_t)(tmp >> 8);
			checksum[2] = c;
			c3 = (c & 0x55) << 1;
			c = (uint8_t)tmp;
			checksum[3] = c;
			c4 = (c & 0x55) << 1;

			tmp = spi_w(0);
			c = (uint8_t)(tmp >> 8);
			checksum[0] ^= c;
			c1 |= c & 0x55;
			c = (uint8_t)tmp;
			checksum[1] ^= c;
			c2 |= c & 0x55;

			tmp = spi_w(0);
			c = (uint8_t)(tmp >> 8);
			checksum[2] ^= c;
			c3 |= c & 0x55;
			c = (uint8_t)tmp;
			checksum[3] ^= c;
			c4 |= c & 0x55;

			if (c1 != 0xFF) // always 0xFF
				Error = 22;
			else if (c2 > 159) // Track number (0-159)
				Error = 23;
			else if (c3 > 10) // Sector number (0-10)
				Error = 24;
			else if (c4 > 11 || c4 == 0) // Number of sectors to gap (1-11)
				Error = 25;

			if (Error)
			{
				mainprintf("\nWrong header: %u.%u.%u.%u\n", c1, c2, c3, c4);
				break;
			}

			*pTrack = c2;
			*pSector = c3;

			for (i = 0; i < 8; i++)
			{
				tmp = spi_w(0);
				checksum[0] ^= (uint8_t)(tmp >> 8);
				checksum[1] ^= (uint8_t)tmp;
				tmp = spi_w(0);
				checksum[2] ^= (uint8_t)(tmp >> 8);
				checksum[3] ^= (uint8_t)tmp;
			}

			checksum[0] &= 0x55;
			checksum[1] &= 0x55;
			checksum[2] &= 0x55;
			checksum[3] &= 0x55;

			tmp = (spi_w(0) & 0x5555) << 1;
			c1 = (uint8_t)(tmp >> 8);
			c2 = (uint8_t)tmp;
			tmp = (spi_w(0) & 0x5555) << 1;
			c3 = (uint8_t)(tmp >> 8);
			c4 = (uint8_t)tmp;

			tmp = spi_w(0) & 0x5555;
			c1 |= (uint8_t)(tmp >> 8);
			c2 |= (uint8_t)tmp;
			tmp = spi_w(0) & 0x5555;
			c3 |= (uint8_t)(tmp >> 8);
			c4 |= (uint8_t)tmp;

			if (c1 != checksum[0] || c2 != checksum[1] || c3 != checksum[2] || c4 != checksum[3])
			{
				Error = 26;
				break;
			}

			HPS_DisableFpga();
			return 1;
		}
		else if ((tmp & 0x8000) == 0) // not enough data for header and write dma is not active
		{
			Error = 20;
			break;
		}

		HPS_DisableFpga();
	}

	HPS_DisableFpga();
	return 0;
}

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

void WriteTrack(adfTYPE *drive)
{
	unsigned char Track;
	unsigned char Sector;
  uint32_t foo0;
  uint32_t foo1;
	unsigned long lba = drive->track * SECTOR_COUNT;

	//    drive->track_prev = drive->track + 1; // This causes a read that directly follows a write to the previous track to return bad data.
	drive->track_prev = -1; // just to force next read from the start of current track

	while (FindSync(drive))
	{
		if (GetHeader(&Track, &Sector))
		{
			if (Track == drive->track)
			{
				foo0 = (uint32_t) &sector_bufferfdd0;
				while (!dataslot_ready()){
	      };
				dataslot_write_lba_set(drive->dataslot, foo0 | APF_ADDRESS_OFFSET, lba+Sector);

				if (GetData())
				{
					if (drive->status & DSK_WRITABLE)
					{

						dataslot_write_lba(512, 0);
					}
					else
					{
						Error = 30;
						mainprintf("Write attempt to protected disk!\n");
					}
				}
			}
			else
				Error = 27; //track number reported in sector header is not the same as current drive track
		}
		if (Error)
		{
			mainprintf("WriteTrack: error %u\n", Error);
			mainprintf("Write error");
		}
	}
}



// read a track from disk
void ReadTrack(adfTYPE *drive)
{
	// track number is updated in drive struct before calling this function
  // uint32_t foo0;


	uint32_t foo1;
	unsigned char sector;
	unsigned char status;
	unsigned char track;
	unsigned short dsksync;
	uint16_t tmp;
	adfTYPE *df_cache;
	foo1 = (uint32_t) &sector_bufferfdd1;

	if (drive->track >= drive->tracks)
	{
		mainprintf("Illegal track read: %d\n", drive->track);
		drive->track = drive->tracks - 1;
	}
  // sector_buffer1 = (uint8_t*)malloc(5632);
	unsigned long lba;
	mainprintf("dataslots %d %d\r\n", df_cache->dataslot, drive->dataslot);
	mainprintf("tracks %d %d\r\n", df_cache->track, drive->track);
	mainprintf("status %d %d\r\n", df_cache->status, drive->status);
	if ((drive->track 			== drive->track_prev) &
		  (df_cache->dataslot 	== drive->dataslot) &
			(df_cache->track 		== drive->track) &
			(df_cache->status 		== drive->status))
			{ // same track, start at next sector in track
				sector = drive->sector_offset;
				lba = (drive->track * SECTOR_COUNT);
				mainprintf("we using the cache\r\n");
			} else

			{ // track step or track 0, start at beginning of track and this to refull the cache system
				drive->track_prev 			= drive->track;
				sector 									= 0;
				drive->sector_offset 		= sector;
				lba 										= drive->track * SECTOR_COUNT;
				df_cache->dataslot 			= drive->dataslot;
				df_cache->size 					= drive->size; // Size of the image
				df_cache->status 				= drive->status; /*status of floppy*/
				df_cache->tracks 				= drive->tracks; /*number of tracks*/
				df_cache->sector_offset 	= drive->sector_offset; /*sector offset to handle tricky loaders*/
				df_cache->track 					= drive->track; /*current track*/
				df_cache->track_prev 		= drive->track_prev; /*previous track*/
				mainprintf("we are not using the cache\r\n");
				while (!dataslot_ready()){
	      };
				if (dataslot_read(drive->dataslot, foo1 | APF_ADDRESS_OFFSET, lba << 9, 5632))
				{
					return;
				}
			}




	HPS_EnableFpga();
	tmp = spi_w(0);
	status = (uint8_t)(tmp>>8); // read request signal
	track = (uint8_t)tmp; // track number (cylinder & head)
	dsksync = spi_w(0); // disk sync
	spi_w(0); // mfm words to transfer
	HPS_DisableFpga();

	if (track >= drive->tracks)
		track = drive->tracks - 1;
	// int tmp_count = 0;
	while (1)
	{
		tmp = 0;
		mainprintf("sector %d\r\n", sector);
		while (tmp <= 511){
			sector_bufferfdd0[tmp] = sector_bufferfdd1[((sector<<9)|tmp)];
  		// mainprintf("\033[36;1;1mDATA  %0.4x  Pointer %0.2x\033[0m\r\n", sector_bufferfdd1[((sector<<9)|tmp)], tmp);
			tmp++;
		}
		mainprintf("data sector %d\r\n", ((sector<<9)));
		HPS_EnableFpga();

		// check if FPGA is still asking for data
		tmp = spi_w(0);
		status = (uint8_t)(tmp >> 8); // read request signal
		track = (uint8_t)tmp; // track number (cylinder & head)
		dsksync = spi_w(0); // disk sync
		spi_w(0); // mfm words to transfer

		if (track >= drive->tracks) {
			track = drive->tracks - 1;
		}

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

				SendSector(sector_bufferfdd0, sector, track, (unsigned char)(dsksync >> 8), (unsigned char)dsksync);

				if (sector == LAST_SECTOR)
					SendGap();
			}
		}

		// we are done accessing FPGA
		HPS_DisableFpga();

		// track has changed
		if (track != drive->track){
			// mainprintf("track has changed\r\n");
			break;
		}

		// read dma request
		if (!(status & CMD_RDTRK)){
			// mainprintf("CMD_RDTRK \r\n");
			break;
		}


		sector++;
		if (sector >= SECTOR_COUNT)
		{
			// go to the start of current track
			sector = 0;
		}

		// remember current sector
		drive->sector_offset = sector;
		riscusleep(1000);
	}
}

void UpdateDriveStatus_fdd()
{
	adfTYPE *df_cache;
	HPS_EnableFpga();
	spi_w(0x1000 | df[0].status | (df[1].status << 1) | (df[2].status << 2) | (df[3].status << 3));
	mainprintf("States disk 0: %0.4x\r\n", (df[0].status));
	mainprintf("States disk 1: %0.4x\r\n", (df[1].status));
	mainprintf("States disk 2: %0.4x\r\n", (df[2].status));
	mainprintf("States disk 3: %0.4x\r\n", (df[3].status));
	mainprintf("States of disks: %0.4x\r\n", (0x1000 | df[0].status | (df[1].status << 1) | (df[2].status << 2) | (df[3].status << 3)));
	HPS_DisableFpga();
	df_cache->dataslot 			= 0;
	df_cache->size 					= 0; // Size of the image
	df_cache->status 				= 0; /*status of floppy*/
	df_cache->tracks 				= 0; /*number of tracks*/
	df_cache->sector_offset 	= 0; /*sector offset to handle tricky loaders*/
	df_cache->track 					= 0; /*current track*/
	df_cache->track_prev 		= 0; /*previous track*/
}

void UnsertFloppy(adfTYPE *drive)
{
	mainprintf("unload\r\n" );
	drive->dataslot = 0;
  drive->size = 0;
	drive->tracks = 0;
	drive->status = 0;
	drive->sector_offset = 0;
	drive->track = 0;
	drive->track_prev = 0;
	UpdateDriveStatus_fdd();
	return;
}

void RemoveDriveStatus()
{
	HPS_EnableFpga();
	spi_w(0x1000);
	HPS_DisableFpga();
}

void HandleFDD(unsigned char c1, unsigned char c2)
{

	unsigned char sel;
	drives = (c1 >> 4) & 0x03; // number of active floppy drives

	if (c1 & CMD_RDTRK)
	{
		sel = (c1 >> 6) & 0x03;
		df[sel].track = c2;
		printf("Selected drive %d\r\nSelected track %d\r\n", sel, c2);
		ReadTrack(&df[sel]);
	}
	else if (c1 & CMD_WRTRK)
	{
		sel = (c1 >> 6) & 0x03;
		df[sel].track = c2;
		WriteTrack(&df[sel]);
	}
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
		mainprintf("UNSUPPORTED ADF SIZE!!! Too many tracks: %lu\n", tracks);
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
	mainprintf("file writable: %d\r\n", writable);
	mainprintf("file size: %lu (%lu KB)\r\n", drive->size, drive->size >> 10);
	mainprintf("drive tracks: %u\r\n", drive->tracks);
	mainprintf("drive track_prev: %u\r\n", drive->track_prev);
	mainprintf("drive status: 0x%02X\r\n", drive->status);
	return;
}
