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

#include "XML.h"
#include "Log.h"

#include <stdlib.h>
#include <string.h>

KON_XMLDocument* KON_LoadXml(const char* filePath) {
    KON_XMLDocument* newDocument;

    if (!(newDocument = (KON_XMLDocument*)malloc(sizeof(KON_XMLDocument)))) {
        KON_SystemMsg("(KON_LoadXml) Couldn't alocate more memory", MESSAGE_ERROR, 0);
        return NULL;
    }

    if (!(newDocument->document = KON_BKD_LoadXMLDoc(filePath)))
        return NULL;
    
    if (!(newDocument->rootNode = KON_BKD_GetXMLNodeChild(newDocument->document)))
        return NULL;

    return newDocument;
}

void KON_FreeXML(KON_XMLDocument* document) {
    KON_BKD_FreeXMLDoc(document->document);
    free(document);
}

const char* KON_GetXMLNodeName(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeName(node);
}

bool KON_CompareXMLNodeName(KON_XMLNode* node, const char* name) {
    if (!node || !name) return false;
    return (strcmp(KON_GetXMLNodeName(node), name) == 0);
}

KON_XMLNode* KON_GetXMLNodeChild(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeChild(node);
}

KON_XMLNode* KON_GetXMLNodeSibling(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeSibling(node);
}

const char* KON_GetXMLAttribute(KON_XMLNode* node, const char* attribute) {
    return KON_BKD_GetXMLAttribute(node, attribute);
}

int KON_GetXMLAttributeAsInt(KON_XMLNode* node, const char* attribute) {
    const char* buffer = KON_BKD_GetXMLAttribute(node, attribute);
    return buffer ? atoi(buffer) : 0;
}

double KON_GetXMLAttributeAsDouble(KON_XMLNode* node, const char* attribute) {
    const char* buffer = KON_BKD_GetXMLAttribute(node, attribute);
    return buffer ? atof(buffer) : 0.0;
}

unsigned int KON_GetXMLNodeChildCount(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeChildCount(node);
}

unsigned int KON_GetXMLNodeCount(KON_XMLNode* node, const char* name) {
    unsigned int nodeCount = 0;
    KON_XMLNode* childNode = KON_BKD_GetXMLNodeChild(node);

    while (childNode) {
        if (KON_CompareXMLNodeName(childNode, name))
            nodeCount++;

        childNode = KON_BKD_GetXMLNodeSibling(childNode);
    }

    return nodeCount;
}

void KON_LoadRectFromXmlNode(KON_XMLNode* node, KON_Rect* rect) {
    KON_InitRect((*rect),
        atoi(KON_BKD_GetXMLAttribute(node, "X")),
        atoi(KON_BKD_GetXMLAttribute(node, "Y")),
        atoi(KON_BKD_GetXMLAttribute(node, "W")),
        atoi(KON_BKD_GetXMLAttribute(node, "H"))
    );
}

const char* KON_GetXMLNodeText(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeText(node);
}