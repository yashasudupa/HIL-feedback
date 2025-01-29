/** @file uart_driver.h
*
* @brief This module provides stable, lossless UART communication for Pico 1.
*
*/

#ifndef _UART_DRIVER_H
#define _UART_DRIVER_H

#include <stdlib.h>
#include <stdio.h>
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

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0

// Debug print macro
#if DEBUG_PRINT_ENABLED
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

// Constants
#define NO_ACK 0
#define ERROR_IN_RECEIVING_MSG "E"

// Time durations in microseconds
#define THIRTY_SECS (30 * 1000000)  // 30 seconds
#define TWO_SECONDS (2 * 1000000)   // 2 seconds
#define ONE_MILLISECOND (1000)      // 1 millisecond

#define BUFFER_SIZE 100

#define ERROR_IN_ACK 'O'
#define SUCCESS_IN_ACK 'S'

// UART Configuration
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define RP1_UART_NUMBER   0

#define NEWLINE '\n'
#define NULL_CHARACTER '\0'

// Test LEDs for RP1
#define TEST_PIN_ONE  0
#define TEST_PIN_TWO  2
#define TEST_PIN_THREE  3

// On board LED
#define RP1_OB_LED 25  // On board LED

// On-Off commands
#define LOW 0
#define HIGH 1

// Status Code
#define error_in_receiving_ack -1

// Function Prototypes
char uart_write_bytes(const char *data, size_t len, int uart_number); 
char read_message_uart_channel(char *data_str, uart_inst_t *uart_instance);
void clear_buffer(uart_inst_t *uart_instance);
char read_message_from_master(uart_inst_t *uart_instance, char *data_str);

#endif /* UART_DRIVER_H */

/*** end of file ***/