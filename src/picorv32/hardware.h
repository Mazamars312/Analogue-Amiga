#ifndef HARDWARE_H
#define HARDWARE_H

#include <inttypes.h>
#include <stdio.h>

#define SPI_RST_USR         0x1
#define SPI_RST_CPU         0x2
#define SPI_CPU_HLT         0x4

#define INTERRUPTBASE             0xffffffb0
#define MISTERGPOHARDWAREBASE     0xffffffd0
#define MISTERGPIHARDWAREBASE     0xffffffd4

#define RESTARTBASE 0xFFFFFFA4
#define RESET_CORE(x) *(volatile unsigned int *)(RESTARTBASE+x)

#endif // SPI_H
/***********************************************************
	Memory map for the 832 CPU

	32'h0000_0000 - 32'h0000_3FFF - Program Ram for the CPU - The program is in here and starts at address 0000_0000.
									This is also mapped at address 32'8000_0000 on the APF Bus so data can be up/downloaded
									from the APF PIC32. Mostly used for the target dataslot bridge address location.

	32'hFFFF_0000 - 32'hFFFF_0FFF - Data slot ram from APF Core (R/W)

	// Target Interface to APF
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

	32'hFFFF_FFA4 - Bit 0 Reset_out - Used for reseting the core if required (R/W)

	// Interupts and dataslot updates

	32'hFFFF_FFB0 - Interrupt core - Bit 0 = Dataslot updated by APF (R) Read clears the interupt to the CPU

	32'hFFFF_FFB4 - dataslot_update_id ID = Dataslot updated by APF (R) 15 bit
	32'hFFFF_FFB8 - dataslot_update_size ID = Dataslot updated by APF (R) 32 bit

	// UART core
	32'hFFFF_FFC0 - UART access - ser_rxrecv,ser_txready,ser_rxdata
		bit [7:0] data received,
		Bit8 - Transmit ready,
		Bit9 - Data received (cleared once read) (R)
	32'hFFFF_FFC0 - UART access
		ser_txdata [7:0] data to send (W)

	32'hFFFF_FFC8 - Timer - millisecond_counter (R) 32 bit

	The SPI/Mister EXT interface
	32'hFFFF_FFD0 - This is setup for the SPI interface (R)
					Bit [15:0] 	- IO_DIN Data from core
					Bit [16]	- IO_WIDE 1 = 16bit, 0 = 8 bit
					Bit [17]	- io_ack - Data has been ack from core (Both read and writes)

	32'hFFFF_FFD0 - This is setup for the SPI interface (W)
					Bit [15:0] 	- IO_DIN Data from core
					Bit [17]	- io_clk - This is send as a clock from the CPU when data is send or received - Also part of the strobe signal
					Bit [18]	- io_ss0
					Bit [19]	- io_ss1
					Bit [20]	- io_ss2

	assign IO_FPGA     = ~io_ss1 & io_ss0; - This is for the CPU to send commands to the FPGA
	assign IO_UIO      = ~io_ss1 & io_ss2; - This is used for a Broardcast on the bus (Floopy drives use this access)



***********************************************************/
