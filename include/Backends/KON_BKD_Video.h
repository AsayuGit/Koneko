/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2023 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#ifndef _KON_BKD_VIDEO_H
#define _KON_BKD_VIDEO_H

#include <stdint.h>

#include "Bool.h"
#include "Rect.h"
#include "KON_Bitmap.h"

typedef struct KON_CPUSurface KON_CPUSurface;
typedef void* KON_Texture;

typedef struct {
    bool readOnly;
    int width;
    int height;
    KON_Texture* texture;
} KON_Surface;

void KON_BKD_InitVideo(int screenResX, int screenResY, char* title);

void KON_BKD_FreeVideo(void);

KON_CPUSurface* KON_BKD_LoadCPUSurface(const char* path);

KON_CPUSurface* KON_BKD_LoadCPUSurfaceFromMem(BITMAP* bitmap);

KON_Surface* KON_BKD_UploadSurface(KON_CPUSurface* surface);

KON_Surface* KON_BKD_CreateSurface(unsigned int width, unsigned int height);

void KON_BKD_FreeCPUSurface(KON_CPUSurface* surface);

void KON_BKD_FreeSurface(KON_Surface* surface);

void KON_BKD_KeyCpuSurface(KON_CPUSurface* surface, uint32_t colorKey);

void KON_BKD_QuerryCPUSurface(KON_CPUSurface* surface, unsigned int* bpp, size_t* pitch, unsigned int* width, unsigned int* height);

void KON_BKD_QuerrySurface(KON_Surface* surface, Vector2d* size);

void* KON_BKD_GetPixelData(KON_CPUSurface* surface);

uint32_t KON_BKD_GetPixel(KON_CPUSurface* surface, unsigned int x, unsigned int y);

void KON_BKD_LockCPUSurface(KON_CPUSurface* surface);

void KON_BKD_UnlockCPUSurface(KON_CPUSurface* surface);

int KON_BKD_SetRenderSurface(KON_Surface* surface);

void KON_BKD_SetRenderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void KON_BKD_DrawCoverFrame(KON_Rect Frame[4]); /* Deprecated */

void KON_BKD_UpdateRenderRect(KON_Rect* RenderRect, Vector2i* ScreenResolution);

void KON_BKD_SetVSync(bool value);

void KON_BKD_Draw(KON_Surface* surface, const KON_Rect* srcrect, const KON_Rect* dstrect, uint8_t flags);

void KON_BKD_Display(void);

void KON_BKD_ClearScreen(void);

void KON_BKD_Debug(void);

#endif