Exercise 2: SPI Communication with ADXL355 Accelerometer
==========================================================

Overview
--------

Exercise 2 builds upon Exercise 1 by adding SPI communication to read data from the ADXL355 3-axis MEMS accelerometer. This exercise demonstrates how to use the CAPI SPI interface to communicate with external sensors and process their data.

**Learning Objectives:**

- Initialize and configure SPI communication
- Interface with the ADXL355 accelerometer driver
- Read acceleration data on three axes (X, Y, Z)
- Read and convert temperature sensor data

Hardware Setup
--------------

Required Components
~~~~~~~~~~~~~~~~~~~

- MAX32655 FTHR development board
- FTHR to PMOD adapter (:adi:`FTHR-PMD-INTZ`)
- ADXL355 3-axis accelerometer evaluation board
- USB cable for programming and serial communication

Pin Connections
~~~~~~~~~~~~~~~

+------------------+------------------+------------------+------------------+
| Function         | MAX32655 Pin     | ADXL355 Pin      | FTHR-PMD-INTZ    |
+==================+==================+==================+==================+
| SPI CLK          | P0.6             | SCK              | P6               |
+------------------+------------------+------------------+------------------+
| SPI MOSI         | P0.5             | SDI              | P6               |
+------------------+------------------+------------------+------------------+
| SPI MISO         | P0.4             | SDO              | P6               |
+------------------+------------------+------------------+------------------+
| SPI CS           | P0.7             | CS               | P6               |
+------------------+------------------+------------------+------------------+
| Power            | 3.3V             | VDD              | VDD (P6)         |
+------------------+------------------+------------------+------------------+
| Ground           | GND              | GND              | GND (P6)         |
+------------------+------------------+------------------+------------------+

Hardware Connection Diagram
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   MAX32655 FTHR Board                    ADXL355 Accelerometer
   ┌────────────────────────────┐         ┌──────────────────────┐
   │                            │         │                      │
   │  P0.9 (UART_TX) ──────► USB         │                      │
   │  P0.8 (UART_RX) ◄────── USB         │                      │
   │                            │         │                      │
   │  P0.6 (SPI_SCK)  ─────────────────► SCK                   │
   │  P0.5 (SPI_MOSI) ─────────────────► SDI                   │
   │  P0.4 (SPI_MISO) ◄─────────────────  SDO                  │
   │  P0.7 (SPI_CS)   ─────────────────► CS                    │
   │                            │         │                      │
   │  3.3V ───────────────────────────►  VDD                   │
   │  GND  ───────────────────────────►  GND                   │
   │                            │         │                      │
   └────────────────────────────┘         └──────────────────────┘

   SPI Configuration:
   - Mode: 0 (CPOL=0, CPHA=0)
   - Speed: 1 MHz
   - Bit order: MSB first
   - CS active low

Functional Description
----------------------

Program Flow
~~~~~~~~~~~~

1. **System Initialization**:

   - Initialize UART for console output (115200 baud)
   - Initialize SPI controller at 1 MHz
   - Configure SPI device for ADXL355 (Mode 0, CS on pin 0)

2. **ADXL355 Initialization**:

   - As part of the exercise, the user is expected to call the initialization function. Follow the hints in the source code.
   - Perform soft reset to ensure clean state
   - Configure output data rate to 62.5 Hz
   - Set operation mode to measurement with temperature enabled

3. **Main Loop**:

   - As part of the exercise, the user is expected to call the function which reads a single acceleration measurement. Follow the hints in the source code.
   - Read temperature data (in Celsius)
   - Format and print data to UART console
   - Sleep for 500ms between readings

Expected Output
~~~~~~~~~~~~~~~

When running, the serial console will display:

.. code-block:: text

   ADXL355 Accelerometer Exercise 2
   ADXL355 initialized successfully
   ADXL355 configured. Starting data acquisition...

   X: 0.012 g, Y: -0.003 g, Z: 0.987 g | Temp: 24.123 C
   X: 0.015 g, Y: -0.001 g, Z: 0.989 g | Temp: 24.156 C
   X: 0.013 g, Y: -0.002 g, Z: 0.988 g | Temp: 24.134 C

The Z-axis should read approximately 1 g when the sensor is lying flat (due to gravity).

SPI Controller Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Hardware to CAPI mapping:**

.. code-block:: c

   // Hardware: SPI0 peripheral on MAX32655
   struct capi_spi_controller_handle *spi_controller = NULL;

   // SPI controller configuration
   struct capi_spi_config adxl355_spi_config = {
       .identifier = 0,              // SPI0 peripheral
       .clk_freq_hz = 1000000,       // 1 MHz clock
       .ops = &maxim_capi_spi_ops    // Platform-specific operations
   };

   // Initialize SPI controller
   capi_spi_init(&spi_controller, &adxl355_spi_config);

SPI Device Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

**SPI device parameters for ADXL355:**

.. code-block:: c

   // SPI device configuration for ADXL355
   struct capi_spi_device adxl355_spi_dev = {
       .controller = spi_controller,  // Reference to controller
       .native_cs = 0,                // Use CS0 (P0.7)
       .mode = CAPI_SPI_MODE_0,       // CPOL=0, CPHA=0
       .max_speed_hz = 1000000,       // 1 MHz max speed
   };

The SPI device structure links to the controller and specifies device-specific parameters like chip select pin and SPI mode.

ADXL355 Driver Integration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ADXL355 initialization with CAPI SPI:**

.. code-block:: c

   // ADXL355 initialization parameters
   struct adxl355_init_param adxl355_param = {
       .spi_controller = spi_controller,
       .comm_param = adxl355_spi_config,
       .dev_type = ID_ADXL355_CAPI,
       .spi_dev = &adxl355_spi_dev,
   };

   struct adxl355_dev *adxl355;

   // Initialize the driver
   adxl355_init(&adxl355, adxl355_param);

   // Configure the sensor
   adxl355_soft_reset(adxl355);
   adxl355_set_odr_lpf(adxl355, ADXL355_ODR_62_5HZ);
   adxl355_set_op_mode(adxl355, ADXL355_MEAS_TEMP_ON_DRDY_OFF);

The ADXL355 driver uses the CAPI SPI interface internally, abstracting all register-level communication. The SPI controller has to be configured before this step.

Data Representation
~~~~~~~~~~~~~~~~~~~

The ADXL355 driver returns data using a fractional representation structure:

.. code-block:: c

   struct adxl355_frac_repr {
       int64_t integer;      // Integer part
       int32_t fractional;   // Fractional part (scaled)
   };

   // Example: Reading acceleration
   struct adxl355_frac_repr x_accel, y_accel, z_accel;
   adxl355_get_xyz(adxl355, &x_accel, &y_accel, &z_accel);

   // Print with 3 decimal places
   printf("X: %lld.%03d m/s^2\n",
          (long long)x_accel.integer,
          abs(x_accel.fractional));

This avoids floating-point arithmetic, which is expensive on embedded systems without an FPU.

Build and Run
-------------

Configuration
~~~~~~~~~~~~~

The project defconfig is defined in ``project_ex2.conf``:

.. code-block:: kconfig

   CONFIG_EXERCISE_2=y

   CONFIG_CAPI=y
   CONFIG_UART=y
   CONFIG_SPI=y
   CONFIG_IRQ=y
   CONFIG_GPIO=y

This enables:

- CAPI framework
- UART and SPI peripheral drivers
- GPIO and interrupt support

Note: The project won't compile out of the box. As part of the exercise, the user is expected to use menuconfig (or modify the defconfig and running a fresh build) to enable the ADXL355 driver.

Enabling the ADXL355 Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before building the project, you need to enable the ADXL355 driver using menuconfig:

**Step 1: Generate initial configuration**

First, configure the build and generate the initial ``.config`` file:

**Linux/macOS:**

.. code-block:: bash

   cmake --preset max32655_fthr -B ftc_workshop_build \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex2.conf --fresh

**Windows (PowerShell):**

.. code-block:: powershell

   cmake --preset max32655_fthr -B ftc_workshop_build `
     -DPROJECT_DEFCONFIG="ftc_workshop_capi/project_ex2.conf" --fresh

**Windows (Command Prompt):**

.. code-block:: batch

   cmake --preset max32655_fthr -B ftc_workshop_build ^
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex2.conf --fresh

**Step 2: Launch menuconfig**

Run menuconfig from the binary directory:

.. code-block:: bash

   cmake --build ftc_workshop_build --target menuconfig

This will launch an interactive text-based configuration interface.

**Step 3: Enable the ADXL355 driver**

In the menuconfig interface:

1. Navigate using arrow keys to **"Device Drivers"**
2. Press **Enter** to expand the menu
3. Navigate to **"Accelerometer Drivers"**
4. Press **Space** or **Y** to enable it (it should show ``[*]``)
5. Press **Enter** to expand the submenu
6. Navigate to **"Enable the ADXL355 driver"**
7. Press **Space** or **Y** to enable it (it should show ``[*]``)
8. Press **q** to exit, and press **y** to save the configuration

The menuconfig path is:

.. code-block:: text

   Device Drivers  --->
       [*] Accelerometer Drivers  --->
           [*] Enable the ADXL355 driver

**Step 4: Verify configuration**

You can verify that the driver is enabled by checking the ``.config`` file:

.. code-block:: bash

   grep ADXL355 ftc_workshop_build/build/.config

You should see:

.. code-block:: text

   CONFIG_ACCEL_ADXL355=y

Alternatively, you can manually add these lines to the ``project_ex2.conf`` file and regenerate the build:

.. code-block:: kconfig

   CONFIG_ACCEL=y
   CONFIG_ACCEL_ADXL355=y

Then run the CMake configuration command again with ``--fresh`` to regenerate the configuration.

After the driver is enabled, you can continue compiling the project:

.. code-block:: bash

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

Build Commands
~~~~~~~~~~~~~~

For complete build instructions including Windows-specific syntax, refer to the main :doc:`ftc_workshop_capi` documentation.

**Quick Reference (Linux/macOS):**

.. code-block:: bash

   # Configure CMake build
   cmake --preset max32655_fthr -B ftc_workshop_build \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex2.conf --fresh

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

**Quick Reference (Windows PowerShell):**

.. code-block:: powershell

   # Configure CMake build (note: quotes required for paths with /)
   cmake --preset max32655_fthr -B ftc_workshop_build `
     -DPROJECT_DEFCONFIG="ftc_workshop_capi/project_ex2.conf" --fresh

   # Build the project
   cmake --build ftc_workshop_build --target ftc_workshop

   # Flash to the board
   cmake --build ftc_workshop_build --target flash

Output binary location: ``ftc_workshop_build/build/ftc_workshop.elf`` (Linux/macOS) or ``ftc_workshop_build\build\ftc_workshop.elf`` (Windows)

Running the Example
~~~~~~~~~~~~~~~~~~~

1. **Connect hardware**:

   - Connect ADXL355 to MAX32655 FTHR by inserting it in the P6 connector of the FTHR-PMD-INTZ.
   - Ensure power and ground are connected
   - Connect USB cable for programming and serial console

2. **Flash the firmware** using the build commands above

3. **Open serial terminal** at 115200 baud:

   .. code-block:: bash

      # Recommended: picocom with automatic CR after LF
      picocom -b 115200 --imap lfcrlf /dev/ttyACM0

      # Or use screen with automatic CR after LF
      screen /dev/ttyACM0 115200,onlcr

      # Or use minicom (configure CR after LF interactively)
      minicom -D /dev/ttyACM0 -b 115200
      # Press Ctrl-A Z, then O → "Screen and keyboard" → enable "Add carriage return"

4. **Observe output**: You should see acceleration and temperature readings updating twice per second

Testing the Accelerometer
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **Test gravity response**:

   - Lay the board flat: Z-axis should read ~1 g, X and Y near 0 g
   - Tilt 90° on edge: One of X or Y should read ~1 g
   - Flip upside down: Z-axis should read ~-1 g

2. **Test temperature sensor**:

   - Temperature should be near ambient (20-30°C typically)

Next Steps
----------

Once you understand Exercise 2, proceed to :doc:`ftc_workshop_capi_exercise_3` to learn about:

- I2C communication with MAX20303 battery fuel gauge
- Displaying data on SSD1306 OLED screen
- GPIO bit-banging for I2C
- Multiple sensor integration

Additional Resources
--------------------

- :dokuwiki:`CAPI SPI Documentation </resources/no-os/capi/spi>`
- `ADXL355 Datasheet <https://www.analog.com/ADXL355>`_
- `SPI Protocol Tutorial <https://en.wikipedia.org/wiki/Serial_Peripheral_Interface>`_
