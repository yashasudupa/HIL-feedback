/**
 * @file uart_driver.h
 * @brief This file contains UART related Macros, structures, function prototypes
 * 
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/watchdog.h"
#include "debug_print.h"

// RP2 main UART CONFIG
#define MAIN_UART_INSTANCE              uart0
#define MAIN_UART_BAUDRATE              115200
#define MAIN_UART_DATA_LEN              8
#define MAIN_UART_PARITY                UART_PARITY_NONE
#define MAIN_UART_STOP_BIT              1
#define MAIN_UART_TX_INTR_ENABLE        false
#define MAIN_UART_RX_INTR_ENABLE        true    

// RP2 main UART Pins
#define MAIN_UART_TX                    16
#define MAIN_UART_RX                    17

// RP2 main UART buffer size
#define UART_RX_BUFFER_SIZE             100

// RP2 main UART RX/TX Timeout
#define MAIN_UART_TX_TIMEOUT            (100 * 1000) // 100 ms UART tx timeout
#define MAIN_UART_RX_TIMEOUT            (100 * 1000) // 100 ms UART rx timeout

// UART config structure
typedef struct
{
    uart_inst_t *uartInst;
    uint64_t baudRate; 
    uint16_t dataLen;
    uint16_t parityBit;
    uint16_t stopBit;
    uint16_t txPin;
    uint16_t rxPin;
    bool txIntrEnable;
    bool rxIntrEnable;
    irq_handler_t handler;
}uart_config_t;

// UART rx data structure
typedef struct {
    char rxBuffer[UART_RX_BUFFER_SIZE];
    uint32_t rxBufferCount;
    bool commandRecieved;
}uartRxData_t;

/**
 * @brief This function Initializes UART
 * 
 * @param uartconfig - pointer to UART config structure
 * @return nothing
 */
void initialise_uart(uart_config_t *uartconfig);

/**
 * @brief writes number of bytes on UART (non blocking with a MAIN_UART_TX_TIMEOUT timeout)
 * 
 * @param uart - uart instance
 * @param src - pointer to buffer to write
 * @param len - length of buffer
 * @return true - on successfully sent
 * @return false - on timeout
 */
bool uart_write(uart_inst_t *uart, const uint8_t *src, size_t len);

/**
 * @brief writes single byte on UART (non blocking with a MAIN_UART_TX_TIMEOUT timeout)
 * 
 * @param uartconfig - pointer to UART config
 * @param byte - byte to be sent
 * @return true - on successfully sent
 * @return false - on timeout
 */
bool uart_send_byte(uart_config_t *uartconfig, uint8_t byte);

/**
 * @brief writes n number of bytes on UART (non blocking with a MAIN_UART_TX_TIMEOUT timeout)
 * 
 * @param uartconfig - pointer to UART config
 * @param buffer - pointer to buffer
 * @param size - length of buffer
 * @return true - on successfully write
 * @return false - on timeout
 */
bool uart_send_bytes(uart_config_t *uartconfig, uint8_t *buffer, size_t size);

/**
 * @brief writes string on UART (non blocking with a MAIN_UART_TX_TIMEOUT timeout)
 * 
 * @param uartconfig - pointer to UART config
 * @param str - pointer to string
 * @return true - on successfully write
 * @return false - on timeout
 */
bool uart_send_string(uart_config_t *uartconfig, char *str);

/**
 * @brief reads single byte on UART (non blocking with a MAIN_UART_RX_TIMEOUT timeout)
 * 
 * @param uart - uart instance
 * @param dst - pointer to a variable which needs to be filled with data
 * @return true - on successfully read
 * @return false - on timeout
 */
bool uart_read_byte(uart_inst_t *uart, uint8_t *dst);

#endif
