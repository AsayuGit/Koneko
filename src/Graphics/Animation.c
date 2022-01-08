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

#include "Animation.h"
#include "XML.h"
#include <string.h>

Animation* KON_ParseAnimation(xmlNode* array){
    Animation* LoadingAnimation;
    xmlNode *entry;
    unsigned int ArrayID;

    LoadingAnimation = (Animation*)malloc(xmlChildElementCount(array)*sizeof(Animation));
    array = array->children;

    ArrayID = 0;
    while (array){
        if (strcmp((char*)array->name, "anim") == 0) {

            LoadingAnimation[ArrayID].NbOfFrames = atoi((char*)xmlGetProp(array, (xmlChar*)"nbOfFrames"));
            LoadingAnimation[ArrayID].Framerate = atoi((char*)xmlGetProp(array, (xmlChar*)"framerate"));

            entry = array->children;
            while (entry){
                if (strcmp((char*)entry->name, "src") == 0){                    
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].SrcRect);
                } else if (strcmp((char*)entry->name, "dst") == 0){
                    KON_LoadRectFromXmlNode(entry, &LoadingAnimation[ArrayID].DstRect);
                }
                entry = entry->next;
            }
            ArrayID++;
        }
        array = array->next;
    }

    return LoadingAnimation;
}

void KON_FreeAnimation(Animation* anim){
    free(anim);
}