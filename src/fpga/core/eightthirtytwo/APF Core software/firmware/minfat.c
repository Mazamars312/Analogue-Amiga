/*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski
Copyright 2021 Alastair M. Robinson

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

This is a simple FAT16/FAT32 handler. It works on a sector basis to allow fastest acces on disk
images.

11-12-2005 - first version, ported from FAT1618.C

JB:
2008-10-11  - added SeekFile() and cluster_mask
            - limited file create and write support added
2009-05-01  - modified LoadDirectory() and GetDirEntry() to support sub-directories (with limitation of 511 files/subdirs per directory)
            - added GetFATLink() function
            - code cleanup
2009-05-03  - modified sorting algorithm in LoadDirectory() to display sub-directories above files
2009-08-23  - modified ScanDirectory() to support page scrolling and parent dir selection
2009-11-22  - modified FileSeek()
            - added FileReadEx()
2009-12-15  - all entries are now sorted by name with extension
            - directory short names are displayed with extensions

2012-07-24  - Major changes to fit the MiniSOC project - AMR
2021-02-20  - Adapted for the MiST firmware replacement project.  Directory-handling fixes  -  AMR
*/

#define NULL 0
#include <sys/types.h>
// #include <stdio.h>
#include <string.h>
//#include <ctype.h>

#include "spi.h"
#include "spi_sd.h"

#include "swap_le.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"

#include "minfat.h"

#define tolower(x) (x|32)

// internal global variables
unsigned int fat32;                // volume format is FAT32
uint32_t fat_start;                // start LBA of first FAT table
uint32_t data_start;               // start LBA of data field
uint32_t root_directory_cluster;   // root directory cluster (used in FAT32)
uint32_t root_directory_start;     // start LBA of directory table
uint32_t root_directory_size;      // size of directory region in sectors
unsigned int cluster_size;             // size of a cluster in sectors
uint32_t cluster_mask;             // binary mask of cluster number
unsigned int dir_entries;             // number of entry's in directory table

uint32_t cachedsector=-1;

uint32_t current_directory_cluster;
uint32_t current_directory_start;

unsigned char sector_buffer[512];       // sector buffer
#ifndef DISABLE_LONG_FILENAMES
char longfilename[261];
#endif

//unsigned char *sector_buffer=0x18000;

//struct PartitionEntry partitions[4]; 	// [4];	// lbastart and sectors will be byteswapped as necessary

#define fat_buffer (*(FATBUFFER*)&sector_buffer) // Don't need a separate buffer for this.
// uint32_t buffered_fat_index;       // index of buffered FAT sector


//#define STATUS(x) puts(x);
#define STATUS(x)

#undef FAT_DEBUG

#ifdef FAT_DEBUG
#define DBG(x) printf(x)
#define PDBG(x,y) printf(x,y)
#else
#define DBG(x)
#define PDBG(x,y)
#endif

const char* FAT32_ID="FAT32   ";
const char* FAT16_ID="FAT16   ";

// FindDrive() checks if a card is present and contains FAT formatted primary partition
unsigned int FindDrive(void)
{
	uint32_t fat_number;
	uint32_t fat_size;
	uint32_t boot_sector;
	int partitioncount;

	fat32=0;

	STATUS("Reading MBR\n");

    if (!sd_read_sector(0, sector_buffer)) // read MBR
	{
		STATUS("Read of MBR failed\n");
        return(0);
	}

	STATUS("MBR successfully read\n");

	boot_sector=0;
	partitioncount=1;

	// If we can identify a filesystem on block 0 we don't look for partitions
    if (strncmp((const char*)&sector_buffer[0x36], FAT16_ID,8)==0) // check for FAT16
		partitioncount=0;
    if (strncmp((const char*)&sector_buffer[0x52], FAT32_ID,8)==0) // check for FAT32
		partitioncount=0;

	PDBG("Partitioncount %d\n",partitioncount);

	if(partitioncount)
	{
		// We have at least one partition, parse the MBR.
		struct MasterBootRecord *mbr=(struct MasterBootRecord *)sector_buffer;
		struct PartitionEntry *pe=(struct PartitionEntry *)&mbr->Partition[0][0];

		boot_sector = pe->startlba;
		if(mbr->Signature==0x55aa)
				boot_sector=ConvBBBB_LE(pe->startlba);
		else if(mbr->Signature!=0xaa55)
		{
			STATUS("No part sig");
			return(0);
		}
		PDBG("Reading boot sector %d\n",boot_sector);
		if (!sd_read_sector(boot_sector, sector_buffer)) // read discriptor
		    return(0);
//		hexdump(sector_buffer,512);
		STATUS("Boot sector...");
	}

	STATUS("Seeking FS...");

    if (strncmp(sector_buffer+0x52, FAT32_ID,8)==0) // check for FAT16
		fat32=1;
	else if (strncmp(sector_buffer+0x36, FAT16_ID,8)!=0) // check for FAT32
	{
        STATUS("Unsupported partition type!\r");
		return(0);
	}

    if (sector_buffer[510] != 0x55 || sector_buffer[511] != 0xaa)  // check signature
        return(0);

    // check for near-jump or int16_t-jump opcode
    if (sector_buffer[0] != 0xe9 && sector_buffer[0] != 0xeb)
        return(0);

    // check if blocksize is really 512 bytes
    if (sector_buffer[11] != 0x00 || sector_buffer[12] != 0x02)
        return(0);

    // get cluster_size
    cluster_size = sector_buffer[13];

    // calculate cluster mask
    cluster_mask = cluster_size - 1;

	PDBG("Cluster size: %d\n",cluster_size);
	PDBG("Cluster mask, %d\n",cluster_mask);

    fat_start = boot_sector + sector_buffer[0x0E] + (sector_buffer[0x0F] << 8); // reserved sector count before FAT table (usually 32 for FAT32)
	fat_number = sector_buffer[0x10];

    if (fat32)
    {
        if (strncmp((const char*)&sector_buffer[0x52], FAT32_ID,8) != 0) // check file system type
            return(0);

        dir_entries = cluster_size << 4; // total number of dir entries (16 entries per sector)
        root_directory_size = cluster_size; // root directory size in sectors
        fat_size = sector_buffer[0x24] + (sector_buffer[0x25] << 8) + (sector_buffer[0x26] << 16) + (sector_buffer[0x27] << 24);
        data_start = fat_start + (fat_number * fat_size);
        root_directory_cluster = sector_buffer[0x2C] + (sector_buffer[0x2D] << 8) + (sector_buffer[0x2E] << 16) + ((sector_buffer[0x2F] & 0x0F) << 24);
        root_directory_start = (root_directory_cluster - 2) * cluster_size + data_start;
    }
    else
    {
        // calculate drive's parameters from bootsector, first up is size of directory
        dir_entries = sector_buffer[17] + (sector_buffer[18] << 8);
        root_directory_size = ((dir_entries << 5) + 511) >> 9;

        // calculate start of FAT,size of FAT and number of FAT's
        fat_size = sector_buffer[22] + (sector_buffer[23] << 8);

        // calculate start of directory
        root_directory_start = fat_start + (fat_number * fat_size);
        root_directory_cluster = 0; // unused

        // calculate start of data
        data_start = root_directory_start + root_directory_size;
    }

//	ChangeDirectory(0);
	current_directory_cluster = root_directory_cluster;
	current_directory_start = root_directory_start;
	dir_entries = fat32 ?  cluster_size << 4 : root_directory_size << 4; // 16 entries per sector

    return(1);
}


uint32_t GetCluster(uint32_t cluster)
{
	uint32_t i;
	uint32_t sb;
    if (fat32)
    {
        sb = cluster >> 7; // calculate sector number containing FAT-link
        i = cluster & 0x7F; // calculate link offset within sector
    }
    else
    {
        sb = cluster >> 8; // calculate sector number containing FAT-link
        i = cluster & 0xFF; // calculate link offset within sector
    }

	sb+=fat_start;
	if(cachedsector!=sb)
	{
		cachedsector=sb;
		if (!sd_read_sector(sb, (unsigned char*)&fat_buffer))
		    return(0);
	}

    i = fat32 ? ConvBBBB_LE(fat_buffer.fat32[i]) & 0x0FFFFFFF : ConvBB_LE(fat_buffer.fat16[i]); // get FAT link
	return(i);
}


unsigned int FileOpen(fileTYPE *file, const char *name)
{
    DIRENTRY      *p = NULL;        // pointer to current entry in sector buffer
	int bm;

	if(!file)
		return(0);

	file->size=0;

	/* Reject null or empty filenames, since an empty filename will match a file with no long filename. */
	if(!name || !name[0])
		return(0);

	while(p=NextDirEntry(p==NULL,0))
	{
#ifndef DISABLE_LONG_FILENAMES
		if(strcasecmp(longfilename,name)==0)
			break;
#endif
		if(strncasecmp((const char*)p->Name, name,11)==0)
			break;
	}

	if(p)
	{
		file->size = ConvBBBB_LE(p->FileSize);
		file->cluster = ConvBB_LE(p->StartCluster);
		file->cluster += (fat32 ? (ConvBB_LE(p->HighCluster) & 0x0FFF) << 16 : 0);
		file->sector = 0;
		file->firstcluster=file->cluster;
		file->cursor=0;

#ifdef CONFIG_FILEBOOKMARKS
		for(bm=0;bm<CONFIG_FILEBOOKMARKS;++bm)
		{
			file->bookmarks[bm].sector=0;
			file->bookmarks[bm].cluster=file->cluster;
		}
		file->bookmark_threshold=(file->size>>9)/(8*CONFIG_FILEBOOKMARKS);
#endif

		return(1);
	}

    return(0);
}


void FileNextSector(fileTYPE *file,int count)
{
    uint32_t sb;
    uint16_t i;
	if(!file || !file->size)
		return;
	count+=file->sector;
	while((file->sector ^ count)&~cluster_mask)
	{
		file->cluster=GetCluster(file->cluster);
		file->sector+=cluster_size;
	}
	file->sector=count;
}


unsigned int FileReadSector(fileTYPE *file, unsigned char *pBuffer)
{
    uint32_t sb;

	if(!file || !file->size)
		return(0);

    sb = data_start;                         // start of data in partition
    sb += cluster_size * (file->cluster-2);  // cluster offset
    sb += file->sector & cluster_mask;      // sector offset in cluster
	cachedsector=sb;
    if (!sd_read_sector(sb, pBuffer)) // read sector from drive
        return(0);
    else
        return(1);
}


unsigned int FileWriteSector(fileTYPE *file, unsigned char *pBuffer)
{
    uint32_t sb;

	if(!file || !file->size)
		return(0);

    sb = data_start;                         // start of data in partition
    sb += cluster_size * (file->cluster-2);  // cluster offset
    sb += file->sector & cluster_mask;      // sector offset in cluster
	cachedsector=sb;
    if (!sd_write_sector(sb, pBuffer)) // write sector to drive
        return(0);
    else
        return(1);
}

#ifdef CONFIG_FILEBOOKMARKS

void DumpBookmarks(fileTYPE *file)
{
	int idx;
	if(!file || !file->size)
		return;
	for(idx=0;idx<CONFIG_FILEBOOKMARKS;++idx)
	{
		printf("(Bookmark %d, %x, %x)\n",idx,file->bookmarks[idx].sector,file->bookmarks[idx].cluster);
	}
}


int BestBookmark(fileTYPE *file, uint32_t pm)
{
	int idx,best;
	int32_t bestd,d;
	best=-1;
	bestd=0x7fffffff;
	for(idx=0;idx<CONFIG_FILEBOOKMARKS;++idx)
	{
		d=pm-file->bookmarks[idx].sector;
		if(d>=0 && d<bestd)
		{
			best=idx;
			bestd=d;
		}
	}
	return(best);
}


/* Find the least useful bookmark */
int WorstBookmark(fileTYPE *file)
{
	int idx,idx2;
	uint32_t worstd=0x7fffffff;
	int worst=-1;
	for(idx=0;idx<CONFIG_FILEBOOKMARKS;++idx)
	{
		for(idx2=0;idx2<CONFIG_FILEBOOKMARKS;++idx2)
		{
			int d=file->bookmarks[idx2].sector-file->bookmarks[idx].sector;
			if(idx!=idx2 && (d<worstd))
			{
				worst=idx2;
				worstd=d;
			}
		}
	}
	return(worst);
}


void FileSeek(fileTYPE *file, uint32_t pos)
{
	uint32_t p=pos>>9;
	uint32_t pm=p&~cluster_mask;

	uint32_t currentsector;
	uint32_t cluster;

	if(!file || !file->size)
		return;

	currentsector=file->sector&~cluster_mask;
	cluster=file->cluster;

	if(pm==currentsector)	// Is the new position within the same cluster?
	{
		file->sector=p;
	}
	else	// Crossing a cluster boundary
	{
		int idx;
		idx=BestBookmark(file,pm);
		if(idx>=0)
		{
//			printf("Found bookmark %d for %x (%x, %x)\n",idx,pm,file->bookmarks[idx].sector,file->bookmarks[idx].cluster);
			file->sector=file->bookmarks[idx].sector;
			file->cluster=file->bookmarks[idx].cluster;
		}
		else
		{
//			printf("No bookmark found\n");
			file->sector=0;
			file->cluster=file->firstcluster;
		}

		/* record bookmark */
		p-=file->sector;

		idx=BestBookmark(file,currentsector);

		/* We don't bother bookmarking at the start of the file, or if we're within bookmark_threshold of an existing bookmark */
		if((currentsector>file->bookmark_threshold) && (idx>=0)
			&& ((currentsector-file->bookmarks[idx].sector) > file->bookmark_threshold))
		{
			idx=WorstBookmark(file);
			file->bookmarks[idx].sector=currentsector;
			file->bookmarks[idx].cluster=cluster;
//			file->bookmark_index=file->bookmark_index==CONFIG_FILEBOOKMARKS-1 ? 0 : file->bookmark_index+1;
		}

		FileNextSector(file,p);
	}
	FileReadSector(file, sector_buffer);
	file->cursor=pos;
}
#else
void FileSeek(fileTYPE *file, uint32_t pos)
{
	uint32_t p=pos;
	if(!file || !file->size)
		return;
//	printf("Fseek: %d, %d\n",file->cursor,pos);
	if(p<(file->cursor&(~cluster_mask)))
	{
		file->sector=0;
		file->cursor=0;
		file->cluster=file->firstcluster;
	}
	else
		p-=file->cursor&~511;
	FileNextSector(file,p>>9);
	FileReadSector(file, sector_buffer);
	file->cursor=pos;
}
#endif

unsigned int FileRead(fileTYPE *file, unsigned char *buffer, int count)
{
	unsigned char *p;
	int c;
	uint32_t curs;
	if(!file || !file->size)
		return(0);
	if(count+file->cursor>file->size)
		count=file->size-file->cursor;
	if(count<=0)
		return(0);
	curs=file->cursor&0x1ff;
	if(curs)
	{
		c=512-curs;
		p=sector_buffer+curs;
		if(c>count)
			c=count;
		file->cursor+=c;
		count-=c;
		while(c--)
			*buffer++=*p++;
		FileNextSector(file,1);
	}
	while(count>0)
	{
		if(count>511)
		{
			FileReadSector(file, buffer);
			buffer+=512;
			file->cursor+=512;
			count-=512;
			FileNextSector(file,1);
		}
		else
		{
			FileReadSector(file, sector_buffer);
			p=sector_buffer;
			file->cursor+=count;
			while(count--)
				*buffer++=*p++;
		}
	}
	return(1);
}


int FileGetCh(fileTYPE *file)
{
	if(!file || !file->size)
		return -1;
	if (!(file->cursor&0x1ff)) {
		// reload buffer
		if(file->cursor)
			FileNextSector(file,1);
		FileReadSector(file, sector_buffer);
	}
	if (file->cursor >= file->size)
		return -1;
	else
		return (sector_buffer[(file->cursor++)&0x1ff]);
}


int LoadFile(const char *fn, unsigned char *buf)
{
	fileTYPE file;
	if(FileOpen(&file,fn))
	{
		uint32_t c=0;
		STATUS("Opened file, loading...\n");

		while(c<file.size)
		{
			if(!FileReadSector(&file,buf))
				return(0);
			FileNextSector(&file,1);

			buf+=512;
			c+=512;
		}
	}
	else
	{
		PDBG("Can't open %s\n",fn);
		return(0);
	}
	return(1);
}


void ChangeDirectoryByCluster(uint32_t cluster)
{
	if(cluster)
	{
		current_directory_cluster=cluster;
	    current_directory_start = data_start + cluster_size * (current_directory_cluster - 2);
		dir_entries = cluster_size << 4;
	}
	else
	{
		current_directory_cluster = root_directory_cluster;
		current_directory_start = root_directory_start;
		dir_entries = fat32 ?  cluster_size << 4 : root_directory_size << 4; // 16 entries per sector
	}
}


uint32_t CurrentDirectory()
{
	return(current_directory_cluster == root_directory_cluster ? 0 : current_directory_cluster);
}


void ChangeDirectory(DIRENTRY *p)
{
	uint32_t cluster=0;
	if(p)
	{
		cluster = ConvBB_LE(p->StartCluster);
		cluster |= fat32 ? (ConvBB_LE(p->HighCluster) & 0x0FFF) << 16 : 0;
	}
	ChangeDirectoryByCluster(cluster);
}


DIRENTRY *NextDirEntry(int init,int (*matchfunc)(const char *fn))
{
	static DIRENTRY      *pEntry = NULL;        // pointer to current entry in sector buffer
	static unsigned long  iDirectorySector;     // current sector of directory entries table
	static unsigned long  iDirectoryCluster;    // start cluster of subdirectory or FAT32 root directory
	static unsigned long  iEntry;        // entry index in directory cluster or FAT16 root directory
	int prevlfn=0;

	if(init)
	{
		iEntry=0;
		iDirectorySector=current_directory_start;
		iDirectoryCluster=current_directory_cluster;
	}
#ifndef DISABLE_LONG_FILENAMES
	longfilename[13]=0;
#endif

	while(1)
	{
		while (iEntry<dir_entries)
		{
			if ((iEntry & 0x0F) == 0) // first entry in sector, load the sector
			{
				cachedsector=iDirectorySector;
				sd_read_sector(iDirectorySector++, sector_buffer);
				pEntry = (DIRENTRY*)sector_buffer;
			}
			else
				pEntry++;
			++iEntry;

            if (pEntry->Name[0] != SLOT_EMPTY && pEntry->Name[0] != SLOT_DELETED) // valid entry??
			{
				#ifndef DISABLE_LONG_FILENAMES
				if (pEntry->Attributes == ATTR_LFN)	// Do we have a long filename entry?
				{
					unsigned char *p=&pEntry->Name[0];
					int seq=p[0];
					int offset=((seq&0x1f)-1)*13;
					char *o=&longfilename[offset];
					*o++=p[1];
					*o++=p[3];
					*o++=p[5];
					*o++=p[7];
					*o++=p[9];

					*o++=p[0xe];
					*o++=p[0x10];
					*o++=p[0x12];
					*o++=p[0x14];
					*o++=p[0x16];
					*o++=p[0x18];

					*o++=p[0x1c];
					*o++=p[0x1e];
					prevlfn=1;
				}
				#else
				if(0)
				{

				}
				#endif
				else if ((!(pEntry->Attributes & ATTR_VOLUME)) &&
					 ( (pEntry->Attributes & ATTR_DIRECTORY) || (!matchfunc) || matchfunc(&pEntry->Name[0])))
				{
#ifndef DISABLE_LONG_FILENAMES
					if(!prevlfn)
						longfilename[0]=0;
#endif
					prevlfn=0;
					// FIXME - should check the lfn checksum here.
					return(pEntry);
				}
				else
				{
#ifndef DISABLE_LONG_FILENAMES
					longfilename[13]=0;
#endif
					prevlfn=0;
				}
			}
		}
//		printf("iEntry %d is >= dir_entries %d\n",iEntry,dir_entries);

		if (current_directory_start || fat32) // subdirectory is a linked cluster chain
		{
			iDirectoryCluster = GetCluster(iDirectoryCluster); // get next cluster in chain
			 // check if end of cluster chain
			if (fat32 ? (iDirectoryCluster & 0x0FFFFFF8) == 0x0FFFFFF8 : (iDirectoryCluster & 0xFFF8) == 0xFFF8)
				break; // no more clusters in chain

			iDirectorySector = data_start + cluster_size * (iDirectoryCluster - 2); // calculate first sector address of the new cluster
			iEntry=0;
		}
		else
			break;
	}
    return(0);
}


int FindByCluster(uint32_t parent, uint32_t cluster)
{
    DIRENTRY      *p = NULL;        // pointer to current entry in sector buffer
	ChangeDirectoryByCluster(parent);
	while(p=NextDirEntry(p==NULL,0))
	{
		uint32_t c;
		c = ConvBB_LE(p->StartCluster);
		c += (fat32 ? (ConvBB_LE(p->HighCluster) & 0x0FFF) << 16 : 0);
		if(c==cluster)
			return(1);
	}
	return(0);
}


// Verify that a directory cluster is valid by recursively tracing ".." entries back up to the root,
// then verifying that each directory entry exists within its parent.
// Returns 0 on failure
int ValidateDirectory(uint32_t directory)
{
    DIRENTRY      *pEntry = NULL;        // pointer to current entry in sector buffer
    unsigned long  iDirectorySector;     // current sector of directory entries table
    unsigned long  iDirectoryCluster;    // start cluster of subdirectory or FAT32 root directory
    unsigned long  iEntry;               // entry index in directory cluster or FAT16 root directory

	if(!directory || (directory==root_directory_cluster))
	{
		return(1);
	}
    else // subdirectory
    {
        iDirectoryCluster = directory;
        iDirectorySector = data_start + cluster_size * (iDirectoryCluster - 2);
    }

    if(!sd_read_sector(iDirectorySector++, sector_buffer)) // root directory is linear
		return(0);
    pEntry = (DIRENTRY*)sector_buffer;
    for (iEntry = 0; iEntry < 16; iEntry++)	// 16 entries in a single sector.  Assume ".." will be in the first sector.
    {
        if (pEntry->Name[0] != SLOT_EMPTY && pEntry->Name[0] != SLOT_DELETED) // valid entry??
        {
            if (pEntry->Attributes & ATTR_DIRECTORY) // is this a directory
            {
                if (strncmp((const char*)pEntry->Name, "..         ", sizeof(pEntry->Name)) == 0)
                {
					unsigned long parent=ConvBB_LE(pEntry->StartCluster) + (fat32 ? (ConvBB_LE(pEntry->HighCluster) & 0x0FFF) << 16 : 0);

					/* Safer, but requires more resources */
                    return(ValidateDirectory(parent) && FindByCluster(parent,directory));

					/* Lighter-weight version, merely checks that a path can be traced to the root. */
/*                    return(ValidateDirectory(parent)); */
                }
            }
        }
        pEntry++;
    }
	return(0);
}

