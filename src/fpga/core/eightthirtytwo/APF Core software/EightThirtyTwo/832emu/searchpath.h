#ifndef SEARCHPATH_H
#define SEARCHPATH_H

#include <list>

#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <ostream>

/*
	SearchPathHandler - a class to handle multiple search paths.
	Written by Alastair M. Robinson to handle PhotoPrint's ICC profiles.

	The following public functions are provided:

	char *SearchPaths(const char *file)
		Returns an absolute path, given a relative path, or NULL for failure.
		If the supplied path is already absolute, and the referenced file exists, a
		strdup()ed version of it will be returned.
		Result must be free()d.
	
	char *MakeRelative(const char *file)
		Attempts to reduce an absolute path into a relative path, matching against one of the
		current search paths,such that a subsequent call to	Search() will find the file.
		May return either an absolute or relative path, which must be free()d.
		
	void AddPath(const char *path)
		Adds a new search path to the list.  UNIX-style multiple paths are supported
		with ':' as the separator character (Use ; on Windows).
		$HOME at the start of a path will be replaced by the contents of the HOME environment variable.
		
	void RemovePath(const char *path);
		Attempts to remove the specified path from the list.  Fails silently if
		the path isn't there!

	void ClearPaths()
		Removes all paths.

	const char *GetNextFilename(const char *last);
		A simple method of stepping through all the files within the searchpaths.
		Provide NULL as an argument for the first call, and the returned value for
		subsequent calls.
		Returns NULL when there are no more files.

	const char *GetNextPath(const char *last);
		Returns each path in turn.  Provide NULL as an argument for the first call,
		and the returned value for subsequent calls.

	char *GetPaths();
		Returns a Path specification in PATH1:PATH2:PATH3:... form

	TODO: Make path separation character runtime definable for Win32 support.
	2007-07-29: Added a conditional define to handle that at built time...

	2008-07-15: Renamed class to SearchPathHandler to avoid Win32 name clash

*/


#ifdef WIN32
#define SEARCHPATH_DELIMITER_C ';'
#define SEARCHPATH_DELIMITER_S ";"
#define SEARCHPATH_SEPARATOR '\\'
#define SEARCHPATH_SEPARATOR_S "\\"
#else
#define SEARCHPATH_DELIMITER_C ':'
#define SEARCHPATH_DELIMITER_S ":"
#define SEARCHPATH_SEPARATOR '/'
#define SEARCHPATH_SEPARATOR_S "/"
#endif


class SearchPathInstance;
class SearchPathHandler;

class SearchPathIterator
{
	public:
	SearchPathIterator(SearchPathHandler &header);
	virtual ~SearchPathIterator();
	virtual const char *GetNextFilename(const char *last);
	const char *GetNextPath(const char *last);
	protected:
	SearchPathHandler &header;
	std::list<SearchPathInstance *>::iterator spiterator;
#ifdef WIN32
//	_WDIR *searchdirectory;
#else
//	DIR *searchdirectory;
#endif
//	char *searchfilename;
	std::string searchfn;
	DirTreeWalker *toplevel;
	DirTreeWalker *subdirs;
};


class SearchPathHandler
{
	public:
	SearchPathHandler();
	SearchPathHandler(SearchPathHandler &other);
	SearchPathHandler &operator=(SearchPathHandler &other);
	virtual ~SearchPathHandler();
	virtual char *SearchPaths(const char *file,bool recursive=true);
	virtual char *MakeRelative(const char *file);
	virtual void AddPath(const char *path);
	virtual void RemovePath(const char *path);
	virtual void ClearPaths();
	virtual char *GetPaths();
	protected:
	SearchPathInstance *FindPath(const char *path);
	std::list<SearchPathInstance *> paths;
	// Used by GetNextFilename();
//	DIR *searchdirectory;
//	char *searchfilename;
	SearchPathIterator *searchiterator;
	friend class SearchPathInstance;
	friend class SearchPathIterator;
	friend std::ostream& operator<<(std::ostream &s,SearchPathHandler &sp);
};

#endif
