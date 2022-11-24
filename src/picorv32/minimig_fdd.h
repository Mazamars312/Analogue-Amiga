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
void HandleFDD(unsigned char c1, unsigned char c2);
void InsertFloppy(adfTYPE *df, uint32_t fsize, uint32_t drive_dataslot);
#ifdef __cplusplus
}
#endif
#endif
