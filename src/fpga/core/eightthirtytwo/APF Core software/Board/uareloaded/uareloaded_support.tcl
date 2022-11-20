set_global_assignment -name SDC_FILE ${boardpath}/uareloaded/constraints.sdc

# Audio support files
set_global_assignment -name VHDL_FILE ${boardpath}/uareloaded/audio_top.vhd

set_global_assignment -name VERILOG_MACRO "DEMISTIFY_PARALLEL_AUDIO=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_UARELOADED=1"

