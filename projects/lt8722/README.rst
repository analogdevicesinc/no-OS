Evaluating the LT8722
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC3145A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3145a.html>`_

Overview
--------

Demonstration circuit 3145A demonstrates a high efficiency thermoelectric cooler
driver and features the LT8722 monolithic driver.

The input voltage range of the DC3145A is from 3.1V to 15V and the output is a
SPI programmable differential voltage output. The maximum output current is 4A.
The switching frequency SPI programmable from 500kHz to 3MHz.

The LT8722 is controlled through an SPI interface and therefore the
demonstration circuit is also setup to interface through SPI.

Full performance details are provided in the LT8722 data sheet, which should
be consulted in conjunction with user guide.

Hadrware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply is used to provide a 3.1V
to 15V input voltage for the DC3145A evaluation board. The board also needs a
2.7V to 5.5V supply to power the serial interface.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The (JP1) jumper has two positions: LIN and MAN. Connect the jumper to the LIN
position to allow the MCU to control the state of the SWEN pin via GPIO.

The (JP3) jumper has two positions: ENABLE and LIN. Connect the jumper to the
LIN position to allow the MCU to control the state of the EN pin via GPIO.

**Pin Description**

	Please see the following table for the pin assignments for the interface
	connector (JP2).

	JP2:

	+-----+----------+-------------------------------------------+
	| Pin | Name     | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | NC       | No Connection			     |
	+-----+----------+-------------------------------------------+
	| 2   | L-VSPI   | Connect to VDDIO supply (2.7V to 5.5V)    |
	+-----+----------+-------------------------------------------+
	| 3   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 4   | SCK      | Connect to SPI Clock (SCK)		     |
	+-----+----------+-------------------------------------------+
	| 5   | MISO     | Connect to SPI Master In Slave Out (MISO) |
	+-----+----------+-------------------------------------------+
	| 6   | #CS      | Connect to SPI Chip Select (CS)	     |
	+-----+----------+-------------------------------------------+
	| 7   | MOSI     | Connect to SPI Master Out Slave In (MOSI) |
	+-----+----------+-------------------------------------------+
	| 8   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 9   | NC       | No Connection			     |
	+-----+----------+-------------------------------------------+
	| 10  | NC       | No Connection			     |
	+-----+----------+-------------------------------------------+
	| 11  | LIN_ENA  | Connect to GPIO pin (ENABLE)		     |
	+-----+----------+-------------------------------------------+
	| 12  | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 13  | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 14  | LIN_SWEN | Connect to GPIO pin (SWEN)		     |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt8722/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt8722/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LT8722, perform the start-up
sequence and configure the output voltage.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt8722/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for DC3145A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LT8722 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt8722/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt8722/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC3145A evaluation board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3145a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+--------------------------+----------------------------------------------+------------------+
| DC3145A (JP2) Pin Number | Function					  | MAX32666FTHR Pin |
+--------------------------+----------------------------------------------+------------------+
| 2                        | VDDIO (2.7V to 5.5V)			  | 3V3              |
+--------------------------+----------------------------------------------+------------------+
| 4                        | SPI Clock (SCK)				  | AIN3 (SPI1_SCK)  |
+--------------------------+----------------------------------------------+------------------+
| 5                        | SPI Master In Slave Out (MISO)		  | AIN2 (SPI1_MISO) |
+--------------------------+----------------------------------------------+------------------+
| 6                        | SPI Chip Select (CS)			  | AIN0 (SPI1_SS0)  |
+--------------------------+----------------------------------------------+------------------+
| 7                        | SPI Master Out Slave In (MOSI)		  | AIN1 (SPI1_MOSI) |
+--------------------------+----------------------------------------------+------------------+
| 11                       | GPIO (EN Pin)				  | AIN4 (P0_20)     |
+--------------------------+----------------------------------------------+------------------+
| 12                       | Ground (GND) 				  | GND              |
+--------------------------+----------------------------------------------+------------------+
| 14                       | GPIO (SWEN Pin)                              | AIN5 (P0_21)     |
+--------------------------+----------------------------------------------+------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32665 reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32665 run
