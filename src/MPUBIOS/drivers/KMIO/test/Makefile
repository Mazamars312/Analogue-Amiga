# Host-side unit tests for mouse_math.h. Uses the host's own C compiler
# (NOT the riscv32 cross-compiler -- mouse_math.h is plain portable C with
# no MPU/hardware register access), so no toolchain setup is needed beyond
# whatever cc/clang/gcc is already on the machine.
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror
CXX ?= c++
CXXFLAGS ?= -std=c++14 -Wall -Wextra

REPO_ROOT := ../../../../..

.PHONY: test clean

test: test_mouse_math test_input_update_firmware
	./test_mouse_math
	./test_input_update_firmware

test_mouse_math: test_mouse_math.c ../mouse_math.h
	$(CC) $(CFLAGS) -o test_mouse_math test_mouse_math.c

# Exercises the REAL input_update() from ../inputs.cpp (compiled against mock
# registers), pinning its AFP_REGISTOR(5) reads to one per poll. Complements
# the model-only tests in $(REPO_ROOT)/tests/test_mouse_inputs.cpp.
test_input_update_firmware: $(REPO_ROOT)/tests/test_input_update_firmware.cpp ../inputs.cpp
	$(CXX) $(CXXFLAGS) -I$(REPO_ROOT)/tests/firmware_support -I.. -o $@ $(REPO_ROOT)/tests/test_input_update_firmware.cpp

clean:
	rm -f test_mouse_math test_input_update_firmware
