EVAL-ADE9000 no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EV-ADE9000SHIELDZ`

Overview
--------

The EV-ADE9000SHIELDZ is an Arduino form factor evaluation board
designed for energy and power quality monitoring using the ADE9000 chip.
It supports various microcontroller platforms such as Arduino Zero and
ESP8266. The shield integrates current and
voltage sensing capabilities with direct connections for various
transformers and Rogowski coils. It includes onboard burden resistors
and connectors for precise measurement settings, enhancing prototyping
in energy management. The board provides detailed user guidance for
setup, calibration, and software integration, ensuring effective use of
its monitoring functions.

Applications
------------

- Energy and power monitoring
- Power quality monitoring
- Protective devices
- Machine health
- Smart power distribution units
- Polyphase energy meters

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EV-ADE9000SHIELDZ evaluation board requires both 3.3V and 5V power
supplies. These voltages can be provided via connected Arduino-based
MCUs or ESP8266 boards. Additionally, an optional external 5V supply can
be connected to J9 if the main board does not provide sufficient current
during debugging. This configuration supports efficient application
performance while maintaining compatibility with various microcontroller
platforms, facilitating energy and power quality measurement tasks.

Onboard Connectors
~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Purpose                           |
+-----------------------------------+-----------------------------------+
| J1 to J4                          | These connectors interface with   |
|                                   | the microcontroller unit (MCU) or |
|                                   | Arduino headers, enabling         |
|                                   | integration and communication     |
|                                   | with the board.                   |
+-----------------------------------+-----------------------------------+
| J9                                | Allows connection of an optional  |
|                                   | external 5V supply if the main    |
|                                   | board does not provide sufficient |
|                                   | current.                          |
+-----------------------------------+-----------------------------------+
| SL1 to SL4                        | Terminal blocks for connection to |
|                                   | current transformer (CT) leads,   |
|                                   | facilitating current measurement. |
+-----------------------------------+-----------------------------------+
| L1 to L4                          | Used for measuring multiphase     |
|                                   | voltages, ensuring accurate       |
|                                   | voltage detection across phases.  |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Source Data Path </projects/eval-ade9000/src>`.

Demo example
~~~~~~~~~~~~

The basic example code for the ADE9000 evaluation board utilizes SPI
communication facilitated by the ``max32690`` microcontroller platform
to interface with the ADE9000 device. It initializes UART for logging
and GPIO for LED control and configures the ADE9000 for energy
measurement. Core functions include reading and displaying voltage and
current RMS values and temperature data via UART. The program
continuously monitors these parameters with time delays, ensuring
ongoing feedback about energy usage. This setup requires
platform-specific UART and SPI configurations to integrate effectively
with the ADE9000’s measurement capabilities.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EV-ADE9000SHIELDZ
- AD-APARD32690-SL
- MAX32625PICO microcontroller (flashed with firmware image)

SPI Connections
^^^^^^^^^^^^^^^

================= ==== ================
EV-ADE9000SHIELDZ Pin  AD-APARD32690-SL
J10 Header             P5 Header
SCLK-ISO          SCLK 5
MISO-ISO          MISO 6
MOSI-ISO          MOSI 7
SS-ISO            CS   8
================= ==== ================

**NOTE**: The AD-APARD32690-SL microcontroller does not have integrated
connections for the ICSP pins of the ADE9000. Users need to manually
connect the microcontroller’s SPI pins to the J10 header on the
evaluation board to establish SPI communication. This setup is crucial
for proper interfacing and functionality of the ADE9000 chip with the
microcontroller for effective data acquisition and processing.

Power
^^^^^^

================= ================
EV-ADE9000SHIELDZ  AD-APARD32690-SL
J3 Header          P2 Header
3.3 V              3.3 V
5 V                5 V
================= ================

Build
^^^^^

.. code-block:: bash

   # Go into the ADE9000 project directory
   cd no-OS/projects/eval-ade9000

   # To build the ADE9000 project
   make PLATFORM=maxim TARGET=max32690

   # To upload the .elf, .hex, or .bin file into the AD-APARD32690-SL
   make PLATFORM=maxim TARGET=max32690 run
