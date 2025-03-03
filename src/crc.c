/**
 * @file crc.c
 * @brief This file contains CRC related function definitions
 * @author Yashas Nagaraj Udupa 
 */

#include "crc.h"

uint32_t calculate_crc(char *string, uint64_t length)
{
    watchdog_update();  // to clear watchdog timer

    uint32_t crc = 0;   // variable for calculated CRC
    uint32_t sum = 0;   // variable for sum of each element in string

    DEBUG_PRINT("\n****Calculating CRC****\n");
    // calculating sum of each element of string
    for(uint32_t i = 0 ; i < length ; i++)
    {
        sum += string[i];
    }
    crc = ~sum;    //1's complement of sum
    DEBUG_PRINT("CRC in HEX = %x\n", crc);
    return crc;
}

bool check_crc(char *string)
{
    watchdog_update();  // to clear watchdog timer

    uint8_t recievedCRCstr[9] = {};
    size_t crcStartPos = 0, lengthOfCrc = 0;                        // variable to store CRC start position from string
    crcStartPos = strlen(string);                                   // getting the length of UART string length
    lengthOfCrc = (&string[crcStartPos-1] - strrchr(string, ','));  // getting length of recieved CRC 
    crcStartPos -= lengthOfCrc;                                     // crc is at the end, so finding last ',' for getting crc start position

    uint8_t tempstring[CRC_TEMP_BUFFER_SIZE] = {};                  // temperory string for storing UART string without crc
    strncpy(tempstring, string, crcStartPos);                       // copying UART string to tempstring without CRC for calculating CRC

    strncpy(recievedCRCstr, &string[crcStartPos], lengthOfCrc-1);   // copying recived HEX crc to another buffer without '#'

    uint32_t recievedCrc = 0, calculatedCrc = 0;
    recievedCrc = strtoul(recievedCRCstr, NULL, 16);                // converts Hex CRC string to integer CRC value

    calculatedCrc = CalculateCRC(tempstring, crcStartPos);          // calculating crc of recieved string

    // error if calculated CRC and Recieved CRC doen't match
    if(calculatedCrc != recievedCrc)
    {
        return CRC_FAIL;
    }

    //success if it is matches
    return CRC_OK;
}

/*** end of file ***/