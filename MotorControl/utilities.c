#include "utilities.h"

volatile mode_t mode;

mode_t get_mode(void) {
    return mode;
}

mode_t set_mode(mode_t newMode) {
    mode = newMode;
    return mode; 
}

const char* mode_t_2_str(mode_t mode) {
    if (mode == IDLE) {
        return "IDLE";
    }
    else if (mode == PWM) {
        return "PWM";
    }
    else if (mode == ITEST) {
        return "ITEST";
    }
    else if (mode == HOLD) {
        return "HOLD";
    }
    else if (mode == TRACK) {
        return "TRACK";
    }
}