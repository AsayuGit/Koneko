#include "KON_FileSystem.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
    #define PATH_SEPARATOR '\\'
#elif defined(GEKKO)
    #include <limits.h>
    #define PATH_SEPARATOR '/'
#else
	#include <linux/limits.h>
    #define PATH_SEPARATOR '/'
#endif

#include <stdlib.h>
#include <string.h>

char* KON_DirName(char *path) {
    #ifdef _XBOX
        /* TODO: Check */
        char buffer[PATH_MAX];
        char* dirname;
		
		_splitpath(path, dirname, buffer, NULL, NULL);
		strcat(dirname, buffer);

        return dirname;
    #else
        char* dirname;

        if (!path || *path == '\0')
            return ".";

        do {
            if(!(dirname = strrchr(path, PATH_SEPARATOR)))
                return ".";
            
            if (dirname == path)
                return (*path == PATH_SEPARATOR) ? "/" : ".";

            *dirname = '\0';
        } while (*(dirname + 1) == '\0');

        return path;
    #endif
}