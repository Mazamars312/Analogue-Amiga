#include <stddef.h>
#include <string_asm.h>

#define voidstarvoidstarsizet(x) x(const void *a,const void *b,size_t c) { return(__##x(a,b,c)); }
#define voidstarncvoidstarsizet(x) x(void *a,const void *b,size_t c) { return(__##x(a,b,c)); }

int voidstarvoidstarsizet(memcmp)
int voidstarvoidstarsizet(memmove)
int voidstarvoidstarsizet(strncmp)
//void *voidstarncvoidstarsizet(memcpy)
void *voidstarncvoidstarsizet(strncpy)
void *voidstarvoidstarsizet(strncat)

#define voidstarvoidstar(x) x(const void *a,const void *b) { return(__##x(a,b)); }

char *voidstarvoidstar(strcat)
//char *voidstarvoidstar(strcpy)
int voidstarvoidstar(strcmp)

void memset(void *a,int b, size_t c) { __memset(a,b,c); }
size_t strlen(const char *a) { return(__strlen(a)); }
int toupper(int c) { return(__toupper(c)); }
int tolower(int c) { return(__tolower(c)); }

