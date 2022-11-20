set_global_assignment -name ORIGINAL_QUARTUS_VERSION 5.1
set_global_assignment -name PROJECT_CREATION_TIME_DATE "22:49:11  JANUARY 31, 2006"
set_global_assignment -name LAST_QUARTUS_VERSION "13.0 SP1"


# Pin & Location Assignments
# ==========================

# Timing Assignments
# ==================
set_global_assignment -name IGNORE_CLOCK_SETTINGS ON
set_global_assignment -name TPD_REQUIREMENT "5 ns"
set_global_assignment -name TSU_REQUIREMENT "5 ns"
set_global_assignment -name TCO_REQUIREMENT "5 ns"
set_global_assignment -name TH_REQUIREMENT "5 ns"
set_global_assignment -name FMAX_REQUIREMENT "101.58 MHz"

# Analysis & Synthesis Assignments
# ================================
set_global_assignment -name DEVICE_FILTER_PIN_COUNT 144
set_global_assignment -name DEVICE_FILTER_SPEED_GRADE 8
set_global_assignment -name FAMILY "Cyclone III"
set_global_assignment -name REMOVE_REDUNDANT_LOGIC_CELLS ON
set_global_assignment -name AUTO_IMPLEMENT_IN_ROM ON
set_global_assignment -name FLEX10K_OPTIMIZATION_TECHNIQUE AREA
set_global_assignment -name AUTO_RESOURCE_SHARING ON

# Fitter Assignments
# ==================
set_global_assignment -name DEVICE EP3C25E144C8
set_global_assignment -name ERROR_CHECK_FREQUENCY_DIVISOR 1
set_global_assignment -name FLEX10K_DEVICE_IO_STANDARD LVTTL/LVCMOS

# Assembler Assignments
# =====================
set_global_assignment -name ON_CHIP_BITSTREAM_DECOMPRESSION ON
set_global_assignment -name GENERATE_RBF_FILE ON
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files
set_global_assignment -name SMART_RECOMPILE ON
set_global_assignment -name ENABLE_DRC_SETTINGS OFF
set_global_assignment -name ENABLE_CLOCK_LATENCY ON
set_global_assignment -name ENABLE_RECOVERY_REMOVAL_ANALYSIS ON
set_global_assignment -name IGNORE_LCELL_BUFFERS ON
set_global_assignment -name ROUTER_TIMING_OPTIMIZATION_LEVEL MAXIMUM
set_global_assignment -name OPTIMIZE_HOLD_TIMING "ALL PATHS"
set_global_assignment -name FITTER_EFFORT "STANDARD FIT"
set_global_assignment -name PLACEMENT_EFFORT_MULTIPLIER 4.0
set_global_assignment -name ROUTER_EFFORT_MULTIPLIER 2.0
set_global_assignment -name OPTIMIZE_TIMING "NORMAL COMPILATION"
set_global_assignment -name FINAL_PLACEMENT_OPTIMIZATION ALWAYS
set_global_assignment -name AUTO_GLOBAL_MEMORY_CONTROLS ON
set_global_assignment -name AUTO_PACKED_REGISTERS "MINIMIZE AREA"
set_global_assignment -name ALLOW_ANY_RAM_SIZE_FOR_RECOGNITION OFF
set_global_assignment -name ALLOW_ANY_ROM_SIZE_FOR_RECOGNITION OFF
set_global_assignment -name ADD_PASS_THROUGH_LOGIC_TO_INFERRED_RAMS OFF
set_global_assignment -name IGNORE_CARRY_BUFFERS OFF
set_global_assignment -name IGNORE_CASCADE_BUFFERS OFF
set_global_assignment -name AUTO_GLOBAL_CLOCK ON
set_global_assignment -name AUTO_RAM_RECOGNITION ON
set_global_assignment -name AUTO_SHIFT_REGISTER_RECOGNITION AUTO
set_global_assignment -name STATE_MACHINE_PROCESSING AUTO
set_global_assignment -name FMAX_REQUIREMENT "34 MHz" -section_id clk32
set_global_assignment -name BLOCK_RAM_AND_MLAB_EQUIVALENT_PAUSED_READ_CAPABILITIES "DONT CARE"
set_global_assignment -name MIN_CORE_JUNCTION_TEMP 0
set_global_assignment -name MAX_CORE_JUNCTION_TEMP 85
set_global_assignment -name DEVICE_FILTER_PACKAGE TQFP

set_global_assignment -name USE_CONFIGURATION_DEVICE OFF
set_global_assignment -name CYCLONEII_RESERVE_NCEO_AFTER_CONFIGURATION "USE AS REGULAR IO"
set_global_assignment -name OPTIMIZE_POWER_DURING_FITTING OFF
set_global_assignment -name ADV_NETLIST_OPT_SYNTH_WYSIWYG_REMAP OFF
set_global_assignment -name PHYSICAL_SYNTHESIS_EFFORT NORMAL
set_global_assignment -name SEED 1
set_global_assignment -name OPTIMIZE_POWER_DURING_SYNTHESIS "NORMAL COMPILATION"
set_global_assignment -name PRE_MAPPING_RESYNTHESIS ON

