#ifndef IDE_H
#define IDE_H
// #include <stdlib.h>
#include <string.h>
// #include <inttypes.h>
#include <stdio.h>
#include "timer.h"
#include "spi.h"
#include "riscprintf.h"
#include "apf.h"
#include "fileio.h"
// #include "osd_menu.h"
#define ATA_STATUS_BSY  0x80  // busy
#define ATA_STATUS_RDY  0x40  // ready
#define ATA_STATUS_RDP  0x20  // performance read
#define ATA_STATUS_DSC  0x10  // seek complete
#define ATA_STATUS_SERV 0x10  // service
#define ATA_STATUS_DRQ  0x08  // data request
#define ATA_STATUS_IRQ  0x04  // rise IRQ
#define ATA_STATUS_END  0x02  // last read
#define ATA_STATUS_ERR  0x01  // error (ATA)
#define ATA_STATUS_CHK  0x01  // check (ATAPI)

#define ATA_ERR_ICRC    0x80    // ATA Ultra DMA bad CRC
#define ATA_ERR_BBK     0x80    // ATA bad block
#define ATA_ERR_UNC     0x40    // ATA uncorrected error
#define ATA_ERR_MC      0x20    // ATA media change
#define ATA_ERR_IDNF    0x10    // ATA id not found
#define ATA_ERR_MCR     0x08    // ATA media change request
#define ATA_ERR_ABRT    0x04    // ATA command aborted
#define ATA_ERR_NTK0    0x02    // ATA track 0 not found
#define ATA_ERR_NDAM    0x01    // ATA address mark not found

#define IDE_STATE_IDLE          0
#define IDE_STATE_RESET         1
#define IDE_STATE_INIT_RW       2
#define IDE_STATE_WAIT_PKT_CMD  3
#define IDE_STATE_WAIT_PKT_RD   4
#define IDE_STATE_WAIT_PKT_END  5
#define IDE_STATE_WAIT_PKT_MODE 6

#define RDB_MAGIC 0x4B534452 // "RDSK"

struct RigidDiskBlock {
	unsigned long rdb_ID;              // "RDSK"
	unsigned long rdb_Summedlongs;     // 0x40
	long          rdb_ChkSum;          // Sum to zero
	unsigned long rdb_HostID;          // 0x07
	unsigned long rdb_BlockBytes;      // 0x200
	unsigned long rdb_Flags;           // 0x12 (Disk ID valid, no LUNs after this one)
	unsigned long rdb_BadBlockList;    // -1 since we don't provide one
	unsigned long rdb_PartitionList;   // 1
	unsigned long rdb_FileSysHeaderList; // -1
	unsigned long rdb_DriveInit;       // -1
	unsigned long rdb_Reserved1[6];    // 0xffffffff
	unsigned long rdb_Cylinders;
	unsigned long rdb_Sectors;
	unsigned long rdb_Heads;
	unsigned long rdb_Interleave;      // 1
	unsigned long rdb_Park;            // =Cylinder count
	unsigned long rdb_Reserved2[3];
	unsigned long rdb_WritePreComp;    // High cylinder ?
	unsigned long rdb_ReducedWrite;    // High cylinder ?
	unsigned long rdb_StepRate;        // 3 ?
	unsigned long rdb_Reserved3[5];
	unsigned long rdb_RDBBlocksLo;     // block zero
	unsigned long rdb_RDBBlocksHi;     // block one
	unsigned long rdb_LoCylinder;      // 1
	unsigned long rdb_HiCylinder;      // From the hardfile: cylinder count -1
	unsigned long rdb_CylBlocks;       // From the hardfile: heads * sectors
	unsigned long rdb_AutoParkSeconds; // zero
	unsigned long rdb_HighRDSKBlock;   // 1
	unsigned long rdb_Reserved4;
	char          rdb_DiskVendor[8];   // "Don't"
	char          rdb_DiskProduct[16]; // " repartition!"
	char          rdb_DiskRevision[4];
	char          rdb_ControllerVendor[8];
	char          rdb_ControllerProduct[16];
	char          rdb_ControllerRevision[4];
	unsigned long rdb_Reserved5[10];
} __attribute__((packed));

struct DosEnvec {
	unsigned long de_TableSize;	     // Size of Environment vector - 0x10
	unsigned long de_SizeBlock;	     // in longwords - 0x80
	unsigned long de_SecOrg;	     // 0
	unsigned long de_Surfaces;		 // Heads?
	unsigned long de_SectorPerBlock; // 1
	unsigned long de_BlocksPerTrack;
	unsigned long de_Reserved;	     // 2 ?
	unsigned long de_PreAlloc;	     // 0
	unsigned long de_Interleave;     // 0
	unsigned long de_LowCyl;
	unsigned long de_HighCyl;
	unsigned long de_NumBuffers;     // 30
	unsigned long de_BufMemType;     // 0 - any available
	unsigned long de_MaxTransfer;    // 0x00ffffff
	unsigned long de_Mask;           // 0x7ffffffe
	long          de_BootPri;	     // 0
	unsigned long de_DosType;	     // 0x444f5301 or 3
							         // Extra fields
	unsigned long de_Baud;
	unsigned long de_Control;
	unsigned long de_BootBlocks;
} __attribute__((packed));

struct PartitionBlock {
	unsigned long pb_ID;             // "PART"
	unsigned long pb_Summedlongs;	 // 0x40
	long          pb_ChkSum;		 // Sum to zero
	unsigned long pb_HostID;		 // 0x07
	unsigned long pb_Next;           // -1
	unsigned long pb_Flags;          // 1 - Bootable
	unsigned long pb_Reserved1[2];   // 0
	unsigned long pb_DevFlags;       // 0
	char          pb_DriveName[32];  // 0x03"DH0"
	unsigned long pb_Reserved2[15];
	DosEnvec      pb_Environment;
	unsigned long pb_EReserved[12];  // reserved for future environment vector
} __attribute__((packed));

struct regs_t
{
	uint8_t io_done = 0;
	uint8_t io_fast = 0;
	uint8_t features = 0;
	uint16_t dataslot = 0;
	uint8_t sector_count = 0;
	uint8_t sector = 0;
	uint16_t cylinder = 0;
	uint8_t head = 0;
	uint8_t drv = 0;
	uint8_t lba = 0;
	uint8_t cmd = 0;

	uint16_t pkt_size_limit = 0;
	uint16_t pkt_io_size = 0;
	uint32_t pkt_lba = 0;
	uint32_t pkt_cnt = 0;

	uint8_t io_size = 0;
	uint8_t error = 0;
	uint8_t status = 0;
};

struct track_t
{
	uint16_t dataslot = 0;
	uint32_t start;
	uint32_t length;
	uint32_t skip;
	uint16_t sectorSize;
	uint8_t  attr;
	uint8_t  mode2;
	uint8_t  number;
	int      chd_offset;
};

struct drive_t
{
	uint16_t dataslot = 0;

	uint8_t  present = 0;
	uint8_t  drvnum = 0;
	uint32_t Size = 0;

	uint16_t cylinders = 0;
	uint16_t heads = 0;
	uint16_t spt = 0;
	uint32_t total_sectors = 0;
	uint32_t spb = 0;

	uint32_t offset = 0;
	uint32_t type = 0;

	uint8_t  placeholder = 0;
	uint8_t  allow_placeholder = 0;
	uint8_t  cd = 0;
	uint8_t  load_state = 0;
	uint8_t  last_load_state = 0;
	uint8_t  track_cnt = 0;
	uint8_t  data_num = 0;
	track_t  track[50];

	uint8_t  playing = 0;
	uint8_t  paused = 0;
	uint32_t play_start_lba = 0;
	uint32_t play_end_lba = 0;

	int      chd_hunknum;
	uint8_t	 *chd_hunkbuf = 0;
	uint32_t  chd_total_size = 0;
	uint32_t  chd_last_partial_lba = 0;

	uint16_t id[256];
};

struct ide_config
{
	uint32_t base = 0;
	uint32_t bitoff = 0;
	uint32_t state = 0;
	uint32_t null = 0;
	uint32_t prepcnt = 0;
	regs_t   regs;

	drive_t drive[2];
};

struct chs_t
{
	uint16_t dataslot = 0;
	uint32_t sectors = 0;
	uint32_t heads = 0;
	uint32_t cylinders = 0;
	uint32_t offset = 0;
};

extern ide_config ide_inst[];
extern const uint32_t ide_io_max_size;
extern uint8_t ide_buf[];

void ide_get_regs(ide_config *ide);
void ide_set_regs(ide_config *ide);

void ide_sendbuf(ide_config *ide, uint16_t reg, uint32_t length, uint16_t *data, bool printtxt);
void ide_recvbuf(ide_config *ide, uint16_t reg, uint32_t length, uint16_t *data, bool printtxt);
void ide_reg_set(ide_config *ide, uint16_t reg, uint16_t value);

uint16_t ide_check();
void ide_img_set(uint32_t drvnum, uint16_t dataslot, int cd, int sectors = 0, int heads = 0, int offset = 0);
int ide_is_placeholder(int num);
void ide_reset(uint8_t hotswap[4]);
int ide_open(uint8_t unit, uint16_t dataslot);

void ide_io(int num, int req);

#endif
