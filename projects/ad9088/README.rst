AD9088 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD9084-FMCA-EBZ <https://www.analog.com/AD9084>`_ (Rev. C), carrying either
  the `AD9084 <https://www.analog.com/AD9084>`_ or the
  `AD9088 <https://www.analog.com/AD9088>`_

Overview
--------

The AD9088 and AD9084 are Apollo MxFE (mixed-signal front end) devices that
combine wideband RF DACs, wideband RF ADCs, and a configurable digital
datapath in a single package. The AD9088 is an octal 16-bit, 16 GSPS RF DAC
with an octal 12-bit, 8 GSPS RF ADC (8T8R); the AD9084 is a quad 16-bit,
28 GSPS RF DAC with a quad 12-bit, 20 GSPS RF ADC (4T4R). Each device is
organized as two independent *sides*, and each side carries its own
converters, coarse and fine digital up/down converters, and JESD204C link.

The datapath is not fixed in hardware. It is described by a *device profile*
image that the driver loads over SPI at start-up, together with signed core
firmware for the on-chip processor. The profile sets the JESD204 mode,
converter counts, decimation and interpolation ratios, and sample rates, so the
same silicon can present very different link geometries. This is why the
profile chosen at build time has to match the HDL design loaded on the FPGA.

Host control is over a SPI bus. Sample data moves over JESD204C links to
the FPGA, where the ADI HDL reference design terminates them in JESD204
transport-layer cores and moves samples to and from system memory with AXI
DMA.

Applications
------------

* Multiband, multimode digital radios
* 3G/4G/5G massive MIMO base stations
* Wideband phased-array radar and electronic warfare
* Electronic test and measurement equipment
* Wideband satellite communications

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

Power the evaluation board and the VCU118 as described in their respective
user guides. The firmware makes no assumptions about the supply arrangement
beyond the board being powered and its clocks running before the AD9088 is
initialized.

Clocking and Synchronization Devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Three support devices share the clock-generation SPI bus and are brought up by
this project before the Apollo itself:

======= ====== ================================================ =======
Device  SPI CS Function                                         Setting
======= ====== ================================================ =======
ADF4382 0      Device clock PLL, drives the Apollo device clock 20 GHz
HMC7044 1      Clock distribution for the FPGA reference clocks 2.5 GHz
ADF4030 4      JESD204 SYSREF provider, BSYNC source for MCS    2.5 GHz
======= ====== ================================================ =======

All three run from the board's 125 MHz reference. The HMC7044 figure is its
PLL2 frequency and the ADF4030 figure its VCO frequency; SYSREF itself is
9.765625 MHz.

The ADF4030 is the JESD204 SYSREF provider in this design.
It supplies the SYSREF pulse that subclass 1 deterministic latency depends on,
and it drives the BSYNC line that the multi-chip synchronization (MCS)
calibration measures against.

The Apollo itself sits on a second SPI bus at chip select 0, and its reset is
driven from an AXI GPIO.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9088/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9088/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

Build with ``--variant basic_example``.

The basic example performs the full bring-up and stops there. It initializes
the ADF4382, HMC7044, and ADF4030, binds the multi-chip synchronization
callbacks, brings up the AXI DMAC, transceiver, and JESD204 link-layer cores,
then initializes the AD9088 itself — loading the core firmware and the device
profile over SPI. It builds the JESD204 topology and runs the link state
machine, reads back both link-layer cores' status, and prints
``Project configured`` when the link is up.

It then exercises the NCO control API as a read-back check, setting and reading
four frequencies on side A's first datapath: the receive and transmit coarse
NCOs at 125 MHz and the receive and transmit fine NCOs at 40.2 MHz. Each round
trip is printed as it is made. No sample data is moved.

Use this variant to confirm that the SPI buses, profile, clocking, and JESD204
link are all healthy before involving the datapath.

DMA Example
~~~~~~~~~~~

Build with ``--variant dma_example``.

The DMA example performs the same bring-up as the basic example, then moves
sample data in both directions over a cabled loopback. It transmits a tone and
captures into memory; it does not analyse the capture. What it reports is where
the capture landed, so the data can be pulled out and examined with an external
tool.

Once the link is up it works out the capture geometry from the links the
profile actually has in use. Both
sides share one transport layer in this HDL build, so a capture holds every
side at once — side A's converters first, side B's after them — and the example
prints each side's window along with the total sample count and width. A link
carrying more than four converters per side is rejected rather than captured
half-wrong, and a transmit link is required to carry its converters in I/Q
pairs.

It then tunes every coarse and fine NCO on every side in use to the same
frequency — the coarse ones to an eighth of the DAC rate, the fine ones to
0 Hz. Tuning transmit and receive identically makes the two frequency
translations cancel, so a tone arrives at the frequency it was sent at. Every
datapath on the side is tuned, not just the one carrying the first I/Q pair: a
datapath left on the profile's own frequencies would lose its tone outside the
passband while the rest of the capture looked fine. The example checks that
transmit and receive ended up matched and fails if they did not.

The transmit side tiles a shared sine table across its buffer and replays it
cyclically through the AXI DMAC and the transmit data offload. The transfer is
sized to the smaller of the buffer and the offload's own memory, then floored
to a whole number of table passes so the cyclic wrap leaves no phase
discontinuity — which matters because the capture is shorter than the replay
and can start anywhere in it.

The receive data offload is reset and re-armed immediately before the capture.
This design synthesizes it with auto-bringup enabled and the receive instance
in one-shot mode, so by capture time it holds a fill from before the link
existed and its store phase is long over; without the re-arm the capture
returns that stale fill or times out.

The example finishes by parking in an idle loop with the link still up, so the
capture buffer stays intact and readable from a debugger.

A coaxial cable between the DAC output and the ADC input is what makes the
captured tone the transmitted one. Without it the capture still runs and
reports normally — it just contains noise.

Capture depth is ``ADC_BUFFER_SAMPLES`` (16384 samples per converter) in
`src/platform/xilinx/parameters.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9088/src/platform/xilinx/parameters.h>`__.

If multi-chip synchronization leaves the receiver out of sync, the example
brings it back before deriving anything from the link. This is controlled by
``RX_LINK_RECOVER`` in
`src/examples/dma/dma_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9088/src/examples/dma/dma_example.c>`__.

Selecting the Device Profile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both variants embed a device profile image in the ELF. It is selected by the
``CONFIG_AD9088_PROFILE`` Kconfig string and defaults to
``204C_M4_L8_NP16_20p0_4x2.bin``. A bare file name is resolved relative to
`drivers/rf-transceiver/ad9088/firmware <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/rf-transceiver/ad9088/firmware>`__;
an absolute path is used as given, so a profile generated outside the tree
works too. Whichever image is picked, it is embedded under a fixed name, so the
symbols the driver looks for never change.

The profile **must** match the HDL design programmed into the FPGA. A mismatch
between the link geometry the profile describes and the one the HDL implements
shows up as a JESD204 link that never reaches ``LINK_RUNNING``.

The file name encodes the configuration. For
``204C_M4_L8_NP16_20p0_4x2.bin``:

========= ==============================================================
Field     Meaning
========= ==============================================================
``204C``  JESD204C
``M4``    converters per link
``L8``    lanes per link
``NP16``  bits per sample
``20p0``  converter sample rate in GSPS (20.0)
``4x2``   datapath configuration for the fine/coarse up and down converters
========= ==============================================================

Three profiles ship with the driver: ``204C_M4_L8_NP16_20p0_4x2.bin`` (the
default), ``204C_M4_L8_NP16_20p0_4x4.bin``, and
``204C_M4_L4_NP16_20p0_4x4.bin``.

Change it with:

.. code-block:: bash

   cmake --build build/ad9088-dma_example-vcu118 --target menuconfig

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD9088 <https://www.analog.com/AD9088>`_ or
  `AD9084 <https://www.analog.com/AD9084>`_ evaluation board
* `VCU118 <https://www.xilinx.com/products/boards-and-kits/vcu118.html>`_
  FPGA carrier

The FPGA design comes from the ADI HDL repository, project
`ad9084_ebz <https://github.com/analogdevicesinc/hdl/tree/main/projects/ad9084_ebz>`__,
carrier ``vcu118``. Build it there and export ``system_top.xsa``, which the
no-OS build consumes through ``--hardware``.

Connections
^^^^^^^^^^^

* Seat the evaluation board on the VCU118 FMC+ connector (J22).
* Power the boards as described in their user guides.
* Connect the VCU118 USB UART bridge to the host PC. The console runs at
  **115200** baud, 8N1.
* Connect the VCU118 JTAG USB port to the host PC for programming.
* For the DMA example, connect a coaxial cable between the DAC output and the
  ADC input on the evaluation board. The basic example does not need it.

The clock-generation devices (ADF4382, HMC7044, ADF4030) and the Apollo are
wired on the evaluation board and need no external connections; their chip
selects are listed in `Clocking and Synchronization Devices`_.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system. Available variants:
``basic_example``, ``dma_example``. Available boards: ``vcu118``.

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

``HARDWARE`` must be an **absolute** path: the toolchain and the flash target
read it from different working directories.

.. code-block:: bash

   # Source the Vitis environment (sets XILINX_VITIS and adds tools to PATH)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "<C:\path\to\Vitis>"

   cd no-OS

   # configure (replace the variant as needed)
   cmake -B build/ad9088-dma_example-vcu118 --preset vcu118 \
      -DPROJECT_DEFCONFIG=ad9088/dma_example.conf \
      -DHARDWARE=/abs/path/to/system_top.xsa

   # build
   cmake --build build/ad9088-dma_example-vcu118 --target ad9088

   # program the bitstream and run the ELF over JTAG
   cmake --build build/ad9088-dma_example-vcu118 --target flash

The ELF is written to
``build/ad9088-<variant>-vcu118/build/ad9088.elf``.

Xilinx uses its own JTAG flow through the Vitis tools, so the ``flash`` target
needs no ``PROBE`` setting. The same build directory also carries ``erase``,
``debug``, ``debug_server``, and ``menuconfig`` targets.

``tools/scripts/no_os_build.py`` wraps the three commands above if you prefer
a single invocation:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project ad9088 --variant dma_example --board vcu118 \
      --hardware /abs/path/to/system_top.xsa

   # list the valid project/variant/board combinations
   python tools/scripts/no_os_build.py list --project ad9088
