AD9265-FMC-125EBZ no-OS Example Project
=======================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD9265-FMC-125EBZ`

Overview
--------

The AD9265-FMC-125EBZ evaluation board is designed for evaluating the
AD9265 ADC, offering a comprehensive setup for performance testing. It
includes support circuitry enabling the ADC to operate in various
configurations and modes, with a built-in SPI interface for easy setup
and control. The board supports external oscillators like the AD9517 and
offers multiple power supply options. Integration with VisualAnalog
software and SPI controller interfaces ensures effective data capture,
making it suitable for users testing or developing with the AD9265 ADC.
It also supports optional IIO features for enhanced functionality.

Applications
------------

- Communications
- Multimedia digital receivers (3G)
  - GSM, EDGE, W-CDMA, LTE, CDMA2000, WiMax, TD-SCDMA
- Smart antenna systems
- General-purpose software-defined radios
- Broadband data applications
- Ultrasound equipment

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9265-FMC-125EBZ Evaluation Board requires specific power supplies 
for optimal performance. The core voltage supplies, AVDD and DVDD, should be 
between 1.7V and 1.9V, with AVDD typically needing 194mA to 202mA. 
The digital output power, controlled by DRVDD, operates within the same 
voltage range, requiring 24mA for CMOS and 49mA for LVDS output
configurations. Total power consumption ranges from 373mW to 439mW,
influenced by output mode and load conditions. Power-saving modes
include standby at 54mW and power-down at 0.05mW. Adhering to these
requirements ensures accurate ADC functionality.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Type                    | Description                       |
+-----------------------------------+-----------------------------------+
| SMA                               | Analog signal source connector,   |
|                                   | used for inputting analog signals |
|                                   | into the board.                   |
+-----------------------------------+-----------------------------------+
| USB                               | Interface for connecting to a     |
|                                   | computer or other USB-compatible  |
|                                   | devices.                          |
+-----------------------------------+-----------------------------------+
| Power Supply (12V)                | Connector for supplying power to  |
|                                   | the board.                        |
+-----------------------------------+-----------------------------------+
| FMC Connector                     | Used to connect to other FPGA     |
|                                   | boards                            |
+-----------------------------------+-----------------------------------+
| JTAG                              | Debugging connector used for      |
|                                   | interfacing with JTAG tools.      |
+-----------------------------------+-----------------------------------+
| SPI Interface                     | Used for setting up and           |
|                                   | controlling the ADC operation     |
|                                   | through serial communication.     |
+-----------------------------------+-----------------------------------+
| Clock Input                       | For external clock signal input,  |
|                                   | necessary for ADC sampling.       |
+-----------------------------------+-----------------------------------+
| Data Output Connectors            | Connectors for digital data       |
|                                   | output from the ADC.              |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The AD9265-FMC-125EBZ no-OS example project focuses on demonstrating the
initialization and data acquisition capabilities of the AD9265 ADC,
configured via SPI interface, ADC core, and AXI DMAC for data transfers.
It operates the ADC in test mode, manages DMA transfers, and configures
cache settings on Xilinx platforms.

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9265-fmc-125ebz/src>`__

IIO example
~~~~~~~~~~~

The IIO demo is a standard example designed to demonstrate basic ADC
initialization and data acquisition using the IIO framework within the
no-OS environment. Through the IIO Oscilloscope application, users can
visualize capture and processing of data from the AD9265 ADC with
minimal configuration.

If you are not familiar with the ADI IIO framework, please take a look
at: `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with the ADI IIO Oscilloscope app, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add this flag to the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9265-fmc-125ebz/Makefile>`__
when invoking make. This will build the IIOD server and the IIO section of the driver.

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- AD9265-FMC-125EBZ (AD9265 Evaluation Board)
- AMD Zynq™ 7000 SoC ZC706 Evaluation Kit

Requirements
^^^^^^^^^^^^

**Software Configuration**

The SD card must have the ``BOOT.bin``, ``Image``, and ``system.dtb``
files for booting the Zynq ZC706 board. Full instructions on how to
build the BOOT files are available in this page: 
`Build the boot image <https://analogdevicesinc.github.io/hdl/user_guide/build_boot_bin.html>`__

**Hardware Setup**

To connect the AD9265 Evaluation Board to the ZC706 Evaluation Board,
follow these steps:

- Mount the AD9265-FMC-125EBZ to the ZC706 through the FMC LPC
  Connector.

- Enable SD Card BOOT mode by switching SW11 to:

    ============ ========
    Switch       Position
    SW11-1       DOWN
    SW11-2       DOWN
    SW11-3       UP
    SW11-4       UP
    SW11-5       DOWN
    ============ ========

- Insert the SD card into the SD card slot (J100). Ensure that the boot
  files are properly loaded in the card before use.

- Connect a micro-USB cable to the ZCU102 board via the USB-UART
  connector (J1), and the other end of the cable to the Host PC.

- Connect the power supply to the 12V Power Input Connector (J22).

- Switch ON the ZC706 board via SW1, the green LED (DS22) will
  illuminate when power is ON.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
