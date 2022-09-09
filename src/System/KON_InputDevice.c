#include "KON_InputDevice.h"

#include "Koneko.h"
#include "API.h"
#include "LinkedList.h"
#include "KON_FIFO.h"

#include <stdbool.h>

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
    LinkedList* bindings;
} KON_Action;

const static KON_Event eventNone;
static KON_InputInterface inInt;
static LinkedList* userActions = NULL;
static KON_FIFO* eventQueue = NULL;

#define KON_EVENT_QUEUE_SIZE 256

void KON_InitInputs() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        inInt.Joy1 = NULL;
        if (SDL_NumJoysticks())
        inInt.Joy1 = SDL_JoystickOpen(0); /* Open Joystick */
        
        Koneko.iDevice.KeyStates = SDL_GetKeyboardState(NULL); /* Open Keyboard */
        inInt.JoyEnabled = (bool)(inInt.Joy1);
        inInt.EventEnabled = true;
    #endif

    eventQueue = KON_CreateFIFO(KON_EVENT_QUEUE_SIZE, sizeof(KON_Event));
}

void KON_FreeInputDevice() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        if (inInt.Joy1)
            SDL_JoystickClose(inInt.Joy1);
    #endif
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

void KON_PumpEvent() {
    KON_Event newEvent;
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        SDL_Event sdlEvent;

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
}

bool KON_PollEvent() {
    if (KON_FIFOPop(eventQueue, &Koneko.iDevice.event))
        return true;
    Koneko.iDevice.event = eventNone;
    return false;
}

void KON_RegisterAction(unsigned int actionID, KON_BindingType type, unsigned int binding) {
    KON_Action newAction;
    KON_Binding newBinding;

    newAction.actionID = actionID;
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

static void KON_FreeUserActions() {
    LinkedList** actions = &userActions;

    while (*actions) {
        KON_FreeLinkedList(&((KON_Action*)(*actions)->data)->bindings);
        actions = &(*actions)->next;
    }
    
    KON_FreeLinkedList(&userActions);
}

static bool KON_PollKeyBinding(KON_Keyboard binding) {
    return (Koneko.iDevice.KeyStates[binding]);
}

bool KON_PollAction(unsigned int actionID) {
    LinkedList** action = NULL;
    LinkedList* bindingList = NULL;
    register KON_Binding* binding = NULL;

    if (!(action = KON_SearchActionNode(actionID))) {
        KON_SystemMsg("(KON_PollAction) No Such Action", MESSAGE_WARNING, 0);
        return false;
    }
    bindingList = ((KON_Action*)(*action)->data)->bindings;

    while (bindingList) {
        binding = (KON_Binding*)bindingList->data;

        switch (binding->type) {
            case KON_BINDING_KEY:
                if (KON_PollKeyBinding(binding->binding))
                    return true;
                break;
            default:
                break;
        }

        bindingList = bindingList->next;
    }

    return false;
}

static void KON_PollAllActions() {
    
}