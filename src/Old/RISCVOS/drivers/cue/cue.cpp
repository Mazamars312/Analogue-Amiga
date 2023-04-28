
#include "cue.h"

int get_cd_seek_ms(int start_sector, int target_sector);

#define CD_DATA_IO_INDEX 2
uint32_t foo1;
cue_t cue;
cue_t::cue_t() {
	latency = 0;
	audiodelay = 0;
	loaded = 0;
	index = 0;
	lba = 0;
	scanOffset = 0;
	isData = 1;
	state = CD_STATE_NODISC;
	audioLength = 0;
	audioOffset = 0;
	SendData = NULL;
	has_status = 0;
	data_req = false;
	can_read_next = false;
	CDDAStart = 0;
	CDDAEnd = 0;
	CDDAMode = CD_CDDAMODE_SILENT;
	region = 0;

	stat = 0x0000;

}

static int sgets(char *out, int sz, int cnt)
{
	*out = 0;
	do
	{
		char *instr = (char*)foo1;
		int cnt = 0;
		while (*instr && *instr != 10)
		{
			if (*instr == 13)
			{
				instr++;
				foo1++;
				continue;
			}
			if (cnt < sz - 1)
			{
				out[cnt++] = *instr;
				out[cnt] = 0;
			}
			instr++;
			foo1++;
		}
		if(*instr == 10) {
			instr++;
			foo1++;
		}
		if(*instr == 00) {
			return *out;
		}
		printf("foo %0.4x cnt %0.4x, %0.4x \r\n", foo1, cnt, &instr);
	}
	while ((foo1 - 0x8000) >= cnt);
	return *out;
}

#define DATASLOT_BRAM_SAVE(x) *(volatile unsigned int *)(DATASLOT_BRAM_BASE+(5<<2))

int cue_t::LoadCUE(int dataslot) {
	static char line[128];
	char *ptr, *lptr;
	int hdr = 0;
	int toc_size;
	toc_size = dataslot_size(100); // This is where the Cue file will be
	bool file_processer = 0;
	if (dataslot_size(102) >= 1) file_processer = 1; // this is to confirm that this is a single or multi file system
	DATASLOT_BRAM_SAVE(0) = (uint32_t)0x00000800;

	foo1 = RAMBUFFER_BASE;
	clearbufferram();

	dataslot_read(dataslot, APF_ADDRESS_OFFSET | foo1, 0, toc_size);
	int mm, ss, bb, pregap = 0;
	while ((sgets(line, sizeof(line), toc_size)))
	{
		lptr = line;
		printf("%s\r\n", line);
		while (*lptr == 0x20) lptr++;

		/* decode FILE commands */
		if (!(memcmp(lptr, "FILE", 4)))
		{

			if (strstr(lptr, ".wav")){
				hdr = 44;
			};
			pregap = 0;

			this->toc.tracks[this->toc.last].offset = 0;

			if (!strstr(lptr, "BINARY") && !strstr(lptr, "MOTOROLA") && !strstr(lptr, "WAVE"))
			{
				return -1;
			}
		}

		/* decode TRACK commands */
		else if ((sscanf(lptr, "TRACK %d %*s", &bb)) || (sscanf(lptr, "TRACK %d %*s", &bb)))
		{
			if (bb != (this->toc.last + 1))
			{
				break;
			}
			else
			{
				if (file_processer){
					this->toc.tracks[this->toc.last].dataslot = (uint16_t)bb + 100;
					this->toc.tracks[this->toc.last].size  = (int)dataslot_size((uint16_t)bb + 100);
					this->toc.tracks[this->toc.last].opened = 0;
				} else {
					this->toc.tracks[this->toc.last].dataslot = 101;
					this->toc.tracks[this->toc.last].size  = (int)dataslot_size(101);
					this->toc.tracks[this->toc.last].opened = 0;
				}
				if (this->toc.tracks[this->toc.last].size > 0) this->toc.tracks[this->toc.last].opened = 1;
				if (strstr(lptr, "MODE1/2048"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2048;
					this->toc.tracks[this->toc.last].type = 1;
				}
				else if (strstr(lptr, "MODE1/2352"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2352;
					this->toc.tracks[this->toc.last].type = 1;
				}
				else if (strstr(lptr, "AUDIO"))
				{
					this->toc.tracks[this->toc.last].sector_size = 2352;
					this->toc.tracks[this->toc.last].type = 0;
				}
			}

			if (this->toc.last)
			{
				if (!this->toc.tracks[this->toc.last].opened)
				{
					this->toc.tracks[this->toc.last - 1].end = 0;
				}
			}
		}

		/* decode PREGAP commands */
		else if (sscanf(lptr, "PREGAP %02d:%02d:%02d", &mm, &ss, &bb) == 3)
		{
			pregap += bb + ss * 75 + mm * 60 * 75;
		}
		/* decode INDEX commands */
		else if ((sscanf(lptr, "INDEX 00 %02d:%02d:%02d", &mm, &ss, &bb) == 3) ||
			(sscanf(lptr, "INDEX 0 %02d:%02d:%02d", &mm, &ss, &bb) == 3))
		{
			if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
			{
				this->toc.tracks[this->toc.last - 1].end = bb + ss * 75 + mm * 60 * 75 + pregap;
			}
		}
		else if ((sscanf(lptr, "INDEX 01 %02d:%02d:%02d", &mm, &ss, &bb) == 3) ||
			(sscanf(lptr, "INDEX 1 %02d:%02d:%02d", &mm, &ss, &bb) == 3))
		{
			if (!this->toc.tracks[this->toc.last].opened)
			{
				this->toc.tracks[this->toc.last].start = bb + ss * 75 + mm * 60 * 75 + pregap;
				this->toc.tracks[this->toc.last].offset = (pregap * this->toc.tracks[this->toc.last].sector_size) - hdr;
				if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
				{
					this->toc.tracks[this->toc.last - 1].end = this->toc.tracks[this->toc.last].start;
				}
			}
			else
			{
				this->toc.tracks[this->toc.last].start = this->toc.end + pregap;
				this->toc.tracks[this->toc.last].offset = (this->toc.tracks[this->toc.last].start * this->toc.tracks[this->toc.last].sector_size) - hdr;
				this->toc.tracks[this->toc.last].end = this->toc.tracks[this->toc.last].start + ((this->toc.tracks[this->toc.last].size - hdr + this->toc.tracks[this->toc.last].sector_size - 1) / this->toc.tracks[this->toc.last].sector_size);

				this->toc.tracks[this->toc.last].start += (bb + ss * 75 + mm * 60 * 75);
				this->toc.end = this->toc.tracks[this->toc.last].end;
			}
			this->toc.last++;
			if (file_processer){
				if (this->toc.last >= 30) break;
			} else {
					if (this->toc.last >= 100) break;
			}
		}
	}

	if (this->toc.last && !this->toc.tracks[this->toc.last - 1].end)
	{
		this->toc.end += pregap;
		this->toc.tracks[this->toc.last - 1].end = this->toc.end;
	}
	for (int i = 0; i < this->toc.last; i++)
	{
		printf("\x1b[32mPCECD: Track = %u, start = %u, end = %u, offset = %d, sector_size=%d, type = %u, dataslot = %u\r\n\x1b[0m", i, this->toc.tracks[i].start, this->toc.tracks[i].end, this->toc.tracks[i].offset, this->toc.tracks[i].sector_size, this->toc.tracks[i].type, this->toc.tracks[i].dataslot);
	}
	printf("Dataslot 0 %d %d \r\n", DATASLOT_BRAM(0), DATASLOT_BRAM(1));
  printf("Dataslot 1 %d %d \r\n", DATASLOT_BRAM(2), DATASLOT_BRAM(3));
  printf("Dataslot 2 %d %d \r\n", DATASLOT_BRAM(4), DATASLOT_BRAM(5));
  printf("Dataslot 3 %d %d \r\n", DATASLOT_BRAM(6), DATASLOT_BRAM(7));
  printf("Dataslot 99 %d %d \r\n", DATASLOT_BRAM(8), DATASLOT_BRAM(9));
  printf("Dataslot 101 %d %d \r\n", DATASLOT_BRAM(10), DATASLOT_BRAM(11));
  // printf("Dataslot 102 %d %d %d %0.4x \r\n", DATASLOT_BRAM(12), DATASLOT_BRAM(13), dataslot, RAMBUFFER_BASE);
	clearbufferram();
	return 1;
}

int cue_t::Load(int dataslot)
{
	// Unload();

	if (LoadCUE(dataslot)){
		this->loaded = 1;
		return 1;
	} else {
		this->loaded = 0;
		return 0;
	}
return 0;
}

void cue_t::Unload()
{
	if (this->loaded)
	{


		// this->toc.tracks = 0;


		for (int i = 0; i < this->toc.last; i++)
			{
				this->toc.tracks[i].dataslot = 0;
				this->toc.tracks[i].opened = 0;
				this->toc.tracks[i].size = 0;
				this->toc.tracks[i].offset = 0;
				this->toc.tracks[i].pregap = 0;
				this->toc.tracks[i].start = 0;
				this->toc.tracks[i].end = 0;
				this->toc.tracks[i].type = 0;
				this->toc.tracks[i].sector_size = 0;
				this->toc.tracks[i].index1 = 0;
			}
			this->loaded = 0;
			this->toc.end=0;
			this->toc.last=0;
	}
}

void cue_t::Reset() {
	latency = 0;
	audiodelay = 0;
	index = 0;
	lba = 0;
	scanOffset = 0;
	isData = 1;
	state = loaded ? CD_STATE_IDLE : CD_STATE_NODISC;
	audioLength = 0;
	audioOffset = 0;
	has_status = 0;
	data_req = false;
	can_read_next = false;
	CDDAStart = 0;
	CDDAEnd = 0;
	CDDAMode = CD_CDDAMODE_SILENT;
	this->toc.last = 0;
	stat = 0x0000;

}

void cue_t::Update() {
	if (this->state == CD_STATE_READ)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}

		if (this->index >= this->toc.last)
		{
			this->state = CD_STATE_IDLE;
			return;
		}

		if (!this->can_read_next)
			return;

		this->can_read_next = false;

		if (this->toc.tracks[this->index].type)
		{
			// CD-ROM (Mode 1)
			ReadData();
		}
		else
		{
			if (this->lba >= this->toc.tracks[this->index].start)
			{
				this->isData = 0x00;
			}

		}

		this->cnt--;

		if (!this->cnt) {
			PendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
			this->state = CD_STATE_IDLE;
		}
		else {

		}

		this->lba++;
		if (this->lba >= this->toc.tracks[this->index].end)
		{
			this->index++;

			this->isData = 0x01;

			if (this->toc.tracks[this->index].opened)
			{
				RISCFileSeek(this->toc.tracks[this->index].dataslot, (this->toc.tracks[this->index].start * 2352) - this->toc.tracks[this->index].offset, 2352, 0);
			}
		}
	}
	else if (this->state == CD_STATE_PLAY)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}

		if (this->audiodelay > 0)
		{
			this->audiodelay--;
			return;
		}

		this->index = GetTrackByLBA(this->lba, &this->toc);

		for (int i = 0; i <= this->CDDAFirst; i++)
		{
			if (!this->toc.tracks[this->index].type)
			{
				if (this->toc.tracks[this->index].opened)
				{
					RISCFileSeek(this->toc.tracks[index].dataslot, (this->lba * 2352) - this->toc.tracks[index].offset, 2352, 0);
				}
				ReadCDDA();
			}
			this->lba++;
		}

		this->CDDAFirst = 0;

		if ((this->lba > this->CDDAEnd) || this->toc.tracks[this->index].type || this->index > this->toc.last)
		{
			if (this->CDDAMode == CD_CDDAMODE_LOOP) {
				this->lba = this->CDDAStart;
			}
			else {
				this->state = CD_STATE_IDLE;
			}
			if (this->CDDAMode == CD_CDDAMODE_INTERRUPT) {
				SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
			}
			//riscprintf("\x1b[32mPCECD: playback reached the end %d\r\n\x1b[0m", this->lba);
		}
	}
	else if (this->state == CD_STATE_PAUSE)
	{
		if (this->latency > 0)
		{
			this->latency--;
			return;
		}
	}
}

void cue_t::CommandExec() {
	msf_t msf;
	int new_lba = 0;
	static uint8_t buf[32];
	uint32_t temp_latency;

	memset(buf, 0, 32);
	switch (comm[0]) {
	case CD_COMM_TESTUNIT:
		if (state == CD_STATE_NODISC) {
			CommandError(SENSEKEY_NOT_READY, NSE_NO_DISC, 0, 0);
			SendStatus(MAKE_STATUS(CD_STATUS_CHECK_COND, 0));
		}
		else {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

		break;

	case CD_COMM_REQUESTSENSE:
		buf[0] = 18;
		buf[1] = 0 | 0x80;

		buf[2] = 0x70;
		buf[4] = sense.key;
		buf[9] = 0x0A;
		buf[14] = sense.asc;
		buf[15] = sense.ascq;
		buf[16] = sense.fru;

		sense.key = sense.asc = sense.ascq = sense.fru = 0;

		if (SendData)
			SendData(buf, 18 + 2, CD_DATA_IO_INDEX);

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));

		break;

	case CD_COMM_GETDIRINFO: {
		int len = 0;
		switch (comm[1]) {
		case 0:
		default:
			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = 1;
			buf[3] = BCD(this->toc.last);
			buf[4] = 0;
			buf[5] = 0;
			len = 4 + 2;
			break;
		case 1:
			new_lba = this->toc.end + 150;
			LBAToMSF(new_lba, &msf);

			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = BCD(msf.m);
			buf[3] = BCD(msf.s);
			buf[4] = BCD(msf.f);
			buf[5] = 0;
			len = 4 + 2;
			break;

		case 2:
			int track = U8(comm[2]);
			new_lba = this->toc.tracks[track - 1].start + 150;
			LBAToMSF(new_lba, &msf);

			buf[0] = 4;
			buf[1] = 0 | 0x80;
			buf[2] = BCD(msf.m);
			buf[3] = BCD(msf.s);
			buf[4] = BCD(msf.f);
			buf[5] = this->toc.tracks[track - 1].type << 2;
			len = 4 + 2;
			break;
		}

		if (SendData && len)
			SendData(buf, len, CD_DATA_IO_INDEX);

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;


	case CD_COMM_MODESELECT6:
		if (comm[4]) {
			data_req = true;
		}
		else {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

		break;

	case CD_COMM_SAPSP: {
		switch (comm[9] & 0xc0)
		{
		default:
		case 0x00:
			new_lba = (comm[3] << 16) | (comm[4] << 8) | comm[5];
			break;

		case 0x40:
			MSFToLBA(&new_lba, U8(comm[2]), U8(comm[3]), U8(comm[4]));
			break;

		case 0x80:
		{
			int track = U8(comm[2]);

			if (!track)
				track = 1;
			else if (track > toc.last)
				track = toc.last;
			new_lba = this->toc.tracks[track - 1].start;
		}
		break;
		}

		if (comm[13] & 0x80) // fast seek (OSD setting)
		{
			this->latency = 0;
			this->audiodelay = 0;
		}
		else
		{
			temp_latency = (get_cd_seek_ms(this->lba, new_lba) / 13);
			this->audiodelay = (int)(220 / 13);

			if (temp_latency > this->audiodelay)
				this->latency = temp_latency - this->audiodelay;
			else {
				this->latency = temp_latency;
				this->audiodelay = 0;
			}
		}

		this->lba = new_lba;
		int index = GetTrackByLBA(new_lba, &this->toc);

		this->index = index;

		this->CDDAStart = new_lba;
		this->CDDAEnd = this->toc.end;
		this->CDDAMode = comm[1];
		this->CDDAFirst = 1;

		if (this->CDDAMode == CD_CDDAMODE_SILENT) {
			this->state = CD_STATE_PAUSE;
		}
		else {
			this->state = CD_STATE_PLAY;
		}

		PendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	case CD_COMM_SAPEP: {
		switch (comm[9] & 0xc0)
		{
		default:
		case 0x00:
			new_lba = (comm[3] << 16) | (comm[4] << 8) | comm[5];
			break;

		case 0x40:
			MSFToLBA(&new_lba, U8(comm[2]), U8(comm[3]), U8(comm[4]));
			break;

		case 0x80:
		{
			int track = U8(comm[2]);

			if (!track)	track = 1;
			new_lba = ((track-1) >= toc.last) ? this->toc.end : (this->toc.tracks[track - 1].start);
		}
		break;
		}

		this->CDDAMode = comm[1];
		this->CDDAEnd = new_lba;

		if (this->CDDAMode == CD_CDDAMODE_SILENT) {
			this->state = CD_STATE_IDLE;
		}
		else {
			this->state = CD_STATE_PLAY;
		}

		if (this->CDDAMode != CD_CDDAMODE_INTERRUPT) {
			SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
		}

	}
		break;

	case CD_COMM_PAUSE: {
		this->state = CD_STATE_PAUSE;

		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	case CD_COMM_READSUBQ: {
		int lba_rel = this->lba - this->toc.tracks[this->index].start;

		buf[0] = 0x0A;
		buf[1] = 0 | 0x80;
		buf[2] = this->state == CD_STATE_PAUSE ? 2 : (this->state == CD_STATE_PLAY ? 0 : 3);
		buf[3] = 0;
		buf[4] = BCD(this->index + 1);
		buf[5] = BCD(this->index);

		LBAToMSF(lba_rel, &msf);
		buf[6] = BCD(msf.m);
		buf[7] = BCD(msf.s);
		buf[8] = BCD(msf.f);

		LBAToMSF(this->lba+150, &msf);
		buf[9] = BCD(msf.m);
		buf[10] = BCD(msf.s);
		buf[11] = BCD(msf.f);

		if (SendData)
			SendData(buf, 10 + 2, CD_DATA_IO_INDEX);
		SendStatus(MAKE_STATUS(CD_STATUS_GOOD, 0));
	}
		break;

	default:
		CommandError(SENSEKEY_ILLEGAL_REQUEST, NSE_INVALID_COMMAND, 0, 0);
		has_status = 0;
		SendStatus(MAKE_STATUS(CD_STATUS_CHECK_COND, 0));
		break;
	}
}

uint16_t cue_t::GetStatus() {
	return stat;
}

int cue_t::SetCommand(uint8_t* buf) {
	memcpy(comm, buf, 14);
	return 0;
}

void cue_t::PendStatus(uint16_t status) {
	stat = status;
	has_status = 1;
}

void cue_t::SendStatus(uint16_t status) {
	HPS_spi_uio_cmd_cont(UIO_CD_SET);
	spi_w(status);
	spi_w(region ? 2 : 0);
	HPS_DisableIO();
}

void cue_t::SendDataRequest() {
	HPS_spi_uio_cmd_cont(UIO_CD_SET);
	spi_w(0);
	spi_w((region ? 2 : 0) | 1);
	HPS_DisableIO();
}

void cue_t::SetRegion(uint8_t rgn) {
	region = rgn;
}

void cue_t::LBAToMSF(int lba, msf_t* msf) {
	msf->m = (lba / 75) / 60;
	msf->s = (lba / 75) % 60;
	msf->f = (lba % 75);
}

void cue_t::MSFToLBA(int* lba, uint8_t m, uint8_t s, uint8_t f) {
	*lba = f + s * 75 + m * 60 * 75 - 150;
}

void cue_t::MSFToLBA(int* lba, msf_t* msf) {
	*lba = msf->f + msf->s * 75 + msf->m * 60 * 75 - 150;
}

int cue_t::GetTrackByLBA(int lba, toc_t* toc) {
	int index = 0;
	while ((toc->tracks[index].end <= lba) && (index < toc->last)) index++;
	return index;
}

void cue_t::ReadData()
{
	if (this->toc.tracks[this->index].type && (this->lba >= 0))
	{
			if (this->toc.tracks[this->index].sector_size == 2048)
			{
				RISCFileSeek(this->toc.tracks[this->index].dataslot, this->lba * 2048 - this->toc.tracks[this->index].offset, 2048, 0);
				RISCFileReadAdv(0x00,(0x08 | 0x80),UIO_CD_DATA, 2048);
			} else {
				RISCFileSeek(this->toc.tracks[this->index].dataslot, this->lba * 2352 + 16 - this->toc.tracks[this->index].offset, 2352, 0);
				RISCFileReadAdv(0x00,(0x08 | 0x80),UIO_CD_DATA, 2352);
			}
	}
}

int cue_t::ReadCDDA()
{
	this->audioLength = 2352;// 2352 + 2352 - this->audioOffset;
	this->audioOffset = 0;// 2352;

	if (this->toc.tracks[this->index].opened) {
		RISCFileReadAdv(0x30,0x09,UIO_CD_DATA, this->audioLength);
	}

	return this->audioLength;
}

void cue_t::ReadSubcode(uint16_t* buf)
{
	(void)buf;
	/*
	uint8_t subc[96];
	int i, j, n;
	fread(subc, 96, 1, this->toc.sub);
	for (i = 0, n = 0; i < 96; i += 2, n++)
	{
		int code = 0;
		for (j = 0; j < 8; j++)
		{
			int bits = (subc[(j * 12) + (i / 8)] >> (6 - (i & 6))) & 3;
			code |= ((bits & 1) << (7 - j));
			code |= ((bits >> 1) << (15 - j));
		}
		buf[n] = code;
	}
	*/
}

void cue_t::CommandError(uint8_t key, uint8_t asc, uint8_t ascq, uint8_t fru) {
	sense.key = key;
	sense.asc = asc;
	sense.ascq = ascq;
	sense.fru = fru;
}
