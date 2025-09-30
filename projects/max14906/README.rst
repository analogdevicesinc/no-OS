MAX14906 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `MAX14906EVKIT <https://www.analog.com/MAX14906EVKIT>`_

Overview
--------

The `MAX14906 <https://www.analog.com/MAX14906>`_ is an IEC 61131-2 compliant,
high-speed, four-channel industrial digital output, digital input device that
can be configured on a per-channel basis as a high-side switch, push-pull
driver, or a Type 1 and 3, or Type 2 digital input. The device operates with
a supply voltage range of 10V to 40V and is tolerant up to 65V.

The MAX14906EVKIT evaluation kit is designed to evaluate the MAX14906 device.
It comes with the MAX14906ATM+ part installed, features an on-board FT2232
USB controller for PC connectivity, and provides per-channel configurability
for digital output and input modes. The kit includes diagnostic monitoring for
open-wire conditions, output channel states, undervoltage, overtemperature,
and other fault conditions. The evaluation kit achieves up to 3.75kVRMS
galvanic isolation and supports SafeDemag for safe shutdown of inductive loads
in digital output modes.

Applications
------------

* Industrial Digital Output and Input Modules
* Configurable Digital Input/Output
* Motor Control
* PLC Systems
* Distributed Control Systems (DCS)

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The MAX14906EVKIT requires a 24V DC power supply capable of delivering up to
6.4A. The MAX14906 device operates with a supply voltage range of 10V to 40V
and is tolerant up to 65V.

====================  =================  =================
Supply                Minimum Voltage    Maximum Voltage
====================  =================  =================
VDD, VDD1-VDD4        10 V               40 V
V5                    4.5 V              5.5 V
VL                    2.5 V              5.5 V
VLED                  3.0 V              40 V
====================  =================  =================

Board Connectors
^^^^^^^^^^^^^^^^

====================  ==================================================================
Connector             Description
====================  ==================================================================
USB Port              Connects the evaluation kit to a PC via the on-board FT2232
VDD1 to VDD4          Field supply connectors powering each DOI channel
J13 Header            Access to all digital signals (SPI, D1-D4, SYNCH, READY, FAULT)
SW1                   DIP switch to disconnect on-board FT2232 for external MCU use
CRCEN Jumper          Configures CRC functionality on the SPI interface
GND Test Point        Ground connection for 24 V DC power supply and scope probes
DOI Test Points       Oscilloscope probe points for digital output signal verification
====================  ==================================================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This basic example initializes the MAX14906 device via SPI and configures
a GPIO interrupt handler for fault detection. It demonstrates LED control
by cycling the status LEDs on each channel, sets current limits on the
output channels, and monitors fault conditions through interrupt callbacks.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/Makefile>`_:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO Example
^^^^^^^^^^^

The IIO example demonstrates how to use the Industrial I/O (IIO) framework
to interface with the MAX14906 device. It initializes the IIO application
descriptor and configures the I/O channels, enabling users to interact with
the device using IIO-based tools.

If you are not familiar with the ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with the ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14906/Makefile>`_:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `AD-APARD32690-SL <https://www.analog.com/AD-APARD32690-SL>`_
* `MAX32666FTHR <https://www.analog.com/MAX32666FTHR>`_

**Connections**

When using an external microcontroller, first disconnect the on-board FT2232
controller by opening all switches on SW1 of the MAX14906EVKIT. Then use
jumper wires for point-to-point wiring between the J13 header and the target
board.

+-------------------+-------------+---------------------+------------------+
| Function          | MAX14906    | AD-APARD32690-SL    | MAX32666FTHR     |
|                   | Interface   |                     |                  |
+===================+=============+=====================+==================+
| SPI Device ID     | SPI         | 4                   | 1                |
+-------------------+-------------+---------------------+------------------+
| SPI Chip Select   | SPI         | 0                   | 0                |
+-------------------+-------------+---------------------+------------------+
| SPI Baudrate      | SPI         | 100000              | 100000           |
+-------------------+-------------+---------------------+------------------+
| GPIO Fault Port   | GPIO        | 2                   | 0                |
+-------------------+-------------+---------------------+------------------+
| GPIO Fault Pin    | GPIO        | 21                  | 5                |
+-------------------+-------------+---------------------+------------------+
| GPIO IRQ ID       | GPIO        | 2                   | 0                |
+-------------------+-------------+---------------------+------------------+

**Build Command**

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
^^^^^^^^^^^^^^

**Used Hardware**

* `MAX14906EVKIT <https://www.analog.com/MAX14906EVKIT>`_
* NUCLEO-F401RE

**Connections**

The MAX14906EVKIT communicates via SPI. Connect the following pins between
the NUCLEO-F401RE board and the MAX14906EVKIT J13 header:

+-------------+----------------------+-------------+
| MAX14906    | Function             | STM32F401RE |
| Signal      |                      | Pin         |
+=============+======================+=============+
| FAULT       | Interrupt            | PA0         |
+-------------+----------------------+-------------+
| SPI CS      | Chip Select          | PA4         |
+-------------+----------------------+-------------+
| SPI MISO    | Data from MAX14906   | PA6         |
+-------------+----------------------+-------------+
| SPI MOSI    | Data to MAX14906     | PA7         |
+-------------+----------------------+-------------+
| SPI SCK     | SPI Clock            | PB3         |
+-------------+----------------------+-------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=stm32
	# to flash the code
	make run
