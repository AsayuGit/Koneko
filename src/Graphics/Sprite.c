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
#include "Sprite.h"
#include "Surface.h" /* Load textures from disk */
#include "System.h" /* Draw texture on screen */
#include "XML.h"

/* defines an empty sprite */
static const Sprite emptySprite;

void KON_LoadSprite(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey, uint8_t textureFlags) {
    Vector2d spriteSize;
    
    if (!sprite)
        return;

    sprite->spriteTexture = KON_LoadSurface(spritePath, colorKey, textureFlags);
    
    if (source)
        sprite->source = *source;
    else {
        KON_GetSurfaceSize(sprite->spriteTexture, &spriteSize);
        sprite->source.w = (int)spriteSize.x;
        sprite->source.h = (int)spriteSize.y;
    }
    
    sprite->destination = (destination) ? *destination : sprite->source;
    sprite->colorKey = colorKey;
    sprite->isVisible = true;
}

void KON_LoadSpriteAlpha(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination) {
    KON_LoadSprite(sprite, spritePath, source, destination, 0x0, SURFACE_ALPHA);
}

void KON_LoadSpriteKeyed(Sprite* sprite, char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey) {
    KON_LoadSprite(sprite, spritePath, source, destination, colorKey, SURFACE_KEYED);
}

void KON_LoadSpriteFromXml(Sprite* sprite, char* spriteXmlPath) {
    xmlDoc* spriteXml;
    xmlNode *spriteNode, *property;
    Uint32 colorKey;
    char* spritePath, *Buffer;
    bool sourceDestDefined = false;

    if (!sprite || !spriteXmlPath)
        return;

    *sprite = emptySprite;

    spriteXml = KON_LoadXml(spriteXmlPath); /* Load the xml file in memory */
    spriteNode = xmlDocGetRootElement(spriteXml); /* root node */

    if ((spritePath = (char*)xmlGetProp(spriteNode, (xmlChar*)"texture"))){
        if ((Buffer = (char*)xmlGetProp(spriteNode, (xmlChar*)"colorKey"))){
            sscanf(Buffer, "%x", &colorKey);
            KON_LoadSpriteKeyed(sprite, spritePath, NULL, NULL, colorKey);
        } else {
            KON_LoadSpriteAlpha(sprite, spritePath, NULL, NULL);
        }
    }

    /* Parsing */
    property = spriteNode->children;
    while (property){
        if (strcmp((char*)property->name, "src") == 0) {
            sourceDestDefined = true;
            KON_LoadRectFromXmlNode(property, &sprite->source);
        } else if (strcmp((char*)property->name, "dst") == 0) {
            sourceDestDefined = true;
            KON_LoadRectFromXmlNode(property, &sprite->destination);
        } else if (strcmp((char*)property->name, "animArray") == 0) {
            sprite->animationArray = KON_ParseAnimation(property);
        }
        property = property->next;
    }

    if (sprite->animationArray && !sourceDestDefined)
        KON_PlaySpriteAnimation(sprite, 0, true, true);

    xmlFreeDoc(spriteXml);
}

void KON_FreeSprite(Sprite* sprite) {
    if (!sprite) /* Check if sprite's not null */
        return;

    KON_FreeSurface(sprite->spriteTexture);
    *sprite = emptySprite;
}

void KON_PlaySpriteAnimation(Sprite* sprite, unsigned int animationID, bool reset, bool loop) {
    if (reset || animationID != sprite->playingAnimation){
        sprite->playingAnimation = animationID;
        sprite->currentFrame = 0;
        sprite->alimationLoop = loop;
    }
}

void KON_UpdateSpriteAnimation(Sprite* sprite) {
    Animation* spriteAnimation = NULL;
    
    if (!sprite->animationArray)
        return;

    spriteAnimation = sprite->animationArray + sprite->playingAnimation;

    /* Early return if we shouldn't update the sprite frame yet */
    if (SDL_GetTicks() < (sprite->lastFrame + spriteAnimation->Framerate))
        return;

    sprite->currentFrame++;
    if (sprite->currentFrame == spriteAnimation->NbOfFrames)
        sprite->currentFrame = (sprite->alimationLoop) ? 0 : sprite->currentFrame - 1;


    /* Updating source and dest rects */
    sprite->source = spriteAnimation->SrcRect;
    sprite->source.x += sprite->currentFrame * sprite->source.w;
    sprite->destination = spriteAnimation->DstRect;

    /* Flip sprite coordinates horizontally of vertically if needed */
    if (sprite->flipX)
        sprite->destination.x = -(sprite->destination.x + spriteAnimation->DstRect.w);

    if (sprite->flipY)
        sprite->destination.y = -(sprite->destination.y + spriteAnimation->DstRect.h);
    
    sprite->lastFrame = SDL_GetTicks();
}

void KON_DrawSprite(Sprite* sprite) {
    DrawFlags flags = 0;
    if (!sprite || !sprite->isVisible)
        return;
    KON_UpdateSpriteAnimation(sprite);
    
    sprite->boundingBox = sprite->destination;

    /* Convert world space coordinates into screen space */
    sprite->boundingBox.x += (int)(sprite->spritePosition.x - Koneko.dDevice.camera.position.x);
    sprite->boundingBox.y += (int)(sprite->spritePosition.y - Koneko.dDevice.camera.position.y);

    /* Draws sprite in screen space*/
    if (sprite->flipX) flags |= DRAW_HORIZONTAL_FLIP;
    if (sprite->flipY) flags |= DRAW_VERTICAL_FLIP;
    KON_DrawScaledSurfaceRectEx(sprite->spriteTexture, &sprite->source, &sprite->boundingBox, flags);
}