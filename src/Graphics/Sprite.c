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
#include "System.h"
#include "XML.h"

#include <stdio.h>
#include <string.h>

Sprite* KON_LoadSprite(const char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey, uint8_t textureFlags) {
    Sprite* sprite = NULL;
    Vector2d spriteSize;

    if (!(sprite = (Sprite*)calloc(1, sizeof(Sprite)))) {
        KON_SystemMsg("(KON_LoadSprite) No more memory", MESSAGE_WARNING, 0);
        return NULL;
    }

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

    return sprite;
}

Sprite* KON_LoadSpriteAlpha(const char* spritePath, KON_Rect* source, KON_Rect* destination) {
    return KON_LoadSprite(spritePath, source, destination, 0x0, SURFACE_ALPHA);
}

Sprite* KON_LoadSpriteKeyed(const char* spritePath, KON_Rect* source, KON_Rect* destination, uint32_t colorKey) {
    return KON_LoadSprite(spritePath, source, destination, colorKey, SURFACE_KEYED);
}

Sprite* KON_LoadSpriteFromXml(char* spriteXmlPath) {
    KON_XMLDocument* spriteXml;
    KON_XMLNode* property;
    uint32_t colorKey;
    Sprite* sprite = NULL;
    const char* spritePath, *colorKeyBuffer;
    bool sourceDestDefined = false;

    if (!spriteXmlPath) {
        KON_SystemMsg("(KON_LoadSpriteFromXml) Invalid parameters", MESSAGE_WARNING, 0);
        return NULL;
    }

    if (!(spriteXml = KON_LoadXml(spriteXmlPath))) { /* Load the xml file in memory */
        KON_SystemMsg("(KON_LoadSpriteFromXml) Can't load xml file:", MESSAGE_WARNING, 1, spriteXmlPath);
        return NULL;
    }

    if ((spritePath = KON_GetXMLAttribute(spriteXml->rootNode, "texture"))){
        if ((colorKeyBuffer = KON_GetXMLAttribute(spriteXml->rootNode, "colorKey"))){
            sscanf(colorKeyBuffer, "%x", &colorKey);
            sprite = KON_LoadSpriteKeyed(spritePath, NULL, NULL, colorKey);
        } else {
            sprite = KON_LoadSpriteAlpha(spritePath, NULL, NULL);
        }
    } else {
        KON_SystemMsg("(KON_LoadSpriteFromXml) Invalid xml sprite file:", MESSAGE_WARNING, 1, spriteXmlPath);
        return NULL;
    }

    /* Parsing */
    property = KON_GetXMLNodeChild(spriteXml->rootNode);
    while (property) {
        if (KON_CompareXMLNodeName(property, "src")) {
            sourceDestDefined = true;
            KON_LoadRectFromXmlNode(property, &sprite->source);
        } else if (KON_CompareXMLNodeName(property, "dst")) {
            sourceDestDefined = true;
            KON_LoadRectFromXmlNode(property, &sprite->destination);
        } else if (KON_CompareXMLNodeName(property, "animArray")) {
            sprite->animationArray = KON_ParseSpriteAnimation(property, &sprite->nbOfSpriteAnimations);
        }
        property = KON_GetXMLNodeSibling(property);
    }

    if (sprite->animationArray && !sourceDestDefined)
        KON_PlaySpriteAnimation(sprite, 0, true, true);

    KON_FreeXML(spriteXml);

    return sprite;
}

Sprite* KON_CreateSprite(unsigned int width, unsigned int height) {
    Sprite* newSprite = NULL;

    if (!(newSprite = (Sprite*)calloc(1, sizeof(Sprite)))) {
        KON_SystemMsg("(KON_CreateSprite) No more memory!", MESSAGE_ERROR, 0);
        return NULL;
    }

    newSprite->spriteTexture = KON_CreateSurface(width, height);
    KON_InitRect(newSprite->source, 0, 0, width, height);


    KON_InitRect(newSprite->destination, 0, 0, width, height);
    newSprite->isVisible = true;

    KON_SystemMsg("(KON_CreateSprite) Created new Sprite", MESSAGE_LOG, 0);

    return newSprite;
}

void KON_FreeSprite(Sprite** sprite) {
    if (!sprite || !(*sprite)) /* Check if sprite's not null */
        return;

    KON_FreeSurface((*sprite)->spriteTexture);
    *sprite = NULL;
}

void KON_PlaySpriteAnimation(Sprite* sprite, unsigned int animationID, bool reset, bool loop) {
    if (!sprite) {
        KON_SystemMsg("(KON_PlaySpriteAnimation) Invalid Arguments!", MESSAGE_WARNING, 0);
        return;
    }
    if (animationID >= sprite->nbOfSpriteAnimations) {
        KON_SystemMsg("(KON_PlaySpriteAnimation) Invalid Sprite Animation!", MESSAGE_WARNING, 0);
        return;
    }
    if (reset || animationID != sprite->playingAnimation){
        sprite->playingAnimation = animationID;
        sprite->currentFrame = 0;
        sprite->alimationLoop = loop;
    }
}

void KON_UpdateSpriteAnimation(Sprite* sprite) {
    KON_SpriteAnimation* spriteAnimation = NULL;
    
    if (!sprite->animationArray)
        return;

    spriteAnimation = sprite->animationArray + sprite->playingAnimation;

    /* Early return if we shouldn't update the sprite frame yet */
    if (KON_GetMs() < (sprite->lastFrame + spriteAnimation->Framerate))
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
    
    sprite->lastFrame = KON_GetMs();
}

void KON_DrawSprite(Sprite* sprite) {
    KON_DrawSpriteAt(sprite, sprite->spritePosition.x, sprite->spritePosition.y);
}

void KON_DrawSpriteOffset(Sprite* sprite, double x, double y) {
    KON_DrawSpriteAt(sprite, sprite->spritePosition.x + x, sprite->spritePosition.y + y);
}

void KON_DrawSpriteAt(Sprite* sprite, double x, double y) {
    DrawFlags flags = 0;
    if (!sprite || !sprite->isVisible)
        return;
    KON_UpdateSpriteAnimation(sprite);
    
    sprite->boundingBox = sprite->destination;

    /* Convert world space coordinates into screen space */
    sprite->boundingBox.x += (int)(x - Koneko.dDevice.camera.position.x);
    sprite->boundingBox.y += (int)(y - Koneko.dDevice.camera.position.y);

    /* Draws sprite in screen space*/
    if (sprite->flipX) flags |= DRAW_HORIZONTAL_FLIP;
    if (sprite->flipY) flags |= DRAW_VERTICAL_FLIP;
    KON_DrawScaledSurfaceRectEx(sprite->spriteTexture, &sprite->source, &sprite->boundingBox, flags);
}
