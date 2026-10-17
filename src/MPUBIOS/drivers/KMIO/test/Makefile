# Host-side unit tests for mouse_math.h. Uses the host's own C compiler
# (NOT the riscv32 cross-compiler -- mouse_math.h is plain portable C with
# no MPU/hardware register access), so no toolchain setup is needed beyond
# whatever cc/clang/gcc is already on the machine.
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror

.PHONY: test clean

test: test_mouse_math
	./test_mouse_math

test_mouse_math: test_mouse_math.c ../mouse_math.h
	$(CC) $(CFLAGS) -o test_mouse_math test_mouse_math.c

clean:
	rm -f test_mouse_math
