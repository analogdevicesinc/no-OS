Evaluating the EVAL-ADXL366Z
=============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADXL366Z <https://www.analog.com/ADXL366>`_

Overview
--------

This evaluation project demonstrates the ADXL366 ultra-low-power 3-axis
accelerometer driver integration across multiple platforms. The ADXL366
consumes only 2 μA at 100 Hz ODR and 370 nA in motion-triggered wake-up mode.

The project includes a complete Phase 1 feature set with hardware-validated
examples on the MAX32655 FTHR platform. All 14 core driver functions have been
tested and validated with the ADXL366Z evaluation board.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADXL366Z evaluation board requires:

* VS: 1.6V to 3.5V (typically 3.3V)
* VDDIO: 1.6V to VS (typically 3.3V)

Board Connections
^^^^^^^^^^^^^^^^^

**ADXL366Z Pin Description**

	+----------+-------------------------------------+
	| Name     | Description                         |
	+----------+-------------------------------------+
	| VS       | Analog power supply                 |
	+----------+-------------------------------------+
	| VDDIO    | Digital I/O power supply            |
	+----------+-------------------------------------+
	| GND      | Ground                              |
	+----------+-------------------------------------+
	| CS       | SPI Chip Select (active low)        |
	+----------+-------------------------------------+
	| SCLK     | SPI Clock                           |
	+----------+-------------------------------------+
	| MOSI     | SPI Master Out Slave In             |
	+----------+-------------------------------------+
	| MISO     | SPI Master In Slave Out             |
	+----------+-------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl366z/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl366z/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* Initializes the ADXL366
* Configures range and output data rate
* Continuously reads and displays accelerometer data
* Updates at 500ms intervals

The example demonstrates basic usage of the ADXL366 driver for reading
3-axis acceleration data in units of g.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl366z/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	COMPREHENSIVE_EXAMPLE = n

Comprehensive Test Example
^^^^^^^^^^^^^^^^^^^^^^^^^^

This example validates all 14 driver functions with explicit test labeling:

* Device initialization and ID verification
* Range configuration (±2g, ±4g, ±8g)
* Output data rate configuration (12.5Hz - 400Hz)
* Temperature sensor reading
* Accelerometer data acquisition
* Status register reading
* Power mode control (standby/measurement)
* Register read/write operations

Each function is tested individually with pass/fail reporting. This example
is ideal for hardware validation and driver verification.

In order to build the comprehensive test example make sure you have the
following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl366z/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	COMPREHENSIVE_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADXL366Z <https://www.analog.com/ADXL366>`_
* `MAX32655FTHR <https://www.analog.com/MAX32655>`_

**Connections**:

+------------------+----------------------------------+----------------------+
| ADXL366Z Pin     | Function                         | MAX32655 FTHR Pin    |
+------------------+----------------------------------+----------------------+
| VS               | Analog Power Supply              | 3.3V                 |
+------------------+----------------------------------+----------------------+
| VDDIO            | Digital I/O Power Supply         | 3.3V                 |
+------------------+----------------------------------+----------------------+
| GND              | Ground                           | GND                  |
+------------------+----------------------------------+----------------------+
| CS               | SPI Chip Select                  | P0.20 (SS0)          |
+------------------+----------------------------------+----------------------+
| SCLK             | SPI Clock                        | P0.23 (SCK)          |
+------------------+----------------------------------+----------------------+
| MOSI             | SPI Master Out Slave In          | P0.21 (MOSI)         |
+------------------+----------------------------------+----------------------+
| MISO             | SPI Master In Slave Out          | P0.22 (MISO)         |
+------------------+----------------------------------+----------------------+

**Build Command**

.. code-block:: bash

	# Set MAXIM_LIBRARIES environment variable
	export MAXIM_LIBRARIES=/path/to/maxim/libraries

	# to delete current build
	make PLATFORM=maxim TARGET=max32655 clean

	# to build the project with basic example
	make PLATFORM=maxim TARGET=max32655 BASIC_EXAMPLE=y

	# to build the project with comprehensive test
	make PLATFORM=maxim TARGET=max32655 COMPREHENSIVE_EXAMPLE=y

**Flash Command**

.. code-block:: bash

	openocd -f interface/cmsis-dap.cfg -f target/max32655.cfg \
	  -c "program build/eval-adxl366z.elf verify reset exit"

Hardware Testing Results
------------------------

All 14 driver functions have been tested and validated on MAX32655 FTHR +
ADXL366Z hardware (March 30th, 2026):

* ✅ Device initialization and ID verification
* ✅ Range configuration (±2g, ±4g, ±8g)
* ✅ ODR configuration (12.5Hz, 50Hz, 100Hz, 400Hz)
* ✅ Temperature reading (25°C measured)
* ✅ Accelerometer data reading (~1.4g magnitude)
* ✅ Status register reading (DATA_RDY, AWAKE verified)
* ✅ Register write operations
* ✅ Multi-byte read operations
* ✅ Power mode control (standby/measure)

For detailed test output, see the project README.md file.
