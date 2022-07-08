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

#include "Animation.h"
#include "XML.h"

#include <stdlib.h>

Animation* KON_ParseAnimation(KON_XMLNode* array) {
    Animation* LoadingAnimation;
    KON_XMLNode *entry;
    unsigned int ArrayID;

    LoadingAnimation = (Animation*)malloc(KON_GetXMLNodeChildCount(array)*sizeof(Animation));
    array = KON_GetXMLNodeChild(array);

    ArrayID = 0;
    while (array){
        if (KON_CompareXMLNodeName(array, "anim")) {
            LoadingAnimation[ArrayID].NbOfFrames = atoi(KON_GetXMLAttribute(array, "nbOfFrames"));
            LoadingAnimation[ArrayID].Framerate = atoi(KON_GetXMLAttribute(array, "framerate"));

            entry = KON_GetXMLNodeChild(array);
            while (entry) {
                if (KON_CompareXMLNodeName(entry, "src")) {                    
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].SrcRect);
                } else if (KON_CompareXMLNodeName(entry, "dst")) {
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].DstRect);
                }
                entry = KON_GetXMLNodeSibling(entry);
            }
            ArrayID++;
        }
        array = KON_GetXMLNodeSibling(array);
    }

    return LoadingAnimation;
}

void KON_FreeAnimation(Animation* anim) {
    free(anim);
}