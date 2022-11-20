#ifndef CONFIGSTRING_H
#define CONFIGSTRING_H

#define CONFIGSTRING_INDEX_CFG 0x1f
#define CONFIGSTRING_INDEX_ARC 0x1e

extern unsigned char configstring_index;
extern unsigned char configstring_coretype;

void configstring_begin();
void configstring_end();
int configstring_next();
int configstring_nextfield();
int configstring_copytocomma(char *buf, int limit,int copy);
int configstring_getdigit();
int configstring_matchextension(const char *ext);
int configstring_setindex(const char *fn);

#endif

