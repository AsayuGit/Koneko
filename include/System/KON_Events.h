#ifndef KON_EVENTS_H
#define KON_EVENTS_H

    #include <stdint.h>
    #include <Vector.h>

    typedef enum{
        KON_INPUT_IDLE,
        KON_INPUT_PRESSED,
        KON_INPUT_HELD,
        KON_INPUT_RELEASED
    } KON_InputState;

    enum {
        KON_EVENT_NONE = 0,
        KON_EVENT_GAME_EXIT,
        KON_EVENT_RESOLUTION_CHANGED,
        KON_EVENT_KEY_DOWN,
        KON_EVENT_KEY_UP,
        KON_EVENT_ACTION
    };

    struct KON_EventResChanged {
        uint8_t type;
        unsigned int width;
        unsigned int height;
    };

    struct KON_EventJoystick {
        uint8_t type;
        int8_t X;
        int8_t Y;
    };

    typedef union {
        struct KON_MouseActionData {
            Vector2i pos;
            Vector2i mvt;
        } mouse;
    } KON_ActionData;

    struct KON_EventAction {
        uint8_t type;
        unsigned int actionID;
        KON_InputState state;
        KON_ActionData data;
    };

    typedef union {
        uint8_t type;
        struct KON_EventResChanged res;
        struct KON_EventJoystick joy;
        struct KON_EventAction action;
    } KON_Event;

#endif