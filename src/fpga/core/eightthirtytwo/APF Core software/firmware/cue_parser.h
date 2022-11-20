#ifndef __CUE_PARSER_H__
#define __CUE_PARSER_H__

#ifndef CUE_PARSER_TEST
#include "minfat.h"
#endif

#define SECTOR_AUDIO 0
#define SECTOR_DATA 1

#define CUE_RES_OK       0
#define CUE_RES_NOTFOUND 1
#define CUE_RES_INVALID  2
#define CUE_RES_UNS      3
#define CUE_RES_BINERR   4

typedef struct
{
        int offset;
        int start;
        int end;
        int type;
        int sector_size;
		int idx;
} cd_track_t;

typedef struct
{
        int end;
        int last;
        int sectorSize;
		cd_track_t track;
//        cd_track_t tracks[100]; // Nope!
#ifndef CUE_PARSER_TEST
        fileTYPE file; // the .bin file
#endif
} toc_t;

typedef struct
{
        unsigned char m;
        unsigned char s;
        unsigned char f;
} msf_t;

extern toc_t toc;

#define user_io_is_cue_mounted() toc.file.size

int cue_open(const char *filename);
#ifdef CUE_PARSER_TEST
int cue_parse(int trackno);
#else
int cue_parse(int trackno);
#endif
void LBA2MSF(int lba, msf_t* msf);
int MSF2LBA(unsigned char m, unsigned char s, unsigned char f);
int cue_gettrackbylba(int lba);

#endif // __CUE_PARSER_H__

