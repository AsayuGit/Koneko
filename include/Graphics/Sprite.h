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

#ifndef _SPRITE_H
#define _SPRITE_H

    #include "Animation.h"
    #include "Surface.h"
    #include "Rect.h"
    
    #include "System.h"

    typedef struct {
        Vector2d spritePosition;
        /* Texture */
        KON_Rect source;
        KON_Rect destination;
        KON_Rect boundingBox;
        uint32_t colorKey;
        KON_Surface* spriteTexture;
        bool flipX;                       /* Defines if the sprite should be drawned flipped */
        bool flipY;
        bool isVisible;
        
        /* Animation */
        Animation* animationArray;
        uint32_t lastFrame;                /* Time at which the last frame was displayed */
        unsigned int currentFrame;       /* Frame currently being displayed */
        unsigned int playingAnimation;   /* The Animation currently playing */
        bool alimationLoop;
    } Sprite;

    /*
        SUMMARY : Provided with a path loads a spite object in memory.
        INPUT   : Sprite*                : Pointer to the newly created sprite.
        INPUT   : char* spritePath       : Path the sprite should be loaded from.
        INPUT   : KON_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from.
        INPUT   : KON_Rect* destination  : Pointer to the destination rect the sprite should be displayed at.
        INPUT   : uint32_t colorKey        : The colorKey the texture should use if keyed.
        INPUT   : uint8_t textureFlags     : Texture parameters (Ex: Alpha or Not).
    */
    void KON_LoadSprite(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey, uint8_t textureFlags);

    /*
        SUMMARY : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags.
        INPUT   : Sprite*                : Pointer to the newly created sprite.
        INPUT   : char* spritePath       : Path the sprite should be loaded from.
        INPUT   : KON_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from.
        INPUT   : KON_Rect* destination  : Pointer to the destination rect the sprite should be displayed at.
    */
    void KON_LoadSpriteAlpha(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination);

    /*
        SUMMARY : Helper function for KON_LoadSprite() if you don't need to mess with the textureFlags.
        INPUT   : Sprite*                : Pointer to the newly created sprite.
        INPUT   : char* spritePath       : Path the sprite should be loaded from.
        INPUT   : KON_Rect* source       : Pointer to the sub-rect of the source texture the sprite should be displayed from.
        INPUT   : KON_Rect* destination  : Pointer to the destination rect the sprite should be displayed at.
    */
    void KON_LoadSpriteKeyed(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey);

    /*
        SUMMARY : Loads a sprite from a xml property file.
        INPUT   : Sprite*                   : Pointer to the newly created sprite.
        INPUT   : char* spriteXmlPath       : Path the sprite should be loaded from.
    */
    void KON_LoadSpriteFromXml(Sprite* sprite, char* spriteXmlPath);

    /*
        SUMMARY : Free a loaded sprite form memory.
        INPUT   : Sprite** sprite : Pointer to a Sprite loaded in memory.
    */
    void KON_FreeSprite(Sprite* sprite);

    /*
        SUMMARY : Starts a new sprite animation.
        INPUT   : Sprite* sprite           : The Sprite to start the animation for.
        INPUT   : unsigned int animationID : The animation to start.
        INPUT   : bool reset               : If the animation should start by the begining.
        INPUT   : bool loop                : If the animation should loop.
    */
    void KON_PlaySpriteAnimation(Sprite* sprite, unsigned int animationID, bool reset, bool loop);

    /*
        SUMMARY : Draws a sprite on screen.
        INPUT   : Sprite* sprite          : Sprite to draw.
    */
    void KON_DrawSprite(Sprite* sprite);

#endif