EVAL-AD5933ARDZ no-OS Example Project
=====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD5933ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad5933ardz.html>`_

Overview
--------

The **EVAL-AD5933ARDZ** is an Arduino-compatible evaluation board for the
:adi:`AD5933`, a high precision impedance converter system that combines an
on-board frequency generator with a 12-bit, 1 MSPS ADC and an on-chip DFT
engine. The frequency generator excites an external unknown impedance, and the
DFT engine returns a real and an imaginary data word for each measured
frequency point, from which the host computes magnitude, phase, and impedance.

The AD5933 is controlled over a 400 kHz I2C interface at slave address ``0x0D``.
The excitation frequency is swept from a programmable start frequency in a
programmable increment over up to 511 increments (512 points total), with a
software-selectable output range, PGA gain, and settling-time count. These
examples target the **SDP-CK1Z** controller board, which carries an STM32
microcontroller and mates with the EVAL-AD5933ARDZ through the SDP connector.

Applications
------------

* Electrochemical analysis
* Bioelectrical impedance analysis
* Complex impedance measurement
* Corrosion monitoring and protection equipment
* Biomedical and automotive sensors
* Proximity sensing
* Nondestructive testing and material property analysis

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD5933ARDZ is powered through the controller board over the SDP
connector; no external supply is required for the default configuration. The
UART console is available over the controller board's USB connection.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow. Selecting a
variant at build time (``--variant <name>``) chooses which application is
compiled. The platform ``main()`` is a thin dispatcher that calls
``example_main()``, provided by the selected example. Shared initialization data
is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad5933ardz/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad5933ardz/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the AD5933, reads back the on-chip temperature,
configures a frequency sweep, and then walks the sweep point by point. For each
point it computes a gain factor from the calibration resistor and uses it to
convert the raw real/imaginary DFT words into an impedance value, which is
printed over UART.

The sweep parameters (start frequency, frequency increment, and number of
increments) and the calibration resistor value are set as macros at the top of
`basic_example.c <src/examples/basic/basic_example.c>`__. The device-level
configuration (clock source and frequency, PGA gain, output range, and settling
cycles) lives in the ``ad5933_user_init`` structure in
`common_data.c <src/common/common_data.c>`__.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the board so that any libiio client
can connect, configure the AD5933, run a frequency sweep, and read back the
collected real/imaginary data. The sweep parameters used to seed the device at
start-up (start frequency, frequency increment, number of increments, and the
calibration impedance) are set at the top of
`iio_example.c <src/examples/iio_example/iio_example.c>`__; the device-level
configuration is shared with the basic example through ``ad5933_user_init`` in
`common_data.c <src/common/common_data.c>`__.

The whole flow uses standard libiio operations — there is no new protocol:

1. **Configure the sweep parameters** by writing the device attributes
   ``out_altvoltage0_frequency_start``, ``out_altvoltage0_frequency_increment``,
   ``out_altvoltage0_frequency_points`` (the number of increments; points =
   ``out_altvoltage0_frequency_points`` + 1) and
   ``out_altvoltage0_settling_cycles``. The excitation output range
   (``out_altvoltage0_raw``) and the input PGA gain (``in_voltage0_scale``) can
   be adjusted the same way.
2. **Enable the scan channels and open a buffer.** The ``real`` (``voltage0``)
   and ``imag`` (``voltage1``) channels are scannable, signed 16-bit. Enabling
   the buffer initializes and starts the sweep automatically; the driver then
   walks every point, waiting for each to become valid and collecting its
   real/imaginary words.
3. **Read the buffer** to retrieve the collected points. Each point is delivered
   as two interleaved words — real then imaginary — for up to ``num_increments +
   1`` points. Requested samples beyond the number of points collected are
   padded with ``INT16_MAX``. Disabling the buffer places the device in
   power-down mode. The host performs any magnitude / phase / impedance math
   from the raw data.

Individual points can also be driven manually through the debug attributes
(``sweep_initialized``, ``sweep_started``, ``repeat_measurement``,
``incremented_measurement`` and the read-only ``current_output_frequency``); see
the driver documentation for details.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
-------------------------

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD5933ARDZ <https://www.analog.com/EVAL-AD5933ARDZ>`_
* SDP-CK1Z controller board (STM32)

Connections
^^^^^^^^^^^

The EVAL-AD5933ARDZ connects to the SDP-CK1Z through the SDP connector. The
AD5933 is accessed over I2C, and the example console is emitted over the
controller board's UART:

.. list-table::
   :header-rows: 1

   * - Function
     - STM32 Peripheral
     - Notes
   * - I2C (SCL/SDA)
     - I2C1
     - 400 kHz, AD5933 slave address 0x0D
   * - UART (debug console)
     - UART5
     - 115200 baud, 8N1

Build Command
^^^^^^^^^^^^^

The STM32 platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``basic``, ``iio_example``. Available boards:
``sdp-ck1z``.

For toolchain setup and prerequisites, see the
`STM32 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_stm32_cmake.html>`__.

.. code-block:: bash

	# point at the STM32 toolchain installations
	export STM32CUBEMX=</path/to/stm32cubemx>
	export STM32CUBEIDE=</path/to/stm32cubeide>
	# Windows (PowerShell) equivalent:
	#   $env:STM32CUBEMX = "C:\ST\STM32CubeMX"
	#   $env:STM32CUBEIDE = "C:\ST\STM32CubeIDE"

	cd no-OS

	# build the IIO example on the SDP-CK1Z board
	python tools/scripts/no_os_build.py build \
		--project eval-ad5933ardz --variant iio_example --board sdp-ck1z

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project eval-ad5933ardz --variant iio_example --board sdp-ck1z \
		--probe openocd --flash
