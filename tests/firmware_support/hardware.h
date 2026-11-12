// Host-side stub of src/MPUBIOS/drivers/apf/hardware.h for unit tests.
// Every register access is routed through a mock function so tests can
// count exactly how many MMIO reads/writes the real firmware code performs.
#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <stdint.h>

uint32_t mock_afp_reg_read(int idx);
uint32_t mock_controller_key_read(int idx);
uint32_t mock_controller_joy_read(int idx);
uint32_t mock_controller_trig_read(int idx);
uint32_t mock_core_output_read(void);
void mock_core_output_write(uint32_t v);

#define AFP_REGISTOR(x)        mock_afp_reg_read(x)
#define CONTROLLER_KEY_REG(x)  mock_controller_key_read(x)
#define CONTROLLER_JOY_REG(x)  mock_controller_joy_read(x)
#define CONTROLLER_TRIG_REG(x) mock_controller_trig_read(x)

// CORE_OUTPUT_REGISTOR() is used as an lvalue by the firmware
// (read-modify-write), so it needs a proxy that counts both directions.
struct CoreOutputReg {
  operator uint32_t() const { return mock_core_output_read(); }
  CoreOutputReg& operator=(uint32_t v) { mock_core_output_write(v); return *this; }
};
#define CORE_OUTPUT_REGISTOR() CoreOutputReg{}

#endif
