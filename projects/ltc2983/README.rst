Evaluating the LTC2983
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC2296A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2296a.html>`_
* `DC2420A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2420a.html>`_
* `DC2531A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2531a.html>`_

Overview
--------

The **DC2296/DC2420/DC2531** is the starter kit for demonstrating the performance
and ease of use of the LTC2983/LTC2984/LTC2986, which is a complete temperature
measurement system on a chip. This kit includes the **DC2209/DC2399/DC2508**
(main demo circuit containing the **LTC2983/LTC2984/LTC2986**) and the
**DC2210** (a simple experiment circuit allowing bread boarding). In addition to
the starter demonstration kit, sensor specific demonstration boards highlighting
the performance of RTDs, thermistors, or thermocouples are also available. 

The DC2209/DC2399/DC2508 is a member of the QuikEval family of demonstration
boards. It is designed to allow easy evaluation of the LTC2983/LTC2984/LTC2986
and may be connected to any one of the sensor daughter boards.

These daughter boards allow evaluation of the various LTC2983/LTC2984/LTC2986
sensor types:

* Universal Temperature Measurement Board - **DC2211**
* Thermocouple Board - **DC2212**
* Dedicated RTD Board - **DC2213**
* Dedicated Thermistor Board - **DC2214**

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project, the DC2209/DC2399/DC2508 evaluation board is powered
by the 3V3 supply from MAX32666FTHR.

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description			       |
	+----------+-------------------------------------------+
	| VDD      | Connect to 3V3 supply		       |
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
	| RESET    | Connect to GPIO pin (RESET)	       |
	+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2983/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2983/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTC2983, perform the start-up
sequence and configure the output voltage.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2983/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually an IIOD demo for DC2209/DC2399/DC2508 evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTC2983 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2983/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2983/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2209A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2296a.html>`_
* `DC2399A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2420a.html>`_
* `DC2508A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2531a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+--------------------------+----------------------------------------------+------------------+
| DC2209/DC2399/DC2508 Pin | Function					  | MAX32666FTHR Pin |
+--------------------------+----------------------------------------------+------------------+
| VDD                      | VDD					  | 3V3              |
+--------------------------+----------------------------------------------+------------------+
| SCK                      | SPI Clock (SCK)				  | AIN3 (SPI1_SCK)  |
+--------------------------+----------------------------------------------+------------------+
| SDO                      | SPI Master In Slave Out (MISO)		  | AIN2 (SPI1_MISO) |
+--------------------------+----------------------------------------------+------------------+
| CS                       | SPI Chip Select (CS)			  | AIN0 (SPI1_SS0)  |
+--------------------------+----------------------------------------------+------------------+
| SDI                      | SPI Master Out Slave In (MOSI)		  | AIN1 (SPI1_MOSI) |
+--------------------------+----------------------------------------------+------------------+
| RESET                    | GPIO (RESET Pin)				  | AIN4 (P0_27)     |
+--------------------------+----------------------------------------------+------------------+
| GND                      | Ground (GND) 				  | GND              |
+--------------------------+----------------------------------------------+------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32665 reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32665 run
