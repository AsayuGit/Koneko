#ifndef _PLATFORM_H
#define _PLATFORM_H

    /*
        Platform dependencies
    */
    #ifdef GEKKO
        #define GX
        #define MXML
    #else
        #define SDL2
        /*#define LIBXML2*/
        #define MXML
    #endif

#endif
