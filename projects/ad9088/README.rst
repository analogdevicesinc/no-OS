ad9088 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Devices
-----------------

* `AD9084 <https://www.analog.com/AD9084>`_ (and the AD9088 variant of the
  Apollo MxFE family)

The device is driven through the vendor "Apollo" API, vendored under the
``ad9088`` driver (see `Driver Layout`_).

Supported Carriers
------------------

* **AD9084-EBZ on Intel Agilex 5** (Agilex 5E065B Premium Development Kit,
  Nios V/g RV32 soft core) — the primary target, a JESD204B design.
* `VCU118 <https://www.xilinx.com/VCU118>`_ (MicroBlaze) — a JESD204C design.

Overview
--------

The AD9084/AD9088 is a mixed-signal front-end (MxFE) combining wideband
ADCs, DACs and digital up/down conversion with a JESD204B/C serial data
interface. This project brings the device up on an FPGA carrier: it
configures the on-board clock chain, loads the device use-case profile and
signed core firmware, and runs the JESD204 link to ``DATA``/LINK_RUNNING.

The clock chain on the evaluation board is:

::

   ADF4382 (device-clock PLL) ─► HMC7044 (clock distribution) ─► ADF4030 (SYSREF / one-PPS sync)

all on a shared clock-domain SPI bus, followed by the AD9084/AD9088 itself
on its own SPI bus, and the JESD204 datapath (link cores, transport layers,
DMAs and the transceiver PHY) in the FPGA fabric.

The same source tree builds for two very different carriers; every value
that differs between them (JESD mode, lane rate, clock-tree dividers,
firmware set) is selected per board (see `Use-Case Profiles`_) and gated at
compile time on ``CONFIG_ALTERA_PLATFORM_NIOSV`` so the Intel and Xilinx
builds never disturb each other.

Driver Layout
-------------

The Apollo API and the ad9088 glue live under:

::

   no-OS/drivers/rf-transceiver/ad9088/
   ├── ad9088.c              # top-level device driver
   ├── ad9088.h
   ├── ad9088_dt.c           # device-tree-style default configuration
   ├── ad9088_jesd204_fsm.c  # JESD204 FSM integration
   ├── ad9088_mcs.c          # multi-chip sync helpers
   ├── ad9088_runtime.c      # runtime control (NCO, etc.)
   ├── ad9088_firmware.cmake # embeds the profile + signed core firmware in the ELF
   ├── firmware/             # use-case profiles and signed firmware blobs
   ├── public/               # vendor Apollo API (public headers + sources)
   ├── private/              # vendor Apollo API (private)
   ├── adi_inc/  adi_utils/  # vendor support code
   └── ...

The clock-chain devices use the standard no-OS drivers:
``drivers/frequency/adf4382``, ``.../hmc7044`` and ``.../adf4030``.

Use-Case Profiles
-----------------

The device profile describes the datapath (JESD204 mode, converter counts,
sample rates) and **must match the FPGA bitstream on the board**. Because the
bitstream fixes the JESD mode, the profile is selected per board rather than
globally.

* ``CONFIG_AD9088_PROFILE`` — the use-case profile image embedded in the ELF.
  A bare file name is taken relative to
  ``drivers/rf-transceiver/ad9088/firmware/``; an absolute path is used as
  given, so a profile generated outside the tree also works. Whichever image
  is picked, it is embedded under the fixed name ``usecase.bin`` so the
  symbols the driver looks for never change.

  * AD9084-EBZ (Agilex 5): ``204B_L2_M4_NP16_16p0_16x4.bin`` (JESD204B, per-link
    M4/L2 ×2, 10 Gbps lanes, 250 MHz link clock).
  * VCU118: ``204C_M4_L8_NP16_20p0_4x2.bin`` (JESD204C, 20 Gbps lanes).

* ``CONFIG_AD9088_FW_SET`` — which signed core-firmware set(s) to embed. Each
  set is ~570 KB; embedding both needs over 1 MB of contiguous memory for
  firmware alone, which small soft-SoC targets (Nios V on-chip memory) do not
  have. The vendor API requests one set based on the detected silicon, so the
  choice is **functional** — a mismatch fails startup with
  ``Unknown firmware ID``.

  * ``AD9088_FW_SET_ALL`` (default) — engineering + production (used by the
    Xilinx build).
  * ``AD9088_FW_SET_ENG`` — engineering silicon only
    (``app_signed_encrypted_B``); selected by the Agilex 5 board config, which
    assumes the AD9084-EBZ carries engineering silicon.
  * ``AD9088_FW_SET_PROD`` — production silicon only.

The per-board selections live in
``projects/ad9088/boards/<variant>/<board>.conf``.

No-OS Supported Examples
------------------------

Two example variants are provided, selected by ``CONFIG_AD9088_EXAMPLE``
(``basic_example`` / ``dma_example``).

Basic Example
~~~~~~~~~~~~~

The basic example (variant ``basic_example``) brings the board up: it
initializes the clock chain (ADF4382 → HMC7044 → ADF4030), loads the device
profile and core firmware over SPI, and runs the JESD204 link through to
LINK_RUNNING, reporting the TX/RX link status.

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) exercises the datapath: it captures
from the ADCs and plays back through the DACs over AXI DMA, including an NCO
test tone and cabled DAC-to-ADC loopback coherence checks. As with other
no-OS DMA examples, the received data is left at a known memory address that
the application prints, so it can be retrieved and plotted. See the
`DAC DMA example wiki <https://wiki.analog.com/resources/no-os/dac_dma_example>`_
for background.

.. note::

   The DMA example currently ships a board config for VCU118 only
   (``boards/dma_example/vcu118.conf``); there is no ``agilex5.conf`` for it
   yet. Building it for Agilex 5 requires adding that board config (selecting
   the 204B profile and the engineering firmware set, as the basic example
   does) and validating the datapath addresses against the Agilex 5 bitstream.

No-OS Supported Platforms
-------------------------

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
``niosv-bsp`` → no-OS — is documented in
`docs/hdl_noos_build_flow.md <docs/hdl_noos_build_flow.md>`_. Read it first;
the paragraphs below assume the BSP already exists.

.. note::

   After any hand-edit to the HDL Qsys ``.tcl`` (e.g. on-chip-memory size),
   do a clean HDL rebuild (``make clean && make``) so the generated Qsys is
   fully regenerated, then regenerate the BSP. A stale partial regeneration
   can produce a bitstream that behaves differently from the source.

Build Command
^^^^^^^^^^^^^

The Intel build uses CMake presets. The tracked base preset ``agilex5``
(in ``board_configs/altera/CMakePresets.json``) sets the platform, board and
toolchain file. Machine-specific paths — the BSP directory, the BSP archive
and the RISC-V compilers — belong in an **untracked** ``CMakeUserPresets.json``
that inherits ``agilex5`` (see the ``agilex5-ebz`` example preset), so absolute
paths never land in the tracked tree:

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

``MinSizeRel`` is required so the image fits the Nios V on-chip memory. The
board config (``boards/basic_example/agilex5.conf``) selects the 204B profile
and the engineering firmware set.

.. code-block:: bash

   cd no-OS
   export ALTERA_PLATFORM_NIOSV=1

   # Configure and build the ad9088 ELF against the EBZ BSP
   cmake --preset agilex5-ebz
   cmake --build build_agilex5_ebz --target ad9088

   # Output ELF: build_agilex5_ebz/build/ad9088

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

Xilinx (VCU118, MicroBlaze)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `VCU118 Evaluation Kit <https://www.xilinx.com/VCU118>`__
* AD9084-EBZ evaluation board
* USB-UART for the serial console at 115200 baud, 8N1

Build Command
^^^^^^^^^^^^^

The Xilinx build uses the CMake/Ninja build system via the ``no_os_build.py``
helper and requires a hardware description (``.xsa``) exported from the HDL
design. The VCU118 bitstream is the JESD204C (204C) use case.

.. code-block:: bash

   # Source the Vitis toolchain environment
   source ~/.xilinx/2025.1/Vitis/settings64.sh

   cd no-OS

   # Build the basic example for VCU118
   python tools/scripts/no_os_build.py build \
       --project ad9088 --variant basic_example --board vcu118 \
       --hardware /path/to/system_topad9084_ebz_vcu118_np16.xsa

   # Build the DMA example for VCU118
   python tools/scripts/no_os_build.py build \
       --project ad9088 --variant dma_example --board vcu118 \
       --hardware /path/to/system_topad9084_ebz_vcu118_np16.xsa

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.
