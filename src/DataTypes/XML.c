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

#include "XML.h"

#include "Log.h"
#include <stdlib.h>

#ifdef MXML
    #include <stdio.h>
#endif

/* spriteNode = xmlDocGetRootElement(spriteXml); rootNode */
KON_XMLDocument* KON_LoadXml(const char* filePath) {
    #ifdef LIBXML2
        xmlKeepBlanksDefault(0); /* Ignore white space */

        #ifdef _XBOX
            return xmlParseFile(filePath); /* Load File into memory */
        #else
            return xmlReadFile(filePath, NULL, 0); /* Load File into memory */
        #endif
    #elif defined(MXML)
        /* TODO: Implement MXML */
        FILE* xmlFile;
        KON_XMLNode* xmlTree;
        KON_XMLDocument* loadedNode;

        if (!(xmlFile = fopen(filePath, "r"))) {
            KON_SystemMsg("(KON_LoadXml) Couldn't open XML file: ", MESSAGE_ERROR, 1, filePath);
            return NULL;
        }

        if (!(xmlTree = mxmlLoadFile(NULL, xmlFile, MXML_OPAQUE_CALLBACK))) {
            KON_SystemMsg("(KON_LoadXml) Couldn't parse XML file: ", MESSAGE_ERROR, 1, filePath);
            fclose(xmlFile);
            return NULL;
        }
        fclose(xmlFile);

        if (!(loadedNode = (KON_XMLDocument*)malloc(sizeof(KON_XMLDocument)))) {
            KON_SystemMsg("(KON_LoadXml) Couldn't alocate more memory", MESSAGE_ERROR, 0);
            return NULL;
        }

        loadedNode->document = xmlTree;
        loadedNode->rootNode = KON_GetXMLNodeChild(xmlTree);

        return loadedNode;
    #endif
}

void KON_FreeXML(KON_XMLDocument* document) {
    #ifdef LIBXML2
        xmlFreeDoc(spriteXml);
    #elif defined(MXML)
        mxmlDelete(document->document);
    #endif

    free(document);
}

const char* KON_GetXMLNodeName(KON_XMLNode* node) {
    #ifdef LIBXML2
        return node->name
    #elif defined(MXML)
        return mxmlGetElement(node);
    #endif
}

bool KON_CompareXMLNodeName(KON_XMLNode* node, const char* name) {
    if (!node || !name)
        return false;
    #ifdef LIBXML2
        return (strcmp(node->name, name) == 0);
    #elif defined(MXML)
        return (strcmp(mxmlGetElement(node), name) == 0);
    #endif
}

KON_XMLNode* KON_GetXMLNodeChild(KON_XMLNode* node) {
    #ifdef LIBXML2
        return node->children;
    #elif defined(MXML)
        return KON_GetXMLNodeSibling(mxmlGetFirstChild(node));
    #endif
}

KON_XMLNode* KON_GetXMLNodeSibling(KON_XMLNode* node) {
    #ifdef LIBXML2
        return node->next;
    #elif defined(MXML)
        KON_XMLNode* sibling = node;
        mxml_type_t nodeType;

        do {
            sibling = mxmlGetNextSibling(sibling);
            if ((nodeType = mxmlGetType(sibling)) == MXML_IGNORE)
                return NULL;
        } while (nodeType != MXML_ELEMENT);

        return sibling;
    #endif
}

const char* KON_GetXMLAttribute(KON_XMLNode* node, const char* attribute) {
    #ifdef LIBXML2
        return (char*)xmlGetProp(node, (xmlChar*)attribute);
    #elif defined(MXML)
        return mxmlElementGetAttr(node, attribute);
    #endif
}

int KON_GetXMLAttributeAsInt(KON_XMLNode* node, const char* attribute) {
    const char* buffer = KON_GetXMLAttribute(node, attribute);
    return buffer ? atoi(buffer) : 0;
}

double KON_GetXMLAttributeAsDouble(KON_XMLNode* node, const char* attribute) {
    const char* buffer = KON_GetXMLAttribute(node, attribute);
    return buffer ? atof(buffer) : 0.0;
}

unsigned int KON_GetXMLNodeChildCount(KON_XMLNode* node) {
    #ifdef LIBXML2
        return xmlChildElementCount(node);
    #elif defined(MXML)
        unsigned int nbOfChilds = 0;

        node = mxmlGetFirstChild(node);
        while (node) {
            nbOfChilds++;
            node = mxmlGetNextSibling(node);
        }

        return nbOfChilds;
    #endif
}

unsigned int KON_GetXMLNodeCount(KON_XMLNode* node, const char* name) {
    unsigned int nodeCount = 0;
    KON_XMLNode* childNode = KON_GetXMLNodeChild(node);

    while (childNode) {
        if (KON_CompareXMLNodeName(childNode, name))
            nodeCount++;

        childNode = KON_GetXMLNodeSibling(childNode);
    }

    return nodeCount;
}

void KON_LoadRectFromXmlNode(KON_XMLNode* node, KON_Rect* rect) {
    #ifdef LIBXML2
        KON_InitRect((*rect),
            atoi((char*)xmlGetProp(node, (xmlChar*)"X")),
            atoi((char*)xmlGetProp(node, (xmlChar*)"Y")),
            atoi((char*)xmlGetProp(node, (xmlChar*)"W")),
            atoi((char*)xmlGetProp(node, (xmlChar*)"H"))
        );
    #elif defined(MXML)
        KON_InitRect((*rect),
            atoi(mxmlElementGetAttr(node, "X")),
            atoi(mxmlElementGetAttr(node, "Y")),
            atoi(mxmlElementGetAttr(node, "W")),
            atoi(mxmlElementGetAttr(node, "H"))
        );
    #endif
}

const char* KON_GetXMLNodeText(KON_XMLNode* node) {
    #ifdef LIBXML2
        return (const char*)xmlNodeGetContent(node);
    #elif defined(MXML)
        return mxmlGetOpaque(node);
    #endif
}