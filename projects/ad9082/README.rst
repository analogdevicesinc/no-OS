AD9082 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD9082-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9082.html>`_

Supported Parts
---------------

* `AD9082 <https://www.analog.com/en/products/ad9082.html>`_

Supported Carriers
------------------

.. list-table::
   :widths: 40 35 25
   :header-rows: 1

   * - Evaluation board
     - Carrier
     - FMC slot
   * - `AD9082-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9082.html>`_
     - `VPK180 <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/vpk180.html>`_
     - FMC+

Overview
--------

The AD9082 is a highly integrated, multi-channel mixed-signal front-end (MxFE)
device combining high-speed DACs and ADCs with on-chip digital up-conversion
(DUC), digital down-conversion (DDC) and DSP blocks, aimed at wideband
software-defined radio across 5G infrastructure, broadband communications and
defense electronics. The device connects to the FPGA over a high-speed
JESD204B/C serial interface and is clocked by an on-board HMC7044 jitter
attenuator / clock generator.

The AD9082 shares the no-OS ``ad9081`` device driver
(`drivers/adc/ad9081 <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/adc/ad9081>`__);
the driver reads the chip ID at probe and configures the AD9082 datapath
accordingly. This project targets the AD9082-FMCA-EBZ on the AMD **VPK180**
(Versal Premium ``xcvp1802``) carrier. The on-board Versal hardens the JESD204
transceiver (GTY) line rate in the programmable device image (PDI), so no-OS
does not drive an ADXCVR or AXI-CLKGEN IP on this platform — it only brings up
the HMC7044 clock tree, releases the GT reset over an AXI GPIO, and runs the
JESD204 FSM.

The bundled profile (``profiles/vpk180_ad9082``) matches the stock
``ad9082_fmca_ebz`` VPK180 bitstream: JESD204C, M=4, L=8, S=4, N=NP=12, 24.75 Gbps
lane rate, with the MxFE configured for a 4.0 GSPS DAC / 4.0 GSPS ADC datapath.

Applications
------------

* 5G wireless infrastructure
* Broadband communication systems
* Defense electronics and phased array radar
* Electronic warfare
* Ground-based SATCOM

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9082-FMCA-EBZ is powered from the FMC carrier through the FMC+ connector;
on-board regulators derive the internal supply rails. The VPK180 carrier is
powered from its external 12 V laboratory supply. The carrier system controller
applies the FMC adjustable supply (``VADJ_FMC``, 1.5 V) a few seconds after
power-on — allow the rail to come up before running the application, otherwise
the MxFE and HMC7044 SPI buses read back ``0xFF``.

Reference Input Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board can use its on-board HMC7044 clock generator (default) or an external
reference applied at the SMA input.

.. note::

   The following rework may be required on older board revisions:

   * To rely on the on-board HMC7044 instead of an external clock source, rotate
     the C6D/C4D capacitors to position C5D/C3D. (On the latest board revision
     this is the default and may not be needed.)
   * If LEDs V1P0_LED and VINT_LED are not lit, depopulate R22M and populate R2M.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

========= =====================================================
Connector Function
========= =====================================================
J1        FMC+ connector that mates with the VPK180 carrier
J10       SMA connector for optional external reference clock
========= =====================================================

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow. Selecting
a variant at build time (``--variant <name>``) chooses which application is
compiled. The platform ``main()`` is a thin dispatcher that calls
``example_main()``, provided by the selected example. Shared initialization
(HMC7044 clocks + AXI-JESD204 link bring-up) is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9082/src/common>`__,
and platform-specific macros and init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9082/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example (variant ``basic_example``) is the bring-up vehicle: it
initializes the HMC7044 clock tree, brings up the AXI-JESD204 RX/TX link cores,
initializes the AD9082 (via the ``ad9081`` driver), runs the JESD204 FSM and
prints the link status. No data is moved. The output looks like the one below:

.. code-block:: bash

   AD9082 Rev. 3 Grade 2 (API 1.7.0) probed
   JESD TX (JRX) Link0 204C status: Link is good (6)
   tx_jesd status:
           Measured Link Clock: 375.002 MHz
           Reported Link Clock: 375.000 MHz
           Measured Device Clock: 250.002 MHz
           Reported Device Clock: 250.000 MHz
           Desired Device Clock: 250.000 MHz
           Lane rate: 24750.000 MHz
           Lane rate / 66: 375.000 MHz
           LEMC rate: 11.718 MHz
           Link status: DATA
           SYSREF captured: Yes
           SYSREF alignment error: No
   tx_dac: Successfully initialized (4000048828 Hz)
   rx_adc: Successfully initialized (4000048828 Hz)
   Bye

The JESD profile is selected unconditionally from ``profiles/vpk180_ad9082`` and
**must match the link mode the loaded bitstream (PDI) was synthesized for**. The
VPK180 hardens the GT line rate in the PDI, so a profile/bitstream mismatch
leaves the GT PLL unlocked and the JESD ``reset-done`` never asserts. A second
profile for a different link rate will be added under ``profiles/`` when its
matching bitstream is available.

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) plays a sine wave out of the DACs and
captures ADC samples back, both over the PL AXI DMACs into the Cortex-A72 DDR.

A sine look-up table is written to a DDR buffer and loaded, with a single
non-cyclic transfer, into the TX data-offload in the HDL; the offload then
replays it continuously, so the DAC output is gap-free. The ADC samples are
captured into a second DDR buffer through the RX DMAC. Both buffers live in a
dedicated DDR region (``DAC_DDR_BASEADDR`` / ``ADC_DDR_BASEADDR`` in
``parameters.h``), clear of the CPU image, and the CPU flushes / invalidates
them by hand because the PL DMACs are not cache-coherent with the APU and master
DDR over the Versal NoC.

The DMA and IIO examples play the DAC data at baseband: the TX main NCO shift
is forced to 0 Hz instead of the profile's 1 GHz, which would alias in the
full-bandwidth RX path and make the loopback capture unreadable. This matches
the Linux reference design.

On success the console reports the buffers in the shared ``DMA_EXAMPLE`` format::

    DMA_EXAMPLE Tx: address=0x10000000 samples=32768 channels=4 bits=32
    DMA_EXAMPLE Rx: address=0x14000000 samples=65536 channels=4 bits=16

The RX line means the captured data is at ``0x14000000``: 65536 16-bit samples
across 4 converters, i.e. 16384 samples per converter. The profile carries two
complex (I/Q) channels, so the four converters are rx1 I, rx1 Q, rx2 I, rx2 Q.

Retrieve the captured data into ``.csv`` files with the standard Tcl script,
passing ``VERSAL`` as the processor type:

.. code-block:: bash

   xsct tools/scripts/platform/xilinx/capture.tcl VERSAL 0x14000000 65536 4 16

The ``.csv`` files can then be visualised using the
`plot.py <https://github.com/analogdevicesinc/no-OS/blob/master/tools/scripts/platform/xilinx/plot.py>`_
script in the no-OS repository. The following command plots all 4 channels:

.. code-block:: bash

   python tools/scripts/platform/xilinx/plot.py 4

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio_example``) brings the links up via the shared
code, then runs an IIOD server (via ``iio_axi_adc`` / ``iio_axi_dac``) so a host
can capture ADC samples and inject DAC samples over the console UART. The board
can be used with any ADI IIO client:

* `libiio <https://wiki.analog.com/resources/tools-software/linux-software/libiio>`_ command-line tools
* `IIO-Oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope/releases>`_
* `Scopy v2 <https://github.com/analogdevicesinc/scopy/releases/tag/v2.0.0>`_

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

To run the IIOD demo, connect to the board via UART with the following
settings:

.. code-block:: bash

   Baud Rate: 115200bps
   Data: 8 bit
   Parity: None
   Stop bits: 1 bit
   Flow Control: none

After the link bring-up messages, the application prints ``Running IIOD
server...``. Close the serial terminal, then start IIO-Oscilloscope and connect
it through the serial backend with the same settings, for example:

.. code-block:: bash

   osc -u serial:/dev/ttyUSB0,115200,8n1n

The IIO buffers hold up to 8192 DAC and 16384 ADC samples
(``MAX_DAC_BUF_SAMPLES`` / ``MAX_ADC_BUF_SAMPLES`` in ``parameters.h``).

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD9082-FMCA-EBZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad9082.html>`_
* `VPK180 Evaluation Kit <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/vpk180.html>`_ (AMD Versal Premium ``xcvp1802``)

Connections
^^^^^^^^^^^

Mate the AD9082-FMCA-EBZ with the VPK180 **FMC+** connector before powering the
carrier. The JESD204C high-speed serial lanes, the MxFE and HMC7044 SPI buses,
the clock signals and the power rails are all routed through the FMC connector.
Connect the carrier USB-UART cable to the host for serial console access at
**115200 baud, 8N1** (the Versal PS-UART appears as one interface of the
on-board USB-serial bridge). Set the carrier boot-mode switch ``SW1`` to **all
OFF (JTAG)** so the device is configured over JTAG.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``basic_example``, ``dma_example``,
``iio_example``. Available board: ``vpk180``.

The VPK180 runs no-OS on the Versal **Cortex-A72** application processor (APU).
A Xilinx XSA hardware handoff is required and is passed with ``--hardware``; the
HDL design is ``ad9082_fmca_ebz`` (VPK180 variant). The Vitis 2025.1 Versal BSP
is System-Device-Tree (SDT) flavored, which the project's ``CMakeLists.txt``
selects automatically for this board.

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

.. code-block:: bash

   # Source the Vitis toolchain environment (adjust the path for your install):
   export XILINX_VITIS=/opt/Xilinx/2025.1/Vitis
   source "$XILINX_VITIS/settings64.sh"
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "C:\Xilinx\2025.1\Vitis"
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the basic example for the VPK180
   python tools/scripts/no_os_build.py build \
      --project ad9082 --variant basic_example --board vpk180 \
      --hardware /path/to/ad9082_fmca_ebz_vpk180/system_top.xsa

   # Build the DMA example
   python tools/scripts/no_os_build.py build \
      --project ad9082 --variant dma_example --board vpk180 \
      --hardware /path/to/ad9082_fmca_ebz_vpk180/system_top.xsa

   # Build the IIO example
   python tools/scripts/no_os_build.py build \
      --project ad9082 --variant iio_example --board vpk180 \
      --hardware /path/to/ad9082_fmca_ebz_vpk180/system_top.xsa

The built ELF is at
``build/ad9082-<variant>-vpk180/build/ad9082.elf``.

.. note::

   Versal is configured with a PDI over JTAG, not a ZynqMP FSBL, so the
   ``--probe openocd --flash`` download path (FSBL/ZynqMP only) does not apply
   here. Program the device and load the ELF with ``xsct``: extract the
   ``system_top.pdi`` from the XSA (it is a zip), then program the PDI, reset the
   APU and download the ELF:

   .. code-block:: bash

      # Wait a few seconds after power-on for VADJ_FMC to come up, then:
      xsct -eval '
        connect
        device program /path/to/system_top.pdi
        after 2000
        targets -set -filter {name =~ "*Cortex-A72*#0"}
        rst -proc
        dow build/ad9082-basic_example-vpk180/build/ad9082.elf
        con'

   The ``after 2000`` avoids a race where the A72 target is not yet enumerated
   (it appears only once the PLM has booted the APU).
