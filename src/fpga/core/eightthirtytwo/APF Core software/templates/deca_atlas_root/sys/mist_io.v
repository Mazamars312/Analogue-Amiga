//
// mist_io.v
//
// mist_io for the MiST board
// http://code.google.com/p/mist-board/
//
// Copyright (c) 2014 Till Harbaum <till@harbaum.org>
// Copyright (c) 2015-2017 Sorgelig
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
///////////////////////////////////////////////////////////////////////

//
// Use buffer to access SD card. It's time-critical part.
// Made module synchroneous with 2 clock domains: clk_sys and SPI_SCK
//                                                           (Sorgelig)
//
// for synchronous projects default value for PS2DIV is fine for any frequency of system clock.
// clk_ps2 = clk_sys/(PS2DIV*2)
//

module mist_io #(parameter STRLEN=0, parameter PS2DIV=100)
(

	// parameter STRLEN and the actual length of conf_str have to match
	input [(8*STRLEN)-1:0] conf_str,

	// Global clock. It should be around 100MHz (higher is better).
	input             clk_sys,

	// Global SPI clock from ARM. 24MHz
	input             SPI_SCK,

	input             CONF_DATA0,
	input             SPI_SS2,
	output            SPI_DO,
	input             SPI_DI,

	output reg  [7:0] joystick_0,
	output reg  [7:0] joystick_1,
	output reg [15:0] joystick_analog_0,
	output reg [15:0] joystick_analog_1,
	output      [1:0] buttons,
	output      [1:0] switches,
	output            scandoubler_disable,
	output            ypbpr,

	output reg [31:0] status,

	// SD config
	input             sd_conf,
	input             sd_sdhc,
	output      [1:0] img_mounted, // signaling that new image has been mounted
	output reg [31:0] img_size,    // size of image in bytes

	// SD block level access
	input      [31:0] sd_lba,
	input       [1:0] sd_rd,
	input       [1:0] sd_wr,
	output reg        sd_ack,
	output reg        sd_ack_conf,

	// SD byte level access. Signals for 2-PORT altsyncram.
	output reg  [8:0] sd_buff_addr,
	output reg  [7:0] sd_buff_dout,
	input       [7:0] sd_buff_din,
	output reg        sd_buff_wr,

	// ps2 keyboard emulation
	output            ps2_kbd_clk,
	output reg        ps2_kbd_data,
	output            ps2_mouse_clk,
	output reg        ps2_mouse_data,

	// ps2 alternative interface. 

	// [8] - extended, [9] - pressed, [10] - toggles with every press/release
	output reg [10:0] ps2_key = 0,

	// [24] - toggles with every event
	output reg [24:0] ps2_mouse = 0,

	// ARM -> FPGA download
	input             ioctl_ce,
	output reg        ioctl_download = 0, // signal indicating an active download
	output reg  [7:0] ioctl_index,        // menu index used to upload the file
	output reg        ioctl_wr = 0,
	output reg [24:0] ioctl_addr,
	output reg  [7:0] ioctl_dout
);

reg [7:0] but_sw;
reg [2:0] stick_idx;

reg [1:0] mount_strobe = 0;
assign img_mounted  = mount_strobe;

assign buttons = but_sw[1:0];
assign switches = but_sw[3:2];
assign scandoubler_disable = but_sw[4];
assign ypbpr = but_sw[5];

// this variant of user_io is for 8 bit cores (type == a4) only
wire [7:0] core_type = 8'ha4;

// command byte read by the io controller
wire       drive_sel = sd_rd[1] | sd_wr[1];
wire [7:0] sd_cmd = { 4'h6, sd_conf, sd_sdhc, sd_wr[drive_sel], sd_rd[drive_sel] };

reg [7:0] cmd;
reg [2:0] bit_cnt;    // counts bits 0-7 0-7 ...
reg [9:0] byte_cnt;   // counts bytes

reg spi_do;
assign SPI_DO = CONF_DATA0 ? 1'bZ : spi_do;

reg [7:0] spi_data_out;

// SPI transmitter
always@(negedge SPI_SCK) spi_do <= spi_data_out[~bit_cnt];

reg [7:0] spi_data_in;
reg       spi_data_ready = 0;

// SPI receiver
always@(posedge SPI_SCK or posedge CONF_DATA0) begin
	reg [6:0]  sbuf;
	reg [31:0] sd_lba_r;
	reg        drive_sel_r;

	if(CONF_DATA0) begin
	   bit_cnt <= 0;
	   byte_cnt <= 0;
		spi_data_out <= core_type;
	end
	else
	begin
		bit_cnt <= bit_cnt + 1'd1;
		sbuf <= {sbuf[5:0], SPI_DI};

		// finished reading command byte
      if(bit_cnt == 7) begin
			if(!byte_cnt) cmd <= {sbuf, SPI_DI};

			spi_data_in <= {sbuf, SPI_DI};
			spi_data_ready <= ~spi_data_ready;
			if(~&byte_cnt) byte_cnt <= byte_cnt + 8'd1;
			
			spi_data_out <= 0;
			case({(!byte_cnt) ? {sbuf, SPI_DI} : cmd})
				// reading config string
				8'h14: if(byte_cnt < STRLEN) spi_data_out <= conf_str[(STRLEN - byte_cnt - 1)<<3 +:8];

				// reading sd card status
				8'h16: if(byte_cnt == 0) begin
							spi_data_out <= sd_cmd;
							sd_lba_r <= sd_lba;
							drive_sel_r <= drive_sel;
						end else if (byte_cnt == 1) begin
							spi_data_out <= drive_sel_r;
						end else if(byte_cnt < 6) spi_data_out <= sd_lba_r[(5-byte_cnt)<<3 +:8];

				// reading sd card write data
				8'h18: spi_data_out <= sd_buff_din;
			endcase
		end
	end
end

reg [31:0] ps2_key_raw = 0;
wire       pressed  = (ps2_key_raw[15:8] != 8'hf0);
wire       extended = (~pressed ? (ps2_key_raw[23:16] == 8'he0) : (ps2_key_raw[15:8] == 8'he0));

// transfer to clk_sys domain
always@(posedge clk_sys) begin
	reg old_ss1, old_ss2;
	reg old_ready1, old_ready2;
	reg [2:0] b_wr;
	reg       got_ps2 = 0;

	old_ss1 <= CONF_DATA0;
	old_ss2 <= old_ss1;
	old_ready1 <= spi_data_ready;
	old_ready2 <= old_ready1;
	
	sd_buff_wr <= b_wr[0];
	if(b_wr[2] && (~&sd_buff_addr)) sd_buff_addr <= sd_buff_addr + 1'b1;
	b_wr <= (b_wr<<1);

	if(old_ss2) begin
		got_ps2      <= 0;
		sd_ack       <= 0;
		sd_ack_conf  <= 0;
		sd_buff_addr <= 0;
		if(got_ps2) begin
			if(cmd == 4) ps2_mouse[24] <= ~ps2_mouse[24]; 
			if(cmd == 5) begin
				ps2_key <= {~ps2_key[10], pressed, extended, ps2_key_raw[7:0]};
				if(ps2_key_raw == 'hE012E07C) ps2_key[9:0] <= 'h37C; // prnscr pressed
				if(ps2_key_raw == 'h7CE0F012) ps2_key[9:0] <= 'h17C; // prnscr released
				if(ps2_key_raw == 'hF014F077) ps2_key[9:0] <= 'h377; // pause  pressed
			end
		end
	end
	else
	if(old_ready2 ^ old_ready1) begin

		if(cmd == 8'h18 && ~&sd_buff_addr) sd_buff_addr <= sd_buff_addr + 1'b1;

		if(byte_cnt < 2) begin

			if (cmd == 8'h19) sd_ack_conf  <= 1;
			if((cmd == 8'h17) || (cmd == 8'h18)) sd_ack <= 1;
			mount_strobe <= 0;

			if(cmd == 5) ps2_key_raw <= 0;
		end else begin
		
			case(cmd)
				// buttons and switches
				8'h01: but_sw <= spi_data_in; 
				8'h02: joystick_0 <= spi_data_in;
				8'h03: joystick_1 <= spi_data_in;

				// store incoming ps2 mouse bytes 
				8'h04: begin
						got_ps2 <= 1;
						case(byte_cnt)
							2: ps2_mouse[7:0]   <= spi_data_in;
							3: ps2_mouse[15:8]  <= spi_data_in;
							4: ps2_mouse[23:16] <= spi_data_in;
						endcase 
						ps2_mouse_fifo[ps2_mouse_wptr] <= spi_data_in; 
						ps2_mouse_wptr <= ps2_mouse_wptr + 1'd1;
					end

				// store incoming ps2 keyboard bytes 
				8'h05: begin
						got_ps2 <= 1;
						ps2_key_raw[31:0] <= {ps2_key_raw[23:0], spi_data_in}; 						
						ps2_kbd_fifo[ps2_kbd_wptr] <= spi_data_in; 
						ps2_kbd_wptr <= ps2_kbd_wptr + 1'd1;
					end
			
				8'h15: status[7:0] <= spi_data_in;
			
				// send SD config IO -> FPGA
				// flag that download begins
				// sd card knows data is config if sd_dout_strobe is asserted
				// with sd_ack still being inactive (low)
				8'h19,
				// send sector IO -> FPGA
				// flag that download begins
				8'h17: begin
						sd_buff_dout <= spi_data_in;
						b_wr <= 1;
					end

				// joystick analog
				8'h1a: begin
						// first byte is joystick index
						if(byte_cnt == 2) stick_idx <= spi_data_in[2:0];
						else if(byte_cnt == 3) begin
							// second byte is x axis
							if(stick_idx == 0) joystick_analog_0[15:8] <= spi_data_in;
								else if(stick_idx == 1) joystick_analog_1[15:8] <= spi_data_in;
						end else if(byte_cnt == 4) begin
							// third byte is y axis
							if(stick_idx == 0) joystick_analog_0[7:0] <= spi_data_in;
								else if(stick_idx == 1) joystick_analog_1[7:0] <= spi_data_in;
						end
					end

				// notify image selection
				8'h1c: mount_strobe[spi_data_in[0]] <= 1;

				// send image info
				8'h1d: if(byte_cnt<6) img_size[(byte_cnt-2)<<3 +:8] <= spi_data_in;

				// status, 32bit version
				8'h1e: if(byte_cnt<6) status[(byte_cnt-2)<<3 +:8] <= spi_data_in;
				default: ;
			endcase
		end
	end
end


///////////////////////////////   PS2   ///////////////////////////////
// 8 byte fifos to store ps2 bytes
localparam PS2_FIFO_BITS = 3;

reg clk_ps2;
always @(negedge clk_sys) begin
	integer cnt;
	cnt <= cnt + 1'd1;
	if(cnt == PS2DIV) begin
		clk_ps2 <= ~clk_ps2;
		cnt <= 0;
	end
end

// keyboard
reg [7:0] ps2_kbd_fifo[1<<PS2_FIFO_BITS];
reg [PS2_FIFO_BITS-1:0] ps2_kbd_wptr;
reg [PS2_FIFO_BITS-1:0] ps2_kbd_rptr;

// ps2 transmitter state machine
reg [3:0] ps2_kbd_tx_state;
reg [7:0] ps2_kbd_tx_byte;
reg ps2_kbd_parity;

assign ps2_kbd_clk = clk_ps2 || (ps2_kbd_tx_state == 0);

// ps2 transmitter
// Takes a byte from the FIFO and sends it in a ps2 compliant serial format.
reg ps2_kbd_r_inc;
always@(posedge clk_sys) begin
	reg old_clk;
	old_clk <= clk_ps2;
	if(~old_clk & clk_ps2) begin
		ps2_kbd_r_inc <= 0;

		if(ps2_kbd_r_inc) ps2_kbd_rptr <= ps2_kbd_rptr + 1'd1;

		// transmitter is idle?
		if(ps2_kbd_tx_state == 0) begin
			// data in fifo present?
			if(ps2_kbd_wptr != ps2_kbd_rptr) begin
				// load tx register from fifo
				ps2_kbd_tx_byte <= ps2_kbd_fifo[ps2_kbd_rptr];
				ps2_kbd_r_inc <= 1;

				// reset parity
				ps2_kbd_parity <= 1;

				// start transmitter
				ps2_kbd_tx_state <= 1;

				// put start bit on data line
				ps2_kbd_data <= 0;			// start bit is 0
			end
		end else begin

			// transmission of 8 data bits
			if((ps2_kbd_tx_state >= 1)&&(ps2_kbd_tx_state < 9)) begin
				ps2_kbd_data <= ps2_kbd_tx_byte[0];	          // data bits
				ps2_kbd_tx_byte[6:0] <= ps2_kbd_tx_byte[7:1]; // shift down
				if(ps2_kbd_tx_byte[0]) 
					ps2_kbd_parity <= !ps2_kbd_parity;
			end

			// transmission of parity
			if(ps2_kbd_tx_state == 9) ps2_kbd_data <= ps2_kbd_parity;

			// transmission of stop bit
			if(ps2_kbd_tx_state == 10) ps2_kbd_data <= 1;    // stop bit is 1

			// advance state machine
			if(ps2_kbd_tx_state < 11) ps2_kbd_tx_state <= ps2_kbd_tx_state + 1'd1;
				else ps2_kbd_tx_state <= 0;
		end
	end
end

// mouse
reg [7:0] ps2_mouse_fifo[1<<PS2_FIFO_BITS];
reg [PS2_FIFO_BITS-1:0] ps2_mouse_wptr;
reg [PS2_FIFO_BITS-1:0] ps2_mouse_rptr;

// ps2 transmitter state machine
reg [3:0] ps2_mouse_tx_state;
reg [7:0] ps2_mouse_tx_byte;
reg ps2_mouse_parity;

assign ps2_mouse_clk = clk_ps2 || (ps2_mouse_tx_state == 0);

// ps2 transmitter
// Takes a byte from the FIFO and sends it in a ps2 compliant serial format.
reg ps2_mouse_r_inc;
always@(posedge clk_sys) begin
	reg old_clk;
	old_clk <= clk_ps2;
	if(~old_clk & clk_ps2) begin
		ps2_mouse_r_inc <= 0;

		if(ps2_mouse_r_inc) ps2_mouse_rptr <= ps2_mouse_rptr + 1'd1;

		// transmitter is idle?
		if(ps2_mouse_tx_state == 0) begin
			// data in fifo present?
			if(ps2_mouse_wptr != ps2_mouse_rptr) begin
				// load tx register from fifo
				ps2_mouse_tx_byte <= ps2_mouse_fifo[ps2_mouse_rptr];
				ps2_mouse_r_inc <= 1;

				// reset parity
				ps2_mouse_parity <= 1;

				// start transmitter
				ps2_mouse_tx_state <= 1;

				// put start bit on data line
				ps2_mouse_data <= 0;			// start bit is 0
			end
		end else begin

			// transmission of 8 data bits
			if((ps2_mouse_tx_state >= 1)&&(ps2_mouse_tx_state < 9)) begin
				ps2_mouse_data <= ps2_mouse_tx_byte[0];			  // data bits
				ps2_mouse_tx_byte[6:0] <= ps2_mouse_tx_byte[7:1]; // shift down
				if(ps2_mouse_tx_byte[0]) 
					ps2_mouse_parity <= !ps2_mouse_parity;
			end

			// transmission of parity
			if(ps2_mouse_tx_state == 9) ps2_mouse_data <= ps2_mouse_parity;

			// transmission of stop bit
			if(ps2_mouse_tx_state == 10) ps2_mouse_data <= 1;	  // stop bit is 1

			// advance state machine
			if(ps2_mouse_tx_state < 11) ps2_mouse_tx_state <= ps2_mouse_tx_state + 1'd1;
				else ps2_mouse_tx_state <= 0;
		end
	end
end


///////////////////////////////   DOWNLOADING   ///////////////////////////////

reg  [7:0] data_w;
reg [24:0] addr_w;
reg        rclk   = 0;

localparam UIO_FILE_TX      = 8'h53;
localparam UIO_FILE_TX_DAT  = 8'h54;
localparam UIO_FILE_INDEX   = 8'h55;

reg        rdownload = 0;

// data_io has its own SPI interface to the io controller
always@(posedge SPI_SCK, posedge SPI_SS2) begin
	reg  [6:0] sbuf;
	reg  [7:0] cmd;
	reg  [4:0] cnt;
	reg [24:0] addr;

	if(SPI_SS2) cnt <= 0;
	else begin
		// don't shift in last bit. It is evaluated directly
		// when writing to ram
		if(cnt != 15) sbuf <= { sbuf[5:0], SPI_DI};

		// count 0-7 8-15 8-15 ... 
		if(cnt < 15) cnt <= cnt + 1'd1;
			else cnt <= 8;

		// finished command byte
      if(cnt == 7) cmd <= {sbuf, SPI_DI};

		// prepare/end transmission
		if((cmd == UIO_FILE_TX) && (cnt == 15)) begin
			// prepare 
			if(SPI_DI) begin
				case(ioctl_index[4:0]) 
							1: addr <= 25'h200000; // TRD buffer  at 2MB
							2: addr <= 25'h400000; // tape buffer at 4MB 
					default: addr <= 25'h150000; // boot rom
				endcase
				rdownload <= 1; 
			end else begin
				addr_w <= addr;
				rdownload <= 0;
			end
		end

		// command 0x54: UIO_FILE_TX
		if((cmd == UIO_FILE_TX_DAT) && (cnt == 15)) begin
			addr_w <= addr;
			data_w <= {sbuf, SPI_DI};
			addr <= addr + 1'd1;
			rclk <= ~rclk;
		end

      // expose file (menu) index
      if((cmd == UIO_FILE_INDEX) && (cnt == 15)) ioctl_index <= {sbuf, SPI_DI};
	end
end

// transfer to ioctl_clk domain.
// ioctl_index is set before ioctl_download, so it's stable already
always@(posedge clk_sys) begin
	reg        rclkD, rclkD2;

	if(ioctl_ce) begin
		ioctl_download <= rdownload;

		rclkD    <= rclk;
		rclkD2   <= rclkD;
		ioctl_wr <= 0;

		if(rclkD != rclkD2) begin
			ioctl_dout <= data_w;
			ioctl_addr <= addr_w;
			ioctl_wr   <= 1;
		end
	end
end

endmodule
