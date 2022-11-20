#ifndef STRING_ASM_H
#define STRING_ASM_H

void *memcpy(void *,void *,size_t);
int __memcmp(__reg("r3") const void *, __reg("r2") const void *, __reg("r1") size_t);
int __memmove(__reg("r3") const void *, __reg("r2") const void *, __reg("r1") size_t);
void *__memset(__reg("r3") void *, __reg("r2") int,__reg("r1") size_t);
char *__strcat(__reg("r2") char *, __reg("r1") const char *);
int __strcmp(__reg("r2") const char *, __reg("r1") const char *);
int __strcasecmp(__reg("r2") const char *, __reg("r1") const char *);
char *__strncpy(__reg("r3") char *, __reg("r2") const char *, __reg("r1") size_t);
size_t __strlen(__reg("r1") const char *);
char *__strncat(__reg("r2") char *, __reg("r1") const char *, __reg("r3") size_t);
int __strncmp(__reg("r2") const char *, __reg("r1") const char *, __reg("r3") size_t);
int __strncasecmp(__reg("r2") const char *, __reg("r1") const char *, __reg("r3") size_t);

#endif

