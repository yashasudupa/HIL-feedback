/** @file drv8827.h
*
* @brief This driver module is responsible for driving the vibration motor.
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Achira Labs Pvt Ltd. All rights reserved.
*/

#ifndef DRV8827_H
#define DRV8827_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "gpio_control.h"

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0

// Debug print macro
#if DEBUG_PRINT_ENABLED
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

// Function prototypes
static void change_pwm_signal_pattern(const uint slice_num, const uint16_t duty_cycle, uint32_t delay);
int8_t process_washing_vibration();

#endif /* DRV8827_H */

/*** end of file ***/