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


module controller_rom #(parameter top_address = 16'h8000)
(

	input                  	clk,
	input [11:0]           	addr,
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

controller_ram_duel_port controller_ram_duel_port (
	.address_a	(addr),
	.address_b	(bridge_addr[13:2]),
	.byteena_a	(bytesel),
	.rden_a		(1'b1),
	.rden_b		(bridge_rd),
	.clock_a		(clk),
	.clock_b		(clk_74a),
	.data_a		(d),
	.data_b		(little_enden ? {bridge_wr_data[7:0], bridge_wr_data[15:8], bridge_wr_data[23:16], bridge_wr_data[31:24]} : bridge_wr_data),
	.wren_a		(we),
	.wren_b		(wren_b),
	.q_a			(q),
	.q_b			(q_b));

always @(posedge clk_74a) bridge_rd_data <= little_enden ? {q_b[7:0], q_b[15:8], q_b[23:16], q_b[31:24]} : q_b;
	
endmodule
