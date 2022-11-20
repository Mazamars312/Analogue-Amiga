create_clock -name {clk_50} -period 20.000 -waveform {0.000 10.000} { FPGA_CLK1_50 }
create_generated_clock -name spiclk -source [get_ports {FPGA_CLK1_50}] -divide_by 16 [get_registers {substitute_mcu:controller|spi_controller:spi|sck}]

set hostclk { clk_50 }
set supportclk { clk_50 }

set cyclonev 1

derive_pll_clocks -create_base_clocks
derive_clock_uncertainty

# Set pin definitions for downstream constraints
set RAM_CLK SDRAM_CLK
set RAM_OUT {SDRAM_DQ* SDRAM_A* SDRAM_BA* SDRAM_nRAS SDRAM_nCAS SDRAM_nWE SDRAM_DQM* SDRAM_nCS SDRAM_CKE}
set RAM_IN {SDRAM_DQ[*]}

set VGA_OUT {VGA_R[*] VGA_G[*] VGA_B[*] VGA_HS VGA_VS VGA_SYNC_n VGA_BLANK_n}

# non timing-critical pins would be in the "FALSE_IN/OUT" collection (IN inputs, OUT outputs)
set FALSE_OUT {LED[*] PS2_* JOYX_SEL_O AUD_* UART_TXD SD_CS_N_O SD_MOSI_O SD_SCLK_O}
set FALSE_IN  {KEY[*] SW[*] PS2_* JOY1* AUD_* UART_RXD SD_MISO_I}
#the HDMI signals are probably fast enough to worth constraining properly at some point

# JTAG constraints for debug interface (if enabled)
#create_clock -name {altera_reserved_tck} -period 40 {altera_reserved_tck}
#set_input_delay -clock altera_reserved_tck -clock_fall 3 altera_reserved_tdi
#set_input_delay -clock altera_reserved_tck -clock_fall 3 altera_reserved_tms
#set_output_delay -clock altera_reserved_tck 3 altera_reserved_tdo

set topmodule guest|
