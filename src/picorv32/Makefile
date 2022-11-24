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
#../output.bit: mpu.elf ../main.bit
	#bash -c '. /opt/Xilinx/14.7/ISE_DS/settings64.sh; data2mem -bm ../cpu.bmm -bd mpu.elf -bt ../main.bit -o b ../output.bit'

all: mpu.bin

ram0_byte0.hex: mpu.bin generate_hex_files.sh Makefile
	#./generate_hex_files.sh

generate_hex_files.sh: #make_block_memory.sh
	#./make_block_memory.sh -s

mpu.bin: mpu-stripped.elf Makefile
	riscv32-unknown-elf-objcopy -O binary --pad-to 0x800 mpu.elf mpu.bin
	du -sh mpu.bin

OBJECTS := main.o \
           interrupts.o \
		   timer.o \
           start.o \
		   spi.o \
		   printf.o \
           uart.o \
           apf.o \
           minimig.o \
           minimig_fdd.o \
           startup.o

mpu-stripped.elf: Makefile mpu.elf
	riscv32-unknown-elf-strip -o mpu-stripped.elf mpu.elf

mpu.elf: $(OBJECTS) Makefile mpu.ld
	riscv32-unknown-elf-ld -o mpu.elf $(OBJECTS) -static -T mpu.ld -L$(LIBGCC_DIR) -L/opt/riscv/riscv32-unknown-elf/lib -lgcc

startup.o: startup.S Makefile
	riscv32-unknown-elf-g++ -c -o startup.o startup.S -march=rv32i -mabi=ilp32

main.o: main.cpp Makefile
start.o: start.cpp Makefile
interrupts.o: interrupts.cpp Makefile
timer.o: timer.cpp Makefile
uart.o: uart.cpp Makefile
spi.o: spi.cpp Makefile
apf.o: apf.cpp Makefile
printf.o: printf.cpp Makefile
minimig.o: minimig.cpp Makefile
minimig_fdd.o: minimig_fdd.cpp Makefile

main-emulated.o: main.cpp Makefile
	g++ -g -c -o main-emulated.o -std=c++14 -Wall main.cpp -DEMULATE_TARGET -Wno-narrowing
	
emulated: main-emulated.o Makefile
	g++ -g -o emulated -std=c++14 -Wall main-emulated.o -static

%.o: %.cpp
	riscv32-unknown-elf-g++ -Os -c -o $@ $< -std=c++14 -Wall -march=rv32i -mabi=ilp32 -fno-exceptions

clean:
	rm -f mpu*.hex mpu.bin mpu.elf mpu-stripped.elf *.o emulated generate_hex_files.sh
