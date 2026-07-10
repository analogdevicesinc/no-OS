EVAL-ADXL38x no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADXL38x <https://www.analog.com/en/products/adxl382.html>`_

Overview
--------

The EVAL-ADXL38x evaluation board enables quick evaluation of the ADXL38x
family of low noise, high-g MEMS accelerometers. The ADXL380 and ADXL382
are ultralow noise, low power, 3-axis accelerometers designed for
high-g industrial applications including vibration monitoring, condition
monitoring, and inertial measurement. The devices provide SPI and I2C
digital interfaces and support measurement ranges up to ±15 g (ADXL380)
and ±30 g (ADXL382).

Applications
------------

* Vibration monitoring and condition monitoring
* Inertial measurement units (IMUs)
* Industrial machinery health monitoring
* Structural health monitoring
* High-g impact and shock detection

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADXL38x board must be supplied with 3.3 V on the VS and VDDIO
pins. The host system should be capable of providing a stable 3.3 V supply.

Connections
~~~~~~~~~~~

The EVAL-ADXL38x communicates with the host microcontroller via SPI.
The following signals are required:

* **SCLK** — SPI clock
* **MOSI** — SPI data to device
* **MISO** — SPI data from device
* **CS** — Chip select (active low)
* **INT1** / **INT2** — Interrupt outputs (connect to interrupt-capable GPIOs)

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl38x/src>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the ADXL38x driver, configures the
accelerometer for continuous measurement, and periodically reads
acceleration data for all three axes. Data is printed to the host
terminal via a UART serial connection.

In order to build the basic example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant basic --board nucleo-f767zi

Selftest Example
~~~~~~~~~~~~~~~~

The selftest example runs the built-in self-test routine of the ADXL38x
to verify that the device is functioning correctly. The result of the
self-test is reported over UART.

In order to build the selftest example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant selftest --board nucleo-f767zi

FIFO Example
~~~~~~~~~~~~

The FIFO example configures the ADXL38x to collect samples into its
internal FIFO buffer and then bursts the FIFO contents over UART. This
demonstrates low-power, batch-read operation of the accelerometer.

In order to build the FIFO example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant fifo --board nucleo-f767zi

No-OS Supported Platforms
-------------------------

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADXL38x <https://www.analog.com/en/products/adxl382.html>`_ (targets ADXL382)
* `NUCLEO-F767ZI <https://www.st.com/en/evaluation-tools/nucleo-f767zi.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADXL38x to the NUCLEO-F767ZI via SPI (SCLK, MOSI,
MISO, CS) and connect the INT1 pin to an interrupt-capable GPIO on the
Nucleo board.

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``, ``fifo``, ``selftest``.
Available boards: ``nucleo-f767zi``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

	# set the path to STM32CubeMX and STM32CubeIDE (only if they are not
	# in a default install location)
	export STM32CUBEMX=</path/to/stm32cubemx>
	export STM32CUBEIDE=</path/to/stm32cubeide>
	# Windows (PowerShell):
	#   $env:STM32CUBEMX = "C:\ST\STM32CubeMX"
	#   $env:STM32CUBEIDE = "C:\ST\STM32CubeIDE"

	cd no-OS

	# build the project (basic example on the NUCLEO-F767ZI board)
	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant basic --board nucleo-f767zi

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant basic --board nucleo-f767zi \
		--probe openocd --flash

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADXL38x <https://www.analog.com/en/products/adxl382.html>`_ (targets ADXL380)
* `MAX78000FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max78000fthr.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADXL38x to the MAX78000FTHR via SPI (SCLK, MOSI,
MISO, CS) and connect the INT1 pin to an interrupt-capable GPIO on the
MAX78000FTHR board.

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``, ``fifo``, ``selftest``.
Available boards: ``max78000fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

	# point at the Maxim SDK libraries (only if not auto-detected)
	export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
	# Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

	cd no-OS

	# build the project (basic example on the MAX78000FTHR board)
	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant basic --board max78000fthr

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project eval-adxl38x --variant basic --board max78000fthr \
		--probe openocd --flash
