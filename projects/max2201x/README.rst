Evaluating the MAX2201X
=======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX22017_EVKIT <https://www.analog.com/MAX22017EVKIT>`_

Overview
--------

The MAX22017 evaluation kit provides the hardware to evaluate the MAX22017
GPIO controller and DAC voltage outputs.

Refer to the MAX22017 IC data sheet for detailed information regarding
operationof the IC.

Applications
------------

* Programmable Logic Controllers 
* Programmable Automation Controllers 
* Distributed Control Systems

Hardware Specifications
-----------------------

The MAX22017EVKIT device has to be supplied with a 24V DC output AC adapter and
a micro-USB cable.

**Pin Description**

	Please see the following table for the pin assignments for the
	interface connector (J201).

	+-----+--------+--------------------------------+
	| Pin |  Name  | Description			|
	+-----+--------+--------------------------------+
	| 1   |	  CS   | Chip-Select			|
	+-----+--------+--------------------------------+
	| 2   |  DNC   | Do Not Connect			|
	+-----+--------+--------------------------------+
	| 3   |  MOSI  | Master-Out Slave-In		|
	+-----+--------+--------------------------------+
	| 4   |  RSTB  | Active-Low Reset Pin		|
	+-----+--------+--------------------------------+
	| 5   |  MISO  | Master-In Slave-Out		|
	+-----+--------+--------------------------------+
	| 6   |  DNC   | Do Not Connect			|
	+-----+--------+--------------------------------+
	| 7   |  SCLK  | Serial Clock			|
	+-----+--------+--------------------------------+
	| 8   |  DNC   | Do Not Connect			|
	+-----+--------+--------------------------------+
	| 9   |  GND   | Ground				|
	+-----+--------+--------------------------------+
	| 10  |  GND   | Ground				|
	+-----+--------+--------------------------------+
	| 11  |  DNC   | Do Not Connect			|
	+-----+--------+--------------------------------+
	| 12  |  DNC   | Do Not Connect			|
	+-----+--------+--------------------------------+

	Turn off all the switches at the SW1, connect micro-USB cable to J202
	and connect 24V DC power supply to J5.

	Also please refer to the following table for the jumper positions on
	the board.

	+--------+--------------+
	| Jumper | Position	|
	+--------+--------------+
	| J11	 | 1-2		|
	+--------+--------------+
	| J10	 | 1-2		|
	+--------+--------------+
	| J6	 | 1-2		|
	+--------+--------------+
	| J7	 | 1-2		|
	+--------+--------------+
	| J8	 | 1-2		|
	+--------+--------------+
	| J9	 | 1-2		|
	+--------+--------------+
	| J12	 | 1-2		|
	+--------+--------------+
	| J1	 | 2-3		|
	+--------+--------------+

No-OS Build Setup
-----------------

`Please see: <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max2201x/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max2201x/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example which initializes the max22017 and configures the
first channel's AO, then recreating a triangle wave.

Values of the maximum value, frequency and increment can be changed in the
basic_example.c source file.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max2201x/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This is an example that initializes the max22017 iio descriptor through which
an IIO device is initialized as well as it's channels then to be implemented
in an iio application initialization parameter used for initializing and running
an IIO app through the IIO lib. After which the client can configure the
MAX22017 in every way possible througout an IIO interface such as IIO
oscilloscope.

In order to build the IIO example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max2201x/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX22017EVKIT <https://www.analog.com/MAX22017EVKIT>`_
* `MAX32666FTHR Board <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html#eb-documentation>`_

**Connections**:

+-------------------------------+----------+---------------------------------+-------------------------+
| MAX22017EVKIT J201 Pin Number | Mnemonic | Function			     | MAX32666FTHR Pin Number |
+-------------------------------+----------+---------------------------------+-------------------------+
| 1			        | CS       | SPI interface Chip-Select	     | P0_16		       |
+-------------------------------+----------+---------------------------------+-------------------------+
| 3				| MOSI     | SPI interface MOSI		     | P0_17		       |
+-------------------------------+----------+---------------------------------+-------------------------+
| 4				| RSTB     | Active-Low Reset PIN (GPIO)     | P0_5		       |
+-------------------------------+----------+---------------------------------+-------------------------+
| 5				| MISO	   | SPI interface MISO		     | P0_18		       |
+-------------------------------+----------+---------------------------------+-------------------------+
| 7			        | SCLK	   | SPI interface Serial Clock Line | P0_19		       |
+-------------------------------+----------+---------------------------------+-------------------------+
| 9				| UGND	   | Ground			     | GND		       |
+-------------------------------+----------+---------------------------------+-------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
