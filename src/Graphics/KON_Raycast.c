#include "KON_Raycast.h"
#include "Koneko.h"
#include "System.h"
#include "math.h"
#include "TileMap.h"
#include "API.h"

/*
    SUMMARY: Draw a tile pixel on screen.
*/
void DrawTilePixel(MapLayer* layer, unsigned int* layerZMapData, unsigned int* tileZMapData, Vector2d* rayDirection, double pointDistance, int screenY, int screenX) {
    Vector2d tileCoord;
    Vector2i textCoord;
    KON_Rect tileRect;

    TileMap* tileMap = layer->layerData;
    
    register unsigned int tileID, offset, index;

    tileCoord.x = Koneko.dDevice.camera.position.x + rayDirection->x * pointDistance;
    tileCoord.y = Koneko.dDevice.camera.position.y + rayDirection->y * pointDistance;
    
    offset = ((int)tileCoord.y * tileMap->MapSizeX) + (int)tileCoord.x;

    /* If the tile is empty and the one below is solid */
    if (!tileMap->TileIndex[layerZMapData[offset]].isSolid) {
        tileID = tileZMapData[offset];
        if (layerZMapData == tileZMapData || tileMap->TileIndex[tileID].isSolid) {
            /* Get the texture coordinates */
            KON_GetTileSrcRectInTileMap((*(TileMap*)layer->layerData), tileID, tileRect);
            textCoord.x = tileRect.x + ((tileCoord.x - (int)tileCoord.x) * ((TileMap*)layer->layerData)->TileSize);
            textCoord.y = tileRect.y + ((tileCoord.y - (int)tileCoord.y) * ((TileMap*)layer->layerData)->TileSize);
        
            /* Draw the tile pixel on screen */
            index = screenY * Koneko.dDevice.InternalResolution.x + screenX;

            layer->effectBuffer[index] = layer->texture.cpuSide.pixelData[textCoord.y * layer->texture.cpuSide.width + textCoord.x];
            layer->zBuffer[index] = pointDistance;
        }
    }
}

/*
    SUMMARY: Draws textured floor from a wall at a "wallDistance" distance.
*/
void DrawFloorStripe(MapLayer* layer, unsigned int layerZ, Vector2d* rayDirection, int screenX, double wallDistance) {
    unsigned int screenY, tileZ, midScreen = Koneko.dDevice.InternalResolution.y >> 1;
    double floorDistance, cameraHeight = Koneko.dDevice.camera.cameraHeight - layerZ;

    register TileMap* tileMap = (TileMap*)layer->layerData;
    register unsigned int* layerZMapData;
    register unsigned int* tileZMapData;

    tileZ = (layerZ > 0) ? layerZ - 1 : 0;

    layerZMapData = tileMap->MapData + (layerZ * tileMap->MapDataLayerPitch);
    tileZMapData = tileMap->MapData + (tileZ * tileMap->MapDataLayerPitch);

    /* For each pixel down the screen from the wall */
    screenY = ((Koneko.dDevice.InternalResolution.y * cameraHeight) / wallDistance) + midScreen;
    if (screenY <= midScreen)
        screenY = midScreen + 1;
    for (; screenY < Koneko.dDevice.InternalResolution.y; screenY++) {
        /* Will be stored in a lookup array ahead of time */
        floorDistance = (Koneko.dDevice.InternalResolution.y * cameraHeight) / (screenY - midScreen);
        if (floorDistance <= wallDistance)
            DrawTilePixel(layer, layerZMapData, tileZMapData, rayDirection, floorDistance, screenY, screenX);
    }
}

/*
    SUMMARY: Draws textured ceiling from a wall at a "wallDistance" distance.
*/
void DrawCeilStripe(MapLayer* layer, unsigned int layerZ, Vector2d* rayDirection, int screenX, double wallDistance) {
    unsigned int tileZ, midScreen = Koneko.dDevice.InternalResolution.y >> 1;
    double ceilDistance, cameraHeight = Koneko.dDevice.camera.cameraHeight - layerZ;

    register TileMap* tileMap = (TileMap*)layer->layerData;
    register unsigned int* layerZMapData;
    register unsigned int* tileZMapData;

    int screenY = midScreen - ((Koneko.dDevice.InternalResolution.y * (1 - cameraHeight)) / wallDistance);
    tileZ = (layerZ < (((TileMap*)layer->layerData)->MapSizeZ - 1)) ? layerZ + 1 : layerZ;

    layerZMapData = tileMap->MapData + (layerZ * tileMap->MapDataLayerPitch);
    tileZMapData = tileMap->MapData + (tileZ * tileMap->MapDataLayerPitch);

    if (screenY >= midScreen)
        screenY = midScreen - 1;
    for (; screenY >= 0; screenY--) {
        ceilDistance = (double)Koneko.dDevice.InternalResolution.y * (1.0 - cameraHeight) / (double)(midScreen - screenY);
        if (ceilDistance <= wallDistance)
            DrawTilePixel(layer, layerZMapData, tileZMapData, rayDirection, ceilDistance, screenY, screenX);
    }
}

/*
    SUMMARY: Draws a wall stripe at the x coordinate the ray is casted from.
*/
void DrawWallStripe(MapLayer* layer, int screenX, unsigned int wallStripe, double cameraHeight, double wallDistance, unsigned int tileID) {
    KON_Rect tileRect;
    int wallStart, wallSize, wallTextureLine;
    unsigned int i, midScreen = Koneko.dDevice.InternalResolution.y >> 1;

    register unsigned int index;

    wallSize = ((double)midScreen / wallDistance) * 2.0;
    wallStart = midScreen - wallSize * (1.0 - cameraHeight);

    KON_GetTileSrcRectInTileMap((*(TileMap*)layer->layerData), tileID, tileRect);
    wallStripe += tileRect.x;

    for (i = 0; i < wallSize; i++) {
        wallTextureLine = ((TileMap*)layer->layerData)->TileSize * (i / (double)wallSize) + tileRect.y;

        if ((wallStart >= 0) && (wallStart < Koneko.dDevice.InternalResolution.y)) {
            index = wallStart * Koneko.dDevice.InternalResolution.x + screenX;

            layer->effectBuffer[index] = layer->texture.cpuSide.pixelData[wallTextureLine * layer->texture.cpuSide.width + wallStripe];
            layer->zBuffer[index] = wallDistance;
        }
        wallStart++;
    }
}

/*
    SUMMARY: Cast a ray on the current layer and return both the distance and the hit position
*/
void CastRay(MapLayer* layer, unsigned int layerZ, bool inWall, Vector2d* rayDirection, Vector2d* start, Vector2d* end, double* distance, unsigned int* tileID) {
    Vector2d rayLength;
    Vector2d rayInterpol;
    Vector2d rayCheck;
    Vector2d rayRelativeMapPos;

    Vector2i ray, rayStep;
    Vector2d rayMarch;

    TileMap* tileMap = (TileMap*)layer->layerData;
    unsigned int* tileMapLayer = tileMap->MapData + (layerZ * tileMap->MapDataLayerPitch);
    
    unsigned int DDADepth;
    double wallDistance;

    /* Ensure the data going out are initialised */
    *tileID = 0;

    /* Init ray */
    rayStep.x = (rayDirection->x > 0) ? 1 : -1;
    rayStep.y = (rayDirection->y > 0) ? 1 : -1;

    ray.x = start->x + !((*(unsigned long*)&rayDirection->x) >> 63);
    ray.y = start->y + !((*(unsigned long*)&rayDirection->y) >> 63);

    rayMarch.x = fabs(rayDirection->x / rayDirection->y); /* y * thix = x */
    rayMarch.y = fabs(rayDirection->y / rayDirection->x); /* x * this = y */

    /* Compute */
    for (DDADepth = 0; DDADepth < 100; DDADepth++) {
        rayRelativeMapPos.x = ray.x - start->x;
        rayRelativeMapPos.y = ray.y - start->y;

        rayInterpol.x = rayStep.x * fabs(rayRelativeMapPos.y) * rayMarch.x;
        rayInterpol.y = rayStep.y * fabs(rayRelativeMapPos.x) * rayMarch.y;

        /* 0. Compute the length of each rays */
        rayLength.x = sqrt(pow(rayRelativeMapPos.x, 2) + pow(rayInterpol.y, 2));
        rayLength.y = sqrt(pow(rayRelativeMapPos.y, 2) + pow(rayInterpol.x, 2));

        if (rayLength.x < rayLength.y) {
            /* Iterate on the X direction */

            rayCheck.x = ray.x - ((*(unsigned int*)&rayStep.x) >> 31);
            rayCheck.y = rayInterpol.y + start->y;

            end->x = ray.x;
            end->y = rayCheck.y;

            /*
                We want |ray| * cos(alpha)
                Or : cameraDirection . ray = |cameraDirection| * |ray| * cos(alpha)
                So : cameraDirection . ray / |cameraDirection| = |ray| * cos(alpha)
                And since : |cameraDirection| = 1
                Then : |ray| * cos(alpha) = cameraDirection . ray
            */
            /* Fish-eye correction : Return the perpendicular distance to the camera instead of the euclidian one */
            wallDistance = (Koneko.dDevice.camera.direction.x * rayRelativeMapPos.x + Koneko.dDevice.camera.direction.y * rayInterpol.y);
            /*wallDistance = rayLength.x;*/
            ray.x += rayStep.x;
        } else {
            /* Iterate on the Y direction */

            rayCheck.x = rayInterpol.x + start->x;
            rayCheck.y = ray.y - ((*(unsigned int*)&rayStep.y) >> 31);

            end->x = rayCheck.x;
            end->y = ray.y;

            wallDistance = (Koneko.dDevice.camera.direction.x * rayInterpol.x + Koneko.dDevice.camera.direction.y * rayRelativeMapPos.y);
            /*wallDistance = rayLength.y;*/
            ray.y += rayStep.y;
        }

        if (rayCheck.x < 0 || rayCheck.x >= tileMap->MapSizeX || rayCheck.y < 0 || rayCheck.y >= tileMap->MapSizeY) {
            *distance = -1;
            return;
        }
        
        *tileID = tileMapLayer[((int)rayCheck.y * tileMap->MapSizeX) + (int)rayCheck.x];
        if (tileMap->TileIndex[*tileID].isSolid) {
            if (!inWall) {
                *distance = wallDistance;
                return;
            }
        } else {
            inWall = false;
        }
    }
}

/*
    SUMMARY: Returns the stripe of the wall texture to draw depending on where the wall was hit by the ray.
*/
double GetWallStripe(TileMap* tileMap, Vector2d* rayDirection, Vector2d* rayHitPosition) {
    double delta;

    if((delta = rayHitPosition->x - (int)rayHitPosition->x)) {
        if (rayDirection->y > 0)
            delta = 1.0 - delta;
    } else {
        delta = (rayHitPosition->y - (int)rayHitPosition->y);
        if (rayDirection->x < 0)
            delta = 1.0 - delta;
    }

    return delta * tileMap->TileSize;
}

/* Forward declaration */
void CastRecursiveLayers(MapLayer* layer, int cameraLayer, Vector2d* rayDirection, int step, double minRayDistance, int screenX);
/*
    SUMMARY: cast a ray recursively until the minRayDistance is matched or exeded.
*/
void CastRecursiveRay(MapLayer* layer, int layerZ, bool inWall, Vector2d* rayDirection, Vector2d* start, int step, double minRayDistance, double accumulatedRayDistance, int screenX) {
    unsigned int tileID;
    Vector2d rayHitPosition;
    double rayDistance;

    register TileMap* tileMap = (TileMap*)layer->layerData;
    register unsigned int nextLayer;
    
    CastRay(layer, layerZ, inWall, rayDirection, start, &rayHitPosition, &rayDistance, &tileID);
    if (rayDistance < 0)
        accumulatedRayDistance = minRayDistance;
    else
        accumulatedRayDistance += rayDistance;

    if (accumulatedRayDistance < minRayDistance) {
        CastRecursiveRay(layer, layerZ, true, rayDirection, &rayHitPosition, step, minRayDistance, accumulatedRayDistance, screenX);
    } else {
        nextLayer = layerZ + step;
        if (nextLayer >= 0 && nextLayer < tileMap->MapSizeZ)
            CastRecursiveLayers(layer, nextLayer, rayDirection, step, accumulatedRayDistance, screenX);

        if (layerZ < Koneko.dDevice.camera.cameraHeight)
            DrawFloorStripe(layer, layerZ, rayDirection, screenX, accumulatedRayDistance);
        else
            DrawCeilStripe(layer, layerZ, rayDirection, screenX, accumulatedRayDistance);
    }

    if (accumulatedRayDistance >= 0)
        DrawWallStripe(layer, screenX, GetWallStripe(tileMap, rayDirection, &rayHitPosition), Koneko.dDevice.camera.cameraHeight - layerZ, accumulatedRayDistance, tileID);
}

/*
    SUMMARY: Cast all remaning layers in the stepping direction, ensuring visiblity to at least minRayDistance.
*/
void CastRecursiveLayers(MapLayer* layer, int cameraLayer, Vector2d* rayDirection, int step, double minRayDistance, int screenX) {
    register TileMap* tileMap = (TileMap*)layer->layerData;
    register bool inWall;

    /* Compute */
    if (Koneko.dDevice.camera.position.x > 0 && Koneko.dDevice.camera.position.x < tileMap->MapSizeX && Koneko.dDevice.camera.position.y > 0 && Koneko.dDevice.camera.position.y < tileMap->MapSizeY) {
        inWall = tileMap->TileIndex[tileMap->MapData[(cameraLayer * tileMap->MapDataLayerPitch) + ((int)Koneko.dDevice.camera.position.y * tileMap->MapSizeX) + (int)Koneko.dDevice.camera.position.x]].isSolid;
        /*inWall = KON_IsTileMapTileSolid(tileMap, Koneko.dDevice.camera.position.x, Koneko.dDevice.camera.position.y, cameraLayer, NULL);*/
        CastRecursiveRay(layer, cameraLayer, inWall, rayDirection, &Koneko.dDevice.camera.position, step, minRayDistance, 0, screenX);
    }
}

/*
    SUMMARY: Draws a raycasted stripe
*/
void DrawRaycastStripe(MapLayer* layer, int screenX) {
    TileMap* tileMap;
    double progress; /* The progress of the ray angle relative to the screen (-1 and 1) */
    Vector2d rayDirection; /* The ray vector (direction) */
    register bool inWall;

    Vector2d rayHitPosition;
    double rayDistance = 0;
    unsigned int hitTileID = 0;
    int cameraLayer;
    double cameraLayerRelativeHeight;

    tileMap = (TileMap*)layer->layerData;
    progress = (((double)screenX * 2 / (double)Koneko.dDevice.InternalResolution.x) - 1);

    rayDirection.x = Koneko.dDevice.camera.direction.x + progress * Koneko.dDevice.camera.plane.x;
    rayDirection.y = Koneko.dDevice.camera.direction.y + progress * Koneko.dDevice.camera.plane.y;

    /* Test if the camera starts in a wall */
    if (Koneko.dDevice.camera.cameraHeight >= 0 && Koneko.dDevice.camera.cameraHeight < tileMap->MapSizeZ) {
        cameraLayerRelativeHeight = Koneko.dDevice.camera.cameraHeight - (int)Koneko.dDevice.camera.cameraHeight;
        cameraLayer = Koneko.dDevice.camera.cameraHeight;

        inWall = tileMap->TileIndex[tileMap->MapData[((int)Koneko.dDevice.camera.cameraHeight * tileMap->MapDataLayerPitch) + ((int)Koneko.dDevice.camera.position.y * tileMap->MapSizeX) + (int)Koneko.dDevice.camera.position.x]].isSolid;
        CastRay(layer, cameraLayer, inWall, &rayDirection, &Koneko.dDevice.camera.position, &rayHitPosition, &rayDistance, &hitTileID);

        /* Cast above layers */
        if (cameraLayer + 1 < tileMap->MapSizeZ)
            CastRecursiveLayers(layer, cameraLayer + 1, &rayDirection, 1, rayDistance, screenX);
        /* Cast below layers */
        if (cameraLayer >= 1)
            CastRecursiveLayers(layer, cameraLayer - 1, &rayDirection, -1, rayDistance, screenX);
        

        if (rayDistance > 0) {
            DrawWallStripe(layer, screenX, GetWallStripe(tileMap, &rayDirection, &rayHitPosition), cameraLayerRelativeHeight, rayDistance, hitTileID);

            DrawFloorStripe(layer, cameraLayer, &rayDirection, screenX, rayDistance);
            DrawCeilStripe(layer, cameraLayer, &rayDirection, screenX, rayDistance);
        }
    }
}

/*
    SUMMARY: Draws a raycasted frame of the current layer
*/
void KON_DrawRaycast(MapLayer* layer) {
    int screenX;

    /* Clear the effect buffer */
    /*memset(layer->effectBuffer, 0x0F, Koneko.dDevice.InternalResolution.x * Koneko.dDevice.InternalResolution.y * sizeof(uint32_t));*/

    /* Render each vertical stripe of the screen */
    for (screenX = 0; screenX < Koneko.dDevice.InternalResolution.x; screenX++) {
        DrawRaycastStripe(layer, screenX);
    }

    /* Update the screen with the content of the effect buffer */
    SDL_UpdateTexture(layer->effectTexture, NULL, layer->effectBuffer, layer->effectBufferPitch);
    SDL_RenderCopy(Koneko.dDevice.Renderer, layer->effectTexture, NULL, (SDL_Rect*)&Koneko.dDevice.RenderRect);
}