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

#include "Font.h"
#include "Graphics.h"
#include "RessourceManager.h"
#include "Log.h"

/*
    SUMMARY : Loads an unmanagedBitmap font from disk.
    INPUT   : char* filePath         : The path of the font to load.
    INPUT   : uint32_t fontColorKey  : The color key the font should be keyed with.
    OUTPUT  : BitmapFont*            : The newly loaded font (or NULL on error).
*/
static BitmapFont* KON_LoadRawBitmapFont(char* filePath, uint32_t fontColorKey) {
    BitmapFont* LoadingFont;
    KON_CPUSurface* LoadingSurface;
    unsigned int FontPixX = 1, FontPixY;
    unsigned int LetterIndex = 0;
    unsigned int FontTexSizeW, FontTexSizeH;
    size_t pitch;

    /* Load font surface*/
    LoadingFont = (BitmapFont*)malloc(sizeof(BitmapFont));
    LoadingSurface = KON_LoadCpuSurface(filePath, fontColorKey, SURFACE_KEYED);
    if (!LoadingSurface) {
        KON_SystemMsg("(KON_LoadBitmapFont) Can't load font : ", MESSAGE_ERROR, 2, filePath, SDL_GetError());
        return NULL;
    }
    
    KON_GetCPUSurfaceSize(LoadingSurface, &pitch, &FontTexSizeW, &FontTexSizeH);

    /* Fill-in rects for each letter */
    KON_LockCPUSurface(LoadingSurface);
    while ((FontPixX < FontTexSizeW) && (LetterIndex < 95)){

        /* Get letter coordinates */
        FontPixY = 0;
        LoadingFont->Rects[LetterIndex].x = FontPixX;
        LoadingFont->Rects[LetterIndex].y = 1;

        /* Get letter width */
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixX < FontTexSizeW)){
            FontPixX++;
        }

        LoadingFont->Rects[LetterIndex].w = FontPixX - LoadingFont->Rects[LetterIndex].x;
        
        /* Get letter height */
        FontPixY = 1;
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixY < FontTexSizeH)){
            FontPixY++;
        }
        LoadingFont->Rects[LetterIndex].h = FontPixY - LoadingFont->Rects[LetterIndex].y;

        FontPixX++;
        LetterIndex++;
    }
    KON_UnlockCPUSurface(LoadingSurface);

    LoadingFont->FontSurface = KON_CpuToGpuSurface(LoadingSurface);
    KON_FreeCPUSurface(LoadingSurface);

    return LoadingFont;
}

/*
    SUMMARY : Loads an unmanagedBitmap font from disk.
    INPUT   : char* filePath         : The path of the font to load.
    INPUT   : uint32_t fontColorKey  : The color key the font should be keyed with.
    OUTPUT  : BitmapFont*            : The newly loaded font (or NULL on error).
*/
BitmapFont* KON_LoadBitmapFontFromMem(BITMAP* bitmap, uint32_t fontColorKey) {
    BitmapFont* LoadingFont;
    KON_CPUSurface* LoadingSurface;
    unsigned int FontPixX = 1, FontPixY;
    unsigned int LetterIndex = 0;

    /* Load font surface*/
    LoadingFont = (BitmapFont*)malloc(sizeof(BitmapFont));
    LoadingSurface = KON_LoadCPUSurfaceFromMem(bitmap, fontColorKey, SURFACE_KEYED);
    if (!LoadingSurface) {
        KON_SystemMsg("(KON_LoadBitmapFont) Can't load font from memory", MESSAGE_ERROR, 1, SDL_GetError());
        return NULL;
    }

    /* Fill-in rects for each letter */
    KON_LockCPUSurface(LoadingSurface);
    while ((FontPixX < bitmap->width) && (LetterIndex < 95)){

        /* Get letter coordinates */
        FontPixY = 0;
        LoadingFont->Rects[LetterIndex].x = FontPixX;
        LoadingFont->Rects[LetterIndex].y = 1;

        /* Get letter width */
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixX < bitmap->width)){
            FontPixX++;
        }

        LoadingFont->Rects[LetterIndex].w = FontPixX - LoadingFont->Rects[LetterIndex].x;
        
        /* Get letter height */
        FontPixY = 1;
        while ((getpixel(LoadingSurface, FontPixX, FontPixY) != 0x0) && (FontPixY < bitmap->height)){
            FontPixY++;
        }
        LoadingFont->Rects[LetterIndex].h = FontPixY - LoadingFont->Rects[LetterIndex].y;

        FontPixX++;
        LetterIndex++;
    }
    KON_UnlockCPUSurface(LoadingSurface);

    LoadingFont->FontSurface = KON_CpuToGpuSurface(LoadingSurface);
    KON_FreeCPUSurface(LoadingSurface);

    return LoadingFont;
}

static void KON_FreeRawBitmapFont(BitmapFont* font) {
    if (!font)
        return;
    
    KON_FreeSurface(font->FontSurface);
    free(font);
}

BitmapFont* KON_LoadBitmapFont(char* FilePath, uint32_t FontColorKey) {
    BitmapFont* loadedFont; 

    if (!FilePath) {
        KON_SystemMsg("(KON_LoadBitmapFont) No filePath: ", MESSAGE_ERROR, 0);
        return NULL;
    }
    if ((loadedFont = KON_GetManagedRessource(FilePath, RESSOURCE_FONT)))
        return loadedFont;
    if (!(loadedFont = KON_LoadRawBitmapFont(FilePath, FontColorKey))) {
        KON_SystemMsg("(KON_LoadBitmapFont) Failed to load font : ", MESSAGE_ERROR, 1, FilePath);
        return NULL;
    }

    KON_AddManagedRessource(FilePath, RESSOURCE_FONT, loadedFont);

    KON_SystemMsg("(KON_LoadBitmapFont) Loaded new font : ", MESSAGE_LOG, 1, FilePath);

    return loadedFont;
}

void KON_FreeBitmapFont(BitmapFont* font) {
    if (!font)
        return;

    KON_FreeRawBitmapFont(KON_FreeManagedRessourceByRef(font));
}