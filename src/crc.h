/**
 * @file crc.h
 * @brief This file contains CRC related headers, Macros and function prototypes
 * 
 */

#ifndef CRC_H
#define CRC_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Define or undefine this macro to enable or disable debug prints
#define DEBUG_PRINT_ENABLED 0

// Debug print macro
#if DEBUG_PRINT_ENABLED
    #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

// Macro for CRC Enable/Disable
#define CRC_ENABLE              1  // 1=Enable, 0=Disable

// Macro for CRC status
#define CRC_OK                  0   // CRC success
#define CRC_FAIL                1   // CRC fail

// Macro for temp CRC buffer size
#define CRC_TEMP_BUFFER_SIZE    100 // size of temporary CRC buffer

/**
 * @brief Function Calculates CRC of given string (character array)
 * Example : your string is like this "$DomeMotor,Up,16,1600,0,<CRC>#"
 *           this function will take string till <CRC>
 * 
 * -    CRC LOGIC :    make a sum of all character's ascii value then 1's complement of the sum
 * 
 * @param string - pointer to string
 * @param length - length of string
 * @return uint32_t - calculated CRC
 */
uint32_t calculate_crc(char *string, uint64_t length);

/**
 * @brief This function calculates crc from the string and compares it with crc recieved from string
 *        It calculates CRC using CalculateCRC function
 *        then it will convert Recieved HEX CRC to integer
 *        and then compares both
 *        if CRC matches success
 *        else failure
 * @note  This function should be used after parsing paramatrers
 * 
 * @param string - pointer to UART command
 * @return CRC_FAIL failure
 * @return CRC_OK success
 */
bool check_crc(char *string);

#endif // CRC_H
