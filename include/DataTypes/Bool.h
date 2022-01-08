#ifndef _BOOL_H
#define _BOOL_H

    #ifdef _XBOX
        typedef enum{
            false,
            true
        } bool;
    #else
        #include <stdbool.h>
    #endif

#endif