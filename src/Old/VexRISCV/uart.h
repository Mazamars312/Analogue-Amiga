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

#ifndef UART_H
#define UART_H

/* Hardware registers for a supporting UART to the ZPUFlex project. */

#ifdef __cplusplus
extern "C" {
#endif

#define UARTBASE 0xFFFFFFC0
#define SETUARTBASE 0xFFFFFF94
#define HW_UART(x) *(volatile unsigned int *)(UARTBASE+x)
#define SET_UART(x) *(volatile unsigned int *)(SETUARTBASE+x)

#define REG_UART 0x0
#define REG_UART_RXINT 9
#define REG_UART_TXREADY 8
//
int putchar(int c);
int puts(const char *msg);
void SetUART(int sys_clock, int uart_rate);

char getserial();

#ifdef __cplusplus
}
#endif

#endif
