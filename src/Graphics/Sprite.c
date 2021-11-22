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

#include "Sprite.h"
#include "Load.h" /* Load textures from disk */
#include "System.h" /* Draw texture on screen */

/* defines an empty sprite */
static const Sprite emptySprite;

/*
    KON_LoadSprite() : Provided with a path loads a spite object in memory
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite*                : Pointer to the newly created sprite
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
    INPUT: Uint32 colorKey        : The colorKey the texture should use if keyed
    INPUT: Uint8 textureFlags     : Texture parameters (Ex: Alpha or Not)
*/
void KON_LoadSprite(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, SDL_Rect* source, SDL_Rect* destination, Uint32 colorKey, Uint8 textureFlags) {
    if (!sprite)
        return;

    /* FIXME: This should change one we have the ressource manager implemented */
    sprite->spriteTexture = KON_LoadSurface(spritePath, dDevice, colorKey, textureFlags);
    
    if (source)
        sprite->source = *source;
    else {
        sprite->source.x = 0;
        sprite->source.y = 0;
        SDL_QueryTexture(sprite->spriteTexture, NULL, NULL, &sprite->source.w, &sprite->source.h);
    }
    
    sprite->destination = (destination) ? *destination : sprite->source;
    sprite->colorKey = colorKey;
}

/*
    HON_LoadSpriteAlpha() : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite*                : Pointer to the newly created sprite
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
*/
void KON_LoadSpriteAlpha(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, SDL_Rect* source, SDL_Rect* destination) {
    KON_LoadSprite(dDevice, sprite, spritePath, source, destination, 0x0, SURFACE_ALPHA);
}

/*
    HON_LoadSpriteKeyed() : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite*                : Pointer to the newly created sprite
    INPUT: char* spritePath       : Path the sprite should be loaded from
    INPUT: SDL_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from
    INPUT: SDL_Rect* destination  : Pointer to the destination rect the sprite should be displayed at
*/
void KON_LoadSpriteKeyed(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, SDL_Rect* source, SDL_Rect* destination, Uint32 colorKey) {
    KON_LoadSprite(dDevice, sprite, spritePath, source, destination, colorKey, SURFACE_KEYED);
}

/*
    KON_FreeSprite() : Free a loaded sprite form memory
    INPUT: Sprite** sprite : Pointer to a Sprite loaded in memory
*/
void KON_FreeSprite(Sprite* sprite) {
    if (!sprite) /* Check if sprite's not null */
        return;

    /* FIXME: This should change one we have the ressource manager implemented */
    KON_FreeSurface(sprite->spriteTexture);
    *sprite = emptySprite;
}

/*
    KON_DrawSprite() : Draws a sprite on screen
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite* sprite         : Sprite to display
*/
void KON_DrawSprite(DisplayDevice* dDevice, Sprite* sprite) {
    KON_ScaledDraw(dDevice, sprite->spriteTexture, &sprite->source, &sprite->destination);
}
