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
  Nios V/g RV32 soft core) — the primary target, a JESD204C design.
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

  * AD9084-EBZ (Agilex 5): ``204C_L2_M4_NP16_10p8_8x8_8x4.bin`` (JESD204C,
    per-link M4/L2, 11.1375 Gbps lanes, 168.75 MHz link clock, 10.8 GHz device
    clock, 337.5 MSPS).
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
set are selected in ``boards/dma_example/<board>.conf`` (the same 204C profile
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
board config (``boards/basic_example/agilex5.conf``) selects the 204C profile
and the engineering firmware set.

To build a different variant, point ``PROJECT_DEFCONFIG`` at its config — e.g.
``ad9088/iio_example.conf`` for the IIO server (add an ``agilex5-iio`` preset
that inherits ``agilex5``, or override ``-DPROJECT_DEFCONFIG`` on the command
line). The IIO variant links the same way; if the BSP archive is not
auto-detected (it lives in a ``build/`` subdirectory), pass it explicitly with
``-DALTERA_BSP_LIB=/path/to/.../software/bsp/build/libhal2_bsp.a``.

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
