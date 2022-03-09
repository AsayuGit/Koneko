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
#include "System.h"
#include "TileMap.h"
#include "TileMap.h"
#include "math.h"

#include "API.h"

/* FIXME: Implement DDA algorithm */
double KON_CastRayOnTileMap(MapLayer* layer, Vector2d* position, unsigned int Z, Vector2d* rayDirection, unsigned int* tile, unsigned int* tileScanline) {
    unsigned int DDADepth;
    Vector2i ray; /* The end of ray being casted AKA the ray from the 0,0 to itself.
                     X: The ray displacement in the X direction
                     Y: The ray displacement in the Y direction
                     We need to interpolate in the other direction to get the resulting vector */

    Vector2i rayStep; /* X: The way the ray step forward in the X axis.
                         Y: The way the ray step forward in the Y axis. */

    Vector2d rayMarch; /* X : For each displacement of 1 in the Y axis, by how much to go in the X axis.
                         Y : For each displacement of 1 in the X axis, by how much to go in the Y axis. */

    Vector2d rayLength;
    Vector2d rayInterpol;
    Vector2d rayCheck;

    TileMap* tileMapLayer = (TileMap*)layer->layerData;

    if (rayDirection->x > 0) {
        ray.x = (int)position->x + 1;
        rayStep.x = 1;
    } else {
        ray.x = (int)position->x;
        rayStep.x = -1;
    }

    if (rayDirection->y > 0) {
        ray.y = (int)position->y + 1;
        rayStep.y = 1;
    } else {
        ray.y = (int)position->y;
        rayStep.y = -1;
    }

    rayMarch.x = fabs(rayDirection->x / rayDirection->y); /* y * thix = x */
    rayMarch.y = fabs(rayDirection->y / rayDirection->x); /* x * this = y */

    /* DDA Loop */
    for (DDADepth = 0; DDADepth < 100; DDADepth++) {

        rayInterpol.x = rayStep.x * fabs((double)ray.y - position->y) * rayMarch.x;
        rayInterpol.y = rayStep.y * fabs((double)ray.x - position->x) * rayMarch.y;

        /* 0. Compute the length of each rays */
        rayLength.x = sqrt(pow(ray.x - position->x, 2) + pow(rayInterpol.y, 2));
        rayLength.y = sqrt(pow(ray.y - position->y, 2) + pow(rayInterpol.x, 2));

        if (rayLength.x < rayLength.y) {
            /* Iterate on the X direction */

            rayCheck.x = ray.x;
            rayCheck.y = rayInterpol.y + position->y;

            if (rayStep.x < 0)
                rayCheck.x--;
            /* FIXME: Check all layers ? */
            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x, rayCheck.y, 0, tile)) {
                if (tileScanline) {
                    *tileScanline = fabs(floor(rayCheck.y) - (rayCheck.y)) * tileMapLayer->TileSize;
                    if (rayStep.x < 0)
                        *tileScanline = tileMapLayer->TileSize - *tileScanline - 1;
                }
                /* Fish-eye correction : Return the perpendicular distance to the camera instead of the euclidian one */
                
                /*
                    We want |ray| * cos(alpha)
                    Or : cameraDirection . ray = |cameraDirection| * |ray| * cos(alpha)
                    So : cameraDirection . ray / |cameraDirection| = |ray| * cos(alpha)
                    And since : |cameraDirection| = 1
                    Then : |ray| * cos(alpha) = cameraDirection . ray
                */
                return (Koneko.dDevice.camera.direction.x * (ray.x - position->x) + Koneko.dDevice.camera.direction.y * rayInterpol.y);
            }
            ray.x += rayStep.x;
        } else {
            /* Iterate on the Y direction */

            rayCheck.x = rayInterpol.x + position->x;
            rayCheck.y = ray.y;

            if (rayStep.y < 0)
                rayCheck.y--;
            /* FIXME: Check all layers ? */
            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x, rayCheck.y, Z, tile)) {
                if (tileScanline) {
                    *tileScanline = fabs(floor(rayCheck.x) - (rayCheck.x)) * tileMapLayer->TileSize;
                    if (rayStep.y > 0)
                        *tileScanline = tileMapLayer->TileSize - *tileScanline - 1;
                }
                return (Koneko.dDevice.camera.direction.x * rayInterpol.x + Koneko.dDevice.camera.direction.y * (ray.y - position->y));
            }
            ray.y += rayStep.y;
        }
    }

    return 0;
}

void KON_DrawWallLine(MapLayer* layer, unsigned int screenX, double length, unsigned int tile, unsigned int scanline) {
    int halfHeight;
    unsigned int wallSize;
    KON_Rect tileRect, wall;
    TileMap* tileMap;

    /* FIXME: We should check if its a tilemap first */
    tileMap = (TileMap*)layer->layerData;
    halfHeight = Koneko.dDevice.InternalResolution.y >> 1;
    
    if (length <= 0.0)
        length = 1.0;
    
    wallSize = (halfHeight / length) * 2;

    wall.x = screenX;
    wall.y = halfHeight - wallSize * Koneko.dDevice.camera.cameraHeight;
    wall.w = 1;
    wall.h = wallSize;

    KON_GetTileSrcRectInTileMap(tileMap, tile, &tileRect);
    tileRect.x += scanline;
    tileRect.w = 1;

    KON_DrawScaledSurfaceRect(tileMap->tileSet, &tileRect, &wall);
}

void KON_DrawRaycast(MapLayer* layer) {
    unsigned int currentTile, wallScanline;
    int screenX;
    double rayLength;
    double progress;

    Vector2d rayDirection;
    Vector2d mapPosition;

    double camHeight = Koneko.dDevice.camera.cameraHeight;

    for (screenX = 0; screenX < Koneko.dDevice.InternalResolution.x; screenX++) {
        progress = (((double)screenX * 2 / (double)Koneko.dDevice.InternalResolution.x) - 1);
        rayDirection.x = Koneko.dDevice.camera.direction.x + progress * Koneko.dDevice.camera.plane.x;
        rayDirection.y = Koneko.dDevice.camera.direction.y + progress * Koneko.dDevice.camera.plane.y;
        
        mapPosition.x = Koneko.dDevice.camera.position.x;
        mapPosition.y = Koneko.dDevice.camera.position.y;

        Koneko.dDevice.camera.cameraHeight = camHeight;
        rayLength = KON_CastRayOnTileMap(layer, &mapPosition, 0, &rayDirection, &currentTile, &wallScanline);
        KON_DrawWallLine(layer, screenX, rayLength, currentTile, wallScanline);
        Koneko.dDevice.camera.cameraHeight++;
        rayLength = KON_CastRayOnTileMap(layer, &mapPosition, 1, &rayDirection, &currentTile, &wallScanline);
        KON_DrawWallLine(layer, screenX, rayLength, currentTile, wallScanline);
    }

    Koneko.dDevice.camera.cameraHeight = camHeight;
}

void KON_RenderLayer(MapLayer* layer) {
    switch (layer->layerRenderer)
    {
    case RENDER_2D_TILEMAP:
        KON_DrawTileMap(layer);
        break;

    case RENDER_2D_BITMAP:
        KON_DrawBitMap(layer);
        break;

    case RENDER_3D_RAYCAST:
        KON_DrawRaycast(layer);
        break;

    default:
        break;
    }
}