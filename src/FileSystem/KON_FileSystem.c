/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2022 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "KON_FileSystem.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
    #define PATH_SEPARATOR '\\'
#elif defined(GEKKO)
    #include <limits.h>
    #include <fat.h>
    #define PATH_SEPARATOR '/'
#else
	#include <linux/limits.h>
    #define PATH_SEPARATOR '/'
#endif

#include <stdlib.h>
#include <string.h>

void KON_InitFileSystem(void) {
    #ifdef GEKKO
        fatInitDefault();
    #endif
}

void KON_FreeFileSystem(void) {}

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
