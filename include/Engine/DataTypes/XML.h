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

    #include "Platform.h"
    #include "Rect.h"

    #ifdef LIBXML2
        #ifdef _XBOX
            #include <libxml/parser.h>
        #else
            #include <libxml2/libxml/parser.h>
        #endif

        typedef xmlNode KON_XMLNode;
    #elif defined(MXML)
        #include <mxml.h>

        typedef mxml_node_t KON_XMLNode;
    #endif

    typedef struct {
        #ifdef LIBXML2
            xmlDoc* document;
            KON_XMLNode* rootNode;
        #elif defined(MXML)
            KON_XMLNode* document;
            KON_XMLNode* rootNode;
        #endif
    } KON_XMLDocument;

    /*
        SUMMARY : Loads a XML file from disk.
        INPUT   : const char* filePath : The path to the xml file to load.
        OUTPUT  : KON_XMLDocument*     : The newly loaded xml file (or NULL on error).
    */
    KON_XMLDocument* KON_LoadXml(const char* filePath);

    /*
        SUMMARY : Free a previously loaded xml document.
        INPUT   : KON_XMLDocument* document : The xml document to free.
    */
    void KON_FreeXML(KON_XMLDocument* document);

    /*
        SUMMARY : Returns a node's name.
        INPUT   : KON_XMLNode* node : The xml node to get the name of.
        OUTPUT  : char*             : The xml node's name.
    */
    const char* KON_GetXMLNodeName(KON_XMLNode* node);

    /*
        SUMMARY : Test if the node's name is the passed name.
        INPUT   : KON_XMLNode* node : The xml node to test the name of. 
        INPUT   : cosnt char* name  : The name to test the node with.
        OUTPUT  : bool              : The test result.
    */
    bool KON_CompareXMLNodeName(KON_XMLNode* node, const char* name);

    /*
        SUMMARY : Returns a node's child node.
        INPUT   : KON_XMLNode* node : Parent node.
        OUTPUT  : KON_XMLNode*      : Child node.
    */
    KON_XMLNode* KON_GetXMLNodeChild(KON_XMLNode* node);

    /*
        SUMMARY : Returns a node's next sibling.
        INPUT   : KON_XMLNode* node : Curent node.
        OUTPUT  : KON_XMLNode*      : Next node.
    */
    KON_XMLNode* KON_GetXMLNodeSibling(KON_XMLNode* node);

    /*
        SUMMARY : Returns an attribute from an xml node.
        INPUT   : KON_XMLNode* node     : The xml node.
        INPUT   : const char* attribute : The attribute to look for.
        OUTPUT  : char*                 : The attribute value.
    */
    const char* KON_GetXMLAttribute(KON_XMLNode* node, const char* attribute);

    /*
        SUMMARY : Returns an attribute from an xml node as an int.
        INPUT   : KON_XMLNode* node     : The xml node.
        INPUT   : const char* attribute : The attribute to look for.
        OUTPUT  : int                   : The attribute value.
    */
    int         KON_GetXMLAttributeAsInt(KON_XMLNode* node, const char* attribute);

    /*
        SUMMARY : Returns an attribute from an xml node as a double.
        INPUT   : KON_XMLNode* node     : The xml node.
        INPUT   : const char* attribute : The attribute to look for.
        OUTPUT  : double                : The attribute value.
    */
    double      KON_GetXMLAttributeAsDouble(KON_XMLNode* node, const char* attribute);

    /*
        SUMMARY : Returns the number of child of an xml node.
        INPUT   : KON_XMLNode* node : The node to count the child of.
        OUTPUT  : unsigned int      : The number of child of the node.
    */
    unsigned int KON_GetXMLNodeChildCount(KON_XMLNode* node);

    /*
        SUMMARY : Returns the number of child nodes matching the provided name.
        INPUT   : KON_XMLNode* node : The node to count the child of.
        INPUT   : const char* name  : The name of the nodes to check for.
        OUTPUT  : undigned int      : the number of nodes.
    */
    unsigned int KON_GetXMLNodeCount(KON_XMLNode* node, const char* name);

    /*
        SUMMARY : Parses a KON_Rect from a xml node.
        INPUT   : KON_XMLDocument* node : The node where the rect should be parsed from.
        OUTPUT  : KON_Rect* rect        : The resulting parsed rect.
    */
    void    KON_LoadRectFromXmlNode(KON_XMLNode* node, KON_Rect* rect);

    /*
        SUMMARY : Return the text data of an XML node.
        INPUT   : KON_XMLNode* node : The node to return the text data of.
        OUTPUT  : const char*       : The text data of said node.
    */
    const char* KON_GetXMLNodeText(KON_XMLNode* node);

#endif
