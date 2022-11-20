#ifndef STDINT_H
#define STDINT_H

#include <sys/types.h>

typedef long intmax_t;
typedef unsigned long uintmax_t;

typedef char int_least8_t;
typedef unsigned char uint_least8_t;
typedef short int_least16_t;
typedef unsigned short uint_least16_t;
typedef int int_least32_t;
typedef unsigned int uint_least32_t;

typedef int int_fast8_t;
typedef unsigned int uint_fast8_t;
typedef int int_fast16_t;
typedef unsigned int uint_fast16_t;
typedef int int_fast32_t;
typedef unsigned int uint_fast32_t;

typedef unsigned int uintptr_t;

#endif

