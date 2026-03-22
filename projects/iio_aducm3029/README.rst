IIO_ADUCM3029 no-OS Example Project
===================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADICUP3029`

Overview
--------

The IIO_ADUCM3029 no-OS project initializes and manages several hardware
components, including the Analog-to-Digital Converter (ADC), Pulse Width
Modulation (PWM) modules, and Universal Asynchronous
Receiver-Transmitter (UART) communication. The program configures these
components and continuously reads data from the ADC channels, which it
then transmits over UART. The ADC channels are enabled using a channel
mask, and the program reads a specified number of samples from these
channels. The data is formatted and sent through UART for further
processing or display.

The code also includes conditional compilation for Industrial I/O (IIO)
support, which allows the program to interface with IIO clients such as
the IIO Oscilloscope. When IIO support is enabled, the program
initializes an IIO application and configures the ADC and PWM devices
for data acquisition and control. The IIO application facilitates
communication with external clients, enabling remote control and data
visualization. The main function handles the initialization of the
platform and peripherals, and it enters an infinite loop to continuously
read and transmit ADC data.

Applications
------------

- Internet of Things (IoT)
- Electronic shelf label (ESL) and signage
- Smart infrastructure
- Smart lock
- Asset tracking
- Smart machine, smart metering, smart building, smart city, 
  and smart agriculture
- Wearables
- Fitness and clinical
- Machine learning and neural network

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADICUP3029 board can be powered through either the DC barrel
jack (+7V to +12V DC input) or the USB connector. The USB connection
provides power when connected to a PC and is also used for flash
programming and as a debug interface.

Connections
~~~~~~~~~~~

+-------------------+-------------------------+-------------------------------------------+
| Connector         | Ref / Pin No.           | Description                               |
+===================+=========================+===========================================+
| USB Port          | P10                     | Power, SWD programming/debug,             |
|                   |                         | virtual serial (UART via S2 switch)       |
+-------------------+-------------------------+-------------------------------------------+
| DC Power Jack     | P2                      | +7 V to +12 V DC input                    |
+-------------------+-------------------------+-------------------------------------------+
| Battery Header    | BT1                     | Alternative 5 V / 3.3 V power;            |
|                   |                         | does not power the debugger               |
+-------------------+-------------------------+-------------------------------------------+
| SPI PMOD          | 1: CS (P1_09)           | 12‑pin SPI PMOD; uses SPI1 bus            |
|                   | 2: MOSI (P1_07)         |                                           |
|                   | 3: MISO (P1_08)         |                                           |
|                   | 4: SCLK (P1_06)         |                                           |
|                   | 5: GND                  |                                           |
|                   | 6: 3.3 V                |                                           |
|                   | 7: IO16 (P1_00)         |                                           |
|                   | 8: RESET                |                                           |
|                   | 9: RDY (P0_14)          |                                           |
|                   | 10: IO12 (P0_12)        |                                           |
|                   | 11: GND                 |                                           |
|                   | 12: 3.3 V               |                                           |
+-------------------+-------------------------+-------------------------------------------+
| I2C PMOD          | 1–2: SCL (P0_04)        | 8‑pin I2C PMOD; pins duplicated           |
|                   | 3–4: SDA (P0_05)        | per PMOD standard                         |
|                   | 5–6: GND                |                                           |
|                   | 7–8: 3.3 V              |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Grove I2C         | 1: GND                  | 4‑pin Grove connector                     |
|                   | 2: 3.3 V                |                                           |
|                   | 3: SDA (P0_05)          |                                           |
|                   | 4: SCL (P0_04)          |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Arduino DIO High  | 1: SCL                  | SPI0 + I2C0 signals                       |
|                   | 2: SDA                  |                                           |
|                   | 3: AREF                 |                                           |
|                   | 4: AGND                 |                                           |
|                   | 5: SCLK                 |                                           |
|                   | 6: MISO                 |                                           |
|                   | 7: MOSI                 |                                           |
|                   | 8: CS                   |                                           |
|                   | 9: RDY                  |                                           |
|                   | 10: IO28                |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Arduino DIO Low   | 1: IO08                 | UART on pins 7–8; switchable              |
|                   | 2: IO27                 | via S2                                    |
|                   | 3: IO33                 |                                           |
|                   | 4: IO09                 |                                           |
|                   | 5: IO13                 |                                           |
|                   | 6: IO15                 |                                           |
|                   | 7: TX (P0_10)           |                                           |
|                   | 8: RX (P0_11)           |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Arduino Analog    | 1: AIN0 (P2_03)         | ADC inputs on ADuCM3029                   |
|                   | 2: AIN1 (P2_04)         |                                           |
|                   | 3: AIN2 (P2_05)         |                                           |
|                   | 4: AIN3 (P2_06)         |                                           |
|                   | 5: AIN4 (P2_07)         |                                           |
|                   | 6: AIN5 (P2_08)         |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Arduino Power     | 1: NC                   | Standard Uno R3 power header              |
|                   | 2: IOREF (3.3 V)        |                                           |
|                   | 3: RESET                |                                           |
|                   | 4: 3.3 V                |                                           |
|                   | 5: 5 V                  |                                           |
|                   | 6–7: GND                |                                           |
|                   | 8: Vin                  |                                           |
+-------------------+-------------------------+-------------------------------------------+
| Arduino ICSP      | 1: MISO                 | SPI0 signals; shared with DIO High        |
|                   | 2: 3.3 V                |                                           |
|                   | 3: SCLK                 |                                           |
|                   | 4: MOSI                 |                                           |
|                   | 5: RESET                |                                           |
|                   | 6: GND                  |                                           |
+-------------------+-------------------------+-------------------------------------------+
| BLE Module        | U8, P15                 | On‑board; P15 shunt must be fitted        |
| (EM9304)          |                         | to power BLE                              |
+-------------------+-------------------------+-------------------------------------------+
| WiFi Module       | P1                      | UART0; S2 switch must be in               |
| (ESP8266)         |                         | “WiFi” position                           |
+-------------------+-------------------------+-------------------------------------------+
| JTAG / SWD        | ARM Cortex 10‑pin       | On‑board MK20DX128 acts as debug          |
|                   | standard                | adapter; external connector also          |
|                   | (SWDIO, SWDCLK,         | available                                 |
|                   | GND, VCC, RESET)        |                                           |
+-------------------+-------------------------+-------------------------------------------+
| RESET Button      | S3 (3029_RESET)         | Hardware reset to ADuCM3029; hold         |
|                   |                         | during power‑cycle to enter debugger      |
|                   |                         | maintenance mode                          |
+-------------------+-------------------------+-------------------------------------------+
| BOOT Button       | S4 (3029_BOOT)          | Hold with RESET to enter UART             |
|                   |                         | download mode via P0_10 / P0_11           |
+-------------------+-------------------------+-------------------------------------------+
| WiFi RESET Button | S5 (WIFI_RESET)         | Hardware reset to the ESP8266 WiFi        |
|                   |                         | module                                    |
+-------------------+-------------------------+-------------------------------------------+

No-OS Build Setup
-----------------

For build setup instructions, see `<https://wiki.analog.com/resources/no-os/build>`.

No-OS Supported Examples
------------------------

The initialization data used in the examples is in the 
:git-no-OS:`Project Source Path </projects/iio_aducm3029/src>`.

The macros used in Common Data are defined in platform-specific files
in the :git-no-OS:`Project Configuration </projects/iio_aducm3029/pinmux_config.c>`.

IIO Example
~~~~~~~~~~~

The example initializes the platform, ADC, three PWM channels, and UART
to sample analog data, control PWM channels, and transmit collected data
via UART.

The IIO demo is a standard example, provided in most no-OS projects,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO Oscilloscope application, users
can access device settings such as the ADC and PWM functionalities.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`.

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
:git-no-OS:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`.

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIOD = y

No-OS Supported Platforms
-------------------------

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADICUP3029

Connections
^^^^^^^^^^^

1. Power Options

+-----------------------+-----------------------+-----------------------+
| USB Power             | Micro‑USB (P10)       | Powers and programs   |
|                       |                       | the board from a      |
|                       |                       | PC/laptop.            |
+-----------------------+-----------------------+-----------------------+
| DC Power              | Barrel Jack (P2)      | Requires 7V to 12V    |
|                       |                       | external supply.      |
+-----------------------+-----------------------+-----------------------+
| Battery Power         | BT1 (two AAA          | Set power switch to   |
|                       | batteries)            | BATT for battery      |
|                       |                       | operation.            |
+-----------------------+-----------------------+-----------------------+

2. Debugger and Node Board Connections

+-----------------------+-----------------------+-----------------------+
| Debugger ↔ Node Board | 10‑pin ARM JTAG/ SWD  | Required if boards    |
|                       | ribbon cable          | are separated.        |
+-----------------------+-----------------------+-----------------------+
| Board Separation      | Cut along perforation | Allows                |
|                       |                       | remote/standalone     |
|                       |                       | Node board use.       |
+-----------------------+-----------------------+-----------------------+

3. Programming Interfaces

+-----------------------+-----------------------+-----------------------+
| JTAG                  | P11                   | Programs the Debugger |
|                       |                       | chip.                 |
+-----------------------+-----------------------+-----------------------+
| SWD                   | P12, P14              | Programs the          |
|                       |                       | ADuCM3029 MCU.        |
+-----------------------+-----------------------+-----------------------+
| USB Programming       | P10 (via Debugger     | Standard programming  |
|                       | board)                | connection to PC.     |
+-----------------------+-----------------------+-----------------------+
| Standalone Node Power | Battery (BT1)         | Required when         |
|                       |                       | programming Node      |
|                       |                       | board alone.          |
+-----------------------+-----------------------+-----------------------+

4. UART Switch (S2) Positions

====== ====================== =============================
Left   USB (P10)              PC terminal connection.
Middle Arduino Connector (P7) Arduino shield communication
Right  WiFi Module (P1)       WiFi communication.
====== ====================== =============================

5. Buttons

+-----------------------------------+-----------------------------------+
| 3029_RESET                        | Hardware reset for ADuCM3029 MCU. |
+-----------------------------------+-----------------------------------+
| 3029_BOOT                         | Hold during reset to enter UART   |
|                                   | download mode (S2 must be set to  |
|                                   | USB).                             |
+-----------------------------------+-----------------------------------+
| WIFI_RESET                        | Hardware reset for WiFi (ESP8266) |
|                                   | module.                           |
+-----------------------------------+-----------------------------------+

6. Wireless Module Setup

+-----------------------+-----------------------+-----------------------+
| BLE Module            | Shunt on P15          | Install shunt to      |
|                       |                       | enable BLE; remove to |
|                       |                       | disable/save power.   |
+-----------------------+-----------------------+-----------------------+
| WiFi Module           | WiFi connector        | Communicates over     |
|                       |                       | UART with Node board. |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/iio_aducm3029

   export PLATFORM=aducm3029
   # to build the project
   make
   # to flash the code
   make run
   # to debug the code
   make debug
