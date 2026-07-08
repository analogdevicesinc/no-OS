EVAL-ADXL367Z no-OS Example Project
=====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADXL367Z <https://www.analog.com/EVAL-ADXL367Z>`_

Overview
--------

The `ADXL367 <https://www.analog.com/ADXL367>`_ is an ultralow power, 3-axis
MEMS accelerometer that consumes only 0.89 uA at a 100 Hz output data rate
and 180 nA in motion-triggered wake-up mode. Unlike accelerometers that use
power duty cycling to achieve low power consumption, the ADXL367 does not
alias input signals by undersampling, but samples the full bandwidth of the
sensor at all data rates.

The device provides 14-bit output resolution with selectable measurement
ranges of +/-2 g, +/-4 g, and +/-8 g (0.25 mg/LSB on the +/-2 g range).
It features a deep 512-sample FIFO, built-in temperature sensor, single-tap
and double-tap detection, and supports both SPI (4-wire) and I2C digital
interfaces. The ADXL367 operates on a 1.1 V to 3.6 V supply range.

The EVAL-ADXL367Z evaluation board provides a simple platform for evaluating
the ADXL367 accelerometer.

Applications
------------

* 24/7 sensing
* Hearing aids
* Vital signs monitoring devices
* Motion-enabled power save switches
* Motion-enabled metering devices
* Smart watch with single-cell operation
* Smart home

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADXL367 operates on a supply voltage range of 1.1 V to 3.6 V (VDD),
with a separate digital interface supply (VDDIO). Current consumption is
0.89 uA at 100 Hz ODR (2.0 V supply), 180 nA in motion-activated wake-up
mode, and 40 nA in standby.

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl367z/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl367z/src/platform>`__.

Dummy Example
~~~~~~~~~~~~~

The dummy example initializes the ADXL367 accelerometer, performs a
self-test, enables temperature reading, sets the output data rate to
200 Hz, and configures the FIFO for data collection. The device is then
switched to measurement mode, and acceleration data along the x, y, and
z axes along with temperature data is read from the FIFO buffer and
printed to the console.

This example is built by selecting the ``dummy`` variant (see the Build Command
sections below).

IIO Example
~~~~~~~~~~~

The IIO example initializes the ADXL367 accelerometer and sets up an
IIO device server that allows users to interact with the sensor via an
IIO client such as IIO Oscilloscope.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

This example is built by selecting the ``iio_example`` variant (see the Build
Command sections below).

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADXL367Z <https://www.analog.com/EVAL-ADXL367Z>`_
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADXL367Z to the ZedBoard via the SPI interface. The
ADXL367 communicates over SPI using device ID 0 as configured in
``src/platform/xilinx/parameters.h``. The UART console appears on the
ZedBoard USB-UART adapter at **115200 baud, 8N1**.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``dummy``,
``iio_example``. Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "<path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # build the dummy example on the ZedBoard (requires a .xsa hardware file)
   python tools/scripts/no_os_build.py build \
      --project eval-adxl367z --variant dummy --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-adxl367z --variant dummy --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
