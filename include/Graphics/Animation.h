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

#ifndef _ANIMATION_H
#define _ANIMATION_H

    #include "Rect.h"
    #include "XML.h"
    #include <stdint.h>

    typedef struct {
        KON_Rect SrcRect;       /* Character source rect */
        KON_Rect DstRect;       /* Character destination */
        int NbOfFrames;         /* Number of frames in an animation */
        int Framerate;          /* Number of milliseconds between two frames */
    } KON_SpriteAnimation;

    typedef enum {
        KON_NO_INTERPOLATION,
        KON_LINEAR_INTERPOLATION,
        KON_CUBIC_INTERPOLATION
    } KON_InterpolationFunction;

    typedef struct {
        uint32_t timeCodeMS;
        Vector2d pos;
    } KON_KeyFrame;

    typedef struct {
        unsigned int nbOfKeyFrames;
        unsigned int currentKeyFrame;
        uint32_t lastTimeCode;
        KON_InterpolationFunction interpolation;
        KON_KeyFrame* keyFrameArray;
    } KON_KeyFrameAnimation;

    /*
        SUMMARY : Parses an KON_SpriteAnimation array from a XML Node.
        INPUT   : KON_XMLNode* animArray      : The XML Node the Sprite animation should be parsed from.
        INPUT   : size_t* nbOfSpriteAnumation : The number of sprite animations in the array.
        OUTPUT  : KON_SpriteAnimation*        : The parsed Sprite Animation.
    */
    KON_SpriteAnimation* KON_ParseSpriteAnimation(KON_XMLNode* animArray, size_t* nbOfSpriteAnimation);

    /*
        SUMMARY : Frees a previously loaded Sprite Animation.
        INPUT   : KON_SpriteAnimation* anim : The Sprite Animation to free.
    */
    void KON_FreeSpriteAnimation(KON_SpriteAnimation* spriteAnim);

    /*
        SUMMARY : Parses a KON_KeyFrameAnimation array from an XML Node
        INPUT   : KON_XMLNode* animArray        : The XML Node the KeyFrame animation should be parsed from.
        INPUT   : size_t* nbOfKeyFrameAnimation : The number of KeyFrame Animations in the array.
        OUTPUT  : KON_KeyFrameAnimation*        : The parsed KeyFrame Animation.
    */
    KON_KeyFrameAnimation* KON_ParseKeyFrameAnimation(KON_XMLNode* animArray, size_t* nbOfKeyFrameAnimation);

    /*
        SUMMARY : Frees a previously loaded KeyFrame Animation.
        INPUT   : KON_KeyFrameAnimation* keyframeAnim : The KeyFrame Animation to free.
    */
    void KON_FreeKeyFrameAnimation(KON_KeyFrameAnimation* keyframeAnim);

    /*
        SUMMARY : Animate a vector.
        INPUT   : Vector2d* vector            : The Vector to animate.
        INPUT   : KON_KeyFrameAnimation* anim : The KeyFrame animation to animate the vector with.
    */
    void KON_UpdateAnimation(Vector2d* vector, KON_KeyFrameAnimation* anim);

#endif
