#============================================================
# CLOCK
#============================================================
set_location_assignment PIN_M2  -to CLK12M
set_location_assignment PIN_E15 -to CLK_X

#============================================================
# KEY
#============================================================
set_location_assignment PIN_N6 -to KEY0

#============================================================
# LED
#============================================================
set_location_assignment PIN_M6 -to LED[0]
set_location_assignment PIN_T4 -to LED[1]
set_location_assignment PIN_T3 -to LED[2]
set_location_assignment PIN_R3 -to LED[3]
set_location_assignment PIN_T2 -to LED[4]
set_location_assignment PIN_R4 -to LED[5]
set_location_assignment PIN_N5 -to LED[6]
set_location_assignment PIN_N3 -to LED[7]


#============================================================
# HDMI TDMS    ### VGA222 adapter
#============================================================              
set_location_assignment PIN_L16 -to TMDS[0]
  # CLK-                        ### VGA_HS
set_location_assignment PIN_L15 -to TMDS[1]
  # CLK+ # clock channel        ### VGA_VS
set_location_assignment PIN_P1  -to TMDS[2]
  # 0-                          ### VGA_B[0]
set_location_assignment PIN_P2  -to TMDS[3] 
 # 0+   # blue channel         ### VGA_B[1]
set_location_assignment PIN_J1  -to TMDS[4]
  # 1-                          ### VGA_G[0]
set_location_assignment PIN_J2  -to TMDS[5]
  # 1+   # green channel        ### VGA_G[1]
set_location_assignment PIN_N1  -to TMDS[6]
  # 2-                          ### VGA_R[0]
set_location_assignment PIN_N2  -to TMDS[7]
  # 2+   # red channel          ### VGA_R[1]


#============================================================
# Keyboard before AT1
#============================================================
#set_location_assignment PIN_L2 -to PS2_KEYBOARD_CLK
#set_location_assignment PIN_K2 -to PS2_KEYBOARD_DAT
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_CLK
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_DAT

#============================================================
# Keyboard AT1
#============================================================
#set_location_assignment PIN_K2 -to PS2_KEYBOARD_CLK
#set_location_assignment PIN_L2 -to PS2_KEYBOARD_DAT
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_CLK
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_DAT

#============================================================
# Keyboard Agnostic version
#============================================================
set_location_assignment PIN_K2 -to PS2_KEYBOARD_1
set_location_assignment PIN_L2 -to PS2_KEYBOARD_2
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_1
#set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_2

#============================================================
# Keyboard USB - Internal cyc1000 pulldowns
#============================================================
set_location_assignment PIN_L1 -to PDP_4k7
set_location_assignment PIN_K1 -to PDM_4k7

#============================================================
# Mouse 
#============================================================
set_location_assignment PIN_C16  -to PS2_MOUSE_CLK
set_location_assignment PIN_B16  -to PS2_MOUSE_DAT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_DAT

#============================================================
# Audio pwm output   #(Atlas board: T12_left 1, 2 gnd, 3 R11_right)
#============================================================
set_location_assignment PIN_T12  -to SIGMA_L
set_location_assignment PIN_R11  -to SIGMA_R
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_L
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_R

#============================================================
# BUS RPI (SHARED SPI RPI, I2S, MIDI)
#============================================================
set_location_assignment PIN_F15 -to PI_MISO_I2S_BCLK
set_location_assignment PIN_F16 -to PI_MOSI_I2S_LRCLK
set_location_assignment PIN_F13 -to PI_CLK_I2S_DATA
set_location_assignment PIN_C15 -to PI_CS_MIDI_CLKBD

#============================================================
# UART (SHARED MIDI)
#============================================================
set_location_assignment PIN_D16 -to UART_RXD_MIDI_WSBD
set_location_assignment PIN_D15 -to UART_TXD_MIDI_OUT

#============================================================
# Shared PIN_P11:   Audio EAR / Joystick select / MIDI_WSBD
#============================================================
set_location_assignment PIN_P11 -to JOYX_SEL_EAR_MIDI_DABD

#============================================================
# JOYSTICK 
#============================================================
set_location_assignment PIN_K16 -to JOY1_B2_P9
set_location_assignment PIN_K15 -to JOY1_B1_P6
set_location_assignment PIN_J14 -to JOY1_UP
set_location_assignment PIN_R1  -to JOY1_DOWN
set_location_assignment PIN_T15 -to JOY1_LEFT
set_location_assignment PIN_N16 -to JOY1_RIGHT

#============================================================
# JOYSTICK added for AT1
#============================================================
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B2_P9
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B1_P6
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_UP
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_DOWN
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_LEFT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_RIGHT

#============================================================
# MicroSD Card
#============================================================
set_location_assignment PIN_T13 -to SD_SCLK_O
set_location_assignment PIN_T14 -to SD_MOSI_O
set_location_assignment PIN_R13 -to SD_MISO_I
set_location_assignment PIN_R12 -to SD_CS_N_O
set_location_assignment PIN_P14 -to SD_DAT1		
set_location_assignment PIN_R14 -to SD_DAT2		

#============================================================
# SDRAM
#============================================================
set_location_assignment PIN_A3 -to DRAM_ADDR[0]
set_location_assignment PIN_B5 -to DRAM_ADDR[1]
set_location_assignment PIN_B4 -to DRAM_ADDR[2]
set_location_assignment PIN_B3 -to DRAM_ADDR[3]
set_location_assignment PIN_C3 -to DRAM_ADDR[4]
set_location_assignment PIN_D3 -to DRAM_ADDR[5]
set_location_assignment PIN_E6 -to DRAM_ADDR[6]
set_location_assignment PIN_E7 -to DRAM_ADDR[7]
set_location_assignment PIN_D6 -to DRAM_ADDR[8]
set_location_assignment PIN_D8 -to DRAM_ADDR[9]
set_location_assignment PIN_A5 -to DRAM_ADDR[10]
set_location_assignment PIN_E8 -to DRAM_ADDR[11]
set_location_assignment PIN_A2 -to DRAM_ADDR[12]
set_location_assignment PIN_A4 -to DRAM_BA[0]
set_location_assignment PIN_B6 -to DRAM_BA[1]
set_location_assignment PIN_B10 -to DRAM_DQ[0]
set_location_assignment PIN_A10 -to DRAM_DQ[1]
set_location_assignment PIN_B11 -to DRAM_DQ[2]
set_location_assignment PIN_A11 -to DRAM_DQ[3]
set_location_assignment PIN_A12 -to DRAM_DQ[4]
set_location_assignment PIN_D9  -to DRAM_DQ[5]
set_location_assignment PIN_B12 -to DRAM_DQ[6]
set_location_assignment PIN_C9  -to DRAM_DQ[7]
set_location_assignment PIN_D11 -to DRAM_DQ[8]
set_location_assignment PIN_E11 -to DRAM_DQ[9]
set_location_assignment PIN_A15 -to DRAM_DQ[10]
set_location_assignment PIN_E9  -to DRAM_DQ[11]
set_location_assignment PIN_D14 -to DRAM_DQ[12]
set_location_assignment PIN_F9  -to DRAM_DQ[13]
set_location_assignment PIN_C14 -to DRAM_DQ[14]
set_location_assignment PIN_A14 -to DRAM_DQ[15]
set_location_assignment PIN_B14 -to DRAM_CLK
set_location_assignment PIN_C8  -to DRAM_CAS_N
set_location_assignment PIN_B7  -to DRAM_RAS_N
set_location_assignment PIN_A7  -to DRAM_WE_N
set_location_assignment PIN_A6  -to DRAM_CS_N
set_location_assignment PIN_F8  -to DRAM_CKE
set_location_assignment PIN_B13 -to DRAM_LDQM
set_location_assignment PIN_D12 -to DRAM_UDQM

#============================================================

