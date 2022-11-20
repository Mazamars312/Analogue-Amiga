#ifndef ARCFILE_H
#define ARCFILE_H

#include "minfat.h"

enum arcparam { ARC_PREAMBLE=0,ARC_PREAMBLE2,ARC_PREAMBLE3,ARC_NONE,ARC_CONF,ARC_MOD, ARC_NAME, ARC_DEFAULT };

struct arcfilestate {
	fileTYPE file;
	unsigned char matchidx[4];
	enum arcparam param;
	int mod;
	int def;
	int nameidx;
	char name[12];
};

extern struct arcfilestate arcstate;


int arcfile_open(const char *filename);
void arcfile_begin();
char arcfile_next();

#endif

