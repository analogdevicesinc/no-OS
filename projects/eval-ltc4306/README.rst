EVAL-LTC4306 no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-LTC4306`

Overview
--------

The EVAL-LTC4306 features the LTC4306, a 4-channel, 2-wire I2C
bus and SMBus-compatible multiplexer, which has bus buffers that provide
capacitive isolation between the upstream bus and downstream buses. 
This evaluation board provides 6-pin PMOD connectors for upstream and
downstream connection for compatibility with PMOD form factors such as
EVAL-ADICUP3029, an Arduino-based wireless development platform 
for Internet of Things applications based on an ultralow power 
ARM Cortex-M3 processor.

The EVAL-LTC4306 comes with PMOD connectors and 10kΩ pull up resistors 
on both the upstream and downstream side, and 0.1µF bypass capacitor 
on the upstream side. The user also has the option to add a 6-pin, 
2.54 mm pitch pin header for connection to the GPIO pins 
for added functionality. The user can also add test points 
on different places on the board for fault monitoring and 
place additional bypass capacitors (0805 size footprint) on the
upstream and the downstream side for defense against any unwanted noise
in their system and for adequate bypassing on power supply lines.

Applications
------------

- Nested Addressing
- 5V/3.3V Level Translator
- Capacitance Buffer/Bus Extender

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-LTC4306 requires a supply voltage (Vcc) between 2.2V and 5.5V.
To ensure proper functionality, pull-up resistors must be chosen to
provide a minimum of 100µA pull-up current while adhering to a maximum
capacitance load of 2nF. The device can handle a maximum pull-up current
of 4mA, ensuring optimal performance for I2C and SMBus configurations.
Additionally, bypass capacitors of at least 0.01µF should be placed
between Vcc and GND to stabilize the power supply. The LTC4306 includes
rise time accelerators which source current into the 2-wire bus pins,
but care must be taken to avoid sourcing current from Vcc into the VBUS
when the pull-up voltage is below Vcc. Proper power management should
include considerations for undervoltage lockout (UVLO) to maintain
consistent performance across its operational range.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/eval-ltc4306/src/common>`.

The macros used in Common Data are defined in platform specific files
found in the 
:git-no-OS:`Project Platform Configuration Path </projects/eval-ltc4306/src/platform>`.

Basic example
~~~~~~~~~~~~~

The basic example in the EVAL-LTC4306 no-OS project initializes an
LTC4306 I²C bus buffer and two MAX5380 DACs, blinks two GPIO-controlled
LEDs for status indication, and cyclically enables downstream I²C
channels to sequentially set and cycle voltages on each DAC. The example
showcases downstream channel switching and multi-DAC control, running
continuously while providing error handling and status output through
logs.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
~~~~~~~~~~~~~~~~~~~~~~~~~

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-LTC4306
- MAX32666FTHR

Connections
^^^^^^^^^^^

The MAX32666FTHR does not have a PMOD interface, but you may use 
Dupont female-female cables to make the required connections. 
The table below shows the connection between
EVAL-LTC4306 and MAX32666FTHR.

+-----------------+-----------------+-----------------+-----------------+
| EVAL-LTC4306Pin | MAX32666 Pin    | Function        | Mnemonic        |
| Number (P3)     | Number          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| VCC             | 3V3             | 3.3 V Supply    | 3V3             |
|                 |                 | (for IO)        |                 |
+-----------------+-----------------+-----------------+-----------------+
| GND             | GND             | Board Ground    | GND             |
+-----------------+-----------------+-----------------+-----------------+
| SCLIN           | SCL             | Serial Clock    | SCL             |
+-----------------+-----------------+-----------------+-----------------+
| SDAIN           | SDA             | Serial Data     | SDA             |
+-----------------+-----------------+-----------------+-----------------+

- Use the PMOD connectors in EVAL-LTC4306 to connect downstream devices.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # Delete the current build
   make reset
   # Build the project
   make PLATFORM=maxim TARGET=max32650
   # Flash the code
   make run

----

ADuCM Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-LTC4306
- EVAL-ADICUP3029

Connections
^^^^^^^^^^^

- Connect the EVAL-LTC4306 board to the EVAL-ADICUP3029 through the PMOD
  I2C headers (P9).

- Use the PMOD connectors in EVAL-LTC4306 to connect downstream devices.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/eval-ltc4306

   export PLATFORM=aducm3029
   # to build the project
   make
   # to flash the code
   make run
   # to debug the code
   make debug
