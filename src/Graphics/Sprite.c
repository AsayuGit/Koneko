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
#include "xmlTools.h"
#include "RessourceManager.h"

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
    sprite->isVisible = true;
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
    KON_LoadSpriteFromXml() : Loads a sprite from a xml property file
    INPUT: DisplayDevice* dDevice    : Pointer to the engine's display device
    INPUT: Sprite*                   : Pointer to the newly created sprite
    INPUT: char* spriteXmlPath       : Path the sprite should be loaded from
*/
void KON_LoadSpriteFromXml(DisplayDevice* DDevice, Sprite* sprite, char* spriteXmlPath) {
    xmlDoc* spriteXml;
    xmlNode *spriteNode, *property;
    Uint32 colorKey;
    char* spritePath, *Buffer;
    bool sourceDestDefined = false;

    if (!DDevice || !sprite || !spriteXmlPath)
        return;

    *sprite = emptySprite;

    spriteXml = KON_LoadXml(spriteXmlPath); /* Load the xml file in memory */
    spriteNode = xmlDocGetRootElement(spriteXml); /* root node */

    if ((spritePath = (char*)xmlGetProp(spriteNode, (xmlChar*)"texture"))){
        if ((Buffer = (char*)xmlGetProp(spriteNode, (xmlChar*)"colorKey"))){
            sscanf(Buffer, "%x", &colorKey);
            KON_LoadSpriteKeyed(DDevice, sprite, spritePath, NULL, NULL, colorKey);
        } else {
            KON_LoadSpriteAlpha(DDevice, sprite, spritePath, NULL, NULL);
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

/*
    KON_DrawSprite() : Draws a sprite on screen
    INPUT: DisplayDevice* dDevice : Pointer to the engine's display device
    INPUT: Sprite* sprite         : Sprite to display
*/
void KON_DrawSprite(DisplayDevice* dDevice, Sprite* sprite, Vector2d spritePosition) {    
    if (!sprite || !sprite->isVisible)
        return;
    KON_UpdateSpriteAnimation(sprite);
    
    sprite->boundingBox = sprite->destination;
    sprite->boundingBox.x += spritePosition.x - dDevice->Camera.x; /* FIXME: we shouldn't have to take the camera into account here since we're drawing in worldspace right ? */
    sprite->boundingBox.y += spritePosition.y - dDevice->Camera.y;

    KON_ScaledDrawEx(dDevice, sprite->spriteTexture, &sprite->source, &sprite->boundingBox, sprite->flipX);
}