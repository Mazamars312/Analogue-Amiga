// A variation on Nicolai M. Josuttis' "fdstream.hpp",
// adapted by Alastair M. Robinson
// to solve the problem of opening files with UTF-8 filenames on Win32
// using Mingw.

// Original copyright mesage:
/*
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 * Version: Jul 28, 2002
 * History:
 *  Jul 28, 2002: bugfix memcpy() => memmove()
 *                fdinbuf::underflow(): cast for return statements
 *  Aug 05, 2001: first public version
 */

#ifndef HACKSTREAM_HPP
#define HACKSTREAM_HPP

#include <iostream>
#include <streambuf>
// for EOF, fopen and fread/frwite:
#include <cstdio>
// for memmove():
#include <cstring>

#include "util.h"


/************************************************************
 * fdostream
 * - a stream that writes on a file handle
 *
 * FIXME - initial version always operates in binary mode
 *
 ************************************************************/


class hack_outbuf : public std::streambuf {
  protected:
    FILE *file;    // file handle
  public:
    // constructor
    hack_outbuf (const char *utf8name,std::ios_base::openmode mode=std::ios_base::out) : file(NULL) {
		open(utf8name,mode);
    }
    hack_outbuf() : file(NULL) {
    }
	~hack_outbuf()
	{
		if(file)
			fclose(file);
	}
	bool is_open()
	{
		return(file!=NULL);
	}
	void close()
	{
		if(file)
			fclose(file);
		file=NULL;
	}
	virtual void open(const char *utf8name,std::ios_base::openmode mode=std::ios_base::out)
	{
		if(file)
			fclose(file);
		file=NULL;
		if(mode && std::ios_base::out)
		{
			if(mode && std::ios_base::binary)
				file=FOpenUTF8(utf8name,"wb");
			else
				file=FOpenUTF8(utf8name,"w");
		}
	}
  protected:
    // write one character
    virtual int_type overflow (int_type c) {
		if(!file)
			return EOF;
        if (c != EOF) {
            if (fputc (c, file) != c) {
                return EOF;
            }
        }
        return c;
    }
    // write multiple characters
    virtual
    std::streamsize xsputn (const char* s,
                            std::streamsize num) {
		if(file)
	        return fwrite(s,1,num,file);
		else
			return(0);
    }
	virtual
    int sync()
	{
        if(file)
           return(fflush(file));
		else
			return(0);
    }
};

class hack_ostream : public std::ostream, public hack_outbuf
{
	public:
	hack_ostream (const char *utf8name,std::ios_base::openmode mode=std::ios_base::out)
		: std::ostream(0), hack_outbuf(utf8name,mode)
	{
	    rdbuf(this);
	}
	hack_ostream() : std::ostream(0), hack_outbuf()
	{
	    rdbuf(this);
	}
	protected:
};


/************************************************************
 * fdistream
 * - a stream that reads on a file handle
 *
 *
 ************************************************************/

class hack_inbuf : public std::streambuf {
  protected:
    FILE *file;    // file handle
  protected:
    /* data buffer:
     * - at most, pbSize characters in putback area plus
     * - at most, bufSize characters in ordinary read buffer
     */
    static const int pbSize = 4;        // size of putback area
    static const int bufSize = 1024;    // size of the data buffer
    char buffer[bufSize+pbSize];        // data buffer

  public:
    hack_inbuf () : file(NULL)
	{
        setg (buffer+pbSize,     // beginning of putback area
              buffer+pbSize,     // read position
              buffer+pbSize);    // end position
	}
    hack_inbuf (const char *utf8fn, std::ios_base::openmode mode=std::ios_base::in) : file(NULL)
	{
		open(utf8fn,mode);
        setg (buffer+pbSize,     // beginning of putback area
              buffer+pbSize,     // read position
              buffer+pbSize);    // end position
    }
	~hack_inbuf()
	{
		if(file)
			fclose(file);
	}
	void open(const char * filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		if(file)
			fclose(file);
		file=NULL;
		if(mode && std::ios_base::in)
		{
			if(mode && std::ios_base::binary)
				file=FOpenUTF8(filename,"rb");
			else
				file=FOpenUTF8(filename,"r");
		}
	}
	bool is_open()
	{
		return(file!=NULL);
	}
  protected:
    // insert new characters into the buffer
    virtual int_type underflow () {
#ifndef _MSC_VER
        using std::memmove;
#endif

        // is read position before end of buffer?
        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());
        }

        /* process size of putback area
         * - use number of characters read
         * - but at most size of putback area
         */
        int numPutback;
        numPutback = gptr() - eback();
        if (numPutback > pbSize) {
            numPutback = pbSize;
        }

        /* copy up to pbSize characters previously read into
         * the putback area
         */
        memmove (buffer+(pbSize-numPutback), gptr()-numPutback,
                numPutback);

        // read at most bufSize new characters
        int num;
        num = fread (buffer+pbSize, 1, bufSize, file);
        if (num <= 0) {
            // ERROR or EOF
            return EOF;
        }

        // reset buffer pointers
        setg (buffer+(pbSize-numPutback),   // beginning of putback area
              buffer+pbSize,                // read position
              buffer+pbSize+num);           // end of buffer

        // return next character
        return traits_type::to_int_type(*gptr());
    }
};

class hack_istream : public std::istream {
	public:
	hack_istream (const char *utf8fn, std::ios_base::openmode mode=std::ios_base::in) : std::istream(0), buf(utf8fn,mode)
	{
		rdbuf(&buf);
	}
	protected:
	hack_inbuf buf;
};


#endif
