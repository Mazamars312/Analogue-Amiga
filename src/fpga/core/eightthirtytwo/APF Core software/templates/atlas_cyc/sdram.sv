//
//
// sdram controller implementation for the MiST/MiSTer boards
// 
// Copyright (c) 2015 Till Harbaum <till@harbaum.org> 
// Copyright (c) 2017 Sorgelig
// 
// This source file is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License 
// along with this program.  If not, see <http://www.gnu.org/licenses/>. 
//

//
//
// This SDRAM module provides/writes the data in 8 cycles of clock.
// So, with 64MHz of system clock, it can emulate 8MHz asynchronous DRAM.
//
//

module sdram
(

	// interface to the W9864G6JT chip
	inout [15:0]  		sd_data,    // 16 bit bidirectional data bus
	output reg [11:0]	sd_addr,    // 12 bit multiplexed address bus
	output reg [1:0] 	sd_dqm,     // two byte masks
	output reg [1:0] 	sd_ba,      // two banks
	output 				sd_cs,      // a single chip select
	output 				sd_we,      // write enable
	output 				sd_ras,     // row address select
	output 				sd_cas,     // columns address select

	// cpu/chipset interface
	input 		 		init,			// init signal after FPGA config to initialize RAM
	input 		 		clk,		   // sdram is accessed at 64MHz
	input             sync,

	input      [15:0] din,			// data input from chipset/cpu
	output reg [15:0] dout,			// data output to chipset/cpu
	input      [21:0] addr,       // 22 bit word address
	input       [1:0] ds,         // upper/lower data strobe
	input 		 		oe,         // cpu/chipset requests read
	input 		 		we,         // cpu/chipset requests write

	input      [15:0] bios_din,			// BIOS upload data from MCU
	output reg [15:0] bios_dout,			// BIOS output
	input      [10:0] bios_addr,       // 24 bit word address
	input 		 		bios_we          // BIOS upload strobe
);


localparam RASCAS_DELAY   = 3'd2;   // tRCD=20ns -> 3 cycles@128MHz
localparam BURST_LENGTH   = 3'b000; // 000=1, 001=2, 010=4, 011=8
localparam ACCESS_TYPE    = 1'b0;   // 0=sequential, 1=interleaved
localparam CAS_LATENCY    = 3'd2;   // 2/3 allowed
localparam OP_MODE        = 2'b00;  // only 00 (standard operation) allowed
localparam NO_WRITE_BURST = 1'b1;   // 0= write burst enabled, 1=only single access write

localparam MODE = { 2'b00, NO_WRITE_BURST, OP_MODE, CAS_LATENCY, ACCESS_TYPE, BURST_LENGTH}; 


// ---------------------------------------------------------------------
// ------------------------ cycle state machine ------------------------
// ---------------------------------------------------------------------

// The state machine runs at 128Mhz synchronous to the 8 Mhz chipset clock.
// It wraps from T15 to T0 on the rising edge of clk_8

localparam STATE_FIRST     = 3'd0;   // first state in cycle
localparam STATE_CMD_START = 3'd1;   // state in which a new command can be started
localparam STATE_CMD_CONT  = STATE_CMD_START + RASCAS_DELAY; // command can be continued
localparam STATE_READ      = STATE_CMD_CONT + CAS_LATENCY + 4'd2;
localparam STATE_HIGHZ     = STATE_READ - 4'd2; // disable output to prevent contention


// ---------------------------------------------------------------------
// --------------------------- startup/reset ---------------------------
// ---------------------------------------------------------------------

// wait 1ms (32 8Mhz cycles) after FPGA config is done before going
// into normal operation. Initialize the ram in the last 16 reset cycles (cycles 15-0)
reg [4:0] reset;
always @(posedge clk or posedge init) begin
	if(init)
		reset <= 5'h1f;
	else begin
		if((stage == STATE_FIRST) && (reset != 0))
			reset <= reset - 5'd1;
	end
end

// ---------------------------------------------------------------------
// ------------------ generate ram control signals ---------------------
// ---------------------------------------------------------------------

// all possible commands
localparam CMD_INHIBIT         = 4'b1111;
localparam CMD_NOP             = 4'b0111;
localparam CMD_ACTIVE          = 4'b0011;
localparam CMD_READ            = 4'b0101;
localparam CMD_WRITE           = 4'b0100;
localparam CMD_BURST_TERMINATE = 4'b0110;
localparam CMD_PRECHARGE       = 4'b0010;
localparam CMD_AUTO_REFRESH    = 4'b0001;
localparam CMD_LOAD_MODE       = 4'b0000;

reg [3:0] sd_cmd;   // current command sent to sd ram

// drive control signals according to current command
assign sd_cs  = sd_cmd[3];
assign sd_ras = sd_cmd[2];
assign sd_cas = sd_cmd[1];
assign sd_we  = sd_cmd[0];

reg active;
reg port;

// drive ram data lines when writing, set them as inputs otherwise
reg drive_dq;
assign sd_data = drive_dq ? din_r : 16'bZZZZZZZZZZZZZZZZ;

reg [15:0] sd_data_r;
reg [15:0] bios_dout_r;
reg [15:0] dout_r;

reg old_oe;
reg [15:0] din_r;
reg  [2:0] stage;

reg bios_oe;
wire [21:0] bios_addr_ext = {11'b11111111111,bios_addr};
reg [10:0] bios_addr_d;

always @(posedge clk) begin
	reg [11:0] addr_r;
	reg  [1:0] ds_r;
	reg        old_sync;
	
	if(|stage) stage <= stage + 1'd1;

	old_sync <= sync;
	if(~old_sync & sync) stage <= 1;

	if(bios_addr!=bios_addr_d)
		bios_oe<=1'b1;
	else
		bios_oe<=1'b0;
	
	sd_cmd <= CMD_INHIBIT;  // default: idle

	sd_data_r<=sd_data;
	
	if(reset != 0) begin
		// initialization takes place at the end of the reset phase
		if(stage == STATE_CMD_START) begin

			if(reset == 13) begin
				sd_cmd <= CMD_PRECHARGE;
				sd_addr[10] <= 1'b1;      // precharge all banks
			end
				
			if(reset == 2) begin
				sd_cmd <= CMD_LOAD_MODE;
				sd_addr <= MODE;
			end
			
		end
		drive_dq<=1'b0;
		active<=1'b0;
		
		bios_addr_d<=11'h7ff;
		sd_dqm  <= 2'b11;
	end else begin

		// normal operation
		if(stage == STATE_CMD_START) begin
			if(we || oe) begin
				port<=1'b0;
				active<=1'b1;
				drive_dq<=we;

				// RAS phase
				sd_cmd  <= CMD_ACTIVE;
				sd_addr <= addr[19:8];
				sd_ba   <= addr[21:20];

				ds_r    <= ds;
				din_r   <= din;
				addr_r  <= { 4'b0100, addr[7:0] };  // auto precharge
			end else if(bios_we || bios_oe) begin
				port<=1'b1;
				active<=1'b1;
				drive_dq<=bios_we;

				// RAS phase
				sd_cmd  <= CMD_ACTIVE;
				sd_addr <= bios_addr_ext[19:8];
				sd_ba   <= bios_addr_ext[21:20];

				ds_r    <= 2'b11;
				din_r   <= bios_din;
				addr_r  <= { 4'b0100, bios_addr_ext[7:0] };  // auto precharge
			end else begin
				drive_dq<=1'b0;
				active<=1'b0;
				sd_cmd <= CMD_AUTO_REFRESH;
			end
		end

		// CAS phase 
		if(stage == STATE_CMD_CONT && active) begin
			sd_cmd  <= drive_dq ? CMD_WRITE : CMD_READ;
			sd_addr <= addr_r;

			if(drive_dq) sd_dqm <= ~ds_r;
			else         sd_dqm <= 2'b00;
		end

		if(stage == STATE_HIGHZ) begin
			sd_dqm  <= 2'b11; // disable chip output
			drive_dq <= 1'b0; // disable data output
		end

		if(stage == STATE_READ && active) begin
			if(port) begin
				bios_dout_r<=sd_data_r;
				bios_addr_d<=bios_addr;
			end else
				dout_r <= sd_data_r;
			active<=1'b0;
		end
	end
end

assign dout = (active && !port) ? sd_data_r : dout_r;
assign bios_dout = (active && port) ? sd_data_r : bios_dout_r;

endmodule
