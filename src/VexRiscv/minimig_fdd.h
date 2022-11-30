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

#ifndef MINIMIG_FDD_H
#define MINIMIG_FDD_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// floppy disk interface defs
#define CMD_RDTRK 0x01
#define CMD_WRTRK 0x02

// floppy status
#define DSK_INSERTED 0x01 /*disk is inserted*/
#define DSK_WRITABLE 0x10 /*disk is writable*/

#define MAX_TRACKS (83*2)

typedef struct adfTYPE
{
	uint32_t      dataslot;
	uint32_t      size; // Size of the image
	int      update; // True = this dataslot has been updated and is not loaded. False = is loaded
	int      updatetimer; // This is wait value using the timer value + the wanted delay.
	unsigned char status; /*status of floppy*/
	unsigned char tracks; /*number of tracks*/
	unsigned char sector_offset; /*sector offset to handle tricky loaders*/
	unsigned char track; /*current track*/
	unsigned char track_prev; /*previous track*/
} adfTYPE;

extern unsigned char drives;
extern adfTYPE df[4];

void UpdateDriveStatus(void);
void RemoveDriveStatus();
void HandleFDD(unsigned char c1, unsigned char c2);
void UnsertFloppy(adfTYPE *df);
void InsertFloppy(adfTYPE *df, uint32_t fsize, uint32_t drive_dataslot);
#ifdef __cplusplus
}
#endif
#endif
