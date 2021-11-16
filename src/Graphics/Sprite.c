#include "Sprite.h"
#include "Load.h" /* Load textures from disk */
#include "System.h" /* Draw texture on screen */

/*
    KON_LoadSprite() : Provided with a path loads a spite object in memory
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
    INPUT: Uint32 colorKey        : The colorKey the texture should use if keyed
    INPUT: Uint8 textureFlags     : Texture parameters (Ex: Alpha or Not)
    OUTPUT: Sprite*               : Pointer to the newly created sprite
*/
Sprite* KON_LoadSprite(DisplayDevice* dDevice, char* spritePath, SDL_Rect* source, SDL_Rect* destination, Uint32 colorKey, Uint8 textureFlags) {
    Sprite* loadedSprite = (Sprite*)malloc(sizeof(Sprite));

    /* FIXME: This should change one we have the ressource manager implemented */
    loadedSprite->spriteTexture = KON_LoadSurface(spritePath, dDevice, colorKey, textureFlags);
    
    if (source)
        loadedSprite->source = *source;
    else {
        loadedSprite->source.x = 0;
        loadedSprite->source.y = 0;
        SDL_QueryTexture(loadedSprite->spriteTexture, NULL, NULL, &loadedSprite->source.w, &loadedSprite->source.h);
    }
    
    loadedSprite->destination = (destination) ? *destination : loadedSprite->source;
    loadedSprite->colorKey = colorKey;

    return loadedSprite;
}

/*
    HON_LoadSpriteAlpha() : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
    OUTPUT: Sprite*               : Pointer to the newly created sprite
*/
Sprite* KON_LoadSpriteAlpha(DisplayDevice* dDevice, char* spritePath, SDL_Rect* source, SDL_Rect* destination) {
    return KON_LoadSprite(dDevice, spritePath, source, destination, 0x0, SURFACE_ALPHA);
}

/*
    HON_LoadSpriteKeyed() : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
    OUTPUT: Sprite*               : Pointer to the newly created sprite
*/
Sprite* KON_LoadSpriteKeyed(DisplayDevice* dDevice, char* spritePath, SDL_Rect* source, SDL_Rect* destination, Uint32 colorKey) {
    return KON_LoadSprite(dDevice, spritePath, source, destination, colorKey, SURFACE_KEYED);
}
/*
    KON_FreeSprite() : Free a loaded sprite form memory
    INPUT: Sprite** sprite : Pointer to a Sprite loaded in memory
*/
void KON_FreeSprite(Sprite** sprite) {
    if (!sprite) /* Check if sprite's not null */
        return;

    /* FIXME: This should change one we have the ressource manager implemented */
    KON_FreeSurface((*sprite)->spriteTexture);
    free(*sprite);
    *sprite = NULL;
}

/*
    KON_DrawSprite() : Draws a sprite on screen
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite* sprite         : Sprite to display
*/
void KON_DrawSprite(DisplayDevice* dDevice, Sprite* sprite) {
    KON_ScaledDraw(dDevice, sprite->spriteTexture, &sprite->source, &sprite->destination);
}
