AD9467 no-OS Example Project
============================

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
----------------------------

* `AD9467-FMC-250EBZ <https://www.analog.com/AD9467>`_

Overview
--------

The AD9467 evaluation board (AD9467-FMC-250EBZ) is engineered to
facilitate the comprehensive evaluation of the AD9467 analog-to-digital
converter. It offers high-performance data conversion capabilities by
providing leading signal-to-noise ratio (SNR) and spurious-free dynamic
range (SFDR) within a compact design. Notable features include
differential analog inputs optimized for broad bandwidth down
conversion, configurable clock inputs for both single-ended and
differential signals, and a flexible power supply setup. The AD9517
clock distribution IC provides a programmable reference clock to the
ADC. This board is ideally suited for demanding applications such as
instrumentation, spectrum analysis, and radar systems requiring precise
and rapid data acquisition.

Applications
------------

* Multicarrier, multimode cellular receivers
* Power amplifier linearization
* Broadband wireless
* Radar
* Infrared imaging
* Communications instrumentation
* Antenna array positioning

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

To power the AD9467 evaluation board with an external power supply,
connect a switching power supply rated for 100 V to 240 V AC, configured
to output 6 V at up to 2.5 A, directly to the power input jack labeled
P700. Ensure all jumpers, including those for reference settings and
clock input configurations, are correctly placed. Consider using
additional components like COAX cables or terminators if deviating from
standard configurations.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9467/src>`__

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9467 ADC and AD9517 clock distribution
IC via SPI, configures the AXI ADC core and AXI DMAC, runs test pattern
verification (including various test modes via ``ad9467_test_mode``), and
captures ADC samples into DDR memory via DMA.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can configure the AD9467 ADC operational modes, output
current, clock delay, and other parameters, and perform real-time data
capture over UART.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD9467-FMC-250EBZ <https://www.analog.com/AD9467>`_
* `ZedBoard <https://digilent.com/shop/zedboard-zynq-7000-arm-fpga-soc-development-board/>`_ (Zynq-7000)

Connections
^^^^^^^^^^^

1. Connect the AD9467-FMC-250EBZ board to the ZedBoard using the FMC
   LPC interface connector.

2. Use a USB Type-A to mini-B cable to connect the ZedBoard to the host
   PC via the USB-UART port (J2).

3. Attach the power adapter to the ZedBoard (via J3, 12 V input) and
   power on the board via SW8.

4. Set the adjustable voltage (VADJ) on the ZedBoard to supply 2.5 V for
   the AD9467 board (consult the ZedBoard hardware guide for the jumper
   settings).

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment (adjust version as needed)
   source ~/.xilinx/2025.1/Vitis/settings64.sh

   cd no-OS

   # build the demo example
   python tools/scripts/no_os_build.py build \
      --project ad9467 --variant demo --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash
   python tools/scripts/no_os_build.py build \
      --project ad9467 --variant demo --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

   # build the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9467 --variant iio --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9467 --variant iio --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
