/**
 * @file main.c
 * @brief Optical sensor feedback Implementation
 * @author Yashas Nagaraj Udupa 
 */

#include "main.h"

// main UART config Variable
uart_config_t _mainUartConfig = {
    .uartInst = MAIN_UART_INSTANCE,
    .baudRate = MAIN_UART_BAUDRATE,
    .dataLen = MAIN_UART_DATA_LEN,
    .parityBit = MAIN_UART_PARITY,
    .stopBit = MAIN_UART_STOP_BIT,
    .txPin = MAIN_UART_TX,
    .rxPin = MAIN_UART_RX,
    .txIntrEnable = MAIN_UART_TX_INTR_ENABLE,
    .rxIntrEnable = MAIN_UART_RX_INTR_ENABLE,
    .handler = mainUartHandler,
};

// Private function that gets launched at core 1 to trigger kill switch and report status to Master
static void core1_entry() {
    gpio_set_irq_enabled_with_callback(ENC_CH1, GPIO_IRQ_EDGE_RISE, true, &detect_rise_in_channel_one_isr);
    while(1) {
        if (atomic_load(&uart_k_flag)){
            memset(mainUartStruct.rxBuffer, 0, MAX_SIZE);
            reset_pico(valve_coordinates[1].valve_type);
            atomic_store(&uart_k_flag, false);
        }
        sleep_ms(100);
    }
}


void rp1_feedback(rp1_feedback_response_t rp1Response, uart_config_t *mainUartConfig){
    char responseMsg[RP1_RESPONSE_BUFFER_SIZE] = {}; // buffer for sending Response
    // crc failed response
    if(rp1Response == CRC_FAILED)
    {
        sprintf(responseMsg, "ERROR:CRC FAIL\r\n");
        uart_send_string(mainUartConfig, responseMsg);
    }
    // invalid commnd response
    else if(rp1Response == INVALID_COMMAND)
    {
        sprintf(responseMsg, "ERROR:INVALID COMMAND TYPE\r\n");
        uart_send_string(mainUartConfig, responseMsg);
    }
    // health check failed response
    else if(rp1Response == RP1_HEALTH_CHECK_FAILED)
    {
        sprintf(responseMsg, "ERROR:RP1 HEALTH CHECK FAILED\r\n");
        uart_send_string(mainUartConfig, responseMsg);
    }
    // health check success response
    else if(rp1Response == RP1_HEALTH_CHECK_SUCCESS)
    {
        sprintf(responseMsg, "SUCCESS:RP1 HEALTH CHECK SUCCESS\r\n");
        uart_send_string(mainUartConfig, responseMsg);
    }

    // clearing UART Rx buffers
    memset(mainUartStruct.rxBuffer,0,UART_RX_BUFFER_SIZE);
    mainUartStruct.rxBufferCount = 0;
    mainUartStruct.commandRecieved = false;
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
            default:
                DEBUG_PRINT("Invalid UART message \n");
                const char *ack = "Invalid UART message \n";
                uart_send_bytes(ack, strlen(ack), RP1_UART_NUMBER);
                break;
        }
    }
    return status;
}

int main(){
    // enable watchdog for 8388 ms 
    watchdog_enable(8388, true);

    //Invoke the application handlers    
    initialisations(&_mainUartConfig);

    //Launch core 1
    multicore_launch_core1(core1_entry);

    #ifdef ENABLE_UNIT_TEST
        //Launch unit tests
        test_rotate_stepper_motor();
    #endif
    
    while(1){
        watchdog_update();  // to clear watchdog timer

        // CRC Checking
        #if CRC_ENABLE
            ret = CheckCRC(mainUartStruct.rxBuffer);
            if(ret == CRC_FAIL)
            {
                DEBUG_PRINT("CRC failed\n");
                rp1_feedback(CRC_FAILED, &_mainUartConfig);
                continue;
            }
            DEBUG_PRINT("CRC success\n");
        #endif

        if (atomic_load(&uart_ix_flag)){
            DEBUG_PRINT("Interrupt enabled \n");
            atomic_store(&uart_ix_flag, false);
            process_state_machine(mainUartStruct.rxBuffer);
        }
        sleep_ms(100);
        tight_loop_contents();
    }
}

/*** end of file ***/