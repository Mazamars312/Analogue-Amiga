create_clock -name {clk_50} -period 20.000 -waveform {0.000 10.000} { clock_50_i }
create_generated_clock -name spiclk -source [get_ports {clock_50_i}] -divide_by 16 [get_registers {substitute_mcu:controller|spi_controller:spi|sck}]

set hostclk { clk_50 }
set supportclk { clk_50 }

derive_pll_clocks -create_base_clocks
derive_clock_uncertainty

# Create a clock for i2s
create_clock -name i2sclk -period 20.000 {audio_top:audio_i2s|tcount[4]}

# Set pin definitions for downstream constraints
set RAM_CLK DRAM_CLK
set RAM_OUT {DRAM_DQ* DRAM_ADDR* DRAM_BA* DRAM_RAS_N DRAM_CAS_N DRAM_WE_N DRAM_*DQM DRAM_CS_N DRAM_CKE}
set RAM_IN {DRAM_D*}

set VGA_OUT {VGA_R[*] VGA_G[*] VGA_B[*] VGA_HS VGA_VS}

# non timing-critical pins would be in the "FALSE_IN/OUT" collection (IN inputs, OUT outputs)
set FALSE_OUT {LED I2S_* JOY_CLK JOY_LOAD PS2* sd_cs_n_o sd_mosi_o sd_sclk_o SIGMA_*}
set FALSE_IN  {PS2* sd_miso_i JOY_DATA AUDIO_INPUT}

#create_clock -name {altera_reserved_tck} -period 40 {altera_reserved_tck}
#set_input_delay -clock altera_reserved_tck -clock_fall 3 altera_reserved_tdi
#set_input_delay -clock altera_reserved_tck -clock_fall 3 altera_reserved_tms
#set_output_delay -clock altera_reserved_tck 3 altera_reserved_tdo

set topmodule guest|
