#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum {
    IDLE, // 0
    PWM, // 1
    ITEST, // 2
    HOLD, // 3
    TRACK, // 4
} mode_t;

mode_t get_mode(void);
mode_t set_mode(mode_t newMode);
const char* mode_t_2_str(mode_t mode);

#endif // UTILITIES_H