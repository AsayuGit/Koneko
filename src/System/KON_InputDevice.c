#include "KON_InputDevice.h"

#include "Koneko.h"
#include "API.h"
#include "LinkedList.h"

#include <stdbool.h>

typedef struct {
    #ifdef GEKKO

    #else
        /* Events */
        SDL_Event event;
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

static KON_InputInterface inInt;
static LinkedList* userActions = NULL;
static bool dupEvent = false;

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

bool KON_PollEvent() {
    #ifdef GEKKO
        /* TODO: implement libogc */
    #else
        while (true) {
            if (!dupEvent && !SDL_PollEvent(&inInt.event))
                break;

            switch (inInt.event.type) {
                case SDL_WINDOWEVENT:
                    switch (inInt.event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            Koneko.iDevice.event.type = KON_EVENT_GAME_EXIT;
                            return true;

                        case SDL_WINDOWEVENT_RESIZED:
                            Koneko.iDevice.event.type = KON_EVENT_RESOLUTION_CHANGED;
                            Koneko.iDevice.event.res.width = inInt.event.window.data1;
                            Koneko.iDevice.event.res.height = inInt.event.window.data2;
                            return true;
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
                    break;
            }
        }

        Koneko.iDevice.event.type = KON_EVENT_NONE;
        return false;
    #endif
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