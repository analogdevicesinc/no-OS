AD9545 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD9545`

Overview
--------

The EVAL-AD9545 evaluation board is designed for testing the AD9545
clock generator IC, notable for its high-precision multi-output clock
generation. It includes two digital cleaning PLL cores to optimize clock
synchronization. The board offers up to five differential clock outputs,
configurable through a serial port, and optionally supports an external
EEPROM for settings retention and power management. Applications range
from telecommunications to industrial automation, requiring precise
timing solutions.

Applications
------------

- Global positioning system (GPS), PTP (IEEE 1588), and synchronous
  Ethernet (SyncE) jitter cleanup and synchronization
- Optical transport networks (OTN), synchronous digital hierarchy (SDH),
  and macro and small cell base stations
- Small base station clocking, including baseband and radio
- Stratum 2, Stratum 3e, and Stratum 3 holdover, jitter cleanup, and
  phase transient control
- JESD204B support for analog-to-digital converter (ADC) and
  digital-to-analog converter (DAC) clocking
- Cable infrastructures 
- Carrier Ethernet

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD9545 evaluation board requires a 6V power connection via a
wall adapter to activate its on-board LDO voltage regulators. These
regulators provide key voltage outputs of 1.8V and 3.3V, crucial for the
board’s operation. Critical components, such as the ADP2384 and ADP7104
regulators, manage these voltage levels to support the board’s
functionalities, including generating up to five differential clock
outputs and utilizing EEPROM for settings retention. Additionally, power
is supplied via USB for communication with a PC, enabling configuration
through the provided Windows-based software.

Connectors
~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector Type        | Description           | Functionality         |
+-----------------------+-----------------------+-----------------------+
| J300                  | SMB Connector         | Reference input for   |
|                       |                       | single-ended or       |
|                       |                       | differential signals  |
+-----------------------+-----------------------+-----------------------+
| J301                  | SMB Connector         | Reference input for   |
|                       |                       | single-ended or       |
|                       |                       | differential signals  |
+-----------------------+-----------------------+-----------------------+
| J400                  | SMA Connector         | System clock input    |
|                       |                       | option for external   |
|                       |                       | sources               |
+-----------------------+-----------------------+-----------------------+
| P605                  | Jumper Block          | GPIO configuration    |
|                       |                       | for clock and data    |
|                       |                       | lines                 |
+-----------------------+-----------------------+-----------------------+
| P504                  | Jumper Block          | Serial data line      |
|                       |                       | configuration         |
+-----------------------+-----------------------+-----------------------+
| P511                  | Jumper Block          | Serial data line      |
|                       |                       | configuration         |
+-----------------------+-----------------------+-----------------------+

Serial Port Configuration Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9545 serial port configuration is determined by the logic state of
multi-function pins M4, M5, and M6 upon exit from a reset state. M4
selects which protocol (SPI versus I²C), and M5 and M6 determine the I²C
address. Jumper Block P605 allows the user to enable pull-up/down
resistors to control the state of Pins M4, M5, and M6 on the AD9545.

+-------------+-------------+-------------+-------------+-------------+
| Serial      | Slave       | Jumper      | Jumper P504 | Jumper P511 |
| Protocol    | Address     | P605,       |             |             |
|             |             | Top/Mi      |             |             |
|             |             | ddle/Bottom |             |             |
|             |             | Row         |             |             |
+-------------+-------------+-------------+-------------+-------------+
| SPI         | N/A         | Jumpers     | SCLK        | SDIO        |
|             |             | removed     |             |             |
+-------------+-------------+-------------+-------------+-------------+
| SPI         | N/A         | 2-3, others | SCLK        | SDIO        |
|             |             | do not      |             |             |
|             |             | matter      |             |             |
+-------------+-------------+-------------+-------------+-------------+
| I²C         | 0x48        | 8-9 / 5-6 / | SCL         | SDA         |
|             |             | 1-2         |             |             |
+-------------+-------------+-------------+-------------+-------------+
| I²C         | 0x49        | 8-9 / 4-5 / | SCL         | SDA         |
|             |             | 1-2         |             |             |
+-------------+-------------+-------------+-------------+-------------+
| I²C         | 0x4A        | 7-8 / 5-6 / | SCL         | SDA         |
|             |             | 1-2         |             |             |
+-------------+-------------+-------------+-------------+-------------+
| I²C         | 0x4B        | 7-8 / 4-5 / | SCL         | SDA         |
|             |             | 1-2         |             |             |
+-------------+-------------+-------------+-------------+-------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the example is taken out from the
:git-no-OS:`Project Common Data Path </projects/ad9545/src/common>`.

The macros used in Common Data are defined in platform specific files
found in the :git-no-OS:`Project Platform Configuration Path </projects/ad9545/src/platform>`.

Basic example
~~~~~~~~~~~~~

The basic example code for the AD9545 device within the no-OS project
showcases the initialization and configuration of the device’s clock
systems. This includes the setup of reference clocks, PLLs, NCOs, and
output clocks. Configuration begins by specifying initialization
parameters for communication interfaces like UART, SPI, and I2C via
structures including ``ad9545_uart_ip``, ``ad9545_spi_ip``, and
``ad9545_i2c_ip``. The ``basic_example_main`` function oversees the
initialization and configuration process, setting clock rates, assigning
parent clocks, and recalibrating PLLs through functions such as
``ad9545_init``, ``ad9545_setup``, and ``ad9545_calib_aplls``. This
ensures accurate clock source management. Error handling is incorporated
to address possible initialization failures, with ongoing verification
through continuous operation checks.

In order to build the basic example, make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/ad9545/Makefile>`:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Linux Platform
~~~~~~~~~~~~~~

Connections
^^^^^^^^^^^

- Connect the 6V wall power supply to the main power connector labeled
  P700.
- Connect the USB cable to the EVAL-AD9545 evaluation board and the
  Linux platform. The red LED labeled ‘DS301’ should illuminate, and the
  LED labeled ‘USB_STA’ should blink.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   no-OS/projects/ad9545

   export PLATFORM=linux

   # to build the project
   make PLATFORM=linux
   # to flash the code
   make run
