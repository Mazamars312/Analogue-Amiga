#============================================================
# CLOCK
#============================================================
set_location_assignment PIN_K14  -to FPGA_CLK1_50
set_location_assignment PIN_Y26  -to FPGA_CLK2_50
set_location_assignment PIN_AA16 -to FPGA_CLK3_50
set_location_assignment PIN_AF14 -to FPGA_CLK4_50
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to FPGA_CLK1_50
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to FPGA_CLK2_50
set_instance_assignment -name IO_STANDARD "1.5 V" -to FPGA_CLK3_50
set_instance_assignment -name IO_STANDARD "1.5 V" -to FPGA_CLK4_50

#============================================================
# KEY
#============================================================
set_location_assignment PIN_AE9  -to KEY[0]
set_location_assignment PIN_AE12 -to KEY[1]
set_location_assignment PIN_AD9  -to KEY[2]
set_location_assignment PIN_AD11 -to KEY[3]
set_location_assignment PIN_AD27 -to KEY_RESET_n
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to KEY[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to KEY[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to KEY[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to KEY[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to KEY_RESET_n

#============================================================
# SW
#============================================================
set_location_assignment PIN_W25  -to SW[0]
set_location_assignment PIN_V25  -to SW[1]
set_location_assignment PIN_AC28 -to SW[2]
set_location_assignment PIN_AC29 -to SW[3]
set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[0]
set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[1]
set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[2]
set_instance_assignment -name IO_STANDARD "2.5 V" -to SW[3]

#============================================================
# LED
#============================================================
set_location_assignment PIN_AF10 -to LED[0]
set_location_assignment PIN_AD10 -to LED[1]
set_location_assignment PIN_AE11 -to LED[2]
set_location_assignment PIN_AD7  -to LED[3]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LED[0]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LED[1]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LED[2]
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to LED[3]

#============================================================
# VGA
#============================================================
set_location_assignment PIN_AE28 -to VGA_B[0]
set_location_assignment PIN_Y23 -to VGA_B[1]
set_location_assignment PIN_Y24 -to VGA_B[2]
set_location_assignment PIN_AG28 -to VGA_B[3]
set_location_assignment PIN_AF28 -to VGA_B[4]
set_location_assignment PIN_V23 -to VGA_B[5]
set_location_assignment PIN_W24 -to VGA_B[6]
set_location_assignment PIN_AF29 -to VGA_B[7]
set_location_assignment PIN_Y21 -to VGA_G[0]
set_location_assignment PIN_AA25 -to VGA_G[1]
set_location_assignment PIN_AB26 -to VGA_G[2]
set_location_assignment PIN_AB22 -to VGA_G[3]
set_location_assignment PIN_AB23 -to VGA_G[4]
set_location_assignment PIN_AA24 -to VGA_G[5]
set_location_assignment PIN_AB25 -to VGA_G[6]
set_location_assignment PIN_AE27 -to VGA_G[7]
set_location_assignment PIN_AG5 -to VGA_R[0]
set_location_assignment PIN_AA12 -to VGA_R[1]
set_location_assignment PIN_AB12 -to VGA_R[2]
set_location_assignment PIN_AF6 -to VGA_R[3]
set_location_assignment PIN_AG6 -to VGA_R[4]
set_location_assignment PIN_AJ2 -to VGA_R[5]
set_location_assignment PIN_AH5 -to VGA_R[6]
set_location_assignment PIN_AJ1 -to VGA_R[7]
set_location_assignment PIN_AD12 -to VGA_HS
set_location_assignment PIN_AC12 -to VGA_VS
set_location_assignment PIN_AG2 -to VGA_SYNC_n
set_location_assignment PIN_AH3 -to VGA_BLANK_n
set_location_assignment PIN_W20 -to VGA_CLK

set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to VGA_*
set_instance_assignment -name CURRENT_STRENGTH_NEW 8MA -to VGA_*

#============================================================
# Keyboard 
#============================================================
# assign GPIO0_D[4]  = PS2_KEYBOARD_CLK;  //HSMC_CLKIN_n1 PIN_AB27
set_location_assignment PIN_AB27 -to PS2_KEYBOARD_CLK
# assign GPIO0_D[5]  = PS2_KEYBOARD_DAT;  //HSMC_RX _n[7] PIN_F8 
set_location_assignment PIN_F8 -to PS2_KEYBOARD_DAT

set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_DAT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to PS2_KEYBOARD_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to PS2_KEYBOARD_DAT

#============================================================
# Mouse
#============================================================
# assign GPIO0_D[6]  = PS2_MOUSE_CLK;     //HSMC_CLKIN_p1 PIN_AA26
set_location_assignment PIN_AA26 -to PS2_MOUSE_CLK
# assign GPIO0_D[7]  = PS2_MOUSE_DAT;     //HSMC_RX _p[7] PIN_F9
set_location_assignment PIN_F9 -to PS2_MOUSE_DAT

set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_DAT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to PS2_MOUSE_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to PS2_MOUSE_DAT

#============================================================
# Audio pwm output 
#============================================================
# set_location_assignment PIN_Y19  -to SIGMA_L
# set_location_assignment PIN_AA17 -to SIGMA_R
# set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_L
# set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SIGMA_R
# # EAR (in some mist cores ear function is using UART_RXD)
# set_location_assignment PIN_H4 -to EAR

#============================================================
# UART   (P8:3 Tx, 4 Rx GPIO0_D00/01) 
#============================================================
# assign GPIO0_D[8]  = UART_RXD;          //HSMC_TX _n[7] PIN_B3
set_location_assignment PIN_B3 -to UART_RXD
# assign GPIO0_D[9]  = UART_TXD;          //HSMC_RX _n[6] PIN_G8
set_location_assignment PIN_G8 -to UART_TXD

set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to UART_RXD
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to UART_TXD

#============================================================
# DETO - GENERAL USAGE GPIOS
#============================================================

# HSMC J3 connector pin 37 HSMC_TX _n[1] PIN_D7 – DETO1
# HSMC J3 connector pin 38 HSMC_TX _n[0] PIN_A8 – DETO2
# HSMC J3 connector pin 39 HSMC_TX _p[1] PIN_E8 – DETO3
# HSMC J3 connector pin 40 HSMC_TX _p[0] PIN_A9 – DETO4
set_location_assignment PIN_D7 -to DETO1
set_location_assignment PIN_A8 -to DETO2
set_location_assignment PIN_E8 -to DETO3
set_location_assignment PIN_A9 -to DETO4

set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to DETO1
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to DETO2
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to DETO3
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to DETO4


#============================================================
# JOYSTICK 
#============================================================
# assign GPIO0_D[10] = JOY1_B2_P9;        //HSMC_TX _p[7] PIN_C3
set_location_assignment PIN_C3 -to JOY1_B2_P9
# assign GPIO0_D[11] = JOY1_B1_P6;        //HSMC_RX _p[6] PIN_H8
set_location_assignment PIN_H8 -to JOY1_B1_P6
# assign GPIO0_D[12] = JOY1_UP;           //HSMC_TX _n[6] PIN_D4
set_location_assignment PIN_D4 -to JOY1_UP
# assign GPIO0_D[13] = JOY1_DOWN;         //HSMC_RX _n[5] PIN_H7
set_location_assignment PIN_H7 -to JOY1_DOWN
# assign GPIO0_D[16] = JOY1_LEFT;         //HSMC_TX _p[6] PIN_E4
set_location_assignment PIN_E4 -to JOY1_LEFT
# assign GPIO0_D[17] = JOY1_RIGHT;        //HSMC_RX _p[5] PIN_J7
set_location_assignment PIN_J7 -to JOY1_RIGHT
# assign GPIO0_D[18] = JOYX_SEL_O;        //HSMC_TX _n[5] PIN_E2
set_location_assignment PIN_E2 -to JOYX_SEL_O

set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B2_P9
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_B1_P6
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_UP
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_DOWN
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_LEFT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOY1_RIGHT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_B1_P6
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_B2_P9
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_DOWN
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_LEFT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_RIGHT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOY1_UP
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to JOYX_SEL_O

#============================================================
# Audio CODEC SSM2603
#============================================================
set_location_assignment PIN_AC27 -to AUD_ADCDAT
set_location_assignment PIN_AG30 -to AUD_ADCLRCK
set_location_assignment PIN_AE7  -to AUD_BCLK
set_location_assignment PIN_AG3  -to AUD_DACDAT
set_location_assignment PIN_AH4  -to AUD_DACLRCK
set_location_assignment PIN_AD26 -to AUD_MUTE
set_location_assignment PIN_AC9  -to AUD_XCK
set_location_assignment PIN_AH30 -to AUD_I2C_SCLK
set_location_assignment PIN_AF30 -to AUD_I2C_SDAT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to AUD_*

#============================================================
# MicroSD Card (ON DECA RETRO CAPE PMOD3)
#============================================================
# assign GPIO0_D[19] = PMOD3[0];          //HSMC_RX _n[4] PIN_K8
set_location_assignment PIN_K8 -to SD_CS_N_O
# assign GPIO0_D[20] = PMOD3[1];          //HSMC_TX _p[5] PIN_E3
set_location_assignment PIN_E3 -to SD_MOSI_O
# assign GPIO0_D[21] = PMOD3[2];          //HSMC_RX _p[4] PIN_K7
set_location_assignment PIN_K7 -to SD_MISO_I
# assign GPIO0_D[22] = PMOD3[3];          //HSMC_CLKOUT_n1  PIN_E6
set_location_assignment PIN_E6 -to SD_SCLK_O
#
# assign GPIO0_D[23] = PMOD3[4];          //HSMC_RX _n[3] PIN_J9
set_location_assignment PIN_J9 -to SD_DAT1
# assign GPIO0_D[24] = PMOD3[5];          //HSMC_CLKOUT_p1  PIN_E7
set_location_assignment PIN_E7 -to SD_DAT2
# assign GPIO0_D[25] = PMOD3[6];          //HSMC_RX _p[3] PIN_J10
set_location_assignment PIN_J10 -to SD_CD_I
# assign GPIO0_D[26] = PMOD3[7];          //HSMC_TX _n[4] PIN_C4
#set_location_assignment PIN_C4 -to 

set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to SD_MISO_I
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SD_*

#============================================================
# SDRAM
#============================================================
set_location_assignment PIN_B1 -to SDRAM_A[0]
set_location_assignment PIN_C2 -to SDRAM_A[1]
set_location_assignment PIN_B2 -to SDRAM_A[2]
set_location_assignment PIN_D2 -to SDRAM_A[3]
set_location_assignment PIN_D9 -to SDRAM_A[4]
set_location_assignment PIN_C7 -to SDRAM_A[5]
set_location_assignment PIN_E12 -to SDRAM_A[6]
set_location_assignment PIN_B7 -to SDRAM_A[7]
set_location_assignment PIN_D12 -to SDRAM_A[8]
set_location_assignment PIN_A11 -to SDRAM_A[9]
set_location_assignment PIN_B6 -to SDRAM_A[10]
set_location_assignment PIN_D11 -to SDRAM_A[11]
set_location_assignment PIN_A10 -to SDRAM_A[12]
set_location_assignment PIN_B5 -to SDRAM_BA[0]
set_location_assignment PIN_A4 -to SDRAM_BA[1]
set_location_assignment PIN_F14 -to SDRAM_DQ[0]
set_location_assignment PIN_G15 -to SDRAM_DQ[1]
set_location_assignment PIN_F15 -to SDRAM_DQ[2]
set_location_assignment PIN_H15 -to SDRAM_DQ[3]
set_location_assignment PIN_G13 -to SDRAM_DQ[4]
set_location_assignment PIN_A13 -to SDRAM_DQ[5]
set_location_assignment PIN_H14 -to SDRAM_DQ[6]
set_location_assignment PIN_B13 -to SDRAM_DQ[7]
set_location_assignment PIN_C13 -to SDRAM_DQ[8]
set_location_assignment PIN_C8 -to SDRAM_DQ[9]
set_location_assignment PIN_B12 -to SDRAM_DQ[10]
set_location_assignment PIN_B8 -to SDRAM_DQ[11]
set_location_assignment PIN_F13 -to SDRAM_DQ[12]
set_location_assignment PIN_C12 -to SDRAM_DQ[13]
set_location_assignment PIN_B11 -to SDRAM_DQ[14]
set_location_assignment PIN_E13 -to SDRAM_DQ[15]
set_location_assignment PIN_D10 -to SDRAM_CLK
set_location_assignment PIN_A5 -to SDRAM_nWE
set_location_assignment PIN_A6 -to SDRAM_nCAS
set_location_assignment PIN_A3 -to SDRAM_nCS
set_location_assignment PIN_E9 -to SDRAM_nRAS

#DQMH/L & CKE not connected in Mister new SDRAM modules
#HSMC_TX _n[8] PIN_D1
set_location_assignment PIN_D1 -to SDRAM_CKE
#HSMC_TX _p[8] PIN_E1
set_location_assignment PIN_E1 -to SDRAM_DQMH
#HSMC_RX _n[8] PIN_E11
set_location_assignment PIN_E11 -to SDRAM_DQML

set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to SDRAM_*
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_*
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_*
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[*]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[*]
set_instance_assignment -name ALLOW_SYNCH_CTRL_USAGE OFF -to *|SDRAM_*

#============================================================







