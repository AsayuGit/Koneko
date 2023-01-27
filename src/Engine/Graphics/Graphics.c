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
#include "Rect.h"
#include "KON_BKD_Video.h"

uint32_t getpixel(KON_CPUSurface *surface, int x, int y) {
    return KON_BKD_GetPixel(surface, x, y);
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
