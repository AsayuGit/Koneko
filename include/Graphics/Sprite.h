#ifndef _SPRITE_H
#define _SPRITE_H

    #include "Types.h"

    typedef struct {
        SDL_Rect source;
        SDL_Rect destination;
        Uint32 colorKey;
        SDL_Texture* spriteTexture;
    } Sprite;

    Sprite* KON_LoadSprite(DisplayDevice* dDevice, char* spritePath, SDL_Rect* source, SDL_Rect* destination, Uint32 colorKey, Uint8 textureFlags);
    Sprite* KON_LoadSpriteAlpha(DisplayDevice* dDevice, char* spritePath, SDL_Rect* source, SDL_Rect* destination);
    void    KON_FreeSprite(Sprite** sprite);
    void    KON_DrawSprite(DisplayDevice* dDevice, Sprite* sprite);

#endif