#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include <libgen.h>
#include "config.h"

#ifdef HAVE_GTK
#include <glib.h>
#endif

#ifdef WIN32
#include <w32api.h>
#define _WIN32_IE IE5
#define _WIN32_WINNT Windows2000
#include <shlobj.h>
#endif

#include "util.h"
#include "debug.h"
#include "pathsupport.h"

using namespace std;


const char *get_homedir()
{
//	return(getenv("HOME"));
#ifdef WIN32
	static wchar_t homedir_w[MAX_PATH]={0};
	static char homedir[MAX_PATH]={0};
	static bool init=false;
	if(!init)
	{
		SHGetFolderPathW(NULL,CSIDL_APPDATA,NULL,SHGFP_TYPE(SHGFP_TYPE_CURRENT),homedir_w);
		char *utf=WCharToUTF8(homedir_w);
		strncpy(homedir,utf,MAX_PATH);
		free(utf);
		init=true;
	}
	return(homedir);
#else
#ifdef HAVE_GTK
	return(g_get_home_dir());
#else
	return(getenv("HOME"));
#endif
#endif
}


char *substitute_homedir(const char *path)
{
	char *result=NULL;
	const char *subst=NULL;
	if(path)
	{
		// First try to substitute a "$HOME_PICTURES" path.  On Win32 this will be My Pictures.
		// On UNIX we'll just use $HOME for now.
		if(strncmp(path,"$HOME_PICTURES",14)==0)
		{
			path+=14;
#ifdef WIN32
			static wchar_t pixdir_w[MAX_PATH]={0};
			static char pixdir[MAX_PATH]={0};
			static bool init=false;
			if(!init)
			{
				SHGetFolderPathW(NULL,CSIDL_COMMON_PICTURES,NULL,SHGFP_TYPE(SHGFP_TYPE_CURRENT),pixdir_w);
				char *utf=WCharToUTF8(pixdir_w);
				strncpy(pixdir,utf,MAX_PATH);
				free(utf);
				init=true;
			}
			subst=pixdir;
#else
			subst=get_homedir();
#endif
		}
		else if(path[0]=='~')
		{
			++path;
			subst=get_homedir();
		}
		else if(strncmp(path,"$HOME",5)==0)
		{
			path+=5;
			subst=get_homedir();
		}
		else	// No substitution to be done...
			return(strdup(path));

		if(path[0]=='/' || path[0]=='\\')
			++path;

		// If we get this far, then we need to substitute - and path now points
		// to the beginning of the path proper...

		if(subst && strlen(subst))
		{
			result=(char *)malloc(strlen(path)+strlen(subst)+2);	
			sprintf(result,"%s%c%s",subst,SEARCHPATH_SEPARATOR,path);
		}
		else
			result=strdup(path);
	}
	return(result);
}


char *substitute_xdgconfighome(const char *path)
{
	const char *envvar="$XDG_CONFIG_HOME";
	char *result=NULL;
	if(path)
	{
		if(path[0]=='~')
			++path;

		else if(strncmp(path,envvar,strlen(envvar))==0)
			path+=strlen(envvar);

		else	// No substitution to be done...
			return(strdup(path));

		if(path[0]=='/' || path[0]=='\\')
			++path;

		// If we get this far, then we need to substitute - and path now points
		// to the beginning of the path proper...
		const char *hd=NULL;
		if((hd=getenv(envvar+1)))
		{
//			Debug[TRACE] << "Got XDG_CONFIG_HOME: " << hd << endl;
			result=(char *)malloc(strlen(path)+strlen(hd)+2);
			sprintf(result,"%s%c%s",hd,SEARCHPATH_SEPARATOR,path);
		}
		else
		{
//			Debug[TRACE] << "No XDG_CONFIG_HOME set - using $HOME/.config instead" << endl;
			const char *hd=get_homedir();
			if(hd && strlen(hd))
			{
				result=(char *)malloc(strlen(hd)+strlen("/.config/")+strlen(path)+strlen(hd)+2);
				sprintf(result,"%s%c.config%c%s",hd,SEARCHPATH_SEPARATOR,SEARCHPATH_SEPARATOR,path);
			}
			else
				result=strdup(path);
		}
		
	}
	return(result);
}


// Given a top-level directory, extracts the last component, and matches against a prefix.
// Useful for locating an executable.

int MatchBaseName(const char *prefix,const char *path)
{
	int result=-1;
	char *fn=strdup(path);
	char *bn=basename(fn);
	if(!bn)
		return(-1);

	Debug[TRACE] << "  Comparing " << prefix << " against " << bn << std::endl;
	result=strncasecmp(prefix,bn,strlen(prefix));
	free(fn);
	Debug[TRACE] << "  result of comparison: " << result << std::endl;
	return(result);
}

