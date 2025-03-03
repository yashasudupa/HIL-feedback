/**
 * @file drv8827.c
 * @brief DRV8827 Motor Driver Implementation
 * @author Yashas Nagaraj Udupa 
 */

#include "drv8827.h"

// Time delays in milliseconds
#define MS(x) ((x) * 1000)
#define HALF_SECOND MS(0.5)
#define ONE_SECOND MS(1)
#define TWO_SECONDS MS(2)
#define THREE_SECONDS MS(3)
#define FOUR_SECONDS MS(4)
#define FIVE_SECONDS MS(5)
#define SIX_SECONDS MS(6)
#define SEVEN_SECONDS MS(7)
#define TEN_SECONDS MS(10)
#define TWO_FIFTY_MS 250
#define FIFTY_MS 50
#define TEN_MS 10

// Sets the duty cycle and starts the PWM signal
static void change_pwm_signal_pattern(const uint slice_num, const uint16_t duty_cycle, uint32_t delay) {  
    pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);
    pwm_set_enabled(slice_num, true);
    sleep_ms(delay);
    pwm_set_enabled(slice_num, false);
}

// Executes the sequences of PWM signal for standard vibration
static int8_t execute_vibration_sequence(uint slice_num, uint16_t wrap_value, const uint16_t *duty_cycles, const uint32_t *delays, size_t sequence_length) {
    for (size_t i = 0; i < sequence_length; ++i) {
        change_pwm_signal_pattern(slice_num, duty_cycles[i], delays[i]);
    }
    return VIBRATION_SUCCESSFUL;
}

// Executes vibration sequences
int8_t process_vibration_sequences() {
    gpio_put(M3_SLEEP, HIGH);
    gpio_set_function(M3_IN2, GPIO_FUNC_PWM);

    const uint16_t wrap_value = 936;
    const uint16_t duty_cycles[] = {205, 160, 179}; // 21.9%, 17.09%, 19%
    const uint32_t delays[] = {TWO_FIFTY_MS, THREE_SECONDS, TWO_FIFTY_MS};
    
    uint slice_num = pwm_gpio_to_slice_num(M3_IN2);
    pwm_set_clkdiv(slice_num, 256.0f);
    pwm_set_wrap(slice_num, wrap_value);

    int8_t status = execute_vibration_sequence(slice_num, wrap_value, duty_cycles, delays, sizeof(duty_cycles) / sizeof(duty_cycles[0]));

    gpio_put(M3_SLEEP, LOW);
    gpio_init(M3_IN2);
    gpio_set_dir(M3_IN2, GPIO_OUT);
    gpio_put(M3_IN2, LOW);

    DEBUG_PRINT("Vibration ended \n");
    return status;
}

// Executes washing vibration sequence
int8_t process_washing_vibration() {
    gpio_put(M3_SLEEP, HIGH);
    gpio_set_function(M3_IN2, GPIO_FUNC_PWM);

    const uint16_t wrap_value = 936;
    const uint16_t duty_cycles[] = {205, 150}; // 21.9%, 16.02%
    const uint32_t delays[] = {TWO_FIFTY_MS, THREE_SECONDS};
    
    uint slice_num = pwm_gpio_to_slice_num(M3_IN2);
    pwm_set_clkdiv(slice_num, 256.0f);
    pwm_set_wrap(slice_num, wrap_value);

    int8_t status = execute_vibration_sequence(slice_num, wrap_value, duty_cycles, delays, sizeof(duty_cycles) / sizeof(duty_cycles[0]));

    gpio_put(M3_SLEEP, LOW);
    gpio_init(M3_IN2);
    gpio_set_dir(M3_IN2, GPIO_OUT);
    gpio_put(M3_IN2, LOW);

    DEBUG_PRINT("Vibration during washing ended \n");
    return status;
}

// Executes washing vibration with frequency input
int8_t process_washing_vibration_input(uint16_t freq) {
    gpio_put(M3_SLEEP, HIGH);
    gpio_set_function(M3_IN2, GPIO_FUNC_PWM);

    const uint16_t wrap_value = 936;
    const uint16_t duty_cycles[] = {205, freq, 197}; // 21.9%, variable, 21.7%
    const uint32_t delays[] = {TWO_FIFTY_MS, THREE_SECONDS, TWO_FIFTY_MS};
    
    uint slice_num = pwm_gpio_to_slice_num(M3_IN2);
    pwm_set_clkdiv(slice_num, 256.0f);
    pwm_set_wrap(slice_num, wrap_value);

    int8_t status = execute_vibration_sequence(slice_num, wrap_value, duty_cycles, delays, sizeof(duty_cycles) / sizeof(duty_cycles[0]));

    gpio_put(M3_SLEEP, LOW);
    gpio_init(M3_IN2);
    gpio_set_dir(M3_IN2, GPIO_OUT);
    gpio_put(M3_IN2, LOW);

    DEBUG_PRINT("Vibration with input ended \n");
    return status;
}

/*** end of file ***/