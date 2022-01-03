/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "Surface.h"
#include "RessourceManager.h"
#include <linux/limits.h>


/*
    SUMMARY : Free a GPU-Side surface
    INPUT   : SDL_Texture* surface : Surface to be freed
*/
#define KON_FreeRawSurface(surface) SDL_DestroyTexture(surface)

/*
    SUMMARY : Applies a ColorKey to a CPU-Side Surface
    INPUT   : SDL_Surface* SurfaceToKey : Pointer to the Surface to key
    INPUT   : uint8_t ColorKey          : The ColorKey to apply to the Surface
*/
#define KON_KeyCpuSurface(SurfaceToKey, ColorKey) SDL_SetColorKey(SurfaceToKey, true, ColorKey)

/*
    SUMMARY : Loads a CPU-Side Surface from disk
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : SDL_Surface*          : Pointer to the loaded surface (UnMannaged)
*/
static SDL_Surface* KON_LoadRawCPUSurface(char* FilePath, uint32_t ColorKey, uint8_t flags) {
    SDL_Surface* loadingCPUSurface = NULL;
    
    if (!FilePath)
        return NULL;
    if (!(loadingCPUSurface = SDL_LoadBMP(FilePath))) {
        printf("ERROR: (KON_LoadRawCPUSurface) Couldn't load %s !\n", FilePath);
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    return loadingCPUSurface;
}

/*
    SUMMARY : Loads a GPU-Side Surface from disk
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : DisplayDevice* Device : Pointer to Koneko's display device
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : SDL_Surface*          : Pointer to the loaded surface (UnMannaged)
*/
static SDL_Texture* KON_LoadRawSurface(char* FilePath, DisplayDevice* Device, uint32_t ColorKey, uint8_t flags) {
    SDL_Surface* loadingCPUSurface = NULL;
    SDL_Texture* loadingSurface;
    
    if (!FilePath || !Device)
        return NULL;
    if (!(loadingCPUSurface = KON_LoadRawCPUSurface(FilePath, ColorKey, flags))) {
        printf("ERROR: (KON_LoadRawSurface) Couldn't load %s !\n", FilePath);
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    loadingSurface = SDL_CreateTextureFromSurface(Device->Renderer, loadingCPUSurface);
    SDL_FreeSurface(loadingCPUSurface);

    return loadingSurface;
}

SDL_Texture* KON_LoadSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags) {
    SDL_Texture* loadedTexture = NULL;
    char buffer[PATH_MAX];

    if (!FilePath || !Device)
        return NULL;

    sprintf(buffer, "[GPU-SURFACE]>%s", FilePath);
    loadedTexture = (SDL_Texture*)KON_GetManagedRessource(buffer);
    if (!loadedTexture) {
        loadedTexture = KON_LoadRawSurface(FilePath, Device, ColorKey, flags);
        KON_AddManagedRessource(buffer, loadedTexture);
    }

    return loadedTexture;
}

SDL_Surface* KON_LoadCpuSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags) {
    SDL_Surface* loadedCpuSurface = NULL;
    char buffer[PATH_MAX];

    if (!FilePath || !Device)
        return NULL;

    sprintf(buffer, "[CPU-SURFACE]>%s", FilePath);
    loadedCpuSurface = (SDL_Surface*)KON_GetManagedRessource(buffer);
    if (!loadedCpuSurface) {
        loadedCpuSurface = KON_LoadRawCPUSurface(FilePath, ColorKey, flags);
        KON_AddManagedRessource(buffer, loadedCpuSurface);
    }

    return loadedCpuSurface;
}

void KON_FreeSurface(SDL_Texture* surface) {
    KON_FreeRawSurface(KON_FreeManagedRessourceByRef(surface));
}

/* TODO: Update do managed ressource */
SDL_Texture* KON_CreateTargetSurface(DisplayDevice* DDevice, int w, int h) {
    SDL_Texture* LoadingSurface;

    LoadingSurface = NULL;
#ifdef _SDL
    LoadingSurface = CreateEmptySurface(SDL_HWSURFACE, w, h);
#else
    LoadingSurface = SDL_CreateTexture(DDevice->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(LoadingSurface, SDL_BLENDMODE_BLEND);
#endif
    if (LoadingSurface == NULL)
        fprintf(stderr, "Can't load texture %s\n", SDL_GetError());
    return LoadingSurface;
}