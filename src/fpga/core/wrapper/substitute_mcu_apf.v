// 832 CPU MCU for the Analogue Pocket
// Copyright � 2022 by Murray Aickin(mazamars312

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Big thanks to Alastair M. Robinson as it was his fantastic 832 CPU that allowed me to build this.
// This runs a Mister interface and other interfaces can be built for this for other projects :-)

module substitute_mcu_apf_mister(
	input                  clk_sys,
	input                  reset_n,
	output reg             reset_out,
		
	input 	     		   clk_74a,
	input [31:0]  		   bridge_addr,
	input 	     		   bridge_rd,
	output [31:0]  	       bridge_rd_data,
	input 	     		   bridge_wr,
	input  [31:0]   	   bridge_wr_data,
		 
	//Mister HPS Bus via the 32bit bus
	output 	     	   	IO_UIO,
	output    		   	IO_FPGA,
	output    		   	IO_STROBE,
	input 	     		   IO_WAIT,
	input  [15:0] 		   IO_DIN,
	output reg [15:0] 	IO_DOUT,
	input 					IO_WIDE, // 1 = 16bit, 0 = 8bit;
	
    input 	     	       dataslot_update,
    input 	     	[15:0] dataslot_update_id,
    input 	     	[31:0] dataslot_update_size,

	output reg             target_dataslot_read,       // rising edge triggered
	output reg             target_dataslot_write,

	input                  target_dataslot_ack,        // asserted upon command start until completion
	input                  target_dataslot_done,       // asserted upon command finish until next command is issued    
	input        [2:0]     target_dataslot_err,        // contains result of command execution. zero is OK

	output reg   [15:0]    target_dataslot_id,         // parameters for each of the read/reload/write commands
	output reg   [31:0]    target_dataslot_slotoffset,
	output reg   [31:0]    target_dataslot_bridgeaddr,
	output reg   [31:0]    target_dataslot_length,

	output       [9:0]     datatable_addr,
	output reg             datatable_wren,
	output reg   [31:0]    datatable_data,
	input      	 [31:0]    datatable_q,
	
	// UART
	output                 txd,
	input                  rxd
    );

parameter false = 0;
parameter true = 1;

// some timing for the UART and timer cores
parameter sysclk_frequency = 284;
parameter sysclk_hz = sysclk_frequency*1000;
parameter uart_divisor = sysclk_hz/1152;

// CPU Wires
wire [31:0] cpu_addr;
wire [31:0] from_cpu, from_rom;
wire [3:0]  cpu_bytesel;
reg  [31:0] to_cpu;
reg         cpu_ack;
wire        rom_wr;

// UART

reg  [7:0]  ser_txdata; 
wire [7:0]  ser_rxdata;
reg         ser_txgo;

// We need to see what is happening right?
simple_uart simple_uart (
.clk        (clk_sys),
.reset      (reset_n),
.txdata     (ser_txdata),
.txready    (ser_txready),
.txgo       (ser_txgo),
.rxdata     (ser_rxdata),
.rxint      (ser_rxint),
.txint      (open),
.clock_divisor (uart_divisor),
.rxd        (rxd),
.txd        (txd));
    
// Ram controller that is duel ported
    
controller_rom 
#(.top_address(16'h8000))
controller_rom(
    .clk               (clk_sys),
    .addr              (cpu_addr[13:2]),
    .d                 (from_cpu),
    .q                 (from_rom),
    .we                (rom_wr),
    .bytesel           (cpu_bytesel),
    .clk_74a           (clk_74a),
	 .little_enden		  (~reset_n), // THe compiled code is in little enden. So this helps swap the BRAM from little to big once running
	.bridge_addr       (bridge_addr),
	.bridge_rd         (bridge_rd),
	.bridge_rd_data    (bridge_rd_data),
	.bridge_wr         (bridge_wr),
	.bridge_wr_data    (bridge_wr_data)
);

assign rom_wr = ~|cpu_addr[31:16] && cpu_wr;
wire cpu_req;
// The CPU
    
//eightthirtytwo_cpu 
//eightthirtytwo_cpu
//	(
//		.clk (clk_sys),
//		.reset_n (reset_n),
//		.interrupt (cpu_int),
//		.addr (cpu_addr[31:2]),
//		.d (to_cpu),
//		.q (from_cpu),
//		.bytesel (cpu_bytesel),
//		.wr (cpu_wr),
//		.req (cpu_req),
//		.ack (cpu_ack)	
//	);

	parameter [ 0:0] ENABLE_COUNTERS = 1;
	parameter [ 0:0] ENABLE_COUNTERS64 = 1;
	parameter [ 0:0] ENABLE_REGS_16_31 = 1;
	parameter [ 0:0] ENABLE_REGS_DUALPORT = 1;
	parameter [ 0:0] LATCHED_MEM_RDATA = 0;
	parameter [ 0:0] TWO_STAGE_SHIFT = 1;
	parameter [ 0:0] BARREL_SHIFTER = 1;
	parameter [ 0:0] TWO_CYCLE_COMPARE = 0;
	parameter [ 0:0] TWO_CYCLE_ALU = 0;
	parameter [ 0:0] COMPRESSED_ISA = 0;
	parameter [ 0:0] CATCH_MISALIGN = 1;
	parameter [ 0:0] CATCH_ILLINSN = 1;
	parameter [ 0:0] ENABLE_PCPI = 0;
	parameter [ 0:0] ENABLE_MUL = 1;
	parameter [ 0:0] ENABLE_FAST_MUL = 0;
	parameter [ 0:0] ENABLE_DIV = 1;
	parameter [ 0:0] ENABLE_IRQ = 1;
	parameter [ 0:0] ENABLE_IRQ_QREGS = 1;
	parameter [ 0:0] ENABLE_IRQ_TIMER = 1;
	parameter [ 0:0] ENABLE_TRACE = 0;
	parameter [ 0:0] REGS_INIT_ZERO = 0;
	parameter [31:0] MASKED_IRQ = 32'h 0000_0001;
	parameter [31:0] LATCHED_IRQ = 32'h ffff_ffff;
	parameter [31:0] PROGADDR_RESET = 32'h 0000_0000;
	parameter [31:0] PROGADDR_IRQ = 32'h 0000_0010;
	parameter [31:0] STACKADDR = 32'h 0000_ffff;
    
picorv32 #(
		.ENABLE_COUNTERS     (ENABLE_COUNTERS     ),
		.ENABLE_COUNTERS64   (ENABLE_COUNTERS64   ),
		.ENABLE_REGS_16_31   (ENABLE_REGS_16_31   ),
		.ENABLE_REGS_DUALPORT(ENABLE_REGS_DUALPORT),
		.TWO_STAGE_SHIFT     (TWO_STAGE_SHIFT     ),
		.BARREL_SHIFTER      (BARREL_SHIFTER      ),
		.TWO_CYCLE_COMPARE   (TWO_CYCLE_COMPARE   ),
		.TWO_CYCLE_ALU       (TWO_CYCLE_ALU       ),
		.COMPRESSED_ISA      (COMPRESSED_ISA      ),
		.CATCH_MISALIGN      (CATCH_MISALIGN      ),
		.CATCH_ILLINSN       (CATCH_ILLINSN       ),
		.ENABLE_PCPI         (ENABLE_PCPI         ),
		.ENABLE_MUL          (ENABLE_MUL          ),
		.ENABLE_FAST_MUL     (ENABLE_FAST_MUL     ),
		.ENABLE_DIV          (ENABLE_DIV          ),
		.ENABLE_IRQ          (ENABLE_IRQ          ),
		.ENABLE_IRQ_QREGS    (ENABLE_IRQ_QREGS    ),
		.ENABLE_IRQ_TIMER    (ENABLE_IRQ_TIMER    ),
		.ENABLE_TRACE        (ENABLE_TRACE        ),
		.REGS_INIT_ZERO      (REGS_INIT_ZERO      ),
		.MASKED_IRQ          (MASKED_IRQ          ),
		.LATCHED_IRQ         (LATCHED_IRQ         ),
		.PROGADDR_RESET      (PROGADDR_RESET      ),
		.PROGADDR_IRQ        (PROGADDR_IRQ        ),
		.STACKADDR           (STACKADDR           )
	) picorv32_core (
		.clk      (clk_sys   ),
		.resetn   (reset_n),
		.trap     (trap  ),
		.irq		 (cpu_int),
		.mem_valid(cpu_req),
		.mem_addr (cpu_addr ),
		.mem_wdata(from_cpu),
		.mem_wstrb(cpu_bytesel),
		.mem_instr(mem_instr),
		.mem_ready(cpu_ack),
		.mem_rdata(to_cpu)
	);
	
	assign cpu_wr = |{cpu_bytesel};
    
// Timer for the cpu to make sure things are in time

reg [31:0] millisecond_counter;
reg [19:0] millisecond_tick;
reg        timer_tick;

always @(posedge clk_sys) begin
    timer_tick <= 0;
    millisecond_tick <= millisecond_tick + 1;
    if (millisecond_tick == sysclk_frequency * 100) begin
        if (millisecond_counter[3:0] == 'h0) begin
            timer_tick <= 1;
        end
        millisecond_counter <= millisecond_counter + 1;
        millisecond_tick <= 'h00000;
    end
end

// Interupt core for the data slot updates
reg int_ack;
wire int_status;

interupt_clock interupt_clock (
	.clk       (clk_sys),        // the system clock
	.int_clk   (clk_74a),    // the interupt clock domain
	.reset_n   (reset_n),
	.trigger   (dataslot_update),
	.ack       (int_ack),
	.int       (cpu_int),  
	.out       (int_status)
);

wire [15:0] dataslot_update_id_latched;

clock_reg_latch #(.data_size(16) ) dataslot_update_id_latch(
	.write_clk             (clk_74a),        // the APF clock
	.read_clk              (clk_sys),        // the system clock
	.reset_n               (reset_n),
	.write_trigger         (dataslot_update),
	.write_data_in         (dataslot_update_id),
	.read_data_out         (dataslot_update_id_latched)
);

wire [31:0] dataslot_update_size_latched;

clock_reg_latch #(.data_size(32) ) dataslot_update_size_latch(
	.write_clk             (clk_74a),        // the APF clock
	.read_clk              (clk_sys),        // the system clock
	.reset_n               (reset_n),
	.write_trigger         (dataslot_update),
	.write_data_in         (dataslot_update_size),
	.read_data_out         (dataslot_update_size_latched)
);
    
// external access to the CPU or cores for other cool stuff
reg [31:0]  ext_data_out;
reg         ext_data_en;
reg         ser_rxrecv;
reg         mem_busy, rom_ack;
reg         data_slot_ram_ack, data_slot_ram_ack_1;

reg        	io_clk;
reg        	io_ss0;
reg        	io_ss1;
reg        	io_ss2;

assign datatable_addr = cpu_addr[11:2];

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

always @(posedge clk_sys) begin
    mem_busy <= 1'b1;
    rom_ack <= 0;
    ser_txgo <= 0;
    int_ack <= 'b0;
    target_dataslot_write <= 'b0;
    target_dataslot_read <= 'b0;
    data_slot_ram_ack <= &{cpu_addr[31:16] == 16'hffff, cpu_addr[15:12] == 4'h0, cpu_req};
    data_slot_ram_ack_1 <= data_slot_ram_ack;
    datatable_wren <= 'b0;
    // UART Received signal
    if (ser_rxint) ser_rxrecv <= 1;
	 
    if (cpu_req)begin
        if (cpu_addr[31:16] == 16'hffff) begin
            if (~cpu_wr) begin
                casez (cpu_addr[15:0])
                    16'h0zzz : begin // target_dataslot_id read
                        ext_data_out <= datatable_q;
                        mem_busy <= data_slot_ram_ack_1;
                    end
                    16'hff80 : begin // target_dataslot_id read
                        ext_data_out <= target_dataslot_id;
                        mem_busy <= 0;
                    end
                    16'hff84 : begin // target_dataslot_bridgeaddr read
                        ext_data_out <= target_dataslot_bridgeaddr;
                        mem_busy <= 0;
                    end
                    16'hff88 : begin // target_dataslot_length read
						ext_data_out <= target_dataslot_length;
						mem_busy<= 0;
					end
					 16'hff8C : begin // target_dataslot_slotoffset read
                        ext_data_out <= target_dataslot_slotoffset;
                        mem_busy <= 0;
                    end
                    16'hff90 : begin // target_dataslot_slotoffset read
                        ext_data_out <= {target_dataslot_ack, target_dataslot_done, target_dataslot_err[2:0]};
                        mem_busy <= 0;
                    end
                    
                    16'hffA4 : begin // The reset the core function incase the system wants to make sure it is in sync
                        ext_data_out[0] <= reset_out;
                        mem_busy <= 0;
                    end
                    16'hffB0 : begin // Interrupt
						ext_data_out <= int_status;
						int_ack <= 1;
						mem_busy<= 0;
					end
					16'hffB4 : begin // dataslot_update_id ID
                        ext_data_out <= dataslot_update_id_latched;
                        mem_busy <= 0;
                    end
                    16'hffB8 : begin // dataslot_update_size ID
                        ext_data_out <= dataslot_update_size_latched;
                        mem_busy <= 0;
                    end
                    16'hffC0 : begin // UART access
                        ext_data_out <= {ser_rxrecv,ser_txready,ser_rxdata};
                        if (ser_rxrecv) ser_rxrecv<= 0;
                        mem_busy <= 0;
                    end
                    16'hffC8 : begin // Timer
                        ext_data_out <= millisecond_counter;
                        mem_busy <= 0;
                    end
					16'hffD0 : begin // This is setup for the SPI interface
                        ext_data_out <= {io_ack, IO_WIDE, IO_DIN};
                        mem_busy <= 0;
                    end
                    default : mem_busy <= 0;
                endcase
                ext_data_en <= 1;
            end
            else begin
                casez (cpu_addr[15:0])
                    16'h0zzz : begin // target_dataslot_id read
                        datatable_wren <= 1'b1;
                        datatable_data <= from_cpu;
                        mem_busy <= 1'b0;
                    end
                    16'hff80 : begin // target_dataslot_id read
                        target_dataslot_id <= from_cpu;
                        mem_busy <= 0;
                    end
                    16'hff84 : begin // target_dataslot_bridgeaddr read
                        target_dataslot_bridgeaddr <= from_cpu;
                        mem_busy <= 0;
                    end
                    16'hff88 : begin // target_dataslot_length read
						target_dataslot_length <= from_cpu;
						mem_busy<= 0;
					end
					16'hff8C : begin // target_dataslot_slotoffset read
                        target_dataslot_slotoffset <= from_cpu;
                        mem_busy <= 0;
                    end
                    16'hff90 : begin // target_dataslot_slotoffset read
                        {target_dataslot_write, target_dataslot_read} <= from_cpu;
                        mem_busy <= 0;
                    end
                    
                    16'hffA4 : begin // The reset the core function incase the system wants to make sure it is in sync
                        reset_out <= from_cpu[0];
                        mem_busy <= 0;
                    end
                    16'hffC0 : begin // UART Data
                        ser_txdata <= from_cpu[7:0];
                        ser_txgo <= 1;
                        mem_busy <= 0;
                    end
					 16'hffD0 : begin // This is setup for the SPI interface
                        io_clk = from_cpu[17];
						io_ss0 = from_cpu[18];
						io_ss1 = from_cpu[19];
						io_ss2 = from_cpu[20];
                        IO_DOUT <= from_cpu[15:0];
                        mem_busy <= 0;
                    end
                    default : mem_busy <= 0;
                endcase
                ext_data_en <= 1;
            end
        end
        else begin 
            ext_data_en <= 0;
            rom_ack <= 1;
        end
    end
    
    if (|{~mem_busy, rom_ack} && ~cpu_ack && cpu_req) cpu_ack <= 1 ;
    else cpu_ack <= 0 ;
    to_cpu <= ext_data_en ? ext_data_out : from_rom;
end

assign IO_FPGA     = ~io_ss1 & io_ss0;
assign IO_UIO      = ~io_ss1 & io_ss2;
reg  io_ack;
reg  rack;
assign IO_STROBE = ~rack & io_clk;
always @(posedge clk_sys) begin
	if(~(IO_WAIT) | IO_STROBE) begin
		rack <= io_clk;
		io_ack <= rack;
	end
end
    
endmodule

module interupt_clock (
	input          clk,        // the system clock
	input          int_clk,    // the interupt clock domain
	input          reset_n,
	input          trigger,
	input          ack,
	output reg     int,
	output reg     out
);

reg ack_clk_1, ack_clk_2, ack_clk_3; // Doing a 3 stage sync between the clock domains - Fuck I hate these.......
// But this "should" help with different clocks on both sides.
reg interupt_int_clk_1, interupt_int_clk_2, interupt_int_clk_3;

always @(posedge int_clk or negedge reset_n) begin
    if (~reset_n) begin
        interupt_int_clk_1 <= 'b0;
        interupt_int_clk_2 <= 'b0;
        interupt_int_clk_3 <= 'b0;
    end
    else begin
        if (trigger) interupt_int_clk_1 <= 1'b1;
        else if (|{ack_clk_1, ack_clk_2, ack_clk_3}) interupt_int_clk_1 <= 1'b0;
        interupt_int_clk_2 <= interupt_int_clk_1;
        interupt_int_clk_3 <= interupt_int_clk_2;
    end
end

always @(posedge clk or negedge reset_n) begin
    if (~reset_n) begin
        ack_clk_1 <= 'b0;
        int <= 'b0;
    end
    else begin
        ack_clk_1 <= ack;
        ack_clk_2 <= ack_clk_1;
        ack_clk_3 <= ack_clk_2;
        int <= &{interupt_int_clk_1, interupt_int_clk_2, interupt_int_clk_3};
    end
end

endmodule

module clock_reg_latch #(parameter data_size=32 )(
	input                          write_clk,        // the APF clock
	input                          read_clk,        // the system clock
	input                          reset_n,
	input                          write_trigger,
	input [data_size-1:0]          write_data_in,
	output reg [data_size-1:0]     read_data_out
);

reg [data_size-1:0] write_reg_triggered;

always @(posedge write_clk or negedge reset_n) begin
    if (~reset_n) begin
        write_reg_triggered <= 'b0;
    end
    else begin
        if (write_trigger) write_reg_triggered <= write_data_in;
    end
end

reg [data_size-1:0] read_data_clocked_1, read_data_clocked_2;

always @(posedge read_clk or negedge reset_n) begin
    if (~reset_n) begin
        read_data_clocked_1 <= 'b0;
        read_data_clocked_2 <= 'b0;
        read_data_out <= 'b0;
    end
    else begin
        read_data_clocked_1 <= write_reg_triggered;
        read_data_clocked_2 <= read_data_clocked_1;
        read_data_out <= read_data_clocked_2;
    end
end

endmodule


