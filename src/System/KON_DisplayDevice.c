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
    int ScreenWidth, ScreenHeight; /* Signed because SDL said so :c */

    SDL_GetWindowSize(Koneko.dDevice.Screen, &ScreenWidth, &ScreenHeight);

    if (Koneko.dDevice.integerScalling && (ScreenWidth > Koneko.dDevice.InternalResolution.x) && (ScreenHeight > Koneko.dDevice.InternalResolution.y))
        Koneko.dDevice.IRScalar = MIN(ScreenWidth / Koneko.dDevice.InternalResolution.x, ScreenHeight / Koneko.dDevice.InternalResolution.y);
    else
        Koneko.dDevice.IRScalar = MIN((double)ScreenWidth / (double)Koneko.dDevice.InternalResolution.x, (double)ScreenHeight / (double)Koneko.dDevice.InternalResolution.y);

    Koneko.dDevice.ScreenResolution.x = ScreenWidth;
    Koneko.dDevice.ScreenResolution.y = ScreenHeight;

    Koneko.dDevice.RenderRect.w = (int)(Koneko.dDevice.InternalResolution.x * Koneko.dDevice.IRScalar);
    Koneko.dDevice.RenderRect.h = (int)(Koneko.dDevice.InternalResolution.y * Koneko.dDevice.IRScalar);

    Koneko.dDevice.RenderRect.x = (int)(Koneko.dDevice.ScreenResolution.x - Koneko.dDevice.RenderRect.w) >> 1;
    Koneko.dDevice.RenderRect.y = (int)(Koneko.dDevice.ScreenResolution.y - Koneko.dDevice.RenderRect.h) >> 1;

    Koneko.dDevice.OffScreenRender = false;

    KON_InitRect(Koneko.dDevice.Frame[0], 0, 0, Koneko.dDevice.RenderRect.x, ScreenHeight);                                                                                                            /* Left Frame */
    KON_InitRect(Koneko.dDevice.Frame[1], Koneko.dDevice.RenderRect.x + Koneko.dDevice.RenderRect.w, 0, Koneko.dDevice.RenderRect.x, ScreenHeight);                                                /* Right Frame */
    KON_InitRect(Koneko.dDevice.Frame[2], Koneko.dDevice.RenderRect.x, 0, Koneko.dDevice.RenderRect.w, Koneko.dDevice.RenderRect.y);                                                               /* Top Frame */
    KON_InitRect(Koneko.dDevice.Frame[3], Koneko.dDevice.RenderRect.x, Koneko.dDevice.RenderRect.y + Koneko.dDevice.RenderRect.h, Koneko.dDevice.RenderRect.w, Koneko.dDevice.RenderRect.y);   /* Bottom Frame */
}

void KON_SetVSync(bool value) {
    SDL_GL_SetSwapInterval(value);
}

void KON_CreateDisplayDevice(int resX, int resY, char* gameTitle) {
    Koneko.dDevice.Screen = SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resX, resY, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    Koneko.dDevice.Renderer = SDL_CreateRenderer(Koneko.dDevice.Screen , -1, 0);
    if (Koneko.dDevice.Renderer == NULL){
        KON_SystemMsg("(KON_CreateDisplayDevice) Can't create main renderer : ", MESSAGE_ERROR, 1, SDL_GetError());
    }
    KON_SetVSync(true);

    /* FIXME : TEMPORARY: Makes the start resulution de default internal resolution */
    Koneko.dDevice.InternalResolution.x = resX;
    Koneko.dDevice.InternalResolution.y = resY;

    Koneko.dDevice.camera.direction = KON_InitVector2d(0, -1);
    Koneko.dDevice.camera.plane = KON_InitVector2d(1, 0);
    Koneko.dDevice.camera.cameraHeight = 0.5;

    KON_ResetCameraFOV();

    KON_UpdateRenderRect();
}

void KON_FreeDisplayDevice() {
    SDL_DestroyRenderer(Koneko.dDevice.Renderer);
    SDL_DestroyWindow(Koneko.dDevice.Screen);
}