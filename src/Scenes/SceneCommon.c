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

#include "SceneCommon.h"
#include "Load.h"

/* Idée : Remplacer BackgroundID avec un "Background*" de cette façon les scènes gêrent elles meme leurs bg */

void FreeBGAnimation(BGAnimation* AnimationToFree){
    if (AnimationToFree->AnimStates)
        free(AnimationToFree->AnimStates);
    
    if (AnimationToFree->AnimRuntime)
        free(AnimationToFree->AnimRuntime);

    if (AnimationToFree->AnimRange)
        free(AnimationToFree->AnimRange);
}

void FreeBGAnimationArray(BGAnimation** ArrayToFree, unsigned int nbOfAnimations){
    unsigned int i;

    for (i = 0; i < nbOfAnimations; i++){
        FreeBGAnimation(&(*ArrayToFree)[i]);
    }
    free(*ArrayToFree);
}

int ParseBGAnimationArray(xmlNode* property, BGAnimation** AnimationArray){
    xmlNode *array, *entry;
    unsigned int ArrayID, EntryID;
    unsigned long nbOfAnimations;

    nbOfAnimations = xmlChildElementCount(property);
    (*AnimationArray) = NULL;
    (*AnimationArray) = (BGAnimation*)calloc(nbOfAnimations, sizeof(BGAnimation));
    if (!(*AnimationArray)){
        printf("ERROR: Not enough memory!\n");
        goto ERROR;
    }
    array = property->children;

    ArrayID = 0;
    while (array){
        if (strcmp((char*)array->name, "anim") == 0) {
            (*AnimationArray)[ArrayID].NbOfAnimStates = xmlChildElementCount(array);
            (*AnimationArray)[ArrayID].AnimRegion = InitRect(
                atoi((char*)xmlGetProp(array, (xmlChar*)"X")),
                atoi((char*)xmlGetProp(array, (xmlChar*)"Y")),
                atoi((char*)xmlGetProp(array, (xmlChar*)"W")), 
                atoi((char*)xmlGetProp(array, (xmlChar*)"H"))
            );
            
            (*AnimationArray)[ArrayID].AnimStates = (Vector2d*)malloc(sizeof(Vector2d)*(*AnimationArray)[ArrayID].NbOfAnimStates);
            (*AnimationArray)[ArrayID].AnimRange = (Vector2d*)malloc(sizeof(Vector2d)*(*AnimationArray)[ArrayID].NbOfAnimStates);
            (*AnimationArray)[ArrayID].AnimRuntime = (int*)malloc(sizeof(int)*(*AnimationArray)[ArrayID].NbOfAnimStates);
            if (!(*AnimationArray)[ArrayID].AnimStates || !(*AnimationArray)[ArrayID].AnimRange || !(*AnimationArray)[ArrayID].AnimRuntime){
                printf("ERROR: Not enough memory!\n");
                goto ERROR;
            }
            entry = array->children;
            
            EntryID = 0;
            while (entry){
                if (strcmp((char*)entry->name, "entry") == 0){
                    (*AnimationArray)[ArrayID].AnimStates[EntryID] = InitVector2d(atof((char*)xmlGetProp(entry, (xmlChar*)"stateA")), atof((char*)xmlGetProp(entry, (xmlChar*)"stateB")));
                    (*AnimationArray)[ArrayID].AnimRange[EntryID] = InitVector2d(atof((char*)xmlGetProp(entry, (xmlChar*)"rangeA")), atof((char*)xmlGetProp(entry, (xmlChar*)"rangeB")));
                    (*AnimationArray)[ArrayID].AnimRuntime[EntryID] = atoi((char*)xmlGetProp(entry, (xmlChar*)"runtime"));
                    EntryID++;
                }
                entry = entry->next;
            }
            
            ArrayID++;
        }
        array = array->next;
    }

    return nbOfAnimations;

ERROR:

    FreeBGAnimationArray(AnimationArray, nbOfAnimations);

    return 0;
}

int ParseScenesCoordinatesArray(xmlNode* property, Vector2i** CoordinatesArray){
    xmlNode* entry = NULL;
    unsigned int EntryID;
    unsigned long nbOfCoordinates;

    nbOfCoordinates = xmlChildElementCount(property);
    (*CoordinatesArray) = NULL;
    (*CoordinatesArray) = (Vector2i*)calloc(nbOfCoordinates, sizeof(Vector2i));
    if (!(*CoordinatesArray)){
        printf("ERROR: Not enough memory!\n");
        goto ERROR;
    }
    entry = property->children;

    EntryID = 0;
    while (entry){
        if (strcmp((char*)entry->name, "scene") == 0){
            (*CoordinatesArray)[EntryID] = InitVector2i(atoi((char*)xmlGetProp(entry, (xmlChar*)"X")), atoi((char*)xmlGetProp(entry, (xmlChar*)"Y")));
            EntryID++;
        }
        entry = entry->next;
    }

    return nbOfCoordinates;

ERROR:

    return 0;
}

void FreeBackground(BackgroundContext* SceneToFree){
    if (SceneToFree->Surface)
    #ifdef _SDL
        free(SceneToFree->Surface);
    #else 
        SDL_DestroyTexture(SceneToFree->Surface);
    #endif

    if (SceneToFree->Animation){
        FreeBGAnimationArray(&SceneToFree->Animation, SceneToFree->nbOfAnimations);
    }

    if (SceneToFree->ScenesCoordinates)
        free(SceneToFree->ScenesCoordinates);

    free(SceneToFree);
}

BackgroundContext* InitBackground(DisplayDevice* DDevice, char* ScenePath){
    /* Declaration */
    BackgroundContext* LoadingContext;
    xmlDoc* SceneFile;
    xmlNode *background, *property;
    char* SurfacePath, *Buffer;
    Uint32 ColorKey;

    /* Init */
    LoadingContext = (BackgroundContext*)calloc(1, sizeof(BackgroundContext));
    if (!LoadingContext){
        printf("ERROR: Not enough memory!\n");
        goto ERROR;
    }

    /* Logic */
    if (ScenePath){
        SceneFile = loadXml(ScenePath);                 /* Load the xml file in memory */
        if (!SceneFile){
            printf("ERROR: \"%s\" not found or not enough memory!\n", ScenePath);
            goto ERROR;
        }
        background = xmlDocGetRootElement(SceneFile);   /* root node */

        if ((SurfacePath = (char*)xmlGetProp(background, (xmlChar*)"texture"))){
            if ((Buffer = (char*)xmlGetProp(background, (xmlChar*)"colorKey"))){
                sscanf(Buffer, "%x", &ColorKey);
                LoadingContext->Surface = LoadSurface(SurfacePath, DDevice, ColorKey, SURFACE_KEYED);
            } else {
                LoadingContext->Surface = LoadSurface(SurfacePath, DDevice, 0x0, SURFACE_OPAQUE);
            }
        }

        /* Parsing */
        property = background->children;
        while (property){
            if (strcmp((char*)property->name, "animArray") == 0){
                LoadingContext->nbOfAnimations = ParseBGAnimationArray(property, &LoadingContext->Animation);
            } else if (strcmp((char*)property->name, "sceneArray") == 0){
                LoadingContext->nbOfCoordinates = ParseScenesCoordinatesArray(property, &LoadingContext->ScenesCoordinates);
            }
            property = property->next;
        }

        xmlFreeDoc(SceneFile);
    }

    LoadingContext->SrcRect.w = DDevice->InternalResolution.x;
    LoadingContext->SrcRect.h = DDevice->InternalResolution.y;
    LoadingContext->PlayingAnimation = -1;
    LoadingContext->Shown = true;

    return LoadingContext;

ERROR:

    if (LoadingContext)
        FreeBackground(LoadingContext);

    return NULL;
}

void MoveBackground(BackgroundContext* Context, int TileID, char Effect){ /* Change the background tile */
    if (TileID >= 0){
        Context->Shown = true;
        Context->SrcRect.x = Context->ScenesCoordinates[TileID].x;
        Context->SrcRect.y = Context->ScenesCoordinates[TileID].y;
        Context->ObjectLayerOffset = 0;

        /* Effects */
        switch (Effect){
            case 0:
                Context->Flipped = 0;
                break;
            case 1:
                Context->Flipped = 1;
                break;
        }
    } else {
        Context->Shown = false;
    }
}

void BackgroundPlayAnimation(BackgroundContext* Context, int AnimationID, bool* EndSignal, bool EndSignalValue){ /* Start the background animation */
    Context->PlayingAnimation = AnimationID;
    Context->StartFrame = Context->CurrentState = Context->AnimOffset = 0;
    Context->ObjectLayerOffset = 0;
    (*EndSignal) = !EndSignalValue;
    Context->EndSignal = EndSignal;
    Context->EndSignalValue = EndSignalValue;
}

/* FIXME: Curently only support Horizontal motion. It would be great to be able to have vertical animations as well */
void DisplayBackground(DisplayDevice* DDevice, BackgroundContext* Context){ /* Display the background on screen */
    SDL_Rect AnimSrcRect;
    SDL_Rect BGDstRect;
    Uint32 TimeProgress;
    double Progress;
    
    double MaxProgress;
    int MaxOffset;
    int NewOffset;

    /* Only process the following if the background is actually shown*/
    if (!Context->Shown)
        return;

    BGDstRect = InitRect(0, 0, DDevice->InternalResolution.x, DDevice->InternalResolution.y);
    if ((Context->PlayingAnimation >= 0) && (Context->CurrentState < Context->Animation[Context->PlayingAnimation].NbOfAnimStates)){ /* Background slide animation */
        
        if (Context->StartFrame == 0){
            Context->StartFrame = SDL_GetTicks();
        }

        if (Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y > Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].x){
            
            TimeProgress = SDL_GetTicks() - Context->StartFrame;
            Progress = ((double)TimeProgress / (double)Context->Animation[Context->PlayingAnimation].AnimRuntime[Context->CurrentState]) * (Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y - Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].x) + Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].x;

            if (Progress > Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y){
                Progress = Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y;
            }
            
            NewOffset = (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].x * (Progress * Progress)) + (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].y * Progress);
            Context->ObjectLayerOffset = NewOffset + Context->AnimOffset;
        }else {
            
            TimeProgress = Context->Animation[Context->PlayingAnimation].AnimRuntime[Context->CurrentState] - (SDL_GetTicks() - Context->StartFrame);
            
            if (TimeProgress > Context->Animation[Context->PlayingAnimation].AnimRuntime[Context->CurrentState]){
                TimeProgress = 0;
            }
            Progress = ((double)TimeProgress / (double)Context->Animation[Context->PlayingAnimation].AnimRuntime[Context->CurrentState]) * (Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].x - Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y) + Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y;

            if (Progress < Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y){
                Progress = Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y;
            }

            MaxProgress = Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].x;
            MaxOffset = (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].x * (MaxProgress * MaxProgress)) + (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].y * MaxProgress);/* + Context->AnimOffset; */
            
            NewOffset = (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].x * (Progress * Progress)) + (Context->Animation[Context->PlayingAnimation].AnimStates[Context->CurrentState].y * Progress);
            Context->ObjectLayerOffset = MaxOffset - NewOffset + Context->AnimOffset;
        }
        AnimSrcRect.x =  Context->ObjectLayerOffset + Context->Animation[Context->PlayingAnimation].AnimRegion.x; /* + Offset */

        AnimSrcRect.y = Context->Animation[Context->PlayingAnimation].AnimRegion.y;
        AnimSrcRect.w = DDevice->InternalResolution.x;
        AnimSrcRect.h = DDevice->InternalResolution.y;

        /* Effects */
        /* Not used for now :3 */
        /*
        switch (Context->Animation[Context->PlayingAnimation].AnimEffects[Context->CurrentState]){
            case 0:
                Context->Flipped = 0;
                Context->SrcRect = AnimSrcRect;
                break;
            case 1:
                Context->Flipped = 1;
                Context->SrcRect = AnimSrcRect;
                break;
        }
        */

        Context->SrcRect = AnimSrcRect; /* Used to sync up to other layers */

        ScaledDraw(DDevice, Context->Surface, &AnimSrcRect, &BGDstRect);
        
        if (Progress == Context->Animation[Context->PlayingAnimation].AnimRange[Context->CurrentState].y){
            Context->CurrentState++;
            Context->StartFrame = 0;
            /*Context->AnimOffset = AnimSrcRect.x; */
            Context->AnimOffset = Context->ObjectLayerOffset;
            if (Context->CurrentState == Context->Animation[Context->PlayingAnimation].NbOfAnimStates){
                /*Context->SrcRect = AnimSrcRect; */
                Context->SrcRect.x = Context->Animation[Context->PlayingAnimation].AnimRegion.w;
                Context->SrcRect.y = Context->Animation[Context->PlayingAnimation].AnimRegion.h;
                Context->PlayingAnimation = -1;
                if (Context->EndSignal){
                    *(Context->EndSignal) = Context->EndSignalValue;
                }
            }
        }

    }else{ /* Display the current background tile if not animated */
        ScaledDraw(DDevice, Context->Surface, &Context->SrcRect, &BGDstRect);
    }
}

xmlNode* searchNodeLabel(xmlNode* entry, char* label){
    xmlNode* root;
    char* checkLabel;
    root = entry->parent->children;
    if (!label)
        return NULL;
    while (root){
        checkLabel = (char*)xmlGetProp(root, (xmlChar*)"label");
        if (checkLabel){
            if (strcmp(checkLabel, label) == 0){
                return root;
            }
        }
        root = root->next;
    }
    return NULL;
}

xmlNode* searchNode(xmlNode* entry, char* name){
    if (entry->parent)
        entry = entry->parent->children;
    while (entry){
        if (strcmp((char*)entry->name, name) == 0){
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}
/*
Characters** initCharArray(DisplayDevice* DDevice, xmlNode* array){
    Characters** CharactersIndex;
    unsigned int ArrayID;

    CharactersIndex = (Characters**)malloc(xmlChildElementCount(array)*sizeof(Characters*));
    array = array->children;
    
    ArrayID = 0;
    while (array){
        if (strcmp((char*)array->name, "init") == 0) {
            CharactersIndex[ArrayID] = InitCharacter(DDevice, (char*)xmlGetProp(array, (xmlChar*)"path"));
            ArrayID++;
        }
        array = array->next;
    }

    return CharactersIndex;
}

void FreeScene(SceneContext* SceneContext){
}

void SceneForward(SceneContext* SContext){
    if (SContext->Jump) {
        SContext->Jump = false;
    } else {
        SContext->entry = SContext->next; 
    }
}


void SceneBackward(SceneContext* SContext){
    if (SContext->entry->prev)
        SContext->entry = SContext->entry->prev;
}
*/