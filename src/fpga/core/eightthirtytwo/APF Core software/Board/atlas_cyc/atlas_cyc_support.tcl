set_global_assignment -name SDC_FILE  ${boardpath}/atlas_cyc/constraints.sdc

set_global_assignment -name VHDL_FILE ${boardpath}/atlas_cyc/audio/audio_i2s.vhd

set_global_assignment -name VERILOG_FILE ${boardpath}/atlas_cyc/USB2PS2/USB_PS2_PUBLIC.v

####### JUST LEAVE ONE HDMI WRAPPER (1/2/3) UNCOMMENTED
#### HDMI 1 NO SOUND (MultiCPM, Next186)
set_global_assignment -name QIP_FILE ${boardpath}/atlas_cyc/hdmi1/hdmi1.qip
#### HDMI 2 (MSX)
#set_global_assignment -name QIP_FILE ${boardpath}/atlas_cyc/hdmi2/hdmi2.qip
### HDMI 3 (Atari)
#set_global_assignment -name QIP_FILE ${boardpath}/atlas_cyc/hdmi3/hdmi3.qip

#if {[info exists atlas_cyc_HDMI_wrapper] && ($atlas_cyc_HDMI_wrapper==3)} {
#} 

# VERILOG_MACRO
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_PARALLEL_AUDIO=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_HDMI=1"
set_global_assignment -name VERILOG_MACRO "DEMISTIFY_ATLAS_CYC=1"

