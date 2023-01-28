/*
    Koneko is a 2D/Raycast3D game engine written from scratch in ANSI C
    using SDL and libxml2. This engine is meant to allow game developpement
    for Linux, Windows and the og Xbox.

    Copyright (C) 2021-2023 Killian RAIMBAUD [Asayu] (killian.rai@gmail.com)

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

#include "Animation.h"
#include "Log.h"
#include "System.h"

#include <stdlib.h>
#include <string.h>

KON_SpriteAnimation* KON_ParseSpriteAnimation(KON_XMLNode* animArray, size_t* nbOfSpriteAnimation) {
    KON_SpriteAnimation* LoadingAnimation;
    KON_XMLNode *entry;
    unsigned int ArrayID;

    if (!animArray || !nbOfSpriteAnimation) {
        KON_SystemMsg("(KON_ParseSpriteAnimation) Invalid Parameters!", MESSAGE_ERROR, 0);
        return NULL;
    }

    *nbOfSpriteAnimation = KON_GetXMLNodeCount(animArray, "anim");
    if (!(LoadingAnimation = (KON_SpriteAnimation*)calloc(*nbOfSpriteAnimation, sizeof(KON_SpriteAnimation)))) {
        KON_SystemMsg("(KON_ParseSpriteAnimation) No more memory!", MESSAGE_ERROR, 0);
        return NULL;
    }
    animArray = KON_GetXMLNodeChild(animArray);

    ArrayID = 0;
    while (animArray){
        if (KON_CompareXMLNodeName(animArray, "anim")) {
            LoadingAnimation[ArrayID].NbOfFrames = atoi(KON_GetXMLAttribute(animArray, "nbOfFrames"));
            LoadingAnimation[ArrayID].Framerate = atoi(KON_GetXMLAttribute(animArray, "framerate"));

            for (entry = KON_GetXMLNodeChild(animArray); entry; entry = KON_GetXMLNodeSibling(entry)) {
                if (KON_CompareXMLNodeName(entry, "src")) {                    
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].SrcRect);
                } else if (KON_CompareXMLNodeName(entry, "dst")) {
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].DstRect);
                }
            }
            ArrayID++;
        }
        animArray = KON_GetXMLNodeSibling(animArray);
    }

    return LoadingAnimation;
}

void KON_FreeSpriteAnimation(KON_SpriteAnimation* spriteAnim) {
    free(spriteAnim);
}

KON_KeyFrameAnimation* KON_ParseKeyFrameAnimation(KON_XMLNode* animArray, size_t* nbOfKeyFrameAnimation) {
    KON_KeyFrameAnimation* newKeyFrameAnimation;
    KON_XMLNode* animNode, *keyFrameNode;
    unsigned int keyFrameIndex;
    const char* interpolation;

    if (!animArray || !nbOfKeyFrameAnimation) {
        KON_SystemMsg("(KON_ParseKeyFrameAnimation) Invalid Parameters!", MESSAGE_ERROR, 0);
        return NULL;
    }

    *nbOfKeyFrameAnimation = KON_GetXMLNodeCount(animArray, "anim");
    if (!(newKeyFrameAnimation = (KON_KeyFrameAnimation*)calloc(*nbOfKeyFrameAnimation, sizeof(KON_KeyFrameAnimation)))) {
        KON_SystemMsg("(KON_ParseKeyFrameAnimation) No more memory!", MESSAGE_ERROR, 0);
        return NULL;
    }

    for (animNode = KON_GetXMLNodeChild(animArray); animNode; animNode = KON_GetXMLNodeSibling(animNode)) {
        if (!KON_CompareXMLNodeName(animNode, "anim"))
            continue;

        if ((interpolation = KON_GetXMLAttribute(animNode, "interpolation"))) {
            if (strcmp(interpolation, "None") == 0) {
                newKeyFrameAnimation->interpolation = KON_NO_INTERPOLATION;
            } else if (strcmp(interpolation, "Linear") == 0) {
                newKeyFrameAnimation->interpolation = KON_LINEAR_INTERPOLATION;
            } else if (strcmp(interpolation, "Cubic") == 0) {
                newKeyFrameAnimation->interpolation = KON_CUBIC_INTERPOLATION;
            }
        }
        
        newKeyFrameAnimation->nbOfKeyFrames = KON_GetXMLNodeCount(animNode, "keyFrame");
        if (!(newKeyFrameAnimation->keyFrameArray = (KON_KeyFrame*)malloc(newKeyFrameAnimation->nbOfKeyFrames * sizeof(KON_KeyFrame)))) {
            KON_SystemMsg("(KON_ParseKeyFrameAnimation) No more memory!", MESSAGE_ERROR, 0);
            return NULL;
        }

        keyFrameIndex = 0;
        for (keyFrameNode = KON_GetXMLNodeChild(animNode); keyFrameNode; keyFrameNode = KON_GetXMLNodeSibling(keyFrameNode)) {
            if (!KON_CompareXMLNodeName(keyFrameNode, "keyFrame"))
                continue;
            newKeyFrameAnimation->keyFrameArray[keyFrameIndex].pos.x = KON_GetXMLAttributeAsDouble(keyFrameNode, "X");
            newKeyFrameAnimation->keyFrameArray[keyFrameIndex].pos.y = KON_GetXMLAttributeAsDouble(keyFrameNode, "Y");
            newKeyFrameAnimation->keyFrameArray[keyFrameIndex].timeCodeMS = KON_GetXMLAttributeAsDouble(keyFrameNode, "length") * 1000;
            keyFrameIndex++;
        }
    }

    KON_SystemMsg("(KON_ParseKeyFrameAnimation) Loaded new keyframe animations", MESSAGE_LOG, 0);
    return newKeyFrameAnimation;
}

void KON_FreeKeyFrameAnimation(KON_KeyFrameAnimation* keyframeAnim) {
    if (!keyframeAnim)
        return;
    free(keyframeAnim->keyFrameArray);
    free(keyframeAnim);
}

static double KON_DoubleLinearInterpolate(double DstA, double DstB, double SrcA, double SrcB, double Value) {
    if (SrcA == SrcB)
        return DstA; /* In this case any dest value is solution */
    return DstA + (DstB - DstA) * ((Value - SrcA) / (SrcB - SrcA));
}

/*
    SUMMARY : Animate a vector with no interpolation.
    INPUT   : Vector2d* vector            : The Vector to animate.
    INPUT   : KON_KeyFrameAnimation* anim : The KeyFrame animation to animate the vector with.
*/
static void KON_UpdateJumpAnimation(Vector2d* vector, KON_KeyFrameAnimation* anim) {
    uint32_t currentTimeCode = KON_GetMs();

    if (currentTimeCode < anim->lastTimeCode + anim->keyFrameArray[anim->currentKeyFrame].timeCodeMS)
        return;

    anim->lastTimeCode = currentTimeCode;
    *vector = anim->keyFrameArray[anim->currentKeyFrame].pos;
    anim->currentKeyFrame++;
}

/*
    SUMMARY : Animate a vector with linear interpolation.
    INPUT   : Vector2d* vector            : The Vector to animate.
    INPUT   : KON_KeyFrameAnimation* anim : The KeyFrame animation to animate the vector with.
*/
static void KON_UpdateLinearAnimation(Vector2d* vector, KON_KeyFrameAnimation* anim) {
    #define keyFramePos anim->keyFrameArray[anim->currentKeyFrame].pos
    uint32_t currentTimeCode = KON_GetMs();
    register uint32_t nextTimeCode = anim->lastTimeCode + anim->keyFrameArray[anim->currentKeyFrame].timeCodeMS;

    if (!anim->currentKeyFrame || currentTimeCode >= nextTimeCode) {
        anim->lastTimeCode = currentTimeCode;
        *vector = keyFramePos;
        anim->currentKeyFrame++;
        return;
    }

    vector->x = KON_DoubleLinearInterpolate(anim->keyFrameArray[anim->currentKeyFrame - 1].pos.x, anim->keyFrameArray[anim->currentKeyFrame].pos.x, anim->lastTimeCode, nextTimeCode, currentTimeCode);
    vector->y = KON_DoubleLinearInterpolate(anim->keyFrameArray[anim->currentKeyFrame - 1].pos.y, anim->keyFrameArray[anim->currentKeyFrame].pos.y, anim->lastTimeCode, nextTimeCode, currentTimeCode);
}

/*
    SUMMARY : Animate a vector with cubic interpolation.
    INPUT   : Vector2d* vector            : The Vector to animate.
    INPUT   : KON_KeyFrameAnimation* anim : The KeyFrame animation to animate the vector with.
*/
static void KON_UpdateCubicAnimation(Vector2d* vector, KON_KeyFrameAnimation* anim) {

}

void KON_UpdateAnimation(Vector2d* vector, KON_KeyFrameAnimation* anim) {
    if (!vector || !anim) {
        KON_SystemMsg("(KON_UpdateAnimation) Invalid parameters!", MESSAGE_WARNING, 0);
        return;
    }

    if (anim->currentKeyFrame == -1)
        return;

    switch (anim->interpolation) {
    case KON_NO_INTERPOLATION:
        KON_UpdateJumpAnimation(vector, anim);
        break;

    case KON_LINEAR_INTERPOLATION:
        KON_UpdateLinearAnimation(vector, anim);
        break;

    case KON_CUBIC_INTERPOLATION:
        KON_UpdateCubicAnimation(vector, anim);
        break;
    
    default:
        break;
    }

    if (anim->currentKeyFrame >= anim->nbOfKeyFrames)
        anim->currentKeyFrame = -1;
}
