set have_sdram 1
set base_clock 25
set vendor "lattice_yosys"
set fpga "ECP5"
set device --25k
set device_package CABGA256
set device_speed 6

lappend verilog_files ${boardpath}/${board}/icesugarpro_sdram_defs.v

