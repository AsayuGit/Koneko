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

#include "KON_DisplayDevice.h"
#include "Koneko.h"
#include "CommunFunctions.h"

#include "API.h"

#include "Koneko.h"
#include "RessourceManager.h"

#include "Log.h"
#include "Graphics.h" /* RectOnScreen() */

#include "KON_TextRendering.h"

#include "KON_BKD_Video.h"

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
    KON_Rect Frame[4];              /* Screen Border Frame */
    KON_Rect RenderRect;            /* Where the game is drawn on screen */
    Vector2i ScreenResolution;      /* The external resolution of the game */
    bool offScreenRendering;
} KON_VideoInterface;

static KON_VideoInterface vi;

extern struct BITMAP_SystemFont { int width; int height; int depth; int pitch; uint8_t pixels[113772];} SystemFont;

static bool drawFPS;
static BitmapFont* font;
static KON_Surface* fpsTextBox;
static const KON_Rect fpsTextBoxSize = {0, 0, 100, 50};

void KON_SetDrawFPS(bool value) {
    drawFPS = value;
}

void KON_DrawFPS(void) {
    char fpsText[100];
    double fps = 1000.0 / Koneko.dDevice.frametime;

    sprintf(fpsText, "FPS: %.2f\nFrametime: %u ms", fps, Koneko.dDevice.frametime);
    KON_PrintEx(font, fpsTextBox, fpsText, 1, false, 0, 0);
    KON_DrawScaledSurface(fpsTextBox, &fpsTextBoxSize);
}

void KON_FinishFrame(void) {
    uint32_t ticks;
    KON_BKD_DrawCoverFrame(vi.Frame);

    if (drawFPS) KON_DrawFPS();

    KON_BKD_Display();

    ticks = KON_GetMs();
    Koneko.dDevice.frametime = ticks - Koneko.dDevice.lastFrame;
    Koneko.dDevice.lastFrame = ticks;
}

void KON_SetCameraFOVRAD(double fov) {
    Koneko.dDevice.camera.plane = KON_GetVectScalarProduct(
        KON_GetVectScalarDivision(Koneko.dDevice.camera.plane, KON_GetVectNorm(Koneko.dDevice.camera.plane)),
        tan(fov / 2)
    );
}

void KON_SetCameraFOV(double fov) {
    KON_SetCameraFOVRAD(KON_DegToRad(fov));
}

void KON_ResetCameraFOV(void) {
    KON_SetCameraFOVRAD(2.0 * atan(0.5) / (double)Koneko.dDevice.InternalResolution.y * (double)Koneko.dDevice.InternalResolution.x);
}

void KON_CenterCameraOnCoordinates(double X, double Y){
    Koneko.dDevice.camera.position.x = X - (Koneko.dDevice.InternalResolution.x >> 1);
    Koneko.dDevice.camera.position.y = Y - (Koneko.dDevice.InternalResolution.y >> 1);
}

void KON_BoundCameraToRegion(KON_Rect Region){
    if (Koneko.dDevice.camera.position.x + Koneko.dDevice.InternalResolution.x > Region.x + Region.w){
        Koneko.dDevice.camera.position.x = Region.x + Region.w - Koneko.dDevice.InternalResolution.x;
    }
    if (Koneko.dDevice.camera.position.x < Region.x){
        Koneko.dDevice.camera.position.x = Region.x;
    }

    if (Koneko.dDevice.camera.position.y + Koneko.dDevice.InternalResolution.y > Region.y + Region.h){
        Koneko.dDevice.camera.position.y = Region.y + Region.h - Koneko.dDevice.InternalResolution.y;
    }
    if (Koneko.dDevice.camera.position.y < Region.y){
        Koneko.dDevice.camera.position.y = Region.y;
    }
}

void KON_RotateCamera(double angle) {
    KON_RotateVector(&Koneko.dDevice.camera.direction, angle);
    KON_RotateVector(&Koneko.dDevice.camera.plane, angle);
}

Vector2i KON_ScreenToRenderCoordinate(Vector2i screenCoordinate) {
    Vector2i renderCoordinate = {0};

    if (screenCoordinate.x < vi.RenderRect.x || screenCoordinate.y < vi.RenderRect.y)
        return renderCoordinate;
    if (screenCoordinate.x > vi.RenderRect.x + vi.RenderRect.w || screenCoordinate.y > vi.RenderRect.y + vi.RenderRect.h)
        return renderCoordinate;
    

    renderCoordinate.x = (screenCoordinate.x - vi.RenderRect.x) / Koneko.dDevice.IRScalar;
    renderCoordinate.y = (screenCoordinate.y - vi.RenderRect.y) / Koneko.dDevice.IRScalar;

    return renderCoordinate;
}

void KON_UpdateRenderRect(void) {
    KON_BKD_UpdateRenderRect(&vi.RenderRect, &vi.ScreenResolution);

    KON_InitRect(vi.Frame[0], 0, 0, vi.RenderRect.x, vi.ScreenResolution.y);                                           /* Left Frame */
    KON_InitRect(vi.Frame[1], vi.RenderRect.x + vi.RenderRect.w, 0, vi.RenderRect.x, vi.ScreenResolution.y);           /* Right Frame */
    KON_InitRect(vi.Frame[2], vi.RenderRect.x, 0, vi.RenderRect.w, vi.RenderRect.y);                                   /* Top Frame */
    KON_InitRect(vi.Frame[3], vi.RenderRect.x, vi.RenderRect.y + vi.RenderRect.h, vi.RenderRect.w, vi.RenderRect.y);   /* Bottom Frame */
}

void KON_ClearScreen(void) {
    KON_BKD_ClearScreen();
}

void KON_SetVSync(bool value) {
    KON_BKD_SetVSync(value);
}

void KON_InitDisplayDevice(int resX, int resY, char* gameTitle) {
    BITMAP SystemFontBitmap;

    KON_BKD_InitVideo(resX, resY, gameTitle);

    KON_SetVSync(true);

    initBitmap(SystemFontBitmap, SystemFont);

    /* FIXME : TEMPORARY: Makes the start resulution de default internal resolution */
    Koneko.dDevice.InternalResolution.x = resX;
    Koneko.dDevice.InternalResolution.y = resY;

    Koneko.dDevice.camera.direction = KON_InitVector2d(0, -1);
    Koneko.dDevice.camera.plane = KON_InitVector2d(1, 0);
    Koneko.dDevice.camera.cameraHeight = 0.5;
    KON_ResetCameraFOV();

    KON_UpdateRenderRect();

    font = KON_LoadBitmapFontFromMem("KON_SystemFont", &SystemFontBitmap, 0xff00ff);
    fpsTextBox = KON_CreateSurface(fpsTextBoxSize.w, fpsTextBoxSize.h);
}

void KON_FreeDisplayDevice(void) {
    KON_FreeBitmapFont(&font);
    KON_BKD_FreeSurface(fpsTextBox);
    KON_BKD_FreeVideo();
}

/*
    SUMMARY : Applies a ColorKey to a CPU-Side Surface
    INPUT   : KON_CPUSurface* SurfaceToKey : Pointer to the Surface to key
    INPUT   : uint8_t ColorKey          : The ColorKey to apply to the Surface
*/
void KON_KeyCpuSurface(KON_CPUSurface* SurfaceToKey, uint32_t ColorKey) {
    KON_BKD_KeyCpuSurface(SurfaceToKey, ColorKey);
}

/*
    SUMMARY : Loads a CPU-Side Surface from disk (Unmanaged)
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : KON_CPUSurface*          : Pointer to the loaded surface (UnMannaged)
*/
KON_CPUSurface* KON_LoadCPUSurface(const char* FilePath, uint32_t ColorKey, uint8_t flags) {
    KON_CPUSurface* loadingCPUSurface = NULL;

    if (!FilePath)
        return NULL;

    if (!(loadingCPUSurface = KON_BKD_LoadCPUSurface(FilePath))) {
        KON_SystemMsg("(KON_LoadCPUSurface) Couldn't load : ", MESSAGE_WARNING, 1, FilePath);
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsg("(KON_LoadCPUSurface) Loaded NEW CPU Surface : ", MESSAGE_LOG, 1, FilePath);

    return loadingCPUSurface;
}

/*
    SUMMARY : Loads a CPU-Side Surface from memory (Unmanaged)
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : KON_CPUSurface*          : Pointer to the loaded surface (UnMannaged)
*/
KON_CPUSurface* KON_LoadCPUSurfaceFromMem(BITMAP* bitmap, uint32_t ColorKey, uint8_t flags) {
    KON_CPUSurface* loadingCPUSurface = NULL;

    if (!(loadingCPUSurface = KON_BKD_LoadCPUSurfaceFromMem(bitmap))) {
        KON_SystemMsg("(KON_LoadCPUSurfaceFromMem) Couldn't load memory surface", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsg("(KON_LoadCPUSurfaceFromMem) Loaded NEW CPU Surface from memory", MESSAGE_LOG, 0);

    return loadingCPUSurface;
}

KON_Surface* KON_CreateSurface(unsigned int width, unsigned int height) {
    return KON_BKD_CreateSurface(width, height);
}

KON_Surface* KON_LoadSurface(const char* filePath, uint32_t colorKey, uint8_t flags) {
    KON_CPUSurface* loadedCPUSurface;
    KON_Surface* loadedSurface;

    if (!filePath) {
        KON_SystemMsg("(KON_LoadSurface) Incorrect Parameters", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (!(loadedSurface = (KON_Surface*)KON_GetManagedRessource(filePath, RESSOURCE_GPU_SURFACE))) {
        if (!(loadedCPUSurface = KON_LoadCPUSurface(filePath, colorKey, flags)))
            return NULL;

        loadedSurface = KON_BKD_UploadSurface(loadedCPUSurface);
        KON_FreeCPUSurface(loadedCPUSurface);
        
        if (!loadedSurface)
            return NULL;
        
        KON_AddManagedRessource(filePath, RESSOURCE_GPU_SURFACE, loadedSurface);
        KON_SystemMsg("(KON_LoadSurface) Loaded NEW GPU Surface : ", MESSAGE_LOG, 1, filePath);
        return loadedSurface;
    }

    KON_SystemMsg("(KON_LoadSurface) Referenced GPU Surface : ", MESSAGE_LOG, 1, filePath);
    return loadedSurface;
}

void KON_GetCPUSurfaceSize(KON_CPUSurface* surface, size_t* pitch, unsigned int* w, unsigned int* h) {
    KON_BKD_QuerryCPUSurface(surface, NULL, pitch, w, h);
}

void* KON_GetCPUSurfacePixelData(KON_CPUSurface* surface) {
    return KON_BKD_GetPixelData(surface);
}

void KON_LockCPUSurface(KON_CPUSurface* surface) {
    KON_BKD_LockCPUSurface(surface);
}

void KON_UnlockCPUSurface(KON_CPUSurface* surface) {
    KON_BKD_UnlockCPUSurface(surface);
}

void KON_FreeSurface(KON_Surface* surface) {
    KON_BKD_FreeSurface(KON_FreeManagedRessourceByRef(surface));
}

void KON_FreeCPUSurface(KON_CPUSurface* surface) {
    KON_BKD_FreeCPUSurface(surface);
}

void KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size) {
    KON_BKD_QuerrySurface(surface, size);
}

int KON_SetRenderTarget(KON_Surface* target) {
    if (target) { 
        if (KON_BKD_SetRenderSurface(target)) {
            KON_SystemMsg("(KON_SetRenderTarget) Sprite is readonly!", MESSAGE_WARNING, 0);
            return -1;
        }
        vi.offScreenRendering = true;
        return 0;
    }
    vi.offScreenRendering = false;
    return KON_BKD_SetRenderSurface(NULL);
}

void KON_SetRenderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    KON_BKD_SetRenderColor(r, g, b, a);
}

void KON_DrawScaledSurfaceRectEx(KON_Surface* surface, const KON_Rect* rect, const KON_Rect* dest, DrawFlags flags) {
    KON_Rect ScaledDstRect;

    KON_InitRect(ScaledDstRect, 0, 0, Koneko.dDevice.InternalResolution.x, Koneko.dDevice.InternalResolution.y);
    
    if (!surface)
        return;

    if (vi.offScreenRendering) {
        KON_BKD_Draw(surface, rect, dest, flags);
        return;
    }

    if (dest){
        if (!RectOnScreen(dest))
            return;
        KON_InitRect(ScaledDstRect,
            (dest->x * Koneko.dDevice.IRScalar) + vi.RenderRect.x,
            (dest->y * Koneko.dDevice.IRScalar) + vi.RenderRect.y,
            dest->w * Koneko.dDevice.IRScalar,
            dest->h * Koneko.dDevice.IRScalar
        );
    } else {
        ScaledDstRect = vi.RenderRect;
    }

    KON_BKD_Draw(surface, rect, &ScaledDstRect, flags);
}

void KON_DrawSurfaceRectEx(KON_Surface* surface, const KON_Rect* rect, const Vector2d* pos, DrawFlags flags) {
    KON_Rect dest;

    dest.x = pos->x;
    dest.y = pos->y;
    dest.w = surface->width;
    dest.y = surface->height;
    KON_DrawScaledSurfaceRectEx(surface, rect, &dest, flags);
}

void KON_DrawScaledSurfaceEx(KON_Surface* surface, const KON_Rect* dest, DrawFlags flags) {
    KON_DrawScaledSurfaceRectEx(surface, NULL, dest, flags);
}

void KON_DrawSurfaceEx(KON_Surface* surface, Vector2d* pos, DrawFlags flags) {
    KON_Rect dest;
    
    dest.x = pos->x;
    dest.y = pos->y;
    dest.w = surface->width;
    dest.h = surface->height;

    KON_DrawScaledSurfaceRectEx(surface, NULL, &dest, flags);
}

void KON_ClearTargetSurface(KON_Surface* target) {
    if (!KON_SetRenderTarget(target)) {
        KON_ClearScreen();
        KON_SetRenderTarget(NULL);
    }
}

void KON_ShowDebugScreen(void) {
    KON_BKD_Debug();
}
