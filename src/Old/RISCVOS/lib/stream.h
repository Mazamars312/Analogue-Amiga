#ifndef __STREAM_H_
#define __STREAM_H_ 1
#include <stdarg.h>
/*
 * Поток данных.
 * Имеет интерфейс, состоящий из четырех процедур:
 * выдача байта, опрос последнего выданного байта,
 * прием байта, и опрос первого байта во входящей очереди без удаления.
 */
 /*
  * Reading a byte from flash memory.
  * By default - handle it like a simple pointer.
  */
 #define FETCH_BYTE(p)	(*(unsigned char*)(p))
 #define FETCH_WORD(p)	(*(unsigned short*)(p))
 #define FETCH_LONG(p)	(*(unsigned long*)(p))
 #define FETCH_PTR(p)	({ void *x = (void*)(p); *(void**)x; })

 #define false 0
 #define true 1
 typedef int bool_t; // or #define bool int

typedef struct _stream_t {
	struct _stream_interface_t *interface;
} stream_t;

typedef struct _stream_interface_t {
	void (*putc) (stream_t *u, short c);
	unsigned short (*getc) (stream_t *u);
	int (*peekc) (stream_t *u);
	void (*flush) (stream_t *u);
	bool_t (*eof) (stream_t *u);
	void (*close) (stream_t *u);
	struct _mutex_t *(*receiver) (stream_t *u);
} stream_interface_t;

#define to_stream(x)   ((stream_t*)&(x)->interface)





/*
 * Методы приходится делать в виде макросов,
 * т.к. необходимо приведение типа к родительскому.
 */
#define putchar(x,s)    (x)->interface->putc(to_stream (x), s)
#define getchar(x)	(x)->interface->getc(to_stream (x))
#define peekchar(x)	(x)->interface->peekc(to_stream (x))
// #define fflush(x)       if ((x)->interface->flush) \
			(x)->interface->flush(to_stream (x))
#define feof(x)		((x)->interface->eof ? \
			(x)->interface->eof(to_stream (x)) : 0)
// #define fclose(x)       if ((x)->interface->close) \
			(x)->interface->close(to_stream (x))
// #define freceiver(x)	((x)->interface->receiver ? \
// 			(x)->interface->receiver(to_stream (x)) : 0)

#define puts(x,str)	stream_puts (to_stream (x), str)
#define gets(x,str,n)	stream_gets (to_stream (x), str, n)
// #define vprintf(x,f,a)	stream_vprintf (to_stream (x), f, a)
#define vscanf(x,f,a)	stream_vscanf (to_stream (x), f, a)

/* LY: умышленно вызываем ошибки там, где без необходимости вместо puts() используется printf() */
// #define printf(x,f,...) stream_printf (to_stream (x), f, ##__VA_ARGS__)

// void drain_input (stream_t *u); /* LY: чистит забуферизированный в потоке ввод. */
// int stream_puts (stream_t *u, const char *str);
// unsigned char *stream_gets (stream_t *u, unsigned char *str, int len);
// int stream_printf (stream_t *u, const char *fmt, ...);
// int stream_vprintf (stream_t *u, const char *fmt, va_list args);
// int stream_scanf (stream_t *u, const char *fmt, ...);
int stream_vscanf (stream_t *u, const char *fmt, va_list argp);

// int snprintf (unsigned char *buf, int size, const char *fmt, ...);
// int vsnprintf (unsigned char *buf, int size, const char *fmt, va_list args);
// int sscanf (const unsigned char *buf, const char *fmt, ...);

/*
 * Вывод в строку как в поток.
 * Для snprintf и т.п.
 */
typedef struct {
	const stream_interface_t *interface;
	unsigned char *buf;
	int size;
} stream_buf_t;

stream_t *stropen (stream_buf_t *u, unsigned char *buf, int size);
// void strclose (stream_buf_t *u);

/*
 * LY: выдает результат vprintf без печати, т.е. считает кол-во символов.
 */
// int vprintf_getlen (const char *fmt, va_list args);
// extern stream_t null_stream;

#endif /* __STREAM_H_ */
