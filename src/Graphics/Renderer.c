#define _GNU_SOURCE

#include "Koneko.h"
#include "Renderer.h"
#include "System.h"
#include "TileMap.h"
#include "TileMap.h"
#include "math.h"

#include "API.h"

/* FIXME: Implement DDA algorithm */
double KON_CastRayOnTileMap(MapLayer* layer, Vector2d* position, Vector2d* rayDirection) {
    unsigned int DDADepth;
    Vector2d ray; /* The end of ray being casted AKA the ray from the position to itself.
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

    rayStep.x = (rayDirection->x < 0) ? -1 : 1;
    rayStep.y = (rayDirection->y < 0) ? -1 : 1;
    
    /* First set up the ray to the boundaries of the X and Y wall*/
    ray.x = floor(position->x) - position->x;
    ray.y = floor(position->y) - position->y;

    if (rayDirection->x > 0)
        ray.x++;

    if (rayDirection->y > 0)
        ray.y++;

    rayMarch.x = fabs(rayDirection->x / rayDirection->y);
    rayMarch.y = fabs(rayDirection->y / rayDirection->x);

    /* DDA Loop */
    for (DDADepth = 0; DDADepth < 100; DDADepth++) {

        rayInterpol.x = rayStep.x * fabs(ray.y) * rayMarch.x;
        rayInterpol.y = rayStep.y * fabs(ray.x) * rayMarch.y;

        /* 0. Compute the length of each rays */
        rayLength.x = sqrt(pow(ray.x, 2) + pow(rayInterpol.y, 2));
        rayLength.y = sqrt(pow(ray.y, 2) + pow(rayInterpol.x, 2));

        if (rayLength.x < rayLength.y) {
            /* Iterate on the X direction */

            rayCheck.x = ray.x;
            rayCheck.y = rayInterpol.y;

            if (rayStep.x < 0)
                rayCheck.x--;

            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x + position->x, rayCheck.y + position->y, NULL)) {
                /* Fish-eye correction : Return the perpendicular distance to the camera instead of the euclidian one */
                
                /*
                    We want |ray| * cos(alpha)
                    Or : cameraDirection . ray = |cameraDirection| * |ray| * cos(alpha)
                    So : cameraDirection . ray / |cameraDirection| = |ray| * cos(alpha)
                    And since : |cameraDirection| = 1
                    Then : |ray| * cos(alpha) = cameraDirection . ray
                */
                return (Koneko.dDevice.camera.direction.x * ray.x + Koneko.dDevice.camera.direction.y * rayInterpol.y); /* / KON_GetVectNorm(Koneko.dDevice.camera.direction);*/
            }
            ray.x += rayStep.x;
        } else {
            /* Iterate on the Y direction */

            rayCheck.x = rayInterpol.x;
            rayCheck.y = ray.y;

            if (rayStep.y < 0)
                rayCheck.y--;

            if (KON_IsTileMapTileSolid(tileMapLayer, rayCheck.x + position->x, rayCheck.y + position->y, NULL)) {
                return (Koneko.dDevice.camera.direction.x * rayInterpol.x + Koneko.dDevice.camera.direction.y * ray.y); /* / KON_GetVectNorm(Koneko.dDevice.camera.direction);*/
            }
            ray.y += rayStep.y;
        }
    }

    return 0;
}

void KON_DrawWallLine(MapLayer* layer, unsigned int screenX, double length) {
    unsigned int startY, endY, halfHeight;

    /*printf("Wall is at %u\n", length);*/

    halfHeight = (unsigned int)Koneko.dDevice.InternalResolution.y >> 1;
    
    if (length <= 0.0)
        length = 1.0;
    
    length =  halfHeight / length;
    
    startY = halfHeight - length;
    endY = halfHeight + length;

    SDL_SetRenderDrawColor(Koneko.dDevice.Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(Koneko.dDevice.Renderer, screenX, startY, screenX, endY);
    SDL_SetRenderDrawColor(Koneko.dDevice.Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void KON_DrawRaycast(MapLayer* layer) {
    int screenX;
    double rayLength;
    double progress;

    Vector2d rayDirection;
    Vector2d mapPosition;

    for (screenX = 0; screenX < Koneko.dDevice.InternalResolution.x; screenX++) {
        progress = (((double)screenX * 2 / (double)Koneko.dDevice.InternalResolution.x) - 1);
        rayDirection.x = Koneko.dDevice.camera.direction.x + progress * Koneko.dDevice.camera.plane.x;
        rayDirection.y = Koneko.dDevice.camera.direction.y + progress * Koneko.dDevice.camera.plane.y;
        
        mapPosition.x = Koneko.dDevice.camera.position.x / ((TileMap*)layer->layerData)->TileSize;
        mapPosition.y = Koneko.dDevice.camera.position.y / ((TileMap*)layer->layerData)->TileSize;

        rayLength = KON_CastRayOnTileMap(layer, &mapPosition, &rayDirection);
        KON_DrawWallLine(layer, screenX, rayLength);
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