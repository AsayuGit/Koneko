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

#ifndef _SURFACE_H
#define _SURFACE_H


    #include "Rect.h"
    #include "System.h"
    #include "KON_Bitmap.h"

    enum {
        SURFACE_OPAQUE = 0,
        SURFACE_ALPHA = 1,
        SURFACE_KEYED = 2
    };

    typedef enum {
        DRAW_DEFAULT = 0,
        DRAW_HORIZONTAL_FLIP = 1,
        DRAW_VERTICAL_FLIP = 2,
        DRAW_NO_SCALE = 4
    } DrawFlags;

    typedef struct KON_Surface KON_Surface;
    typedef struct KON_CPUSurface KON_CPUSurface;

    /*
        SUMMARY : Loads a KON_Surface from disk.
        INPUT   : char* filePath    : The path to the surface to load.
        INPUT   : uint32_t colorKey : The color to key the surface with (if needs be).
        INPUT   : uint8_t flags     : Flags ruling how to load the surface.
        OUTPUT  : KON_Surface*      : The newly loaded surface (or NULL if error).
    */
    KON_Surface* KON_LoadSurface(char* filePath, uint32_t colorKey, uint8_t flags);

    /*
        SUMMARY : Loads a CPU-Side Surface from disk.
        INPUT   : char* filePath    : The path to the surface to load.
        INPUT   : uint32_t colorKey : The color to key the surface with (if needs be).
        INPUT   : uint8_t flags     : Flags ruling how to load the surface.
        OUTPUT  : KON_CPUSurface*      : The newly loaded surface (or NULL if error).
    */
    KON_CPUSurface* KON_LoadCpuSurface(char* filePath, uint32_t colorKey, uint8_t flags);

    /*
        SUMMARY : Loads a CPU-Side Surface from memory.
        OUTPUT  : KON_CPUSurface* : The newly loaded surface (or NULL if error).
    */
    KON_CPUSurface* KON_LoadCPUSurfaceFromMem(BITMAP* bitmap, uint32_t ColorKey, uint8_t flags);

    /*
        TODO: Doc
    */
    void KON_GetCPUSurfaceSize(KON_CPUSurface* surface, size_t* pitch, unsigned int* w, unsigned int* h);

    /*
        TODO: Doc
    */
    void KON_LockCPUSurface(KON_CPUSurface* surface);

    /*
        TODO: Doc
    */
    void KON_UnlockCPUSurface(KON_CPUSurface* surface);

    /*
        TODO: Doc
    */
    void* KON_GetCPUSurfacePixelData(KON_CPUSurface* surface);

    /*
        SUMMARY : Free a previously loaded KON_Surface.
        INPUT   : KON_Surface* surface : The surface to free.
    */
    void         KON_FreeSurface(KON_Surface* surface);

    /*
        SUMMARY : Free a previously loaded CPU-Side surface.
        INPUT   : KON_CPUSurface* surface : The surface to free.
    */
    void         KON_FreeCPUSurface(KON_CPUSurface* surface);

    /*
        TODO : Doc
    */
    unsigned int KON_GetCPUSurfaceBPP(KON_CPUSurface* surface);

    /*
        TODO : Doc
    */
    unsigned int KON_GetCPUSurfacePitch(KON_CPUSurface* surface);

    /*
        SUMMARY : Loads a KON_Surface from a CPU-Side surface.
        INPUT   : KON_CPUSurface* cpuSurface : The CPU-Side surface to load from.
        OUTPUT  : KON_Surface*            : Tne newly loaded surface (or NULL on error).
    */
    KON_Surface* KON_CpuToGpuSurface(KON_CPUSurface* cpuSurface);

    /*
        SUMMARY : When provided with a surface, allows the user to get its size.
        INPUT   : KON_Surface* surface : The surface to get the size from
        INPUT   : Vector2d* size       : The surface's size
    */
    void         KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size);

    /*
        SUMMARY : Sets the render target for a CPU-Side surface.
        INPUT   : KON_Surface* surface : The CPu-Side surface to change the render target for.
    */
    int          KON_SetRenderTarget(KON_Surface* surface);

    /*
        SUMMARY : Draws parts of a surface scaled to a new size and destination.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* rect       : The part to take out of the source surface.
        INPUT   : KON_Rect* dest       : Where on the screen to draw it.
    */
    #define      KON_DrawScaledSurfaceRect(surface, rect, dest) KON_DrawScaledSurfaceRectEx(surface, rect, dest, DRAW_DEFAULT)

    /*
        SUMMARY : Draws part of a surface to the screen at a new location.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* rect       : The part to take out of the source surface.
        INPUT   : Vector2d* pos        : Where to draw on the screen.
    */
    #define      KON_DrawSurfaceRect(surface, rect, pos) KON_DrawSurfaceRectEx(surface, rect, pos, DRAW_DEFAULT)

    /*
        SUMMARY : Draws a surface at a new size and location.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* dest       : Where on the screen to draw it.
    */
    #define      KON_DrawScaledSurface(surface, dest) KON_DrawScaledSurfaceEx(surface, dest, DRAW_DEFAULT)

    /*
        SUMMARY : Draws a surface on the screen.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : Vector2d* pos        : Where to draw on the screen.
    */
    #define      KON_DrawSurface(surface, pos) KON_DrawSurfaceEx(surface, pos, DRAW_DEFAULT)

    /*
        SUMMARY : Draws parts of a surface scaled to a new size and destination with draw option flags.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* rect       : The part to take out of the source surface.
        INPUT   : KON_Rect* dest       : Where on the screen to draw it.
        INPUT   : DrawFlags flags      : Draw options to be used during the drawing process.
    */
    void         KON_DrawScaledSurfaceRectEx(KON_Surface* surface, KON_Rect* rect, KON_Rect* dest, DrawFlags flags);

    /*
        SUMMARY : Draws part of a surface to the screen at a new location with draw option flags.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* rect       : The part to take out of the source surface.
        INPUT   : Vector2d* pos        : Where to draw on the screen.
        INPUT   : DrawFlags flags      : Draw options to be used during the drawing process.
    */
    void         KON_DrawSurfaceRectEx(KON_Surface* surface, KON_Rect* rect, Vector2d* pos, DrawFlags flags);

    /*
        SUMMARY : Draws a surface at a new size and location with draw option flags.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : KON_Rect* dest       : Where on the screen to draw 
        INPUT   : DrawFlags flags      : Draw options to be used during the drawing process.it.
    */
    void         KON_DrawScaledSurfaceEx(KON_Surface* surface, KON_Rect* dest, DrawFlags flags);

    /*
        SUMMARY : Draws a surface on the screen with draw option flags.
        INPUT   : KON_Surface* surface : The surface to draw.
        INPUT   : Vector2d* pos        : Where to draw on the screen.
        INPUT   : DrawFlags flags      : Draw options to be used during the drawing process.
    */
    void         KON_DrawSurfaceEx(KON_Surface* surface, Vector2d* pos, DrawFlags flags);

#endif