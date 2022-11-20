#ifndef PRINTF_H
#define PRINTF_H

#include <sys/types.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef DISABLE_PRINTF
#define printf(x,...) // FIXME - not compatible with vbcc
#define puts(x)
#else
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
#endif

#endif

