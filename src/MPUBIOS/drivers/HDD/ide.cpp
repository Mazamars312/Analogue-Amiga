#include "ide.h"


#define IDE0_BASE 0xF000
#define IDE1_BASE 0xF100

typedef struct
{
	int      unit;
	int      enabled;
	uint16_t dataslot;
	uint32_t size;
	uint32_t cylinders;
	uint16_t heads;
	uint16_t sectors;
	uint16_t sectors_per_block;
	int32_t  offset; // if a partition, the lba offset of the partition.  Can be negative if we've synthesized an RDB.

	uint8_t  lu;
	int32_t  lba, nextlba;
	uint16_t sector;
	uint16_t cylinder;
	uint16_t head;
	uint16_t sector_count;
} hdfTYPE;


ide_config ide_inst[2];
static hdfTYPE HDF[4];
const uint32_t ide_io_max_size = 32;
uint8_t sector_buffer[ide_io_max_size * 512];

#define ide_send_data(databuf, size, printtxt) ide_sendbuf(ide, 255, (size), (uint16_t*)(databuf), (printtxt))
#define ide_recv_data(databuf, size, printtxt) ide_recvbuf(ide, 255, (size), (uint16_t*)(databuf), (printtxt))

#define SWAP(a)  ((((a)&0x000000ff)<<24)|(((a)&0x0000ff00)<<8)|(((a)&0x00ff0000)>>8)|(((a)&0xff000000)>>24))

void ide_reg_set(ide_config *ide, uint16_t reg, uint16_t value)
{
	mainprintf("ide_reg_set %0.8X %0.4X\r\n", ide->base + reg, value);
	HPS_EnableIO();
	spi8(UIO_DMA_WRITE);
	spi32_w(ide->base + reg);
	spi_w(value);
	HPS_DisableIO();
}

void ide_sendbuf(ide_config *ide, uint16_t reg, uint32_t length, uint16_t *data, bool printtxt)
{
	// if (printtxt) mainprintf("send buffer: length %d\r\n", length);
	HPS_EnableIO();
	HPS_fpga_spi_fast(UIO_DMA_WRITE);
	HPS_fpga_spi_fast(ide->base + reg);
	HPS_fpga_spi_fast(0);
	HPS_fpga_spi_fast_block_write(data, length);
	HPS_DisableIO();
}

void ide_recvbuf(ide_config *ide, uint16_t reg, uint32_t length, uint16_t *data, bool printtxt)
{
	// mainprintf("Received buffer: length %d\r\n", length);
	HPS_EnableIO();
	HPS_fpga_spi_fast(UIO_DMA_READ);
	HPS_fpga_spi_fast(ide->base + reg);
	HPS_fpga_spi_fast(0);
	HPS_fpga_spi_fast_block_read(data, length, printtxt);
	HPS_DisableIO();
}



uint16_t ide_check()
{
	uint16_t res;
	HPS_EnableIO();
	res = spi_w(UIO_DMA_SDIO);
	if (!res) res = (uint8_t)spi_w(0);
	HPS_DisableIO();
	return res;
}

void ide_get_regs(ide_config *ide)
{
	uint32_t data[3];
	ide_recvbuf(ide, 0, 6, (uint16_t*)data, 0);

	ide->regs.io_done = (uint8_t)(data[0] & 1);
	ide->regs.io_fast = (uint8_t)(data[0] & 2);
	ide->regs.features = (uint8_t)(data[0] >> 8);
	ide->regs.sector_count = (uint8_t)(data[0] >> 16);
	ide->regs.sector = (uint8_t)(data[0] >> 24);

	ide->regs.cylinder = data[1] & 0xFFFF;
	ide->regs.head = (data[2] >> 16) & 0xF;
	ide->regs.drv = (data[2] >> 20) & 1;
	ide->regs.lba = (data[2] >> 22) & 1;
	ide->regs.cmd = data[2] >> 24;
	// mainprintf("ide_get_reg: %0.8x %0.2x\r\n", data[2], ide->regs.drv);
	ide->regs.error = 0;
	ide->regs.status = 0;

}

void ide_set_regs(ide_config *ide)
{
	if (!(ide->regs.status & (ATA_STATUS_BSY | ATA_STATUS_ERR))) ide->regs.status |= ATA_STATUS_DSC;

	uint8_t data[12] =
	{
		(uint8_t)((ide->drive[ide->regs.drv].cd) ? 0x80 : ide->regs.io_size),
		(uint8_t)(ide->regs.error),
		(uint8_t)(ide->regs.sector_count),
		(uint8_t)(ide->regs.sector),

		(uint8_t)(ide->regs.cylinder),
		(uint8_t)(ide->regs.cylinder >> 8),
		(uint8_t)(ide->regs.cylinder >> 16),
		(uint8_t)(ide->regs.cylinder >> 24),

		(uint8_t)(ide->drive[ide->regs.drv].cd ? ide->regs.pkt_io_size : 0),
		(uint8_t)(ide->drive[ide->regs.drv].cd ? ide->regs.pkt_io_size >> 8 : 0),
		(uint8_t)((ide->regs.lba ? 0xE0 : 0xA0) | (ide->regs.drv ? 0x10 : 0x00) | ide->regs.head),
		(uint8_t)(ide->regs.status)
	};
	// mainprintf("ide_set_reg: %0.8x %0.8x %0.8x\r\n", data[2], data[1], data[0]);

	ide_sendbuf(ide, 0, 6, (uint16_t*)data, 0);
}

void calc_geometry(hdfTYPE *hdf)
{
	uint32_t head = 0, cyl = 0, spt = 0;
	uint32_t sptt[] = { 63, 127, 255, 0 };
	uint32_t total = hdf->size / 512;
	for (int i = 0; sptt[i] != 0; i++)
	{
		spt = sptt[i];
		for (head = 4; head <= 16; head++)
		{
			cyl = total / (head * spt);
			if (total <= 1024 * 1024)
			{
				if (cyl <= 1023) break;
			}
			else
			{
				if (cyl < 16383) break;
				if (cyl < 32767 && head >= 5) break;
				if (cyl <= 65536) break;
			}
		}
		if (head <= 16) break;
	}

	hdf->cylinders = cyl;
	hdf->heads = (uint16_t)head;
	hdf->sectors = (uint16_t)spt;
}

void get_rdb_geometry(hdfTYPE *hdf)
{
	struct RigidDiskBlock *rdb = (struct RigidDiskBlock *)sector_buffer;
	hdf->heads = SWAP(rdb->rdb_Heads);
	hdf->sectors = SWAP(rdb->rdb_Sectors);
	hdf->cylinders = SWAP(rdb->rdb_Cylinders);
	if (hdf->sectors > 255 || hdf->heads > 16)
	{
		mainprintf("ATTN: Illegal CHS value(s).\r\n");
		if (!(hdf->sectors & 1) && (hdf->sectors < 512) && (hdf->heads <= 8))
		{
			mainprintf(" Translate: sectors %d->%d, heads %d->%d.\r\n", hdf->sectors, hdf->sectors / 2, hdf->heads, hdf->heads * 2);
			hdf->sectors /= 2;
			hdf->heads *= 2;
			return;
		}

		mainprintf(" DANGEROUS: Cannot translate to legal CHS values. Re-calculate the CHS.\r\n");
		calc_geometry(hdf);
	}
}

void guess_geometry(hdfTYPE *hdf)
{
	uint8_t flg = 0;
	hdf->offset = 0;

	for (int i = 0; i < 16; ++i)
	{
		struct RigidDiskBlock *rdb = (struct RigidDiskBlock *)sector_buffer;
		dataslot_read(hdf->dataslot, (uint32_t)&sector_buffer, i *512,  512);
		for (int i = 0; i < 512; i++) {
			flg |= sector_buffer[i];
			// mainprintf("data In : %0.4\r\n", sector_buffer[i]);
		}

		if (rdb->rdb_ID == RDB_MAGIC)
		{
			mainprintf("Found RDB header -> native Amiga image.\r\n");
			get_rdb_geometry(hdf);
			return;
		}
	}

	if (flg)
	{
		hdf->heads = 16;
		hdf->sectors = 128;

		for (int i = 32; i <= 2048; i <<= 1)
		{
			int cylinders = hdf->size / (512 * i) + 1;
			if (cylinders < 65536)
			{
				hdf->sectors = (i < 128) ? i : 128;
				hdf->heads = i / hdf->sectors;
				break;
			}
		}

		int spc = hdf->heads * hdf->sectors;
		hdf->cylinders = hdf->size / (512 * spc) + 1;
		if (hdf->cylinders > 65535) hdf->cylinders = 65535;
		hdf->offset = -spc;
		mainprintf("No RDB header found in HDF image. Assume it's image of single partition. Use Virtual RDB header.\r\n");
	}
	else
	{
		calc_geometry(hdf);
		mainprintf("No RDB header found. Possible non-Amiga or empty image.\r\n");
	}
}

static void ide_set_geometry(drive_t *drive, uint16_t sectors, uint16_t heads)
{
	int info = 0;
	if (drive->heads != heads || drive->spt != sectors)
	{
		info = 1;
		mainprintf("SPT=%d, Heads=%d\r\n", sectors, heads);
	}

	drive->heads = heads ? heads : 16;
	drive->spt = sectors ? sectors : 256;

	uint32_t cylinders = drive->Size / (drive->heads * drive->spt * 512);
	if (drive->offset)
	{
		cylinders++;
		drive->offset = drive->heads * drive->spt;
	}
	if (cylinders > 65535) cylinders = 65535;

	//Maximum 137GB images are supported.
	drive->cylinders = cylinders;
	mainprintf("New SPT=%d, Heads=%d, Cylinders=%d\r\n", drive->spt, drive->heads, drive->cylinders);
}

static uint32_t checksum_rdb(uint32_t *p, int set)
{
	uint32_t count = SWAP(p[1]);
	uint32_t result = 0;
	if (set) p[2] = 0;

	for (uint32_t i = 0; i < count; ++i) result += SWAP(p[i]);
	if (!set) return result;

	result = 0 - result;
	p[2] = SWAP(result);
	return 0;
}


void ide_img_set(uint32_t drvnum, uint16_t dataslot, int cd, int sectors, int heads, int offset)
{
	uint8_t drv = (drvnum & 1);
	uint8_t port = (drvnum >> 1);
	mainprintf("drv %d %d", drv, port);
	drive_t *drive = &ide_inst[port].drive[drv];

	ide_inst[port].base = port ? IDE1_BASE : IDE0_BASE;
	ide_inst[port].drive[drv].drvnum = drvnum;
	mainprintf("port %d", port);

	drive->cylinders = 0;
	drive->heads = 0;
	drive->spt = 0;
	drive->spb = 16;
	drive->offset = 0;
	drive->type = 0;
	drive->dataslot = dataslot;
	drive->present = dataslot_size(dataslot) != 0;
	ide_inst[port].state = IDE_STATE_RESET;
	ide_inst[port].bitoff = port * 3;

	drive->cd = drive->present && cd;
	drive->Size = dataslot_size(dataslot);

	drive->placeholder = drive->allow_placeholder;
	if (drive->placeholder && drive->present && !drive->cd) drive->placeholder = 0;
	if (drive->placeholder) drive->cd = 1;

	ide_reg_set(&ide_inst[port], 6, ((drive->present || drive->placeholder) ? 9 : 8) << (drv * 4));
	ide_reg_set(&ide_inst[port], 6, 0x200);
	drive->total_sectors = (drive->Size / 512);

		if (drive->present)
		{
			ide_set_geometry(drive, sectors, heads);
			if (offset && drive->cylinders < 65535) drive->cylinders++;
			drive->offset = offset;
			drive->id[0] = 0x0040; 											//word 0
			drive->id[1] = drive->cylinders;									//word 1
			drive->id[3] = drive->heads;										//word 3
			drive->id[6] = drive->spt;											//word 6
			drive->id[10] = ('A' << 8) | 'O';									//word 10
			drive->id[11] = ('H' << 8) | 'D';									//word 11
			drive->id[12] = ('0' << 8) | '0';									//word 12
			drive->id[13] = ('0' << 8) | '0';									//word 13
			drive->id[14] = ('0' << 8) | ' ';									//word 14
			drive->id[15] = (' ' << 8) | ' ';									//word 15
			drive->id[16] = (' ' << 8) | ' ';									//word 16
			drive->id[17] = (' ' << 8) | ' ';									//word 17
			drive->id[18] = (' ' << 8) | ' ';									//word 18
			drive->id[19] = (' ' << 8) | ' ';									//word 19
			drive->id[20] = 3;   												//word 20 buffer type
			drive->id[21] = 512;												//word 21 cache size
			drive->id[22] = 4;													//word 22 number of ecc bytes
			drive->id[27] = ('A' << 8) | 'M';									//words 27..46 model number
			drive->id[28] = ('I' << 8) | 'G';
			drive->id[29] = ('A' << 8) | ' ';
			drive->id[30] = ('H' << 8) | 'A';
			drive->id[31] = ('R' << 8) | 'D';
			drive->id[32] = ('D' << 8) | 'R';
			drive->id[33] = ('I' << 8) | 'V';
			drive->id[34] = ('E' << 8) | ' ';
			drive->id[35] = (port << 8) | drv;
			drive->id[36] = (' ' << 8) | ' ';
			drive->id[37] = (' ' << 8) | ' ';
			drive->id[38] = (' ' << 8) | ' ';
			drive->id[39] = (' ' << 8) | ' ';
			drive->id[40] = (' ' << 8) | ' ';
			drive->id[41] = (' ' << 8) | ' ';
			drive->id[42] = (' ' << 8) | ' ';
			drive->id[43] = (' ' << 8) | ' ';
			drive->id[44] = (' ' << 8) | ' ';
			drive->id[45] = (' ' << 8) | ' ';
			drive->id[46] = (' ' << 8) | ' ';
			drive->id[47] = 0x8020;												//word 47 max multiple sectors
			drive->id[48] = 1;													//word 48 dword io
			drive->id[49] = 1 << 9;												//word 49 lba supported
			drive->id[50] = 0x4001;												//word 50 reserved
			drive->id[51] = 0x0200;												//word 51 pio timing
			drive->id[52] = 0x0200;												//word 52 pio timing
			drive->id[53] = 0x0007;												//word 53 valid fields
			drive->id[54] = drive->cylinders; 									//word 54
			drive->id[55] = drive->heads;										//word 55
			drive->id[56] = drive->spt;											//word 56
			drive->id[57] = (uint16_t)(drive->total_sectors & 0xFFFF);			//word 57
			drive->id[58] = (uint16_t)(drive->total_sectors >> 16);				//word 58
			drive->id[59] = 0x110;												//word 59 multiple sectors
			drive->id[60] = (uint16_t)(drive->total_sectors & 0xFFFF);			//word 60 LBA-28
			drive->id[61] = (uint16_t)(drive->total_sectors >> 16);				//word 61 LBA-28
			drive->id[65] = 120;
			drive->id[66] = 120;
			drive->id[67] = 120;
			drive->id[68] = 120;									//word 65..68
			drive->id[80] = 0x007E;												//word 80 ata modes
			drive->id[82] = (1 << 14) | (1 << 9); 								//word 82 supported commands
			drive->id[83] = (1 << 14) | (1 << 13) | (1 << 12);					//word 83
			drive->id[84] = 1 << 14;	    									//word 84
			drive->id[85] = (1 << 14) | (1 << 9);  								//word 85
			drive->id[86] = (1 << 14) | (1 << 13) | (1 << 12);					//word 86
			drive->id[87] = 1 << 14;	    									//word 87
			drive->id[93] = (1 << 14) | (1 << 13) | (1 << 9) | (1 << 8) | (1 << 3) | (1 << 1) | (1 << 0); //word 93
			drive->id[100] = (uint16_t)(drive->total_sectors & 0xFFFF);			//word 100 LBA-48
			drive->id[101] = (uint16_t)(drive->total_sectors >> 16);				//word 101 LBA-48

		}

			
	
	mainprintf("HDD%d:  present %d  hd_cylinders %d  hd_heads %d  hd_spt %d  hd_total_sectors %d\r\n", drvnum, drive->present, drive->cylinders, drive->heads, drive->spt, drive->total_sectors);
}

static uint32_t get_lba(ide_config *ide)
{
	uint32_t lba;
	if (ide->regs.lba)
	{
		lba = ide->regs.sector | (ide->regs.cylinder << 8) | (ide->regs.head << 24);
	}
	else
	{
		drive_t *drive = &ide->drive[ide->regs.drv];
		// mainprintf("  CHS: %d/%d/%d (%d/%d)\r\n", ide->regs.cylinder, ide->regs.head, ide->regs.sector, drive->heads, drive->spt);
		lba = ide->regs.cylinder;
		lba *= drive->heads;
		lba += ide->regs.head;
		lba *= drive->spt;
		lba += ide->regs.sector - 1;
	}

	// mainprintf("  LBA: %u\r\n", lba);
	return lba;
}

static void put_lba(ide_config *ide, uint32_t lba)
{
	lba--;
	// mainprintf("  putLBA: %u\r\n", lba);
	if (ide->regs.lba)
	{
		ide->regs.sector = lba;
		lba >>= 8;
		ide->regs.cylinder = lba;
		lba >>= 16;
		ide->regs.head = lba & 0xF;
	}
	else
	{
		drive_t *drive = &ide->drive[ide->regs.drv];
		uint32_t hspt = drive->heads * drive->spt;
		ide->regs.cylinder = lba / hspt;
		lba = lba % hspt;
		ide->regs.head = lba / drive->spt;
		lba = lba % drive->spt;
		ide->regs.sector = lba + 1;
	}
}

inline uint16_t get_cnt(ide_config *ide)
{
	drive_t *drive = &ide->drive[ide->regs.drv];
	// mainprintf("  Cnt: %d (max = %d)\r\n", ide->regs.sector_count, drive->spb);
	uint16_t cnt = ide->regs.sector_count;
	if (!cnt || cnt > drive->spb)
	{
		cnt = drive->spb;
		// mainprintf("  New cnt: %d\r\n", cnt);
	}
	return cnt;
}

inline int readhdd(drive_t *drive, uint32_t lba, int cnt)
{
	if (lba < drive->offset)
	{
		// memset(sector_buffer, 0, sizeof(sector_buffer));
		mainprintf("FEDDSD UPS\r\n");
		return 1;
	}
	else
	{
		// dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length);
		return dataslot_read(drive-> dataslot, (uint32_t)&sector_buffer, lba << 9, cnt * 512);
		// return FileReadAdv(drive->dataslot, (uint32_t)&sector_buffer, cnt * 512, -1);
	}
}



static void process_read(ide_config *ide, int multi)
{
	uint32_t lba = get_lba(ide);
	uint16_t ide_req = 0;

	mainprintf("  sector_count: %d\r\n", ide->regs.sector_count);

	uint32_t cnt = multi ? get_cnt(ide) : 1;
	ide->null = readhdd(&ide->drive[ide->regs.drv], lba, cnt);
	// if (!ide->null) ide->null = (readhdd(&ide->drive[ide->regs.drv], lba, cnt) <= 0);
	if (ide->null) memset(sector_buffer, 0, cnt * 512);

	while (1)
	{
		lba += cnt;
		ide->regs.sector_count -= cnt;
		put_lba(ide, lba);
		mainprintf("  loop: %d\r\n", ide->regs.sector_count);
		ide->regs.io_size = cnt;
		ide->regs.status = ATA_STATUS_RDP | ATA_STATUS_RDY | ATA_STATUS_DRQ | ATA_STATUS_IRQ;
		if (!ide->regs.sector_count) ide->regs.status |= ATA_STATUS_END;

		if (ide->regs.io_fast)
		{
			ide_set_regs(ide);
			ide_send_data(sector_buffer, cnt * 256, 1);
		}
		else
		{
			ide_send_data(sector_buffer, cnt * 256, 1);
			ide->regs.status &= ~ATA_STATUS_RDP;
			ide_set_regs(ide);
		}

		if (!ide->regs.sector_count)
		{
			//ATA_STATUS_END will set ATA_STATUS_RDY at the end
			ide->state = IDE_STATE_IDLE;
			break;
		}

		cnt = multi ? get_cnt(ide) : 1;
		ide->null = readhdd(&ide->drive[ide->regs.drv], lba, cnt);
		if (ide->null) memset(sector_buffer, 0, cnt * 512);

		ide_req = 0;
		while (!ide_req) ide_req = (ide_check() >> ide->bitoff) & 7;

		if (ide_req != 5)
		{
			ide->state = IDE_STATE_IDLE;
			break;
		}
	}

	mainprintf("  finish\r\n");
}

inline int writehdd(drive_t *drive, uint32_t lba, int cnt)
{
	if (lba < drive->offset)
	{
		return 1;
	}
	else
	{
		return dataslot_write(drive-> dataslot, (uint32_t)&sector_buffer, lba << 9, cnt * 512);
	}
}

static void process_write(ide_config *ide, int multi)
{
	uint32_t lba = get_lba(ide);
	uint32_t cnt = 1;
	uint16_t ide_req;
	
	uint8_t irq = 0;

	while (1)
	{
		cnt = multi ? get_cnt(ide) : 1;
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_DRQ | irq;
		irq = ATA_STATUS_IRQ;

		ide->regs.io_size = cnt;
		ide_set_regs(ide);

		ide_req = 0;
		while (!ide_req) ide_req = (ide_check() >> ide->bitoff) & 7;

		if (ide_req != 5)
		{
			ide->state = IDE_STATE_IDLE;
			break;
		}

		ide_recv_data(sector_buffer, cnt * 256, 1);

		if (ide->regs.cmd == 0xFA)
		{
			ide->regs.sector_count = 0;
			int drvnum = (ide->regs.head == 1) ? 0 : (ide->regs.head == 2) ? 1 : (ide->drive[ide->regs.drv].drvnum + 2);

			static const char* names[6] = { "fdd0", "fdd1", "ide00", "ide01", "ide10", "ide11" };
		}
		else
		{
			writehdd(&ide->drive[ide->regs.drv], lba, cnt);
			lba += cnt;
			ide->regs.sector_count -= cnt;
			put_lba(ide, lba);
		}

		if (!ide->regs.sector_count)
		{
			ide->state = IDE_STATE_IDLE;
			ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_IRQ;
			ide_set_regs(ide);
			break;
		}
	}
}

static int handle_hdd(ide_config *ide)
{
	switch (ide->regs.cmd)
	{
	case 0xEC: // identify
		{
			uint8_t drv = ide->regs.drv;
			memset(&ide->regs, 0, sizeof(ide->regs));
			ide->regs.drv = drv;
		}
		ide->regs.io_size = 1;
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_DRQ | ATA_STATUS_IRQ | ATA_STATUS_END;
		ide_send_data(ide->drive[ide->regs.drv].id, 256, 0);
		ide_set_regs(ide);
		break;

	case 0xC4: // read multiple
		process_read(ide, 1);
		break;

	case 0x20: // read with retry
	case 0x21: // read
		process_read(ide, 0);
		break;

	case 0xC5: // write multiple
		process_write(ide, 1);
		break;

	case 0x30: // write with retry
	case 0x31: // write
		process_write(ide, 0);
		break;

	case 0xFA: // mount image
		ide->regs.pkt_io_size = 256;
		process_write(ide, 0);
		break;

	case 0xC6: // set multople
		if (ide->regs.sector_count > ide_io_max_size)
		{
			return 1;
		}
		ide->drive[ide->regs.drv].spb = ide->regs.sector_count;
		mainprintf("New block size: %d\r\n", ide->drive[ide->regs.drv].spb);
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_IRQ;
		ide_set_regs(ide);
		break;

	case 0x08: // reset (fail)
		mainprintf("Reset command (08h) for HDD not supported\r\n");
		return 1;

	case 0x10: 
	case 0x11: 
	case 0x12: 
	case 0x13: 
	case 0x14: 
	case 0x15: 
	case 0x16: 
	case 0x17: 
	case 0x18: 
	case 0x19: 
	case 0x1a: 
	case 0x1b: 
	case 0x1c: 
	case 0x1d: 
	case 0x1e: 
	case 0x1f: // recalibrate
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_IRQ;
		ide->regs.cylinder = 0;
		ide_set_regs(ide);
		break;

	case 0x40: // READ VERIFY
		mainprintf("Received read verify command. Not implemented but returning OK.\r\n");
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_IRQ;
		ide_set_regs(ide);
		break;

	case 0x91: // initialize device parameters
		ide_set_geometry(&ide->drive[ide->regs.drv], ide->regs.sector_count, ide->regs.head + 1);
		ide->regs.status = ATA_STATUS_RDY | ATA_STATUS_IRQ;
		ide_set_regs(ide);
		break;

	default:
		mainprintf("(!) Unsupported command (%04X)\r\n", ide->base);
		return 1;
	}

	return 0;
}

void ide_io(int num, int req)
{
	ide_config *ide = &ide_inst[num];
	
	
	if (req == 0) // no request
	{
		if (num == 1) mainprintf("IDE Pointer  %08X \r\n", req);
		if (ide_inst[num].state == IDE_STATE_RESET)
		{
			ide_inst[num].state = IDE_STATE_IDLE;

			ide_inst[num].regs.status = ATA_STATUS_RDY;
			
			ide_set_regs(&ide_inst[num]);
		}
	}
	else if (req == 4) // command
	{
		if (num == 1) mainprintf("IDE Pointer  %08X \r\n", req);
		ide_inst[num].state = IDE_STATE_IDLE;
		ide_get_regs(&ide_inst[num]);
		mainprintf("IDE command: %d %02X (on %d)\r\n", num, ide_inst[num].regs.cmd, ide_inst[num].regs.drv);

		int err = 0;

		if(ide_inst[num].regs.cmd == 0xFA) err = handle_hdd(&ide_inst[num]);
		else if (!ide_inst[num].drive[ide_inst[num].regs.drv].present) err = 1;
		else err = handle_hdd(&ide_inst[num]);
		if (err)
		{
			ide_inst[num].regs.status = ATA_STATUS_RDY | ATA_STATUS_ERR | ATA_STATUS_IRQ;
			ide_inst[num].regs.error = ATA_ERR_ABRT;
			ide_set_regs(&ide_inst[num]);
		}
	}
	else if (req == 5) // data request
	{
		if (num == 1) mainprintf("IDE Pointer  %08X \r\n", req);
		mainprintf("(!) IDE unknown state!\r\n");
		ide_inst[num].state = IDE_STATE_IDLE;
		ide_inst[num].regs.status = ATA_STATUS_RDY | ATA_STATUS_ERR | ATA_STATUS_IRQ;
		ide_inst[num].regs.error = ATA_ERR_ABRT;
		ide_set_regs(&ide_inst[num]);
		
	}
	else if (req == 6) // reset
	{
		if (ide_inst[num].state != IDE_STATE_RESET)
		{
			mainprintf("IDE %04X reset start\r\n", ide_inst[num].base);
		}

		ide_inst[num].drive[0].playing = 0;
		ide_inst[num].drive[0].paused = 0;
		ide_inst[num].drive[1].playing = 0;
		ide_inst[num].drive[1].paused = 0;

		ide_get_regs(ide);
		ide_inst[num].regs.head = 0;
		ide_inst[num].regs.error = 0;
		ide_inst[num].regs.sector = 1;
		ide_inst[num].regs.sector_count = 1;
		ide_inst[num].regs.cylinder = (!ide_inst[num].drive[ide_inst[num].regs.drv].present) ? 0xFFFF : ide_inst[num].drive[ide_inst[num].regs.drv].cd ? 0xEB14 : 0x0000;
		if (ide_inst[num].drive[ide_inst[num].regs.drv].placeholder) ide_inst[num].regs.cylinder = 0xEB14;
		ide_inst[num].regs.status = ATA_STATUS_BSY;
		ide_set_regs(&ide_inst[num]);

		ide_inst[num].state = IDE_STATE_RESET;
	}
}

int ide_is_placeholder(int num)
{
	return ide_inst[num / 2].drive[num & 1].placeholder;
}

void ide_reset(uint8_t hotswap[4])
{
	ide_inst[0].drive[0].placeholder = 0;
	ide_inst[0].drive[1].placeholder = 0;
	ide_inst[1].drive[0].placeholder = 0;
	ide_inst[1].drive[1].placeholder = 0;

	ide_inst[0].drive[0].allow_placeholder = hotswap[0];
	ide_inst[0].drive[1].allow_placeholder = hotswap[1];
	ide_inst[1].drive[0].allow_placeholder = hotswap[2];
	ide_inst[1].drive[1].allow_placeholder = hotswap[3];
}

int ide_open(uint8_t unit, 	uint16_t dataslot)
{
	// hdfTYPE *hdf = &HDF[unit];
	mainprintf("HDF[unit] %08X \r\n", &HDF[unit]);
	HDF[unit].unit = unit;
	HDF[unit].enabled = 1;
	HDF[unit].dataslot = dataslot;
	HDF[unit].size = dataslot_size(dataslot);
	mainprintf("\nChecking HDD %d\r\n", unit);
	guess_geometry(&HDF[unit]);
	mainprintf("size: %d (%d MB)\r\n", HDF[unit].size, HDF[unit].size >> 20);
	mainprintf("hdf: %u/%u/%u", HDF[unit].cylinders, HDF[unit].heads, HDF[unit].sectors);
	mainprintf(" (%d MB), ", ((((uint32_t)HDF[unit].cylinders) * HDF[unit].heads * HDF[unit].sectors) >> 11));
	mainprintf("Offset: %d\r\n", HDF[unit].offset);


	int present = 1;
	int cd = 0;

	ide_img_set(unit, dataslot, cd, HDF[unit].sectors, HDF[unit].heads, -HDF[unit].offset);

	return 0;
}
