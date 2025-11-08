Exercise 3: I2C Communication and OLED Display
===============================================

Overview
--------

Exercise 3 introduces I2C communication and visual display output by reading battery voltage from the MAX20303 fuel gauge and displaying it on an SSD1306 OLED screen. This exercise demonstrates multi-peripheral integration and the use of GPIO bit-banging for I2C.

**Learning Objectives:**

- Initialize and use I2C communication
- Interface with the MAX20303 battery management IC
- Implement GPIO bit-banging for I2C when hardware I2C is unavailable
- Initialize and control an OLED display
- Integrate multiple sensors and peripherals in a single application
- Format and display data graphically

Hardware Setup
--------------

Required Components
~~~~~~~~~~~~~~~~~~~

- MAX32655 FTHR development board
- FTHR to PMOD adapter (:adi:`FTHR-PMD-INTZ`)
- SSD1306 128x64 OLED display PMOD module
- USB cable for programming and serial communication
- Lithium battery

Pin Connections
~~~~~~~~~~~~~~~

+------------------+------------------+--------------------+------------------+
| Function         | MAX32655 Pin     | Connected To       | FTHR-PMD-INTZ    |
+==================+==================+====================+==================+
| UART TX          | P0.9             | USB Serial         | —                |
+------------------+------------------+--------------------+------------------+
| UART RX          | P0.8             | USB Serial         | —                |
+------------------+------------------+--------------------+------------------+
| I2C1 SDA         | P0.15            | MAX20303 SDA       | —                |
+------------------+------------------+--------------------+------------------+
| I2C1 SCL         | P0.14            | MAX20303 SCL       | —                |
+------------------+------------------+--------------------+------------------+
| GPIO SDA         | P0.30            | SSD1306 SDA        | P7               |
+------------------+------------------+--------------------+------------------+
| GPIO SCL         | P0.31            | SSD1306 SCL        | P7               |
+------------------+------------------+--------------------+------------------+
| Power            | 3.3V             | Peripherals VDD    | VDD (P7)         |
+------------------+------------------+--------------------+------------------+
| Ground           | GND              | Peripherals GND    | GND (P7)         |
+------------------+------------------+--------------------+------------------+

Hardware Connection Diagram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   MAX32655 FTHR Board + FTHR-PMD-INTZ Adapter
   ┌────────────────────────────────────────────────────┐
   │                                                    │
   │  P0.9  (UART_TX)    ──────► USB Serial            │
   │  P0.8  (UART_RX)    ◄────── USB Serial            │
   │                                                    │
   │  P0.15 (I2C1_SDA)   ◄─────► MAX20303 SDA          │
   │  P0.14 (I2C1_SCL)   ──────► MAX20303 SCL          │
   │                                                    │
   │                      FTHR-PMD-INTZ Adapter        │
   │                      ┌──────────┐                 │
   │  P0.30 (GPIO_SDA) ───┤ P7       ├──► SSD1306 PMOD │
   │  P0.31 (GPIO_SCL) ───┤          │    (SDA/SCL)    │
   │  3.3V ───────────────┤ VDD      │                 │
   │  GND  ───────────────┤ GND      │                 │
   │                      └──────────┘                 │
   │                                                    │
   │  3.3V ──────────────────► MAX20303 VDD            │
   │  GND  ──────────────────► MAX20303 GND            │
   └────────────────────────────────────────────────────┘

   I2C Addresses:
   - MAX20303 Main:     0x28
   - MAX20303 Fuel Gauge: 0x36
   - SSD1306 OLED:      0x3C

   I2C Configuration:
   - Speed: 400 kHz (Fast Mode)

Functional Description
----------------------

Program Flow
~~~~~~~~~~~~

1. **System Initialization**:

   - Initialize UART for debug output (115200 baud)
   - Initialize GPIO port 0 for bit-banging the I2C signals
   - Initialize I2C bit-bang controller for OLED (P0.30, P0.31)
   - Initialize the I2C controller connected to the MAX20303

2. **Display Initialization**:

   - Configure the SSD1306 OLED using its driver
   - Clear the display
   - Display static labels: "Battery Voltage:" and "mV"
   - Most of the code in this section is commented. The user should understand the parameter struct hierarchy and uncomment the code as they go.

3. **MAX20303 Initialization**:

   - Initialize MAX20303 using its driver
   - Disable hibernate mode to enable readings
   - Configure for battery voltage monitoring

4. **Main Loop**:

   - Read battery voltage in microvolts using ``max20303_read_vcell()``
   - Convert voltage to millivolts
   - Format voltage as string
   - Update OLED display with new voltage value
   - Print voltage to UART console
   - Delay 1 second between readings

Expected Output
~~~~~~~~~~~~~~~

**Serial console:**

.. code-block:: text

   MAX20303 Battery Monitor Exercise 3
   Display initialized
   MAX20303 initialized
   Battery voltage: 4182 mV
   Battery voltage: 4180 mV
   Battery voltage: 4179 mV

**OLED display:**

.. code-block:: text

   ┌──────────────────────┐
   │ Battery Voltage:     │
   │ 4182 mV              │
   │                      │
   │                      │
   └──────────────────────┘

CAPI Object Mapping
-------------------

I2C Hardware Controller
~~~~~~~~~~~~~~~~~~~~~~~

**Hardware I2C for MAX20303:**

.. code-block:: c

   // Hardware: I2C1 peripheral on MAX32655
   struct capi_i2c_controller_handle *i2c_controller;

   // I2C controller configuration
   struct capi_i2c_config i2c_config = {
       .identifier = 1,              // I2C1 peripheral
       .initiator = 1,               // Master mode
       .clk_freq_hz = 400000,        // 400 kHz
       .ops = &maxim_i2c_ops    // Platform-specific operations
   };

   // Initialize I2C controller
   capi_i2c_init(&i2c_controller, &i2c_config);

I2C Device Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

**MAX20303 I2C devices:**

.. code-block:: c

   // Main MAX20303 device (0x28)
   struct capi_i2c_device max20303_i2c_dev = {
       .controller = i2c_controller,
       .address = MAX20303_I2C_ADDR,  // 0x28
       .b10addr = false,               // 7-bit addressing
       .speed = CAPI_I2C_SPEED_FAST,   // 400 kHz
   };

   // Fuel gauge device (0x36)
   struct capi_i2c_device max20303_fg_i2c_dev = {
       .controller = i2c_controller,
       .address = MAX20303_FG_I2C_ADDR, // 0x36
       .b10addr = false,
       .speed = CAPI_I2C_SPEED_FAST,
   };

The MAX20303 uses two I2C addresses: one for the main PMIC functionality and one for the fuel gauge.

GPIO Bit-Bang I2C
~~~~~~~~~~~~~~~~~

**I2C bit-banging for OLED:**

.. code-block:: c

   // GPIO pins for bit-banging
   struct capi_gpio_pin sda_pin = {
       .port_handle = gpio_port0,
       .number = 30,
       .flags = CAPI_GPIO_ACTIVE_HIGH,
   };

   struct capi_gpio_pin scl_pin = {
       .port_handle = gpio_port0,
       .number = 31,
       .flags = CAPI_GPIO_ACTIVE_HIGH,
   };

   // Bit-bang I2C configuration
   struct capi_i2c_bitbang_extra bitbang_extra = {
       .sda_pin = sda_pin,
       .scl_pin = scl_pin,
       .pull_type = CAPI_I2C_BITBANG_PULL_EXTERNAL,
       .timeout_us = 100000,
   };

   struct capi_i2c_config bitbang_config = {
       .ops = &capi_i2c_bitbang_ops,
       .identifier = 0,
       .clk_freq_hz = 400000,
       .initiator = true,
       .extra = &bitbang_extra,
   };

   // Initialize bit-bang I2C
   capi_i2c_init(&bitbang_i2c, &bitbang_config);

Display Integration
~~~~~~~~~~~~~~~~~~~

**SSD1306 OLED initialization:**

.. code-block:: c

   // OLED I2C device
   struct capi_i2c_device ssd_1306_i2c_dev = {
       .address = 0x3C,
       .speed = 400000,
       .controller = bitbang_i2c,
   };

   // Display configuration
   ssd_1306_capi_extra oled_extra = {
       .i2c_desc = &ssd_1306_i2c_dev,
       .comm_type = SSD1306_I2C,
   };

   struct display_init_param oled_param = {
       .cols_nb = 128,
       .rows_nb = 64,
       .controller_ops = &ssd1306_ops,
       .extra = &oled_extra,
   };

   // Initialize display
   display_init(&oled_display, &oled_param);
   display_clear(oled_display);
   display_on(oled_display);

   // Display text
   display_print_string(oled_display, "Battery Voltage:", 0, 0);

Build and Run
-------------

Configuration
~~~~~~~~~~~~~

The project defconfig is defined in ``project_ex3.conf``:

.. code-block:: kconfig

   CONFIG_EXERCISE_3=y

   CONFIG_CAPI=y
   CONFIG_UART=y
   CONFIG_I2C=y
   CONFIG_SPI=y
   CONFIG_IRQ=y
   CONFIG_GPIO=y

   CONFIG_CHARGER=y
   CONFIG_CHARGER_MAX20303_CAPI=y

   CONFIG_DISPLAY=y
   CONFIG_DISPLAY_SSD1306_CAPI=y
   CONFIG_PLATFORM_I2C_BITBANG=y

This enables:

- CAPI framework
- UART, I2C, SPI, and GPIO drivers
- MAX20303 battery management driver
- SSD1306 display driver
- I2C bit-banging support

Build Commands
~~~~~~~~~~~~~~

.. code-block:: bash

   # Configure CMake build
   cmake --preset max32655_fthr --fresh -B build_ex3 \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex3.conf

   # Build the project
   cmake --build build_ex3 --target ftc_workshop

   # Flash to the board
   cmake --build build_ex3 --target flash

Output binary location: ``build_ex3/build/ftc_workshop.elf``

Running the Example
~~~~~~~~~~~~~~~~~~~

1. **Connect hardware**:

   - Connect MAX20303 to I2C1 (P0.14, P0.15)
   - Connect SSD1306 OLED to GPIO pins (P0.30, P0.31)
   - Ensure all devices share common ground
   - (Optional) Connect a battery to MAX20303 for real voltage readings

2. **Flash the firmware** using the build commands above

3. **Open serial terminal** at 115200 baud:

   .. code-block:: bash

      # Linux/macOS
      screen /dev/ttyACM0 115200

      # Or with minicom
      minicom -D /dev/ttyACM0 -b 115200

4. **Observe**: Battery voltage displayed on OLED and printed to console every second

Testing the System
~~~~~~~~~~~~~~~~~~

1. **Without battery**: MAX20303 will report 0V

2. **With battery**:

   - Lithium battery: Expect 3.7-4.2V depending on charge state
   - Display updates every second
   - Voltage should be stable

Next Steps
----------

Once you understand Exercise 3, proceed to :doc:`ftc_workshop_capi_exercise_4` to learn about:

- FreeRTOS task management
- Step counting algorithm

Additional Resources
--------------------

- :dokuwiki:`CAPI I2C Documentation </resources/no-os/capi/i2c>`
- `MAX20303 Datasheet <https://www.analog.com/MAX20303>`_
- `SSD1306 OLED Datasheet <https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf>`_
- `I2C Protocol Tutorial <https://i2c.info/>`_
