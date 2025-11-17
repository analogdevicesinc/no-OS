Exercise 4: FreeRTOS-based Pedometer
=====================================

Overview
--------

Exercise 4 builds a pedometer application using FreeRTOS for task management. This exercise combines the ADXL355 accelerometer with the SSD1306 OLED display to create a real-time step counter with visual feedback.

**Learning Objectives:**

- Integrate FreeRTOS for task management
- Implement step counting algorithms with peak detection
- Manage multiple peripherals in a real-time OS environment

Hardware Setup
--------------

Required Components
~~~~~~~~~~~~~~~~~~~

- MAX32655 FTHR development board
- FTHR to PMOD adapter (:adi:`FTHR-PMD-INTZ`)
- ADXL355 3-axis accelerometer PMOD evaluation board
- SSD1306 128x64 OLED display PMOD module
- USB cable for programming and serial communication
- (Optional) Lithium battery for portable operation

The FTHR to PMOD adapter provides a convenient interface for connecting PMOD-compatible modules to the FTHR board without requiring individual jumper wires. It breaks out the necessary SPI, I2C, GPIO, and power connections in a standardized PMOD format. The ADXL355 connects to P6 and the SSD1306 display connects to P7.

Complete Pin Connections
~~~~~~~~~~~~~~~~~~~~~~~~

+------------------+------------------+--------------------+------------------+
| Function         | MAX32655 Pin     | Connected To       | FTHR-PMD-INTZ    |
+==================+==================+====================+==================+
| UART TX          | P0.9             | USB Serial         | —                |
+------------------+------------------+--------------------+------------------+
| UART RX          | P0.8             | USB Serial         | —                |
+------------------+------------------+--------------------+------------------+
| SPI CLK          | P0.6             | ADXL355 SCK        | P6               |
+------------------+------------------+--------------------+------------------+
| SPI MOSI         | P0.5             | ADXL355 SDI        | P6               |
+------------------+------------------+--------------------+------------------+
| SPI MISO         | P0.4             | ADXL355 SDO        | P6               |
+------------------+------------------+--------------------+------------------+
| SPI CS           | P0.7             | ADXL355 CS         | P6               |
+------------------+------------------+--------------------+------------------+
| GPIO SDA         | P0.30            | SSD1306 SDA        | P7               |
+------------------+------------------+--------------------+------------------+
| GPIO SCL         | P0.31            | SSD1306 SCL        | P7               |
+------------------+------------------+--------------------+------------------+
| Power            | 3.3V             | All peripherals    | VDD (P6, P7)     |
+------------------+------------------+--------------------+------------------+
| Ground           | GND              | All peripherals    | GND (P6, P7)     |
+------------------+------------------+--------------------+------------------+

Hardware Connection Diagram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   MAX32655 FTHR Board + FTHR-PMD-INTZ Adapter
   ┌───────────────────────────────────────────────────────────┐
   │                                                           │
   │  P0.9  (UART_TX)    ──────► USB Serial                   │
   │  P0.8  (UART_RX)    ◄────── USB Serial                   │
   │                                                           │
   │                   FTHR-PMD-INTZ Adapter                  │
   │                   ┌────────────────────┐                 │
   │  P0.6 (SPI_SCK) ──┤ P6                 ├──► ADXL355 PMOD │
   │  P0.5 (SPI_MOSI)──┤                    │    (3-axis      │
   │  P0.4 (SPI_MISO)──┤                    │    accel)       │
   │  P0.7 (SPI_CS)  ──┤                    │                 │
   │                   │                    │                 │
   │  P0.30 (GPIO_SDA)─┤ P7                 ├──► SSD1306 PMOD│
   │  P0.31 (GPIO_SCL)─┤                    │    (OLED        │
   │                   │                    │    display)     │
   │  3.3V ────────────┤ VDD                │                 │
   │  GND  ────────────┤ GND                │                 │
   │                   └────────────────────┘                 │
   └───────────────────────────────────────────────────────────┘

Functional Description
----------------------

Program Flow
~~~~~~~~~~~~

1. **System Initialization**:

   - Initialize FreeRTOS kernel
   - Create sensor processing task
   - Note: the application is using a single FreeRTOS task

2. **Task Initialization (bt_task)**:

   - Initialize SPI controller for ADXL355
   - Initialize GPIO-based I2C for SSD1306 display
   - Configure ADXL355 accelerometer (62.5 Hz ODR)
   - Initialize and configure SSD1306 OLED display
   - Display initial "Steps: 0" text

3. **Main Task Loop**:

   - Read accelerometer FIFO data (X, Y, Z axes)
   - Sum the acceleration for all the axes
   - Apply step detection algorithm once we have 2 * STEP_WINDOW_SIZE samples (by default STEP_WINDOW_SIZE = 12)
   - Update step counter when steps detected
   - Periodically update OLED display (every 20 iterations)
   - Delay 50ms between iterations

Step Counting Algorithm
~~~~~~~~~~~~~~~~~~~~~~~

The application implements a peak detection algorithm with low-pass filtering:

1. **Data Acquisition**:

   - Read X, Y, Z acceleration from ADXL355 FIFO
   - Compute vector magnitude: ``sum = X + Y + Z``

2. **Low-Pass Filtering**:

   - Apply 4-sample moving average filter
   - Reduces high-frequency noise from vibration

3. **Peak Detection**:

   - Maintain sliding windows of max and min peaks (STEP_WINDOW_SIZE samples each). A step is defined as a peak followed by a trough, each occuring in an interval of STEP_WINDOW_SIZE samples
   - Detect step when the following are true:

     - Maximum peak exceeds threshold + sensitivity/2
     - Trough below threshold - sensitivity/2

4. **Step Increment**:

   - Increment global step counter
   - Update display every 20 loop iterations

Expected Output
~~~~~~~~~~~~~~~

**Serial console:**

.. code-block:: text

   (No step count logging to console in this exercise)
   Warning messages only appear if sensor read errors occur

**OLED display:**

.. code-block:: text

   ┌──────────────────────┐
   │ Steps: 42            │
   └──────────────────────┘

Note: The step count is displayed modulo 1000 (i.e., resets to 0 after 999).

FreeRTOS Integration
~~~~~~~~~~~~~~~~~~~~

**Task creation:**

.. code-block:: c

   int create_tasks(void)
   {
       TaskHandle_t thread1_handle = NULL;

       // Create sensor processing task
       ret = xTaskCreate(bt_task,           // Task function
                         "BT task",         // Task name
                         2048,              // Stack size (words)
                         NULL,              // Parameters
                         tskIDLE_PRIORITY + 1, // Priority
                         &thread1_handle);

       // Start FreeRTOS scheduler
       vTaskStartScheduler();

       return 0;
   }

FreeRTOS provides task scheduling and delay functions. The main sensor processing runs in the ``bt_task``.

Multi-Peripheral Coordination
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All peripherals are initialized within the FreeRTOS task:

.. code-block:: c

   void bt_task(void *pvParameters)
   {
       // Initialize SPI for accelerometer
       capi_spi_init(&spi_controller, &adxl355_spi_config);

       // Initialize display (uses GPIO I2C bitbang)
       init_display();

       // Initialize ADXL355 driver
       adxl355_init(&adxl355, adxl355_param);
       adxl355_soft_reset(adxl355);
       adxl355_set_odr_lpf(adxl355, ADXL355_ODR_62_5HZ);
       adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);

       // Display initial text
       display_print_string(oled_display, "Steps: ", 0, 0);
       display_print_string(oled_display, "0", 0, 6);

       // Main processing loop
       while (1) {
           // Read FIFO data
           adxl355_get_fifo_data(adxl355, &fifo_entries,
                                 x_accel, y_accel, z_accel);

           // Compute acceleration sum
           for (int i = 0; i < fifo_entries; i++) {
               accel_sum[i] = (x_accel[i] + y_accel[i] + z_accel[i]) / 10;
           }

           // Detect steps
           ret = step_count(accel_sum, fifo_entries);
           if (ret)
               total_step_count += ret;

           // Update display every 20 iterations
           if (i >= 20) {
               sprintf(steps_display_buffer, "%d", total_step_count % 1000);
               display_print_string(oled_display, steps_display_buffer, 0, 6);
               i = 0;
           }

           i++;
           vTaskDelay(pdMS_TO_TICKS(50));
       }
   }

Build and Run
-------------

Configuration
~~~~~~~~~~~~~

The project defconfig is defined in ``project_ex4.conf``:

.. code-block:: kconfig

   CONFIG_EXERCISE_4=y

   CONFIG_CAPI=y
   CONFIG_UART=y
   CONFIG_SPI=y
   CONFIG_I2C=y
   CONFIG_GPIO=y

   CONFIG_FREERTOS=y
   CONFIG_FREERTOS_CONF_PATH="projects/ftc_workshop_capi/src/platform/maxim"

   CONFIG_ACCEL=y
   CONFIG_ACCEL_ADXL355=y

   CONFIG_DISPLAY=y
   CONFIG_DISPLAY_SSD1306=y
   CONFIG_PLATFORM_I2C_BITBANG=y

This enables:

- All CAPI peripherals (UART, SPI, I2C, GPIO)
- FreeRTOS with platform-specific configuration
- ADXL355 accelerometer driver
- SSD1306 display driver with I2C bitbang

Build Commands
~~~~~~~~~~~~~~

For complete build instructions including Windows-specific syntax, refer to the main :doc:`ftc_workshop_capi` documentation.

**Linux/macOS:**

.. code-block:: bash

   # Configure CMake build
   cmake --preset max32655_fthr -B ftc_workshop_build \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex4.conf --fresh

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

**Windows (PowerShell):**

.. code-block:: powershell

   # Configure CMake build (note: quotes required for paths with /)
   cmake --preset max32655_fthr -B ftc_workshop_build `
     -DPROJECT_DEFCONFIG="ftc_workshop_capi/project_ex4.conf" --fresh

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

**Windows (Command Prompt):**

.. code-block:: batch

   # Configure CMake build
   cmake --preset max32655_fthr -B ftc_workshop_build ^
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex4.conf --fresh

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

Output binary location: ``ftc_workshop_build/build/ftc_workshop_capi.elf`` (Linux/macOS) or ``ftc_workshop_build\build\ftc_workshop_capi.elf`` (Windows)

Running the Example
~~~~~~~~~~~~~~~~~~~

1. **Connect hardware**:

   - Attach FTHR-PMD-INTZ adapter to MAX32655 FTHR board
   - Connect ADXL355 PMOD to P6 on the adapter
   - Connect SSD1306 OLED to P7 on the adapter
   - Connect USB cable for power and serial communication

2. **Flash the firmware** using the build commands above

3. **Observe the display**:

   - OLED should show "Steps: 0" initially
   - Watch the step count update as you move the board

4. **Test step counting**:

   - Hold the board and simulate walking motion
   - Shake or tap the board to generate step events
   - Watch the step count increment on the OLED

Testing the System
~~~~~~~~~~~~~~~~~~

1. **Display Test**:

   - Verify "Steps: 0" displays on power-up

2. **Step Counter Test**:

   - Hold board in hand and simulate walking motion
   - Steps should increment on display
   - Note: Count may not be perfectly accurate - algorithm is simplified

3. **Sensitivity Tuning**:

   - The sensitivity threshold is set to 150 (line 81 in main.c)
   - Adjust this value if too sensitive or not sensitive enough
   - Higher values = less sensitive (fewer false positives)
   - Lower values = more sensitive (may detect more false steps)

Conclusion
----------

Exercise 4 demonstrates a FreeRTOS-based pedometer application with:

- Real-time task management using FreeRTOS
- Multi-sensor integration via CAPI framework
- Peak detection algorithm for step counting
- Visual feedback on OLED display
- Convenient hardware interfacing with FTHR-PMD-INTZ adapter

Next Steps
----------

In Exercise 5, you'll extend this application by adding:

- Bluetooth Low Energy connectivity for wireless data transmission
- Battery monitoring with MAX20303 power management IC
- GATT services for exposing step count and battery data to mobile devices
- Python test script for BLE connectivity validation

Additional Resources
--------------------

- :dokuwiki:`FreeRTOS Integration </resources/no-os/freertos>`
- :adi:`FTHR-PMD-INTZ Adapter Documentation <fthr-pmd-intz>`
- `Step Detection Algorithms Paper <https://doi.org/10.1109/JSEN.2014.2360391>`_
- :dokuwiki:`ADXL355 Driver Documentation </resources/no-os/drivers/accel/adxl355>`
- :dokuwiki:`SSD1306 Display Driver </resources/no-os/drivers/display/ssd1306>`
- `FreeRTOS Documentation <https://www.freertos.org/Documentation/RTOS_book.html>`_
