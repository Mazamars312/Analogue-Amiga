#============================================================
# CLOCK
#============================================================
set_location_assignment PIN_M9 -to ADC_CLK_10
set_instance_assignment -name IO_STANDARD "2.5 V" -to ADC_CLK_10
set_location_assignment PIN_M8 -to MAX10_CLK1_50
set_instance_assignment -name IO_STANDARD "2.5 V" -to MAX10_CLK1_50
set_location_assignment PIN_P11 -to MAX10_CLK2_50
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to MAX10_CLK2_50

#============================================================
# KEY
#============================================================
set_location_assignment PIN_H21 -to KEY[0]
set_instance_assignment -name IO_STANDARD "1.5 V Schmitt Trigger" -to KEY[0]
set_location_assignment PIN_H22 -to KEY[1]
set_instance_assignment -name IO_STANDARD "1.5 V Schmitt Trigger" -to KEY[1]

#============================================================
# SW
#============================================================
set_location_assignment PIN_J21 -to SW[0]
set_instance_assignment -name IO_STANDARD "1.5 V Schmitt Trigger" -to SW[0]
set_location_assignment PIN_J22 -to SW[1]
set_instance_assignment -name IO_STANDARD "1.5 V Schmitt Trigger" -to SW[1]

#============================================================
# LED
#============================================================
set_location_assignment PIN_C7 -to LED[0]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[0]
set_location_assignment PIN_C8 -to LED[1]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[1]
set_location_assignment PIN_A6 -to LED[2]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[2]
set_location_assignment PIN_B7 -to LED[3]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[3]
set_location_assignment PIN_C4 -to LED[4]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[4]
set_location_assignment PIN_A5 -to LED[5]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[5]
set_location_assignment PIN_B4 -to LED[6]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[6]
set_location_assignment PIN_C5 -to LED[7]
set_instance_assignment -name IO_STANDARD "1.2 V" -to LED[7]

#============================================================
# VGA (SHARED WITH PMOD 1 & PMOD 2)
#============================================================
#Pins for 333 addon like Waveshare ps2 vga 
#connect MSB from addon to MSB of location assign. (eg VGAR2 from waveshare to VGA_R[3])
set_location_assignment PIN_AA7 -to VGA_R[3]
set_location_assignment PIN_Y7  -to VGA_R[2]
set_location_assignment PIN_U7  -to VGA_R[1]
set_location_assignment PIN_V7  -to VGA_G[3]
set_location_assignment PIN_R11 -to VGA_G[2]
set_location_assignment PIN_AB7 -to VGA_G[1]
set_location_assignment PIN_AB6 -to VGA_B[3]
set_location_assignment PIN_AA6 -to VGA_B[2]
set_location_assignment PIN_V10 -to VGA_B[1]
set_location_assignment PIN_W7  -to VGA_HS
set_location_assignment PIN_W6  -to VGA_VS
#Additional pins for a 444 addon like Digilent Pmod vga 
set_location_assignment PIN_V8  -to VGA_R[0]
set_location_assignment PIN_AB8 -to VGA_G[0]
set_location_assignment PIN_W8  -to VGA_B[0]

#============================================================
# PMOD 1 & 2  (SHARED WITH VGA AND PMOD DETO pins 1&2)
#============================================================
# PMOD 1
#set_location_assignment PIN_V8  -to PMOD1[0]
#set_location_assignment PIN_U7  -to PMOD1[1]
#set_location_assignment PIN_Y7  -to PMOD1[2]
#set_location_assignment PIN_AA7 -to PMOD1[3]
#set_location_assignment PIN_W8  -to PMOD1[4]
#set_location_assignment PIN_V10 -to PMOD1[5]
#set_location_assignment PIN_AA6 -to PMOD1[6]
#set_location_assignment PIN_AB6 -to PMOD1[7]

# PMOD 2
#set_location_assignment PIN_AB8 -to PMOD2[0]
#set_location_assignment PIN_AB7 -to PMOD2[1]
#set_location_assignment PIN_R11 -to PMOD2[2]
#set_location_assignment PIN_V7  -to PMOD2[3]
#set_location_assignment PIN_W7  -to PMOD2[4]
#set_location_assignment PIN_W6  -to PMOD2[5]
# PMOD DETO pins 1&2 are shared with PMOD2
#set_location_assignment PIN_W9  -to DETO1_PMOD2[6]
#set_location_assignment PIN_W5  -to DETO2_PMOD2[7]

#============================================================
# Keyboard (P9:11,12 GPIO1_D0/1) 
#============================================================
set_location_assignment PIN_Y5 -to PS2_KEYBOARD_CLK
set_location_assignment PIN_Y6 -to PS2_KEYBOARD_DAT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_DAT

#============================================================
# Mouse - (P9:10,9)   
#============================================================
set_location_assignment PIN_AA2 -to PS2_MOUSE_CLK
set_location_assignment PIN_U6  -to PS2_MOUSE_DAT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_CLK
#(10k resistor phisical pull-up) set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_DAT

#============================================================
# EAR
#============================================================
# EAR (in some MiST cores ear function is using UART_RXD)
set_location_assignment PIN_H4 -to EAR

#============================================================
# PMOD3 SPI / MIDI_DB  
#============================================================
# MOSI = P9:30 = PIN_W4
# MISO = MIDI_WSBD  = P9:29 = PIN_R9
# SCLK = MIDI_DABD  = P9:31 = PIN_P9
# CS0 = SPI_CS0 (OSD)  = MIDI_CLKBD = P8:5 = PIN_Y19
# CS1 = SPI_CS1 (IO)   = P8:6 = PIN_AA17
# CS2 = SPI_CS2 (FPGA) = P9:42 = PIN_W3
set_location_assignment PIN_W4   -to SPI_MOSI
set_location_assignment PIN_R9   -to SPI_MISO_WSBD
set_location_assignment PIN_P9   -to SPI_SCLK_DABD
set_location_assignment PIN_Y19  -to SPI_CS0_CLKBD
set_location_assignment PIN_AA17 -to SPI_CS1
set_location_assignment PIN_W3   -to SPI_CS2

#============================================================
# PMOD3 UART   (P8:3 Tx, 4 Rx GPIO0_D00/01) 
#============================================================
set_location_assignment PIN_Y18 -to UART_RXD
set_location_assignment PIN_W18 -to UART_TXD


#============================================================
# JOYSTICK DB9 / USB3 USER PORT
#============================================================
set_location_assignment PIN_J4 -to JOY1_B2_P9
set_location_assignment PIN_H3 -to JOY1_B1_P6
set_location_assignment PIN_J8 -to JOY1_UP
set_location_assignment PIN_J9 -to JOY1_DOWN
set_location_assignment PIN_F4 -to JOY1_LEFT
set_location_assignment PIN_F5 -to JOY1_RIGHT
# joystick select pin 
set_location_assignment PIN_K5 -to JOYX_SEL_O
# joystick Mux pin (for DB9 Antonio Villena dual joystick adapters)
# SHARED WITH PMOD DETO pin 3 (NOT SHARED WITH USB3 USER PORT)
set_location_assignment PIN_V17 -to DETO3_JOY_MUX
#
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_B2_P9
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_B1_P6
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_UP
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_DOWN
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_LEFT
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOY1_RIGHT
set_instance_assignment -name IO_STANDARD "2.5 V" -to JOYX_SEL_O
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B2_P9
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B1_P6
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_UP
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_DOWN
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_LEFT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_RIGHT
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_UP
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_DOWN
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_LEFT
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_B1_P6
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_B2_P9
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOY1_RIGHT
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to JOYX_SEL_O

#============================================================
# PMOD DETO (6 PIN MULTIPURPOSE PMOD) (I2C, SPI, I2S, Sigma/delta, ...)
#============================================================
# DETO PINS 1&2 SHARED WITH PMOD2[6] AND PMOD2[7]
set_location_assignment PIN_W9  -to DETO1_PMOD2_6
set_location_assignment PIN_W5  -to DETO2_PMOD2_7
# DETO PIN 3 SHARED WITH JOYSTICK MUX pin 
set_location_assignment PIN_V17 -to DETO3_JOY_MUX
set_location_assignment PIN_Y11 -to DETO4

# This Pmod is compatible with the PCM5102A I2S DAC pinout
# BCLK  = DETO2
# DIN   = DETO3
# LRCLK = DETO4

#(Sigma/Delta Atlas board: T12_left 1, 2 gnd, 3 R11_right)
#SIGMA_L = DETO3
#set_location_assignment PIN_V17 -to SIGMA_L
#set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_L
#SIGMA_R = DETO4
#set_location_assignment PIN_Y11 -to SIGMA_R
#set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_R


#============================================================
# Audio CODEC TLV320AIC3254
#============================================================
# PIN_R14  AUDIO_BCLK      i2sSck
# PIN_P15  AUDIO_DIN_MFP1  i2sD
# PIN_P14  AUDIO_MCLK      i2sMck
# PIN_R15  AUDIO_WCLK      i2sLr
set_location_assignment PIN_R14 -to I2S_SCK
set_instance_assignment -name IO_STANDARD "1.5 V" -to I2S_SCK
set_location_assignment PIN_P15 -to I2S_D
set_instance_assignment -name IO_STANDARD "1.5 V" -to I2S_D
set_location_assignment PIN_M22 -to AUDIO_GPIO_MFP5
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_GPIO_MFP5
set_location_assignment PIN_P14 -to I2S_MCK
set_instance_assignment -name IO_STANDARD "1.5 V" -to I2S_MCK
set_location_assignment PIN_N21 -to AUDIO_MISO_MFP4
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_MISO_MFP4
set_location_assignment PIN_M21 -to AUDIO_RESET_n
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_RESET_n
set_location_assignment PIN_P19 -to AUDIO_SCLK_MFP3
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_SCLK_MFP3
set_location_assignment PIN_P20 -to AUDIO_SCL_SS_n
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_SCL_SS_n
set_location_assignment PIN_P21 -to AUDIO_SDA_MOSI
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_SDA_MOSI
set_location_assignment PIN_N22 -to AUDIO_SPI_SELECT
set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_SPI_SELECT
set_location_assignment PIN_R15 -to I2S_LR
set_instance_assignment -name IO_STANDARD "1.5 V" -to I2S_LR
#alternative ear
#set_location_assignment PIN_P18 -to AUDIO_DOUT_MFP2
#set_instance_assignment -name IO_STANDARD "1.5 V" -to AUDIO_DOUT_MFP2


#============================================================
# MicroSD Card
#============================================================
set_location_assignment PIN_T20 -to SD_SCLK_O
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_SCLK_O
set_location_assignment PIN_T21 -to SD_MOSI_O
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_MOSI_O
set_location_assignment PIN_U22 -to SD_CMD_DIR
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_CMD_DIR
set_location_assignment PIN_T22 -to SD_D0_DIR
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_D0_DIR
set_location_assignment PIN_U21 -to SD_D123_DIR
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_D123_DIR
set_location_assignment PIN_R18 -to SD_MISO_I
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_MISO_I
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to SD_MISO_I
set_location_assignment PIN_R20 -to SD_CS_N_O
set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_CS_N_O
set_location_assignment PIN_P13 -to SD_SEL
#set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SD_SEL
#set_location_assignment PIN_T18 -to SD_DAT1		## NC	## SD_DAT[1]
#set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_DAT1	
#set_location_assignment PIN_T19 -to SD_DAT2		## NC	## SD_DAT[2]
#set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_DAT2
#set_location_assignment PIN_R22 -to SD_FB_CLK		## NOT USED
#set_instance_assignment -name IO_STANDARD "1.5 V" -to SD_FB_CLK


#============================================================
#  USB OTG PHY
#============================================================
set_location_assignment PIN_H11 -to USB_CLKIN
set_location_assignment PIN_F15 -to USB_DATA[7]
set_location_assignment PIN_E15 -to USB_DATA[6]
set_location_assignment PIN_D15 -to USB_DATA[5]
set_location_assignment PIN_H14 -to USB_DATA[4]
set_location_assignment PIN_E14 -to USB_DATA[3]
set_location_assignment PIN_H13 -to USB_DATA[2]
set_location_assignment PIN_E13 -to USB_DATA[1]
set_location_assignment PIN_E12 -to USB_DATA[0]
set_location_assignment PIN_H12 -to USB_NXT
set_location_assignment PIN_J13 -to USB_DIR
set_location_assignment PIN_J12 -to USB_STP
set_location_assignment PIN_J11 -to USB_CS
set_location_assignment PIN_E16 -to USB_RESET_n
set_location_assignment PIN_D8 -to USB_FAULT_n

set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_CS
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[0]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[1]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[2]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[3]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[4]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[5]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[6]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DATA[7]
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_DIR
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_NXT
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_RESET_n
set_instance_assignment -name IO_STANDARD "1.8 V" -to USB_STP
set_instance_assignment -name IO_STANDARD "1.2 V" -to USB_FAULT_n
set_instance_assignment -name IO_STANDARD "1.2 V" -to USB_CLKIN


#============================================================
# HDMI-TX
#============================================================
set_location_assignment PIN_C10 -to HDMI_I2C_SCL
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2C_SCL
set_location_assignment PIN_B15 -to HDMI_I2C_SDA
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2C_SDA
set_location_assignment PIN_A9 -to HDMI_I2S[0]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2S[0]
set_location_assignment PIN_A11 -to HDMI_I2S[1]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2S[1]
set_location_assignment PIN_A8 -to HDMI_I2S[2]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2S[2]
set_location_assignment PIN_B8 -to HDMI_I2S[3]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_I2S[3]
set_location_assignment PIN_A10 -to HDMI_LRCLK
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_LRCLK
set_location_assignment PIN_A7 -to HDMI_MCLK
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_MCLK
set_location_assignment PIN_D12 -to HDMI_SCLK
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_SCLK
set_location_assignment PIN_A20 -to HDMI_TX_CLK
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_CLK
set_location_assignment PIN_C18 -to HDMI_TX_D[0]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[0]
set_location_assignment PIN_D17 -to HDMI_TX_D[1]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[1]
set_location_assignment PIN_C17 -to HDMI_TX_D[2]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[2]
set_location_assignment PIN_C19 -to HDMI_TX_D[3]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[3]
set_location_assignment PIN_D14 -to HDMI_TX_D[4]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[4]
set_location_assignment PIN_B19 -to HDMI_TX_D[5]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[5]
set_location_assignment PIN_D13 -to HDMI_TX_D[6]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[6]
set_location_assignment PIN_A19 -to HDMI_TX_D[7]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[7]
set_location_assignment PIN_C14 -to HDMI_TX_D[8]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[8]
set_location_assignment PIN_A17 -to HDMI_TX_D[9]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[9]
set_location_assignment PIN_B16 -to HDMI_TX_D[10]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[10]
set_location_assignment PIN_C15 -to HDMI_TX_D[11]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[11]
set_location_assignment PIN_A14 -to HDMI_TX_D[12]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[12]
set_location_assignment PIN_A15 -to HDMI_TX_D[13]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[13]
set_location_assignment PIN_A12 -to HDMI_TX_D[14]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[14]
set_location_assignment PIN_A16 -to HDMI_TX_D[15]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[15]
set_location_assignment PIN_A13 -to HDMI_TX_D[16]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[16]
set_location_assignment PIN_C16 -to HDMI_TX_D[17]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[17]
set_location_assignment PIN_C12 -to HDMI_TX_D[18]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[18]
set_location_assignment PIN_B17 -to HDMI_TX_D[19]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[19]
set_location_assignment PIN_B12 -to HDMI_TX_D[20]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[20]
set_location_assignment PIN_B14 -to HDMI_TX_D[21]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[21]
set_location_assignment PIN_A18 -to HDMI_TX_D[22]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[22]
set_location_assignment PIN_C13 -to HDMI_TX_D[23]
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_D[23]
set_location_assignment PIN_C9 -to HDMI_TX_DE
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_DE
set_location_assignment PIN_B11 -to HDMI_TX_HS
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_HS
set_location_assignment PIN_B10 -to HDMI_TX_INT
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_INT
set_location_assignment PIN_C11 -to HDMI_TX_VS
set_instance_assignment -name IO_STANDARD "1.8 V" -to HDMI_TX_VS


#============================================================
# SDRAM
#============================================================
set_location_assignment PIN_Y17 -to DRAM_ADDR[0]
set_location_assignment PIN_W14 -to DRAM_ADDR[1]
set_location_assignment PIN_U15 -to DRAM_ADDR[2]
set_location_assignment PIN_R13 -to DRAM_ADDR[3]
set_location_assignment PIN_Y13 -to DRAM_ADDR[4]
set_location_assignment PIN_AB11 -to DRAM_ADDR[5]
set_location_assignment PIN_AA11 -to DRAM_ADDR[6]
set_location_assignment PIN_AB12 -to DRAM_ADDR[7]
set_location_assignment PIN_AA12 -to DRAM_ADDR[8]
set_location_assignment PIN_AB13 -to DRAM_ADDR[9]
set_location_assignment PIN_V14 -to DRAM_ADDR[10]
set_location_assignment PIN_AA13 -to DRAM_ADDR[11]
set_location_assignment PIN_AB14 -to DRAM_ADDR[12]
set_location_assignment PIN_V11 -to DRAM_BA[0]
set_location_assignment PIN_V13 -to DRAM_BA[1]
set_location_assignment PIN_AA20 -to DRAM_DQ[0]
set_location_assignment PIN_AA19 -to DRAM_DQ[1]
set_location_assignment PIN_AB21 -to DRAM_DQ[2]
set_location_assignment PIN_AB20 -to DRAM_DQ[3]
set_location_assignment PIN_AB19 -to DRAM_DQ[4]
set_location_assignment PIN_Y16 -to DRAM_DQ[5]
set_location_assignment PIN_V16 -to DRAM_DQ[6]
set_location_assignment PIN_AB18 -to DRAM_DQ[7]
set_location_assignment PIN_AA15 -to DRAM_DQ[8]
set_location_assignment PIN_Y14 -to DRAM_DQ[9]
set_location_assignment PIN_W15 -to DRAM_DQ[10]
set_location_assignment PIN_AB15 -to DRAM_DQ[11]
set_location_assignment PIN_W16 -to DRAM_DQ[12]
set_location_assignment PIN_AB16 -to DRAM_DQ[13]
set_location_assignment PIN_V15 -to DRAM_DQ[14]
set_location_assignment PIN_W17 -to DRAM_DQ[15]
set_location_assignment PIN_AA14 -to DRAM_CLK
set_location_assignment PIN_W12 -to DRAM_CAS_N
set_location_assignment PIN_W11 -to DRAM_RAS_N
set_location_assignment PIN_AB10 -to DRAM_WE_N
set_location_assignment PIN_V12 -to DRAM_CS_N
#DQMH/L & CKE not connected in Mister new SDRAM modules
set_location_assignment PIN_AA16 -to DRAM_UDQM 
set_location_assignment PIN_AB17 -to DRAM_LDQM
set_location_assignment PIN_W13 -to DRAM_CKE


# SDRAM set_instance_assignment
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[12]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to DRAM_ADDR[11]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[10]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[9]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[8]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[7]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[6]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[5]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[4]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[3]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[2]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[1]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_ADDR[0]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[15]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[14]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[13]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[12]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[11]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[10]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[9]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[8]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[7]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[6]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[5]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[4]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[3]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[2]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[1]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_DQ[0]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_BA[1]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_BA[0]
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_UDQM
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_LDQM
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_CKE
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_CAS_N
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_RAS_N
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_WE_N
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_CS_N
#set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to DRAM_CLK

set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[0]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[1]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[2]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[3]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[4]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[5]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[6]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[7]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[8]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[9]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[10]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[11]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[12]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[13]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[14]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to DRAM_DQ[15]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[2]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[3]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[4]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[5]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[6]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[7]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[8]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[9]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[10]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[11]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_ADDR[12]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[2]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[3]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[4]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[5]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[6]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[7]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[8]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[9]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[10]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[11]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[12]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[13]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[14]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_DQ[15]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_BA[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_BA[1]
#set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_CS_N
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_LDQM
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_UDQM
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_RAS_N
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_CAS_N
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to DRAM_WE_N
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[0]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[1]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[2]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[3]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[4]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[5]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[6]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[7]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[8]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[9]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[10]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[11]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[12]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[13]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[14]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to DRAM_DQ[15]



#============================================================
# SRAM
#============================================================
#set_location_assignment PIN_AA20 -to sram_addr_o[20]
#set_location_assignment PIN_R13 -to sram_addr_o[19]
#set_location_assignment PIN_AA19 -to sram_addr_o[18]
#set_location_assignment PIN_AB21 -to sram_addr_o[17]
#set_location_assignment PIN_AB20 -to sram_addr_o[16]
#set_location_assignment PIN_V11 -to sram_addr_o[15]
#set_location_assignment PIN_V13 -to sram_addr_o[14]
#set_location_assignment PIN_V14 -to sram_addr_o[13]
#set_location_assignment PIN_Y17 -to sram_addr_o[12]
#set_location_assignment PIN_W14 -to sram_addr_o[11]
#set_location_assignment PIN_U15 -to sram_addr_o[10]
#set_location_assignment PIN_AA12 -to sram_addr_o[9]
#set_location_assignment PIN_AB13 -to sram_addr_o[8]
#set_location_assignment PIN_AA13 -to sram_addr_o[7]
#set_location_assignment PIN_AB14 -to sram_addr_o[6]
#set_location_assignment PIN_AB12 -to sram_addr_o[5]
#set_location_assignment PIN_Y13 -to sram_addr_o[4]
#set_location_assignment PIN_W16 -to sram_addr_o[3]
#set_location_assignment PIN_AB16 -to sram_addr_o[2]
#set_location_assignment PIN_V15 -to sram_addr_o[1]
#set_location_assignment PIN_W17 -to sram_addr_o[0]

#set_location_assignment PIN_AB19 -to sram_data_io[7]
#set_location_assignment PIN_Y16 -to sram_data_io[6]
#set_location_assignment PIN_V16 -to sram_data_io[5]
#set_location_assignment PIN_AB18 -to sram_data_io[4]
#set_location_assignment PIN_Y14 -to sram_data_io[3]
#set_location_assignment PIN_AA15 -to sram_data_io[2]
#set_location_assignment PIN_W15 -to sram_data_io[1]
#set_location_assignment PIN_AB15 -to sram_data_io[0]

#set_location_assignment PIN_W13 -to sram_ce_n_o
#set_location_assignment PIN_AB10 -to sram_we_n_o
#set_location_assignment PIN_W12 -to sram_oe_n_o


#============================================================

