#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

// Pin masks
#define BTN_ONOFF  (1 << PC0)
#define BTN_LEFT   (1 << PC1)
#define BTN_RIGHT  (1 << PC2)
#define BTN_UP     (1 << PC3)
#define BTN_DOWN   (1 << PC4)
#define BTN_SOS    (1 << PC5)

// Hold duration for SOS arm (ms)
#define SOS_HOLD_MS     3000
// Debounce window (ms)
#define DEBOUNCE_MS     50

// Return values from buttons_check()
typedef enum {
    BTN_EVENT_NONE,
    BTN_EVENT_POWER,        // BTN_ONOFF single press
    BTN_EVENT_WAYPOINT,     // any DIR button single press
    BTN_EVENT_DIR_LEFT,     // individual directions for navigation
    BTN_EVENT_DIR_RIGHT,
    BTN_EVENT_DIR_UP,
    BTN_EVENT_DIR_DOWN,
    BTN_EVENT_SOS_ARMING,   // SOS held long enough — arm it
} ButtonEvent;

void        buttons_init(void);
uint8_t     buttons_read(void);
ButtonEvent buttons_check(void);

#endif
