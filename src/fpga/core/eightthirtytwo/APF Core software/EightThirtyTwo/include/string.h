#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#include <string_asm.h>

/* Define aliases for assembly string functions */

#define memcmp __memcmp
#define memmove __memmove
#define memset __memset
#define strcat __strcat
#define strcmp __strcmp
#define strncmp __strncmp
#define strcasecmp __strcasecmp
#define strncasecmp __strncasecmp
#define strncpy __strncpy
#define strlen __strlen
#define strncat __strncat
#define toupper __toupper

char *strcpy(char *,const char *);
//void *memcpy(const void *,const void *,size_t);

size_t strspn(const char *, const char *);
size_t strcspn(const char *, const char *);

char *strtok(char *, const char *);
char *strtok_r(char *, const char *, char **);

/* These really belong in stdlib.h rather than string.h */

long int strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);

char *strchr(const char *, int);

/* Don't yet have implementations of these functions */

#if 0
 void *memccpy(void *, const void *, int, size_t);
 void *memchr(const void *, int, size_t);
 void *memrchr(const void *, int, size_t);
 void *memmem(const void *, size_t, const void *, size_t);
 void memswap(void *, void *, size_t);
 void bzero(void *, size_t);
 int strcasecmp(const char *, const char *);
 int strncasecmp(const char *, const char *, size_t);
 char *strchr(const char *, int);
 char *index(const char *, int);
 char *strrchr(const char *, int);
 char *rindex(const char *, int);
 char *strdup(const char *);
 char *strndup(const char *, size_t);
 char *strerror(int);
 char *strsignal(int);
 size_t strnlen(const char *, size_t);
 size_t strlcat(char *, const char *, size_t);
 size_t strlcpy(char *, const char *, size_t);
 char *strpbrk(const char *, const char *);
 char *strsep(char **, const char *);
 char *strstr(const char *, const char *);
#endif


#endif				/* _STRING_H */
