/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "Load.h"
#include <linux/limits.h>
#include <libgen.h>

#include "Graphics.h"
#include "CommunFunctions.h"

#ifdef _SDL
    #include <SDL/SDL_rotozoom.h>
#endif

#ifdef _SDL
    SDL_Surface* UseAlphaChannel(SDL_Surface* AlphaSurface){
        SDL_SetAlpha(AlphaSurface, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
        AlphaSurface->format->Amask = 0xFF000000;
        /* AlphaSurface->format->Ashift = 24; */
        return AlphaSurface;
    }
#endif

#ifdef _SDL
SDL_Surface* ResizeSurface(SDL_Surface* Source, int Scalar){
    SDL_Surface* newSurface;
    SDL_Surface* convertedSurface;
    SDL_Surface* ResizedSurface;
    SDL_Surface* AcceleratedSurface;

    newSurface = CreateEmptySurface(SDL_HWSURFACE, Source->w * Scalar, Source->h * Scalar);
    if (!newSurface)
        printf("ERROR: Couldn't create new surface !\n");
    convertedSurface = SDL_ConvertSurface(newSurface, Source->format, Source->flags);
    if (!convertedSurface)
        printf("ERROR: Couldn't convert surface format !\n");
    SDL_FreeSurface(newSurface);
    SDL_BlitSurface(Source, NULL, convertedSurface, NULL);
    /*SDL_FreeSurface(LoadingSurface);*/
    ResizedSurface = zoomSurface(convertedSurface, Scalar, Scalar, false);
    if (!ResizedSurface){
        printf("ERROR: Couldn't resize surface !\n");
        ResizedSurface = convertedSurface;
    } else {
        SDL_FreeSurface(convertedSurface);
    }
    AcceleratedSurface = SDL_DisplayFormat(ResizedSurface);
    if (!AcceleratedSurface)
        printf("ERROR: Couldn't optimize surface !\n");
    SDL_FreeSurface(ResizedSurface);

    return AcceleratedSurface;
}
#endif

SDL_Surface* KON_LoadSDLSurface(char FilePath[], DisplayDevice* Device){
    SDL_Surface* LoadingSurface;

    #ifdef _SDL
        SDL_Surface* AcceleratedSurface;
    #endif

    LoadingSurface = SDL_LoadBMP(FilePath);
    #ifdef _SDL
        AcceleratedSurface = ResizeSurface(LoadingSurface, Device->IRScalar);
        SDL_FreeSurface(LoadingSurface);

        return AcceleratedSurface;
    #else
        return LoadingSurface;
    #endif
}

void KON_KeySurface(SDL_Surface* SurfaceToKey, Uint32 ColorKey){
    #ifdef _SDL
        SDL_SetColorKey(SurfaceToKey, SDL_SRCCOLORKEY | SDL_RLEACCEL, ColorKey);
    #else
        SDL_SetColorKey(SurfaceToKey, true, ColorKey);
    #endif
}

SDL_Texture* KON_LoadSurface(char FilePath[], DisplayDevice* Device, Uint32 ColorKey, char flags){
    SDL_Surface* loadingSurface;
    
    if (!FilePath) /* Don't bother loading a surface if the path isn't provided */
        return NULL;
    loadingSurface = KON_LoadSDLSurface(FilePath, Device);
    if (loadingSurface){

        if (SURFACE_KEYED & flags){
            KON_KeySurface(loadingSurface, ColorKey);
        }

        #ifdef _SDL
            return (SURFACE_ALPHA & flags) ? UseAlphaChannel(loadingSurface) : loadingSurface;
        #else
            return SDL_CreateTextureFromSurface(Device->Renderer, loadingSurface);
        #endif

    } else {
        printf("ERROR: (loadSurface) Couldn't load %s !\n", FilePath);
    }
    return NULL;
}

void KON_FreeSurface(SDL_Texture* surface){
    #ifdef _SDL
        SDL_FreeSurface(surface);
    #else
        SDL_DestroyTexture(surface);
    #endif
}

BitmapFont* KON_LoadBitmapFont(char FilePath[], DisplayDevice* DDevice, Uint32 FontColorKey){
    /* Declaration */
    BitmapFont* LoadingFont;
    SDL_Surface* LoadingSurface;
    unsigned int FontPixX, FontPixY;
    unsigned int LetterIndex;
    int FontTexW, FontTexH;

    /* Init */
    LetterIndex = 0;
    FontPixX = 1;

    /* Load font surface*/
    LoadingFont = (BitmapFont*)malloc(sizeof(BitmapFont));
    LoadingSurface = KON_LoadSDLSurface(FilePath, DDevice);
    if (!LoadingSurface){
        printf("ERROR: (KON_LoadBitmapFont) Can't load font %s\n", SDL_GetError());
    }
    KON_KeySurface(LoadingSurface, FontColorKey);
    
    FontTexW = LoadingSurface->w;
    FontTexH = LoadingSurface->h;

    /* Fill-in rects for each letter */
    SDL_LockSurface(LoadingSurface); /* Ensure that the pixel data is available for hw surfaces */ /* VERRRY SLOW AND INEFICIENT */
    while ((FontPixX < FontTexW) && (LetterIndex < 95)){

        /* Get letter coordinates */
        FontPixY = 0;
        LoadingFont->Rects[LetterIndex].x = FontPixX;
        LoadingFont->Rects[LetterIndex].y = 1;

        /* Get letter width */
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixX < FontTexW)){
            FontPixX++;
        }
        LoadingFont->Rects[LetterIndex].w = FontPixX - LoadingFont->Rects[LetterIndex].x;
        
        /* Get letter height */
        FontPixY = 1;
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixY < FontTexH)){
            FontPixY++;
        }
        LoadingFont->Rects[LetterIndex].h = FontPixY - LoadingFont->Rects[LetterIndex].y;

        FontPixX++;
        LetterIndex++;
    }
    SDL_UnlockSurface(LoadingSurface);

    #ifdef _SDL
        LoadingFont->FontSurface = LoadingSurface;
    #else
        LoadingFont->FontSurface = SDL_CreateTextureFromSurface(DDevice->Renderer, LoadingSurface);
        SDL_FreeSurface(LoadingSurface);
    #endif

    return LoadingFont;
}

SDL_Texture* KON_CreateTargetSurface(DisplayDevice* DDevice, int w, int h){
    SDL_Texture* LoadingSurface;

    LoadingSurface = NULL;
#ifdef _SDL
    LoadingSurface = CreateEmptySurface(SDL_HWSURFACE, w, h);
#else
    LoadingSurface = SDL_CreateTexture(DDevice->Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(LoadingSurface, SDL_BLENDMODE_BLEND);
#endif
    if (LoadingSurface == NULL)
        fprintf(stderr, "Can't load texture %s\n", SDL_GetError());
    return LoadingSurface;
}

Mix_Music* KON_LoadMusic(char FilePath[]){
    Mix_Music* LoadingMusic = NULL;

    if (FilePath){
        LoadingMusic = Mix_LoadMUS(FilePath);
        if (LoadingMusic == NULL)
            fprintf(stderr, "Can't load music %s\n", Mix_GetError());
    }
    
    return LoadingMusic;
}

Mix_Chunk* KON_LoadSoundEffect(char FilePath[]){
    Mix_Chunk* LoadingSoundEffect = NULL;

    LoadingSoundEffect = Mix_LoadWAV(FilePath);
    if (LoadingSoundEffect == NULL)
        fprintf(stderr, "Can't load sound effect %s\n", Mix_GetError());
    return LoadingSoundEffect;
}

xmlDoc* KON_LoadXml(char* filePath){
    xmlKeepBlanksDefault(0); /* Ignore white space */

	#ifdef _XBOX
		return xmlParseFile(filePath); /* Load File into memory */
	#else
		return xmlReadFile(filePath, NULL, 0); /* Load File into memory */
	#endif
}

/* Load a bitmap from a map file */
BitMap* KON_LoadBitMap(DisplayDevice* DDevice, FILE* bitMapFile, char* rootDirectory){
    BitMap* loadedBitmap;
    Uint32 colorKey;
    char Buffer[PATH_MAX];

    loadedBitmap = (BitMap*)malloc(sizeof(BitMap));

    fscanf(bitMapFile, "%x\n", &colorKey);
    fgets(Buffer, PATH_MAX, bitMapFile);
    Buffer[strcspn(Buffer, "\n")] = '\0';
    astrcpy(&loadedBitmap->bitMapPath, Buffer);
    loadedBitmap->bitMapSurface = KON_LoadSurface(strcat(strcat(strcpy(Buffer, rootDirectory), "/"), loadedBitmap->bitMapPath), DDevice, colorKey, SURFACE_KEYED);
    SDL_QueryTexture(loadedBitmap->bitMapSurface, NULL, NULL, &loadedBitmap->bitMapSize.x, &loadedBitmap->bitMapSize.y);
    loadedBitmap->colorKey = colorKey;

    return loadedBitmap;
}

/* Load a tilemap from a map file */
TileMap* KON_LoadTileMap(DisplayDevice* DDevice, FILE* tileMapFile, char* rootDirectory){
    TileMap* loadedTilemap = NULL;
    unsigned int nbOfSolidTiles;
    unsigned int i, j;

    loadedTilemap = (TileMap*)malloc(sizeof(TileMap));

    /* TileMap Surface */        
    loadedTilemap->tileSet = KON_LoadBitMap(DDevice, tileMapFile, rootDirectory);
    
    /* Properties */
    fscanf(tileMapFile, "%u %u %u %u %u\n", &loadedTilemap->MapSizeX, &loadedTilemap->MapSizeY, &loadedTilemap->tMSizeX, &loadedTilemap->tMSizeY, &loadedTilemap->TileSize);

    /* LayerData */
    loadedTilemap->MapData = (unsigned int**)malloc(sizeof(unsigned int*)*loadedTilemap->MapSizeY);
    for (i = 0; i < loadedTilemap->MapSizeY; i++){
        loadedTilemap->MapData[i] = (unsigned int*)malloc(sizeof(unsigned int)*loadedTilemap->MapSizeX);
        for (j = 0; j < loadedTilemap->MapSizeX; j++){
            fscanf(tileMapFile, "%u", &loadedTilemap->MapData[i][j]);
        }
    }

    /* Solid tiles */
    fscanf(tileMapFile, "%u", &nbOfSolidTiles);
    loadedTilemap->solidTiles = NULL;
    for (i = 0; i < nbOfSolidTiles; i++){
        fscanf(tileMapFile, "%u", &j);
        KON_appendToList(&loadedTilemap->solidTiles, &j, sizeof(unsigned int));
    }

    return loadedTilemap;
}

Map* KON_LoadMap(DisplayDevice* DDevice, char* MapFilePath){
    /* Declaration */
    Map* LoadedMap = NULL;
    MapLayer* currentLayer = NULL;
    FILE* MapFile = NULL;
    TileMap* loadedTileMap = NULL;
    unsigned int nbOfLayers, layerType;
    unsigned int i;
    char* MapRoot = NULL;
    char filepath[PATH_MAX];

    /* Init */
    MapFile = fopen(MapFilePath, "r");
    if (!MapFile){
        printf("Couldn't load map file: %s !\n", MapFilePath);
        goto Error;
    }

    LoadedMap = (Map*)malloc(sizeof(Map));
    astrcpy(&LoadedMap->MapFilePath, MapFilePath);
    strcpy(filepath, MapFilePath);
    MapRoot = dirname(filepath);

    fscanf(MapFile, "%u", &nbOfLayers);
    LoadedMap->MapLayer = (MapLayer*)malloc(sizeof(MapLayer)*nbOfLayers);
    LoadedMap->nbOfLayers = nbOfLayers;

    /* Logic */
    for (i = 0; i < nbOfLayers; i++){ /* For each layer */
        /* Check layer type */
        layerType = 0;
        fscanf(MapFile, "%u\n", &layerType);
        currentLayer = LoadedMap->MapLayer + i;
        switch (layerType){
            case KON_LAYER_BITMAP:
                currentLayer->layerData = (void*)KON_LoadBitMap(DDevice, MapFile, MapRoot);
                currentLayer->boundingBox.w = ((BitMap*)currentLayer->layerData)->bitMapSize.x;
                currentLayer->boundingBox.h = ((BitMap*)currentLayer->layerData)->bitMapSize.y;
                currentLayer->boundingBox.x = currentLayer->boundingBox.y = 0;
                break;
            case KON_LAYER_TILEMAP:
                loadedTileMap = currentLayer->layerData = (void*)KON_LoadTileMap(DDevice, MapFile, MapRoot);
                currentLayer->boundingBox = InitRect(0, 0, loadedTileMap->MapSizeX * loadedTileMap->TileSize, loadedTileMap->MapSizeY * loadedTileMap->TileSize);
                break;

            default:
                printf("ERROR: unknown layer mode\n");
                break;
        }
        LoadedMap->MapLayer[i].layerType = layerType;
        LoadedMap->MapLayer[i].shown = true;
    }

    /* free */
Error:
    if (MapFile)
        fclose(MapFile);

    return LoadedMap;
}

void KON_SaveTileMap(Map* MapToSave){
    /* Declaration */
    FILE* MapFile;
    MapLayer* currentMapLayer = NULL;
    LinkedList* FBTiles;
    unsigned int i, j, k;
    unsigned int nbOfLayers, nbOfSolidTiles = 0;
    TileMap* currentTileMap;

    /* Init */
    MapFile = fopen(MapToSave->MapFilePath, "w");
    if (!MapFile){
        printf("Couldn't load map file: %s !\n", MapToSave->MapFilePath);
        goto Error;
    }

    /* Logic */
    nbOfLayers = MapToSave->nbOfLayers;
    fprintf(MapFile, "%u\n", nbOfLayers);
    for (k = 0; k < nbOfLayers; k++){
        currentMapLayer = MapToSave->MapLayer + k;
        /* Print Layer header */

        fprintf(MapFile, "%u\n", currentMapLayer->layerType);
        switch (currentMapLayer->layerType)
        {
            case KON_LAYER_BITMAP:
                fprintf(MapFile, "%x\n%s\n", ((BitMap*)(currentMapLayer->layerData))->colorKey, ((BitMap*)(currentMapLayer->layerData))->bitMapPath);
                break;
            
            case KON_LAYER_TILEMAP:
                currentTileMap = ((TileMap*)(currentMapLayer->layerData));
                fprintf(MapFile, "%x\n%s\n", currentTileMap->tileSet->colorKey, currentTileMap->tileSet->bitMapPath);


                fprintf(MapFile, "%u %u %u %u %u\n", currentTileMap->MapSizeX, currentTileMap->MapSizeY,
                currentTileMap->tMSizeX, currentTileMap->tMSizeY, currentTileMap->TileSize);
                
                /* Print layer data */
                for (i = 0; i < currentTileMap->MapSizeY; i++){
                    for (j = 0; j < currentTileMap->MapSizeX; j++){
                        fprintf(MapFile, "%u ", currentTileMap->MapData[i][j]);
                    }
                    fprintf(MapFile, "\n");
                }

                /* Print solid tiles */
                FBTiles = currentTileMap->solidTiles;
                while (FBTiles){
                    nbOfSolidTiles++;
                    FBTiles = FBTiles->next;
                }
                fprintf(MapFile, "%u ", nbOfSolidTiles);
                FBTiles = currentTileMap->solidTiles;
                while (FBTiles){
                    fprintf(MapFile, "%u ", *(unsigned int*)FBTiles->data);
                    FBTiles = FBTiles->next;
                }
                break;

            default:
                break;
        }
        fprintf(MapFile, "\n");
    }

    /* free */
Error:
    if (MapFile)
        fclose(MapFile);
}
