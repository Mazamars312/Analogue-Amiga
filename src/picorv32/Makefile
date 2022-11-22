# Copyright 2018 Jacob Lifshay
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

.PHONY: all clean

LIBGCC := $(shell riscv32-unknown-elf-g++ -print-libgcc-file-name)
LIBGCC_DIR := $(dir $(LIBGCC))

#all: ram0_byte0.hex ../output.bit emulated
#../output.bit: ram.elf ../main.bit
	#bash -c '. /opt/Xilinx/14.7/ISE_DS/settings64.sh; data2mem -bm ../cpu.bmm -bd ram.elf -bt ../main.bit -o b ../output.bit'

all: ram.bin

ram0_byte0.hex: ram.bin generate_hex_files.sh Makefile
	#./generate_hex_files.sh

generate_hex_files.sh: #make_block_memory.sh
	#./make_block_memory.sh -s

ram.bin: ram-stripped.elf Makefile
	riscv32-unknown-elf-objcopy -O binary --pad-to 0x4000 ram.elf ram.bin
	du -sh ram.bin

OBJECTS := main.o \
           interrupts.o \
		   timer.o \
           start.o \
		   spi.o \
		   fpga_io.o \
           uart.o \
           apf.o \
           startup.o

ram-stripped.elf: Makefile ram.elf
	riscv32-unknown-elf-strip -o ram-stripped.elf ram.elf

ram.elf: $(OBJECTS) Makefile ram.ld
	riscv32-unknown-elf-ld -o ram.elf $(OBJECTS) -static -T ram.ld -L$(LIBGCC_DIR) -L/opt/riscv/riscv32-unknown-elf/lib -lgcc

startup.o: startup.S Makefile
	riscv32-unknown-elf-g++ -c -o startup.o startup.S -march=rv32i -mabi=ilp32

main.o: main.cpp Makefile
start.o: start.cpp Makefile
interrupts.o: interrupts.cpp Makefile
timer.o: timer.cpp Makefile
uart.o: uart.cpp Makefile
spi.o: spi.cpp Makefile
fpga_io.o: fpga_io.cpp Makefile
apf.o: apf.cpp Makefile

main-emulated.o: main.cpp Makefile
	g++ -g -c -o main-emulated.o -std=c++14 -Wall main.cpp -DEMULATE_TARGET -Wno-narrowing
	
emulated: main-emulated.o Makefile
	g++ -g -o emulated -std=c++14 -Wall main-emulated.o -static

%.o: %.cpp
	riscv32-unknown-elf-g++ -Os -c -o $@ $< -std=c++14 -Wall -march=rv32i -mabi=ilp32 -fno-exceptions

clean:
	rm -f ram*.hex ram.bin ram.elf ram-stripped.elf *.o emulated generate_hex_files.sh
