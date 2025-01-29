# feedback-optic-control

## Overview

**FeedbackOptiControl** is a sophisticated system designed for Point-of-Care diagnostics, focusing on:

- Real-time image capture based on specific diagnostic events.
- Reducing optics maintenance through splash control.
- Precise control of valve motors and dome actuators via real-time feedback.
- Custom vibration settings tailored to different assay needs.
- Self-diagnostic features to verify hardware functionality.
- Extensive testing and validation to ensure firmware reliability.
- Compliance with ISO 13485:2016 for quality management in medical devices.
- Secure access for software updates by developers.
- Real-time monitoring dashboards for performance tracking.
- Built-in fail-safe mechanisms to enhance system reliability.

### File Descriptions

- **drv8825.c/.h**: Driver files for the DRV8825 stepper motor driver.
- **drv8827.c/.h**: Driver files for the 3-channel optical sensor.
- **gpio_control.c/.h**: GPIO control functions for interfacing with hardware.
- **main.c/.h**: Main application logic and definitions.
- **uart_driver.c/.h**: UART communication driver for serial data transfer.
