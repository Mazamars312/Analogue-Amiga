#!/bin/bash

cat $1 | while read a; do
	b=${a,,}
	if [ "${b: -4}" = ".rom" ]; then
		echo "lappend vhdl_files [file normalize \"$2/${a%.rom}_word.vhd\"]"
	fi
	if [ "${b: -4}" = ".vhd" ]; then
		echo "lappend vhdl_files [file normalize \"$2/${a}\"]"
	fi
	if [ "${b: -2}" = ".v" ]; then
		echo "lappend verilog_files [file normalize \"$2/${a}\"]"
	fi
	if [ "${b: -3}" = ".vh" ]; then
		echo "lappend verilog_files [file normalize \"$2/${a}\"]"
	fi
	if [ "${b: -3}" = ".sv" ]; then
		echo "lappend verilog_files [file normalize \"$2/${a}\"]"
	fi
	if [ "${b: -4}" = ".svh" ]; then
		echo "lappend verilog_files [file normalize \"$2/${a}\"]"
	fi
	if [ "${b: -4}" = ".qip" ]; then
		bash $(dirname $0)/expandtemplate_yosys.sh $2/${a%.qip}.files $2/$(dirname $a)
	fi
	if [ "${b: -4}" = ".tcl" ]; then
		echo "lappend tcl_scripts [file normalize \"$2/${a}\"]"
	fi
done

