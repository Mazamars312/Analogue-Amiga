/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#ifndef MINIMIG_CONFIG_H
#define MINIMIG_CONFIG_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// ---- Minimig v2 constants -------
#define UIO_MM2_RST     0xF0
#define UIO_MM2_AUD     0xF1
#define UIO_MM2_CHIP    0xF2
#define UIO_MM2_CPU     0xF3
#define UIO_MM2_MEM     0xF4
#define UIO_MM2_VID     0xF5
#define UIO_MM2_FLP     0xF6
#define UIO_MM2_HDD     0xF7
#define UIO_MM2_JOY     0xF8
#define UIO_MM2_MIR     0xF9

void minimig_restart_first();
void minimig_restart_running_core();

#ifdef __cplusplus
}
#endif
#endif
