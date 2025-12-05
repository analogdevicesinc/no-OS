DC2903A no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Boards
----------------------------

- :adi:`DC2903A`

Overview
--------

The DC2903A evaluation board is designed to assess the LTC2672, a
five-channel DAC with low dropout current source output. It features
five programmable DAC output channels capable of sourcing currents up to
3.125 mA per channel. These channels allow configuration through a
software interface, supporting operations such as toggling outlined
channels (e.g., OUT3) to switch between preset currents. Communication
is established via SPI, with parameters like device ID and baud rate set
in the software. The board also allows reference settings configuration
for VREF measurement, ensuring precise control. This setup facilitates
efficient interaction and monitoring in various applications.

Applications
------------

- Tunable lasers
- Semiconductor optical amplifier biasing
- Resistive heaters
- Current mode biasing

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The DC2903A evaluation board requires a positive supply voltage at the
VCC (E6) input and a ground connection at E7, supporting a voltage range
from 2.1V to 5.5V DC. Stability within these voltage limits is
necessary for optimal operation. The board is integrated with the
LTC2672 DAC, providing five channels of current output and interfaces
with the DC2026C controller board via a ribbon cable at the J1
connector. Ensure all channels are grounded and can handle the maximum
current output specified. The board also allows monitoring of output
voltages and internal die temperature through a multiplexer pin for
operational diagnostics.

Connections
~~~~~~~~~~~

+----------------------------------+----------------------------------+
| Connector                        | Function                         |
+----------------------------------+----------------------------------+
| J1                               | SPI/I2C interface pin header     |
|                                  | connector                        |
+----------------------------------+----------------------------------+
| JP1                              | REF select                       |
+----------------------------------+----------------------------------+
| JP2                              | FSADJ select                     |
+----------------------------------+----------------------------------+
| JP3 to JP7                       | Internal or external VDDx        |
|                                  | voltage select                   |
+----------------------------------+----------------------------------+
| E1                               | CLR                              |
+----------------------------------+----------------------------------+
| E2, E5, E12 to E14, E16, E18,    | GND connections                  |
| E22, E29                         |                                  |
+----------------------------------+----------------------------------+
| E3                               | Toggle enable (TGP)              |
+----------------------------------+----------------------------------+
| E4                               | LDAC                             |
+----------------------------------+----------------------------------+
| E6                               | VCC positive voltage supply      |
+----------------------------------+----------------------------------+
| E7                               | GND for power supply             |
+----------------------------------+----------------------------------+
| E8                               | V− negative voltage supply       |
+----------------------------------+----------------------------------+
| E9                               | Channel 1 output pin (OUT1)      |
+----------------------------------+----------------------------------+
| E10                              | External reference connection    |
|                                  | (REF)                            |
+----------------------------------+----------------------------------+
| E11                              | Channel 0 output pin (OUT0)      |
+----------------------------------+----------------------------------+
| E15                              | Channel 2 output pin (OUT2)      |
+----------------------------------+----------------------------------+
| E17                              | Channel 3 output pin (OUT3)      |
+----------------------------------+----------------------------------+
| E19                              | Channel 4 output pin (OUT4)      |
+----------------------------------+----------------------------------+
| E20                              | Multiplexer output (MUX)         |
+----------------------------------+----------------------------------+
| E23, E25 to E28                  | External supplies for VDDx pins  |
+----------------------------------+----------------------------------+

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/dc2903a/src>`__.

Basic example
~~~~~~~~~~~~~

The basic example code for the DC2903A evaluation board demonstrates the
configuration of the LTC2672 DAC’s output channels via SPI
communication. It involves setting up the Makefile to enable the basic
example (``BASIC_EXAMPLE = y``). The user must configure SPI
communication parameters and define settings for DAC channels to output
desired current levels up to 3.125 mA. The example also includes
monitoring the board’s outputs and internal diagnostics like the die
temperature via the multiplexer. Essential steps include powering the
board with a voltage between 2.1 V and 5.5 V, ensuring all connections
via specified J1 and E connectors, and verifying the reference settings
through JP connectors.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- DC2903A
- MAX32665

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| Description           | DC2903A Connection    | MAX32665 Connection   |
+-----------------------+-----------------------+-----------------------+
| Power Connection      | E6 (VCC)              | Connect to the power  |
| (VCC)                 |                       | source providing 2.1V |
|                       |                       | to 5.5V               |
+-----------------------+-----------------------+-----------------------+
| Ground Connection     | E7 (GND)              | Connect to Ground     |
+-----------------------+-----------------------+-----------------------+
| Negative Voltage      | E8 (V−)               | Connect to Ground or  |
| Supply                |                       | a negative supply if  |
|                       |                       | required              |
+-----------------------+-----------------------+-----------------------+
| SPI Chip Select       | J1 Pin 6 (CS)         | Chip select signal    |
|                       |                       | from MAX32665         |
+-----------------------+-----------------------+-----------------------+
| SPI Clock Signal      | J1 Pin 4 (SCL/SCK)    | Serial clock signal   |
|                       |                       | from MAX32665         |
+-----------------------+-----------------------+-----------------------+
| SPI Master-Out        | J1 Pin 7 (SDA/MOSI)   | Data from MAX32665 to |
| Slave-In              |                       | DC2903A               |
+-----------------------+-----------------------+-----------------------+
| SPI Master-In         | J1 Pin 5 (MISO)       | Data to MAX32665 from |
| Slave-Out             |                       | DC2903A               |
+-----------------------+-----------------------+-----------------------+
| EEPROM Data           | J1 Pin 9 (EEDA)       | Connect for data      |
| Connection            |                       | transfer with EEPROM  |
+-----------------------+-----------------------+-----------------------+
| EEPROM VCC Connection | J1 Pin 10 (EEVCC)     | Power supply for      |
|                       |                       | EEPROM                |
+-----------------------+-----------------------+-----------------------+
| EEPROM Ground         | J1 Pin 12 (EEGND)     | Ground for EEPROM     |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete the current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=MAX32665
   # to flash the code
   make run
