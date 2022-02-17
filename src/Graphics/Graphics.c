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
#include "Graphics.h"
#include "System.h"
#include "TileMap.h"

uint32_t getpixel(KON_CPUSurface *surface, int x, int y)
{
    /* Declaration */
    unsigned int bpp;
    uint8_t *p;
    void* pixelData;

    /* Init */
    if (!surface){
        printf("ERROR: (getpixel) No surface provided !\n");
        return 0; /* Failsafe */
    } else if (!(pixelData = KON_GetCPUSurfacePixelData(surface))){
        /*printf("ERROR: (getpixel) No pixel data !\n");*/
        return 0; /* Failsafe */
    }

    bpp = KON_GetCPUSurfaceBPP(surface);
    p = (uint8_t *)pixelData + y * KON_GetCPUSurfacePitch(surface) + x * bpp; /* Here p is the address to the pixel we want to retrieve */

    /*printf("%x %d %d %d %d\n", surface->pixels, y, surface->pitch, x, bpp);*/

    /* Logic */
    switch(bpp) {
    case 1:
        /*printf("DEBUG 1)\n");*/
        return *p;
        break;

    case 2:
        /*printf("DEBUG 2)\n");*/
        return *(uint8_t *)p;
        break;

    case 3:
        /*printf("DEBUG 3)\n");*/
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        /*printf("DEBUG 4)\n");*/
        /*printf("Nyan 0x%.6x\n", *(Uint32*)p);*/
        return *((uint32_t*)p);
        break;

    default:
        return 0;
        break;
    }
    return 0;
}

bool RectOnRect(const KON_Rect* SrcRect, const KON_Rect* DstRect){
    if (!SrcRect || !DstRect)
        return false;
    if ((SrcRect->x + SrcRect->w) < DstRect->x)
        return false;
    if (SrcRect->x > (DstRect->x + DstRect->w))
        return false;
    if ((SrcRect->y + SrcRect->h) < DstRect->y)
        return false;
    if (SrcRect->y > (DstRect->y + DstRect->h))
        return false;
    return true;
}

bool RectOnScreen(const KON_Rect* Rect){
    KON_Rect BaseRect = {0};
    
    BaseRect.w = Koneko.dDevice.InternalResolution.x;
    BaseRect.h = Koneko.dDevice.InternalResolution.y;
    return RectOnRect(Rect, &BaseRect);
}

void CenterCameraOnCoordinates(double X, double Y){
    Koneko.dDevice.camera.position.x = X - (Koneko.dDevice.InternalResolution.x >> 1);
    Koneko.dDevice.camera.position.y = Y - (Koneko.dDevice.InternalResolution.y >> 1);
}

void BoundCameraToRegion(KON_Rect Region){
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