AD9265-FMC-125EBZ no-OS Example Project
========================================

.. no-os-doxygen::

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
----------------------------

* `AD9265-FMC-125EBZ <https://www.analog.com/ad9265>`_

Overview
--------

The AD9265 is a 16-bit, 125 MSPS analog-to-digital converter (ADC) designed
to support communications applications where high performance combined with
low cost, small size, and versatility is desired. The ADC core features a
multistage, differential pipelined architecture with integrated output error
correction logic to provide 16-bit accuracy at 125 MSPS data rates and
guarantees no missing codes over the full operating temperature range. The
AD9265-FMC-125EBZ evaluation board provides all of the support circuitry
required to operate the AD9265 in its various modes and configurations,
with a built-in SPI interface for setup and control.

Applications
------------

* Communications
* Multimode digital receivers (3G)

  * GSM, EDGE, W-CDMA, LTE, CDMA2000, WiMAX, TD-SCDMA

* Smart antenna systems
* General-purpose software radios
* Broadband data applications
* Ultrasound equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9265-FMC-125EBZ evaluation board requires specific power supplies
for optimal performance. The core voltage supplies, AVDD and DVDD, should be
between 1.7 V and 1.9 V, with AVDD typically requiring 194 mA to 202 mA.
The digital output power, controlled by DRVDD, operates within the same
voltage range, requiring 24 mA for CMOS and 49 mA for LVDS output
configurations. Total power consumption ranges from 373 mW to 439 mW,
influenced by output mode and load conditions. Power-saving modes
include standby at 54 mW and power-down at 0.05 mW. Adhering to these
requirements ensures accurate ADC functionality.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Type                    | Description                       |
+-----------------------------------+-----------------------------------+
| SMA (J100)                        | Analog signal source connector,   |
|                                   | used for inputting analog signals |
|                                   | into the board.                   |
+-----------------------------------+-----------------------------------+
| USB                               | Interface for connecting to a     |
|                                   | computer or other USB-compatible  |
|                                   | devices.                          |
+-----------------------------------+-----------------------------------+
| Power Supply (12 V)               | Connector for supplying power to  |
|                                   | the board.                        |
+-----------------------------------+-----------------------------------+
| FMC LPC                           | Used to connect to FPGA carrier   |
|                                   | boards.                           |
+-----------------------------------+-----------------------------------+
| Clock Input (J201)                | External clock signal input for   |
|                                   | ADC sampling.                     |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9265-fmc-125ebz/src>`__

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9265 ADC via the AXI ADC core and
AXI DMAC, runs test pattern verification on the digital output lanes,
and performs a DMA capture of ADC samples into DDR memory.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can configure the AD9265 ADC and visualize captured
data from the 16-bit, 125 MSPS ADC.

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

* `AD9265-FMC-125EBZ <https://www.analog.com/ad9265>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_ evaluation kit

Connections
^^^^^^^^^^^

* Mount the AD9265-FMC-125EBZ to the ZC706 through the FMC LPC
  connector.
* Enable SD Card BOOT mode by switching SW11 to:

  ============ ========
  Switch       Position
  ============ ========
  SW11-1       DOWN
  SW11-2       DOWN
  SW11-3       UP
  SW11-4       UP
  SW11-5       DOWN
  ============ ========

* Insert the SD card into the SD card slot (J100). Ensure that the boot
  files are properly loaded in the card before use.
* Connect a micro-USB cable to the ZC706 board via the USB-UART
  connector (J1), and the other end of the cable to the host PC.
* Connect the power supply to the 12 V Power Input Connector (J22).
* Switch ON the ZC706 board via SW1; the green LED (DS22) will
  illuminate when power is ON.

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
      --project ad9265-fmc-125ebz --variant demo --board zc706 \
      --hardware /path/to/system_top.xsa

   # build and flash
   python tools/scripts/no_os_build.py build \
      --project ad9265-fmc-125ebz --variant demo --board zc706 \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

   # build the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9265-fmc-125ebz --variant iio --board zc706 \
      --hardware /path/to/system_top.xsa

   # build and flash the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9265-fmc-125ebz --variant iio --board zc706 \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
