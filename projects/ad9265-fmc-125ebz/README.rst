AD9265-FMC-125EBZ no-OS Example Project
=======================================

.. no-os-doxygen::

.. contents::
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
-------------

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
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9265-fmc-125ebz/src>`_

The macros used in Common Data are defined in platform specific files
found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9265-fmc-125ebz/src/platform>`_

Basic Example
~~~~~~~~~~~~~

The basic example demonstrates initialization and data acquisition
of the AD9265 ADC, configured via SPI interface, ADC core, and AXI DMAC
for data transfers. It operates the ADC in test mode and manages DMA
transfers.

IIO Example
~~~~~~~~~~~

The IIO example demonstrates basic ADC initialization and data
acquisition using the IIO framework within the no-OS environment.
Through the IIO Oscilloscope application, users can visualize
captured data from the AD9265 ADC.

If you are not familiar with the ADI IIO framework, please take a look
at: `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with the ADI IIO Oscilloscope app, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

To build the IIO example, add the following flag when invoking make:

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `AD9265-FMC-125EBZ <https://www.analog.com/ad9265>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_

**Connections**

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
* Switch ON the ZC706 board via SW1, the green LED (DS22) will
  illuminate when power is ON.

**Build Command**

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
