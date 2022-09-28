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

#define _GNU_SOURCE

#include "Koneko.h"
#include "Renderer.h"
#include "TileMap.h"
#include "KON_Raycast.h"

void KON_RenderLayer(MapLayer* layer) {
    switch (layer->layerRenderer)
    {
    case RENDER_2D_TILEMAP:
        KON_DrawTileMap(layer);
        KON_DrawDisplayListAt(layer->displayList, layer->pos.x, layer->pos.y);
        break;

    case RENDER_2D_BITMAP:
        KON_DrawBitMap(layer);
        KON_DrawDisplayListAt(layer->displayList, layer->pos.x, layer->pos.y);
        break;

    case RENDER_3D_RAYCAST:
        KON_DrawRaycast(layer);
        /* We don't draw the display list here because the raycast renderer will do it for us (as 3D sprites) */
        break;

    default:
        KON_DrawDisplayListAt(layer->displayList, layer->pos.x, layer->pos.y);
        break;
    }
}