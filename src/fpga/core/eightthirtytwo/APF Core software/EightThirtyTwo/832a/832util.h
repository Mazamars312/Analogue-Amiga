#ifndef UTIL832_H
#define UTIL832_H

enum eightthirtytwo_endian {EIGHTTHIRTYTWO_BIGENDIAN,EIGHTTHIRTYTWO_LITTLEENDIAN};

void setdebuglevel(int level);
int getdebuglevel();
void debug(int level,const char *fmt,...);
void hexdump(int level,char *p,int l);

void error_setfile(const char *fn);
void error_setline(int line);
void asmerror(const char *err);
void linkerror(const char *err);

void write_int(int i,FILE *f,enum eightthirtytwo_endian endian);
void write_int_le(int i,FILE *f);
void write_int_be(int i,FILE *f);
void write_short(int i,FILE *f,enum eightthirtytwo_endian endian);
void write_short_le(int i,FILE *f);
void write_short_be(int i,FILE *f);
void write_lstr(const char *str,FILE *f);

int read_int(FILE *f,enum eightthirtytwo_endian endian);
int read_int_le(FILE *f);
int read_int_be(FILE *f);
int read_short_le(FILE *f);
void read_lstr(FILE *f,char *ptr);

int count_constantchunks(long v);

char *strtok_escaped(char *str);
void parseescapes(char *str);

#ifdef  SUPPLY_POSIX
/* For the benefit of vbcc on Amiga */
#ifndef ssize_t
typedef int ssize_t;
#endif
char *strdup(const char *src);
int strcasecmp(const char *s1, const char *s2);
ssize_t getdelim (char **lineptr, size_t *n, char delim, FILE *fp);
#define getline(x,y,z) getdelim((x),(y),'\n',(z))
#endif

#endif

