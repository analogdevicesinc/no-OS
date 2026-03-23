ADGM3121 no-OS Example Project
===============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADGM3121 <https://www.analog.com/ADGM3121>`_

Overview
--------

The ADGM3121 is a MEMS-based SP4T (single-pole, four-throw) switch that
supports both SPI and parallel GPIO control modes. The device provides four
independent switches (SW1-SW4) that can be individually controlled via either
a 16-bit SPI interface or direct GPIO pin inputs.

The ADGM3121 features an internal error detection mechanism, SPI daisy chain
support, and a configurable PIN/SPI mode select pin. Typical switching time
is 200 us and the device requires a 45 ms power-up time.

Applications
------------

* RF signal routing and switching
* Automated test equipment (ATE)
* Software-defined radio
* Instrumentation and measurement

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADGM3121 eval device has to be supplied with 3.3V voltage on VDD pin.

**Pin Description**

	Please see the following table for the pin assignments for the
	interface connector.

	+-----+---------+--------------------------------------------+
	| Pin | Name    | Description                                |
	+-----+---------+--------------------------------------------+
	| 1   | SCLK    | Serial Clock (SPI)                         |
	+-----+---------+--------------------------------------------+
	| 2   | ~CS     | Chip Select (SPI), Active Low              |
	+-----+---------+--------------------------------------------+
	| 3   | SDI     | Serial Data Input (SPI)                    |
	+-----+---------+--------------------------------------------+
	| 4   | SDO     | Serial Data Output (SPI)                   |
	+-----+---------+--------------------------------------------+
	| 5   | PIN/SPI | Mode Select (Low = Parallel, High = SPI)   |
	+-----+---------+--------------------------------------------+
	| 6   | IN1     | Parallel Control Input for SW1 (GPIO)      |
	+-----+---------+--------------------------------------------+
	| 7   | IN2     | Parallel Control Input for SW2 (GPIO)      |
	+-----+---------+--------------------------------------------+
	| 8   | IN3     | Parallel Control Input for SW3 (GPIO)      |
	+-----+---------+--------------------------------------------+
	| 9   | IN4     | Parallel Control Input for SW4 (GPIO)      |
	+-----+---------+--------------------------------------------+
	| 10  | GND     | Ground                                     |
	+-----+---------+--------------------------------------------+
	| 11  | VDD     | Power Supply, +3.3V                        |
	+-----+---------+--------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adgm3121/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adgm3121/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is an interactive example which initializes the ADGM3121 in GPIO parallel
mode and provides a serial console menu to control the four switches. The user
can toggle individual switches, enable or disable all switches, and read the
current switch status. Commands are entered via the serial interface.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adgm3121/Makefile>`_

.. code-block:: bash

	EXAMPLE = basic

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `ADGM3121 <https://www.analog.com/en/products/adgm3121.html>`__
* `MAX32655 <https://www.analog.com/en/products/max32655.html>`_

**Connections**:

+---------------------+----------+--------------------------------------------+---------------------+
| ADGM3121 Pin Number | Mnemonic | Function                                   | MAX32655 Pin Number |
+---------------------+----------+--------------------------------------------+---------------------+
| 1                   | SCLK     | Serial Clock (SPI)                         | P0_7 (SPI1 SCLK)    |
+---------------------+----------+--------------------------------------------+---------------------+
| 2                   | ~CS      | Chip Select (SPI), Active Low              | P0_11 (SPI1 CS1)    |
+---------------------+----------+--------------------------------------------+---------------------+
| 3                   | SDI      | Serial Data Input (SPI)                    | P0_5 (SPI1 MOSI)    |
+---------------------+----------+--------------------------------------------+---------------------+
| 4                   | SDO      | Serial Data Output (SPI)                   | P0_6 (SPI1 MISO)    |
+---------------------+----------+--------------------------------------------+---------------------+
| 5                   | PIN/SPI  | Mode Select (Low = Parallel, High = SPI)   | P0_19               |
+---------------------+----------+--------------------------------------------+---------------------+
| 6                   | IN1      | Parallel Control Input for SW1             | P1_6                |
+---------------------+----------+--------------------------------------------+---------------------+
| 7                   | IN2      | Parallel Control Input for SW2             | P1_7                |
+---------------------+----------+--------------------------------------------+---------------------+
| 8                   | IN3      | Parallel Control Input for SW3             | P1_8                |
+---------------------+----------+--------------------------------------------+---------------------+
| 9                   | IN4      | Parallel Control Input for SW4             | P1_9                |
+---------------------+----------+--------------------------------------------+---------------------+
| 10                  | GND      | Ground                                     | GND                 |
+---------------------+----------+--------------------------------------------+---------------------+
| 11                  | VDD      | Power Supply, +3.3V                        | 3V3                 |
+---------------------+----------+--------------------------------------------+---------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset EXAMPLE=basic PLATFORM=maxim TARGET=max32655
	# to build the project
	make EXAMPLE=basic PLATFORM=maxim TARGET=max32655
	# to flash the code
	make run EXAMPLE=basic PLATFORM=maxim TARGET=max32655
	# to debug the code
	make debug EXAMPLE=basic PLATFORM=maxim TARGET=max32655
