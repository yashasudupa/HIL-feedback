/** @file drv8827.h
*
* @brief This driver module is responsible for driving the vibration motor.
*
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

/**
 * @brief This function is used to set the PWM signal at the required duty cycle and frequency
 * @param slice_num
 * @param duty_cycle
 * @param delay
 * 
 */
static void change_pwm_signal_pattern(const uint slice_num, const uint16_t duty_cycle, uint32_t delay);

/**
 * @brief This function is used to trigger the vibration sequence
 *
 */
int8_t process_washing_vibration();

/**
 * @brief This function triggers the vibration as per business logic
 *
 */
int8_t process_vibration_sequences();

/**
 * @brief This function triggers the vibration as per business logic
 * @param freq
 * 
 */
int8_t process_washing_vibration_input(uint16_t freq);

/**
 * @brief Private helper function for vibration module
 * @param slice_num
 * @param wrap_value
 * @param duty_cycles
 * @param delays
 * @param sequence_length
 */
static int8_t execute_vibration_sequence(uint slice_num, uint16_t wrap_value, const uint16_t *duty_cycles, const uint32_t *delays, size_t sequence_length)

#endif /* DRV8827_H */

/*** end of file ***/