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

#ifdef _XBOX
	#define _POSIX_
	#include <limits.h>
#elif defined(GEKKO)
    #include <limits.h>
    #include <malloc.h>
    #include <string.h>

    #include <ogcsys.h>
    #include <gccore.h>
#else
	#include <linux/limits.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifdef GEKKO
    #define DEFAULT_FIFO_SIZE (256*1024)
    typedef void* FIFO;
    typedef void* XFB;
#endif

typedef struct {
    #ifdef GEKKO
        /* TODO: implement libogc */
        XFB xfb[2];
        bool currentXfb;
        GXRModeObj* rmode;
        FIFO fifo;
        bool pageflip;
    #else
        SDL_Window *Screen;
        SDL_Renderer *Renderer;
    #endif
    KON_Rect Frame[4];              /* Screen Border Frame */
    KON_Rect RenderRect;            /* Where the game is drawn on screen */
    Vector2i ScreenResolution;      /* The external resolution of the game */
} KON_VideoInterface;


/* Abstract Koneko's surfaces from their implementations */
struct KON_Surface {
    bool readOnly;
    #ifdef GEKKO
        GXTexObj surface;
        uint16_t width;
        uint16_t height;
    #else
        SDL_Texture* surface;
        unsigned int width;
        unsigned int height;
    #endif
};

struct KON_CPUSurface {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_Surface* surface;
    #endif
};

static KON_VideoInterface vi;

extern struct BITMAP_SystemFont { int width; int height; int depth; int pitch; uint8_t pixels[113772];} SystemFont;

static bool drawFPS;
static BitmapFont* font;
static KON_Surface* fpsTextBox;
static const KON_Rect fpsTextBoxSize = {0, 0, 100, 50};

#ifdef GEKKO
    static void KON_GCInitOrthCamera() {
        Mtx44 orthographicMatrix;

        // 640x480 lines
        guOrtho(orthographicMatrix, 0, 479, 0, 639, 0, 300);
        GX_LoadProjectionMtx(orthographicMatrix, GX_ORTHOGRAPHIC);
    }

    static void KON_GCInitVAT() {
        GX_ClearVtxDesc();

        GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);

        GX_SetNumChans(1);
        GX_SetNumTexGens(1);

        GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

        GX_InvVtxCache();
        GX_InvalidateTexAll();
    }

    static void KON_GCInitGX() {
        GXColor white = {255, 255, 255, 0xff};
        uint32_t xfbHeight;

        GX_Init(vi.fifo, DEFAULT_FIFO_SIZE);
        GX_SetCopyClear(white, GX_MAX_Z24);

        GX_SetViewport(0, 0, vi.rmode->fbWidth, vi.rmode->efbHeight, 0, 1);
        xfbHeight = GX_SetDispCopyYScale(GX_GetYScaleFactor(vi.rmode->efbHeight, vi.rmode->xfbHeight));

        GX_SetScissor(0, 0, vi.rmode->fbWidth, vi.rmode->efbHeight);

        GX_SetDispCopySrc(0, 0, vi.rmode->fbWidth, vi.rmode->efbHeight);
        GX_SetDispCopyDst(vi.rmode->fbWidth, xfbHeight);

        GX_SetCopyFilter(vi.rmode->aa, vi.rmode->sample_pattern, GX_TRUE, vi.rmode->vfilter);
        GX_SetFieldMode(vi.rmode->field_rendering, ((vi.rmode->viHeight == 2 * vi.rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

        if (vi.rmode->aa)
            GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
        else
            GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

        GX_SetCullMode(GX_CULL_NONE);
        GX_CopyDisp(vi.xfb[vi.currentXfb], GX_TRUE);
        GX_SetDispCopyGamma(GX_GM_1_0);

        KON_GCInitVAT();
        KON_GCInitOrthCamera();
    }

    static void KON_GCInit() {
        VIDEO_Init();
        vi.rmode = VIDEO_GetPreferredMode(NULL);
        vi.fifo = memalign(32, DEFAULT_FIFO_SIZE);
        memset(vi.fifo, 0, DEFAULT_FIFO_SIZE);

        vi.xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(vi.rmode));
        vi.xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(vi.rmode));
        CON_Init(vi.xfb[0], 20, 20, vi.rmode->fbWidth, vi.rmode->xfbHeight, vi.rmode->fbWidth * VI_DISPLAY_PIX_SZ);
        vi.pageflip = true;

        VIDEO_Configure(vi.rmode);
        VIDEO_SetNextFramebuffer(vi.xfb[vi.currentXfb]);
        VIDEO_SetBlack(false);
        VIDEO_Flush();

        /* Wait for vi to be ready */
        VIDEO_WaitVSync();
        if (vi.rmode->viTVMode & VI_NON_INTERLACE)
            VIDEO_WaitVSync();
        vi.currentXfb ^= 1;

        KON_GCInitGX();
        printf("\x1b[10;0H");
    }

    static void KON_GCRender() {
        if (vi.pageflip) {
            GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
            GX_SetColorUpdate(GX_TRUE);
            GX_CopyDisp(vi.xfb[vi.currentXfb], GX_TRUE);

            GX_DrawDone();

            VIDEO_SetNextFramebuffer(vi.xfb[vi.currentXfb]);
            VIDEO_Flush();
            VIDEO_WaitVSync();
            vi.currentXfb ^= 1;
        } else {
            VIDEO_SetNextFramebuffer(vi.xfb[0]);
            VIDEO_Flush();
            VIDEO_WaitVSync();
        }
    }
#endif

void KON_SetDrawFPS(bool value) {
    drawFPS = value;
}

void KON_DrawFPS() {
    char fpsText[100];
    double fps = 1000.0 / Koneko.dDevice.frametime;

    sprintf(fpsText, "FPS: %.2f\nFrametime: %u ms", fps, Koneko.dDevice.frametime);
    KON_Print(font, fpsTextBox, fpsText, 1, 0, 0);
    KON_DrawScaledSurface(fpsTextBox, &fpsTextBoxSize);
}

static void DrawBordingFrame() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_RenderFillRect(vi.Renderer, (SDL_Rect*)&vi.Frame[0]);
        SDL_RenderFillRect(vi.Renderer, (SDL_Rect*)&vi.Frame[1]);
        SDL_RenderFillRect(vi.Renderer, (SDL_Rect*)&vi.Frame[2]);
        SDL_RenderFillRect(vi.Renderer, (SDL_Rect*)&vi.Frame[3]);
    #endif
}

void KON_FinishFrame() {
    uint32_t ticks;
    DrawBordingFrame();

    if (drawFPS)
        KON_DrawFPS();

    #ifdef GEKKO
        KON_GCRender();
    #else
        SDL_RenderPresent(vi.Renderer);
    #endif

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

void KON_ResetCameraFOV() {
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

void KON_UpdateRenderRect() {

    #ifdef GEKKO
        Koneko.dDevice.IRScalar = 1;
        vi.RenderRect.w = vi.ScreenResolution.x = 640;
        vi.RenderRect.h = vi.ScreenResolution.y = 480;
        vi.RenderRect.x = vi.RenderRect.y = 0;
    #else
        int ScreenWidth, ScreenHeight; /* Signed because SDL said so :c */
        
        SDL_GetWindowSize(vi.Screen, &ScreenWidth, &ScreenHeight);

        if (Koneko.dDevice.integerScalling && (ScreenWidth > Koneko.dDevice.InternalResolution.x) && (ScreenHeight > Koneko.dDevice.InternalResolution.y))
            Koneko.dDevice.IRScalar = MIN(ScreenWidth / Koneko.dDevice.InternalResolution.x, ScreenHeight / Koneko.dDevice.InternalResolution.y);
        else
            Koneko.dDevice.IRScalar = MIN((double)ScreenWidth / (double)Koneko.dDevice.InternalResolution.x, (double)ScreenHeight / (double)Koneko.dDevice.InternalResolution.y);

        vi.ScreenResolution.x = ScreenWidth;
        vi.ScreenResolution.y = ScreenHeight;   

        vi.RenderRect.w = (int)(Koneko.dDevice.InternalResolution.x * Koneko.dDevice.IRScalar);
        vi.RenderRect.h = (int)(Koneko.dDevice.InternalResolution.y * Koneko.dDevice.IRScalar);

        vi.RenderRect.x = (int)(vi.ScreenResolution.x - vi.RenderRect.w) >> 1;
        vi.RenderRect.y = (int)(vi.ScreenResolution.y - vi.RenderRect.h) >> 1;
    #endif

    KON_InitRect(vi.Frame[0], 0, 0, vi.RenderRect.x, vi.ScreenResolution.y);                                                    /* Left Frame */
    KON_InitRect(vi.Frame[1], vi.RenderRect.x + vi.RenderRect.w, 0, vi.RenderRect.x, vi.ScreenResolution.y);                    /* Right Frame */
    KON_InitRect(vi.Frame[2], vi.RenderRect.x, 0, vi.RenderRect.w, vi.RenderRect.y);                                   /* Top Frame */
    KON_InitRect(vi.Frame[3], vi.RenderRect.x, vi.RenderRect.y + vi.RenderRect.h, vi.RenderRect.w, vi.RenderRect.y);   /* Bottom Frame */
}

void KON_ClearScreen() {
    #ifdef GEKKO
        /* TODO: Implement libogc */
    #else
        SDL_RenderClear(vi.Renderer);
    #endif
}

void KON_SetVSync(bool value) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_GL_SetSwapInterval(value);
    #endif
}

void KON_InitDisplayDevice(int resX, int resY, char* gameTitle) {
    BITMAP SystemFontBitmap;

    #ifdef GEKKO
        KON_GCInit();
    #else
        vi.Screen = SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        vi.Renderer = SDL_CreateRenderer(vi.Screen , -1, 0);
        if (vi.Renderer == NULL)
            KON_SystemMsg("(KON_CreateDisplayDevice) Can't create main renderer : ", MESSAGE_ERROR, 1, SDL_GetError());
    #endif

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

    font = KON_LoadBitmapFontFromMem(&SystemFontBitmap, 0xff00ff);
    fpsTextBox = KON_CreateSurface(fpsTextBoxSize.w, fpsTextBoxSize.h);
}

void KON_FreeDisplayDevice() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_DestroyRenderer(vi.Renderer);
        SDL_DestroyWindow(vi.Screen);
    #endif
}

/*
    SUMMARY : Free a GPU-Side surface
    INPUT   : SDL_Texture* surface : Surface to be freed
*/
void KON_FreeRawSurface(KON_Surface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_DestroyTexture(surface->surface);
    #endif
}

/*
    SUMMARY : Applies a ColorKey to a CPU-Side Surface
    INPUT   : KON_CPUSurface* SurfaceToKey : Pointer to the Surface to key
    INPUT   : uint8_t ColorKey          : The ColorKey to apply to the Surface
*/
void KON_KeyCpuSurface(KON_CPUSurface* SurfaceToKey, uint32_t ColorKey) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (SDL_SetColorKey(SurfaceToKey->surface, true, ColorKey) < 0)
            KON_SystemMsg("() couldn't set color key : ", MESSAGE_WARNING, 1, SDL_GetError());
    #endif
}

/*
    SUMMARY : Loads a CPU-Side Surface from disk (Unmanaged)
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : KON_CPUSurface*          : Pointer to the loaded surface (UnMannaged)
*/
static KON_CPUSurface* KON_LoadRawCPUSurface(const char* FilePath, uint32_t ColorKey, uint8_t flags) {
    KON_CPUSurface* loadingCPUSurface = NULL;

    if (!FilePath)
        return NULL;
    if (!(loadingCPUSurface = (KON_CPUSurface*)malloc(sizeof(KON_CPUSurface)))) {
        KON_SystemMsg("(KON_LoadRawCPUSurface) Couldn't allocate more memory !\n", MESSAGE_ERROR, 0);
        return NULL;
    }

    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (!(loadingCPUSurface->surface = SDL_LoadBMP(FilePath))) {
            KON_SystemMsg("(KON_LoadRawCPUSurface) Couldn't load : ", MESSAGE_WARNING, 2, FilePath, SDL_GetError());
            return NULL;
        }
    #endif

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsg("(KON_LoadRawCPUSurface) Loaded NEW CPU Surface : ", MESSAGE_LOG, 1, FilePath);

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

    if (!(loadingCPUSurface = (KON_CPUSurface*)malloc(sizeof(KON_CPUSurface)))) {
        KON_SystemMsg("(KON_LoadCPUSurfaceFromMem) Couldn't allocate more memory !\n", MESSAGE_ERROR, 0);
        return NULL;
    }

    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (!(loadingCPUSurface->surface = SDL_CreateRGBSurfaceWithFormatFrom(bitmap->pixels, bitmap->width, bitmap->height, bitmap->depth, bitmap->pitch, SDL_PIXELFORMAT_RGB24))) {
            KON_SystemMsg("(KON_LoadCPUSurfaceFromMem) Couldn't load memory surface : ", MESSAGE_WARNING, 1, SDL_GetError());
            return NULL;
        }
    #endif

    if (SURFACE_KEYED & flags)
        KON_KeyCpuSurface(loadingCPUSurface, ColorKey);

    KON_SystemMsg("(KON_LoadCPUSurfaceFromMem) Loaded NEW CPU Surface from memory", MESSAGE_LOG, 0);

    return loadingCPUSurface;
}

/*
    SUMMARY : Loads a GPU-Side Surface from disk (Unmanaged)
    INPUT   : char* FilePath        : Path to the surface to load
    INPUT   : uint32_t ColorKey     : The surface's color to key out
    INPUT   : uint8_t flags         : Wether the surface should be keyed or alpha
    OUTPUT  : KON_Surface*          : Pointer to the loaded surface (UnMannaged) null on error
*/
static KON_Surface* KON_LoadRawSurface(const char* FilePath, uint32_t ColorKey, uint8_t flags) {
    #ifdef GEKKO
        uint16_t surfaceWidth, surfaceHeight;
        TPLFile surfaceTplFile;
        GXTexObj loadedGPUSurface;
    #else
        KON_CPUSurface* loadedCPUSurface = NULL;
        SDL_Texture* loadedGPUSurface;
        int surfaceWidth, surfaceHeight;
    #endif

    KON_Surface* loadedSurface = NULL;

    #ifdef GEKKO
        if (TPL_OpenTPLFromFile(&surfaceTplFile, FilePath) != 1) {
            KON_SystemMsg("(KON_LoadSurface) TPL_OpenTPLFromFile couldn't open :\n", MESSAGE_ERROR, 1, FilePath);
            return NULL;
        }

        if (TPL_GetTexture(&surfaceTplFile, 0, &loadedGPUSurface) != 0) {
            KON_SystemMsg("(KON_LoadSurface) TPL_GetTexture couldn't retreive texture\n", MESSAGE_ERROR, 0);
            return NULL;
        }
    #else
        if (!(loadedCPUSurface = KON_LoadRawCPUSurface(FilePath, ColorKey, flags)))
            return NULL;

        if (SURFACE_KEYED & flags)
            KON_KeyCpuSurface(loadedCPUSurface, ColorKey);

        if (!(loadedGPUSurface = SDL_CreateTextureFromSurface(vi.Renderer, loadedCPUSurface->surface))) {
            KON_SystemMsg("(KON_LoadRawSurface) Couldn't upload surface to GPU : ", MESSAGE_WARNING, 2, FilePath, SDL_GetError());
            return NULL;
        }

        KON_FreeCPUSurface(loadedCPUSurface);
    #endif

    if (!(loadedSurface = (KON_Surface*)calloc(1, sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_LoadSurface) Couldn't allocate memory", MESSAGE_ERROR, 0);
        return NULL;
    }

    #ifdef GEKKO
        if (TPL_GetTextureInfo(&surfaceTplFile, 0, NULL, &surfaceWidth, &surfaceHeight)) {
            KON_SystemMsg("(KON_LoadSurface) Couldn't get texture size", MESSAGE_ERROR, 0);
            return NULL;
        }
    #else
        SDL_QueryTexture(loadedGPUSurface, NULL, NULL, &surfaceWidth, &surfaceHeight);
    #endif

    loadedSurface->surface = loadedGPUSurface;
    loadedSurface->width = surfaceWidth;
    loadedSurface->height = surfaceHeight;

    return loadedSurface;
}

KON_Surface* KON_CreateSurface(unsigned int width, unsigned int height) {
    KON_Surface* newSurface = NULL;
    SDL_Texture* renderTexture = NULL;

    if (!(newSurface = (KON_Surface*)calloc(1, sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_CreateSurface) Couldn't create surface", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(renderTexture = SDL_CreateTexture(vi.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height))) {
        free(newSurface);
        KON_SystemMsg("(KON_CreateSurface) Couldn't create surface", MESSAGE_ERROR, 0);
        return NULL;
    }
    SDL_SetTextureBlendMode(renderTexture, SDL_BLENDMODE_BLEND);

    newSurface->width = width;
    newSurface->height = height;
    newSurface->surface = renderTexture;
    newSurface->readOnly = false;

    return newSurface;
}

KON_Surface* KON_LoadSurface(const char* filePath, uint32_t colorKey, uint8_t flags) {
    KON_Surface* loadedSurface;

    if (!filePath) {
        KON_SystemMsg("(KON_LoadSurface) Incorrect Parameters", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (!(loadedSurface = (KON_Surface*)KON_GetManagedRessource(filePath, RESSOURCE_GPU_SURFACE))) {
        if (!(loadedSurface = KON_LoadRawSurface(filePath, colorKey, flags))) {
            KON_SystemMsg("(KON_LoadSurface) Failed to load : ", MESSAGE_ERROR, 1, filePath);
            return NULL;
        }
        KON_AddManagedRessource(filePath, RESSOURCE_GPU_SURFACE, loadedSurface);
        KON_SystemMsg("(KON_LoadSurface) Loaded NEW GPU Surface : ", MESSAGE_LOG, 1, filePath);
        return loadedSurface;
    }

    KON_SystemMsg("(KON_LoadSurface) Referenced GPU Surface : ", MESSAGE_LOG, 1, filePath);
    return loadedSurface;
}

/* Unmanaged */
KON_CPUSurface* KON_LoadCpuSurface(char* filePath, uint32_t colorKey, uint8_t flags) {
    if (!filePath)
        return NULL;

    return KON_LoadRawCPUSurface(filePath, colorKey, flags);
}

void KON_GetCPUSurfaceSize(KON_CPUSurface* surface, size_t* pitch, unsigned int* w, unsigned int* h) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        *pitch = surface->surface->pitch;
        *w = surface->surface->w;
        *h = surface->surface->h;
    #endif
}

void* KON_GetCPUSurfacePixelData(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
        return NULL;
    #else
        return surface->surface->pixels;
    #endif
}

void KON_LockCPUSurface(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_LockSurface(surface->surface);
    #endif
}

void KON_UnlockCPUSurface(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_UnlockSurface(surface->surface);
    #endif
}

void KON_FreeSurface(KON_Surface* surface) {
    KON_FreeRawSurface(KON_FreeManagedRessourceByRef(surface));
}

void KON_FreeCPUSurface(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_FreeSurface(surface->surface);
    #endif

    free(surface);
}

unsigned int KON_GetCPUSurfaceBPP(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
        return 8;
    #else
        return surface->surface->format->BytesPerPixel;
    #endif
}

unsigned int KON_GetCPUSurfacePitch(KON_CPUSurface* surface) {
    #ifdef GEKKO
        /* TODO: implement libogc */
        return 0;
    #else
        return surface->surface->pitch;
    #endif
}

/* Unmanaged */
KON_Surface* KON_CpuToGpuSurface(KON_CPUSurface* cpuSurface) {
    KON_Surface* newSurface = (KON_Surface*)malloc(sizeof(KON_Surface));

    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (!(newSurface->surface = SDL_CreateTextureFromSurface(vi.Renderer, cpuSurface->surface)))
            return NULL;

        SDL_QueryTexture(newSurface->surface, NULL, NULL, (int*)&newSurface->width, (int*)&newSurface->height);
    #endif

    return newSurface;
}

void KON_GetSurfaceSize(KON_Surface* surface, Vector2d* size) {
    if (surface && size) {
        size->x = surface->width;
        size->y = surface->height;
    }
}

int KON_SetRenderTarget(KON_Surface* target) {
    #ifdef GEKKO
        /* TODO: implement libogc */
        return 0;
    #else
        if (target && target->readOnly) {
            KON_SystemMsg("(KON_SetRenderTarget) Sprite is readonly!", MESSAGE_WARNING, 0);
            return -1;
        }
        return SDL_SetRenderTarget(vi.Renderer, target ? target->surface : NULL);
    #endif
}

void KON_SetRenderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    #ifdef GEKKO
        GXColor renderColor = {r, g, b, a};

        GX_SetCopyClear(renderColor, GX_MAX_Z24);
    #else
        /* TODO: Implement SDL */
    #endif
}

/* API level draw */
static int KON_DrawEx(KON_Surface* surface, const KON_Rect* srcrect, const KON_Rect* dstrect, uint8_t flags) {
    /* TODO: Flags decoding */
    flags = 0;

    #ifdef GEKKO
        f32 normSrectX, normSrectY, normSrectW, normSrectH;

        if (!srcrect) {
            normSrectX = 0;
            normSrectY = 0;
            normSrectW = 1;
            normSrectH = 1;
        } else {
            normSrectX = (f32)srcrect->x / (f32)surface->width;
            normSrectY = (f32)srcrect->y / (f32)surface->height;
            normSrectW = ((f32)srcrect->x + (f32)srcrect->w) / (f32)surface->width;
            normSrectH = ((f32)srcrect->y + (f32)srcrect->h) / (f32)surface->height;
        }

        GX_LoadTexObj(&surface->surface, GX_TEXMAP0);
        
        GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        {
            /* TODO: Implement sourcerect for gx */

            // Top Left
            GX_Position2f32(dstrect->x, dstrect->y);
            GX_Color3f32(1.0f,1.0f,1.0f);
            GX_TexCoord2f32(normSrectX, normSrectY);

            // Top Right
            GX_Position2f32(dstrect->x + dstrect->w, dstrect->y);
            GX_Color3f32(1.0f,1.0f,1.0f);
            GX_TexCoord2f32(normSrectW, normSrectY);

            // Bottom Right
            GX_Position2f32(dstrect->x + dstrect->w, dstrect->y + dstrect->h);
            GX_Color3f32(1.0f,1.0f,1.0f);
            GX_TexCoord2f32(normSrectW, normSrectH);

            // Bottom Left
            GX_Position2f32(dstrect->x, dstrect->y + dstrect->h);
            GX_Color3f32(1.0f,1.0f,1.0f);
            GX_TexCoord2f32(normSrectX, normSrectH);
        }
        GX_End();
        return 0;
    #else
        return SDL_RenderCopyEx(vi.Renderer, surface->surface, (SDL_Rect*)srcrect, (SDL_Rect*)dstrect, 0, 0, flags);
    #endif
}

#define KON_Draw(dDevice, texture, srcrect, dstrect) KON_DrawEx(dDevice, texture, srcrect, dstrect, DRAW_DEFAULT)

void KON_DrawScaledSurfaceRectEx(KON_Surface* surface, const KON_Rect* rect, const KON_Rect* dest, DrawFlags flags) {
    KON_Rect ScaledDstRect;

    KON_InitRect(ScaledDstRect, 0, 0, Koneko.dDevice.InternalResolution.x, Koneko.dDevice.InternalResolution.y);
    
    if (!surface)
        return;

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

    KON_DrawEx(surface, rect, &ScaledDstRect, flags);
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

void KON_ShowDebugScreen() {
    #ifdef GEKKO
        if (!vi.pageflip)
            return;
        KON_GCRender();
        vi.pageflip = false;
    #else
        /* TODO: Implement SDL */
    #endif
}