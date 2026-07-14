AD74413R no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD74413R <https://www.analog.com/EVAL-AD74413R>`_

Overview
--------

The AD74413R is a quad-channel, software configurable input/output solution
for building and process control applications. It contains functionality for
analog output, analog input, digital input, resistance temperature detector
(RTD), and thermocouple measurements integrated into a single chip with a
serial peripheral interface (SPI).

The device features a 16-bit, sigma-delta ADC and four configurable,
13-bit DACs to provide four configurable input/output channels and a suite
of diagnostic functions. Supported modes include voltage output, current
output, voltage input, externally powered current input, loop powered
current input, external RTD measurement, digital input logic, and loop
powered digital input.

The EVAL-AD74413R evaluation board integrates the AD74413R alongside an
on-board 2.5 V ADR525 reference. It interfaces with a host processor via
SPI and supports an AVDD operating supply of 14 V to 28.8 V.

Applications
------------

* Process control
* Factory automation
* Motor drives
* Building control systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD74413R requires an AVDD operating supply of 14 V to 28.8 V.
AVDD powers the ADP2360 switching regulator when JP1 is inserted, providing
a regulated 5 V supply via JP3. The DVCC supply is derived from the ADP1720
when JP6 is in place. The SDP-S board, connected through JP10, supplies 3.3 V
for communication.

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad74413r/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad74413r/src/platform>`__.

Dummy example
~~~~~~~~~~~~~

This is the default example which initializes the AD74413R device, configures
ADC channels for digital input, voltage input/output, and current input,
and continuously reads channel statuses for functionality validation.

This example is built by selecting the ``dummy`` variant (see the Build
Command section below).

IIO example
~~~~~~~~~~~

This project is an IIOD demo for the EVAL-AD74413R evaluation board. The
project launches the IIOD server on the board so that the user may connect
to it via an IIO client application. Using IIO-Oscilloscope, the user can
configure device channels and read samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

This example is built by selecting the ``iio`` variant (see the Build
Command section below).

IIO trigger example
~~~~~~~~~~~~~~~~~~~

This example initializes the AD74413R with configured channels, sets up
hardware triggers using GPIO interrupts, and integrates them into the IIO
subsystem for synchronized data capture.

This example is built by selecting the ``iio_trigger`` variant (see the Build
Command section below).

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD74413R <https://www.analog.com/EVAL-AD74413R>`_
* MAX32655 or MAX78000

Connections
^^^^^^^^^^^

Connect the EVAL-AD74413R to the Maxim platform via SPI (SCLK, MISO,
MOSI, CS). The ALERT pin should be connected to an interrupt-capable GPIO
for trigger-based examples.

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``dummy``, ``iio``.
Available boards: ``max32655fthr``, ``max78000fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

	# point at the Maxim SDK libraries (only if not auto-detected)
	export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
	# Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

	cd no-OS

	# build the project (dummy example on the MAX78000FTHR board)
	python tools/scripts/no_os_build.py build \
		--project ad74413r --variant dummy --board max78000fthr

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project ad74413r --variant dummy --board max78000fthr \
		--probe openocd --flash

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD74413R <https://www.analog.com/EVAL-AD74413R>`_
* `NUCLEO-F756ZG <https://www.st.com/en/evaluation-tools/nucleo-f756zg.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD74413R to the STM32 platform via SPI (SCLK, MISO,
MOSI, CS). The ALERT pin should be connected to an interrupt-capable GPIO
for trigger-based examples.

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
`STM32 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_stm32_cmake.html>`__.

Available variants: ``dummy``, ``iio``, ``iio_trigger``.
Available boards: ``nucleo-f756zg``.
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

	# build the project (dummy example on the NUCLEO-F756ZG board)
	python tools/scripts/no_os_build.py build \
		--project ad74413r --variant dummy --board nucleo-f756zg

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project ad74413r --variant dummy --board nucleo-f756zg \
		--probe openocd --flash
