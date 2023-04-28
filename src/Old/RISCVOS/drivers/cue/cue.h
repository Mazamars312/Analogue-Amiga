#ifndef CUE_H
#define CUE_H

// CDD command
#define CD_COMM_TESTUNIT			0x00
#define CD_COMM_REQUESTSENSE		0x03
#define CD_COMM_READ6			0x08
#define CD_COMM_MODESELECT6		0x15
#define CD_COMM_SAPSP			0xD8
#define CD_COMM_SAPEP			0xD9
#define CD_COMM_PAUSE			0xDA
#define CD_COMM_READSUBQ			0xDD
#define CD_COMM_GETDIRINFO		0xDE

#define CD_STATE_IDLE		0
#define CD_STATE_NODISC		1
#define CD_STATE_READ		2
#define CD_STATE_PLAY		3
#define CD_STATE_PAUSE		4

#define CD_STATUS_GOOD			0
#define CD_STATUS_CHECK_COND		1
#define CD_STATUS_CONDITION_MET	2
#define CD_STATUS_BUSY			4
#define CD_STATUS_INTERMEDIATE	8

#define SENSEKEY_NO_SENSE			0x0
#define SENSEKEY_NOT_READY			0x2
#define SENSEKEY_MEDIUM_ERROR		0x3
#define SENSEKEY_HARDWARE_ERROR		0x4
#define SENSEKEY_ILLEGAL_REQUEST	0x5
#define SENSEKEY_UNIT_ATTENTION		0x6
#define SENSEKEY_ABORTED_COMMAND	0xB

#define NSE_NO_DISC					0x0B
#define NSE_TRAY_OPEN				0x0D
#define NSE_SEEK_ERROR				0x15
#define NSE_HEADER_READ_ERROR		0x16
#define NSE_NOT_AUDIO_TRACK			0x1C
#define NSE_NOT_DATA_TRACK			0x1D
#define NSE_INVALID_COMMAND			0x20
#define NSE_INVALID_ADDRESS			0x21
#define NSE_INVALID_PARAMETER		0x22
#define NSE_END_OF_VOLUME			0x25
#define NSE_INVALID_REQUEST_IN_CDB	0x27
#define NSE_DISC_CHANGED			0x28
#define NSE_AUDIO_NOT_PLAYING		0x2C

#define CD_CDDAMODE_SILENT		0x00
#define CD_CDDAMODE_LOOP			0x01
#define CD_CDDAMODE_INTERRUPT	0x02
#define CD_CDDAMODE_NORMAL		0x03

// #include <stdlib.h>
#include <string.h>
// #include <inttypes.h>
#include <stdio.h>
#include "timer.h"
#include "spi.h"
#include "printf.h"
#include "apf.h"
#include "fileio.h"

typedef int (*SendDataFunc) (uint8_t* buf, int len, uint8_t index);

typedef struct
{
	uint8_t key;
	uint8_t asc;
	uint8_t ascq;
	uint8_t fru;
} sense_t;

typedef enum
{
        SUBCODE_NONE = 0, SUBCODE_RW, SUBCODE_RW_RAW
} cd_subcode_types_t;

typedef struct
{
	int dataslot = 0;
	int opened = 0;
	int size = 0;
	int offset = 0;
	int pregap = 0;
	int start = 0;
	int end = 0;
	int type = 0;
	int sector_size = 0;
	int index1 = 0;
	cd_subcode_types_t sbc_type;
} cd_track_t;

typedef struct
{
	int end=0; // Total size of CD rom
	int last=0; // last track number
	cd_track_t tracks[30];
	int GetTrackByLBA(int lba)
	{
		int i = 0;
		while ((this->tracks[i].end <= lba) && (i < this->last)) i++;
		return i;
	}
} toc_t;

typedef struct
{
	uint8_t m;
	uint8_t s;
	uint8_t f;
} msf_t;

class cue_t
{
public:
	uint32_t latency;
	uint32_t audiodelay;
	uint8_t state;
	uint8_t isData;
	int loaded;
	SendDataFunc SendData;
	int has_status;
	bool data_req;
	bool can_read_next;

	cue_t();
	int Load(int dataslot);
	void Unload();
	void Reset();
	void Update();
	void CommandExec();
	uint16_t GetStatus();
	int SetCommand(uint8_t* buf);
	void PendStatus(uint16_t status);
	void SendStatus(uint16_t status);
	void SendDataRequest();
	void SetRegion(uint8_t rgn);

private:
	toc_t toc;
	int index;
	int lba;
	int cnt;
	int scanOffset;
	int audioLength;
	int audioOffset;
	int CDDAStart;
	int CDDAEnd;
	int CDDAFirst;
	uint8_t CDDAMode;
	sense_t sense;
	uint8_t region;
	uint8_t *chd_hunkbuf;
	int chd_hunknum;

	uint16_t stat;
	uint8_t comm[14];

	uint8_t sec_buf[2352 + 2];

	int LoadCUE(int dataslot);
	int SectorSend(uint8_t* header);
	void ReadData();
	int ReadCDDA();
	void ReadSubcode(uint16_t* buf);
	void LBAToMSF(int lba, msf_t* msf);
	void MSFToLBA(int* lba, msf_t* msf);
	void MSFToLBA(int* lba, uint8_t m, uint8_t s, uint8_t f);
	int GetTrackByLBA(int lba, toc_t* toc);
	void CommandError(uint8_t key,	uint8_t asc, uint8_t ascq, uint8_t fru);
};

#define MAKE_STATUS(s,m)				 ((uint16_t)((((m)&0xFF) << 8) | ((s)&0xFF)))

#define BCD(v)				 ((uint8_t)((((v)/10) << 4) | ((v)%10)))
#define U8(v)				 ((uint8_t)(((((v)&0xF0) >> 4) * 10) + ((v)&0x0F)))

#define CD_SCAN_SPEED 30

//pcecdd.cpp
extern cue_t cue;


void pcecd_poll();
void pcecd_set_image(int dataslot, int size); // Done
int pcecd_send_data(uint8_t* buf, int len, uint8_t index); // Need to work on
void pcecd_reset(); // done
int pcecd_using_cd(); // done

#endif
