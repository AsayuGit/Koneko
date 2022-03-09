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

#ifndef _KON_DISPLAYDEVICE_H
#define _KON_DISPLAYDEVICE_H

    #include "API.h"
    #include "Rect.h"

    typedef struct {
        Vector2d position;
        Vector2d direction;
        Vector2d plane;
        double cameraHeight;
    } KON_Camera;

    typedef struct {
        SDL_Window *Screen;
        SDL_Renderer *Renderer;

        KON_Rect Frame[4];              /* Screen Border Frame */

        KON_Rect RenderRect;            /* Where the game is drawn on screen */
        Vector2i InternalResolution;    /* Internal render resolution of the game */
        Vector2i ScreenResolution;      /* The external resolution of the game */
        double IRScalar;                /* The scaling nessary match the external resolution */
        bool integerScalling;           /* If set to true then the output will be pixel perfect (provided the winow is big enough) */
        KON_Camera camera;              /* The in game camera */

        bool OffScreenRender;

        /* Timing control */
        uint32_t lastFrame;
        uint32_t frametime;
    } DisplayDevice;

    void KON_CreateDisplayDevice(int resX, int resY, char* gameTitle);
    void KON_FreeDisplayDevice();

    void KON_UpdateRenderRect();

    /*
        SUMMARY : Set the camera fov (in radians)
        INPUT   : double fov : The new camera fov.
    */
    void KON_SetCameraFOVRAD(double fov);

    /*
        SUMMARY : Set the camera fov (in degrees)
        INPUT   : double fov : The new camera fov.
    */
    void KON_SetCameraFOV(double fov);

    /*
        SUMMARY : Sets the camera fov to its optimal value for the current resolution.
    */
    void KON_ResetCameraFOV();
    void KON_CenterCameraOnCoordinates(double X, double Y);
    void KON_BoundCameraToRegion(KON_Rect Region);
    void KON_RotateCamera(double angle);

#endif