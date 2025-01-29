/** @file drv8825.h
*
* @brief This driver module is responsible for driving the valve motor.
*
*/

#ifndef _DRV8825_H
#define _DRV8825_H

#include <stdlib.h>
#include <string.h>
#include "hardware/watchdog.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "uart_driver.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

// Define constants for better maintainability
#define DEGREE_FULL_ANGLE 360
#define TOLERANCE_ENCODER_VALUE 2
#define KILL_SWITCH 'K'

// Encoder Channels
#define ENC_CH1 28   // Signal A
#define ENC_CH2 27   // Signal Z
#define ENC_CH3 26   // Signal B

// Utility Macros
#define PICO_MAX(a, b) ((a) > (b) ? (a) : (b))

// Error and Success Codes
#define RESOLUTION_ERROR -1
#define MOTOR_HW_FAIL -1
#define DEG_CALC_ERROR 0.0
#define SUCCESS_INT 1
#define SUCCESS_FLOAT 1.0
#define ROTATION_COMPLETED 2
#define ROTATION_CONTROLLED 3
#define RPM_IS_INVALID -3
#define EXP_IS_INVALID -4
#define ANGLE_IS_INVALID -5
#define HOMING_SUCCESSFUL 2
#define ENCODER_HW_FAIL -2
#define ENCODER_NO_OF_PULSES 179
#define HOMING_TIMEOUT -3

// Motor and Encoder Parameters
#define NO_OF_STEPS 3200
#define DELAY 500
#define CW 1
#define CCW 0
#define DIR_CW 'C'
#define DIR_CCW 'A'
#define STEPS_PER_ROTATION 200
#define PROPORTIONAL_CONSTANT 10
#define STEPPER_RESOLUTION 500000
#define ANGLE_MAX 720
#define RPM_MAX 3200
#define STEPS_TO_CENTRE 10

// Valve rotation settings
#define RPM "800"
#define STEPTYPE "16"
#define HOME_STEPTYPE "32"
#define HOME_RPM_INT 800
#define HOME_NO_OF_STEPS 1

// GPIO Pin Definitions
#define M1_STEP 4    // MOTOR1_STEP
#define M1_DIR 6     // MOTOR1_DIRECTION
#define M1_NFAULT 7  //