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

#ifndef _KON_BKD_XML_H
#define _KON_BKD_XML_H

#include "Bool.h"

typedef void KON_XMLNode;
typedef void KON_XMLDoc;

typedef struct {
    KON_XMLDoc* document;
    KON_XMLNode* rootNode;
} KON_XMLDocument;

KON_XMLDoc* KON_BKD_LoadXMLDoc(const char* filePath);

void KON_BKD_FreeXMLDoc(KON_XMLDoc* doc);

const char* KON_BKD_GetXMLNodeName(KON_XMLNode* node);

KON_XMLNode* KON_BKD_GetXMLNodeChild(KON_XMLNode* node);

KON_XMLNode* KON_BKD_GetXMLNodeSibling(KON_XMLNode* node);

const char* KON_BKD_GetXMLAttribute(KON_XMLNode* node, const char* attribute);

unsigned int KON_BKD_GetXMLNodeChildCount(KON_XMLNode* node);

const char* KON_BKD_GetXMLNodeText(KON_XMLNode* node);

#endif