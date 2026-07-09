ADRV9009 no-OS Example Project
================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `ADRV9009-W/PCBZ <https://www.analog.com/ADRV9009>`_

Overview
--------

The ADRV9009-W/PCBZ is an FMC radio module designed for evaluating the
ADRV9009, a highly integrated RF transceiver offering wideband transmit,
receive, and observation receiver functionality. The board interfaces
with FPGA carrier boards via the FMC connector and enables developers to
evaluate and prototype RF signal chains for a variety of applications.

The ADRV9009 features dual transmitters, dual receivers, and dual
observation receivers covering a frequency range of 75 MHz to 6 GHz.
It integrates a JESD204B interface for high-speed digital data transfer,
on-chip DPD (digital pre-distortion) support, and ARM Cortex-M3 based
firmware for device control. The device operates on multiple RF profiles
configurable through the Talise Evaluation Software (TES).

Applications
------------

* 3G, 4G, and 5G TDD macrocell base stations
* TDD active antenna systems
* Massive multiple input, multiple output (MIMO)
* Phased array radar
* Electronic warfare
* Military communications
* Portable test equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9009-W/PCBZ evaluation board receives power through the FMC
connector from the attached FPGA carrier board. On-board regulators
generate the required voltage domains for the ADRV9009 transceiver.

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv9009/src>`__.

For full system-level design setup, visit:
`ADRV9009/ADRV9008 No-OS System Level Design Setup <https://wiki.analog.com/resources/eval/user-guides/adrv9009/no-os-setup>`__

Demo Example
~~~~~~~~~~~~

The demo example (variant ``demo``) is the default build which initializes
the ADRV9009 device, configures the clocking, JESD204B links, and RF
signal paths. It performs device calibrations and verifies link status.

ADRV9008-1 Example
~~~~~~~~~~~~~~~~~~

The ADRV9008-1 example (variant ``adrv9008-1``) targets the ADRV9008-1,
the TX-only sub-variant of the ADRV9009 family. This variant activates
the TX-only code paths in ``headless_arm.c``.

ADRV9008-2 Example
~~~~~~~~~~~~~~~~~~

The ADRV9008-2 example (variant ``adrv9008-2``) targets the ADRV9008-2,
the RX-only sub-variant of the ADRV9009 family. This variant activates
the RX-only code paths in ``headless_arm.c``.

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) demonstrates direct memory
access data transfer for the ADRV9009. It initializes the JESD204B
links, DMA controllers, and data converter cores, then exercises the
ADC/DAC data paths for test and verification.

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio``) is an IIOD demo for the ADRV9009
evaluation board. The project launches the IIOD server on the board so
that the user may connect to it via an IIO client application. Using
IIO-Oscilloscope, the user can configure and stream data from the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ADRV9009-W/PCBZ <https://www.analog.com/ADRV9009>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_ (Zynq-7000)
* `ZCU102 <https://www.xilinx.com/ZCU102>`_ (Zynq UltraScale+ MPSoC)

Connections
^^^^^^^^^^^

Connect the ADRV9009-W/PCBZ to the ZC706 or ZCU102 using the FMC HPC
connector. The JESD204B serial lanes interface with the FPGA
transceivers via the FMC connector. The ADRV9009 is configured over
SPI from the FPGA. Connect a USB cable to the carrier board USB-UART
port and the host PC for serial console access at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``,
``dma_example``, ``iio``, ``adrv9008-1``, ``adrv9008-2``.
Available boards: ``zc706``, ``zcu102``.

A Xilinx XSA hardware description file is required. The HDL design name
is ``adrv9009``; the hardware name is composed as ``adrv9009_<board>``
(e.g. ``adrv9009_zcu102``).

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # Build the demo example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv9009 --variant demo --board zcu102 \
       --hardware /path/to/adrv9009_zcu102/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adrv9009 --variant demo --board zcu102 \
       --hardware /path/to/adrv9009_zcu102/system_top.xsa \
       --probe openocd --flash

   # Build the demo example for ZC706
   python tools/scripts/no_os_build.py build \
       --project adrv9009 --variant demo --board zc706 \
       --hardware /path/to/adrv9009_zc706/system_top.xsa

   # Build the DMA example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv9009 --variant dma_example --board zcu102 \
       --hardware /path/to/adrv9009_zcu102/system_top.xsa

   # Build the IIO example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv9009 --variant iio --board zcu102 \
       --hardware /path/to/adrv9009_zcu102/system_top.xsa
