// cue_parser.c
// 2021, Gyorgy Szombathelyi
// Amended for low RAM by Alastair M. Robinson

#include <string.h>
#include <stdio.h>
//#include <stdlib.h>
#include "cue_parser.h"
#ifdef CUE_PARSER_TEST
#define cue_parser_debugf(a, ...) 
// printf(a"\n", ## __VA_ARGS__)
#else
#include "minfat.h"
#endif

#define cue_parser_debugf printf;


//// defines ////
#define TOKEN_FILE              "FILE"
#define TOKEN_BINARY            "BINARY"
#define TOKEN_TRACK             "TRACK"
#define TOKEN_AUDIO             "AUDIO"
#define TOKEN_MODE1_2048        "MODE1/2048"
#define TOKEN_MODE1_2352        "MODE1/2352"
#define TOKEN_PREGAP            "PREGAP"
#define TOKEN_INDEX             "INDEX"

#define MODE_NONE               0
#define MODE_FILE               1
#define MODE_TRACK              2
#define MODE_PREGAP             3
#define MODE_INDEX              4

#define CUE_WORD_SIZE           64

#define CUE_EOT                 4
#define CUE_NOWORD              3
//// macros ////
#define CHAR_IS_EOF(c)          ((c) < 0)
#define CHAR_IS_NUM(c)          (((c) >= '0') && ((c) <= '9'))
#define CHAR_IS_ALPHA_LOWER(c)  (((c) >= 'a') && ((c) <= 'z'))
#define CHAR_IS_ALPHA_UPPER(c)  (((c) >= 'A') && ((c) <= 'Z'))
#define CHAR_IS_ALPHA(c)        (CHAR_IS_ALPHA_LOWER(c) || CHAR_IS_ALPHA_UPPER(c))
#define CHAR_IS_ALPHANUM(c)     (CHAR_IS_ALPHA_LOWER(c) || CHAR_IS_ALPHA_UPPER(c) || CHAR_IS_NUM(c))
#define CHAR_IS_SPECIAL(c)      (((c) == '[') || ((c) == ']') || ((c) == '-') || ((c) == '_') || ((c) == ',') || ((c) == '=') || ((c) == '~') || ((c) == ':') || ((c) == '/'))
#define CHAR_IS_VALID(c)        (CHAR_IS_ALPHANUM(c) || CHAR_IS_SPECIAL(c))
#define CHAR_IS_WHITESPACE(c)   (((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n'))
#define CHAR_IS_SPACE(c)        (((c) == ' ') || ((c) == '\t'))
#define CHAR_IS_LINEEND(c)      (((c) == '\n'))
#define CHAR_IS_COMMENT(c)      (((c) == ';'))
#define CHAR_IS_QUOTE(c)        (((c) == '"'))
#define CHAR_TO_UPPERCASE(c)    ({ char _c = (c); if (CHAR_IS_ALPHA_LOWER(_c)) _c = _c - 'a' + 'A'; _c;})
#define CHAR_TO_LOWERCASE(c)    ({ char _c = (c); if (CHAR_IS_ALPHA_UPPER(_c)) _c = _c - 'A' + 'a'; _c;})

#ifdef CUE_PARSER_TEST
FILE* cue_fp = NULL;
char  sector_buffer[512] = {0};
int   cue_size = 0;
#else
fileTYPE cue_file;
#endif

static int cue_pt = 0;

toc_t toc;


void LBA2MSF(int lba, msf_t* msf) {
  msf->m = (lba / 75) / 60;
  msf->s = (lba / 75) % 60;
  msf->f = (lba % 75);
}

int MSF2LBA(unsigned char m, unsigned char s, unsigned char f) {
  return (f + s * 75 + m * 60 * 75 - 150);
}

static char ParseMSF(const char *s, msf_t *msf) {
  char c;

  if (*s && CHAR_IS_NUM(*s)) msf->m = 10*(*s++ - '0'); else return 0;
  if (*s && CHAR_IS_NUM(*s)) msf->m+= (*s++ - '0'); else return 0;
  if (*s && *s == ':') s++; else return 0;
  if (*s && CHAR_IS_NUM(*s)) msf->s = 10*(*s++ - '0'); else return 0;
  if (*s && CHAR_IS_NUM(*s)) msf->s+= (*s++ - '0'); else return 0;
  if (*s && *s == ':') s++; else return 0;
  if (*s && CHAR_IS_NUM(*s)) msf->f = 10*(*s++ - '0'); else return 0;
  if (*s && CHAR_IS_NUM(*s)) msf->f+= (*s++ - '0'); else return 0;
  if (*s) return 0;
  return 1;
}


static int cue_getword(char* word)
{
  int c;
  char literal=0;
  int i=0;

  while(1) {
    c = FileGetCh(&cue_file);
    if (CHAR_IS_EOF(c) || CHAR_IS_LINEEND(c) || (CHAR_IS_WHITESPACE(c) && !literal)) break;
    else if (CHAR_IS_QUOTE(c)) literal ^= 1;
    else if ((literal || (CHAR_IS_VALID(c))) && i<(CUE_WORD_SIZE-1)) word[i++] = c;
  }
  word[i] = '\0';
  return CHAR_IS_EOF(c) ? CUE_EOT : i == 0 ? CUE_NOWORD : literal ? 1 : 0;
}

char cueword[CUE_WORD_SIZE] = {0};

int cue_open(const char *filename)
{
	int result=CUE_RES_OK;
	memset(&toc, 0, sizeof(toc));
//	printf("Opening cuefile %s\n",filename);
#ifdef CUE_PARSER_TEST
	if ((cue_fp = fopen(filename, "rb")) == NULL)
#else
	if (!FileOpen(&cue_file,filename))
#endif
	    result=CUE_RES_NOTFOUND;
	else
	{
		result=cue_parse(1);
		if(!result)
			result=cue_parse(toc.last);
		if(!result)
			toc.end=toc.track.end;
		else
			toc.last=0;
	}
	return(result);
}

//// cue_parse() ////
#ifdef CUE_PARSER_TEST
int cue_parse(int trackno)
#else
int cue_parse(int trackno)
#endif
{
  int word_status;
  char mode = 0, submode = 0, error = CUE_RES_OK, index = 0, bin_valid = 0;
  int track = 0, x, pregap = 0, tracklen;
  msf_t msf;
  int lba, lastindex1 = 0;
  int offset=0,sector_size=0;
  int track_sectorsize;
  int track_type;

  if(!cue_file.size)
    return(CUE_RES_NOTFOUND);

  #ifdef CUE_PARSER_TEST
  fseek(cue_fp, 0L, SEEK_END);
  cue_size = ftell(cue_fp);
  fseek(cue_fp, 0L, SEEK_SET);
  #else
//  cue_parser_debugf("Opened file %s with size %llu bytes.", filename, f_size(&cue_file));
  FileFirstSector(&cue_file);
  #endif
  cue_pt = 0;

  toc.track.end=0;

  // parse ini
  while (1) {
    // get line
    word_status = cue_getword(cueword);
    if (word_status != CUE_NOWORD) {
      //cue_parser_debugf("next word(%d): \"%s\".", word_status, cueword);
      switch (mode) {
        case MODE_NONE:
          submode = 0;
          if (!strcmp(cueword, TOKEN_FILE))   mode = MODE_FILE; else
          if (!strcmp(cueword, TOKEN_TRACK))  mode = MODE_TRACK; else
          if (!strcmp(cueword, TOKEN_PREGAP)) mode = MODE_PREGAP; else
          if (!strcmp(cueword, TOKEN_INDEX))  mode = MODE_INDEX;
          break;
        case MODE_FILE:
          if (submode == 0) {
            pregap = 0;
//            cue_parser_debugf("Filename: %s", cueword);
            if (bin_valid) {
              // only one .bin supported
//              error = CUE_RES_UNS;
            } else {
            #ifdef CUE_PARSER_TEST
              bin_valid = 1;
            }
            #else
//              toc.file = image;
//				printf("BIN file: %s\n",cueword);
              if(toc.file.size)
                bin_valid=1;
              else
              {
                if (FileOpen(&toc.file,cueword))
                  bin_valid = 1;
                else
                  error = CUE_RES_BINERR;
				/* Refresh sector buffer, since opening the other file will have overwritten it */
				FileReadSector(&cue_file,sector_buffer);
              }
            }
            #endif
          } else if (submode == 1) {
//            cue_parser_debugf("Filemode: %s", cueword);
            mode = 0;
          }
          submode++;
          break;
        case MODE_TRACK:
          sector_size=track_sectorsize; // Keep the last track's sector size
          if (submode == 0) {
            x = strtol(cueword, 0, 10);
//            cue_parser_debugf("Trackno: %d -> %d (%s)", track, x, cueword);
            if (!x || x > 99 || x != (track + 1)) error = CUE_RES_INVALID; else track = x;
          } else if (submode == 1) {
//            cue_parser_debugf("Trackmode: %s", cueword);
            if (!strcmp(cueword, TOKEN_AUDIO)) {
				//amr
			  track_sectorsize=2352;
			  track_type=SECTOR_AUDIO;
				//gs
//              toc.tracks[track-1].sector_size = 2352;
 //             toc.tracks[track-1].type = SECTOR_AUDIO;
            } else if (!strcmp(cueword, TOKEN_MODE1_2352)) {
				//amr
			  track_sectorsize=2352;
			  track_type=SECTOR_DATA;
				//gs
 //             toc.tracks[track-1].sector_size = 2352;
//              toc.tracks[track-1].type = SECTOR_DATA;
            } else if (!strcmp(cueword, TOKEN_MODE1_2048)) {
				//amr
			  track_sectorsize=2048;
			  track_type=SECTOR_DATA;
				//gs
//              toc.tracks[track-1].sector_size = 2048;
//              toc.tracks[track-1].type = SECTOR_DATA;
            } else {
              error = CUE_RES_INVALID;
            }
            if(trackno==track)
            {
              toc.track.sector_size = track_sectorsize;
              toc.track.type = track_type;
            }

            mode = 0;
          }
          submode++;
          break;
        case MODE_PREGAP:
//          cue_parser_debugf("Pregap size: %s", cueword);
            if (!ParseMSF(cueword, &msf)) {
              error = CUE_RES_INVALID;
            } else {
              pregap += MSF2LBA(msf.m, msf.s, msf.f) + 150;
            }
          mode = 0;
          break;
        case MODE_INDEX:
          if (submode == 0) {
//            cue_parser_debugf("Index: %s", cueword);
            index = strtol(cueword, 0, 10);
          } else if (submode == 1) {
            if (!ParseMSF(cueword, &msf)) {
              error = CUE_RES_INVALID;
            } else {
              lba = MSF2LBA(msf.m, msf.s, msf.f);
              if (index == 0) {
				//amr
                if (track == trackno+1 && !toc.track.end) {
//					printf("t1 %d, lba %d, pregap %d\n",track,lba,pregap);
                  toc.track.end =  lba + 150 + pregap;
                }
				//gs
//                if (track > 1 && !toc.tracks[track - 2].end) {
//                  toc.tracks[track - 2].end =  lba + 150 + pregap;
//                }
              } else if (index == 1) {
				//amr
                if (track == trackno) {
                  toc.track.start =  lba + 150 + pregap;
                }
				//gs
//                toc.tracks[track - 1].start = lba + 150 + pregap;
                if (track > 1) {
                  tracklen = lba - lastindex1;

				//amr
                  if(track==trackno)
                  {
					//track offset and sector_size from previous track.
                    toc.track.offset = offset + (tracklen * sector_size);
                  }
                  if (track==trackno+1 && !toc.track.end)
					toc.track.end = lba + 150 + pregap - 1;

				//gs
                  offset+= tracklen * sector_size;
//                  offset=toc.tracks[track - 1].offset = toc.tracks[track - 2].offset + (tracklen * toc.tracks[track - 2].sector_size);
//                  if (!toc.tracks[track-2].end) toc.tracks[track - 2].end = toc.tracks[track - 1].start - 1;
                } else {
                  offset+= (lba + 150) * sector_size;
				//gs
//                  offset=toc.tracks[0].offset = (lba + 150) * toc.tracks[0].sector_size;
				//amr
                  if(trackno==1)
                    toc.track.offset = offset;
                }
                lastindex1 = lba;
              }
            }
//            cue_parser_debugf("Pos: (%s) = %d:%d:%d (%d)", cueword, msf.m, msf.s, msf.f, error);
            mode = 0;
          }
          submode++;
          break;
      }
    }
    // if end of file or error, stop
    if (word_status == CUE_EOT || error) break;
  }


  #ifdef CUE_PARSER_TEST
  // close file
  fclose(cue_fp);
  #else
//  f_close(&cue_file);
  if (!bin_valid)
    error = CUE_RES_BINERR;
//  else if (error)
//	 f_close(&toc.file->file);
  else {
    if (track == trackno) {
      tracklen = (toc.file.size - toc.track.offset) / toc.track.sector_size;
      toc.track.end = toc.track.start + tracklen;
    }
  }
  #endif

  if(track<trackno)
	error=CUE_RES_BINERR;

  if (error) {
    toc.last = 0;
  } else {
    toc.last = track;
  }

//  iprintf("track %d: ",trackno);
  LBA2MSF(toc.track.start, &msf);
//  iprintf("start: %d - %02d:%02d:%02d (%d) end: %d sector size:%d\n",
//    toc.track.start, msf.m, msf.s, msf.f, toc.track.offset, toc.track.end, toc.track.sector_size);

//  cue_parser_debugf("TOC end: %d, %d\n",toc.end,toc.end2);
//  cue_parser_debugf("Last track: %d\n",toc.last);

  toc.track.idx=trackno;

  return error;
}

// FIXME - the semantics of this function need to change - rather than looking up a particular track it must
// determine whether a given LBA is still within the current track.
// We also need to be able to hunt for a track by LBA.
int cue_gettrackbylba(int lba) {
  int index = 1;
  do {
//	printf("Comparing lba %d with %d and %d (%d)\n",lba,toc.track.start,toc.track.end,toc.track.idx);
    if(toc.track.start<=lba && toc.track.end>=lba)
      return(toc.track.idx);
    cue_parse(index);
  } while(++index<=toc.last);
  return(-1);
}

