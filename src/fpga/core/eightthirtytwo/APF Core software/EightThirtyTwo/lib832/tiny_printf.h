#ifndef TINY_PRINTF_H
#define TINY_PRINTF_H

#ifdef DISABLE_PRINTF
#define tiny_printf(x,...)
#define printf(x,...)
#define puts(x)
#else
int printf(const char *fmt, ...);
#endif

#endif

