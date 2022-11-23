#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware.h"
#include "uart.h"

#ifndef APF_H
#define APF_H
/*
32'hFFFF_FF80 - target_dataslot_id (R/W) 15 bit - For which asset to read
32'hFFFF_FF84 - target_dataslot_bridgeaddr (R/W) 32 bit - Where in ram to place this this data (Buffer and program) (The MPU Ram starts at 32'h8000_0000 to 32'h8000_3FFF )
32'hFFFF_FF88 - target_dataslot_length (R/W) 32 bit - How much buffer to pull
32'hFFFF_FF8c - target_dataslot_slotoffset (R/W) 32 bit
32'hFFFF_FF90 - target_dataslot Controls (R) Respoce from the APF Core once the reg is written to
        {Bit 4 - target_dataslot_ack,
         Bit 3 - target_dataslot_done,
         Bit [2:0] target_dataslot_err[2:0]}
32'hFFFF_FF90 - target_dataslot Controls (W)
        {Bit 1 - target_dataslot_write - Send a Write signal to the APF ( the target id, Bridgeaddr, length and offset need to be written to first)
         Bit 0 - target_dataslot_read - Send a read signal to the APF ( the target id, Bridgeaddr, length and offset need to be written to first)
*/
#define DATASLOT_ID_BASE      0xFFFF0000
#define DATASLOT_ID_BASE_END  0xFFFF1000

#define DATASLOT_ID_BASE_END  0xFFFF1000

#define TARGET_DATASLOT_ID            0xFFFFFF80
#define TARGET_DATASLOT_BRIDGE_ADD    0xFFFFFF84
#define TARGET_DATASLOT_LENGTH        0xFFFFFF88
#define TARGET_DATASLOT_OFFSET        0xFFFFFF8C
#define TARGET_DATASLOT_CONTROL       0xFFFFFF90

#define TARGET_DATASLOT_READ_REG      0x1
#define TARGET_DATASLOT_WRITE_REG     0x2

#endif
