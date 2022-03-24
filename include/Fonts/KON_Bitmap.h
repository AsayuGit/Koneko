#ifndef _BITMAP_H
#define _BITMAP_H

    #include <stdint.h>

    typedef struct {
        int width;
        int height;
        int depth;
        int pitch;
        void* pixels;
    } BITMAP;

    #define initBitmap(dst, src) do { \
        dst.width = src.width;        \
        dst.height = src.height;      \
        dst.depth = src.depth;        \
        dst.pitch = src.pitch;        \
        dst.pixels = src.pixels;      \
    } while (0)                       \

#endif
