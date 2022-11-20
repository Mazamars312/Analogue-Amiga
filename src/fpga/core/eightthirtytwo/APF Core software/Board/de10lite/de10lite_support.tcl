set_global_assignment -name SDC_FILE  ${boardpath}/de10lite/constraints.sdc

set_global_assignment -name VERILOG_MACRO "SDRAM_WINBOND=0"
set_global_assignment -name VERILOG_MACRO "SDRAM_ROWBITS=13"
set_global_assignment -name VERILOG_MACRO "SDRAM_COLBITS=10"
set_global_assignment -name VERILOG_MACRO "SDRAM_CL=3"
set_global_assignment -name VERILOG_MACRO "SDRAM_tCKminCL2=10000"
set_global_assignment -name VERILOG_MACRO "SDRAM_tRC=66000"
set_global_assignment -name VERILOG_MACRO "SDRAM_tWR=2"
set_global_assignment -name VERILOG_MACRO "SDRAM_tRP=15000"

# set_global_assignment -name VERILOG_MACRO "DEMISTIFY_C64_KEYBOARD=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_PS2_KEYBOARD=1"
