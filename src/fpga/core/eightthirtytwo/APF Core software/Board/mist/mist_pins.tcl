set_location_assignment PIN_7 -to LED
set_location_assignment PIN_144 -to VGA_R[5]
set_location_assignment PIN_143 -to VGA_R[4]
set_location_assignment PIN_142 -to VGA_R[3]
set_location_assignment PIN_141 -to VGA_R[2]
set_location_assignment PIN_137 -to VGA_R[1]
set_location_assignment PIN_135 -to VGA_R[0]
set_location_assignment PIN_133 -to VGA_B[5]
set_location_assignment PIN_132 -to VGA_B[4]
set_location_assignment PIN_125 -to VGA_B[3]
set_location_assignment PIN_121 -to VGA_B[2]
set_location_assignment PIN_120 -to VGA_B[1]
set_location_assignment PIN_115 -to VGA_B[0]
set_location_assignment PIN_114 -to VGA_G[5]
set_location_assignment PIN_113 -to VGA_G[4]
set_location_assignment PIN_112 -to VGA_G[3]
set_location_assignment PIN_111 -to VGA_G[2]
set_location_assignment PIN_110 -to VGA_G[1]
set_location_assignment PIN_106 -to VGA_G[0]
set_location_assignment PIN_136 -to VGA_VS
set_location_assignment PIN_119 -to VGA_HS
set_location_assignment PIN_65 -to AUDIO_L
set_location_assignment PIN_80 -to AUDIO_R
set_location_assignment PIN_46 -to UART_TX
set_location_assignment PIN_31 -to UART_RX
set_location_assignment PIN_105 -to SPI_DO
set_location_assignment PIN_88 -to SPI_DI
set_location_assignment PIN_126 -to SPI_SCK
set_location_assignment PIN_127 -to SPI_SS2
set_location_assignment PIN_91 -to SPI_SS3
set_location_assignment PIN_90 -to SPI_SS4
set_location_assignment PIN_13 -to CONF_DATA0

set_location_assignment PIN_49 -to SDRAM_A[0]
set_location_assignment PIN_44 -to SDRAM_A[1]
set_location_assignment PIN_42 -to SDRAM_A[2]
set_location_assignment PIN_39 -to SDRAM_A[3]
set_location_assignment PIN_4 -to SDRAM_A[4]
set_location_assignment PIN_6 -to SDRAM_A[5]
set_location_assignment PIN_8 -to SDRAM_A[6]
set_location_assignment PIN_10 -to SDRAM_A[7]
set_location_assignment PIN_11 -to SDRAM_A[8]
set_location_assignment PIN_28 -to SDRAM_A[9]
set_location_assignment PIN_50 -to SDRAM_A[10]
set_location_assignment PIN_30 -to SDRAM_A[11]
set_location_assignment PIN_32 -to SDRAM_A[12]
set_location_assignment PIN_83 -to SDRAM_DQ[0]
set_location_assignment PIN_79 -to SDRAM_DQ[1]
set_location_assignment PIN_77 -to SDRAM_DQ[2]
set_location_assignment PIN_76 -to SDRAM_DQ[3]
set_location_assignment PIN_72 -to SDRAM_DQ[4]
set_location_assignment PIN_71 -to SDRAM_DQ[5]
set_location_assignment PIN_69 -to SDRAM_DQ[6]
set_location_assignment PIN_68 -to SDRAM_DQ[7]
set_location_assignment PIN_86 -to SDRAM_DQ[8]
set_location_assignment PIN_87 -to SDRAM_DQ[9]
set_location_assignment PIN_98 -to SDRAM_DQ[10]
set_location_assignment PIN_99 -to SDRAM_DQ[11]
set_location_assignment PIN_100 -to SDRAM_DQ[12]
set_location_assignment PIN_101 -to SDRAM_DQ[13]
set_location_assignment PIN_103 -to SDRAM_DQ[14]
set_location_assignment PIN_104 -to SDRAM_DQ[15]
set_location_assignment PIN_58 -to SDRAM_BA[0]
set_location_assignment PIN_51 -to SDRAM_BA[1]
set_location_assignment PIN_85 -to SDRAM_DQMH
set_location_assignment PIN_67 -to SDRAM_DQML
set_location_assignment PIN_60 -to SDRAM_nRAS
set_location_assignment PIN_64 -to SDRAM_nCAS
set_location_assignment PIN_66 -to SDRAM_nWE
set_location_assignment PIN_59 -to SDRAM_nCS
set_location_assignment PIN_33 -to SDRAM_CKE
set_location_assignment PIN_43 -to SDRAM_CLK


set_location_assignment PIN_31 -to UART_RXD
set_location_assignment PIN_46 -to UART_TXD


set_global_assignment -name CRC_ERROR_OPEN_DRAIN OFF
set_global_assignment -name RESERVE_DATA0_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_DATA1_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_FLASH_NCE_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name RESERVE_DCLK_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name OUTPUT_IO_TIMING_NEAR_END_VMEAS "HALF VCCIO" -rise
set_global_assignment -name OUTPUT_IO_TIMING_NEAR_END_VMEAS "HALF VCCIO" -fall
set_global_assignment -name OUTPUT_IO_TIMING_FAR_END_VMEAS "HALF SIGNAL SWING" -rise
set_global_assignment -name OUTPUT_IO_TIMING_FAR_END_VMEAS "HALF SIGNAL SWING" -fall
set_global_assignment -name CYCLONEIII_CONFIGURATION_SCHEME "PASSIVE SERIAL"
set_global_assignment -name FORCE_CONFIGURATION_VCCIO ON
set_global_assignment -name ALLOW_POWER_UP_DONT_CARE ON
set_global_assignment -name SYNTH_TIMING_DRIVEN_SYNTHESIS ON
set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON


set_global_assignment -name ENABLE_SIGNALTAP OFF
set_global_assignment -name POWER_PRESET_COOLING_SOLUTION "NO HEAT SINK WITH STILL AIR"
set_global_assignment -name POWER_BOARD_THERMAL_MODEL "NONE (CONSERVATIVE)"
set_global_assignment -name TIMEQUEST_MULTICORNER_ANALYSIS ON
set_global_assignment -name STRATIX_DEVICE_IO_STANDARD "3.3-V LVTTL"
set_global_assignment -name CYCLONEII_OPTIMIZATION_TECHNIQUE SPEED
set_global_assignment -name PHYSICAL_SYNTHESIS_ASYNCHRONOUS_SIGNAL_PIPELINING OFF
set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_DUPLICATION ON
set_global_assignment -name PHYSICAL_SYNTHESIS_COMBO_LOGIC_FOR_AREA ON
set_global_assignment -name PHYSICAL_SYNTHESIS_MAP_LOGIC_TO_MEMORY_FOR_AREA ON
set_global_assignment -name PHYSICAL_SYNTHESIS_COMBO_LOGIC ON
set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_RETIMING ON
set_location_assignment PLL_1 -to pll|altpll_component|auto_generated|pll1
set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id Top
set_global_assignment -name PARTITION_NETLIST_TYPE SOURCE -section_id Top
set_global_assignment -name PARTITION_COLOR 2147039 -section_id Top
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
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[2]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[3]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[4]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[5]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[6]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[7]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[8]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[9]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[10]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[11]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[12]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[13]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[14]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQ[15]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[2]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[3]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[4]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[5]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[6]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[7]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[8]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[9]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[10]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[11]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_A[12]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_BA[0]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_BA[1]
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQMH
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_DQML
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nRAS
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nCAS
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nWE
set_instance_assignment -name FAST_OUTPUT_REGISTER ON -to SDRAM_nCS
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[0]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[1]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[2]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[3]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[4]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[5]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[6]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[7]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[8]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[9]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[10]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[11]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[12]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[13]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[14]
set_instance_assignment -name FAST_OUTPUT_ENABLE_REGISTER ON -to SDRAM_DQ[15]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[0]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[1]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[2]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[3]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[4]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[5]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[6]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[7]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[8]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[9]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[10]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[11]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[12]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[13]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[14]
set_instance_assignment -name FAST_INPUT_REGISTER ON -to SDRAM_DQ[15]
set_instance_assignment -name GLOBAL_SIGNAL "GLOBAL CLOCK" -to SPI_SCK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to CONF_DATA0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to LED
set_location_assignment PIN_54 -to CLOCK_27

