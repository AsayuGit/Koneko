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

void KON_DrawWallStripe(unsigned int screenX, unsigned int midScreen, double wallDistance, double sceneRelativeCameraHeight, TileMap* tileMapLayer, unsigned int currentTile, unsigned int wallScanline) {
    KON_Rect tileRect, wall;

    unsigned int wallSize;

    #ifndef KON_RAYCAST_NO_WALLS
        wallSize = (midScreen / wallDistance) * 2;

        wall.x = screenX;
        wall.y = midScreen - wallSize * (1.0 - sceneRelativeCameraHeight);
        wall.w = 1;
        wall.h = wallSize;

        KON_GetTileSrcRectInTileMap(tileMapLayer, currentTile, &tileRect);
        tileRect.x += wallScanline;
        tileRect.w = 1;

        KON_DrawScaledSurfaceRect(tileMapLayer->tileSet, &tileRect, &wall);
    #endif
}

void KON_DrawFloorStripe(double wallDistance, int screenX, unsigned int midScreen, double layerRelativeCameraHeight, double sceneRelativeCameraHeight, Vector2d* rayDirection, TileMap* tileMapLayer) {
    Vector2d floorCoord;
    unsigned int screenY;
    double floorDistance;
    
    screenY = ((2.0 * midScreen * sceneRelativeCameraHeight) / wallDistance) + midScreen;
    for (; screenY < Koneko.dDevice.InternalResolution.y; screenY++) {
        floorDistance = (2.0 * (double)midScreen * sceneRelativeCameraHeight) / ((double)screenY - (double)midScreen);

        floorCoord.x = Koneko.dDevice.camera.position.x + rayDirection->x * floorDistance;
        floorCoord.y = Koneko.dDevice.camera.position.y + rayDirection->y * floorDistance;

        if (!KON_IsTileMapTileSolid(tileMapLayer, floorCoord.x, floorCoord.y, (int)layerRelativeCameraHeight, NULL) && KON_IsTileMapTileSolid(tileMapLayer, floorCoord.x, floorCoord.y, (int)layerRelativeCameraHeight - 1, NULL)) {
            if (((int)floorCoord.x + (int)floorCoord.y) % 2)
                SDL_SetRenderDrawColor(Koneko.dDevice.Renderer, 0, 0, 255, 255);
            else
                SDL_SetRenderDrawColor(Koneko.dDevice.Renderer, 0, 255, 0, 255);

            SDL_RenderDrawPoint(Koneko.dDevice.Renderer, screenX, screenY);
        }
    }
    SDL_SetRenderDrawColor(Koneko.dDevice.Renderer, 0, 0, 0, 255);
}

double KON_DDAStep(double minRayLength, int screenX, unsigned int midScreen, unsigned int* wallScanline, Vector2d* rayDirection, Vector2i* ray, Vector2i* rayStep, Vector2d* rayMarch, Vector2d* mapPosition, TileMap* tileMapLayer, unsigned int* currentTile, double layerRelativeCameraHeight) {
    Vector2d rayLength;
    Vector2d rayInterpol;
    Vector2d rayCheck;
    
    unsigned int DDADepth;
    double wallDistance;
    bool inWall = true;
    /* Compute */
    for (DDADepth = 0; DDADepth < 100; DDADepth++) {
        rayInterpol.x = rayStep->x * fabs((double)ray->y - mapPosition->y) * rayMarch->x;
        rayInterpol.y = rayStep->y * fabs((double)ray->x - mapPosition->x) * rayMarch->y;

        /* 0. Compute the length of each rays */
        rayLength.x = sqrt(pow(ray->x - mapPosition->x, 2) + pow(rayInterpol.y, 2));
        rayLength.y = sqrt(pow(ray->y - mapPosition->y, 2) + pow(rayInterpol.x, 2));

        if (rayLength.x < rayLength.y) {
            /* Iterate on the X direction */

            rayCheck.x = ray->x;
            rayCheck.y = rayInterpol.y + mapPosition->y;

            if (rayStep->x < 0)
                rayCheck.x--;

            /*
                We want |ray| * cos(alpha)
                Or : cameraDirection . ray = |cameraDirection| * |ray| * cos(alpha)
                So : cameraDirection . ray / |cameraDirection| = |ray| * cos(alpha)
                And since : |cameraDirection| = 1
                Then : |ray| * cos(alpha) = cameraDirection . ray
            */
            /* Fish-eye correction : Return the perpendicular distance to the camera instead of the euclidian one */
            wallDistance = (Koneko.dDevice.camera.direction.x * (ray->x - mapPosition->x) + Koneko.dDevice.camera.direction.y * rayInterpol.y);
            ray->x += rayStep->x;

            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x, rayCheck.y, layerRelativeCameraHeight, currentTile)) {
                if (!inWall) {
                    *wallScanline = fabs(floor(rayCheck.y) - (rayCheck.y)) * tileMapLayer->TileSize;
                    if (rayStep->x < 0)
                        *wallScanline = tileMapLayer->TileSize - *wallScanline - 1;
                    break;
                }
            } else {
                inWall = false;
            }
        } else {
            /* Iterate on the Y direction */

            rayCheck.x = rayInterpol.x + mapPosition->x;
            rayCheck.y = ray->y;

            if (rayStep->y < 0)
                rayCheck.y--;

            wallDistance = (Koneko.dDevice.camera.direction.x * rayInterpol.x + Koneko.dDevice.camera.direction.y * (ray->y - mapPosition->y));
            ray->y += rayStep->y;
            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x, rayCheck.y, layerRelativeCameraHeight, currentTile)) {
                if (!inWall) {
                    *wallScanline = fabs(floor(rayCheck.x) - (rayCheck.x)) * tileMapLayer->TileSize;
                    if (rayStep->y > 0)
                        *wallScanline = tileMapLayer->TileSize - *wallScanline - 1;
                    break;
                }
            } else {
                inWall = false;
            }
        }
    }

    return wallDistance;
}

void KON_DDASetup(Vector2d* rayDirection, Vector2d* mapPosition, Vector2i* ray, Vector2i* rayStep, Vector2d* rayMarch) {

    if (rayDirection->x > 0) {
        ray->x = (int)mapPosition->x + 1;
        rayStep->x = 1;
    } else {
        ray->x = (int)mapPosition->x;
        rayStep->x = -1;
    }

    if (rayDirection->y > 0) {
        ray->y = (int)mapPosition->y + 1;
        rayStep->y = 1;
    } else {
        ray->y = (int)mapPosition->y;
        rayStep->y = -1;
    }

    rayMarch->x = fabs(rayDirection->x / rayDirection->y); /* y * thix = x */
    rayMarch->y = fabs(rayDirection->y / rayDirection->x); /* x * this = y */
}

double KON_CastRecursiveRay(double wallDistance, double minRayLength, int screenX, unsigned int midScreen, Vector2d* rayDirection, Vector2d* mapPosition, Vector2i* ray, Vector2i* rayStep, Vector2d* rayMarch, TileMap* tileMapLayer, double layerRelativeCameraHeight, double sceneRelativeCameraHeight) {
    unsigned int wallScanline, currentTile;
    
    if (wallDistance >= minRayLength) {
        if (Koneko.dDevice.camera.cameraHeight > layerRelativeCameraHeight)
            KON_DrawFloorStripe(wallDistance, screenX, midScreen, layerRelativeCameraHeight, sceneRelativeCameraHeight, rayDirection, tileMapLayer);
        return 0;
    }
    wallDistance = KON_DDAStep(minRayLength, screenX, midScreen, &wallScanline, rayDirection, ray, rayStep, rayMarch, mapPosition, tileMapLayer, &currentTile, layerRelativeCameraHeight);
    KON_CastRecursiveRay(wallDistance, minRayLength, screenX, midScreen, rayDirection, mapPosition, ray, rayStep, rayMarch, tileMapLayer, layerRelativeCameraHeight, sceneRelativeCameraHeight);
    KON_DrawWallStripe(screenX, midScreen, wallDistance, sceneRelativeCameraHeight, tileMapLayer, currentTile, wallScanline);

    return wallDistance;
}

void KON_DrawRaycast(MapLayer* layer) {
    unsigned int wallScanline;
    int screenX;
    double progress;

    Vector2d rayDirection;
    Vector2d mapPosition;

    double wallDistance;
    unsigned int midScreen, currentTile, layerHeight;

    double layerRelativeCameraHeight, sceneRelativeCameraHeight;

    TileMap* tileMapLayer = (TileMap*)layer->layerData;

    Vector2i ray; /* The end of ray being casted AKA the ray from the 0,0 to itself.
                    X: The ray displacement in the X direction
                    Y: The ray displacement in the Y direction
                    We need to interpolate in the other direction to get the resulting vector */

    Vector2i rayStep; /* X: The way the ray step forward in the X axis.
                        Y: The way the ray step forward in the Y axis. */

    Vector2d rayMarch; /* X : For each displacement of 1 in the Y axis, by how much to go in the X axis.
                        Y : For each displacement of 1 in the X axis, by how much to go in the Y axis. */

    midScreen = Koneko.dDevice.InternalResolution.y >> 1;

    /*printf("Camera pos %f %f\n", Koneko.dDevice.camera.position.x, Koneko.dDevice.camera.position.y);*/
    /* DDA Loop */
    for (screenX = 0; screenX < Koneko.dDevice.InternalResolution.x; screenX++) {
        progress = (((double)screenX * 2 / (double)Koneko.dDevice.InternalResolution.x) - 1);

        rayDirection.x = Koneko.dDevice.camera.direction.x + progress * Koneko.dDevice.camera.plane.x;
        rayDirection.y = Koneko.dDevice.camera.direction.y + progress * Koneko.dDevice.camera.plane.y;

        mapPosition.x = Koneko.dDevice.camera.position.x;
        mapPosition.y = Koneko.dDevice.camera.position.y;

        /* Initial run for the layer we're in */
        KON_DDASetup(&rayDirection, &mapPosition, &ray, &rayStep, &rayMarch);
        wallDistance = KON_DDAStep(0, screenX, midScreen, &wallScanline, &rayDirection, &ray, &rayStep, &rayMarch, &mapPosition, tileMapLayer, &currentTile, (int)Koneko.dDevice.camera.cameraHeight);

        /* Draw every layer below */
        for (layerHeight = 0; layerHeight < (int)Koneko.dDevice.camera.cameraHeight; layerHeight++) {
            layerRelativeCameraHeight = layerHeight + (Koneko.dDevice.camera.cameraHeight - (int)Koneko.dDevice.camera.cameraHeight);
            sceneRelativeCameraHeight = Koneko.dDevice.camera.cameraHeight - layerHeight;

            KON_DDASetup(&rayDirection, &mapPosition, &ray, &rayStep, &rayMarch);
            KON_CastRecursiveRay(-1, wallDistance, screenX, midScreen, &rayDirection, &mapPosition, &ray, &rayStep, &rayMarch, tileMapLayer, layerRelativeCameraHeight, sceneRelativeCameraHeight);
        }

        /* Draw every layer above */
        for (layerHeight = tileMapLayer->MapSizeZ - 1; layerHeight > (int)Koneko.dDevice.camera.cameraHeight; layerHeight--) {
            layerRelativeCameraHeight = layerHeight + (Koneko.dDevice.camera.cameraHeight - (int)Koneko.dDevice.camera.cameraHeight);
            sceneRelativeCameraHeight = Koneko.dDevice.camera.cameraHeight - layerHeight;

            KON_DDASetup(&rayDirection, &mapPosition, &ray, &rayStep, &rayMarch);
            KON_CastRecursiveRay(-1, wallDistance, screenX, midScreen, &rayDirection, &mapPosition, &ray, &rayStep, &rayMarch, tileMapLayer, layerRelativeCameraHeight, sceneRelativeCameraHeight);
        }

        sceneRelativeCameraHeight = Koneko.dDevice.camera.cameraHeight - (int)Koneko.dDevice.camera.cameraHeight;
        KON_DrawWallStripe(screenX, midScreen, wallDistance, sceneRelativeCameraHeight, tileMapLayer, currentTile, wallScanline);

        KON_DrawFloorStripe(wallDistance, screenX, midScreen, Koneko.dDevice.camera.cameraHeight, sceneRelativeCameraHeight, &rayDirection, tileMapLayer);
    }
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