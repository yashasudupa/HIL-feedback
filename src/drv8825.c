// drv8825.c
// DRV8825 Motor Driver Implementation
// Author: Yashas Nagaraj Udupa

#include "DRV8825.h"

// Constants
#define BASE_10 10
#define TWO_BYTES 2
#define THREE_BYTES 3
#define FOUR_BYTES 4
#define FIFTEEN_BYTES 15
#define HUNDRED_BYTES 100
#define SIX_BYTES 6
#define FIVE_BYTES 5
#define THIRTY_DEGREES 30

// Data structures
VALVE_DICT valve_coordinates[5] = {
    {"V1", {-81}}, // Buffer reservoir
    {"V2", {0}},   // Mixing/Home
    {"V3", {-46}}, // Drain
    {"V4", {72}},  // Metering
    {"V5", {-119}} // Dab
};

STEPTYPE_DICT steptype_dict[6] = {
    {"01", {0, 0, 0, 1}}, // 1 Microstepping, Transition table
    {"02", {1, 0, 0, 2}}, // 1/2 Microstepping, Transition table
    {"04", {0, 1, 0, 4}}, // 1/4 Microstepping, Transition table
    {"08", {1, 1, 0, 8}}, // 1/8 Microstepping, Transition table
    {"16", {0, 0, 1, 16}}, // 1/16 Microstepping, Transition table
    {"32", {1, 0, 1, 32}}  // 1/32 Microstepping, Transition table
};

// Struct to encapsulate motor and encoder data
typedef struct {
    uint16_t actual_encoder_value;
    uint16_t previous_encoder_value;
    int current_valve_position;
    int previous_valve_position;
    uint16_t expected_encoder_value;
    uint8_t no_of_pulse;
    float encoder_resolution;
} MotorEncoderData;

// Static for module scope
static MotorEncoderData motor_data = {0, 0, 0, 0, 0, 0, 0.5};

static int rotate_handler(char direction, char *steptype, uint16_t angle, uint16_t rpm, MotorEncoderData *data) {
    uint8_t index = 0;
    while (index < SIX_BYTES && strcmp(steptype, steptype_dict[index].micro_steps) != 0) {
        index++;
    }
    if (index == SIX_BYTES) return RESOLUTION_ERROR;

    data->expected_encoder_value = (uint16_t)(data->encoder_resolution * angle);
    if (data->expected_encoder_value > ENCODER_NO_OF_PULSES) return EXP_IS_INVALID;
    if (angle > ANGLE_MAX) return ANGLE_IS_INVALID;

    uint32_t steps = (STEPS_PER_ROTATION * steptype_dict[index].step_factor[3] * angle) / DEGREE_FULL_ANGLE;
    resolution(steptype);
    gpio_put(M1_DIR, direction == DIR_CW ? HIGH : LOW);

    if (rpm == 0 || rpm > RPM_MAX) return RPM_IS_INVALID;
    uint32_t stepdelay = PICO_MAX(1, (STEPPER_RESOLUTION / rpm));
    
    gpio_put(M1_ENABLE, LOW);
    for (uint32_t i = 0; i < steps; i++) {
        gpio_put(M1_STEP, HIGH);
        sleep_us(stepdelay); 
        gpio_put(M1_STEP, LOW);
        sleep_us(stepdelay); 
    }
    gpio_put(M1_ENABLE, HIGH);
    return ROTATION_COMPLETED;
}

static void concatenate_encoder(char *ptr_e, char *ptr_a, char direction, int first_call, MotorEncoderData *data) {
    char encoder_temp[HUNDRED_BYTES];
    if (first_call) {
        snprintf(ptr_e, FIFTEEN_BYTES, "%d_%s", data->expected_encoder_value, direction == DIR_CCW ? "CCW" : "CW");
        snprintf(ptr_a, FIFTEEN_BYTES, "%d_%s", data->actual_encoder_value, direction == DIR_CCW ? "CCW" : "CW");
    } else {
        snprintf(ptr_a + strlen(ptr_a), FIFTEEN_BYTES - strlen(ptr_a), "_%d_%s", data->actual_encoder_value, direction == DIR_CCW ? "CCW" : "CW");
    }
}

int rotate_stepper_motor(char direction, char *steptype, char *ptr_e, char *ptr_a, uint16_t angle, uint16_t rpm) {
    int status = rotate_handler(direction, steptype, angle, rpm, &motor_data);
    if (status != ROTATION_COMPLETED) return status;

    static bool first_call = true;
    concatenate_encoder(ptr_e, ptr_a, direction, first_call, &motor_data);
    first_call = false; // Reset for next call

    int16_t encoder_diff = (int16_t)motor_data.expected_encoder_value - (int16_t)motor_data.actual_encoder_value;
    if (encoder_diff != 0 && motor_data.actual_encoder_value != 0) {
        direction = (encoder_diff > 0 == (direction == DIR_CCW)) ? direction : (direction == DIR_CCW ? DIR_CW : DIR_CCW);
        motor_data.actual_encoder_value = 0;
        return rotate_stepper_motor(direction, steptype, ptr_e, ptr_a, abs(encoder_diff) / motor_data.encoder_resolution, rpm);
    }

    watchdog_update();
    return ROTATION_COMPLETED;
}

// Method to home the stepper motor
int home_stepper_motor(const char motor_direction) {
    uint32_t step_counter = 0;
    if (gpio_get(ENC_CH2) == LOW) {
        gpio_put(M1_ENABLE, HIGH);
        return ENCODER_HW_FAIL;
    }

    while (step_counter < MAX_COUNT) {
        if (gpio_get(ENC_CH2) == LOW && gpio_get(ENC_CH1) == LOW && gpio_get(ENC_CH3) == LOW) {
            uint64_t start_time = get_time();
            while (gpio_get(ENC_CH2) == LOW) {
                rotate_handler(motor_direction, HOME_STEPTYPE, HOME_NO_OF_STEPS, HOME_RPM_INT, &motor_data);
            }
            uint64_t end_time = get_time() + (get_time() - start_time) / 2;
            while (get_time() < end_time) {
                rotate_handler(motor_direction == DIR_CW ? DIR_CCW : DIR_CW, HOME_STEPTYPE, HOME_NO_OF_STEPS, HOME_RPM_INT, &motor_data);
            }
            uint16_t expected_homing = (motor_data.actual_encoder_value + 6) % (ENCODER_NO_OF_PULSES + 1);
            while (expected_homing > motor_data.actual_encoder_value) {
                rotate_handler(motor_direction, HOME_STEPTYPE, HOME_NO_OF_STEPS, HOME_RPM_INT, &motor_data);
            }
            gpio_put(M1_ENABLE, HIGH);
            motor_data.previous_encoder_value = motor_data.actual_encoder_value;
            return HOMING_SUCCESSFUL;
        }
        rotate_handler(motor_direction, HOME_STEPTYPE, HOME_NO_OF_STEPS, HOME_RPM_INT, &motor_data);
        step_counter++;
    }
    return MOTOR_HW_FAIL;
}

// Private method to set step resolution
static int resolution(char *steptype) {
    int mode_pins[] = {M1_MODE0, M1_MODE1, M1_MODE2};
    for (int i = 0; i < SIX_BYTES; i++) {
        if (strcmp(steptype, steptype_dict[i].micro_steps) == 0) {
            for (int j = 0; j < THREE_BYTES; j++) {
                gpio_put(mode_pins[j], steptype_dict[i].step_factor[j]);
            }
            return steptype_dict[i].step_factor[THREE_BYTES];  // microsteps per full step
        }
    }
    return RESOLUTION_ERROR;
}

static void concatenate_acknowledgement(char *ack_buffer, int status, char *valve_ack, const char *ptr_e, const char *ptr_a) {
    snprintf(valve_ack, HUNDRED_BYTES, "vf_%d_%s_%s\n", status, ptr_e, ptr_a);
}

// State machine to rotate the valve motor
int state_rotate_valve(const char *data_str) {
    watchdog_update();
    int status;
    char direction, steptype[THREE_BYTES], rpm[FOUR_BYTES];
    char valve_ack[HUNDRED_BYTES] = "vf";
    const char *valve_rotation_2 = "V2";

    for (int i = 0; i < SIX_BYTES; i++) {
        if (strncmp(data_str, valve_coordinates[i].valve_type, TWO_BYTES) == 0) {
            motor_data.current_valve_position = valve_coordinates[i].valve_position - motor_data.current_valve_position;
            break;
        }
    }
    direction = motor_data.current_valve_position < 0 ? DIR_CCW : DIR_CW;
    strcpy(steptype, STEPTYPE);
    strcpy(rpm, RPM);

    int rpm_int = atoi(rpm);
    int angle_int = abs(motor_data.current_valve_position);

    if (motor_data.previous_valve_position == valve_coordinates[0].valve_position && strncmp(data_str, "V3", TWO_BYTES) == 0) {
        angle_int += (VALVE_RESISITANCE * DEGREE_FULL_ANGLE) / (STEPS_PER_ROTATION * steptype_dict[4].step_factor[3]);
    }

    if (gpio_get(ENC_CH1) == HIGH) {
        motor_data.no_of_pulse += 1;
    }

    char expected_valve_char[FIFTEEN_BYTES] = "";
    char actual_valve_char[FIFTEEN_BYTES] = "";

    if (strncmp(data_str, valve_rotation_2, TWO_BYTES) == 0) {
        status = home_stepper_motor(direction);
        angle_int = THIRTY_DEGREES;
        if (direction == DIR_CCW || motor_data.previous_valve_position == valve_coordinates[1].valve_position) {
            status = rotate_handler(DIR_CCW, HOME_STEPTYPE, angle_int, HOME_RPM_INT, &motor_data);
            status = home_stepper_motor(DIR_CW);
        }
    } else {
        status = rotate_stepper_motor(direction, steptype, expected_valve_char, actual_valve_char, angle_int, rpm_int);
    }

    for (int i = 0; i < FIVE_BYTES; i++) {
        if (strncmp(data_str, valve_coordinates[i].valve_type, TWO_BYTES) == 0) {
            motor_data.previous_valve_position = motor_data.current_valve_position = valve_coordinates[i].valve_position;
        }
    }

    char ack_buffer[FOUR_BYTES];
    concatenate_acknowledgement(ack_buffer, status, valve_ack, expected_valve_char, actual_valve_char);

    motor_data.actual_encoder_value = 0;    
    uart_send_bytes(valve_ack, strlen(valve_ack), RP1_UART_NUMBER);
    return status;
}

// Simplified get_time function for 64-bit time from the timer
static uint64_t get_time(void) {
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t) hi << 32u) | lo;
}

/*** end of file ***/