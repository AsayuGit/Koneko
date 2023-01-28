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
#include <stdio.h>

#include <mxml.h>

KON_XMLDoc* KON_BKD_LoadXMLDoc(const char* filePath) {
    FILE* xmlFile;
    KON_XMLNode* xmlTree;
    
    if (!(xmlFile = fopen(filePath, "r"))) {
        KON_SystemMsg("(KON_BKD_LoadXMLDoc) Couldn't open XML file: ", MESSAGE_ERROR, 1, filePath);
        return NULL;
    }

    if (!(xmlTree = mxmlLoadFile(NULL, xmlFile, MXML_OPAQUE_CALLBACK))) {
        KON_SystemMsg("(KON_BKD_LoadXMLDoc) Couldn't parse XML file: ", MESSAGE_ERROR, 1, filePath);
        fclose(xmlFile);
        return NULL;
    }
    fclose(xmlFile);

    return xmlTree;
}

void KON_BKD_FreeXMLDoc(KON_XMLDoc* doc) {
    mxmlDelete(doc);
}

const char* KON_BKD_GetXMLNodeName(KON_XMLNode* node) {
    return mxmlGetElement(node);
}

KON_XMLNode* KON_BKD_GetXMLNodeChild(KON_XMLNode* node) {
    return KON_BKD_GetXMLNodeSibling(mxmlGetFirstChild(node));
}

KON_XMLNode* KON_BKD_GetXMLNodeSibling(KON_XMLNode* node) {
    KON_XMLNode* sibling = node;
    mxml_type_t nodeType;

    do {
        sibling = mxmlGetNextSibling(sibling);
        if ((nodeType = mxmlGetType(sibling)) == MXML_IGNORE)
            return NULL;
    } while (nodeType != MXML_ELEMENT);

    return sibling;
}

const char* KON_BKD_GetXMLAttribute(KON_XMLNode* node, const char* attribute) {
    return mxmlElementGetAttr(node, attribute);
}

unsigned int KON_BKD_GetXMLNodeChildCount(KON_XMLNode* node) {
    unsigned int nbOfChilds = 0;

    node = mxmlGetFirstChild(node);
    while (node) {
        nbOfChilds++;
        node = mxmlGetNextSibling(node);
    }

    return nbOfChilds;
}

const char* KON_BKD_GetXMLNodeText(KON_XMLNode* node) {
    return mxmlGetOpaque(node);
}
