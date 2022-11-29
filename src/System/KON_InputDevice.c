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

#include "KON_InputDevice.h"

#include "Koneko.h"
#include "API.h"
#include "LinkedList.h"
#include "KON_FIFO.h"

#include "Bool.h"

typedef struct {
    #ifdef GEKKO

    #else
        /* Events */
        bool EventEnabled;

        /* Joystick */
        SDL_Joystick* Joy1; /* Pointers to the Joypad */
        bool JoyEnabled;
    #endif
} KON_InputInterface;

typedef struct {
    KON_BindingType type;
    unsigned int binding;
} KON_Binding;

typedef struct {
    unsigned int actionID;
    KON_InputState state;
    KON_BindingType bindingType;
    LinkedList* bindings;
} KON_Action;

const static KON_Event eventNone;
static KON_InputInterface inInt;

static LinkedList* userActions = NULL;
static KON_FIFO* eventQueue = NULL;

#define KON_EVENT_QUEUE_SIZE 256

void KON_InitInputs(void) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        inInt.Joy1 = NULL;
        if (SDL_NumJoysticks()) inInt.Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
        
        Koneko.iDevice.KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
        inInt.JoyEnabled = inInt.Joy1 != 0;
        inInt.EventEnabled = true;
    #endif

    eventQueue = KON_CreateFIFO(KON_EVENT_QUEUE_SIZE, sizeof(KON_Event));
}

void KON_FreeInputDevice(void) {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (inInt.Joy1)
            SDL_JoystickClose(inInt.Joy1);
    #endif

    KON_FreeFIFO(&eventQueue);
    KON_FreeUserActions();
}

static LinkedList** KON_SearchActionNode(unsigned int actionID) {
    LinkedList** searchList = &userActions;

    while (searchList) {
        if (((KON_Action*)((*searchList)->data))->actionID == actionID)
            return searchList;

        searchList = &(*searchList)->next;
    }

    return NULL;
}

static bool KON_PollKeyBinding(KON_Keyboard binding) {
    return (Koneko.iDevice.KeyStates[binding]);
}

static bool KON_PollMouseButtonBinding(KON_Mouse binding, Vector2i* mousePos, Vector2i* mouseMvt) {
    *mousePos = KON_ScreenToRenderCoordinate(Koneko.iDevice.mousePos);
    *mouseMvt = Koneko.iDevice.mouseMvt;
    return (Koneko.iDevice.mouseState & binding);
}
static bool KON_PollMouseAxisBinding(KON_MouseAxis binding, Vector2i* mousePos, Vector2i* mouseMvt) {
    uint8_t mvtMask = 0;
    
    *mousePos = KON_ScreenToRenderCoordinate(Koneko.iDevice.mousePos);
    *mouseMvt = Koneko.iDevice.mouseMvt;

    if (Koneko.iDevice.mouseMvt.x)
        mvtMask |= KON_MOUSE_AXIS_X;
    if (Koneko.iDevice.mouseMvt.y)
        mvtMask |= KON_MOUSE_AXIS_Y;

    return binding & mvtMask;
}

static bool KON_PollActionRef(KON_Action* action, struct KON_EventAction* eventAction) {
    LinkedList* bindingList = NULL;
    bool matchFound = false;
    register KON_Binding* binding = NULL;

    bindingList = action->bindings;

    while (bindingList) {
        binding = (KON_Binding*)bindingList->data;

        switch (binding->type) {
            case KON_BINDING_KEY:
                if (KON_PollKeyBinding(binding->binding)) {
                    action->bindingType = KON_BINDING_KEY;
                    matchFound = true;
                }
                break;

            case KON_BINDING_MOUSE_BUTTON:
                if (KON_PollMouseButtonBinding(binding->binding, &eventAction->data.mouse.pos, &eventAction->data.mouse.mvt)) {
                    action->bindingType = KON_BINDING_MOUSE_BUTTON;
                    matchFound = true;
                }
                break;

            case KON_BINDING_MOUSE_AXIS:
                if (KON_PollMouseAxisBinding(binding->binding, &eventAction->data.mouse.pos, &eventAction->data.mouse.mvt)) {
                    action->bindingType = KON_BINDING_MOUSE_AXIS;
                    matchFound = true;
                }
                break;

            default:
                break;
        }

        bindingList = bindingList->next;
    }

    eventAction->bindingType = action->bindingType;
    if (matchFound) {
        switch (action->state) {
        case KON_INPUT_IDLE:
        case KON_INPUT_RELEASED:
            action->state = KON_INPUT_PRESSED;
            break;

        case KON_INPUT_PRESSED:
            action->state = KON_INPUT_HELD;
            break;
        
        case KON_INPUT_HELD:
            break;
        }
        return true;
    } else {
        switch (action->state) {
        case KON_INPUT_IDLE:
            break;

        case KON_INPUT_RELEASED:
            action->state = KON_INPUT_IDLE;
            return true;

        case KON_INPUT_HELD:
        case KON_INPUT_PRESSED:
            action->state = KON_INPUT_RELEASED;
            return true;
        }
    }

    return false;
}

static void KON_PollAllActions(void) {
    LinkedList* actionList = userActions;
    KON_Action* action = NULL;
    KON_Event newEvent;

    while (actionList) {
        action = (KON_Action*)actionList->data;
        newEvent = eventNone;

        if (KON_PollActionRef(action, &newEvent.action)) {
            newEvent.type = KON_EVENT_ACTION;
            newEvent.action.actionID = action->actionID;
            newEvent.action.state = action->state;
            KON_FIFOPush(eventQueue, &newEvent);
        }
        
        actionList = actionList->next;
    }
}

void KON_PumpEvent(void) {
    KON_Event newEvent;
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_Event sdlEvent;
        Koneko.iDevice.mouseState = SDL_GetMouseState(&Koneko.iDevice.mousePos.x, &Koneko.iDevice.mousePos.y);
        SDL_GetRelativeMouseState(&Koneko.iDevice.mouseMvt.x, &Koneko.iDevice.mouseMvt.y);

        while (SDL_PollEvent(&sdlEvent)) {
            newEvent = eventNone;

            switch (sdlEvent.type) {
                case SDL_WINDOWEVENT:
                    switch (sdlEvent.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            newEvent.type = KON_EVENT_GAME_EXIT;
                            break;

                        case SDL_WINDOWEVENT_RESIZED:
                            newEvent.type = KON_EVENT_RESOLUTION_CHANGED;
                            newEvent.res.width = sdlEvent.window.data1;
                            newEvent.res.height = sdlEvent.window.data2;
                            break;
                    }
                    break;

                /*
                case SDL_KEYDOWN:
                    if (!dupEvent && KON_GetKeyActionEvent(&Koneko.iDevice.event)) {
                        dupEvent = true;
                        return true;
                    }
                    Koneko.iDevice.event.type = KON_EVENT_KEY_DOWN;
                    dupEvent = false;
                    return true;

                case SDL_KEYUP:
                    if (!dupEvent && KON_GetKeyActionEvent(&Koneko.iDevice.event)) {
                        dupEvent = true;
                        return true;
                    }
                    Koneko.iDevice.event.type = KON_EVENT_KEY_UP;
                    dupEvent = false;
                    return true;
                */
                
                default:
                    continue;
            }

            KON_FIFOPush(eventQueue, &newEvent);
        }
    #endif

    KON_PollAllActions();
}

bool KON_PollEvent(void) {
    if (KON_FIFOPop(eventQueue, &Koneko.iDevice.event))
        return true;
    Koneko.iDevice.event = eventNone;
    return false;
}

void KON_RegisterAction(unsigned int actionID, KON_BindingType type, unsigned int binding) {
    KON_Action newAction;
    KON_Binding newBinding;

    newAction.actionID = actionID;
    newAction.state = KON_INPUT_IDLE;
    newAction.bindings = NULL;

    newBinding.type = type;
    newBinding.binding = binding;

    KON_AppendToLinkedList(&newAction.bindings, &newBinding, sizeof(KON_Binding));
    KON_AppendToLinkedList(&userActions, &newAction, sizeof(KON_Action));
}

void KON_DeleteAction(unsigned int actionID) {
    LinkedList** action = KON_SearchActionNode(actionID);

    KON_FreeLinkedList(&((KON_Action*)(*action)->data)->bindings);
    KON_DeleteLinkedListNode(action);
}

void KON_AddActionBinding(unsigned int actionID, KON_BindingType type, unsigned int binding) {
    LinkedList** action;
    KON_Binding newBinding;

    action = KON_SearchActionNode(actionID);

    newBinding.type = type;
    newBinding.binding = binding;

    KON_AppendToLinkedList(&((KON_Action*)(*action)->data)->bindings, &newBinding, sizeof(KON_Binding));
}

void KON_FreeUserActions(void) {
    LinkedList** actions = &userActions;

    while (*actions) {
        KON_FreeLinkedList(&((KON_Action*)(*actions)->data)->bindings);
        actions = &(*actions)->next;
    }
    
    KON_FreeLinkedList(&userActions);
}

bool KON_PollAction(unsigned int actionID, struct KON_EventAction* eventAction) {
    LinkedList** action = NULL;

    if (!(action = KON_SearchActionNode(actionID))) {
        KON_SystemMsg("(KON_PollAction) No Such Action", MESSAGE_WARNING, 0);
        return false;
    }

    return KON_PollActionRef((KON_Action*)(*action)->data, eventAction);
}
