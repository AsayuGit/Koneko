#ifndef KON_EVENTS_H
#define KON_EVENTS_H

    #include <stdint.h>

    enum {
        KON_EVENT_NONE,
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

    struct KON_EventAction {
        uint8_t type;
        unsigned int actionID;
        int8_t X;
        int8_t Y;
    };

    typedef union {
        uint8_t type;
        struct KON_EventResChanged res;
        struct KON_EventJoystick joy;
        struct KON_EventAction action;
    } KON_Event;

#endif