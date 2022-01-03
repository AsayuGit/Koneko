#ifndef _SURFACE_H
#define _SURFACE_H

    #include "Types.h"

    enum {
        SURFACE_OPAQUE = 0,
        SURFACE_ALPHA = 1,
        SURFACE_KEYED = 2
    };

    SDL_Texture* KON_LoadSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags);
    SDL_Surface* KON_LoadCpuSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags);
    void         KON_FreeSurface(SDL_Texture* surface);
    SDL_Texture* KON_CreateTargetSurface(DisplayDevice* DDevice, int w, int h);


#endif