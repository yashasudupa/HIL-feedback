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

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0  // Use 0 or 1 for easier toggling

// Debug print macro
#if DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do {} while(0)  // More efficient than ((void)0)
#endif

// Extern declarations for global variables
extern char data_str[];
extern atomic_bool uart_ix_flag;
extern atomic_bool uart_k_flag;

// Function prototypes
static void core1_entry(void);
static int process_state_machine(const char *data_str);

// Firmware version string, made const for read-only access
static const char firmware_version[] = "Bv2.7.2\n";

#endif // _RP1_APPLICATION

/*** end of file ***/