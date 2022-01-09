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

#include "Koneko.h"
#include "Surface.h"
#include "RessourceManager.h"
#include <linux/limits.h>
#include "Log.h"

#include "Graphics.h" /* RectOnScreen() */
#include "CommunFunctions.h"

/* Abstract Koneko's surfaces from their implementations */
struct KON_Surface {
    SDL_Texture* surface;
    Vector2d size;
};

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
    SUMMARY : Loads a CPU-Side Surface from disk (Unmanaged)
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
        KON_SystemMsg("(KON_LoadRawCPUSurface) Couldn't load : ", MESSAGE_WARNING, 2, FilePath, SDL_GetError());
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsg("(KON_LoadRawCPUSurface) Loaded NEW CPU Surface : ", MESSAGE_LOG, 1, FilePath);

    return loadingCPUSurface;
}

/*
    SUMMARY : Loads a GPU-Side Surface from disk (Unmanaged)
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : SDL_Surface*          : Pointer to the loaded surface (UnMannaged)
*/
static KON_Surface* KON_LoadRawSurface(char* FilePath, uint32_t ColorKey, uint8_t flags) {
    SDL_Surface* loadedCPUSurface = NULL;
    SDL_Texture* loadedGPUSurface;
    KON_Surface* loadedSurface;
    Vector2i surfaceSize;
    
    if (!FilePath) {
        KON_SystemMsg("(KON_LoadRawSurface) Incorrect Parameters", MESSAGE_WARNING, 0);
        return NULL;
    }
    if (!(loadedCPUSurface = KON_LoadRawCPUSurface(FilePath, ColorKey, flags)))
        return NULL;

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadedCPUSurface, ColorKey);

    if (!(loadedGPUSurface = SDL_CreateTextureFromSurface(Koneko.dDevice.Renderer, loadedCPUSurface))) {
        KON_SystemMsg("(KON_LoadRawSurface) Couldn't upload surface to GPU : ", MESSAGE_WARNING, 2, FilePath, SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(loadedCPUSurface);


    if (!(loadedSurface = (KON_Surface*)malloc(sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_LoadSurface) Couldn't allocate memory", MESSAGE_WARNING, 0);
        return NULL;
    }
    
    SDL_QueryTexture(loadedGPUSurface, NULL, NULL, &surfaceSize.x, &surfaceSize.y);

    loadedSurface->surface = loadedGPUSurface;
    KON_VectToVect(loadedSurface->size, surfaceSize);

    return loadedSurface;
}

KON_Surface* KON_LoadSurface(char* filePath, uint32_t colorKey, uint8_t flags) {
    KON_Surface* loadedSurface;

    if (!filePath) {
        KON_SystemMsg("(KON_LoadSurface) Incorrect Parameters", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (!(loadedSurface = (KON_Surface*)KON_GetManagedRessource(filePath, RESSOURCE_GPU_SURFACE))) {
        loadedSurface = KON_LoadRawSurface(filePath, colorKey, flags);
        KON_AddManagedRessource(filePath, RESSOURCE_GPU_SURFACE, loadedSurface);
        KON_SystemMsg("(KON_LoadSurface) Loaded NEW GPU Surface : ", MESSAGE_LOG, 1, filePath);
        return loadedSurface;
    }

    KON_SystemMsg("(KON_LoadSurface) Referenced GPU Surface : ", MESSAGE_LOG, 1, filePath);
    return loadedSurface;
}

/* Unmanaged */
SDL_Surface* KON_LoadCpuSurface(char* filePath, uint32_t colorKey, uint8_t flags) {
    if (!filePath)
        return NULL;

    return KON_LoadRawCPUSurface(filePath, colorKey, flags);
}

void KON_FreeSurface(KON_Surface* surface) {
    KON_FreeRawSurface(KON_FreeManagedRessourceByRef(surface));
}

/* Unmanaged */
KON_Surface* KON_CpuToGpuSurface(SDL_Surface* cpuSurface) {
    int w, h;
    KON_Surface* newSurface = (KON_Surface*)malloc(sizeof(KON_Surface));

    if (!(newSurface->surface = SDL_CreateTextureFromSurface(Koneko.dDevice.Renderer, cpuSurface)))
        return NULL;

    SDL_QueryTexture(newSurface->surface, NULL, NULL, &w, &h);
    newSurface->size = KON_InitVector2d(w, h);

    return newSurface;
}

void KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size) {
    if (surface && size)
        *size = surface->size;
}

/* API level draw */
static int KON_DrawEx(SDL_Texture* texture, const KON_Rect* srcrect, const KON_Rect* dstrect, uint8_t flags) {

    /* Flags decoding */

    return SDL_RenderCopyEx(Koneko.dDevice.Renderer, texture, (SDL_Rect*)srcrect, (SDL_Rect*)dstrect, 0, 0, flags);
}

#define KON_Draw(dDevice, texture, srcrect, dstrect) KON_DrawEx(dDevice, texture, srcrect, dstrect, DRAW_DEFAULT)

void KON_DrawScaledSurfaceRectEx(KON_Surface* surface, KON_Rect* rect, KON_Rect* dest, DrawFlags flags) {
    KON_Rect ScaledDstRect;

    KON_InitRect(ScaledDstRect, 0, 0, Koneko.dDevice.InternalResolution.x, Koneko.dDevice.InternalResolution.y);
    
    if (!surface)
        return;

    if (dest){
        if (!RectOnScreen(dest))
            return;
        KON_InitRect(ScaledDstRect,
            (dest->x * Koneko.dDevice.IRScalar) + Koneko.dDevice.RenderRect.x,
            (dest->y * Koneko.dDevice.IRScalar) + Koneko.dDevice.RenderRect.y,
            dest->w * Koneko.dDevice.IRScalar,
            dest->h * Koneko.dDevice.IRScalar
        );
    } else {
        ScaledDstRect = Koneko.dDevice.RenderRect;
    }

    KON_DrawEx(surface->surface, rect, &ScaledDstRect, flags);
}

void KON_DrawSurfaceRectEx(KON_Surface* surface, KON_Rect* rect, Vector2d* pos, DrawFlags flags) {
    KON_Rect dest;

    KON_CatVectToRect(dest, (*pos), surface->size);
    KON_DrawScaledSurfaceRectEx(surface, rect, &dest, flags);
}

void KON_DrawScaledSurfaceEx(KON_Surface* surface, KON_Rect* dest, DrawFlags flags) {
    KON_DrawScaledSurfaceRectEx(surface, NULL, dest, flags);
}

void KON_DrawSurfaceEx(KON_Surface* surface, Vector2d* pos, DrawFlags flags) {
    KON_Rect dest;
    
    KON_CatVectToRect(dest, (*pos), surface->size);
    KON_DrawScaledSurfaceRectEx(surface, NULL, &dest, flags);
}