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
