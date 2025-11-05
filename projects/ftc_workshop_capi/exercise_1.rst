Exercise 1: GPIO Input with UART Output
========================================

Overview
--------

Exercise 1 introduces the fundamentals of the CAPI framework by demonstrating GPIO input and UART communication. This exercise reads the state of two push buttons and displays their status via UART console output.

**Learning Objectives:**

- Initialize UART for console communication
- Configure GPIO pins for input with internal pull-ups
- Read digital input states
- Use CAPI abstractions for portable peripheral access
- Understand static memory allocation in embedded systems

Hardware Setup
--------------

Required Components
~~~~~~~~~~~~~~~~~~~

- MAX32655 FTHR development board
- USB cable for programming and serial communication
- Two push buttons (if not built into the board)

Pin Connections
~~~~~~~~~~~~~~~

+------------------+------------------+--------------------------+
| Function         | MAX32655 Pin     | Hardware Connection      |
+==================+==================+==========================+
| UART TX          | P0.9             | USB-Serial (console)     |
+------------------+------------------+--------------------------+
| UART RX          | P0.8             | USB-Serial (console)     |
+------------------+------------------+--------------------------+
| Button 1         | P0.2             | Push button (active low) |
+------------------+------------------+--------------------------+
| Button 2         | P0.3             | Push button (active low) |
+------------------+------------------+--------------------------+

Hardware Connection Diagram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   MAX32655 FTHR Board
   ┌────────────────────────────────────┐
   │                                    │
   │  P0.9 (UART_TX) ──────► USB Serial│
   │  P0.8 (UART_RX) ◄────── USB Serial│
   │                                    │
   │  P0.2 (Button1) ◄─┐               │
   │                   └─ [Button] ─ GND
   │  P0.3 (Button2) ◄─┐               │
   │                   └─ [Button] ─ GND
   │                                    │
   └────────────────────────────────────┘

   Internal Pull-up Resistors Enabled
   Button Press = Logic Low (0)
   Button Release = Logic High (1)

Functional Description
----------------------

Program Flow
~~~~~~~~~~~~

1. **System Initialization**:

   - Initialize the UART peripheral (115200 baud, 8N1)
   - Redirect ``printf()`` to UART for console output
   - Initialize GPIO port 0 with 32 pins

2. **GPIO Configuration**:

   - The user is expencted to implement this step. Follow the hints in the application code.
   - Configure P0.2 and P0.3 as inputs with internal pull-ups
   - Buttons are active-low (pressed = 0, released = 1) due to external pull up resistors.

3. **Main Loop**:

   - Read button states using ``capi_gpio_pin_get_value()``
   - Print button states to UART console
   - Delay 1 second between readings

Expected Output
~~~~~~~~~~~~~~~

When running, the serial console will display:

.. code-block:: text

   Button state: 1 1     (Both buttons released)
   Button state: 0 1     (Button 1 pressed)
   Button state: 1 0     (Button 2 pressed)
   Button state: 0 0     (Both buttons pressed)

CAPI Object Mapping
-------------------

This exercise demonstrates how hardware resources are mapped to CAPI objects:

UART Initialization
~~~~~~~~~~~~~~~~~~~

**Hardware to CAPI mapping:**

.. code-block:: c

   // Hardware: UART0 peripheral on MAX32655
   // CAPI Controller Handle
   struct capi_uart_handle *uart;

   // Configuration structure
   struct capi_uart_config uart_config = {
       .identifier = 0,           // UART0
       .clk_freq_hz = 115200,     // Baud rate
       .ops = &maxim_capi_uart_ops // Platform-specific operations
   };

   // Initialize UART controller
   capi_uart_init(&uart, &uart_config);

   // Redirect stdio to UART for printf() support
   capi_uart_stdio(uart);

The ``maxim_capi_uart_ops`` structure contains function pointers to the Maxim-specific UART driver implementation. This abstraction allows the same initialization code to work across different microcontroller families.

GPIO Initialization
~~~~~~~~~~~~~~~~~~~

**Hardware to CAPI mapping:**

.. code-block:: c

   // Hardware: GPIO Port 0 on MAX32655 (32 pins)
   struct capi_gpio_port_handle *gpio_port0;

   // Configuration for GPIO port
   struct capi_gpio_port_config gpio_port0_config = {
       .identifier = 0,           // Port 0
       .num_pins = 32,            // 32 GPIO pins
       .ops = &maxim_capi_gpio_ops // Platform-specific operations
   };

   // Initialize GPIO port
   capi_gpio_port_init(&gpio_port0, &gpio_port0_config);

   // Configure individual pins
   struct capi_gpio_pin gpio2 = {
       .port_handle = gpio_port0, // Reference to port
       .number = 2,               // Pin number within port
   };

   struct capi_gpio_pin gpio3 = {
       .port_handle = gpio_port0,
       .number = 3,
   };

GPIO pins are accessed through a pin structure that references the parent port. This hierarchical model matches the hardware organization where pins belong to ports.

Build and Run
-------------

Configuration
~~~~~~~~~~~~~

The project configuration is defined in ``project_ex1.conf``:

.. code-block:: kconfig

   CONFIG_EXERCISE_1=y

   CONFIG_CAPI=y
   CONFIG_UART=y
   CONFIG_GPIO=y
   CONFIG_IRQ=y

This enables:

- CAPI framework
- UART peripheral driver
- GPIO peripheral driver
- Interrupt controller (used internally)

Build Commands
~~~~~~~~~~~~~~

.. code-block:: bash

   # Configure CMake build
   cmake --preset max32655_fthr --fresh -B build_ex1 \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex1.conf

   # Build the project
   cmake --build build_ex1 --target ftc_workshop

   # Flash to the board
   cmake --build build_ex1 --target flash

Output binary location: ``build_ex1/build/ftc_workshop.elf``

Running the Example
~~~~~~~~~~~~~~~~~~~

1. **Connect the board** via USB (provides both power and serial communication)

2. **Open a serial terminal** at 115200 baud:

   .. code-block:: bash

      # Linux/macOS
      screen /dev/ttyACM0 115200

      # Or with minicom
      minicom -D /dev/ttyACM0 -b 115200

3. **Press the buttons** and observe the output change in real-time

4. **Expected behavior**: Button state updates every second, showing "1" for released and "0" for pressed

Troubleshooting
~~~~~~~~~~~~~~~

**No serial output:**

- Check USB cable connection
- Verify correct serial port (``ls /dev/tty*`` on Linux/macOS)
- Confirm baud rate is 115200
- Try pressing the reset button on the board

**Button states always show 1:**

- Internal pull-ups are enabled, so released buttons read as "1"
- Press buttons to see "0" state
- Check button connections (active-low configuration)

Next Steps
----------

Once you understand Exercise 1, proceed to :doc:`ftc_workshop_capi_exercise_2` to learn about:

- SPI communication
- Reading sensor data from ADXL355 accelerometer
- Processing and displaying acceleration values

Additional Resources
--------------------

- :dokuwiki:`CAPI GPIO Documentation </resources/no-os/capi/gpio>`
- :dokuwiki:`CAPI UART Documentation </resources/no-os/capi/uart>`
- `MAX32655 GPIO Specifications <https://www.analog.com/MAX32655>`_
