Evaluating the LTC2378-20
=========================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

`DC2135A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2135a.html>`_

Overview
--------

The DC2135A evaluation board features the LTC2378-20, a high-speed 20-bit SAR
ADC that can be configured for both unipolar (0 to VREF) and bipolar (±VREF)
input ranges. The board provides all necessary hardware connections and signal
conditioning for comprehensive ADC evaluation.

The evaluation board provides easy access to all necessary signals through
standard connectors, enabling quick setup for high-speed data acquisition
applications up to 1Msps sample rate.

For full performance details, refer to the LTC2378-20 data sheet, which should
be consulted in conjunction with the user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The DC2135A evaluation board requires external ±16V supplies for the analog
input circuitry and signal conditioning. The LTC2378-20 ADC itself operates
from a regulated 2.5V supply that is generated on-board.

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description			       |
	+----------+-------------------------------------------+
	| GND      | Connect to Ground			       |
	+----------+-------------------------------------------+
	| SCK      | Connect to SPI Clock (SCK)		       |
	+----------+-------------------------------------------+
	| SDO      | Connect to SPI Master In Slave Out (MISO) |
	+----------+-------------------------------------------+
	| SDI      | Connect to SPI Master Out Slave In (MOSI) |
	+----------+-------------------------------------------+
	| CNV      | Connect to GPIO pin (CNV)		       |
	+----------+-------------------------------------------+
	| BUSY     | Connect to GPIO pin (BUSY)		       |
	+----------+-------------------------------------------+

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the user guide for the corresponding demo board:
`DC2135A user guide <https://www.analog.com/media/en/technical-documentation/user-guides/DC2135AF.PDF>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2378/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2378/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTC2378-20, perform the start-up
sequence and configure the output voltage.

To build the basic example, run:

.. code-block:: bash

	make EXAMPLE=basic TARGET=max32665

Or alternatively:

.. code-block:: bash

	export EXAMPLE=basic
	make TARGET=max32665

IIO example
^^^^^^^^^^^

This project is actually an IIOD demo for DC2135A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTC2378-20 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2378/src/examples/iio>`_

To build the IIO example, run:

.. code-block:: bash

	make EXAMPLE=iio TARGET=max32665

Or alternatively:

.. code-block:: bash

	export EXAMPLE=iio
	make TARGET=max32665

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2135A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2135a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:


+--------------------------+----------------------------------------------+------------------+
| DC2135A Pin              | Function					  | MAX32666FTHR Pin |
+--------------------------+----------------------------------------------+------------------+
| SCK                      | SPI Clock (SCK)				  | AIN3 (SPI1_SCK)  |
+--------------------------+----------------------------------------------+------------------+
| SDO                      | SPI Master In Slave Out (MISO)		  | AIN2 (SPI1_MISO) |
+--------------------------+----------------------------------------------+------------------+
| SDI                      | SPI Master Out Slave In (MOSI)		  | AIN1 (SPI1_MOSI) |
+--------------------------+----------------------------------------------+------------------+
| CNV                      | GPIO (CNV Pin)				  | P0_9             |
+--------------------------+----------------------------------------------+------------------+
| BUSY                     | GPIO (BUSY Pin)				  | P0_10            |
+--------------------------+----------------------------------------------+------------------+
| GND                      | Ground (GND) 				  | GND              |
+--------------------------+----------------------------------------------+------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32665 reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32665 run
