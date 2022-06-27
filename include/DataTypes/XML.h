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

#ifndef _XML_H
#define _XML_H

    #include "Rect.h"

    #ifdef _XBOX
        #include <libxml/parser.h>
    #elif defined(GEKKO)
        /* Temporary until we port libxml2 over to the gamecube */
        typedef void xmlDoc;
        typedef void xmlNode;
        typedef void xmlChar;
    #else
        #include <libxml2/libxml/parser.h>
    #endif

    /*
        SUMMARY : Loads a XML file from disk.
        INPUT   : char* filePath : The path to the xml file to load.
        OUTPUT  : xmlDoc*        : The newly loaded xml file (or NULL on error).
    */
    xmlDoc* KON_LoadXml(char* filePath);
    
    /*
        SUMMARY : Parses a KON_Rect from a xml node.
        INPUT   : xmlNode* node  : The node where the rect should be parsed from.
        OUTPUT  : KON_Rect* rect : The resulting parsed rect.
    */
    void    KON_LoadRectFromXmlNode(xmlNode* node, KON_Rect* rect);

#endif