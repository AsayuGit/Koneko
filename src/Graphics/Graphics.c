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
#include "Map.h"

/*
    SUMMARY : Draws a bitmap layer on screen.
    INPUT   : MapLayer* Layer : The bitmap layer to draw
*/
void KON_DrawBitMap(MapLayer* Layer) {
    Vector2d pos;

    pos.x = (int)(Layer->pos.x - Koneko.dDevice.Camera.x);
    pos.y = (int)(Layer->pos.y - Koneko.dDevice.Camera.y);

    KON_DrawSurface((KON_Surface*)Layer->layerData, &pos);
}


void KON_DrawTile(MapLayer* Layer, TileMap* map, unsigned int TileID, unsigned int X, unsigned int Y){
    /* Declaration */
    KON_Rect SrcTile, DstTile;
    unsigned int tileSize;

    tileSize = map->TileSize;

    /* Init */
    SrcTile.x = (TileID % map->tMSizeX) * tileSize;
    SrcTile.y = (TileID / map->tMSizeX) * tileSize;
    DstTile.x = X * tileSize + (int)(Layer->pos.x - Koneko.dDevice.Camera.x);
    DstTile.y = Y * tileSize + (int)(Layer->pos.y - Koneko.dDevice.Camera.y);

    SrcTile.w = SrcTile.h = DstTile.w = DstTile.h = tileSize;
    /* Logic */

    KON_DrawScaledSurfaceRect(map->tileSet, &SrcTile, &DstTile);
}

void KON_DrawTileMap(MapLayer* Layer){
    /* Declaration */
    unsigned int i, j;
    TileMap* map;

    map = (TileMap*)Layer->layerData;

    /* Logic */
    if (Layer->shown && (Layer->layerType == KON_LAYER_TILEMAP)){
        for (i = 0; i < map->MapSizeY; i++){
            for (j = 0; j < map->MapSizeX; j++){
                KON_DrawTile(Layer, map, map->MapData[i][j], j, i);
            }
        }
    }
}

uint32_t getpixel(SDL_Surface *surface, int x, int y)
{
    /* Declaration */
    int bpp;
    uint8_t *p;
    
    /* Init */
    if (!surface){
        printf("ERROR: (getpixel) No surface provided !\n");
        return 0; /* Failsafe */
    } else if (!surface->pixels){
        /*printf("ERROR: (getpixel) No pixel data !\n");*/
        return 0; /* Failsafe */
    }

    bpp = surface->format->BytesPerPixel;
    p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp; /* Here p is the address to the pixel we want to retrieve */

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
    Koneko.dDevice.Camera.x = X - (Koneko.dDevice.InternalResolution.x / 2);
    Koneko.dDevice.Camera.y = Y - (Koneko.dDevice.InternalResolution.y / 2);
}

void BoundCameraToRegion(KON_Rect Region){
    if (Koneko.dDevice.Camera.x + Koneko.dDevice.InternalResolution.x > Region.x + Region.w){
        Koneko.dDevice.Camera.x = Region.x + Region.w - Koneko.dDevice.InternalResolution.x;
    }
    if (Koneko.dDevice.Camera.x < Region.x){
        Koneko.dDevice.Camera.x = Region.x;
    }

    if (Koneko.dDevice.Camera.y + Koneko.dDevice.InternalResolution.y > Region.y + Region.h){
        Koneko.dDevice.Camera.y = Region.y + Region.h - Koneko.dDevice.InternalResolution.y;
    }
    if (Koneko.dDevice.Camera.y < Region.y){
        Koneko.dDevice.Camera.y = Region.y;
    }
}

unsigned int KON_GetTile(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y){
    MapLayer* selectedLayer;

    selectedLayer = scene->WorldMap->MapLayer + layerID;
    if (selectedLayer->layerType == KON_LAYER_TILEMAP){
        if (X >= ((TileMap*)selectedLayer->layerData)->MapSizeX || Y >= ((TileMap*)selectedLayer->layerData)->MapSizeY)
            return 0;
        return ((TileMap*)selectedLayer->layerData)->MapData[Y][X];
    }
    return 0;
}

unsigned int KON_GetTileAtCoordinates(SceneHandle* scene, unsigned int layerID, double X, double Y){
    MapLayer* currentMapLayer = NULL;
    unsigned int tileSize;

    currentMapLayer = scene->WorldMap->MapLayer + layerID;
    if (currentMapLayer->layerType == KON_LAYER_TILEMAP){
        tileSize = ((TileMap*)currentMapLayer->layerData)->TileSize;
        X /= tileSize;
        Y /= tileSize;
        return KON_GetTile(scene, layerID, X, Y);
    }
    return 0;
}

bool KON_IsTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int tile){
    LinkedList* nodePointer = NULL;
    MapLayer* currentMapLayer = NULL;
    
    currentMapLayer = scene->WorldMap->MapLayer + layerID;
    if (currentMapLayer->layerType == KON_LAYER_TILEMAP){
        nodePointer = ((TileMap*)(currentMapLayer->layerData))->solidTiles;
        while (nodePointer){
            if (tile == *(unsigned int*)(nodePointer->data)){
                return true;
            }
            nodePointer = nodePointer->next;
        }
    }
    return false;
}

/* Returns true if the Map Tile is solid */
bool KON_IsMapTileSolid(SceneHandle* scene, unsigned int layerID, unsigned int X, unsigned int Y, unsigned int *tile){
    unsigned int mapTile;
    MapLayer* selectedLayer = NULL;
    
    selectedLayer = scene->WorldMap->MapLayer + layerID;
    if (selectedLayer->layerType == KON_LAYER_TILEMAP){
        if (X >= ((TileMap*)selectedLayer->layerData)->MapSizeX || Y >= ((TileMap*)selectedLayer->layerData)->MapSizeY)
            return true;
        
        mapTile = KON_GetTile(scene, layerID, X, Y);
        if (tile)
            (*tile) = mapTile;

        return KON_IsTileSolid(scene, layerID, mapTile);
    }
    return false;
}

bool KON_IsWorldTileSolid(SceneHandle* scene, unsigned int layerID, double X, double Y, unsigned int *tile){
    unsigned int tileSize;
    MapLayer* currentMapLayer = NULL;
    
    currentMapLayer = scene->WorldMap->MapLayer + layerID;
    if (currentMapLayer->layerType == KON_LAYER_TILEMAP){
        tileSize = ((TileMap*)(currentMapLayer->layerData))->TileSize;
        X /= tileSize;
        Y /= tileSize;
        return KON_IsMapTileSolid(scene, layerID, X, Y, tile);
    }
    return false;
}