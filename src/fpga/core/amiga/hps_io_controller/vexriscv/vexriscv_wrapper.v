module vexriscv_wrapper (
	input  					clk,
	input 					reset_n,

	input 	     			clk_74a,
	input 	     			bridge_addr,
	input 	     			bridge_rd,
	output [31:0]  		bridge_rd_data,
	input 	     			bridge_wr,
	input  [31:0]   		bridge_wr_data,
		 
	//host controller interface (SPI)
	output 	     			IO_UIO,
	output 	     			IO_FPGA,
	output 	     			IO_STROBE,
	input 	     			IO_WAIT,
	input  [15:0] 			IO_DIN,
	output [15:0] 			IO_DOUT,

	output reg           target_dataslot_read,       // rising edge triggered
	output reg           target_dataslot_write,

	input               	target_dataslot_ack,        // asserted upon command start until completion
	input               	target_dataslot_done,       // asserted upon command finish until next command is issued    
	input        [2:0]   target_dataslot_err,        // contains result of command execution. zero is OK

	output reg   [15:0]  target_dataslot_id,         // parameters for each of the read/reload/write commands
	output reg   [31:0]  target_dataslot_slotoffset,
	output reg   [31:0]  target_dataslot_bridgeaddr,
	output reg   [31:0]  target_dataslot_length,

	output       [9:0]   datatable_addr,
	output               datatable_wren,
	output       [31:0]  datatable_data,
	input      	 [31:0]  datatable_q
);

   parameter CLK_FREQUENCY = 50000000;
   parameter AVR109_BAUD_RATE = 19200;
   parameter mem_init = "";
   parameter mem_size = 1;

   // -------------------------------
   // Reset generator

   wire nrst;
   assign nrst = reset_n;


   // -------------------------------
   // VexRiscv Core

   wire 	iBus_cmd_valid;
   wire 	iBus_cmd_ready;
   wire [31:0] 	iBus_cmd_payload_pc;
   wire 	iBus_rsp_valid;
   wire 	iBus_rsp_payload_error;
   wire [31:0] 	iBus_rsp_payload_inst;
   wire 	dBus_cmd_valid;
   wire 	dBus_cmd_ready;
   wire 	dBus_cmd_payload_wr;
   wire [31:0] 	dBus_cmd_payload_address;
   wire [31:0] 	dBus_cmd_payload_data;
   wire [1:0] 	dBus_cmd_payload_size;
   wire 	dBus_rsp_ready;
   wire 	dBus_rsp_error;
   wire [31:0] 	dBus_rsp_data;

   VexRiscv cpu(.iBus_cmd_valid(iBus_cmd_valid),
		.iBus_cmd_ready(iBus_cmd_ready),
		.iBus_cmd_payload_pc(iBus_cmd_payload_pc),
		.iBus_rsp_valid(iBus_rsp_valid),
		.iBus_rsp_payload_error(1'b0),
		.iBus_rsp_payload_inst(iBus_rsp_payload_inst),
		.timerInterrupt(timer0_comp_irq),
		.externalInterrupt(ext_irq3),
		.softwareInterrupt(1'b0),
		.dBus_cmd_valid(dBus_cmd_valid),
		.dBus_cmd_ready(dBus_cmd_ready),
		.dBus_cmd_payload_wr(dBus_cmd_payload_wr),
		.dBus_cmd_payload_address(dBus_cmd_payload_address),
		.dBus_cmd_payload_data(dBus_cmd_payload_data),
		.dBus_cmd_payload_size(dBus_cmd_payload_size),
		.dBus_rsp_ready(dBus_rsp_ready),
		.dBus_rsp_error(dBus_rsp_error),
		.dBus_rsp_data(dBus_rsp_data),
		.clk(clk), .reset(rst_out | prog_mode));



endmodule // vexriscv_wrapper
