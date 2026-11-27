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

Two FPGA carriers are supported (see `No-OS Supported Platforms`_):

* `VCU118 <https://www.xilinx.com/products/boards-and-kits/vcu118.html>`_
  (MicroBlaze), a JESD204C design
* Intel Agilex 5E065B Premium Development Kit (Nios V/g soft core), a JESD204B
  design

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

On Agilex 5 the AD9088 brings up the links of both sides (four links: the
side A and side B framers and deframers); on the VCU118 it brings up side A.

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

IIO Example
~~~~~~~~~~~

Build with ``--variant iio_example``.

The IIO example performs the same bring-up as the DMA example, including the
receive link recovery and the default NCO tuning, then launches an IIOD server
on the board so that an IIO client can connect over UART. Two devices are
exposed:

========================== ===================================================
Device                     Function
========================== ===================================================
``cf-ad9088-lpc``          Receive transport core, one channel per converter
``cf-ad9088-dds-core-lpc`` Transmit transport core, DDS or DMA data per channel
========================== ===================================================

Both are as wide as the profile's links together, side A's converters first
and side B's after them. The transmit core starts on its DDS, so a tone plays
as soon as the link is up; with a coaxial cable between the DAC output and the
ADC input it shows up in the receive capture at the frequency it was sent at.

Each capture re-arms the receive data offload before the DMA starts, since it
holds a single fill otherwise. Buffer sizes are limited to the data offload
memories, so a client cannot request a capture or a transmit buffer the
hardware cannot hold.

Any combination of channels can be enabled, including counts that are not a
power of two. IIO places each buffer at a multiple of the sample count times
the bytes per scan, which for three channels, for example, is not aligned to
the 1024-bit bus of the DMA. The example therefore runs both DMAs on aligned
staging buffers of its own and copies the samples to or from the IIO buffer.

The DDS starts at 3 MHz with each of its two tones at a scale of 0.05, which
is about -20 dBFS. For a clearer loopback, raise the level and move the tone,
for example on the first transmit channel:

.. code-block:: bash

   iio_attr -u serial:/dev/ttyUSB1,115200,8n1n -c cf-ad9088-dds-core-lpc \
      altvoltage0 scale 0.25
   iio_attr -u serial:/dev/ttyUSB1,115200,8n1n -c cf-ad9088-dds-core-lpc \
      altvoltage0 frequency 30000000

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

After the link bring-up messages, the application prints:

.. code-block:: text

   Running IIOD server...
   If successful, you may connect an IIO client application by:
   1. Disconnecting the serial terminal you use to view this message.
   2. Connecting the IIO client application using the serial backend configured as shown:
       Baudrate: 115200
       Data size: 8 bits
       Parity: none
       Stop bits: 1
       Flow control: none

Close the serial terminal, then connect with a serial-backend enabled
`iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`__ or
with libiio, for example:

.. code-block:: bash

   iio_info -u serial:/dev/ttyUSB1,115200,8n1n

On Agilex 5 the IIOD server runs over the Nios V JTAG-UART, not a USB UART,
and the build differs in three ways, all behind
``CONFIG_ALTERA_PLATFORM_NIOSV``:

* The JTAG-UART is reachable only through Intel JTAG Atlantic, not as a host
  serial device, so a libiio client needs a byte-transparent bridge from JTAG
  Atlantic to a pseudo-terminal (see `Program and Run`_). Do not open
  ``juart-terminal`` against the IIO ELF: the UART carries the IIOD protocol.
* The receive buffer holds 8192 samples per converter instead of 16384, so the
  staging buffers fit the 2 MB Nios V on-chip memory.
* The transmit data offload is put in bypass before IIOD starts, so a cyclic
  DMA buffer streams to the transport layer; a bitstream built without bypass
  only warns, and DDS tones are unaffected.

``boards/iio_example/agilex5.conf`` adds the Altera JTAG-UART and interrupt
controller drivers (``CONFIG_UART_ALTERA``, ``CONFIG_IRQ_ALTERA``).

Selecting the Device Profile
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Every variant embeds a device profile image in the ELF. It is selected by the
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

The firmware directory ships these profiles:

* ``204C_M4_L8_NP16_20p0_4x2.bin`` (the default, VCU118)
* ``204C_M4_L8_NP16_20p0_4x4.bin``, ``204C_M4_L4_NP16_20p0_4x4.bin``
* ``204C_L2_M4_NP16_10p0_8x4.bin``, ``204C_L2_M4_NP16_10p8_8x8_8x4.bin``
* ``204B_L2_M4_NP16_16p0_16x4.bin``
* ``204B_M4_L2_NP16_21p6_10p8_8x8_8x4.bin`` (Agilex 5: JESD204B, 13.5 Gbps
  lanes)

Any other profile has to be supplied as a file, either copied into the firmware
directory or referenced by absolute path.

The profile is set per carrier in ``boards/<variant>/<board>.conf``: the VCU118
configs select ``204C_M4_L8_NP16_20p0_4x2.bin``, and the Agilex 5 configs select
``204B_M4_L2_NP16_21p6_10p8_8x8_8x4.bin`` together with the engineering signed
firmware set (``CONFIG_AD9088_FW_SET_ENG``), since the Nios V on-chip memory
holds only one set. The variant defconfigs do not set
``CONFIG_AD9088_PROFILE``, so without a board config a build uses the Kconfig
default. In an already configured build
directory (see `Xilinx build command`_), change it with menuconfig and rebuild:

.. code-block:: bash

   cmake --build build/ad9088-dma_example-vcu118 --target menuconfig
   cmake --build build/ad9088-dma_example-vcu118 --target ad9088

The selection is kept in the build directory and survives rebuilds. To make it
part of a variant instead, add the symbol to the variant's defconfig
(``basic_example.conf``, ``dma_example.conf`` or ``iio_example.conf``) and
rebuild:

.. code-block:: text

   CONFIG_AD9088_PROFILE="/abs/path/to/my_profile.bin"

Removing the line again does not restore the default in an existing build
directory, since the value is already stored there. Reconfigure with
``--fresh`` to start from the defconfigs again:

.. code-block:: bash

   cmake -B build/ad9088-dma_example-vcu118 --preset vcu118 --fresh \
      -DPROJECT_DEFCONFIG=ad9088/dma_example.conf \
      -DHARDWARE=/abs/path/to/system_top.xsa

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

.. _Xilinx build command:

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system. Available variants:
``basic_example``, ``dma_example``, ``iio_example``. Available boards:
``vcu118``.

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

The build drives the Vitis Python CLI (``vitis -s``) and was tested with Vitis
2025.1. Vitis 2022.x does not provide that CLI and fails at configure time.

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

Intel / Altera (Agilex 5, Nios V)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* Intel Agilex 5E065B Premium Development Kit (Nios V/g soft core)
* AD9084-EBZ evaluation board on the FMC connector
* A USB-Blaster / on-board JTAG for FPGA configuration, ELF download and the
  JTAG-UART console

HDL → BSP → no-OS handoff
^^^^^^^^^^^^^^^^^^^^^^^^^

The Nios V build is not self-contained: the no-OS CMake build consumes a BSP
generated from the FPGA design (the memory map, linker script and HAL). The
end-to-end flow — Quartus/HDL → ``system_bd.qsys``/``.sopcinfo`` handoff →
``niosv-bsp`` → no-OS — is documented in the
`HDL / Quartus to no-OS Build Flow (Agilex 5)`_ section below. Read it first;
the paragraphs below assume the BSP already exists.

.. note::

   After any hand-edit to the HDL Qsys ``.tcl`` (e.g. on-chip-memory size),
   do a clean HDL rebuild (``make clean && make``) so the generated Qsys is
   fully regenerated, then regenerate the BSP. A stale partial regeneration
   can produce a bitstream that behaves differently from the source.

Build Command
^^^^^^^^^^^^^

Put the toolchain on your ``PATH`` and export the build variables once per
shell. Save this as e.g. ``~/altera.sh`` (editing ``A=`` and ``ALTERA_BSP_DIR=``)
and ``source`` it:

.. code-block:: bash

   #!/usr/bin/env bash
   # Altera / Quartus Prime Pro + Nios V environment for the ad9088 Agilex build.
   A="$HOME/altera_pro/26.1.1"                                  # install root
   RF="$A/riscfree/toolchain/riscv32-unknown-elf/bin"

   # Tools on PATH: quartus_pgm, jtagconfig, juart-terminal, niosv-bsp,
   # niosv-download, and the RISC-V toolchain.
   export PATH="$A/quartus/bin:$A/niosv/bin:$RF:$PATH"
   export QUARTUS_ROOTDIR="$A/quartus"

   # no-OS CMake build variables (gcc also drives the assembler, handling .S).
   export CMAKE_C_COMPILER="$RF/riscv32-unknown-elf-gcc"
   export CMAKE_CXX_COMPILER="$RF/riscv32-unknown-elf-g++"
   export CMAKE_ASM_COMPILER="$RF/riscv32-unknown-elf-gcc"

   # Prebuilt Nios V BSP, matched to the AD9084-EBZ .sof.
   export ALTERA_BSP_DIR="$HOME/hdl/projects/ad9084_ebz/nios_a5e/software/bsp"
   export ALTERA_BSP_LIB="$ALTERA_BSP_DIR/build/libhal2_bsp.a"

There are two ways to build: directly with the tracked ``agilex5`` preset (no
user preset needed), or via a one-time ``CMakeUserPresets.json`` convenience.

Option 1 — tracked preset + environment (works on a fresh clone)
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

The tracked base preset ``agilex5`` (in ``board_configs/altera/CMakePresets.json``)
sets the platform, board and toolchain file. The per-machine paths come from the
environment sourced above; the build type and example come from ``-D`` flags:

.. code-block:: bash

   source ~/altera.sh     # PATH + CMAKE_* + ALTERA_BSP_* from the block above

   cmake --preset agilex5 -B build_agilex5_ebz \
         -DCMAKE_BUILD_TYPE=MinSizeRel \
         -DPROJECT_DEFCONFIG=ad9088/basic_example.conf
   cmake --build build_agilex5_ebz --target ad9088

   # Output ELF: build_agilex5_ebz/build/ad9088
   # DMA example: use -DPROJECT_DEFCONFIG=ad9088/dma_example.conf into build_agilex5_dma

``MinSizeRel`` is required so the image fits the Nios V on-chip memory. The
board config (``boards/basic_example/agilex5.conf``) selects the 204B profile
and the engineering firmware set.

Option 2 — user preset (one-time setup, then no flags)
""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Put the machine-specific paths — the BSP directory, the BSP archive and the
RISC-V compilers — in an **untracked** ``CMakeUserPresets.json`` that inherits
``agilex5``, so absolute paths never land in the tracked tree. The file must be
at the **repository root** (next to the tracked ``CMakePresets.json``); CMake
only reads user presets from there:

.. code-block:: json

   {
     "version": 7,
     "configurePresets": [
       {
         "name": "agilex5-ebz",
         "inherits": "agilex5",
         "binaryDir": "${sourceDir}/build_agilex5_ebz",
         "cacheVariables": {
           "CMAKE_BUILD_TYPE": "MinSizeRel",
           "PROJECT_DEFCONFIG": "ad9088/basic_example.conf",
           "ALTERA_BSP_DIR": "/path/to/hdl/projects/ad9084_ebz/nios_a5e/software/bsp",
           "ALTERA_BSP_LIB": "/path/to/.../software/bsp/build/libhal2_bsp.a",
           "CMAKE_C_COMPILER":   "/path/to/riscfree/toolchain/riscv32-unknown-elf/bin/riscv32-unknown-elf-gcc",
           "CMAKE_ASM_COMPILER": "/path/to/riscfree/toolchain/riscv32-unknown-elf/bin/riscv32-unknown-elf-gcc",
           "CMAKE_CXX_COMPILER": "/path/to/riscfree/toolchain/riscv32-unknown-elf/bin/riscv32-unknown-elf-g++"
         }
       }
     ]
   }

With that file at the repo root, the build needs no flags (and, since the
compilers/BSP are in the preset, only ``PATH`` from ``altera.sh``):

.. code-block:: bash

   cmake --preset agilex5-ebz
   cmake --build build_agilex5_ebz --target ad9088

   # Output ELF: build_agilex5_ebz/build/ad9088

To build a different variant, point ``PROJECT_DEFCONFIG`` at its config — e.g.
``ad9088/iio_example.conf`` for the IIO server (see `IIO Example`_; add an ``agilex5-iio`` preset
that inherits ``agilex5``, or override ``-DPROJECT_DEFCONFIG`` on the command
line). The IIO variant links the same way; if the BSP archive is not
auto-detected (it lives in a ``build/`` subdirectory), pass it explicitly with
``-DALTERA_BSP_LIB=/path/to/.../software/bsp/build/libhal2_bsp.a``.

Program and Run
^^^^^^^^^^^^^^^

The software is JTAG-downloaded onto the running bitstream (it is not booted
from flash). Program the FPGA, download the ELF, then open the console.

.. code-block:: bash

   # 1. Configure the FPGA. Full-chip programming needs exclusive JTAG, so
   #    close any open juart-terminal first (else "Conflict with another device").
   quartus_pgm -c 1 -m jtag -o "p;ad9084_ebz_nios_a5e.sof"

   # 2. Download the ELF and start the core.
   niosv-download -r -g build_agilex5_ebz/build/ad9088

   # 3. Open the JTAG-UART console.
   juart-terminal --cable=1 --device=1 --instance=0

.. note::

   The Nios V/g instruction master reaches only the on-chip memory, so the
   ELF must fit it (hence ``MinSizeRel`` and a single firmware set). The BSP
   uses ``LMA == VMA`` (no boot-copy) because ``niosv-download`` writes each
   segment directly to its run address.

.. note::

   **IIO variant transport.** With the ``iio_example`` ELF the JTAG-UART no
   longer carries a human console — it carries the IIOD serial protocol, so do
   not open ``juart-terminal`` against it. A libiio host connects with the
   serial backend, whose URI is ``serial:<device>,<baud>,8n1`` (libiio 1.0),
   e.g. ``iio_info -u serial:/dev/pts/N,115200,8n1``.

   The catch: the Nios V JTAG-UART is not exposed as a POSIX serial device — it
   is reachable only through Intel JTAG Atlantic (the transport behind
   ``juart-terminal``/``jtagd``), so there is no ``/dev/ttyUSB*`` node for
   libiio to open. A **byte-transparent** bridge from the JTAG Atlantic endpoint
   to a pseudo-terminal is required, and ``serial:`` then points at that pty.
   ``juart-terminal`` itself is not guaranteed byte-transparent for the binary
   IIOD framing, so validate the bridge on hardware before relying on it.

Open in the RiscFree IDE (optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Every Agilex configure also generates an Eclipse-CDT project under ``.riscfree/``
that imports into the Ashling RiscFree IDE (the Eclipse-based IDE bundled with
the Nios V toolchain), the Nios V counterpart to Vitis for Xilinx. It is a
**build + code-navigation** project: building from the IDE just drives
``cmake --build`` against the configured build directory, and indexing is fed by
``compile_commands.json``. Hardware debug is not wired.

Let ``no_os_build.py`` build and open it in one step. ``--open`` builds, imports
the project into a dedicated workspace headlessly, then launches the GUI already
populated (the Altera analogue of ``--open`` for Vitis). It needs the compilers
and BSP paths in the environment — ``source`` the ``altera.sh`` setup block shown
above, or export ``CMAKE_C_COMPILER`` / ``CMAKE_CXX_COMPILER`` /
``CMAKE_ASM_COMPILER`` and ``ALTERA_BSP_DIR`` / ``ALTERA_BSP_LIB`` by hand:

.. code-block:: bash

   cd no-OS
   source ~/altera.sh     # or export the CMAKE_* and ALTERA_BSP_* variables manually

   python tools/scripts/no_os_build.py build \
       --project ad9088 --variant basic_example --board agilex5 --open

If you configured with ``cmake --preset`` instead, the ``.riscfree/`` project is
generated too; import it once with **File → Import → Existing Projects into
Workspace** and set the root directory to ``no-OS/.riscfree`` (a hidden folder —
type the path rather than browsing). The workspace stays populated afterwards.

Re-importing after a change: the project's source view is fixed at import time.
If you regenerate ``.riscfree`` (a different config, or an updated layout), a
plain re-import *merges* the new linked folders onto the old ones, leaving stale
entries. To refresh cleanly, either open a **fresh workspace**
(``RiscFree -data <new-dir>``) and import ``no-OS/.riscfree`` there, or in the
current workspace **delete the project** (right-click → Delete, with *Delete
project contents on disk* unchecked) and import it again.

HDL / Quartus to no-OS Build Flow (Agilex 5)
--------------------------------------------------

How the AD9084-EBZ FPGA design (Quartus/HDL) connects to the ad9088 no-OS
software build for the Agilex 5 Nios V/g soft core: what the HDL produces, what
the software consumes, and exactly which facts must be kept in sync between them.

Big Picture: Three Stages, One Handoff
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

   ┌─ STAGE 1: HDL / Quartus ────────────┐   ┌─ STAGE 2: BSP ──────┐   ┌─ STAGE 3: no-OS app ─┐
      *.tcl + ADI hdl lib + Intel IP          niosv-bsp reads the       CMake reads the BSP
           │  make → quartus_sh                hardware handoff          (headers + linker +
           ▼                                        │                     HAL archive)
      qsys-generate ──► system_bd.qsys  ───────────►│                          │
           │           system_bd.sopcinfo           ▼                          ▼
           ▼           (THE HANDOFF)           bsp/ (system.h,           build/ad9088 (.elf)
      synth/fit/asm ──► .sof (bitstream)       linker.x, HAL/, …)       (runs on the .sof)

The **hardware handoff** (``system_bd.qsys`` + ``system_bd/system_bd.sopcinfo``)
is the single contract between HDL and software. Everything no-OS needs about the
fabric comes from it, funnelled through the BSP.

Stage 1 — HDL / Quartus Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command:

.. code-block:: bash

   cd hdl/projects/ad9084_ebz/nios_a5e && [ADI_IGNORE_VERSION_CHECK=1] make

``make`` runs ``quartus_sh --64bit -t system_project.tcl``, which sources the
Qsys tcl, runs ``qsys-generate``, then ``execute_flow -compile``.
(``ADI_IGNORE_VERSION_CHECK=1`` is needed when the installed Quartus Pro is
26.1.1 but the project pins 26.1.0.)

Inputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Input
     - Role
   * - ``system_project.tcl``
     - top: device, pins, SDC, JESD defaults, ``execute_flow -compile``
   * - ``system_qsys.tcl`` (project)
     - apollo_spi @0xEA000, GTS PLLs, sysid; sources the common tcl
   * - ``common/ad9084_ebz_qsys.tcl``
     - JESD datapath: DMA, TPL, link_reconfig/mgmt, data_offload, PHY, IRQs,
       device clocks
   * - ``common/nios_a5e/nios_a5e_system_qsys.tcl``
     - the CPU carrier: Nios V/g, ``sys_int_mem`` OCM, dm_agent, timer, uart,
       gpio, sys_spi, DDR window
   * - ``system_top.v``, ``*.sdc``, board pin tcl
     - RTL wrapper, timing, pin assignments
   * - ``LIB_DEPS`` / ``INTEL_LIB_DEPS`` (Makefile)
     - ADI IP (axi_dmac, jesd204 tpl, util_pack, sysid) + Intel IP (adi_jesd204,
       jesd204_phy) built under ``hdl/library/``
   * - ``ad_project_params``
     - JESD mode/rate/M/L/S/NP, clock rates

Outputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Output
     - Consumed by
   * - ``ad9084_ebz_nios_a5e.sof``
     - JTAG config of the FPGA (``quartus_pgm``)
   * - ``system_bd.qsys``
     - → ``niosv-bsp -s`` (Stage 2 input)
   * - ``system_bd/system_bd.sopcinfo``
     - human/tool-readable handoff (NOT accepted by niosv-bsp)
   * - ``.qpf`` / ``.qsf``
     - → ``niosv-bsp -p`` (Stage 2 input)
   * - ``system_bd/`` IP tree, reports
     - fitter/timing artifacts

``make clean`` deletes ``system_bd.qsys``, ``system_bd/``, ``.sopcinfo``,
``.sof``, ``.qpf`` — the handoff is regenerated from tcl every build; never
hand-edit it.

Stage 2 — BSP Generation (the Bridge)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command (what the no-OS CMake integration runs, and what you run by hand to
pre-build a BSP):

.. code-block:: bash

   niosv-bsp -c -t=hal -p=<project.qpf> -s=<system_bd.qsys> <out>/settings.bsp

Inputs = the ``.qpf`` + ``system_bd.qsys`` handoff. Output = a BSP directory
whose files carry the HDL facts into software:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - BSP file
     - What it carries (the HDL→SW contract)
   * - ``system.h``
     - THE memory map: every ``*_BASE``, ``*_SPAN``, ``*_IRQ``, ``*_NAME``; CPU
       arch/caches; ``ALT_CPU_RESET_ADDR``, ``ALT_CPU_MTIME_OFFSET``, clock freqs
   * - ``linker.x`` + ``linker.h``
     - memory regions (OCM origin/length) and section→region placement; reset
       vector; alt_load setting
   * - ``toolchain.cmake``
     - ISA/ABI: ``-march=rv32im_zicbom -mabi=ilp32``, ``-nostdlib``, prefix
       ``riscv32-unknown-elf-``
   * - ``alt_sys_init.c``
     - generated device-init table (which devices exist, in what order)
   * - ``HAL/`` + ``drivers/``
     - Altera HAL + Avalon device drivers/headers
   * - ``CMakeLists.txt``
     - builds the BSP into ``libhal2_bsp.a``
   * - ``memory.gdb``
     - gdb memory-map (for debug sessions)
   * - ``settings.bsp``, ``summary.html``
     - the settings used / a readable report

Building the BSP Archive
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``niosv-bsp`` emits BSP **source**; the no-OS link needs the compiled archive:

.. code-block:: bash

   cd <bsp> && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
   # → <bsp>/build/libhal2_bsp.a

alt_load Must Be OFF for JTAG Download
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default the BSP enables ``alt_load()``, which sets section LMAs after
``.text`` for a boot-copy. For the JTAG-download workflow the loader writes each
segment to its own address, so LMA must equal VMA. Turn alt_load off before
building:

.. code-block:: bash

   niosv-bsp --update <bsp>/settings.bsp \
     "--cmd=set_setting hal.linker.enable_alt_load false" \
     "--cmd=set_setting hal.linker.enable_alt_load_copy_rodata false" \
     "--cmd=set_setting hal.linker.enable_alt_load_copy_rwdata false"

(Use the ``--cmd=...`` equals form; the space form fails. Verify afterwards that
``linker.x`` no longer prints the "alt_load facility is enabled" banner, and that
``readelf -l`` shows PhysAddr == VirtAddr on the LOAD segments.)

Stage 3 — How no-OS Consumes the BSP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``cmake/altera/altera_platform_sdk.cmake`` (``config_altera_sdk``):

- Resolves the BSP dir from ``ALTERA_BSP_DIR`` (CMake cache var **or** env; cache
  var wins), else auto-generates one from ``ALTERA_NIOSV_REPO_DIR``.
- Adds includes: ``${BSP}``, ``${BSP}/HAL/inc``, ``${BSP}/drivers/inc`` (+ the
  project ``src/platform/altera``).
- Links the BSP archive (``ALTERA_BSP_LIB`` cache var or env, else auto-located),
  with ``-nostdlib -T${BSP}/linker.x``.

The application ISA/ABI is set in ``drivers/platform/altera/toolchain.cmake``
(Nios V branch): ``-march=rv32im_zicbom -mabi=ilp32``, matching the core and the
BSP archive.

One-command Build (this checkout)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The machine-specific BSP/toolchain paths are wired in an untracked
``CMakeUserPresets.json`` preset ``agilex5-ebz`` (inherits the tracked
``agilex5`` preset), so:

.. code-block:: bash

   cmake --preset agilex5-ebz
   cmake --build build_agilex5_ebz --target ad9088

What Must Be Kept in Sync Between HDL and no-OS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Everything the app needs about the fabric is in ``system.h`` and ``linker.x``:

.. list-table::
   :header-rows: 1
   :widths: 5 25 35 35

   * - #
     - Handoff item
     - Where no-OS uses it
     - State
   * - 1
     - Peripheral base addresses (``*_BASE``)
     - ``src/platform/altera/parameters.{h,c}``, ``src/common/common_data.c``
       ``.base``
     - Historically hardcoded (reverse-engineered); the values match the EBZ
       ``system.h`` ``*_BASE`` exactly, so they can be sourced from ``system.h``.
   * - 2
     - IRQ numbers (``*_IRQ``), interrupt controller (CLIC)
     - UART/DMA/JESD IRQs
     - Partly hardcoded
   * - 3
     - Memory regions (OCM origin/len, reset)
     - the linker script the app links against
     - Now the EBZ ``linker.x`` via ``ALTERA_BSP_DIR``
   * - 4
     - ISA/ABI (``-march=rv32im_zicbom -mabi=ilp32``)
     - app compile/link flags
     - Set in the Altera toolchain (Nios V branch)
   * - 5
     - CPU clock / mtime base
     - timer/delay drivers
     - ``ALT_CPU_MTIME_OFFSET`` from ``system.h``

Sync Failure Modes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Wrong ``*_BASE`` → SPI/GPIO/DMA read garbage or hang.
- Wrong ``linker.x`` region → app doesn't fit / won't load / fetches from
  unreachable memory (the Nios V instruction master reaches only OCM + dm_agent).
- Wrong ``-march`` → illegal-instruction traps, or missed mul/div/cache ops.
- Wrong IRQ / controller model → interrupts never fire.

Notes Specific to This Design
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Nios V/g is Harvard-like on AXI: the **instruction master reaches only
  ``sys_int_mem`` (OCM) + dm_agent**; the data master reaches everything; DDR is
  data-master-only. The full app therefore has to live in OCM (this design
  enlarges OCM to 2 MB to fit it — see the Qsys carrier tcl).
- Interrupt controller is **CLIC** (not CLINT/PLIC), 48 interrupts.
- Core is ``rv32im`` + Zicbom (4 KB I/D caches, 32 B lines); no FPU → soft-float.
- Software is JTAG-downloaded (``niosv-download``), not booted from the
  bitstream; the reset vector stays in OCM.
