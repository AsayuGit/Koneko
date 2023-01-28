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

#include "KON_BKD_XML.h"

#include "Log.h"
#include <stdlib.h>

#ifdef _XBOX
    #include <libxml/parser.h>
#else
    #include <libxml2/libxml/parser.h>
#endif

typedef xmlNode KON_XMLNode;

KON_XMLDoc* KON_BKD_LoadXMLDoc(const char* filePath) {
    #ifdef _XBOX
        return xmlParseFile(filePath); /* Load File into memory */
    #else
        return xmlReadFile(filePath, NULL, 0); /* Load File into memory */
    #endif
}

void KON_BKD_FreeXMLDoc(KON_XMLDoc* doc) {
    xmlFreeDoc(doc);
}

const char* KON_BKD_GetXMLNodeName(KON_XMLNode* node) {
    return node->name;
}

KON_XMLNode* KON_BKD_GetXMLNodeChild(KON_XMLNode* node) {
    return node->children;
}

KON_XMLNode* KON_BKD_GetXMLNodeSibling(KON_XMLNode* node) {
    return node->next;
}

const char* KON_BKD_GetXMLAttribute(KON_XMLNode* node, const char* attribute) {
    return (char*)xmlGetProp(node, (xmlChar*)attribute);
}

unsigned int KON_BKD_GetXMLNodeChildCount(KON_XMLNode* node) {
    return xmlChildElementCount(node);
}

const char* KON_BKD_GetXMLNodeText(KON_XMLNode* node) {
    return (const char*)xmlNodeGetContent(node);
}