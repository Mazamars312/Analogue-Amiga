set have_sdram 1
set base_clock 100
set vendor "lattice_yosys"
set fpga "ECP5"
set device --85k
set device_package CABGA381
set device_speed 6

lappend verilog_files ${boardpath}/${board}/mmm-v4r0_l5sd_sdram_defs.v

