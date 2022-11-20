#!/bin/bash

# first argument is the relative path from project to root
# second argument is the list of files to add to the project.

cat $2 | while read a; do
	b=${a,,}
	if [ "${b: -4}" = ".rom" ]; then
		echo set_global_assignment -name VHDL_FILE ${1}${a%.rom}_byte.vhd
	fi
	if [ "${b: -4}" = ".vhd" ]; then
		echo set_global_assignment -name VHDL_FILE ${1}${a}
	fi
	if [ "${b: -4}" = ".qip" ]; then
		echo set_global_assignment -name QIP_FILE ${1}${a}
	fi
        if [ "${b: -4}" = ".sdc" ]; then
                echo set_global_assignment -name SDC_FILE ${1}${a}
        fi
	if [ "${b: -2}" = ".v" ]; then
		echo set_global_assignment -name VERILOG_FILE ${1}${a}
	fi
	if [ "${b: -3}" = ".sv" ]; then
		echo set_global_assignment -name SYSTEMVERILOG_FILE ${1}${a}
	fi
	if [ "${b: -4}" = ".tcl" ]; then
		echo set_global_assignment -name PRE_FLOW_SCRIPT_FILE "quartus_sh:${1}${a}"
	fi
done

