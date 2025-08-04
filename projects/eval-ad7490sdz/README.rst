EVAL-AD7490SDZ no-OS Example Project
====================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7490SDZ <https://www.analog.com/EVAL-AD7490SDZ>`_

Overview
--------

The EVAL-AD7490SDZ is a fully featured evaluation kit for the AD7490.
This board operates in standalone mode or in conjunction with the
System Development Platform, EVAL-SDP-CB1Z.

Hardware Specifications
-----------------------

The EVAL-AD7490SDZ device has to be supplied with a 7-9V DC voltage and
a 3V3/5V VDRIVE voltage from the microcontroller (for the logic level
voltage of the SPI communication).

**Pin Description**

	Please see the following table for the pin assignments for the
	interface connector (SPI Test Points).

	+-----+--------+--------------------------------+
	| Pin |  Name  | Description			|
	+-----+--------+--------------------------------+
	| -   |	T_CS   | Chip-Select			|
	+-----+--------+--------------------------------+
	| -   | T_DIN  | Master-Out Slave-In (MOSI)	|
	+-----+--------+--------------------------------+
	| -   | T_DOUT | Master-In Slave-Out (MISO)	|
	+-----+--------+--------------------------------+
	| -   | T_SCLK | Serial Clock			|
	+-----+--------+--------------------------------+

	Please see the following table for the pin assignments for the
	power supply connectors (J8 and J700, respectively).

	+-----+--------+--------------------------------+
	| Pin |  Name  | Description			|
	+-----+--------+--------------------------------+
	| 1   |	VDRIVE | 5V/3V3 voltage of the MCU	|
	+-----+--------+--------------------------------+
	| 2   |  GND   | Ground				|
	+-----+--------+--------------------------------+

	+-----+--------+--------------------------------+
	| Pin |  Name  | Description			|
	+-----+--------+--------------------------------+
	| 1   |EXT_PWR | 7V-9V External DC voltage	|
	+-----+--------+--------------------------------+
	| 2   |EXT_GND | External Ground		|
	+-----+--------+--------------------------------+

	Also please refer to the following table for the jumper positions on
	the board.

	+--------+--------------+
	| Jumper | Position	|
	+--------+--------------+
	| LK701	 | A		|
	+--------+--------------+
	| LK101	 | A		|
	+--------+--------------+
	| LK102	 | A		|
	+--------+--------------+
	| LK19	 | C		|
	+--------+--------------+
	| LK17	 | A		|
	+--------+--------------+

No-OS Build Setup
-----------------

`Please see: <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7490sdz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7490sdz/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example which initializes the ad7490 and reads a single
voltage channel continuoussly applying scale to it.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7490sdz/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	EXAMPLE ?= basic

IIO example
^^^^^^^^^^^

This is an example that initializes the ad7490 iio descriptor through which
an IIO device is initialized as well as it's channels. Then an iio
application initialization parameter is used for initializing and
running an IIO app through the IIO lib. Finally the client can
configure the ad7490 in every way possible througout an IIO
interface such as IIO oscilloscope.

In order to build the IIO example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7490sdz/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	EXAMPLE ?= iio_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-AD7490SDZ <https://www.analog.com/EVAL-AD7490SDZ>`_
* `AD-APARD32690-SL Board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**:

+-------------------------------+----------+---------------------------------+-----------------------------+
| EVAL-AD7490SDZ Pin Name 	| Mnemonic | Function			     | AD-APARD32690-SL Pin Number |
+-------------------------------+----------+---------------------------------+-----------------------------+
| T_CS			        | CS       | SPI interface Chip-Select	     | P1_0 		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+
| T_DIN				| MOSI     | SPI interface MOSI		     | P1_1		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+
| T_DOUT			| MISO     | SPI interface MISO     	     | P1_2		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+
| T_SCLK			| SCLK	   | SPI interface Serial Clock Line | P1_3		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+
| VDRIVE			| 5V/3V3   | 5V/3.3V DC power supply	     | 5V/3V3		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+
| GND				| GND	   | Ground			     | GND		       	   |
+-------------------------------+----------+---------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
