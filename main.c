// main.c
// Achira Labs - RP1 Pico Application
// Author: Yashas Nagaraj Udupa

#include "main.h"

// Private function that gets launched at core 1 to trigger kill switch and report status to Master
static void core1_entry() {
    gpio_set_irq_enabled_with_callback(ENC_CH1, GPIO_IRQ_EDGE_RISE, true, &detect_rise_in_channel_one_isr);
    while(1) {
        if (atomic_load(&uart_k_flag)){
            memset(data_str, 0, MAX_SIZE);
            reset_pico(valve_coordinates[1].valve_type);
            atomic_store(&uart_k_flag, false);
        }
        sleep_ms(100);
    }
}

// Private state machine function to process different states based on received commands
static int process_state_machine(const char *data_str_ptr) {
    int status = INVALID_REQUEST;
    if(*data_str_ptr) {
        enum DesiredFunc userDesiredFunc = get_desired_func(data_str_ptr);

        switch(userDesiredFunc) {
            case V1: case V2: case V3: case V4: case V5:
                DEBUG_PRINT("Entered Valve Rotation\n");
                status = state_rotate_valve(data_str_ptr);
                break;
            case ST:
                DEBUG_PRINT("Entered shaker turn on\n");
                status = shaker_on();
                break;
            case RS:
                DEBUG_PRINT("Entered incubation vibration turn on\n");
                status = incubation_shaker_on();
                break;
            case WV:
                DEBUG_PRINT("Entered vibration shaker turn on\n");
                status = vibration_shaker_on();
                break;
            case FV:
                DEBUG_PRINT("Entered report firmware version function\n");
                status = report_firmware_version(firmware_version);
                break;
            case MO:
                DEBUG_PRINT("Turn off the valve motor \n");
                status = turn_off_motor();
                break;
            case TW:
                status = test_vibration_shaker_on(data_str_ptr);
                break;
            default:
                DEBUG_PRINT("Invalid UART message \n");
                const char *ack = "Invalid UART message \n";
                uart_write_bytes(ack, strlen(ack), RP1_UART_NUMBER);
                break;
        }
    }
    return status;
}

int main(){
    // enable watchdog for 8388 ms 
    watchdog_enable(8388, true);

    //Invoke the application handlers    
    initialisations();

    //Launch core 1
    multicore_launch_core1(core1_entry);

    #ifdef ENABLE_UNIT_TEST
        //Launch unit tests
        test_rotate_stepper_motor();
    #endif
    
    while(1){
        watchdog_update();  // to clear watchdog timer
        if (atomic_load(&uart_ix_flag)){
            DEBUG_PRINT("Interrupt enabled \n");
            atomic_store(&uart_ix_flag, false);
            process_state_machine(data_str);
        }
        sleep_ms(100);
        tight_loop_contents();
    }
}

/*** end of file ***/