/*
 ============================================================================
 Name        : seektime.c
 Author      : Dave Shadoff
 Version     :
 Copyright   : (C) 2018 Dave Shadoff
 Description : Program to determine seek time, based on start and end sector numbers
 ============================================================================
 */

// CHanged to int as Floats are not active in the RISCV cpu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cue.h"

typedef struct sector_group {
		int		sec_per_revolution;
		int		sec_start;
		int		sec_end;
		int	rotation_ms;
		int	rotation_vsync;
} sector_group;

#define NUM_SECTOR_GROUPS	14

sector_group sector_list[NUM_SECTOR_GROUPS] = {
	{ 10,	0,		12572,	133,	 8 },
	{ 11,	12573,	30244,	146,	 8 },		// Except for the first and last groups,
	{ 12,	30245,	49523,	161,	 9 },		// there are 1606.5 tracks in each range
	{ 13,	49524,	70408,	174,	10 },
	{ 14,	70409,	92900,	187,	11 },
	{ 15,	92901,	116998,	201,	12 },
	{ 16,	116999,	142703,	213,	12 },
	{ 17,	142704,	170014,	226,	13 },
	{ 18,	170015,	198932,	240,	14 },
	{ 19,	198933,	229456,	253,	15 },
	{ 20,	229457,	261587,	267,	16 },
	{ 21,	261588,	295324,	281,	16 },
	{ 22,	295325,	330668,	294,	17 },
	{ 23,	330669,	333012,	307,	18 }
};


static int find_group(int sector_num)
{
	int i;
	int group_index = 0;

	for (i = 0; i < NUM_SECTOR_GROUPS; i++)
	{
		if ((sector_num >= sector_list[i].sec_start) && (sector_num <= sector_list[i].sec_end))
		{
			group_index = i;
			break;
		}
	}
	return group_index;
}

int get_cd_seek_ms(int start_sector, int target_sector)
{
	int start_index;
	int target_index;

	int track_difference;
	int milliseconds = 0;

	// First, we identify which group the start and end are in
	start_index = find_group(start_sector);
	target_index = find_group(target_sector);

	// Now we find the track difference
	//
	// Note: except for the first and last sector groups, all groups are 1606.48 tracks per group.
	//
	if (target_index == start_index)
	{
		track_difference = (int)(abs(target_sector - start_sector) / sector_list[target_index].sec_per_revolution);
	}
	else if (target_index > start_index)
	{
		track_difference = (sector_list[start_index].sec_end - start_sector) / sector_list[start_index].sec_per_revolution;
		track_difference += (target_sector - sector_list[target_index].sec_start) / sector_list[target_index].sec_per_revolution;
		track_difference += (1607 * (target_index - start_index - 1));
	}
	else // start_index > target_index
	{
		track_difference = (start_sector - sector_list[start_index].sec_start) / sector_list[start_index].sec_per_revolution;
		track_difference += (sector_list[target_index].sec_end - target_sector) / sector_list[target_index].sec_per_revolution;
		track_difference += (1607 * (start_index - target_index - 1));
	}

	// Now, we use the algorithm to determine how long to wait
	if (abs(target_sector - start_sector) <= 3)
	{
		milliseconds = (2 * 1000 / 60);
	}
	else if (abs(target_sector - start_sector) < 7)
	{
		milliseconds = (9 * 1000 / 60) + (int)(sector_list[target_index].rotation_ms);
	}
	else if (track_difference <= 80)
	{
		milliseconds = (17 * 1000 / 60) + (int)(sector_list[target_index].rotation_ms);
	}
	else if (track_difference <= 160)
	{
		milliseconds = (22 * 1000 / 60) + (int)(sector_list[target_index].rotation_ms);
	}
	else if (track_difference <= 644)
	{
		milliseconds = (22 * 1000 / 60) + (int)(sector_list[target_index].rotation_ms) + (int)((track_difference - 161) * 17 / 80);
	}
	else
	{
		milliseconds = (36 * 1000 / 60) + (int)(sector_list[target_index].rotation_ms) + (int)((track_difference - 644) * 17 / 195);
	}

	riscprintf("From sector %d to sector %d:\n", start_sector, target_sector);
	riscprintf("Time = %d milliseconds\n", milliseconds);

	return milliseconds;
}
