// buttons.c
#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>

void buttons_init(void) {
    DDRC  &= ~0x3F;   // PC0–PC5 as inputs
    PORTC |=  0x3F;   // enable internal pull-ups
}

uint8_t buttons_read(void) {
    return (~PINC) & 0x3F;  // active low, invert so 1 = pressed
}

// ── Internal debounce helper ──────────────────────────────────
// Samples the button mask twice 50ms apart.
// Returns the mask only if both reads agree — kills bounce noise.
static uint8_t debounce(void) {
    uint8_t s1 = buttons_read();
    _delay_ms(DEBOUNCE_MS);
    uint8_t s2 = buttons_read();
    return (s1 == s2) ? s1 : 0;
}

// ── Main button logic ─────────────────────────────────────────
// Call this from your main loop. It is non-blocking for all cases
// except the SOS hold, which intentionally blocks up to SOS_HOLD_MS
// while polling — this is safe because SOS is an emergency path and
// you want the MCU dedicated to confirming that hold.
ButtonEvent buttons_check(void) {
    uint8_t pressed = debounce();

    if (pressed == 0) return BTN_EVENT_NONE;

    // ── Power button: single press ─────────────────────────
    if (pressed & BTN_ONOFF) {
        // Wait for release before returning
        while (buttons_read() & BTN_ONOFF);
        _delay_ms(DEBOUNCE_MS);
        return BTN_EVENT_POWER;
    }

    // ── SOS button: must be held for SOS_HOLD_MS ──────────
    if (pressed & BTN_SOS) {
        uint16_t held_ms = 0;

        while (held_ms < SOS_HOLD_MS) {
            _delay_ms(10);
            held_ms += 10;

            // If released early — not a valid SOS, ignore
            if (!(buttons_read() & BTN_SOS)) {
                return BTN_EVENT_NONE;
            }
        }

        // Still held after 3 seconds — valid SOS arm
        while (buttons_read() & BTN_SOS);  // wait for release
        return BTN_EVENT_SOS_ARMING;
    }

    // ── Direction buttons: single press → waypoint or nav ──
    // Wait for release before deciding which direction
    while (buttons_read() & (BTN_LEFT | BTN_RIGHT | BTN_UP | BTN_DOWN));
    _delay_ms(DEBOUNCE_MS);

    if      (pressed & BTN_LEFT)  return BTN_EVENT_DIR_LEFT;
    else if (pressed & BTN_RIGHT) return BTN_EVENT_DIR_RIGHT;
    else if (pressed & BTN_UP)    return BTN_EVENT_DIR_UP;
    else if (pressed & BTN_DOWN)  return BTN_EVENT_DIR_DOWN;

    return BTN_EVENT_NONE;
}
