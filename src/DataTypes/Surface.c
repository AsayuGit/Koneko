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
#include "Log.h"
#include "System.h"

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
        KON_SystemMsgExt("(KON_LoadRawCPUSurface) Couldn't load : ", FilePath, MESSAGE_WARNING);
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsgExt("(KON_LoadRawCPUSurface) Loaded NEW CPU Surface : ", FilePath, MESSAGE_LOG);

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
static KON_Surface* KON_LoadRawSurface(char* FilePath, DisplayDevice* Device, uint32_t ColorKey, uint8_t flags) {
    SDL_Surface* loadedCPUSurface = NULL;
    SDL_Texture* loadedGPUSurface;
    KON_Surface* loadedSurface;
    Vector2i surfaceSize;
    
    if (!FilePath || !Device) {
        KON_SystemMsg("(KON_LoadRawSurface) Incorrect Parameters", MESSAGE_WARNING);
        return NULL;
    }
    if (!(loadedCPUSurface = KON_LoadRawCPUSurface(FilePath, ColorKey, flags)))
        return NULL;

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadedCPUSurface, ColorKey);

    if (!(loadedGPUSurface = SDL_CreateTextureFromSurface(Device->Renderer, loadedCPUSurface))) {
        KON_SystemMsgExt("(KON_LoadRawSurface) Couldn't upload surface %s to GPU !", FilePath, MESSAGE_WARNING);
        return NULL;
    }
    SDL_FreeSurface(loadedCPUSurface);


    if (!(loadedSurface = (KON_Surface*)malloc(sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_LoadSurface) Couldn't allocate memory", MESSAGE_WARNING);
        return NULL;
    }
    
    SDL_QueryTexture(loadedGPUSurface, NULL, NULL, &surfaceSize.x, &surfaceSize.y);

    loadedSurface->surface = loadedGPUSurface;
    KON_VectToVect(loadedSurface->size, surfaceSize);

    return loadedSurface;
}

KON_Surface* KON_LoadSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags) {
    KON_Surface* loadedSurface;

    if (!FilePath || !Device) {
        KON_SystemMsg("(KON_LoadSurface) Incorrect Parameters", MESSAGE_WARNING);
        return NULL;
    }

    if (!(loadedSurface = (KON_Surface*)KON_GetManagedRessource(FilePath, RESSOURCE_GPU_SURFACE))) {
        loadedSurface = KON_LoadRawSurface(FilePath, Device, ColorKey, flags);
        KON_AddManagedRessource(FilePath, RESSOURCE_GPU_SURFACE, loadedSurface);
        KON_SystemMsgExt("(KON_LoadSurface) Loaded NEW GPU Surface : ", FilePath, MESSAGE_LOG);
        return loadedSurface;
    }

    KON_SystemMsgExt("(KON_LoadSurface) Referenced GPU Surface : ", FilePath, MESSAGE_LOG);
    return loadedSurface;
}

/* Unmanaged */
SDL_Surface* KON_LoadCpuSurface(char* FilePath, DisplayDevice* Device, Uint32 ColorKey, Uint8 flags) {
    if (!FilePath || !Device)
        return NULL;

    return KON_LoadRawCPUSurface(FilePath, ColorKey, flags);
}

void KON_FreeSurface(KON_Surface* surface) {
    KON_FreeRawSurface(KON_FreeManagedRessourceByRef(surface));
}

/* Unmanaged */
KON_Surface* KON_CpuToGpuSurface(DisplayDevice* dDevice, SDL_Surface* cpuSurface) {
    int w, h;
    KON_Surface* newSurface = (KON_Surface*)malloc(sizeof(KON_Surface));

    newSurface->surface = SDL_CreateTextureFromSurface(dDevice->Renderer, cpuSurface);
    SDL_QueryTexture(newSurface->surface, NULL, NULL, &w, &h);
    newSurface->size = KON_InitVector2d(w, h);

    return newSurface;
}

void KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size) {
    if (surface && size)
        *size = surface->size;
}

/* API level draw */
static int KON_DrawEx(DisplayDevice* dDevice, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect, uint8_t flags) {

    /* Flags decoding */

    return SDL_RenderCopyEx(dDevice->Renderer, texture, srcrect, dstrect, 0, 0, flags);
}

#define KON_Draw(dDevice, texture, srcrect, dstrect) KON_DrawEx(dDevice, texture, srcrect, dstrect, DRAW_DEFAULT)

void KON_DrawScaledSurfaceRectEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* rect, SDL_Rect* dest, DrawFlags flags) {
    SDL_Rect ScaledDstRect;

    ScaledDstRect = KON_InitRect(0, 0, dDevice->InternalResolution.x, dDevice->InternalResolution.y);
    if (!surface || !RectOnScreen(dDevice, dest))
        return;

    if (dest){
        ScaledDstRect = KON_InitRect(
            (dest->x * dDevice->IRScalar) + dDevice->RenderRect.x,
            (dest->y * dDevice->IRScalar) + dDevice->RenderRect.y,
            dest->w * dDevice->IRScalar,
            dest->h * dDevice->IRScalar
        );
    } else {
        ScaledDstRect = dDevice->RenderRect;
    }

    KON_DrawEx(dDevice, surface->surface, rect, &ScaledDstRect, flags);
}

void KON_DrawSurfaceRectEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* rect, Vector2d* pos, DrawFlags flags) {
    SDL_Rect dest = KON_CatVectToRect(pos, &surface->size);

    KON_DrawScaledSurfaceRectEx(dDevice, surface, rect, &dest, flags);
}

void KON_DrawScaledSurfaceEx(DisplayDevice* dDevice, KON_Surface* surface, SDL_Rect* dest, DrawFlags flags) {
    KON_DrawScaledSurfaceRectEx(dDevice, surface, NULL, dest, flags);
}

void KON_DrawSurfaceEx(DisplayDevice* dDevice, KON_Surface* surface, Vector2d* pos, DrawFlags flags) {
    SDL_Rect dest = KON_CatVectToRect(pos, &surface->size);

    KON_DrawScaledSurfaceRectEx(dDevice, surface, NULL, &dest, flags);
}