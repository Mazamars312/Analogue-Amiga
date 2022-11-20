set_global_assignment -name QIP_FILE ${boardpath}/chameleon-modules/chameleonv2.qip
set_global_assignment -name SDC_FILE ${boardpath}/chameleon64v2/constraints.sdc
set_global_assignment -name VHDL_FILE ${boardpath}/chameleon64/gen_reset.vhd
set_global_assignment -name QIP_FILE ${boardpath}/chameleon64v2/hostclocks.qip

set_global_assignment -name VERILOG_MACRO "SDRAM_WINBOND=1"
set_global_assignment -name VERILOG_MACRO "SDRAM_ROWBITS=13"
set_global_assignment -name VERILOG_MACRO "SDRAM_COLBITS=9"
set_global_assignment -name VERILOG_MACRO "SDRAM_CL=2"
set_global_assignment -name VERILOG_MACRO "SDRAM_tCKminCL2=7500"
set_global_assignment -name VERILOG_MACRO "SDRAM_tRC=60000"
set_global_assignment -name VERILOG_MACRO "SDRAM_tWR=2"
set_global_assignment -name VERILOG_MACRO "SDRAM_tRP=15000"

set_global_assignment -name VERILOG_MACRO "DEMISTIFY_C64_KEYBOARD=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_PS2_KEYBOARD=1"
