MAX14906 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
----------------------------

- :adi:`MAX14906EVKIT`

Overview
--------

The MAX14906EVKIT Evaluation Kit is designed to assess the MAX14906,
a four-channel industrial digital-output, digital-input device. This kit
offers per-channel configuration for digital output and input modes,
along with diagnostic monitoring that identifies open-wire conditions,
output channel states, undervoltage, overtemperature, and other faults.
The EV kit requires a 24V DC power supply providing up to 6.4A and
connects to a PC via USB using an on-board FT2232 controller, or to
external devices through a 20-pin header. It comes with the MAX14906ATM+
part installed, supports Windows OS, and features SafeDemag™ for safe
shutdown in digital output modes. The design achieves up to 3.75kVRMS
galvanic isolation, is fully assembled, tested, and conforms to RoHS
standards.

Applications
------------

- Industrial Digital Output Modules
- Configurable Digital Input/Output
- Motor Control
- PLC Systems
- Distributed Control Systems (DCS)

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX14906EVKIT requires a 24V DC power supply capable of delivering
up to 6.4A. This evaluation kit includes the MAX14906ATM+ part and is
designed to provide configurability for digital output and input modes
across its four channels.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| USB Port                          | Provides interface for connecting |
|                                   | the evaluation kit to a PC via    |
|                                   | the on-board FT2232 controller.   |
+-----------------------------------+-----------------------------------+
| VDD1 to VDD4                      | Field supply connectors powering  |
|                                   | each DOI channel.                 |
+-----------------------------------+-----------------------------------+
| J13 Header                        | Available for accessing all       |
|                                   | digital signals including SPI, D1 |
|                                   | to D4 signals, SYNCH, READY, and  |
|                                   | FAULT.                            |
+-----------------------------------+-----------------------------------+
| DOI Test Points                   | Used to connect oscilloscope      |
|                                   | probes to verify digital output   |
|                                   | signals in real-time operation.   |
+-----------------------------------+-----------------------------------+
| GND Test Point                    | Used to connect the negative      |
|                                   | terminal of the 24V DC power      |
|                                   | supply.                           |
+-----------------------------------+-----------------------------------+
| CRCEN Jumper                      | Configures CRC functionality on   |
|                                   | the SPI interface when connected  |
|                                   | in specific positions.            |
+-----------------------------------+-----------------------------------+

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Pin Name              | Functionality         | Description           |
+-----------------------+-----------------------+-----------------------+
| SPI                   | Serial Peripheral     | Used for serial       |
|                       | Interface             | communication;        |
|                       |                       | accessible through    |
|                       |                       | the J13 header.       |
+-----------------------+-----------------------+-----------------------+
| D1 to D4              | Digital Signals       | These are digital I/O |
|                       |                       | signals available     |
|                       |                       | through the J13       |
|                       |                       | header.               |
+-----------------------+-----------------------+-----------------------+
| SYNCH                 | Synchronization       | Used to synchronize   |
|                       | Signal                | multiple devices;     |
|                       |                       | available through the |
|                       |                       | J13 header.           |
+-----------------------+-----------------------+-----------------------+
| READY                 | Status Signal         | Indicates the device  |
|                       |                       | is ready; available   |
|                       |                       | through the J13       |
|                       |                       | header.               |
+-----------------------+-----------------------+-----------------------+
| FAULT                 | Error Indicator       | Indicates fault       |
|                       |                       | conditions; available |
|                       |                       | through the J13       |
|                       |                       | header.               |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/src/platform>`__

Basic example
~~~~~~~~~~~~~

The basic example code for the MAX14906 evaluation kit focuses on
initializing and controlling the MAX14906 via SPI and GPIO interfaces.
It sets up UART communication for debugging and configures SPI settings
tailored to either the Maxim or STM32 platforms. The
``basic_example_main()`` function initializes hardware components
relevant to each platform, such as the NVIC for Maxim and system clock
configuration for STM32. It configures the MAX14906 device, registers a
GPIO interrupt handler to handle fault conditions, and establishes
current limits for the output channels. Structured callback functions
manage GPIO interrupts, facilitating the monitoring of diagnostics like
open-wire detection. Faults trigger interrupts that call a callback
function, which reads the interrupt register to detect and report
errors. The example illustrates essential device control and monitoring
capabilities, including initialization and fault management protocols.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max14906/Makefile>`__.

.. code-block:: bash 

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX14906, located in the ``iio_example.c``
file, demonstrates how to use Industrial I/O (IIO) to interface with the
MAX14906 device. The main function, ``iio_example_main``, initializes
structures like ``max14906_iio_desc`` and
``max14906_iio_desc_init_param`` needed to configure the I/O channels.
It involves setting up an IIO application descriptor and invoking
``iio_app_init`` to start the application, enabling users to interact
with the device using IIO commands such as ``iio_info``. The program
includes error handling for initialization and resource cleanup,
offering a guide for developers to integrate the MAX14906 into their
systems via the IIO framework on supported platforms like Maxim and
STM32. Platform-specific configurations are managed through preprocessor
directives to control the active example, whether IIO or basic, during
compilation.

If you are not familiar with the ADI IIO framework, please take a look
at: `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with the ADI IIO Oscilloscope app, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`AD-APARD32690-SL`
- :adi:`MAX32666FTHR`

Connections
^^^^^^^^^^^

Disconnect internal controller first when using other MCUs. Open all
switches on SW1 of the MAX14906EVKIT to disconnect the onboard FT2232
controller. This allows use of an external microcontroller for direct
control via J13.

Then, use jumper wires for point-to-point wiring between the
headers—there is no plug-and-play stacking or direct mechanical
connection between these boards.

**MAX14906 to other Maxim Target Board**

+-------------------+-------------+---------------------+----------------+-------------------+
| Function          | MAX14906    | Target Board        | Target Board   | Notes             |
|                   | Interface   | Pin/Port            | Pin/Port       |                   |
|                   |             | **MAX32690**        | **MAX32665**   |                   |          
|                   |             | (AD-APARD32690-SL)  | (MAX32666FTHR) |                   |
+-------------------+-------------+---------------------+----------------+-------------------+
| SPI Device ID     | SPI         | 4                   | 1              | SPI peripheral    |
|                   |             |                     |                | index             |
+-------------------+-------------+---------------------+----------------+-------------------+
| SPI Chip Select   | SPI         | 0                   | 0              | CS line           |
|                   |             |                     |                |                   |
+-------------------+-------------+---------------------+----------------+-------------------+
| SPI Baudrate      | SPI         | 100000              | 100000         | Low-speed SPI     |
+-------------------+-------------+---------------------+----------------+-------------------+
| GPIO Fault Port   | GPIO        | 2                   | 0              | Fault detection   |
|                   |             |                     |                | port              |
+-------------------+-------------+---------------------+----------------+-------------------+
| GPIO Fault Pin    | GPIO        | 21                  | 5              | Fault detection   |
|                   |             |                     |                | pin               |
+-------------------+-------------+---------------------+----------------+-------------------+
| GPIO IRQ ID       | GPIO        | 2                   | 0              | GPIO interrupt ID |
+-------------------+-------------+---------------------+----------------+-------------------+

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32665
   # to flash the code
   make run

STM32 Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX14906EVKIT
- NUCLEO-F401RE STM32 Nucleo-64 development board with STM32F401RE MCU

Connections
^^^^^^^^^^^

The MAX14906EVKIT supports SPI for communication. To use SPI between the
NUCLEO-F401RE board and the MAX14906EVKIT, connect these pins:

+-------------+----------------------+-------------+--------------+
| MAX14906    | Function             | STM32F401RE | Signal       |
| Signal      |                      | Pin         |              |
+-------------+----------------------+-------------+--------------+
| FAULT/INT   | Interrupt            | PA0-WKUP    | EXTI0        |
|             |                      |             | (GPIO_EXTI0) |
+-------------+----------------------+-------------+--------------+
| UART TX/RX  | Serial Communication | PA2 / PA3   | USART2       |
+-------------+----------------------+-------------+--------------+
| SPI CS      | Chip Select          | PA4         | GPIO Output  |
+-------------+----------------------+-------------+--------------+
| SPI MISO    | Data from MAX14906   | PA6         | SPI1_MISO    |
+-------------+----------------------+-------------+--------------+
| SPI MOSI    | Data to MAX14906     | PA7         | SPI1_MOSI    |
+-------------+----------------------+-------------+--------------+
| SPI SCK     | SPI Clock            | PB3         | SPI1_SCK     |
+-------------+----------------------+-------------+--------------+

**GPIO Settings**:

- CS pin (PA4) is set high by default (``GPIO_PIN_SET``).
- FAULT/INT pin (PA0) is configured for falling edge interrupt.

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=stm32
   # to flash the code
   make run
   # to debug the code
   make debug
