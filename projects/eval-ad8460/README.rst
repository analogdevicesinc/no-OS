Evaluating the EVAL-AD8460
===========================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD8460 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad8460.html>`_

Overview
--------

The EVAL-AD8460SDZ board enables easy evaluation of the AD8460, offered in a 12
mm x 12 mm, 80-lead thin quad flat package (TQFP) with an exposed pad at the top
for a mountable heat sink. The evaluation board provides a platform for quick
and easy evaluation of the AD8460 for various user-defined configurations.

The AD8460 is ideally suited for demanding applications such as high-speed
arbitrary waveform generation, programmable power supplies, and LCD/OLED panel
formation.

The evaluation board hardware and software enable full operation of the AD8460
analog pattern generation (APG) and arbitrary waveform generation (AWG) modes.
The AD8460 data sheet provides the full specifications of the AD8460 and details
on the device operation. Consult it in conjunction with the user guide.

Hardware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project, an external 5V supply is connected to the VCC_5V pin
to supply the SPI communication block of EVAL-AD8460.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description			       |
	+----------+-------------------------------------------+
	| VCC_5V   | Connect to 5V supply		       |
	+----------+-------------------------------------------+
	| GND      | Connect to Ground			       |
	+----------+-------------------------------------------+
	| SCK      | Connect to SPI Clock (SCK)		       |
	+----------+-------------------------------------------+
	| SDO      | Connect to SPI Master In Slave Out (MISO) |
	+----------+-------------------------------------------+
	| CS       | Connect to SPI Chip Select (CS)	       |
	+----------+-------------------------------------------+
	| SDI      | Connect to SPI Master Out Slave In (MOSI) |
	+----------+-------------------------------------------+
	| RESETB   | Connect to GPIO pin (RESET)	       |
	+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad8460/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad8460/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that:

* initializes the AD8460
* resets the AD8460
* switches modes between APG and AWG
* writes then reads HVDAC registers
* and reads the shutdown flag

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad8460/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD8460. The project launches a IIOD
server on the board so that the user may connect to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD8460 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad8460/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad8460/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-AD8460 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad8460.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------+---------------------------------+-------------------------+
| EVAL-AD8460 Pin | Function			    | MAX32666FTHR Pin        |
+-----------------+---------------------------------+-------------------------+
| VCC_5V          | Low Voltage Power Supply	    | 5V Supply (External)    |
+-----------------+---------------------------------+-------------------------+
| SCK             | SPI Clock (SCK)		    | AIN3 (SPI1_SCK)	      |
+-----------------+---------------------------------+-------------------------+
| SDO             | SPI Master In Slave Out (MISO)  | AIN2 (SPI1_MISO)	      |
+-----------------+---------------------------------+-------------------------+
| CS              | SPI Chip Select (CS)	    | AIN0 (SPI1_SS0)	      |
+-----------------+---------------------------------+-------------------------+
| SDI             | SPI Master Out Slave In (MOSI)  | AIN1 (SPI1_MOSI)        |
+-----------------+---------------------------------+-------------------------+
| RESETB          | GPIO (RESET Pin)		    | AIN4 (P0_27)	      |
+-----------------+---------------------------------+-------------------------+
| GND             | Ground (GND) 		    | GND		      |
+-----------------+---------------------------------+-------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32665 reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32665 run
