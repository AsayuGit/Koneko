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

/*
#define KON_RAYCAST_NO_FLOORS
#define KON_RAYCAST_NO_CEILING
*/

void KON_DrawWallStripe(unsigned int screenX, unsigned int midScreen, double wallDistance, double sceneRelativeCameraHeight, MapLayer* layer, unsigned int currentTile, unsigned int wallScanline) {
    KON_Rect tileRect;
    int wallStart, wallEnd, wallSize, wallTextureLine;
    unsigned int i;

    #ifndef KON_RAYCAST_NO_WALLS
        wallSize = ((double)midScreen / wallDistance) * 2.0;
        wallStart = midScreen - wallSize * (1.0 - sceneRelativeCameraHeight);

        wallEnd = wallStart + wallSize;

        KON_GetTileSrcRectInTileMap(layer->layerData, currentTile, &tileRect);
        wallScanline += tileRect.x;

        for (i = 0; i < wallSize; i++) {
            wallTextureLine = ((TileMap*)layer->layerData)->TileSize * (i / (double)wallSize) + tileRect.y;
            
            if ((wallStart >= 0) && (wallStart < Koneko.dDevice.InternalResolution.y))
                layer->effectBuffer[wallStart * Koneko.dDevice.InternalResolution.x + screenX] = layer->texture.cpuSide.pixelData[wallTextureLine * layer->texture.cpuSide.width + wallScanline];
            wallStart++;
        }
    #endif
}

void KON_DrawFloorStripe(double wallDistance, int screenX, unsigned int midScreen, unsigned int layerRelativeCameraHeight, double sceneRelativeCameraHeight, Vector2d* rayDirection, MapLayer* layer) {
    Vector2d floorCoord;
    Vector2i textCoord;
    KON_Rect tileRect;
    unsigned int screenY, currentTile, tileHeight;
    double floorDistance;
    
    #ifndef KON_RAYCAST_NO_FLOORS
        screenY = ((2 * midScreen * sceneRelativeCameraHeight) / wallDistance) + midScreen;
        for (; screenY < Koneko.dDevice.InternalResolution.y; screenY++) {

            floorDistance = (2 * midScreen * sceneRelativeCameraHeight) / (screenY - midScreen);

            floorCoord.x = Koneko.dDevice.camera.position.x + rayDirection->x * floorDistance;
            floorCoord.y = Koneko.dDevice.camera.position.y + rayDirection->y * floorDistance;

            if (!KON_IsTileMapTileSolid(layer->layerData, floorCoord.x, floorCoord.y, layerRelativeCameraHeight, NULL) && KON_IsTileMapTileSolid(layer->layerData, floorCoord.x, floorCoord.y, layerRelativeCameraHeight - 1, NULL)) {
                tileHeight = (layerRelativeCameraHeight > 0) ? layerRelativeCameraHeight - 1 : layerRelativeCameraHeight;

                KON_GetTile((TileMap*)layer->layerData, floorCoord.x, floorCoord.y, tileHeight, &currentTile);
                KON_GetTileSrcRectInTileMap(layer->layerData, currentTile, &tileRect);
                textCoord.x = tileRect.x + ((floorCoord.x - (int)floorCoord.x) * ((TileMap*)layer->layerData)->TileSize);
                textCoord.y = tileRect.y + ((floorCoord.y - (int)floorCoord.y) * ((TileMap*)layer->layerData)->TileSize);

                layer->effectBuffer[screenY * Koneko.dDevice.InternalResolution.x + screenX] = layer->texture.cpuSide.pixelData[textCoord.y * layer->texture.cpuSide.width + textCoord.x];
            }
        }
    #endif
}

void KON_DrawCeilStripe(double wallDistance, int screenX, unsigned int midScreen, unsigned int layerRelativeCameraHeight, double sceneRelativeCameraHeight, Vector2d* rayDirection, MapLayer* layer) {
    Vector2d ceilCoord;
    Vector2i textCoord;
    KON_Rect tileRect;
    unsigned int currentTile, tileHeight;
    int screenY;
    double ceilDistance;
    
    #ifndef KON_RAYCAST_NO_CEILING
        screenY = midScreen - ((2 * midScreen * (1 - sceneRelativeCameraHeight)) / wallDistance);
        for (; screenY >= 0; screenY--) {

            ceilDistance = (2 * midScreen * (1 - sceneRelativeCameraHeight)) / (midScreen - screenY);

            ceilCoord.x = Koneko.dDevice.camera.position.x + rayDirection->x * ceilDistance;
            ceilCoord.y = Koneko.dDevice.camera.position.y + rayDirection->y * ceilDistance;

            if (!KON_IsTileMapTileSolid(layer->layerData, ceilCoord.x, ceilCoord.y, layerRelativeCameraHeight, NULL) && KON_IsTileMapTileSolid(layer->layerData, ceilCoord.x, ceilCoord.y, layerRelativeCameraHeight + 1, NULL)) {
                tileHeight = (layerRelativeCameraHeight < (((TileMap*)layer->layerData)->MapSizeZ - 1)) ? layerRelativeCameraHeight + 1 : layerRelativeCameraHeight;

                KON_GetTile((TileMap*)layer->layerData, ceilCoord.x, ceilCoord.y, tileHeight, &currentTile);
                KON_GetTileSrcRectInTileMap(layer->layerData, currentTile, &tileRect);
                textCoord.x = tileRect.x + ((ceilCoord.x - (int)ceilCoord.x) * ((TileMap*)layer->layerData)->TileSize);
                textCoord.y = tileRect.y + ((ceilCoord.y - (int)ceilCoord.y) * ((TileMap*)layer->layerData)->TileSize);

                layer->effectBuffer[screenY * Koneko.dDevice.InternalResolution.x + screenX] = layer->texture.cpuSide.pixelData[textCoord.y * layer->texture.cpuSide.width + textCoord.x];
            }
        }
    #endif
}

double KON_DDAStep(double minRayLength, int screenX, unsigned int midScreen, unsigned int* wallScanline, Vector2d* rayDirection, Vector2i* ray, Vector2i* rayStep, Vector2d* rayMarch, Vector2d* mapPosition, MapLayer* layer, unsigned int* currentTile, unsigned int layerRelativeCameraHeight) {
    Vector2d rayLength;
    Vector2d rayInterpol;
    Vector2d rayCheck;
    Vector2d rayRelativeMapPos;
    
    unsigned int DDADepth;
    double wallDistance;
    bool inWall = KON_IsTileMapTileSolid(layer->layerData, mapPosition->x, mapPosition->y, layerRelativeCameraHeight, currentTile);

    /* Ensure the data going out are initialised */
    *wallScanline = 0;
    *currentTile = 0;

    /* Compute */
    for (DDADepth = 0; DDADepth < 100; DDADepth++) {
        rayRelativeMapPos.x = ray->x - mapPosition->x;
        rayRelativeMapPos.y = ray->y - mapPosition->y;

        rayInterpol.x = rayStep->x * fabs(rayRelativeMapPos.y) * rayMarch->x;
        rayInterpol.y = rayStep->y * fabs(rayRelativeMapPos.x) * rayMarch->y;

        /* 0. Compute the length of each rays */
        rayLength.x = sqrt(pow(rayRelativeMapPos.x, 2) + pow(rayInterpol.y, 2));
        rayLength.y = sqrt(pow(rayRelativeMapPos.y, 2) + pow(rayInterpol.x, 2));

        if (rayLength.x < rayLength.y) {
            /* Iterate on the X direction */

            rayCheck.x = ray->x - ((*(unsigned int*)&rayStep->x) >> 31);
            rayCheck.y = rayInterpol.y + mapPosition->y;

            /*
                We want |ray| * cos(alpha)
                Or : cameraDirection . ray = |cameraDirection| * |ray| * cos(alpha)
                So : cameraDirection . ray / |cameraDirection| = |ray| * cos(alpha)
                And since : |cameraDirection| = 1
                Then : |ray| * cos(alpha) = cameraDirection . ray
            */
            /* Fish-eye correction : Return the perpendicular distance to the camera instead of the euclidian one */
            wallDistance = (Koneko.dDevice.camera.direction.x * rayRelativeMapPos.x + Koneko.dDevice.camera.direction.y * rayInterpol.y);
            ray->x += rayStep->x;

            if (KON_IsTileMapTileSolid(layer->layerData, rayCheck.x, rayCheck.y, layerRelativeCameraHeight, currentTile)) {
                if (!inWall) {
                    *wallScanline = fabs(floor(rayCheck.y) - (rayCheck.y)) * ((TileMap*)layer->layerData)->TileSize;
                    if (rayStep->x < 0)
                        *wallScanline = ((TileMap*)layer->layerData)->TileSize - *wallScanline - 1;
                    break;
                }
            } else {
                inWall = false;
            }
        } else {
            /* Iterate on the Y direction */

            rayCheck.x = rayInterpol.x + mapPosition->x;
            rayCheck.y = ray->y - ((*(unsigned int*)&rayStep->y) >> 31);

            wallDistance = (Koneko.dDevice.camera.direction.x * rayInterpol.x + Koneko.dDevice.camera.direction.y * rayRelativeMapPos.y);
            ray->y += rayStep->y;
            if (KON_IsTileMapTileSolid(((TileMap*)layer->layerData), rayCheck.x, rayCheck.y, layerRelativeCameraHeight, currentTile)) {
                if (!inWall) {
                    *wallScanline = fabs(floor(rayCheck.x) - (rayCheck.x)) * ((TileMap*)layer->layerData)->TileSize;
                    if (rayStep->y > 0)
                        *wallScanline = ((TileMap*)layer->layerData)->TileSize - *wallScanline - 1;
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
    rayStep->x = (rayDirection->x > 0) ? 1 : -1;
    rayStep->y = (rayDirection->y > 0) ? 1 : -1;

    ray->x = mapPosition->x + !((*(unsigned long*)&rayDirection->x) >> 63);
    ray->y = mapPosition->y + !((*(unsigned long*)&rayDirection->y) >> 63);
    
    rayMarch->x = fabs(rayDirection->x / rayDirection->y); /* y * thix = x */
    rayMarch->y = fabs(rayDirection->y / rayDirection->x); /* x * this = y */
}

#define KON_ResetRay(rayDirection, mapPosition, ray) do {                \
    ray.x = mapPosition.x + !((*(unsigned long*)&rayDirection.x) >> 63); \
    ray.y = mapPosition.y + !((*(unsigned long*)&rayDirection.y) >> 63); \
} while (0)                                                              \

/* TODO double recusion (among the layer and all layers) */
double KON_CastRecursiveRay(double wallDistance, double minRayLength, int screenX, unsigned int midScreen, Vector2d* rayDirection, Vector2d* mapPosition, Vector2i* ray, Vector2i* rayStep, Vector2d* rayMarch, MapLayer* layer, unsigned int layerRelativeCameraHeight, double sceneRelativeCameraHeight) {
    unsigned int wallScanline, currentTile;
    double maxWallDistance;
    
    if (wallDistance >= minRayLength) {
        /* HERE ! */
        if (Koneko.dDevice.camera.cameraHeight > layerRelativeCameraHeight) {/* Bottom Layer */
            if (layerRelativeCameraHeight >= 0) {
                /*printf("L = %u\n", layerRelativeCameraHeight);*/
                KON_ResetRay((*rayDirection), (*mapPosition), (*ray));
                KON_CastRecursiveRay( -1, wallDistance, screenX, midScreen, rayDirection, mapPosition, ray, rayStep, rayMarch, layer, layerRelativeCameraHeight - 1, sceneRelativeCameraHeight + 1);
                KON_DrawFloorStripe(wallDistance, screenX, midScreen, layerRelativeCameraHeight, sceneRelativeCameraHeight, rayDirection, layer);
            }
        } else if (Koneko.dDevice.camera.cameraHeight < layerRelativeCameraHeight) {/* Top layer */
            if (layerRelativeCameraHeight < ((TileMap*)layer->layerData)->MapSizeZ) {
                KON_ResetRay((*rayDirection), (*mapPosition), (*ray));
                KON_CastRecursiveRay( -1, wallDistance, screenX, midScreen, rayDirection, mapPosition, ray, rayStep, rayMarch, layer, layerRelativeCameraHeight + 1, sceneRelativeCameraHeight - 1);
                KON_DrawCeilStripe(wallDistance, screenX, midScreen, layerRelativeCameraHeight, sceneRelativeCameraHeight, rayDirection, layer);
            }
        }
        return wallDistance;
    }
    wallDistance = KON_DDAStep(minRayLength, screenX, midScreen, &wallScanline, rayDirection, ray, rayStep, rayMarch, mapPosition, layer, &currentTile, layerRelativeCameraHeight);
    maxWallDistance = KON_CastRecursiveRay( wallDistance, minRayLength, screenX, midScreen, rayDirection, mapPosition, ray, rayStep, rayMarch, layer, layerRelativeCameraHeight, sceneRelativeCameraHeight);
    KON_DrawWallStripe(screenX, midScreen, wallDistance, sceneRelativeCameraHeight, layer, currentTile, wallScanline);

    return maxWallDistance;
}

void KON_DrawRaycast(MapLayer* layer) {

    Vector2i ray; /* The end of ray being casted AKA the ray from the 0,0 to itself.
                    X: The ray displacement in the X direction
                    Y: The ray displacement in the Y direction
                    We need to interpolate in the other direction to get the resulting vector */

    Vector2i rayStep; /* X: The way the ray step forward in the X axis.
                        Y: The way the ray step forward in the Y axis. */

    Vector2d rayMarch; /* X : For each displacement of 1 in the Y axis, by how much to go in the X axis.
                        Y : For each displacement of 1 in the X axis, by how much to go in the Y axis. */

    Vector2d rayDirection;
    Vector2d mapPosition;

    unsigned int wallScanline, midScreen, currentTile;
    double progress, wallDistance, sceneRelativeCameraHeight;
    int screenX;

    /*memset(layer->effectBuffer, 0x00, Koneko.dDevice.InternalResolution.x * Koneko.dDevice.InternalResolution.y * sizeof(uint32_t));*/

    midScreen = Koneko.dDevice.InternalResolution.y >> 1;

    /* DDA Loop */
    for (screenX = 0; screenX < Koneko.dDevice.InternalResolution.x; screenX++) {
        progress = (((double)screenX * 2 / (double)Koneko.dDevice.InternalResolution.x) - 1);

        rayDirection.x = Koneko.dDevice.camera.direction.x + progress * Koneko.dDevice.camera.plane.x;
        rayDirection.y = Koneko.dDevice.camera.direction.y + progress * Koneko.dDevice.camera.plane.y;

        mapPosition.x = Koneko.dDevice.camera.position.x;
        mapPosition.y = Koneko.dDevice.camera.position.y;

        /* Initial run for the layer we're in */
        KON_DDASetup(&rayDirection, &mapPosition, &ray, &rayStep, &rayMarch);
        wallDistance = KON_DDAStep(0, screenX, midScreen, &wallScanline, &rayDirection, &ray, &rayStep, &rayMarch, &mapPosition, layer, &currentTile, Koneko.dDevice.camera.cameraHeight);
        
        sceneRelativeCameraHeight = Koneko.dDevice.camera.cameraHeight - (int)Koneko.dDevice.camera.cameraHeight;

        /* Draw every layer below */
        if ((int)Koneko.dDevice.camera.cameraHeight) {
            KON_ResetRay(rayDirection, mapPosition, ray);
            KON_CastRecursiveRay(-1, wallDistance, screenX, midScreen, &rayDirection, &mapPosition, &ray, &rayStep, &rayMarch, layer, Koneko.dDevice.camera.cameraHeight - 1, sceneRelativeCameraHeight + 1);
        }

        /* Draw every layer above */
        if ((int)Koneko.dDevice.camera.cameraHeight != ((TileMap*)layer->layerData)->MapSizeZ) {
            KON_ResetRay(rayDirection, mapPosition, ray);
            KON_CastRecursiveRay(-1, wallDistance, screenX, midScreen, &rayDirection, &mapPosition, &ray, &rayStep, &rayMarch, layer, Koneko.dDevice.camera.cameraHeight + 1, sceneRelativeCameraHeight - 1);
        }

        /* Draw Main lauer*/
        KON_DrawWallStripe(screenX, midScreen, wallDistance, sceneRelativeCameraHeight, layer, currentTile, wallScanline);
        KON_DrawCeilStripe(wallDistance, screenX, midScreen, Koneko.dDevice.camera.cameraHeight, sceneRelativeCameraHeight, &rayDirection, layer);
        KON_DrawFloorStripe(wallDistance, screenX, midScreen, Koneko.dDevice.camera.cameraHeight, sceneRelativeCameraHeight, &rayDirection, layer);
    }

    SDL_UpdateTexture(layer->effectTexture, NULL, layer->effectBuffer, layer->effectBufferPitch);
    SDL_RenderCopy(Koneko.dDevice.Renderer, layer->effectTexture, NULL, (SDL_Rect*)&Koneko.dDevice.RenderRect);
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