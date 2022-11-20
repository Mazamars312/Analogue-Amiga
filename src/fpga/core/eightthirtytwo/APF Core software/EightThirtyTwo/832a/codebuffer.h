#ifndef CODEBUFFER_H
#define CODEBUFFER_H

/* CODEBUFFERSIZE must be a power of 2 */
#define CODEBUFFERSIZE 4096

struct codebuffer
{
	struct codebuffer *next;
	char *buffer;
	int cursor;
};

struct codebuffer *codebuffer_new();
void codebuffer_delete(struct codebuffer *buf);

int codebuffer_put(struct codebuffer *buf,int val);
int codebuffer_write(struct codebuffer *buf,const char *data,int size);

void codebuffer_dump(struct codebuffer *buf);

void codebuffer_loadchunk(struct codebuffer *buf,int bytes,FILE *f);
void codebuffer_output(struct codebuffer *buf,FILE *f);

#endif

