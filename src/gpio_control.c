// gpio_control.c
// gpio_control module Implementation
// Author: Yashas Nagaraj Udupa

#include "gpio_control.h"

// Constants moved to header file or made local where possible

// UART interrupt initializations
static char data_str[MAX_SIZE] = {0};  // Changed to static for module scope
static atomic_bool uart_ix_flag = false;
static atomic_bool uart_k_flag = false;

static volatile uint64_t start_time = 0;
static volatile uint64_t end_time = 0;

// Array of strings corresponding to positions of valve motor rotations 
static const char *desiredFuncStrings[] = {
    "K\n", "V1\n", "V2\n", "V3\n", "V4\n", "V5\n", "V6\n", "ST\n", 
    "SF\n", "IV\n", "RS\n", "WV\n", "FV\n", "MO\n", "TS\n", "TW\n"
};

// Method to blink LED when Pico one board is reset
void on_board_led_blink() {
    for (int i = 0; i < 3; i++) {
        gpio_put(RP1_OB_LED, 1);
        sleep_ms(FIVE_HUNDRED_MILLISECONDS);
        gpio_put(RP1_OB_LED, 0);
        sleep_ms(FIVE_HUNDRED_MILLISECONDS);
    }
}

// Function to get DesiredFunc enum from string
enum DesiredFunc get_desired_func(const char *ptr_data_str) {
    for (int i = 0; i < NUM_OF_UART_FUNCS; i++) {
        if (strncmp(ptr_data_str, desiredFuncStrings[i], TWO_BYTES) == 0) {
            return (enum DesiredFunc)i;
        }
    }
    return INVALID_DESIRED_FUNC;  
}

// Method to reset the Pico board
int reset_pico(char *ptr_data_str, const char *kill_switch_ack) {
    gpio_put(M1_ENABLE, LOW);  // Disable motor first
    gpio_init(M3_IN2);
    gpio_set_dir(M3_IN2, GPIO_OUT);
    gpio_put(M3_IN2, 0);
    state_rotate_valve(ptr_data_str);

    // Send the received acknowledgement to PI
    uart_write_bytes(kill_switch_ack, strlen(kill_switch_ack), RP1_UART_NUMBER); 
    return 1;  // Return success without using global variable
}

// Method to trigger the vibration sequence
int shaker_on(const char *st_ack) {
    int status = process_vibration_sequences();  
    uart_write_bytes(st_ack, strlen(st_ack), RP1_UART_NUMBER);  
    return status;    
}

// Method to trigger the vibration during incubation
int incubation_shaker_on(const char *iv_ack) {
    int status = process_vibration_sequences();  
    uart_write_bytes(iv_ack, strlen(iv_ack), RP1_UART_NUMBER);  
    return status;    
}

// Washing shaker started
int vibration_shaker_on(const char *wv_ack) {
    int status = process_washing_vibration();  
    uart_write_bytes(wv_ack, strlen(wv_ack), RP1_UART_NUMBER);  
    return status;    
}

// Washing shaker with custom frequency
int test_vibration_shaker_on(const char *data_str, const char *wv_ack) {
    char freq[FOUR_BYTES];
    strncpy(freq, &data_str[3], FOUR_BYTES - 1);  // Ensure we don't overflow
    freq[FOUR_BYTES - 1] = '\0';  // Null terminate
    uint16_t freq_int = atoi(freq);
    int status = process_washing_vibration_input(freq_int);  
    uart_write_bytes(wv_ack, strlen(wv_ack), RP1_UART_NUMBER);  
    return status;    
}

// Function to report the firmware version to the RPI4
bool report_firmware_version(const char *version, int uart_port) {
    uart_write_bytes(version, strlen(version), uart_port);
    return true;  // Return success without using global variable
}

// Function to turn off the motor
bool turn_off_motor(int uart_port, const char *turn_off_ack) {
    gpio_put(M1_ENABLE, HIGH);

    // Send acknowledgement to RPI
    uart_write_bytes(turn_off_ack, strlen(turn_off_ack), uart_port);
    return true;  // Return success without using global variable
}

// Method to initialize and configure GPIOs
void initialisations(void) {
    watchdog_update();
    stdio_init_all();

    // GPIO initializations and configurations
    static const uint gpio_pins[] = {M1_MODE2, M1_MODE1, M1_MODE0, M1_STEP, M1_ENABLE, M1_DIR, M1_NFAULT, MOTOR_SLEEP, MOTOR_RESET, 
                                     M3_IN2, M3_IN1, M3_SLEEP, RP1_IO16_UART0_TX, RP1_IO17_UART0_RX, ENC_CH3, ENC_CH2, ENC_CH1, RP1_OB_LED};

    for (size_t i = 0; i < sizeof(gpio_pins) / sizeof(gpio_pins[0]); i++) {
        gpio_init(gpio_pins[i]);
    }

    gpio_set_dir(M3_IN2, GPIO_OUT);
    gpio_set_dir(M3_IN1, GPIO_OUT);
    gpio_set_dir(M3_SLEEP, GPIO_OUT);
    gpio_set_dir(M1_MODE2, GPIO_OUT);
    gpio_set_dir(M1_MODE1, GPIO_OUT);
    gpio_set_dir(M1_MODE0, GPIO_OUT);
    gpio_set_dir(M1_STEP, GPIO_OUT);
    gpio_set_dir(M1_ENABLE, GPIO_OUT);
    gpio_set_dir(M1_DIR, GPIO_OUT);
    gpio_set_dir(M1_NFAULT, GPIO_IN);
    gpio_set_dir(MOTOR_RESET, GPIO_OUT);
    gpio_set_dir(MOTOR_SLEEP, GPIO_OUT);
    gpio_set_dir(ENC_CH1, GPIO_IN);
    gpio_set_dir(ENC_CH2, GPIO_IN);
    gpio_set_dir(ENC_CH3, GPIO_IN);
    gpio_set_dir(RP1_OB_LED, GPIO_OUT);

    gpio_put(M3_IN1, LOW);
    gpio_put(MOTOR_SLEEP, HIGH);
    gpio_put(MOTOR_RESET, HIGH);
    gpio_put(M1_ENABLE, HIGH);

    // UART Initialization
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(RP1_IO16_UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(RP1_IO17_UART0_RX, GPIO_FUNC_UART);
    uart_set_baudrate(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);

    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);

    gpio_set_irq_enabled_with_callback(ENC_CH1, GPIO_IRQ_EDGE_FALL, true, &encoder_isr);

    on_board_led_blink();
    watchdog_update();
    state_rotate_valve("V2");  // Assuming "V2" is a constant string for home position
}

// Private Interrupt service routine for UART RX
#pragma irq_entry
static void on_uart_rx() {
    watchdog_update();
    while (uart_is_readable(UART_ID))  {
        memset(data_str, 0, MAX_SIZE);

        char uart_ack = read_message_from_master(UART_ID, data_str);            
        clear_buffer(UART_ID);

        if (*data_str == 'K') {
            uart_k_flag = true;
        }
        else {
            uart_ix_flag = true;
        }
    }
}

// Simplest form of getting 64 bit time from the timer.
static uint64_t get_time(void) {
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t) hi << 32u) | lo;
}

// Private Interrupt service routine for encoder
#pragma irq_entry
void encoder_isr(){
    end_time = get_time();
    if(end_time - start_time > 1000) {
        actual_encoder_value = (actual_encoder_value + 1)%(ENCODER_NO_OF_PULSES + 1);
    }
}

// Private Interrupt service routine for encoder channel 1
#pragma irq_entry
void detect_rise_in_channel_one_isr() {
    start_time = get_time();
}

/* end of file */