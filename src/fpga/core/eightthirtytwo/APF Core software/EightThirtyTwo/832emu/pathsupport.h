#ifndef PATHSUPPORT_H
#define PATHSUPPORT_H

#include <string>

// Wrapper for either getenv("HOME") or g_get_home_dir().

const char *get_homedir();


// substitue_homedir(const char *path)
//
// Accepts a source path argument.  If the source path begins with $HOME" or "~"
// then the user's home directory is substituted.
// Whether or not the directory is substituted, the returned value is a new string
// which may be free()d when finished with.

char *substitute_homedir(const char *path);

// substitute_xdgdatahome(const char *path);
//
// Similar to substitute_homedir, but instead uses the XDG Base Directory specification.
char *substitute_xdgconfighome(const char *path);



// Given a top-level directory, extracts the last component, and matches against a prefix.
// Useful for locating an executable.
int MatchBaseName(const char *prefix,const char *path);


#endif
