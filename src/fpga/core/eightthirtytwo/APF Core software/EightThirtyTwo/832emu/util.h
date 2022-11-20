/*
 * util.h
 * Provides miscellaneous utility functions - any idiom which crops up more
 * than a few times, but doesn't belong within another class or library ends up here.
 *
 * Copyright (c) 2004 by Alastair M. Robinson
 * Distributed under the terms of the GNU General Public License -
 * see the file named "COPYING" for more details.
 *
 */

#ifndef UTIL_H
#define UTIL_H

#include <string>

// Filesystem Utilities

// Function to creates a given directory if it doesn't already exist
bool CreateDirIfNeeded(const char *dirname);

// Functions to check whether a given file or directory exist
bool CheckFileExists(const char *filename);
#define CheckDirExists(x) CheckFileExists(x)

// CheckSettingsDir prepends the current home directory, attempts to create
// the given directory if it doesn't already exist.  Returns false only if
// the home directory isn't availble.
bool CheckSettingsDir(const char *dirname);

// Compares two files for binary equality
bool CompareFiles(const char *fn1,const char *fn2);

#ifdef WIN32
// Convert UTF8 string to wchar_t - only available on Win32
wchar_t *UTF8ToWChar(const char *in);
// And back again
char *WCharToUTF8(const wchar_t *in);
#endif

// Open a file from a utf-8-encoded filename.
// On Unix, this is a straight passthrough to fopen().
// On Win32 the filename is will converted to wchar_t, then opened with _wfopen 
FILE *FOpenUTF8(const char *name,const char *mode);
// Not yet implemented:
// if FOpenUTF8EnableTransation() has been called.
// This allows client code to cope with filenames from GTK+ (in UTF-8) and also
// from the command line.
// void FOpenUTF8EnableTranslation();
// void FOpenUTF8DisableTranslation();


// Filename handling utilities

// Attempts to construct a filename given a root, a suffix and an extension.
// Any extension on the root is removed if present.  Useful for creating
// output filenames of the form <root><channel>.tif or suchlike.
// Result must be free()ed when no longer needed.
char *BuildFilename(const char *root,const char *suffix,const char *fileext);

// Prepends the current working directory to a relative filename.
// Result should be free()ed when no longer needed.
char *BuildAbsoluteFilename(const char *fname);

// Given an input filename such as "out.tif", constructs a filename
// of the form out_<serialno>.tif.
// The max parameter, if provided, is used to set the number of digits which
// will be printed.
// The resulting string should be free()ed when no longer needed.
char *SerialiseFilename(const char *fname,int serialno,int max=0);

// Quote a string to make it safe to pass as a command line argument
std::string ShellQuote(std::string &in);
std::string ShellQuote(const char *in);


// Given a top-level directory, scans recursively looking for an executable,
// and returns its path, minus the executable.
std::string FindParent(std::string initialpath, std::string program);



// String handling utilities

// A variant of strdup() which is safe to use on a NULL string, in which case it returns
// a valid pointer to an empty string.  In all cases, the result must be free()ed when no
// longer needed.  In some cases this reduces the need for special-case handling.
char *SafeStrdup(const char *src);

// A "safe" version of strcat which returns a valid pointer to an empty string
// if both parameters are null, a copy of the non-null parameter if the other is
// null, or a newly-allocated string containing the concatenated parameters if
// both are valid.
// In all cases the result is in newly-allocated storage and may (and must!) be
// free()ed when no longer needed.
char *SafeStrcat(const char *str1,const char *str2);

// A variant of strcasecmp() which ignores spaces as well as case - such that
// "MyString" matches "Mystring", "myString" and "my string".
int StrcasecmpIgnoreSpaces(const char *str1,const char *str2);


// Miscellaneous utilities

// Tests a given string to determine whether it could plausibly be a hostname:port combination.
// If it is, stores the hostname part to the string pointer pointed to by hostname
// (which must later be free()ed), and the port portion to the int pointed to by port.
bool TestHostName(char *str,char **hostname,int *port);


// A random function which returns rand() % max, and is automatically seeded with the
// system time when the program starts.

int RandomSeeded(int max);


// Routine to copy UTF-8 characters into a buffer.
// Note, the buffer should be 4 times the number of characters to
// accommodate the worst-case scenario.
// Always null-terminates the result, so allow space for the terminating null.
void utf8ncpy(char *out,const char *in,unsigned int count);


// Routine to truncate a UTF-8 string to a specified length, with ellipsis to
// indicate truncation.
std::string TruncateUTF8(std::string in,unsigned int count);

#endif
