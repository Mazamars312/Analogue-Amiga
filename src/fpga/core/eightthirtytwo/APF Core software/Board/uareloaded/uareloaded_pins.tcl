# Pin & Location Assignments
# ==========================
set_location_assignment PIN_T2 -to CLOCK_50
set_location_assignment PIN_A5 -to LED

#dac video
set_location_assignment PIN_R22 -to VGA_B[7]
set_location_assignment PIN_P22 -to VGA_B[6]
set_location_assignment PIN_R1 -to VGA_B[5]
set_location_assignment PIN_P1 -to VGA_B[4]
set_location_assignment PIN_N1 -to VGA_B[3]
set_location_assignment PIN_M1 -to VGA_B[2]
set_location_assignment PIN_J1 -to VGA_B[1]
set_location_assignment PIN_H1 -to VGA_B[0]
set_location_assignment PIN_N22 -to VGA_G[7]
set_location_assignment PIN_M22 -to VGA_G[6]
set_location_assignment PIN_P2 -to VGA_G[5]
set_location_assignment PIN_N2 -to VGA_G[4]
set_location_assignment PIN_M2 -to VGA_G[3]
set_location_assignment PIN_J2 -to VGA_G[2]
set_location_assignment PIN_H2 -to VGA_G[1]
set_location_assignment PIN_F2 -to VGA_G[0]
set_location_assignment PIN_L22 -to VGA_R[7]
set_location_assignment PIN_K22 -to VGA_R[6]
set_location_assignment PIN_F1 -to VGA_R[5]
set_location_assignment PIN_D2 -to VGA_R[4]
set_location_assignment PIN_E1 -to VGA_R[3]
set_location_assignment PIN_C2 -to VGA_R[2]
set_location_assignment PIN_C1 -to VGA_R[1]
set_location_assignment PIN_B1 -to VGA_R[0]


set_location_assignment PIN_V22 -to VGA_BLANK
set_location_assignment PIN_U22 -to VGA_CLOCK
set_location_assignment PIN_B2 -to VGA_VS
set_location_assignment PIN_B3 -to VGA_HS

set_location_assignment PIN_V6 -to DRAM_ADDR[12]
set_location_assignment PIN_Y4 -to DRAM_ADDR[11]
set_location_assignment PIN_W1 -to DRAM_ADDR[10]
set_location_assignment PIN_V5 -to DRAM_ADDR[9]
set_location_assignment PIN_Y3 -to DRAM_ADDR[8]
set_location_assignment PIN_AA1 -to DRAM_ADDR[7]
set_location_assignment PIN_Y2 -to DRAM_ADDR[6]
set_location_assignment PIN_V4 -to DRAM_ADDR[5]
set_location_assignment PIN_V3 -to DRAM_ADDR[4]
set_location_assignment PIN_U1 -to DRAM_ADDR[3]
set_location_assignment PIN_U2 -to DRAM_ADDR[2]
set_location_assignment PIN_V1 -to DRAM_ADDR[1]
set_location_assignment PIN_V2 -to DRAM_ADDR[0]

set_location_assignment PIN_V11 -to DRAM_DQ[15]
set_location_assignment PIN_W10 -to DRAM_DQ[14]
set_location_assignment PIN_Y10 -to DRAM_DQ[13]
set_location_assignment PIN_V10 -to DRAM_DQ[12]
set_location_assignment PIN_V9 -to DRAM_DQ[11]
set_location_assignment PIN_Y8 -to DRAM_DQ[10]
set_location_assignment PIN_W8 -to DRAM_DQ[9]
set_location_assignment PIN_Y7 -to DRAM_DQ[8]
set_location_assignment PIN_AB5 -to DRAM_DQ[7]
set_location_assignment PIN_AA7 -to DRAM_DQ[6]
set_location_assignment PIN_AB7 -to DRAM_DQ[5]
set_location_assignment PIN_AA8 -to DRAM_DQ[4]
set_location_assignment PIN_AB8 -to DRAM_DQ[3]
set_location_assignment PIN_AA9 -to DRAM_DQ[2]
set_location_assignment PIN_AB9 -to DRAM_DQ[1]
set_location_assignment PIN_AA10 -to DRAM_DQ[0]

set_location_assignment PIN_W2 -to DRAM_BA[1]
set_location_assignment PIN_Y1 -to DRAM_BA[0]

set_location_assignment PIN_AA5 -to DRAM_LDQM
set_location_assignment PIN_W7 -to DRAM_UDQM

set_location_assignment PIN_AB3 -to DRAM_RAS_N
set_location_assignment PIN_AA4 -to DRAM_CAS_N
set_location_assignment PIN_AB4 -to DRAM_WE_N
set_location_assignment PIN_AA3 -to DRAM_CS_N
set_location_assignment PIN_W6 -to DRAM_CKE
set_location_assignment PIN_Y6 -to DRAM_CLK

set_location_assignment PIN_M19 -to PS2_KEYBOARD_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_CLK
set_location_assignment PIN_M20 -to PS2_KEYBOARD_DAT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_KEYBOARD_DAT

set_location_assignment PIN_N19 -to PS2_MOUSE_CLK
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_CLK
set_location_assignment PIN_N20 -to PS2_MOUSE_DAT
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to PS2_MOUSE_DAT

set_location_assignment PIN_A8 -to AUDIO_IN
set_location_assignment PIN_A5 -to LED

# I2C
set_location_assignment PIN_A13 -to I2C_SCL
set_location_assignment PIN_A14 -to I2C_SDA
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to I2C_SCL
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to I2C_SDA

# 2nd SD
set_location_assignment PIN_C22 -to SD_SCK
set_location_assignment PIN_B22 -to SD_CS
set_location_assignment PIN_C21 -to SD_MISO
set_location_assignment PIN_B21 -to SD_MOSI

# SRAM

set_location_assignment PIN_AB14 -to SRAM_A[20]
set_location_assignment PIN_AB13 -to SRAM_A[19]
set_location_assignment PIN_E21 -to SRAM_A[18]
set_location_assignment PIN_F21 -to SRAM_A[17]
set_location_assignment PIN_H21 -to SRAM_A[16]
set_location_assignment PIN_J21 -to SRAM_A[15]
set_location_assignment PIN_K21 -to SRAM_A[14]
set_location_assignment PIN_L21 -to SRAM_A[13]
set_location_assignment PIN_M21 -to SRAM_A[12]
set_location_assignment PIN_N21 -to SRAM_A[11]
set_location_assignment PIN_P21 -to SRAM_A[10]
set_location_assignment PIN_R21 -to SRAM_A[9]
set_location_assignment PIN_U21 -to SRAM_A[8]
set_location_assignment PIN_V21 -to SRAM_A[7]
set_location_assignment PIN_W21 -to SRAM_A[6]
set_location_assignment PIN_Y21 -to SRAM_A[5]
set_location_assignment PIN_AB20 -to SRAM_A[4]
set_location_assignment PIN_AB19 -to SRAM_A[3]
set_location_assignment PIN_AB18 -to SRAM_A[2]
set_location_assignment PIN_AB17 -to SRAM_A[1]
set_location_assignment PIN_AB16 -to SRAM_A[0]
set_location_assignment PIN_AB15 -to SRAM_Q[7]
set_location_assignment PIN_W22 -to SRAM_Q[6]
set_location_assignment PIN_Y22 -to SRAM_Q[5]
set_location_assignment PIN_AA20 -to SRAM_Q[4]
set_location_assignment PIN_AA19 -to SRAM_Q[3]
set_location_assignment PIN_AA18 -to SRAM_Q[2]
set_location_assignment PIN_AA17 -to SRAM_Q[1]
set_location_assignment PIN_AA16 -to SRAM_Q[0]
set_location_assignment PIN_D21 -to  SRAM_WE

# JOYSTICKS

set_location_assignment PIN_B16 -to JOYSTICK1[0]
set_location_assignment PIN_B17 -to JOYSTICK1[1]
set_location_assignment PIN_B18 -to JOYSTICK1[2]
set_location_assignment PIN_B19 -to JOYSTICK1[3]
set_location_assignment PIN_B14 -to JOYSTICK1[4]
set_location_assignment PIN_B13 -to JOYSTICK1[5]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[0]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[1]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[2]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[3]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[4]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK1[5]

set_location_assignment PIN_B7 -to JOYSTICK2[0]
set_location_assignment PIN_B8 -to JOYSTICK2[1]
set_location_assignment PIN_B9 -to JOYSTICK2[2]
set_location_assignment PIN_B10 -to JOYSTICK2[3]
set_location_assignment PIN_B5 -to JOYSTICK2[4]
set_location_assignment PIN_B4 -to JOYSTICK2[5]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[0]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[1]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[2]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[3]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[4]
set_instance_assignment -name WEAK_PULL_UP_RESISTOR ON -to JOYSTICK2[5]

set_location_assignment PIN_A15 -to JOY_SELECT
### I2S
set_location_assignment PIN_E22 -to SCLK
set_location_assignment PIN_J22 -to SDIN
set_location_assignment PIN_H22 -to MCLK
set_location_assignment PIN_F22 -to LRCLK

#
set_location_assignment PIN_A7 -to STM_RST

#  UART   
set_location_assignment PIN_C3 -to UART_RXD
set_location_assignment PIN_A3 -to UART_TXD
