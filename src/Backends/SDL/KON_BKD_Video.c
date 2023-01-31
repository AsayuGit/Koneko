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

#include "KON_BKD_Video.h"

#ifdef __SWITCH__
#include <limits.h>
#else
#include <linux/limits.h>
#endif

#include <SDL2/SDL.h>

#include "Koneko.h"
#include "Bool.h"
#include "CommunFunctions.h"

static SDL_Window *Screen;
static SDL_Renderer *Renderer;

void KON_BKD_InitVideo(int screenResX, int screenResY, char* title) {
    Screen = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenResX, screenResY, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!(Renderer = SDL_CreateRenderer(Screen , -1, 0)))
        KON_SystemMsg("(KON_BKD_InitVideo) Can't create main renderer : ", MESSAGE_ERROR, 1, SDL_GetError());
}

void KON_BKD_FreeVideo(void) {
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Screen);
}

KON_CPUSurface* KON_BKD_LoadCPUSurface(const char* path) {
    return (KON_CPUSurface*)SDL_LoadBMP(path);
}

KON_CPUSurface* KON_BKD_LoadCPUSurfaceFromMem(BITMAP* bitmap) {
    return (KON_CPUSurface*)SDL_CreateRGBSurfaceWithFormatFrom(bitmap->pixels, bitmap->width, bitmap->height, bitmap->depth, bitmap->pitch, SDL_PIXELFORMAT_RGB24);
}

KON_Surface* KON_BKD_UploadSurface(KON_CPUSurface* surface) {
    KON_Surface* gpuSurface;

    if (!(gpuSurface = (KON_Surface*)calloc(1, sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_BKD_UploadSurface) No more memory", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(gpuSurface->texture = (KON_Texture*)SDL_CreateTextureFromSurface(Renderer, (SDL_Surface*)surface))) {
        KON_SystemMsg("(KON_BKD_UploadSurface) Couldn't upload surface to GPU : ", MESSAGE_WARNING, 1, SDL_GetError());
        return NULL;
    }

    SDL_QueryTexture((SDL_Texture*)gpuSurface->texture, NULL, NULL, &gpuSurface->width, &gpuSurface->height);

    return gpuSurface;
}

KON_Surface* KON_BKD_CreateSurface(unsigned int width, unsigned int height) {
    KON_Surface* newSurface = NULL;
    SDL_Texture* renderTexture = NULL;

    if (!(newSurface = (KON_Surface*)calloc(1, sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_BKD_CreateSurface) Couldn't create surface", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(renderTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height))) {
        free(newSurface);
        KON_SystemMsg("(KON_BKD_CreateSurface) Couldn't create surface", MESSAGE_ERROR, 0);
        return NULL;
    }
    SDL_SetTextureBlendMode(renderTexture, SDL_BLENDMODE_BLEND);

    newSurface->width = width;
    newSurface->height = height;
    newSurface->texture = (KON_Texture*)renderTexture;
    newSurface->readOnly = false;

    return newSurface;
}

void KON_BKD_FreeCPUSurface(KON_CPUSurface* surface) {
    SDL_FreeSurface((SDL_Surface*)surface);
}

void KON_BKD_FreeSurface(KON_Surface* surface) {
    if (!surface) return;
    SDL_DestroyTexture((SDL_Texture*)surface->texture);
    free(surface);
}

void KON_BKD_KeyCpuSurface(KON_CPUSurface* surface, uint32_t colorKey) {
    if (SDL_SetColorKey((SDL_Surface*)surface, true, colorKey) < 0)
        KON_SystemMsg("() couldn't set color key : ", MESSAGE_WARNING, 1, SDL_GetError());
}

void KON_BKD_QuerryCPUSurface(KON_CPUSurface* surface, unsigned int* bpp, size_t* pitch, unsigned int* width, unsigned int* height) {
    if (bpp) *bpp = ((SDL_Surface*)surface)->format->BytesPerPixel;
    if (pitch) *pitch = ((SDL_Surface*)surface)->pitch;
    if (width) *width = ((SDL_Surface*)surface)->w;
    if (height) *height = ((SDL_Surface*)surface)->h;
}

void KON_BKD_QuerrySurface(KON_Surface* surface, Vector2d* size) {
    if (!surface || !size) return;
    size->x = surface->width;
    size->y = surface->height;
}

void* KON_BKD_GetPixelData(KON_CPUSurface* surface) {
    return ((SDL_Surface*)surface)->pixels;
}

uint32_t KON_BKD_GetPixel(KON_CPUSurface* surface, unsigned int x, unsigned int y) {
    unsigned int bpp;
    size_t pitch;
    uint8_t *p;
    void* pixelData;

    if (!surface){
        printf("ERROR: (getpixel) No surface provided !\n");
        return 0; /* Failsafe */
    } else if (!(pixelData = KON_GetCPUSurfacePixelData(surface))){
        /*printf("ERROR: (getpixel) No pixel data !\n");*/
        return 0; /* Failsafe */
    }

    KON_BKD_QuerryCPUSurface(surface, &bpp, &pitch, NULL, NULL);
    p = (uint8_t *)pixelData + y * pitch + x * bpp; /* Here p is the address to the pixel we want to retrieve */

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(uint8_t *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *((uint32_t*)p);

    default:
        return 0;
    }
}

void KON_BKD_LockCPUSurface(KON_CPUSurface* surface) {
    SDL_LockSurface((SDL_Surface*)surface);
}

void KON_BKD_UnlockCPUSurface(KON_CPUSurface* surface) {
    SDL_UnlockSurface((SDL_Surface*)surface);
}

int KON_BKD_SetRenderSurface(KON_Surface* surface) {
    return SDL_SetRenderTarget(Renderer, (surface) ? (SDL_Texture*)surface->texture: NULL);
}

void KON_BKD_SetRenderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {}

void KON_BKD_DrawCoverFrame(KON_Rect Frame[4]) {
    SDL_RenderFillRect(Renderer, (SDL_Rect*)Frame);
    SDL_RenderFillRect(Renderer, (SDL_Rect*)Frame + 1);
    SDL_RenderFillRect(Renderer, (SDL_Rect*)Frame + 2);
    SDL_RenderFillRect(Renderer, (SDL_Rect*)Frame + 3);
}

void KON_BKD_UpdateRenderRect(KON_Rect* RenderRect, Vector2i* ScreenResolution) {
    int ScreenWidth, ScreenHeight; /* Signed because SDL said so :c */
        
    SDL_GetWindowSize(Screen, &ScreenWidth, &ScreenHeight);

    if (Koneko.dDevice.integerScalling && (ScreenWidth > Koneko.dDevice.InternalResolution.x) && (ScreenHeight > Koneko.dDevice.InternalResolution.y))
        Koneko.dDevice.IRScalar = MIN(ScreenWidth / Koneko.dDevice.InternalResolution.x, ScreenHeight / Koneko.dDevice.InternalResolution.y);
    else
        Koneko.dDevice.IRScalar = MIN((double)ScreenWidth / (double)Koneko.dDevice.InternalResolution.x, (double)ScreenHeight / (double)Koneko.dDevice.InternalResolution.y);

    ScreenResolution->x = ScreenWidth;
    ScreenResolution->y = ScreenHeight;   

    RenderRect->w = (int)(Koneko.dDevice.InternalResolution.x * Koneko.dDevice.IRScalar);
    RenderRect->h = (int)(Koneko.dDevice.InternalResolution.y * Koneko.dDevice.IRScalar);

    RenderRect->x = (int)(ScreenResolution->x - RenderRect->w) >> 1;
    RenderRect->y = (int)(ScreenResolution->y - RenderRect->h) >> 1;
}

void KON_BKD_SetVSync(bool value) {
    SDL_GL_SetSwapInterval(value);
}

void KON_BKD_Draw(KON_Surface* surface, const KON_Rect* srcrect, const KON_Rect* dstrect, uint8_t flags) {
    SDL_RendererFlip flip = 0;

    if (flags & DRAW_HORIZONTAL_FLIP) flip |= SDL_FLIP_HORIZONTAL;
    if (flags & DRAW_VERTICAL_FLIP) flip |= SDL_FLIP_VERTICAL;

    SDL_RenderCopyEx(Renderer, (SDL_Texture*)surface->texture, (SDL_Rect*)srcrect, (SDL_Rect*)dstrect, 0, 0, flip);
}

void KON_BKD_Display(void) {
    SDL_RenderPresent(Renderer);
}

void KON_BKD_ClearScreen(void) {
    SDL_RenderClear(Renderer);
}

void KON_BKD_Debug(void) {}