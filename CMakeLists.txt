cmake_minimum_required(VERSION 3.13)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
project(pico-multicore)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Define the macro based on some condition
set(ENABLE_UNIT_TEST OFF)

# rest of your project
if (TARGET tinyusb_device)
    add_executable(rp1)
    message(STATUS "ENABLE_UNIT_TEST is set to: ${ENABLE_UNIT_TEST}")

    # Conditionally include source files
    if(ENABLE_UNIT_TEST)
        target_sources(rp1 PRIVATE ./src/main.c ./src/drv8825.c ./src/drv8827.c ./src/gpio_control.c ./src/drv8827.c ./src/uart_driver.c ./src/test.c)
        target_compile_definitions(rp1 PRIVATE ENABLE_UNIT_TEST)
    else()
        target_sources(rp1 PRIVATE ./src/main.c ./src/drv8825.c ./src/drv8827.c ./src/gpio_control.c ./src/drv8827.c ./src/uart_driver.c)
    endif()

    # pull in common dependencies
    target_link_libraries(rp1
            pico_multicore
            pico_stdlib
            hardware_adc
            hardware_gpio
            hardware_uart
            hardware_timer
            hardware_i2c
            pico_binary_info
            pico_stdio_usb
            hardware_pwm
            hardware_watchdog
            hardware_rtc
            m)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(rp1 1)
    pico_enable_stdio_uart(rp1 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(rp1)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building rp1 because TinyUSB submodule is not initialized in the SDK")
endif()