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

#ifndef _SPRITE_H
#define _SPRITE_H

    #include "Animation.h"
    #include "Surface.h"
    #include "Rect.h"
    
    #include "System.h"

    typedef struct {
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

    void KON_LoadSprite(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey, uint8_t textureFlags);
    void KON_LoadSpriteAlpha(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination);
    void KON_LoadSpriteKeyed(DisplayDevice* dDevice, Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey);
    void KON_LoadSpriteFromXml(DisplayDevice* dDevice, Sprite* sprite, char* spriteXmlPath);
    void KON_FreeSprite(Sprite* sprite);
    void KON_PlaySpriteAnimation(Sprite* sprite, unsigned int animationID, bool reset, bool loop);
    void KON_DrawSprite(DisplayDevice* dDevice, Sprite* sprite, Vector2d spritePosition);

#endif