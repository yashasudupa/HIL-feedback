// uart_driver.c
// Achira Labs - UART Stack Implementation
// Author: Yashas Nagaraj Udupa

#include "uart_driver.h"

// Status messages of Acknowledgements
const char error_in_ack = 'O';
const char success_in_ack = 'S';
const int8_t error_in_receiving_ack = -1;

// Function to send UART message from Pico 1 to Pico 2
char uart_write_bytes(const char *data, size_t len, int uart_number) {
    uint64_t start_time = time_us_64();
    while (!uart_is_writable(UART_ID)) {
        clear_buffer(UART_ID);
        if ((time_us_64() - start_time) > TWO_SECONDS) {
            DEBUG_PRINT("Buffer is empty in RP1 \n");
            return ERROR_IN_ACK;
        }
    }
    uart_write_blocking(UART_ID, (const uint8_t *)data, len);            
    return SUCCESS_IN_ACK;
}

// Function to read one message from UART channel
char read_message_uart_channel(char *data_str, uart_inst_t *uart_instance) {
    uint64_t start_time = time_us_64();
    while (!uart_is_readable(uart_instance)) {
        if ((time_us_64() - start_time) > THIRTY_SECS) {
            strcpy(data_str, ERROR_IN_RECEIVING_MSG);
            return NO_ACK;
        }
        watchdog_update();
    }
    
    char receivedChar = uart_getc(uart_instance);
    DEBUG_PRINT("read_message_uart_channel::data_str is %s \n", data_str);
    DEBUG_PRINT("RP1 received the acknowledgement: %c\n", receivedChar);
    return receivedChar;
}

// Function that clears the UART buffer
void clear_buffer(uart_inst_t *uart_instance) {
    while (uart_is_readable(uart_instance)) {
        uart_getc(uart_instance);
    }
}

// Function that reads UART message from master
char read_message_from_master(uart_inst_t *uart_instance, char *data_str) {
    char receivedChar;
    int no_of_char_bytes = 0; // Start at 0 since we'll increment before writing
    uint64_t start_time = time_us_64();

    while (1) {
        if (!uart_is_readable(uart_instance)) {
            if ((time_us_64() - start_time) > TWO_SECONDS) {
                DEBUG_PRINT("Buffer is empty in RP1 \n");
                return error_in_ack;
            }
            watchdog_update();
            continue;
        }

        receivedChar = uart_getc(uart_instance);
        DEBUG_PRINT("receivedChar is %c\n", receivedChar);

        if (no_of_char_bytes < BUFFER_SIZE - 1) { // -1 to leave room for null terminator
            data_str[no_of_char_bytes++] = receivedChar;
            data_str[no_of_char_bytes] = '\0'; // Null terminate string after each character for safety
        } else {
            // Buffer overflow; handle or log as needed
            DEBUG_PRINT("Buffer overflow in UART read\n");
            return success_in_ack; // or another appropriate action
        }

        if (receivedChar == NEWLINE) {
            DEBUG_PRINT("Newline character received\n");
            return success_in_ack;
        }
    }
}

/*** end of file ***/