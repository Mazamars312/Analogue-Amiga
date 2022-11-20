# Pin & Location Assignments
# ==========================
set_location_assignment PIN_G1 -to LED
set_location_assignment PIN_E1 -to CLOCK_27
set_location_assignment PIN_P16 -to VGA_R[5]
set_location_assignment PIN_P15 -to VGA_R[4]
set_location_assignment PIN_R16 -to VGA_R[3]
set_location_assignment PIN_R14 -to VGA_R[2]
set_location_assignment PIN_T15 -to VGA_R[1]
set_location_assignment PIN_T14 -to VGA_R[0]
set_location_assignment PIN_J16 -to VGA_B[5]
set_location_assignment PIN_J15 -to VGA_B[4]
set_location_assignment PIN_J14 -to VGA_B[3]
set_location_assignment PIN_K16 -to VGA_B[2]
set_location_assignment PIN_K15 -to VGA_B[1]
set_location_assignment PIN_J13 -to VGA_B[0]
set_location_assignment PIN_F16 -to VGA_G[5]
set_location_assignment PIN_F15 -to VGA_G[4]
set_location_assignment PIN_L16 -to VGA_G[3]
set_location_assignment PIN_L15 -to VGA_G[2]
set_location_assignment PIN_N15 -to VGA_G[1]
set_location_assignment PIN_N16 -to VGA_G[0]
set_location_assignment PIN_T10 -to VGA_VS
set_location_assignment PIN_T11 -to VGA_HS
set_location_assignment PIN_T12 -to AUDIO_L
set_location_assignment PIN_T13 -to AUDIO_R
set_location_assignment PIN_T2 -to SPI_DO
set_location_assignment PIN_R1 -to SPI_DI
set_location_assignment PIN_T3 -to SPI_SCK
set_location_assignment PIN_T4 -to SPI_SS2
set_location_assignment PIN_G15 -to SPI_SS3
set_location_assignment PIN_H2 -to CONF_DATA0
set_location_assignment PIN_B14 -to SDRAM_A[0]
set_location_assignment PIN_C14 -to SDRAM_A[1]
set_location_assignment PIN_C15 -to SDRAM_A[2]
set_location_assignment PIN_C16 -to SDRAM_A[3]
set_location_assignment PIN_B16 -to SDRAM_A[4]
set_location_assignment PIN_A15 -to SDRAM_A[5]
set_location_assignment PIN_A14 -to SDRAM_A[6]
set_location_assignment PIN_A13 -to SDRAM_A[7]
set_location_assignment PIN_A12 -to SDRAM_A[8]
set_location_assignment PIN_D16 -to SDRAM_A[9]
set_location_assignment PIN_B13 -to SDRAM_A[10]
set_location_assignment PIN_D15 -to SDRAM_A[11]
set_location_assignment PIN_D14 -to SDRAM_A[12]
set_location_assignment PIN_C3 -to SDRAM_DQ[0]
set_location_assignment PIN_C2 -to SDRAM_DQ[1]
set_location_assignment PIN_A4 -to SDRAM_DQ[2]
set_location_assignment PIN_B4 -to SDRAM_DQ[3]
set_location_assignment PIN_A6 -to SDRAM_DQ[4]
set_location_assignment PIN_D6 -to SDRAM_DQ[5]
set_location_assignment PIN_A7 -to SDRAM_DQ[6]
set_location_assignment PIN_B7 -to SDRAM_DQ[7]
set_location_assignment PIN_E6 -to SDRAM_DQ[8]
set_location_assignment PIN_C6 -to SDRAM_DQ[9]
set_location_assignment PIN_B6 -to SDRAM_DQ[10]
set_location_assignment PIN_B5 -to SDRAM_DQ[11]
set_location_assignment PIN_A5 -to SDRAM_DQ[12]
set_location_assignment PIN_B3 -to SDRAM_DQ[13]
set_location_assignment PIN_A3 -to SDRAM_DQ[14]
set_location_assignment PIN_A2 -to SDRAM_DQ[15]
set_location_assignment PIN_A11 -to SDRAM_BA[0]
set_location_assignment PIN_B12 -to SDRAM_BA[1]
set_location_assignment PIN_C9 -to SDRAM_DQMH
set_location_assignment PIN_C8 -to SDRAM_DQML
set_location_assignment PIN_A10 -to SDRAM_nRAS
set_location_assignment PIN_B10 -to SDRAM_nCAS
set_location_assignment PIN_D8 -to SDRAM_nWE
set_location_assignment PIN_B11 -to SDRAM_nCS
set_location_assignment PIN_C11 -to SDRAM_CKE
set_location_assignment PIN_R4 -to SDRAM_CLK
set_location_assignment PIN_P1 -to TAPE_IN
set_location_assignment PIN_B1 -to UART_RXD
set_location_assignment PIN_D1 -to UART_TXD
set_location_assignment PLL_1 -to "pll:pll|altpll:altpll_component"

set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[*]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[*]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_BA[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_BA[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQMH
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQML
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nRAS
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nCAS
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nWE
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nCS
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[*]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[*]

set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_A[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_DQ[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_BA[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_DQML
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_DQMH
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_nRAS
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_nCAS
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_nWE
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_nCS
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_CKE
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to SDRAM_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to VGA_R[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to VGA_G[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to VGA_B[*]
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to VGA_HS
set_instance_assignment -name CURRENT_STRENGTH_NEW "MAXIMUM CURRENT" -to VGA_VS
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to AUDIO_L
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to AUDIO_R
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to SPI_DO
