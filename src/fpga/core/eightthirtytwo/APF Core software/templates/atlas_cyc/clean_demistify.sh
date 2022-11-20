#demistify
#find ../firmware/ -type f \( ! -iname "config.h" ! -iname "overrides.c" \) -delete
rm -f MiSTery_*
#quartus
find output_files/ -type f \(  ! -iname "*.sof"  ! -iname "*.svf" ! -iname "*.rbf" ! -iname "*.flow.rpt" \) -delete
find software/spl_bsp/ -type f -not -name '*.bin' -delete
#rm -rf output_files
#rm -rf software
rm -rf software/spl_bsp/generated
rm -rf software/spl_bsp/uboot-socfpga
find soc_hps/synthesis -type f -not -name '*.qip' -delete
rm -rf soc_hps/synthesis/submodules
rm -f soc_hps/*
#rm -rf soc_hps
find soc_system/synthesis -type f -not -name '*.qip' -delete
rm -rf soc_system/synthesis/submodules
rm -f soc_system/*
#rm -rf soc_system
rm -rf db
rm -rf incremental_db
rm -rf hps_isw_handoff
rm -rf .qsys_edit
rm -f *.rpt
rm -f *.tcl~
rm -f *.csv
rm -f *.qws
rm -f *.rbf
rm -f *.sopcinfo
rm -f c5_pin*.txt
rm -f hps_sdram_p0*.*
rm -f *.bak
rm -f rtl/*.bak
rm -f V/*.bak
rm -rf greybox_tmp 
rm -rf *.log
