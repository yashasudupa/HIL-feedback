/** @file gpio_control.h
*
* @brief This module is responsible for GPIO configurations and state machine definitions for Pico 1.
*
*/

#ifndef _GPIO_CONTROL_H
#define _GPIO_CONTROL_H

#include "hardware/watchdog.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include <string.h>
#include "pico/multicore.h"
#include "uart_driver.h"
#include <stdatomic.h>
#include <stdbool.h>

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0

// Debug print macro
#if DEBUG_PRINT_ENABLED
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

// Constants
#define MAX_SIZE 20
#define TIMEOUT 10000000

// Shaker Pins
#define M3_IN2 24  // PHOTODIODE1_INPUT
#define M3_IN1 23  // PHOTODIODE2_INPUT
#define M3_SLEEP 22  // PHOTODIODE2_INPUT

// On-Off commands
#define LOW 0
#define HIGH 1

// Encoder Channels
#define ENC_CH1 28   // Signal A
#define ENC_CH2 27   // Signal Z
#define ENC_CH3 26   // Signal B

// Iterations and Sleep Times
#define THREE_ITERATIONS 3
#define TWO_FIFTY_MILLISECONDS 250
#define FIVE_HUNDRED_MILLISECONDS 500
#define ONE_THOUSAND_MILLISECONDS 1000

// On-board LED
#define RP1_OB_LED 25  // On board LED

// Valve Motor Pins
#define M1_STEP 4    // MOTOR1_STEP
#define M1_DIR 6     // MOTOR1_DIRECTION
#define M1_NFAULT 7  // MOTOR1_FAULT    
#define M1_ENABLE 5  // MOTOR1_ENABLE
#define M1_MODE0 3   // MOTOR1_MODE0 Microstepping resolution pins
#define M1_MODE1 2   // MOTOR1_MODE1 Microstepping resolution pins
#define M1_MODE2 1   // MOTOR1_MODE2 Microstepping resolution pins
#define MOTOR_SLEEP 10  // MOTOR_SLEEP
#define MOTOR_RESET 11  // MOTOR_RESET

// UART Pins from Pi to RP1
#define RP1_IO16_UART0_TX 16  // UART0_TX_RP1
#define RP1_IO17_UART0_RX 17  // UART0_RX_RP1

// Encoder
#define ENCODER_NO_OF_PULSES 179

// Byte Definitions
#define ZERO_BYTES 0
#define ONE_BYTES 1
#define TWO_BYTES 2
#define THREE_BYTES 3
#define FOUR_BYTES 4
#define FIVE_BYTES 5
#define SIX_BYTES 6
#define SEVEN_BYTES 7
#define EIGHT_BYTES 8
#define ELEVEN_BYTES 11
#define TWENTY_BYTES 20

#define NUM_OF_UART_FUNCS 16

// Status Codes
#define VIBRATION_SUCCESSFUL 1
#define INVALID_REQUEST -1

// Enumeration for State Machines
enum DesiredFunc {
    K, V1, V2, V3, V4, V5, V6, ST, SF, IV, RS, WV, FV, MO, TS, TW
};

// Function Prototypes
enum DesiredFunc get_desired_func(const char *ptr_data_str);
void initialise_gpios(void);
int state_rotate_valve(const char *ptr_data_str);
int motor_test();
int8_t process_vibration_sequences();
int8_t process_incubation_vibration();
int8_t process_washing_vibration_input(uint16_t freq);
int test_vibration_shaker_on(const char *data_str);
int8_t reshake_sequences();
int vibration_shaker_on();
void initialisations();
void encoder_isr();
void detect_rise_in_channel_one_isr();
int reset_pico(char *ptr_data_str);
int incubation_shaker_on();
int shaker_on();
int reshake_vibrator();
int shaker_off();
void on_board_led_blink();
static void on_uart_rx();
bool report_firmware_version(const char *version);
bool turn_off_motor();
static uint64_t get_time(void);

// Removed global variable declarations; these should be managed in the implementation or passed as parameters

#endif /* GPIO_CONTROL_H */

/*** end of file ***/