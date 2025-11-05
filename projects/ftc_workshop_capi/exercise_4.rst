Exercise 4: Complete Fitness Tracker with BLE
==============================================

Overview
--------

Exercise 4 combines all previous exercises into a pedometer application with Bluetooth Low Energy connectivity. This example demonstrates real-time operating system usage, multi-sensor integration, step counting algorithms, and wireless data transmission.

**Learning Objectives:**

- Integrate FreeRTOS for task management
- Implement Bluetooth Low Energy using Cordio stack
- Manage multiple peripherals concurrently
- Create GATT services for data exposure

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
| I2C1 SDA         | P0.15            | MAX20303 SDA       | —                |
+------------------+------------------+--------------------+------------------+
| I2C1 SCL         | P0.14            | MAX20303 SCL       | —                |
+------------------+------------------+--------------------+------------------+
| GPIO SDA         | P0.30            | SSD1306 SDA        | P7               |
+------------------+------------------+--------------------+------------------+
| GPIO SCL         | P0.31            | SSD1306 SCL        | P7               |
+------------------+------------------+--------------------+------------------+
| BLE Radio        | Internal         | Antenna            | —                |
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
   │                                                           │
   │  P0.15 (I2C1_SDA) ◄─────► MAX20303 (on-board)            │
   │  P0.14 (I2C1_SCL) ──────► MAX20303 (battery fuel gauge)  │
   │                                                           │
   │  BLE Radio ◄────────► Antenna (Internal/External)        │
   └───────────────────────────────────────────────────────────┘

Functional Description
----------------------

Program Flow
~~~~~~~~~~~~

1. **System Initialization**:

   - Initialize FreeRTOS kernel
   - Initialize SPI controller for ADXL355
   - Initialize I2C controllers (hardware and bit-bang)
   - Initialize Cordio BLE stack

2. **Peripheral Initialization**:

   - Configure ADXL355 accelerometer (62.5 Hz ODR)
   - Configure MAX20303 battery management
   - Initialize SSD1306 OLED display
   - Display initial UI elements

3. **BLE Stack Initialization**:

   - Configure WSF (Wireless Software Foundation)
   - Initialize Link Layer and Baseband
   - Set up GATT services:

     - Battery Service (report battery percentage)
     - Running Speed and Cadence Service (report step count)

4. **FreeRTOS Task (bt_task)**:

   - Read accelerometer FIFO data
   - Detect steps using peak detection algorithm
   - Update step counter
   - Read battery voltage and percentage
   - Update OLED display with steps and battery
   - Update the BLE GATT characteristics specific to battery and step count
   - Delay 50ms between iterations

Step Counting Algorithm
~~~~~~~~~~~~~~~~~~~~~~~

The application implements a simple peak detection algorithm:

1. **Data Acquisition**:

   - Read X, Y, Z acceleration from ADXL355 FIFO
   - Compute vector magnitude: ``sum = X + Y + Z``

2. **Low-Pass Filtering**:

   - Apply 4-sample moving average filter
   - Reduces high-frequency noise from vibration

3. **Peak Detection**:

   - Maintain sliding windows of max and min peaks
   - Detect step when:

     - Maximum peak exceeds threshold + sensitivity/2
     - Minimum peak below threshold - sensitivity/2
     - Peak-to-peak difference > sensitivity threshold

4. **Step Increment**:

   - Increment global step counter
   - Update display and BLE characteristics

Expected Output
~~~~~~~~~~~~~~~

**Serial console:**

.. code-block:: text

   Starting BLE in Peripheral mode
   Cordio initialization completed - GATT services should be available
   GATT services available:
   - Core GATT services (GAP/GATT)
   - Battery Service - read battery level
   - Running speed and cadence service - read step count
   Battery voltage: 3987 mV
   Battery percentage: 85

**OLED display:**

.. code-block:: text

   ┌──────────────────────┐
   │ Steps: 42            │
   │ Battery: 85%         │
   │ [████████░░]         │
   └──────────────────────┘

**BLE Advertisement:**

- Device name: "FTC_Workshop"
- Services: Battery, Running Speed and Cadence

CAPI Object Mapping
-------------------

FreeRTOS Integration
~~~~~~~~~~~~~~~~~~~~

**Task creation:**

.. code-block:: c

   int create_tasks(void)
   {
       TaskHandle_t bt_task_handle = NULL;

       // Create BLE/sensor task
       xTaskCreate(bt_task,           // Task function
                   "BT task",         // Task name
                   2048,              // Stack size (words)
                   NULL,              // Parameters
                   tskIDLE_PRIORITY + 1, // Priority
                   &bt_task_handle);

       // Start FreeRTOS scheduler
       vTaskStartScheduler();

       return 0;
   }

FreeRTOS provides task scheduling, allowing the BLE stack and sensor processing to run concurrently with proper time management. FreeRTOS is partially integrated into the no-OS API (mutex, semaphore
and delay API). Other features (such as threads, queues and events operation) need to be handled in the application layer using the FreeRTOS API directly.

Multi-Peripheral Coordination
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All peripherals from previous exercises are initialized and used concurrently:

.. code-block:: c

   void bt_task(void *pvParameters)
   {
       // Initialize all peripherals
       capi_spi_init(&spi_controller, &adxl355_spi_config);
       capi_i2c_init(&i2c_controller, &i2c_config);
       init_display();  // Bit-bang I2C for OLED

       // Initialize drivers
       adxl355_capi_init(&adxl355, adxl355_param);
       max20303_init(&max20303, &max20303_param);

       // Initialize Cordio BLE stack
       cordio_init();

       // Main processing loop
       while (1) {
           // Read sensors
           adxl355_capi_get_fifo_data(...);
           max20303_read_vcell(...);
           max20303_read_soc(...);

           // Process data
           step_count(...);

           // Update displays
           display_battery_level(...);
           display_print_string(...);

           // Update BLE
           AppHwBattTest(battery_percentage);

           vTaskDelay(pdMS_TO_TICKS(50));
       }
   }

Bluetooth Low Energy Stack
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Cordio stack initialization:**

.. code-block:: c

   void cordio_init(void)
   {
       // Initialize WSF
       WsfInit();

       // Initialize Link Layer
       LlInit();

       // Initialize Baseband
       BbInit();

       // Initialize application
       AppInit();

       // Start advertising
       AppStart();
   }

The Cordio stack manages:

- Link Layer (physical radio control)
- Baseband (packet formatting)
- HCI (Host Controller Interface)
- L2CAP (Logical Link Control)
- ATT/GATT (Attribute Protocol/Generic Attribute Profile)

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
   CONFIG_IRQ=y
   CONFIG_GPIO=y

   CONFIG_FREERTOS=y
   CONFIG_FREERTOS_CONF_PATH="projects/ftc_workshop_capi/src/platform/maxim"

   CONFIG_CORDIO=y

   CONFIG_ACCEL=y
   CONFIG_ACCEL_ADXL355_CAPI=y

   CONFIG_CHARGER=y
   CONFIG_CHARGER_MAX20303_CAPI=y

   CONFIG_DISPLAY=y
   CONFIG_DISPLAY_SSD1306_CAPI=y
   CONFIG_PLATFORM_I2C_BITBANG=y

This enables:

- All CAPI peripherals (UART, SPI, I2C, GPIO)
- FreeRTOS
- Cordio Bluetooth Low Energy stack
- All sensor and display drivers

Build Commands
~~~~~~~~~~~~~~

.. code-block:: bash

   # Configure CMake build
   cmake --preset max32655_fthr --fresh -B build_ex4 \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex4.conf

   # Build the project
   cmake --build build_ex4 --target ftc_workshop

   # Flash to the board
   cmake --build build_ex4 --target flash

Output binary location: ``build_ex4/build/ftc_workshop.elf``

Running the Example
~~~~~~~~~~~~~~~~~~~

1. **Connect hardware**:

   - Attach FTHR-PMD-INTZ adapter to MAX32655 FTHR board
   - Connect ADXL355 PMOD to the adapter's PMOD port
   - Connect MAX20303 to I2C1 port on the adapter
   - Connect SSD1306 OLED to GPIO pins (P0.30, P0.31)
   - Ensure all devices share common ground via the adapter
   - (Optional) Connect a battery to MAX20303 for real voltage readings

2. **Flash the firmware** using the build commands above

3. **Open serial terminal** at 115200 baud to see debug output

4. **Test step counting**:

   - Shake or tap the board with accelerometer attached
   - Watch step count increment on OLED
   - Observe console output

5. **Test BLE connectivity** using the provided Python script

Testing with simple_ble_test.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The project includes a Python script specifically designed to test this application's BLE functionality. The script connects to the device and displays step count and battery percentage in real-time.

**Prerequisites:**

.. code-block:: bash

   source build_ex4/.no_os_venv

   # Install required Python packages
   pip3 install bleak asyncio rich

**Running the test script:**

.. code-block:: bash

   # Run the BLE test script
   python3 projects/ftc_workshop_capi/scripts/ble_test.py

The device will stop advertising after 180 s. You can press the S1 button on the FTHR-PMD-INTZ board in order to reset the MCU.

The script will continuously monitor and display updates from the fitness tracker. Press Ctrl+C to stop.

Testing the Complete System
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Display Test**:

   - Verify step count and battery percentage display on OLED
   - Check battery icon animation
   - Ensure text is crisp and readable

2. **Step Counter Test**:

   - Hold board in hand and simulate walking motion
   - Steps should increment on both OLED and BLE
   - Try different motion patterns (walking, running, jumping)
   - Run ``simple_ble_test.py`` to verify BLE updates match display

3. **Battery Monitoring**:

   - With battery connected: See real voltage and percentage
   - Without battery: May show 0V or invalid data
   - Verify BLE Battery Service reports same percentage as display

4. **BLE Connectivity**:

   - Run ``simple_ble_test.py`` to connect
   - Walk around while connected - verify step updates
   - Disconnect and verify device continues functioning
   - Reconnect - verify connection restoration

Advanced Features
-----------------

Digital Signal Processing
~~~~~~~~~~~~~~~~~~~~~~~~~

The low-pass filter smooths acceleration data:

.. code-block:: c

   static void apply_lpf(int32_t *buffer, uint32_t len)
   {
       for (int i = len - 1; i >= 3; i--) {
           buffer[i] = buffer[i] + buffer[i-1] +
                       buffer[i-2] + buffer[i-3];
           buffer[i] /= 4;  // 4-sample moving average
       }
   }

This reduces noise without requiring floating-point operations.

Dynamic Threshold Adaptation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The step detection threshold adapts to walking intensity:

.. code-block:: c

   if (max_peak_val - min_peak_val > sensitivity) {
       threshold = max_peak_val - min_peak_val;
       return 1;  // Step detected
   }

This allows the algorithm to work for both slow walking and running.

Conclusion
----------

Exercise 4 demonstrates a complete embedded systems application with:

- Multi-sensor integration via CAPI framework
- Real-time processing with FreeRTOS
- Wireless connectivity using Cordio BLE stack
- Power management with MAX20303
- User interface on OLED display
- Convenient hardware interfacing with FTHR-PMD-INTZ adapter

Additional Resources
--------------------

- :dokuwiki:`FreeRTOS Integration </resources/no-os/freertos>`
- :dokuwiki:`Cordio BLE Stack </resources/no-os/cordio>`
- :adi:`FTHR-PMD-INTZ Adapter Documentation <fthr-pmd-intz>`
- `Bluetooth Low Energy Specification <https://www.bluetooth.com/specifications/specs/core-specification/>`_
- `FreeRTOS Documentation <https://www.freertos.org/Documentation/RTOS_book.html>`_
- `Step Detection Algorithms Paper <https://doi.org/10.1109/JSEN.2014.2360391>`_
