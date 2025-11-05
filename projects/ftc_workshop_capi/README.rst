FTC Workshop CAPI Project
=========================

Overview
--------

The FTC Workshop CAPI project is a series of progressive embedded systems examples designed to demonstrate the use of Analog Devices' Common API (CAPI) framework on the MAX32655 FTHR platform. This project provides hands-on exercises that build upon each other, introducing concepts from basic GPIO operations to complex multi-sensor systems with wireless connectivity.

CAPI is a hardware abstraction layer that provides a unified interface for peripheral access across different microcontroller platforms.

Supported Hardware
------------------

- **Development Board**: MAX32655 FTHR (:adi:`MAX32655FTHR`)
- **Sensors and Peripherals**:

  - FTHR to PMOD adapter (:adi:`FTHR-PMD-INTZ`)
  - ADXL355 3-axis MEMS accelerometer (:adi:`EVAL-ADXL355-PMDZ`)
  - MAX20303 Battery management IC with fuel gauge (I2C interface)
  - SSD1306 128x64 OLED display (I2C interface via GPIO bitbang)

Project Structure
-----------------

The project consists of four progressive exercises:

.. toctree::
   :maxdepth: 2

   ftc_workshop_capi_exercise_1
   ftc_workshop_capi_exercise_2
   ftc_workshop_capi_exercise_3
   ftc_workshop_capi_exercise_4

Build System
------------

This project uses CMake with Kconfig for configuration management.

Prerequisites
~~~~~~~~~~~~~

- **Maxim SDK (MSDK)**: Install from the `Analog Devices website <https://www.analog.com/en/design-center/evaluation-hardware-and-software/software/software-download.html?swpart=SFW0010820A>`_ or download specific releases from the `MSDK GitHub releases page <https://github.com/analogdevicesinc/msdk/releases>`_
- **CMake**: Version 3.20 or higher
- **ARM GCC Toolchain**: Included with Maxim SDK
- **Python 3**: For Kconfig processing

Building an Exercise
~~~~~~~~~~~~~~~~~~~~

Each exercise has its own configuration file. To build a specific exercise:

.. code-block:: bash

   # Configure the build for exercise 1
   cmake --preset max32655_fthr --fresh -B build_ex1 \
     -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex1.conf

   # Build the project
   cmake --build build_ex1 --target ftc_workshop

   # The output binary is at: build_ex1/build/ftc_workshop.elf

Flashing the Firmware
~~~~~~~~~~~~~~~~~~~~~~

The MAX32655 FTHR board includes an integrated CMSIS-DAP debugger:

.. code-block:: bash

   # Using OpenOCD (automatically configured)
   cmake --build build_ex1 --target flash

   # Or manually with OpenOCD
   openocd -f interface/cmsis-dap.cfg \
           -f target/max32655.cfg \
           -c "program build_ex1/build/ftc_workshop.elf verify reset exit"

Serial Console
~~~~~~~~~~~~~~

Connect a serial terminal at **115200 baud, 8N1** to view debug output:

.. code-block:: bash

   # Linux/macOS
   screen /dev/ttyACM0 115200

   # Or use minicom
   minicom -D /dev/ttyACM0 -b 115200

Quick Start
-----------

1. **Install Prerequisites**: Ensure Maxim SDK is installed and the ``MAXIM_LIBRARIES`` environment variable points to the SDK's Libraries directory.

2. **Clone the Repository**:

   .. code-block:: bash

      git clone https://github.com/adi-innersource/no-OS.git --branch dev/capi
      cd no-OS

3. **Start with Exercise 1**:

   .. code-block:: bash

      # Build exercise 1
      cmake --preset max32655_fthr -B build_ex1 \
        -DPROJECT_DEFCONFIG=ftc_workshop_capi/project_ex1.conf
      cmake --build build_ex1 --target ftc_workshop

      # Flash to board
      cmake --build build_ex1 --target flash

4. **Open Serial Terminal** and press the buttons to see GPIO input in action.

5. **Progress through exercises** 2, 3, and 4, building on the concepts learned.

Architecture Overview
---------------------

CAPI Abstraction Layers
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   ┌─────────────────────────────────────────┐
   │     Application Code (Exercises)        │
   ├─────────────────────────────────────────┤
   │    Driver Layer (ADXL355, MAX20303)     │
   ├─────────────────────────────────────────┤
   │      CAPI Interface (capi_spi.c)        │
   ├─────────────────────────────────────────┤
   │   Platform Implementation (maxim_*.c)   │
   ├─────────────────────────────────────────┤
   │    Hardware Abstraction Layer (HAL)     │
   │        (Maxim PeriphDrivers)            │
   ├─────────────────────────────────────────┤
   │        Hardware Registers               │
   └─────────────────────────────────────────┘

The CAPI framework sits between device drivers and platform-specific implementations, allowing drivers to be written once and used across multiple microcontroller families.

CAPI Object Model
~~~~~~~~~~~~~~~~~

CAPI uses a consistent object model across all peripherals. For protocols which allow multiple targets to be connected to the same controller (e.g SPI, I2C):

1. **Controller Handle**: Represents the peripheral controller (e.g., ``capi_spi_controller_handle``)
2. **Initialization Parameters**: Used during setup to configure the peripheral (e.g., ``capi_spi_config``).
3. **Device Configuration**: Specifies device-specific parameters (e.g., ``capi_spi_device``)
4. **Operations Structure**: Contains function pointers for specific implementations of the API. These need to be passed in by the user.

Example CAPI object hierarchy for SPI:

.. code-block:: text

   SPI Controller (capi_spi_controller_handle)
   ├── Operations (maxim_capi_spi_ops)
   │   ├── init()
   │   ├── transceive()
   │   └── remove()
   └── Device (capi_spi_device)
       ├── Controller reference
       ├── Chip select pin
       ├── SPI mode
       └── Max speed

For point to point protocols (e.g UART):

1. **Controller Handle**: Represents the peripheral controller (e.g., ``capi_uart_handle``)
2. **Initialization Parameters**: Used during setup to configure the peripheral (e.g., ``capi_uart_config``).
3. **Operations Structure**: Contains function pointers for specific implementations of the API. These need to be passed in by the user.

Example CAPI object hierarchy for UART:

.. code-block:: text

   UART Controller (capi_uart_handle)
   ├── Operations (maxim_capi_uart_ops)
   │   ├── init()
   │   ├── read()
   │   ├── write()
   │   └── remove()
   └── Config (capi_uart_config) # Only used during init 
       ├── Identifier (UART instance)
       ├── Baud rate
       └── Data format (8N1, etc.)

Hardware Connections Summary
-----------------------------

Common to All Exercises
~~~~~~~~~~~~~~~~~~~~~~~

The following pin connections are used across all exercises:

+------------------+------------------+-------------------------+
| Signal           | MAX32655 Pin     | Description             |
+==================+==================+=========================+
| UART0_TX         | P0.9             | Debug console output    |
+------------------+------------------+-------------------------+
| UART0_RX         | P0.8             | Debug console input     |
+------------------+------------------+-------------------------+
| Button 1         | P0.2 (pull-up)   | User button input       |
+------------------+------------------+-------------------------+
| Button 2         | P0.3 (pull-up)   | User button input       |
+------------------+------------------+-------------------------+

Exercise-Specific Connections
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Exercise 2, 3, 4:**

+------------------+------------------+-------------------------+
| Signal           | MAX32655 Pin     | Connected To            |
+==================+==================+=========================+
| SPI0_SCK         | P0.6             | ADXL355 SCK             |
+------------------+------------------+-------------------------+
| SPI0_MOSI        | P0.5             | ADXL355 SDI             |
+------------------+------------------+-------------------------+
| SPI0_MISO        | P0.4             | ADXL355 SDO             |
+------------------+------------------+-------------------------+
| SPI0_CS          | P0.7             | ADXL355 CS              |
+------------------+------------------+-------------------------+

**Exercise 3, 4:**

+------------------+------------------+-------------------------+
| Signal           | MAX32655 Pin     | Connected To            |
+==================+==================+=========================+
| I2C1_SDA         | P0.15            | MAX20303 SDA            |
+------------------+------------------+-------------------------+
| I2C1_SCL         | P0.14            | MAX20303 SCL            |
+------------------+------------------+-------------------------+
| GPIO_SDA         | P0.30            | SSD1306 SDA (bitbang)   |
+------------------+------------------+-------------------------+
| GPIO_SCL         | P0.31            | SSD1306 SCL (bitbang)   |
+------------------+------------------+-------------------------+

**Exercise 4 Additional:**

+------------------+------------------+-------------------------+
| Signal           | MAX32655 Pin     | Description             |
+==================+==================+=========================+
| BLE Antenna      | Internal         | Bluetooth radio         |
+------------------+------------------+-------------------------+

Additional Resources
--------------------

- :dokuwiki:`No-OS Build Setup </resources/no-os/build>`
- :git-no-OS:`Project Source Code </projects/ftc_workshop_capi>`
- `MAX32655 Datasheet <https://www.analog.com/MAX32655>`_
- `ADXL355 Datasheet <https://www.analog.com/ADXL355>`_
- `MAX20303 Datasheet <https://www.analog.com/MAX20303>`_

License
-------

This project is licensed under the BSD-3-Clause license. See the LICENSE file for details.

Copyright (c) 2025 Analog Devices, Inc.
