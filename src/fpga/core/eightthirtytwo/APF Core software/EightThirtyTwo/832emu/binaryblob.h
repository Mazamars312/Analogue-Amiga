#ifndef BINARYBLOB_H
#define BINARYBLOB_H
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "debug.h"

#include "util.h"

// Class to handle the loading of a binary blob from a file, taking care of such tedious details as
// determining the filesize, and translating the filename from UTF8 to wchar_t if on Windows.

class BinaryBlob
{
	public:
	BinaryBlob() : pointer(NULL), size(0), owned(false)
	{
	}
	BinaryBlob(const char *filename) : pointer(NULL), size(0), owned(false)
	{
		Load(filename);
	}
	BinaryBlob(const char *buffer,int bufsize) : pointer(NULL), size(bufsize), owned(false)
	{
		pointer=(unsigned char *)malloc(size);
		memcpy(pointer,buffer,size);
		owned=true;
	}
	virtual ~BinaryBlob()
	{
		if(owned && pointer)
			free(pointer);
	}
	virtual unsigned char *Load(const char *filename)
	{
		if(owned && pointer)
			free(pointer);
		pointer=NULL;

		FILE *f;
		if(!(f = FOpenUTF8(filename, "rb")))
			throw "Can't open file";
		
		fseek(f,0,SEEK_END);
		size=ftell(f);
		fseek(f,0,SEEK_SET);

		Debug[TRACE] << "Loading binary blob " << filename << " of size: " << size << std::endl;

		pointer=(unsigned char *)malloc((size+3)&~3); 		// HACK - round up to the nearest longword boundary.
		owned=true;
		size_t readlen = fread(pointer, 1, size, f);
		fclose(f);
		if(readlen!=size)
			throw "Binary blob reading failed";
		return(pointer);
	}
	virtual void Save(const char *filename)
	{
		if(!pointer)
			throw "Binary blob: no data yet loaded!";

		FILE *f;
		if(!(f = FOpenUTF8(filename, "wb")))
			throw "Can't open file";
		
		Debug[TRACE] << "Saving binary blog of size " << size << std::endl;

		size_t wlen = fwrite(pointer, 1, size, f);
		fclose(f);
		if(wlen!=size)
			throw "Binary blob: write failed";
	}
	unsigned char *GetPointer()
	{
		return(pointer);
	}
	int GetSize()
	{
		return(size);
	}
	// Relinquish ownership of the blob.  The caller will then be reponsible for
	// free()ing the data when done with it;
	unsigned char *Relinquish()
	{
		owned=false;
		return(pointer);
	}
	unsigned char &operator[](int idx)
	{
		return(pointer[idx]);
	}
	protected:
	unsigned char *pointer;
	size_t size;
	bool owned;
};

#endif

