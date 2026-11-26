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

  * AD9084-EBZ (Agilex 5): ``204B_M4_L2_NP16_21p6_10p8_8x8_8x4.bin``
    (JESD204B, per-link M4/L2, 13.5 Gbps lanes, 337.5 MHz link clock, 10.8 GHz
    device clock, 337.5 MSPS).
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

Three example variants are provided, selected by ``CONFIG_AD9088_EXAMPLE``
(``basic_example`` / ``dma_example`` / ``iio_example``). The ``iio_example``
variant is Agilex 5 / Nios V only (see below).

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

The DMA example builds for both carriers; the per-board profile and firmware
set are selected in ``boards/dma_example/<board>.conf`` (the same 204B profile
and engineering firmware set as the basic example on Agilex 5).

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio_example``) runs the same clock/JESD204 bring-up
as the DMA example, then starts an IIOD server that exposes the AXI ADC
(``axi_adc``) and AXI DAC (``axi_dac``) over the IIO protocol, so a host can
drive them with ``iio_info`` / ``iio_readdev`` or the IIO oscilloscope instead
of the fixed capture-and-park flow. The RX data offload is re-armed before each
capture and the TX data offload is put in bypass so the DAC replays the host's
buffer.

This variant is **Agilex 5 / Nios V only**: the IIOD transport is the Altera
JTAG-UART plus the Nios V core-local (CLIC) interrupt controller, provided by
the ``UART_ALTERA`` / ``IRQ_ALTERA`` drivers. A Xilinx build refuses at compile
time (``#error``). On top of the DMA example's config it enables ``CONFIG_IIO``,
``CONFIG_AXI_CORE_IIO_AXI_ADC``, ``CONFIG_AXI_CORE_IIO_AXI_DAC``,
``CONFIG_UART_ALTERA`` and ``CONFIG_IRQ_ALTERA`` (see
``projects/ad9088/iio_example.conf`` and ``boards/iio_example/agilex5.conf``).

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
``ad9088/iio_example.conf`` for the IIO server (add an ``agilex5-iio`` preset
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
   source /path/to/Vitis/2025.1/settings64.sh

   cd no-OS

   # Build the basic example for VCU118
   python tools/scripts/no_os_build.py build \
       --project ad9088 --variant basic_example --board vcu118 \
       --hardware /path/to/system_topad9084_ebz_vcu118_np16.xsa

   # Build the DMA example for VCU118
   python tools/scripts/no_os_build.py build \
       --project ad9088 --variant dma_example --board vcu118 \
       --hardware /path/to/system_topad9084_ebz_vcu118_np16.xsa

The ``iio_example`` variant is not available on VCU118: its IIOD transport is
Altera/Nios V specific, so a Xilinx build refuses at compile time.

For toolchain setup and prerequisites, see the
:doc:`Xilinx CMake build guide </build_guides/build_xilinx_cmake>`.

Host simulation (Linux PC, no board)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Agilex 5 build of all three examples can also run as an ordinary Linux program,
with the board replaced by software models in ``src/sim/``. It runs the real
drivers, vendor API, profile and firmware images, and is useful to check
software changes before hardware, or without it. It is enabled with
``-DAD9088_SIM=ON`` (OFF by default; the Nios V and Xilinx builds are
unchanged).

What the models do:

* **AD9084:** the SPI slave with its direct, paged and SPIDMA access paths over
  a sparse memory, so the four firmware images and the profile really load.
  Secure boot, the Core1 boot steps, the mailbox (ping echoes, calibrations
  succeed), multi-chip sync and the JESD204 link status respond the way a
  healthy device does.
* **Clock chips** (ADF4382, HMC7044, ADF4030): register memories that report
  their ID and report locked.
* **FPGA cores:** JESD204 link layer in DATA with SYSREF aligned, TPL cores
  ready, and the two DMACs as a digital loopback: the RX capture is exactly
  the transmit buffer.
* **IIO transport:** on the host the IIO example serves IIOD over TCP (port
  30431) instead of the JTAG-UART, so IIO clients connect to
  ``ip:127.0.0.1``.

The status values are fixed, not derived from the configuration, so the
simulation does not catch a misconfigured link, and there is no converter,
NCO or analog model.

Used Hardware
^^^^^^^^^^^^^

* An x86-64 Linux PC (native or WSL2) with GCC and 32-bit (i386) multilib
  support. The build is 32-bit so the FPGA register windows can be mapped at
  their real addresses.

Build Command
^^^^^^^^^^^^^

``no_os_build.py`` has no way to pass ``-DAD9088_SIM=ON``, so configure with
CMake directly, using the ``native-i386`` preset:

.. code-block:: bash

   sudo apt install gcc-multilib g++-multilib

   cd no-OS

   # Configure and build the basic example
   cmake -B build-sim/ad9088-basic_example-native-i386 --preset native-i386 \
         -DPROJECT_DEFCONFIG=ad9088/basic_example.conf -DAD9088_SIM=ON
   cmake --build build-sim/ad9088-basic_example-native-i386 --target ad9088

   # Configure and build the DMA example
   cmake -B build-sim/ad9088-dma_example-native-i386 --preset native-i386 \
         -DPROJECT_DEFCONFIG=ad9088/dma_example.conf -DAD9088_SIM=ON
   cmake --build build-sim/ad9088-dma_example-native-i386 --target ad9088

   # Configure and build the IIO example
   cmake -B build-sim/ad9088-iio_example-native-i386 --preset native-i386 \
         -DPROJECT_DEFCONFIG=ad9088/iio_example.conf -DAD9088_SIM=ON
   cmake --build build-sim/ad9088-iio_example-native-i386 --target ad9088

The preset also checks the C++ compiler, which fails without the 32-bit C++
runtime (``g++-multilib``). The project compiles no C++, so if you cannot
install it, add ``-DCMAKE_CXX_COMPILER_WORKS=TRUE`` to the configure command.

Run
^^^

.. code-block:: bash

   ./build-sim/ad9088-basic_example-native-i386/build/ad9088.elf

The console output matches the board's, plus lines starting with ``SIM:`` that
log the models' SPI, mailbox and DMA activity; filter them with
``| grep -v '^SIM:'``. The basic example ends after its NCO test and exits 0.

The DMA example, as on the board, parks in its ``while (1)`` after the
capture; stop it with Ctrl-C once ``DMA_EXAMPLE Rx side 0`` is printed. With
``AD9088_SIM_CAPTURE`` set, it writes the RX buffer to that file and the TX
buffer to the same path plus ``.tx``. Plot them with the same script as a board
capture (``tools/scripts/platform/altera/plot_loopback.py``), after
splitting each into the per-channel CSVs that script reads with
``scripts/sim_capture_to_csv.py`` from this project:

.. code-block:: bash

   AD9088_SIM_CAPTURE=/tmp/sim_cap.bin \
       ./build-sim/ad9088-dma_example-native-i386/build/ad9088.elf

   CONV=projects/ad9088/scripts/sim_capture_to_csv.py
   python3 $CONV /tmp/sim_cap.bin    /tmp/sim_plot/rx
   python3 $CONV /tmp/sim_cap.bin.tx /tmp/sim_plot/tx
   python3 tools/scripts/platform/altera/plot_loopback.py \
           --tx /tmp/sim_plot/tx --rx /tmp/sim_plot/rx \
           --out /tmp/sim_plot/loopback.png

Expect RX identical to TX: 0 deg phase and 0 dB gain on all 8 converters.

The IIO example brings the links up, then serves IIOD; it is ready once
``Project configured, starting IIOD`` is printed. From a second terminal:

.. code-block:: bash

   ./build-sim/ad9088-iio_example-native-i386/build/ad9088.elf   # terminal 1

   iio_info -u ip:127.0.0.1          # lists axi_adc and axi_dac
   osc -u ip:127.0.0.1               # or IIO Oscilloscope, connect to 127.0.0.1

The ADC captures what the DAC was last given, so push a transmit buffer first,
either from osc's DAC Data Manager or with ``iio_writedev``, then capture:

.. code-block:: bash

   # tx.bin: 8 interleaved little-endian int16 channels, 4096 samples;
   # -c keeps it cyclic, -b is the waveform length in samples
   iio_writedev -u ip:127.0.0.1 -c -b 4096 axi_dac < tx.bin &
   iio_readdev  -u ip:127.0.0.1 -s 4096 axi_adc > rx.bin

``iio_readdev`` from libiio 1.0 writes a one-line "compatibility layer" notice
to standard output before the data; drop it (``tail -c <bytes>``) when
comparing files. With nothing transmitted the capture is all zeros. The
DAC's DDS tone generator is not modelled, so DDS settings do not appear in the
capture.

``iio_writedev`` sends its ``-b`` buffer size (default 256 samples) as the
cyclic waveform, so pass ``-b`` with the number of samples in the file to
play all of it; the capture then repeats the whole file.

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
