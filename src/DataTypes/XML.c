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

#include "XML.h"

xmlDoc* KON_LoadXml(char* filePath){
    xmlKeepBlanksDefault(0); /* Ignore white space */

    #ifdef _XBOX
        return xmlParseFile(filePath); /* Load File into memory */
    #else
        return xmlReadFile(filePath, NULL, 0); /* Load File into memory */
    #endif
}

void KON_LoadRectFromXmlNode(xmlNode* node, SDL_Rect* rect) {
    *rect = KON_InitRect(
        atoi((char*)xmlGetProp(node, (xmlChar*)"X")),
        atoi((char*)xmlGetProp(node, (xmlChar*)"Y")),
        atoi((char*)xmlGetProp(node, (xmlChar*)"W")),
        atoi((char*)xmlGetProp(node, (xmlChar*)"H"))
    );
}