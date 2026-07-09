AD9208 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD9208-DUAL-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad9208-dual-ebz.html>`_

Supported Carriers
------------------

.. list-table::
   :widths: 35 35 30
   :header-rows: 1

   * - Evaluation board
     - Carrier
     - FMC slot
   * - `AD9208-DUAL-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad9208-dual-ebz.html>`_
     - `VCU118 <https://www.xilinx.com/VCU118>`_
     - FMC+ (J22)

Overview
--------

The AD9208-DUAL-EBZ is an evaluation board designed to demonstrate the
synchronization of dual AD9208 analog-to-digital converters using the HMC7044
clock synthesizer. It provides integrated clocking for both the ADCs and the
associated FPGA, operating from a single external 12 V supply. The board
interfaces with the VCU118 FPGA evaluation kit through an FMC+ (VITA 57.4)
connector.

The AD9208 is a 3 GSPS, 14-bit ADC with on-chip DDC and NCO, designed for
wideband direct RF sampling. Leveraging Analog Devices JESD204B IP, the board
ensures efficient dual-ADC data transfer at high throughput, making it suitable
for high-speed data conversion, signal processing, and wideband communication
systems.

Applications
------------

* Diversity multiband, multimode digital receivers
* 3G/4G/5G communications
* Electronic test and measurement systems
* Phased array radar and electronic warfare
* DOCSIS 3.0 CMTS upstream receive paths
* HFC digital reverse path receivers

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9208-DUAL-EBZ requires a single external 12 V supply. The board uses
Silent Switcher technology for low SWaP, high-efficiency power delivery.
All required voltage rails (1.0 V, 1.8 V, 3.3 V) are generated on-board.

Board Connectors
~~~~~~~~~~~~~~~~

* **P2** — FMC+ (VITA 57.4) connector that mates with the VCU118 carrier board,
  carrying JESD204B serial links (SERDOUT0± to SERDOUT7±), SPI, GPIO, and power
* **J22** — FMC+ slot on VCU118 (the carrier-side connector)
* **J4 (VCU118)** — USB-to-UART bridge for serial console access
* **J10 (VCU118)** — Ethernet connector for network connectivity
* **J15 (VCU118)** — Power input connector for the evaluation board

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9208/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes dual AD9208 RF ADCs via JESD204 RX, AXI ADC core,
and AXI DMA, capturing data to DDR memory. Both ADCs are configured, the
JESD204B link is set up, and data capture status is verified.

IIO Example
~~~~~~~~~~~

The IIO example configures both AD9208 ADC devices, sets up the JESD204B
interfaces for high-speed data transfer, and initializes the AXI DMAC for data
movement to DDR memory. An IIOD server is launched so the user may connect via
an IIO client to read samples.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD9208-DUAL-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad9208-dual-ebz.html>`_
* `VCU118 <https://www.xilinx.com/VCU118>`_

Connections
^^^^^^^^^^^

Connect the AD9208-DUAL-EBZ to the VCU118 via the FMC+ HSPC connector (J22).
The JESD204B interface of the AD9208 eval board connects through serial links
(SERDOUT0± to SERDOUT7±) to the FPGA transceivers on the VCU118 for data
transmission. AD9208 configuration is handled via SPI; power management via
GPIO.

The VCU118 uses AXI buses to transfer data between the ADC outputs and the
FPGA processing elements. Connect a USB cable to J4 on the VCU118 for UART
access (115200 baud) to interact with the IIOD server.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``iio``.
Available boards: ``vcu118``.

The HDL design used is ``ad9208_dual_ebz``. The hardware name composed for the
XSA download is ``ad9208_dual_ebz_vcu118``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the demo example on vcu118
   python tools/scripts/no_os_build.py build \
      --project ad9208 --variant demo --board vcu118 \
      --hardware /path/to/ad9208_dual_ebz_vcu118/system_top.xsa

   # Build the IIO example on vcu118
   python tools/scripts/no_os_build.py build \
      --project ad9208 --variant iio --board vcu118 \
      --hardware /path/to/ad9208_dual_ebz_vcu118/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad9208 --variant iio --board vcu118 \
      --hardware /path/to/ad9208_dual_ebz_vcu118/system_top.xsa \
      --probe openocd --flash
