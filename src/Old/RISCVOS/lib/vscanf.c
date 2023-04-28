/*
 * Scaled down version of scanf(3).
 * Based on Xinu sources, heavily rewritten.
 *
 * Copyright (C) 2000-2002 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
// #include <runtime/lib.h>
#include <stream.h>

#define	SHORT		0
#define	REGULAR		1
// #define	LONG		2

#define ISSPACE(c)	(c==' ' || c=='\t' || c=='\n')
#define ISDIGIT(c,base)	((c>='0' && c<='9') || (base==16 && \
			(('a'<=c && c<='f') || ('A'<=c && c<='F'))))

static unsigned char
scan_number (stream_t *stream, void *ptr, unsigned char base,
	unsigned short len, unsigned char size)
{
	int c;
	long val;
	unsigned char negate;

	/* Skip spaces. */
	for (;;) {
		c = peekchar (stream);
		if (c < 0)
			return 0;
		if (! ISSPACE ((unsigned char) c))
			break;
		getchar (stream);
	}

	/* Get sign. */
	negate = 0;
	if (c == '-' || c == '+') {
		if (c == '-')
			negate = 1;
		getchar (stream);
		len--;
		if (len == 0 || feof (stream))
			return 0;
		c = peekchar (stream);
	}
	if (! ISDIGIT (c, base))
		return 0;

	val = 0;
        while (len-- > 0) {
		/* Scan digits. */
		if (! ISDIGIT (c, base))
			break;

		if (ptr) {
			/* Get next digit. */
			if (base == 8)	     val <<= 3;
			else if (base == 10) val *= 10;
			else		     val <<= 4;

			if      ('0' <= c && c <= '9') c -= '0';
			else if ('a' <= c && c <= 'f') c -= 'a' - 10;
			else			       c -= 'A' - 10;

			val += c;
		}
		getchar (stream);
		c = peekchar (stream);
		if (c < 0)
			break;
	}
	if (! ptr)
		return 0;

	if (negate)
		val = -val;

        switch (size) {
        case SHORT:
		*(short*) ptr = val;
		break;

        case REGULAR:
		*(int*) ptr = val;
		break;

    //     case LONG:
		// *(long*) ptr = val;
		// break;
	}
	return 1;
}

static unsigned char
match_pattern (unsigned char sym, const char *pattern)
{
	unsigned char next, inversion, cstart, cend;

	inversion = 0;
	next = FETCH_BYTE (pattern++);
	if (next == '^') {
		inversion = 1;
		next = FETCH_BYTE (pattern++);
	}
	while (next != ']') {
		if (next == '\0')
			return 0;		/* unterminated [ */

		cstart = cend = next;
		next = FETCH_BYTE (pattern++);
		if (next == '-' && (cend = FETCH_BYTE (pattern)) != ']')  {
			if (cend == '\0')
				return 0;
			++pattern;
			next = FETCH_BYTE (pattern++);
		}

		if (sym >= cstart && sym <= cend)
			return ! inversion;
	}
	return inversion;
}

static unsigned char
scan_string (stream_t *stream, char *ptr, unsigned char type,
	unsigned short len, const char *pattern)
{
	unsigned char ch = 0;
	char *optr;

	optr = ptr;
	while (! feof (stream)) {
		ch = getchar (stream);

		/* Skip spaces, for 's'-format. */
		if (type != 's' || ! ISSPACE (ch))
			break;
	}
	while (! feof (stream)) {
		if (type == 's') {
			if (ISSPACE (ch))
				break;
		} else if (type == '[') {
			if (! match_pattern (ch, pattern))
				break;
		}
		if (ptr)
			*ptr++ = ch;
		if (--len <= 0)
			break;
                ch = getchar (stream);
	}
	if (ptr != optr) {
		if (type != 'c')
			*ptr++ = '\0';
		return 1;
	}
	return 0;
}

/*
 * Scan and recognize input according to a format
 */
int
stream_vscanf (stream_t *stream,
	const char *fmt,		/* Format string for the scanf */
	va_list argp)			/* Arguments to scanf */
{
	unsigned char ch, size, base;
	unsigned short len;
	int nmatch, ic;
	void *ptr;
	const char *pattern = 0;

	nmatch = 0;
	for (;;) switch (ch = FETCH_BYTE (fmt++)) {
	case '\0':
		return nmatch;
	case '%':
		ch = FETCH_BYTE (fmt++);
		if (ch == '%')
			goto def;
		if (ch != '*')
			ptr = va_arg (argp, void*);
		else {
			ptr = 0;
			ch = FETCH_BYTE (fmt++);
		}
		len = 0;
		size = REGULAR;
		while (ch >= '0' && ch <= '9') {
			len = len*10 + ch - '0';
			ch = FETCH_BYTE (fmt++);
		}
		if (len == 0)
			len = 30000;

		// if (ch == 'l') {
		// 	ch = FETCH_BYTE (fmt++);
		// 	size = LONG;
		// } else
		if (ch == 'h') {
			size = SHORT;
			ch = FETCH_BYTE (fmt++);
		} else if (ch == '[') {
			pattern = fmt;
			ch = FETCH_BYTE (fmt);
			if (ch == '^')
				ch = FETCH_BYTE (++fmt);
			while (ch != 0 && ch != ']')
				ch = FETCH_BYTE (++fmt);
			if (ch != 0)
				++fmt;
			ch = '[';
		}

		// if (ch >= 'A' && ch <= 'Z') {
		// 	ch = ch + 'a' - 'A';
		// 	size = LONG;
		// }

		switch (ch) {
		case 0:
			return -1;
		case 'c':
			if (len == 30000)
				len = 1;
			goto string;
		case 's':
		case '[':
string:			if (scan_string (stream, (char*) ptr, ch, len,
			    pattern) && ptr)
				++nmatch;
			break;
		case 'o':
			base = 8;
			goto number;
		case 'x':
			base = 16;
			goto number;
		case 'd':
			base = 10;
number:			if (scan_number (stream, ptr, base, len, size) && ptr)
				++nmatch;
			break;
		}

		if (feof (stream))
			return nmatch ? nmatch : -1;
		break;

	case ' ':
	case '\n':
	case '\t':
		/* Skip spaces. */
		for (;;) {
			ic = peekchar (stream);
			if (ic < 0)
				break;
			if (! ISSPACE ((unsigned char) ic))
				break;
			getchar (stream);
		}
		break;

	default:
def:		ic = peekchar (stream);
		if (ic < 0)
			return -1;
		if ((unsigned char) ic != ch)
			return nmatch;
		getchar (stream);
	}
}
