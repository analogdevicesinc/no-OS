AD9434-FMC-500EBZ no-OS Example Project
========================================

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `AD9434-FMC-500EBZ <https://www.analog.com/AD9434>`_

Overview
--------

The AD9434-FMC-500EBZ evaluation board is a versatile platform designed
for testing the AD9434 analog-to-digital converter (ADC). It features
high-speed digital data output through an FPGA interface for efficient
data capture and analysis. The board supports various input signal
conditions and offers flexible clocking and data-capture options,
accommodating diverse testing scenarios. It integrates seamlessly with
existing development platforms and includes signal integrity assessment
tools, power management configurations, and options for internal or
external clock sources, making it well-suited for high-speed signal
acquisition design and development.

Applications
------------

* Wireless and wired broadband communications
* Cable reverse path
* Communications test equipment
* Radar and satellite subsystems
* Power amplifier linearization

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9434-FMC-500EBZ evaluation board requires a 12 V power supply to
operate efficiently. This voltage level is essential to support all the
onboard components, including the AD9434 ADC and any supplementary devices
integrated into the system. Users should ensure that the power supply is
stable and capable of delivering the necessary current to maintain optimal
performance during data capture and processing tasks. Proper cabling and
connections are critical to prevent power loss and to maintain the
system's reliability during testing and evaluation processes.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Type                    | Description                       |
+-----------------------------------+-----------------------------------+
| FPGA Connector (J1)               | High speed data output connector  |
|                                   | used to interface with an FPGA    |
|                                   | for data capturing.               |
+-----------------------------------+-----------------------------------+
| SMA Connector (J2)                | Clock input connector for         |
|                                   | external clock source or          |
|                                   | oscillator connection.            |
+-----------------------------------+-----------------------------------+
| P1 Header                         | Used for configuring various      |
|                                   | signal conditions and test        |
|                                   | points.                           |
+-----------------------------------+-----------------------------------+
| USB Connector (J6)                | Power and communication interface |
|                                   | with the computer or test setup.  |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9434-fmc-500ebz/src>`__

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9434 ADC via the AXI ADC core and
AXI DMAC, runs test pattern verification on the digital output lanes,
and captures data to DDR memory.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that
the user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can access device settings and capture data from the
AD9434 high-speed ADC via the IIO framework.

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

* `AD9434-FMC-500EBZ <https://www.analog.com/AD9434>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_ evaluation kit

Connections
^^^^^^^^^^^

1. Locate the FMC HPC connector labeled J37 on the ZC706 board.

2. Align the FMC edge connector of the AD9434-FMC-500EBZ with the J37
   connector on the ZC706. The connectors are keyed to prevent improper
   orientation; the components on the FMC board should face away from
   the ZC706. Gently but firmly press the AD9434-FMC-500EBZ into the J37
   connector until it is fully seated.

3. Ensure the ZC706 is powered off before connecting or disconnecting
   the FMC card.

4. The ZC706 provides VADJ to the J37 FMC connector as required for
   FMC-compliant boards.

5. The AD9434-FMC-500EBZ will be automatically added to the JTAG chain
   when installed, due to electronically controlled switches on the
   ZC706.

6. Connect a USB cable to the ZC706 USB-UART connector and then to the
   host PC for serial console access at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zc706``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment (adjust version as needed)
   source ~/.xilinx/2025.1/Vitis/settings64.sh

   cd no-OS

   # build the demo example
   python tools/scripts/no_os_build.py build \
      --project ad9434-fmc-500ebz --variant demo --board zc706 \
      --hardware /path/to/system_top.xsa

   # build and flash
   python tools/scripts/no_os_build.py build \
      --project ad9434-fmc-500ebz --variant demo --board zc706 \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

   # build the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9434-fmc-500ebz --variant iio --board zc706 \
      --hardware /path/to/system_top.xsa

   # build and flash the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9434-fmc-500ebz --variant iio --board zc706 \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
