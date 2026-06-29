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
	| VREF     | Interface reference voltage. Sets the     |
	|          | digital I/O levels; tie to the same rail  |
	|          | as VS (3.3 V for ADXL371, 2.5 V for       |
	|          | ADXL372)                                  |
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

IIO Example
^^^^^^^^^^^

This example exposes the ADXL37x over libiio via a UART backend using
the no-OS IIO framework:

* Initializes the ADXL37x IIO driver
* Exposes 3 acceleration channels (X, Y, Z) with attributes for raw data,
  scale, sampling frequency, LPF corner frequency, HPF enable, and per-axis calibbias
* Supports buffered data reads from the FIFO

.. warning::
	The selection of axes used trough IIO is done trough the main window of IIO oscilloscope. 

No-OS Supported Platforms
--------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `ADXL371 <https://www.analog.com/ADXL371>`_ or `ADXL372 <https://www.analog.com/ADXL372>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

For ADXL371, use a power supply and a reference voltage of 3.3V
For ADXL372, use 2.5V for both. 

+------------------+---------------------------+-------------------------+
| ADXL37x Pin      | Function                  | MAX32655FTHR Pin        |
+------------------+---------------------------+-------------------------+
| VS               | Power Supply (3.3 V)      | 3V3                     |
+------------------+---------------------------+-------------------------+
| VREF             | Interface Reference       | 3V3 (ADXL371) /         |
|                  | Voltage                   | external 2.5 V (ADXL372)|
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

Both examples talk to the host over UART3 (LPUART0) at 230400 baud, not over
the MAX32655FTHR's onboard USB connector. You need a separate USB-to-UART
cable (3.3 V logic level, for example an FTDI TTL-232R-3V3)
wired to the pins below. Note that RX and TX cross over.

+------------------+---------------------------+-------------------------+
| MAX32655FTHR Pin | Function                  | USB-to-UART Cable       |
+------------------+---------------------------+-------------------------+
| P2_6             | UART3 RX                  | TX                      |
+------------------+---------------------------+-------------------------+
| P2_7             | UART3 TX                  | RX                      |
+------------------+---------------------------+-------------------------+
| GND              | Ground                    | GND                     |
+------------------+---------------------------+-------------------------+

.. warning::
	**After connecting to ADXL trough IIO**
	Select 170 samples in IIO oscilloscope.

**Selecting the number of axes and which of them are used**

By selecting the number of axes used, the driver computes
how many samples and how many bytes does a sample have.
This reduces the risk of overflow and keeps the FIFO in
a safe state.

This is used only for the no-OS driver.

.. code-block:: bash

	#define AXES_USED 3
	#define BYTES_PER_SAMPLE (2*AXES_USED)

	#if AXES_USED==3
	#define MAX_SAMPLES 170 //the maximum number of samples in the fifo, according to the number of axes used
	#elif AXES_USED==2
	#define MAX_SAMPLES 256
	#elif AXES_USED==1
	#define MAX_SAMPLES 512
	#endif

	#define EXTRACTED_SAMPLES MAX_SAMPLES-1  //at least 1 sample left in the FIFO for sample integrity

The used axes can be selected from this enum, in adxl37x.h file:

.. code-block:: bash
	
	enum adxl37x_fifo_format {
		ADXL37X_XYZ_FIFO,
		ADXL37X_X_FIFO,
		ADXL37X_Y_FIFO,
		ADXL37X_XY_FIFO,
		ADXL37X_Z_FIFO,
		ADXL37X_XZ_FIFO,
		ADXL37X_YZ_FIFO,
		ADXL37X_XYZ_PEAK_FIFO,
	};

**Build Command**

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``basic``, ``iio``.
Available boards: ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly; see the combination list with
``python tools/scripts/no_os_build.py list --project eval-adxl37x``.

.. code-block:: bash

   # point at the Maxim SDK libraries (only if not auto-detected)
   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the basic example on MAX32655FTHR
   python tools/scripts/no_os_build.py build \
      --project eval-adxl37x --variant basic --board max32655fthr

   # build the IIO example
   python tools/scripts/no_os_build.py build \
      --project eval-adxl37x --variant iio --board max32655fthr

   # remove the build directory and reconfigure from scratch
   python tools/scripts/no_os_build.py build \
      --project eval-adxl37x --variant basic --board max32655fthr --clean

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-adxl37x --variant basic --board max32655fthr \
      --probe openocd --flash
