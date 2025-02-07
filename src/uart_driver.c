/**
 * @file uart_driver.c
 * @brief This file contains UART Related headers, Variables, function definitions. 
 * @author Yashas Nagaraj Udupa 
 */

 #include <string.h>
#include "uart_driver.h"
#include "pico.h"

void initialise_uart(uart_config_t *uartconfig)
{
    watchdog_update();  // to clear watchdog timer

    // set gpio pins for UART functionality
    gpio_set_function(uartconfig->txPin, GPIO_FUNC_UART);
    gpio_set_function(uartconfig->rxPin, GPIO_FUNC_UART);

    // Rx pin pulled up
    gpio_set_pulls(uartconfig->rxPin, true, false);
    
    // Set up the UART baud rate.
    uart_init(uartconfig->uartInst, uartconfig->baudRate);

    // desable UART hardware flow
    uart_set_hw_flow(uartconfig->uartInst, false, false);

    // Set Up UART Frame (stop bit, data bits, parity)
    uart_set_format(uartconfig->uartInst, uartconfig->dataLen, uartconfig->stopBit, uartconfig->parityBit);

    // Turning off FIFO's - we want to recieve character by character
    uart_set_fifo_enabled(uartconfig->uartInst, false);

    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = uartconfig->uartInst == uart0 ? UART0_IRQ : UART1_IRQ;

    // setting up UART interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, uartconfig->handler);

    // enable UART interrupt
    irq_set_enabled(UART_IRQ, true);

    // enable/disable UART RX/TX interrupts
    uart_set_irq_enables(uartconfig->uartInst, uartconfig->rxIntrEnable, uartconfig->txIntrEnable);
}

bool uart_write(uart_inst_t *uart, const uint8_t *src, size_t len)
{
    watchdog_update();  // to clear watchdog timer

    // variables for tracking time
    uint64_t oldTime = 0;
    uint64_t newTime = 0;
    uint64_t elapsedTime = 0;

    // updating as current time
    oldTime = time_us_64();
    newTime = time_us_64();

    // trying to write source buffer to UART
    for (size_t i = 0; i < len; ++i) 
    {
        oldTime = time_us_64();
        // Checking for UART is writable till UART Tx timeout
        while (!uart_is_writable(uart))
        {
            newTime = time_us_64();
            elapsedTime = newTime - oldTime;    // calculating elapsed time for sending 1 characer out
            // error on uart tx timeout 
            if(elapsedTime > MAIN_UART_TX_TIMEOUT)
            {
                return false;
            }
            tight_loop_contents();              // empty function
        }
        uart_get_hw(uart)->dr = *src++;         // copying single character to UART data register
    }
    return true;
}

bool uart_send_byte(uart_config_t *uartconfig, uint8_t byte)
{
    watchdog_update();  // to clear watchdog timer

    bool timeOut = false;   // variable for error checking
    
    timeOut = UartWrite(uartconfig->uartInst, (const uint8_t *) &byte, 1); // sends 1 byte on UART and returns erorr or success
    return timeOut;
}

bool uart_send_bytes(uart_config_t *uartconfig, uint8_t *buffer, size_t size)
{
    watchdog_update();  // to clear watchdog timer

    bool timeOut = false;   // variable for error checking

    // writes number of bytes on UART
    timeOut = UartWrite(uartconfig->uartInst, buffer, size);
    return timeOut;
}

bool uart_send_string(uart_config_t *uartconfig, char *str)
{
    watchdog_update();  // to clear watchdog timer

    bool timeOut = false;   // variable for error checking
    
    // writes a string until '\0'
    timeOut = UartWrite(uartconfig->uartInst, str, strlen(str));
    return timeOut;
}

bool uart_read_byte(uart_inst_t *uart, uint8_t *dst)
{
    watchdog_update();  // to clear watchdog timer

    // variables for tracking time
    uint64_t oldTime = 0;
    uint64_t newTime = 0;
    uint64_t elapsedTime = 0;

    // trying to read source buffer to UART

    oldTime = time_us_64();
    while (!uart_is_readable(uart))
    {
        newTime = time_us_64();
        elapsedTime = newTime - oldTime;    // calculating time differance
        // error on uart tx timeout
        if(elapsedTime > MAIN_UART_RX_TIMEOUT)
        {
            return false;
        }
        tight_loop_contents();
    }
    *dst = (uint8_t) uart_get_hw(uart)->dr; // copying single character from UART data register to destination variable
    return true;
}

/*** end of file ***/