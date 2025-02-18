#ifndef _RP1_APPLICATION
#define _RP1_APPLICATION

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "gpio_control.h"
#include "drv8825.h"
#include "drv8827.h"
#include "test.h"
#include "pico/sync.h"  // For atomic operations
#include "uart_driver.h"

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0  // Use 0 or 1 for easier toggling

// Debug print macro
#if DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do {} while(0)  // More efficient than ((void)0)
#endif

// Extern declarations for global variables
// main UART Rx data struct variable
extern uartRxData_t mainUartStruct = {};
extern atomic_bool uart_ix_flag;
extern atomic_bool uart_k_flag;
// Firmware version string, made const for read-only access
static const char firmware_version[] = "Bv2.7.2\n";

/**
 * @brief This function executes in core 1
 *
 */
static void core1_entry(void);

/**
 * @brief This function processes the operational state machines
 * 
 * @param data_str       - state machines
 *
 */
static int process_state_machine(const char *data_str);

/**
 * @brief This function sends back the general feedback for rp1
 * 
 * @param rp1Response       - rp1 general response to send (rp1_feedback_response_t)
 * @param mainUartConfig    - pointer to main UART configuration
 */
void rp1_feedback(rp1_feedback_response_t rp1Response, uart_config_t *mainUartConfig);

#endif // _RP1_APPLICATION

/*** end of file ***/