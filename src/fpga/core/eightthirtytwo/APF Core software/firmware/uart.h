#ifndef UART_H
#define UART_H

/* Hardware registers for a supporting UART to the ZPUFlex project. */

#ifdef __cplusplus
extern "C" {
#endif

#define UARTBASE 0xFFFFFFC0
#define HW_UART(x) *(volatile unsigned int *)(UARTBASE+x)

#define REG_UART 0x0
#define REG_UART_RXINT 9
#define REG_UART_TXREADY 8

#ifndef DISABLE_UART_TX
int putchar(int c);
int puts(const char *msg);

#else
#define putchar(x) (x)
#define puts(x)
#endif

#ifndef DISABLE_UART_RX
char getserial();
#else
#define getserial 0
#endif

#ifdef __cplusplus
}
#endif

#endif

