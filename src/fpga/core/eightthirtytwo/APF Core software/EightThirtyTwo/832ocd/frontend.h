#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>

int frontend_choice(WINDOW *w,const char *prompt,const char *options,char def);
int frontend_confirm();
char *frontend_getinput(WINDOW *w,const char *prompt,int base);

#endif

