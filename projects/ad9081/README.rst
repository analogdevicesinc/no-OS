AD9081 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD9081-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9081.html>`_
* `QUAD-MXFE <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/quad-mxfe.html>`_

Supported Carriers
------------------

.. list-table::
   :widths: 35 35 30
   :header-rows: 1

   * - Evaluation board
     - Carrier
     - FMC slot
   * - `AD9081-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9081.html>`_
     - `VCU118 <https://www.xilinx.com/VCU118>`_
     - FMC+
   * -
     - `ZCU102 <https://www.xilinx.com/ZCU102>`_
     - FMC HPC0
   * - `QUAD-MXFE <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/quad-mxfe.html>`_
     - `VCU118 <https://www.xilinx.com/VCU118>`_
     - FMC+

Overview
--------

The AD9081 is a highly integrated, multi-channel mixed-signal front-end (MxFE)
device designed for a complete radio solution across a wide range of
applications including 5G wireless infrastructure, broadband communication, and
defense electronics. It combines multiple high-speed ADCs and DACs, a digital
up-converter (DUC) and digital down-converter (DDC), and integrated DSP blocks.
The device communicates with an FPGA over a high-speed JESD204B/C serial
interface.

The Quad-MxFE System Development Platform contains four MxFE software-defined,
direct RF sampling transceivers with associated RF front-ends, clocking, and
power circuitry. The target application is phased array radars, electronic
warfare, and ground-based SATCOM, specifically a 16Tx/16Rx channel direct
sampling phased array at L/S/C band (0.1 GHz to approximately 5 GHz).

Applications
------------

* 5G wireless infrastructure
* Broadband communication
* Defense electronics and phased array radar
* Electronic warfare
* Ground-based SATCOM

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9081-FMCA-EBZ is powered from the FMC carrier board. The carrier supplies
12 V and 3.3 V through the FMC connector; on-board regulators generate all
internal supply rails. For the Quad-MxFE board, an external 12 V laboratory
supply is required.

Reference Input Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For the AD9081-FMCA-EBZ, the board can use an external clock source or the
internal HMC7044 clock chip.

.. note::

   The following rework may be required on older board revisions:

   * To avoid using an external clock source and rely on the HMC7044, rotate
     the C6D/C4D capacitors to position C5D/C3D. (On the latest board revision
     this is the default configuration and may not be needed.)
   * If LEDs V1P0_LED and VINT_LED are not lit, depopulate R22M and populate R2M.

For the Quad-MxFE, an additional 500 MHz reference oscillator or waveform
generator is required.

Board Connectors
~~~~~~~~~~~~~~~~

* **J1** — FMC+ connector that mates with the carrier board (VCU118 or ZCU102)
* **J10** — SMA connector for optional external reference clock input

.. note::

   The QUAD-MXFE project is configured for rev C boards and later. For older
   revisions, refer to the corresponding older no-OS release.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9081/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9081 MxFE device via JESD204 RX/TX links,
AXI ADC and DAC cores, and AXI DMAC, then captures and plays back data to
DDR memory. This is the default example using the ``ad9081_fmca_ebz`` HDL
design (VCU118 or ZCU102 carrier).

IIO Example
~~~~~~~~~~~

The IIO example exposes the AD9081 MxFE through the IIO framework using
iio_axi_adc and iio_axi_dac, enabling host-side data capture and injection
over UART. If IIOD is enabled, the board can be used with any ADI IIO client:

* `libiio <https://wiki.analog.com/resources/tools-software/linux-software/libiio>`_ command-line tools
* `IIO-Oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope/releases>`_
* `Scopy v2 <https://github.com/analogdevicesinc/scopy/releases/tag/v2.0.0>`_

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

* `AD9081-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9081.html>`_
* `VCU118 <https://www.xilinx.com/VCU118>`_ or `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the AD9081-FMCA-EBZ to the carrier board via the FMC+ connector
(FMC+ slot on VCU118, FMC HPC0 on ZCU102). The JESD204B/C high-speed serial
lanes, SPI interface, clock signals, and power rails are all routed through the
FMC connector. Connect a USB-to-serial cable to the carrier board UART
(115200 baud) for console output.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``iio``.
Available boards: ``vcu118``, ``zcu102``.

The HDL design used is ``ad9081_fmca_ebz``. The hardware name composed for the
XSA download is ``ad9081_fmca_ebz_<board>`` (e.g. ``ad9081_fmca_ebz_vcu118``).

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
      --project ad9081 --variant demo --board vcu118 \
      --hardware /path/to/ad9081_fmca_ebz_vcu118/system_top.xsa

   # Build the IIO example on zcu102
   python tools/scripts/no_os_build.py build \
      --project ad9081 --variant iio --board zcu102 \
      --hardware /path/to/ad9081_fmca_ebz_zcu102/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad9081 --variant demo --board vcu118 \
      --hardware /path/to/ad9081_fmca_ebz_vcu118/system_top.xsa \
      --probe openocd --flash

For more information on the AD9081 device and available JESD204 profiles,
refer to the `AD9081 User Guide <https://www.analog.com/media/en/technical-documentation/user-guides/ad9081-ad9082-ug-1578.pdf>`__
and the HDL reference designs for
`AD9081-FMCA-EBZ <https://analogdevicesinc.github.io/hdl/projects/ad9081_fmca_ebz/>`__
and `QUAD-MxFE <https://analogdevicesinc.github.io/hdl/projects/ad_quadmxfe1_ebz/>`__.
