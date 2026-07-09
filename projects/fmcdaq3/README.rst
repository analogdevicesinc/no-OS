FMCDAQ3 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `AD-FMCDAQ3-EBZ <https://www.analog.com/AD-FMCDAQ3-EBZ>`_

Overview
--------

The AD-FMCDAQ3-EBZ evaluation board is a high-speed data acquisition
system designed for interfacing with FPGA boards like the Xilinx ZCU102.
Equipped with an AD9680 dual ADC and AD9152 DAC, it utilizes the JESD204B/C
transceiver protocol for data transfer. The board supports multiple
connectivity options, including SD card slots, USB, and HDMI, allowing
for flexible integration in various testing setups. It can handle
digital signals at lane rates up to 12.33 Gbps and features independent
data paths for transmission and reception. The AD9528 on-board clock
generator provides precise synchronization. Configuration is managed
through software interfaces compatible with no-OS and Linux platforms,
making it adaptable to different project demands.

Applications
------------

* Electronic test and measurement equipment
* General-purpose software radios
* Radar systems
* Ultra-wideband satellite receivers
* Signals intelligence (SIGINT)
* Point to point communication systems
* DOCSIS 3.0 CMTS and HFC networks
* Multiple input/multiple output (MIMO) radios
* Automated test equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCDAQ3-EBZ evaluation board requires a 12V power input,
delivered through its power connector, to operate its integrated
components. This power supply supports crucial board functions,
including the AD9680 ADC, AD9152 DAC, and JESD204B/C interface
operations, as well as precise clock generation via the AD9528 clock
generator. Additionally, the board's GPIOs manage power states across
various sections to enable or disable peripheral features, maintaining
energy efficiency and stability during operation.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector Name        | Component             | Function/Use          |
+-----------------------+-----------------------+-----------------------+
| SD Card Slot (J100)   | SD Card Interface     | Inserting SD card for |
|                       |                       | data storage          |
+-----------------------+-----------------------+-----------------------+
| AD-FMCDAQ3-EBZ FMC    | FMC Connector         | Connects to the FPGA  |
| Board                 |                       | carrier HPC FMC       |
|                       |                       | socket                |
+-----------------------+-----------------------+-----------------------+
| Display Port Video    | Video Output          | Connecting a Display  |
| Connector (J11)       |                       | Port monitor          |
+-----------------------+-----------------------+-----------------------+
| USB 2.0 UPLT          | USB Input             | Connecting USB        |
| Controller (J83)      |                       | mouse/keyboard        |
+-----------------------+-----------------------+-----------------------+
| 12V Power Input       | Power Supply          | Connecting the main   |
| Connector (J52)       |                       | power supply          |
+-----------------------+-----------------------+-----------------------+

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src/app <https://github.com/analogdevicesinc/no-OS/tree/main/projects/fmcdaq3/src/app>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9680 ADC, AD9152 DAC, and AD9528 clock
generator, configures the JESD204B links, and verifies the link status. The
AXI ADC/DAC cores and AXI DMA are initialized for data capture and generation.
Pattern tests and a DMA capture are performed.

This example is built by selecting the ``demo`` variant (see the Build Command
section below).

IIO Example
~~~~~~~~~~~

The IIO example integrates with the Industrial I/O (IIO) framework to
facilitate standardized data acquisition and device configuration. It sets up
the necessary SPI and GPIO interfaces, configures the AD9528 for clock
distribution, and manages data transfers using DMA controllers. The setup
includes initializing IIO application descriptors and UART communication,
enabling real-time signal processing and interaction with IIO clients. It
supports ADC-to-memory data transfers and PRBS tests for data integrity.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

This example is built by selecting the ``iio`` variant (see the Build Command
section below).

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD-FMCDAQ3-EBZ <https://www.analog.com/AD-FMCDAQ3-EBZ>`_
* `AMD Zynq UltraScale+ MPSoC ZCU102 Evaluation Kit <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/ek-u1-zcu102-g.html>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_

Connections
^^^^^^^^^^^

Connect the AD-FMCDAQ3-EBZ to the FMC HPC0 socket on the carrier board.
The JESD204B serial lanes interface with the FPGA transceivers via the FMC
connector. The AD9680, AD9152, and AD9528 are configured over SPI from the
FPGA. Connect a micro-USB cable to the carrier board's USB-UART connector for
the serial console. The UART console appears at **115200 baud, 8N1**.

For the ZCU102, connect the power supply to the 12V Power Input Connector
(J52) and enable SD Card boot mode by switching SW6 to:

====== =====
Switch State
====== =====
SW6-1  ON
SW6-2  OFF
SW6-3  OFF
SW6-4  OFF
====== =====

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zc706``, ``zcu102``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "<path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # build the iio example on the ZCU102 board (requires a .xsa hardware file)
   python tools/scripts/no_os_build.py build \
      --project fmcdaq3 --variant iio --board zcu102 \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project fmcdaq3 --variant iio --board zcu102 \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
