#ifndef SETJMP_H
#define SETJMP_H

typedef struct jmp_buf_s { int storage[5]; } jmp_buf[1];

int setjmp(jmp_buf env);
int longjmp(jmp_buf env, int val);

#endif

