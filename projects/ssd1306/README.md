# SSD1306 OLED Display Example Project

This project demonstrates the usage of the SSD1306 OLED display with the Maxim platform. It integrates the LittlevGL (LVGL) graphics library to create a graphical user interface (GUI) and display animations on the SSD1306.

## Features

- **Display Initialization**: Configures the SSD1306 OLED display using I2C communication.
- **UART Configuration**: Sets up UART for debugging and communication.
- **Graphics Rendering**: Utilizes the LVGL library to render graphics and animations.
- **Startup Screen**: Displays a custom startup screen with a logo.
- **Scrolling Text Animation**: Demonstrates a scrolling text animation inspired by the Star Wars intro.

## Project Structure

### Source Files

- **`src/examples/example/example.c`**: Contains the main implementation of the project, including display initialization, UART setup, and rendering logic.
- **`src/examples/example/example.h`**: Header file for the example implementation.
- **`src/common/common_data.c`**: Defines common data structures and initialization parameters for the project.
- **`src/common/common_data.h`**: Header file for common data definitions.
- **`src/platform/maxim/parameters.c`**: Platform-specific parameters for the Maxim platform.
- **`src/platform/maxim/parameters.h`**: Header file for platform-specific parameters.

### Build Files

- **`build/ssd1306-cflags.txt`**: Compiler flags used during the build process.
- **`build/ssd1306-cppflags.txt`**: Preprocessor flags used during the build process.
- **`build/ssd1306-objs.txt`**: List of object files generated during the build process.

## Dependencies

- **LVGL Library**: A lightweight graphics library for embedded systems.
- **No-OS Drivers**: Drivers for I2C, UART, and other peripherals.
- **Maxim SDK**: Platform-specific drivers and utilities for the Maxim platform.

## How It Works

1. **Initialization**:
   - The `example_main` function initializes the system using `no_os_init`.
   - UART is configured for debugging using `configure_uart`.
   - The SSD1306 display is initialized using `configure_display`.

2. **Startup Screen**:
   - The `startup_screen` function displays a logo on the SSD1306 OLED display for 5 seconds.

3. **Scrolling Text Animation**:
   - The `test_draw` function creates a label with scrolling text and animates it using LVGL's animation API.

4. **Main Loop**:
   - The main loop continuously updates the LVGL timer handler and refreshes the display.

## Functions Overview

### `configure_uart`
Configures the UART for debugging and communication.

### `configure_display`
Initializes the SSD1306 OLED display using I2C communication.

### `display_task`
Sets up the LVGL display buffer and flush callback for rendering.

### `my_flush_cb`
Flushes the LVGL buffer to the SSD1306 display by converting row-major data to column-major format.

### `startup_screen`
Displays a startup logo on the SSD1306 OLED display.

### `test_draw`
Creates a scrolling text animation on the display.

### `example_main`
The main entry point of the project. Initializes the system, configures peripherals, and starts the display tasks.

## Hardware Requirements

- **SSD1306 OLED Display**: A 128x64 pixel OLED display.
- **Maxim Platform**: A development board compatible with the Maxim SDK.
- **I2C Interface**: For communication with the SSD1306 display.

## Build and Run

1. Clone the repository:
   ```bash
   git clone https://github.com/analogdevicesinc/no-os.git
   cd no-os/projects/ssd1306
   ```

2. Initialize submodule
   ```bash
   git submodule update --init --remote libraries/lvgl/lvgl
   ```

3. Build and flash (go to path:```projects/ssd1306/```)
   ```bash
   export MAXIM_LIBRARIES=<path_to_sdk>
   export PLATFORM=maxim
   export TARGET=max78000
   make run
   ```

## NOTE:
Using RTOS implementation, one must make sure to include the ```lc_tick_inc()``` function in the system timer callback, in order to have regular updates on the screen. In baremetal solution, like this project, in the file: \
```drivers/platform/maxim/max78000/maxim_delay.c``` \
needs the following changes:
```console
void SysTick_Handler(void)
{
   MXC_DelayHandler();
+  lv_tick_inc(); 
   _system_ticks++;
}
```