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

#include <limits.h>
#include <malloc.h>
#include <string.h>

#include <ogcsys.h>
#include <gccore.h>

#include "Bool.h"

#define DEFAULT_FIFO_SIZE (256*1024)
typedef void* FIFO;
typedef void* XFB;
typedef GXTexObj KON_Texture;

typedef TPLFile KON_CPUSurface;

XFB xfb[2];
bool currentXfb;
GXRModeObj* rmode;
FIFO fifo;
bool pageflip;

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

    GX_Init(fifo, DEFAULT_FIFO_SIZE);
    GX_SetCopyClear(white, GX_MAX_Z24);

    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    xfbHeight = GX_SetDispCopyYScale(GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight));

    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);

    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);

    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

    if (rmode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    GX_SetCullMode(GX_CULL_NONE);
    GX_CopyDisp(xfb[currentXfb], GX_TRUE);
    GX_SetDispCopyGamma(GX_GM_1_0);

    KON_GCInitVAT();
    KON_GCInitOrthCamera();
}

void KON_BKD_InitVideo(int screenResX, int screenResY, char* title) {
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    fifo = memalign(32, DEFAULT_FIFO_SIZE);
    memset(fifo, 0, DEFAULT_FIFO_SIZE);

    xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    CON_Init(xfb[0], 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    pageflip = true;

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb[currentXfb]);
    VIDEO_SetBlack(false);
    VIDEO_Flush();

    /* Wait for vi to be ready */
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
    currentXfb ^= 1;

    KON_GCInitGX();
    printf("\x1b[10;0H");
}

void KON_BKD_FreeVideo(void) {}

KON_CPUSurface* KON_BKD_LoadCPUSurface(const char* path) {
    KON_CPUSurface surface;

    if (TPL_OpenTPLFromFile(&surface, path) != 1) {
        KON_SystemMsg("(KON_BKD_LoadCPUSurface) TPL_OpenTPLFromFile couldn't open :\n", MESSAGE_ERROR, 1, path);
        return NULL;
    }

    return surface;
}

KON_CPUSurface* KON_BKD_LoadCPUSurfaceFromMem(BITMAP* bitmap) {
    return NULL;
}

KON_Surface* KON_BKD_UploadSurface(KON_CPUSurface* surface) {
    KON_Surface* gpuSurface;

    if (!(gpuSurface = (KON_Surface*)calloc(1, sizeof(KON_Surface)))) {
        KON_SystemMsg("(KON_BKD_UploadSurface) No more memory", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (TPL_GetTexture(&gpuSurface->texture, 0, &surface) != 0) {
        KON_SystemMsg("(KON_BKD_LoadCPUSurface) TPL_GetTexture couldn't retreive texture\n", MESSAGE_ERROR, 0);
        return NULL;
    }
    
    if (TPL_GetTextureInfo(&surface, 0, NULL, &gpuSurface->width, &gpuSurface->height)) {
        KON_SystemMsg("(KON_BKD_LoadCPUSurface) Couldn't get texture size", MESSAGE_ERROR, 0);
        return NULL;
    }

    return gpuSurface;
}

KON_Surface* KON_BKD_CreateSurface(unsigned int width, unsigned int height) {
    return NULL;
}

void KON_BKD_FreeCPUSurface(KON_CPUSurface* surface) {}

void KON_BKD_FreeSurface(KON_Surface* surface) {}

void KON_BKD_KeyCpuSurface(KON_CPUSurface* surface, uint32_t colorKey) {}

void KON_BKD_QuerryCPUSurface(KON_CPUSurface* surface, unsigned int* bpp, size_t* pitch, unsigned int* width, unsigned int* height) {
    if (bpp) *bpp = 0;
    if (pitch) *pitch = 0;
    if (width) *width = 0;
    if (height) *height = 0;
}

void KON_BKD_QuerrySurface(KON_Surface* surface, Vector2d* size) {
    if (!surface || !size) return;
    size->x = surface->width;
    size->y = surface->height;
}

void* KON_BKD_GetPixelData(KON_CPUSurface* surface) {
    return NULL;
}

uint32_t KON_BKD_GetPixel(KON_CPUSurface* surface, unsigned int x, unsigned int y) {
    return 0;
}

void KON_BKD_LockCPUSurface(KON_CPUSurface* surface) {}

void KON_BKD_UnlockCPUSurface(KON_CPUSurface* surface) {}

int KON_BKD_SetRenderSurface(KON_Surface* surface) {
    return 0;
}

void KON_BKD_SetRenderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    GXColor renderColor = {r, g, b, a};
    GX_SetCopyClear(renderColor, GX_MAX_Z24);
}

void KON_BKD_DrawCoverFrame(KON_Rect Frame[4]) {}

void KON_BKD_UpdateRenderRect(KON_Rect* RenderRect, Vector2i* ScreenResolution) {
    Koneko.dDevice.IRScalar = 1;
    RenderRect->w = ScreenResolution->x = 640;
    RenderRect->h = ScreenResolution->y = 480;
    RenderRect->x = RenderRect->y = 0;
}

void KON_BKD_SetVSync(bool value) {}

void KON_BKD_Draw(KON_Surface* surface, const KON_Rect* srcrect, const KON_Rect* dstrect, uint8_t flags) {
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
}

void KON_BKD_Display(void) {
    if (pageflip) {
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GX_SetColorUpdate(GX_TRUE);
        GX_CopyDisp(vi.xfb[vi.currentXfb], GX_TRUE);

        GX_DrawDone();

        VIDEO_SetNextFramebuffer(vi.xfb[vi.currentXfb]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
        currentXfb ^= 1;
    } else {
        VIDEO_SetNextFramebuffer(vi.xfb[0]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
    }
}

void KON_BKD_ClearScreen(void) {}

void KON_BKD_Debug(void) {
    if (!pageflip)
        return;
    KON_GCRender();
    pageflip = false;
}