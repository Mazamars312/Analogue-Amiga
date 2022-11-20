set_time_format -unit ns -decimal_places 3

create_clock -name clock27_0 -period 37.037 [get_ports {CLOCK_27[0]}]
create_clock -name clock27 -period 37.037 [get_ports {CLOCK_27}]
create_clock -name spiclk  -period 41.666 -waveform { 20.8 41.666 } [get_ports {SPI_SCK}]

set hostclk { clock27 }
set supportclk { clock27 }

derive_pll_clocks 

# Set pin definitions for downstream constraints

set RAM_CLK { SDRAM_CLK }
set RAM_OUT { SDRAM_nCS SDRAM_CKE SDRAM_DQ[*] SDRAM_A* SDRAM_DQM* SDRAM_BA* SDRAM_nRAS SDRAM_nCAS SDRAM_nWE}
set RAM_IN {SDRAM_DQ[*]}

set VGA_OUT {VGA_R* VGA_G* VGA_B* VGA_HS VGA_VS}

set FALSE_OUT {AUDIO_L AUDIO_R LED UART_TX}
set FALSE_IN {UART_RX}

# Constraints for board-specific signals

set_input_delay -clock spiclk -min 0.5 { SPI_DO CONF_DATA0 SPI_DI SPI_SS2 SPI_SS3 }
set_input_delay -clock spiclk -max 0.5 { SPI_DO CONF_DATA0 SPI_DI SPI_SS2 SPI_SS3 }
set_output_delay -clock spiclk -min 0.5 { SPI_DO }
set_output_delay -clock spiclk -max 0.5 { SPI_DO }
set_false_path -to SDRAM_CLK

set topmodule ""

