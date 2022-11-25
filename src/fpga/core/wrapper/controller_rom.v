`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 19.11.2022 14:07:05
// Design Name: 
// Module Name: controller_rom
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module controller_rom #(parameter top_address = 16'h8000, address_size = 5'd14)
(

	input                  	clk,
	input [13:0]           	addr,
	input [31:0]           	d,
	output [31:0]      		q,
	input                  	we,
	input [3:0]            	bytesel,
	input 						little_enden,
	input 	     		 		clk_74a,
	input [31:0]  		 		bridge_addr,
	input 	     		 		bridge_rd,
	output reg [31:0]  	 	bridge_rd_data,
	input 	     		 		bridge_wr,
	input  [31:0]   	 		bridge_wr_data
);

wire [31:0] q_b;
wire wren_b = bridge_addr[31:16] == top_address && bridge_wr;

//	altsyncram	altsyncram_component (
//				.address_a (addr),
//				.address_b (bridge_addr[15:2]),
//				.byteena_a (bytesel),
//				.clock0 (clk),
//				.clock1 (clock_b),
//				.data_a (d),
//				.data_b (~little_enden ? {bridge_wr_data[7:0], bridge_wr_data[15:8], bridge_wr_data[23:16], bridge_wr_data[31:24]} : bridge_wr_data),
//				.rden_a (1'b1),
//				.rden_b (bridge_rd),
//				.wren_a (we),
//				.wren_b (wren_b),
//				.q_a (q),
//				.q_b (q_b),
//				.aclr0 (1'b0),
//				.aclr1 (1'b0),
//				.addressstall_a (1'b0),
//				.addressstall_b (1'b0),
//				.byteena_b (1'b1),
//				.clocken0 (1'b1),
//				.clocken1 (1'b1),
//				.clocken2 (1'b1),
//				.clocken3 (1'b1),
//				.eccstatus ());
//	defparam
//		altsyncram_component.address_reg_b = "CLOCK1",
//		altsyncram_component.byte_size = 8,
//		altsyncram_component.clock_enable_input_a = "BYPASS",
//		altsyncram_component.clock_enable_input_b = "BYPASS",
//		altsyncram_component.clock_enable_output_a = "BYPASS",
//		altsyncram_component.clock_enable_output_b = "BYPASS",
//		altsyncram_component.indata_reg_b = "CLOCK1",
//		altsyncram_component.intended_device_family = "Cyclone V",
//		altsyncram_component.lpm_type = "altsyncram",
//		altsyncram_component.numwords_a = 1<<address_size,
//		altsyncram_component.numwords_b = 1<<address_size,
//		altsyncram_component.operation_mode = "BIDIR_DUAL_PORT",
//		altsyncram_component.outdata_aclr_a = "NONE",
//		altsyncram_component.outdata_aclr_b = "NONE",
//		altsyncram_component.outdata_reg_a = "UNREGISTERED",
//		altsyncram_component.outdata_reg_b = "UNREGISTERED",
//		altsyncram_component.power_up_uninitialized = "FALSE",
//		altsyncram_component.read_during_write_mode_port_a = "NEW_DATA_NO_NBE_READ",
//		altsyncram_component.read_during_write_mode_port_b = "NEW_DATA_NO_NBE_READ",
//		altsyncram_component.widthad_a = address_size,
//		altsyncram_component.widthad_b = address_size,
//		altsyncram_component.width_a = 32,
//		altsyncram_component.width_b = 32,
//		altsyncram_component.width_byteena_a = 4,
//		altsyncram_component.width_byteena_b = 1,
//		altsyncram_component.wrcontrol_wraddress_reg_b = "CLOCK1";


controller_ram_duel_port controller_ram_duel_port (
	.address_a	(addr),
	.address_b	(bridge_addr[15:2]),
	.byteena_a	(bytesel),
	.rden_a		(1'b1),
	.rden_b		(bridge_rd),
	.clock_a		(clk),
	.clock_b		(clk_74a),
	.data_a		(d),
	.data_b		(~little_enden ? {bridge_wr_data[7:0], bridge_wr_data[15:8], bridge_wr_data[23:16], bridge_wr_data[31:24]} : bridge_wr_data),
	.wren_a		(we),
	.wren_b		(wren_b),
	.q_a			(q),
	.q_b			(q_b));

always @(posedge clk_74a) bridge_rd_data <= little_enden ? {q_b[7:0], q_b[15:8], q_b[23:16], q_b[31:24]} : q_b;
	
endmodule
