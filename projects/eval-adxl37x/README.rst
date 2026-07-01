EVAL-ADXL37x no-OS Example Project
===================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADXL371 <https://www.analog.com/ADXL371>`_
* `ADXL372 <https://www.analog.com/ADXL372>`_

Overview
--------

The ADXL371 and ADXL372 are ultra low power, 3-axis, ±200 g MEMS
accelerometers. Both devices share the same register map and SPI/I2C
interface; they differ in their internal clock frequency, which results
in different ODR, bandwidth, and wake-up rate values. This project
demonstrates FIFO-based data acquisition, impact detection, and triggered
mode capture using the no-OS ADXL37x driver on the MAX32655FTHR platform.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADXL371/ADXL372 operate from a single 3.3 V supply. On the
MAX32655FTHR platform, the 3.3 V rail is used for both VS and the SPI
interface logic.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description                               |
	+----------+-------------------------------------------+
	| VS       | Connect to 3.3 V supply voltage           |
	+----------+-------------------------------------------+
	| GND      | Connect to ground                         |
	+----------+-------------------------------------------+
	| CS       | SPI chip select (active low)              |
	+----------+-------------------------------------------+
	| SCK      | SPI clock                                 |
	+----------+-------------------------------------------+
	| MOSI     | SPI data in (SDI)                         |
	+----------+-------------------------------------------+
	| MISO     | SPI data out (SDO)                        |
	+----------+-------------------------------------------+
	| INT1     | Interrupt 1 output                        |
	+----------+-------------------------------------------+
	| INT2     | Interrupt 2 output                        |
	+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl37x/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl37x/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* Initializes the ADXL37x over SPI
* Configures the FIFO in streamed XYZ mode
* Continuously reads acceleration data from the FIFO and prints it over UART

Impact Detection Example
^^^^^^^^^^^^^^^^^^^^^^^^

This example configures the device for low-power impact detection using
Instant-On mode:

* Sets activity/inactivity thresholds and LOOPED processing mode
* Configures the FIFO in streamed XYZ peak mode
* Enters Instant-On mode and waits for impact events
* On each event, reads and prints the captured FIFO samples over UART

Triggered Mode Example
^^^^^^^^^^^^^^^^^^^^^^

This example captures full acceleration events surrounding activity
threshold crossings using triggered FIFO mode:

* Sets activity/inactivity thresholds and LOOPED processing mode
* Configures the FIFO in triggered XYZ mode with trailing sample capture
* Reacts on FIFO_RDY (INT2) to read and print event data over UART

IIO Example
^^^^^^^^^^^

This example exposes the ADXL37x over libiio via a UART backend using
the no-OS IIO framework:

* Initializes the ADXL37x IIO driver
* Exposes 3 acceleration channels (X, Y, Z) with attributes for raw data,
  scale, sampling frequency, LPF corner frequency, and per-axis calibbias
* Supports buffered data reads from the FIFO

No-OS Supported Platforms
--------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `ADXL371 <https://www.analog.com/ADXL371>`_ or `ADXL372 <https://www.analog.com/ADXL372>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

+------------------+---------------------------+-------------------------+
| ADXL37x Pin      | Function                  | MAX32655FTHR Pin        |
+------------------+---------------------------+-------------------------+
| VS               | Power Supply (3.3 V)      | 3V3                     |
+------------------+---------------------------+-------------------------+
| GND              | Ground                    | GND                     |
+------------------+---------------------------+-------------------------+
| CS               | SPI Chip Select           | P0_20 (SPI0 CS0)        |
+------------------+---------------------------+-------------------------+
| SCK              | SPI Clock                 | P0_16 (SPI0 SCK)        |
+------------------+---------------------------+-------------------------+
| MOSI             | SPI Data In               | P0_21 (SPI0 MOSI)       |
+------------------+---------------------------+-------------------------+
| MISO             | SPI Data Out              | P0_22 (SPI0 MISO)       |
+------------------+---------------------------+-------------------------+
| INT1             | Interrupt 1               | P1_6                    |
+------------------+---------------------------+-------------------------+
| INT2             | Interrupt 2               | P1_7                    |
+------------------+---------------------------+-------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset PLATFORM=maxim TARGET=max32655
	# to build the basic example
	make PLATFORM=maxim TARGET=max32655 EXAMPLE=basic_example
	# to build the impact detection example
	make PLATFORM=maxim TARGET=max32655 EXAMPLE=impact_example
	# to build the triggered mode example
	make PLATFORM=maxim TARGET=max32655 EXAMPLE=triggered_example
	# to build the IIO example
	make PLATFORM=maxim TARGET=max32655 EXAMPLE=iio_example
	# to flash the code
	make run PLATFORM=maxim TARGET=max32655
