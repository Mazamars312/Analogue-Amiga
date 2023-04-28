// #include <lib.h>
#include <stream.h>

int
sscanf (const unsigned char *buf, const char *fmt, ...)
{
	stream_buf_t u;
	va_list	args;
	int err;

	stropen (&u, (unsigned char*) buf, 0);
	va_start (args, fmt);
	err = vscanf (&u, fmt, args);
	va_end (args);
	return err;
}
