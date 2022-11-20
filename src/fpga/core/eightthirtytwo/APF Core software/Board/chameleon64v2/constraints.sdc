set_time_format -unit ns -decimal_places 3
create_clock -name {clk_50} -period 20 -waveform { 0.000 10.00 } [get_ports {clk50m}]

set hostclk {clocks|altpll_component|auto_generated|pll1|clk[1]}
set supportclk {clocks|altpll_component|auto_generated|pll1|clk[0]}

create_generated_clock -name spiclk -source [get_pins ${hostclk}] -divide_by 4 [get_registers {substitute_mcu:controller|spi_controller:spi|sck}]
create_generated_clock -name spiclkfast -source [get_pins ${supportclk}] -divide_by 4 [get_registers {substitute_mcu:controller|spi_controller:spi|sck}]

derive_pll_clocks 
derive_clock_uncertainty

# Set pin definitions for downstream constraints
set RAM_CLK ram_clk
set RAM_OUT {ram_d* ram_a* ram_ba* ram_ras ram_cas ram_we ram_*dqm}
set RAM_IN {ram_d*}

set VGA_OUT {red* grn* blu* hsync_n vsync_n}

set FALSE_OUT {iec_* sigma_l sigma_r low_a[*] low_d[*] spi_mosi spi_clk mmc_cs usart_rx}
set FALSE_IN { low_d[*] spi_miso nmi_in freeze_btn usart_tx usart_clk usart_rts usart_cts}

# Constraints for board-specific signals

set_input_delay 0.5 -clock [get_clocks ${supportclk}] [get_ports {
	altera_reserved_tdi altera_reserved_tms ba_in dotclk_n
	ioef phi2_n ps2iec[*] reset_btn romlh spi_miso ir_data low_d[*]}]

set_input_delay 0.5 -clock [get_clocks ${hostclk}] [get_ports {
	usart_cts }]

set_output_delay 0.5 -clock [get_clocks ${hostclk}] [get_ports {
	mmc_cs rtc_cs spi_clk spi_mosi }]

set_output_delay 0.5 -clock [get_clocks ${supportclk}] [get_ports {
	altera_reserved_tdo game_out irq_out
	ps2iec_sel rw_out sa15_out sa_oe sd_dir sd_oe
	ser_out_clk ser_out_dat ser_out_rclk }]

set topmodule guest|
