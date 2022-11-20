set_global_assignment -name SDC_FILE     ${boardpath}/sockit/constraints.sdc
set_global_assignment -name VHDL_FILE ${boardpath}/sockit/audio/audio_i2s.vhd
set_global_assignment -name VERILOG_FILE ${boardpath}/sockit/audio/I2C_AV_Config.v
set_global_assignment -name VERILOG_FILE ${boardpath}/sockit/audio/I2C_Controller.v
set_global_assignment -name VERILOG_FILE ${boardpath}/sockit/audio/i2s_decoder.v

set_global_assignment -name VERILOG_MACRO "DEMISTIFY_PARALLEL_AUDIO=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_SOCKIT=1"

