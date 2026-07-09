AD9172 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD917x <https://www.analog.com/eval-ad9172>`_

Overview
--------

The EVAL-AD917x evaluation board family (AD9171-FMC-EBZ, AD9172-FMC-EBZ,
AD9173-FMC-EBZ) are FMC form-factor boards compliant with the VITA 57.1
standard, designed for evaluating the AD917x family of high-performance,
dual, 16-bit RF DACs. The AD9172 supports DAC sample rates up to 12.6 GSPS
with an 8-lane, 15 Gbps JESD204B data input interface. It features three
complex, bypassable data input channels per RF DAC, each with configurable
gain, interpolation filters, and numerically controlled oscillators (NCOs) for
flexible multiband frequency planning.

To operate the evaluation board, attach it to a compatible FMC carrier board
such as the Xilinx ZC706 or ZCU102. A low-noise HMC7044 clock synthesizer on
the evaluation board provides all required clocking, including SYSREF for
JESD204B Subclass 1 operation. An optional external clock input SMA allows
supplying a low-jitter clock source.

Applications
------------

* Wireless communications infrastructure
* Instrumentation and automatic test equipment (ATE)
* Radars and jammers

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD917x board is primarily powered from the FMC carrier board via the
FMC connector (12 V and 3.3 V rails). On-board switching regulators and LDOs
generate 1.8 V (for the AD9172 analog and I/O domains) and 1.2 V (for the
AD9172 digital core). The HMC7044 and associated circuitry are also powered
from these on-board regulated rails. In standalone NCO-only mode, connect an
external +12 V supply to TP41 (red) and GND to TP42 (black).

Board Connectors
~~~~~~~~~~~~~~~~

* **J1** — SMA connector for DAC1 RF output
* **J2** — SMA connector for DAC0 RF output
* **J3** — SMA connector for CLKOUT
* **J34** — SMA connector for direct external DAC clock input
* **J41** — SMA connector for external HMC7044 PLL1 reference clock
* **P1** — FMC HPC connector (VITA 57.1), carries JESD204B data lanes, SPI,
  clocking, and power signals to/from the carrier board

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9172/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9172 dual RF DAC over SPI, configures the
JESD204B high-speed serial link between the FPGA and the DAC, and verifies
lock of the SERDES PLL, code group synchronization, and frame sync status.

DMA Example
~~~~~~~~~~~

The DMA example loads a pre-generated DAC data vector (such as a multi-tone
or sine wave LUT) from DDR memory and streams I/Q samples through the JESD204B
lanes to the AD9172 using the AXI DMAC controller. The expected RF waveform
can be observed on a spectrum analyzer connected to the DAC output SMA
connectors.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the user
may connect via an IIO client. Using IIO-Oscilloscope, users can access device
settings such as DAC interpolation rate, channel NCO frequency offset, output
scale, and JESD204B link parameters for real-time control of the AD9172 RF DAC
output signal.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD917x <https://www.analog.com/eval-ad9172>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_ or `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD917x FMC board to the carrier board FMC HPC connector
(J3 on ZC706, J5/HPC0 on ZCU102). The FMC connector routes all JESD204B
high-speed serial lanes, SPI interface, clock signals, and power rails between
the two boards. Connect a USB-to-serial cable to the carrier board UART
(115200 baud) for console output or IIO server access.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``dma_example``, ``iio``.
Available boards: ``zc706``, ``zcu102``.

The HDL design used is ``dac_fmc_ebz``. The hardware name composed for the XSA
download is ``dac_fmc_ebz_<board>`` (e.g. ``dac_fmc_ebz_zc706``).

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the demo example on zc706
   python tools/scripts/no_os_build.py build \
      --project ad9172 --variant demo --board zc706 \
      --hardware /path/to/dac_fmc_ebz_zc706/system_top.xsa

   # Build the DMA example on zcu102
   python tools/scripts/no_os_build.py build \
      --project ad9172 --variant dma_example --board zcu102 \
      --hardware /path/to/dac_fmc_ebz_zcu102/system_top.xsa

   # Build the IIO example on zc706
   python tools/scripts/no_os_build.py build \
      --project ad9172 --variant iio --board zc706 \
      --hardware /path/to/dac_fmc_ebz_zc706/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad9172 --variant demo --board zc706 \
      --hardware /path/to/dac_fmc_ebz_zc706/system_top.xsa \
      --probe openocd --flash
