*******************************************
Altera / Nios V Build Prerequisites (CMake)
*******************************************

This guide describes how to build a no-OS project for the **Altera / Intel
FPGA Nios V** platform using the **CMake** build system, which is the only
supported build flow for this platform.

The Nios V flow is a compatibility layer on top of an externally generated
Intel FPGA **BSP** (Board Support Package): no-OS drives the standard Nios V
HAL BSP (``libhal2_bsp.a``) rather than shipping its own UART/interrupt
drivers for this target. The BSP is either generated automatically from a
local Nios V hardware reference design (via the Intel ``niosv-bsp`` tool) or
pointed at directly if you already have one built.

.. note::
   This is a newer, CMake-based Nios V flow and is unrelated to the legacy
   Makefile-based Altera/Nios II flow documented in
   :doc:`build_intel`. The two are not interchangeable.

Supported boards
=================

The following Altera board (preset) is available out of the box:

    .. list-table::
       :header-rows: 1

       * - Preset / Board
         - Target
       * - ``altera``
         - Generic Altera / Nios V (BSP-driven)

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** (the presets use the Ninja generator).
- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed in
  ``tools/scripts/requirements.txt`` (``kconfiglib``, ``pylink-square`` and,
  on Windows, ``windows-curses``).
- A **Nios V GCC cross toolchain**. Unlike the Maxim/STM32/Pico platforms,
  the C, C++ and ASM compilers must each be supplied as a full path —
  ``CMAKE_C_COMPILER``, ``CMAKE_CXX_COMPILER`` and ``CMAKE_ASM_COMPILER``
  (as an environment variable or a ``-D``) — typically the
  ``riscv32-unknown-elf-gcc``/``riscv32-unknown-elf-g++`` pair shipped under
  ``riscfree/toolchain/riscv32-unknown-elf/bin`` in an Intel Quartus/Nios V
  tools install (``CMAKE_ASM_COMPILER`` is normally the same binary as the C
  compiler). A bare compiler name, or a missing one, is rejected with a
  fatal configure error. The linker, ``size``, ``objcopy`` and ``objdump``
  tools are resolved automatically next to the C compiler.
- An Intel Quartus / Nios V tools install providing ``niosv-bsp`` (BSP
  generation) and, for flashing, ``niosv-download``.
- A local **Nios V hardware reference design** (a Quartus project with
  ``hw/`` and ``sw/`` directories) unless you already have a pre-built BSP.

Nios V BSP resolution
----------------------
The build resolves the Nios V BSP via the following environment variables:

    .. list-table::
       :header-rows: 1

       * - Variable
         - Required
         - Description
       * - ``ALTERA_NIOSV_REPO_DIR``
         - Yes (unless ``ALTERA_BSP_DIR`` is set)
         - Path to the Nios V example/reference design (containing ``hw/``
           and ``sw/``). If ``<repo>/sw/bsp`` already contains a generated
           BSP (``system.h`` present), it is used as-is; otherwise a BSP is
           generated there automatically via ``niosv-bsp``.
       * - ``ALTERA_BSP_DIR``
         - No
         - Path to an already-built BSP. When set, this overrides BSP
           generation entirely.
       * - ``ALTERA_NIOSV_HOME``
         - No
         - Nios V tools install directory (``niosv-bsp`` is found in its
           ``bin/`` subdirectory).
       * - ``ALTERA_NIOSV_BSP``
         - No
         - Legacy: a direct path to the ``niosv-bsp`` executable.
       * - ``ALTERA_NIOSV_QPF``
         - No
         - Quartus project file, relative to ``ALTERA_NIOSV_REPO_DIR``
           (default: ``hw/top.qpf``).
       * - ``ALTERA_NIOSV_VDS``
         - No
         - System file, relative to ``ALTERA_NIOSV_REPO_DIR`` (default:
           ``hw/src/vds/qsys_top/qsys_top.vds``).
       * - ``ALTERA_CPU_NAME``
         - No
         - CPU name used only for diagnostic/debug messages.
       * - ``ALTERA_BSP_LIB``
         - No
         - Explicit path to the BSP archive (``libhal2_bsp.a``). Without it,
           the build looks for the archive under the BSP directory itself,
           or under a sibling ``app_pio/build/bsp`` or ``app_hello/build/bsp``
           directory (the layout produced by the Nios V example apps).
       * - ``ALTERA_CFLAGS`` / ``ALTERA_LDFLAGS``
         - No
         - Extra compiler / linker flags appended to the toolchain defaults.

When neither ``ALTERA_BSP_DIR`` nor ``ALTERA_NIOSV_REPO_DIR`` is set, the
configure step still succeeds but reports that the BSP will be resolved
later; the build itself fails once it needs BSP paths it cannot find, so
set one of the two before configuring.

Listing build combinations
===========================

From the no-OS repository root, list the valid combinations and filter to
the Altera board:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list --board altera

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be fed
back to the ``build`` subcommand.

Building a project
====================

Recommended: the build helper
-------------------------------
The simplest way to configure and build is via ``no_os_build.py``, which
selects the right preset, defconfig and board config for you:

    .. code-block:: bash

        $ export ALTERA_NIOSV_REPO_DIR=/path/to/nios_v_reference_design
        $ export CMAKE_C_COMPILER=/path/to/riscv32-unknown-elf-gcc
        $ export CMAKE_CXX_COMPILER=/path/to/riscv32-unknown-elf-g++
        $ export CMAKE_ASM_COMPILER=/path/to/riscv32-unknown-elf-gcc
        $ python tools/scripts/no_os_build.py build \
              --project hello_world --variant altera --board altera

Useful options:

    - ``--clean`` — remove the build directory before configuring.
    - ``-j N`` / ``--jobs N`` — parallel compile jobs.
    - ``--dry-run`` — print the ``cmake`` commands without running them.

Each combination is built into its own directory named
``build/<project>-<variant>-<board>`` at the repo root (override the
location with ``--build-dir``).

.. note::
   ``no_os_build.py``'s ``--flash`` option requires ``--probe {openocd,jlink}``,
   which does not apply to this platform. To flash, build the target first
   and then invoke the ``flash`` target manually, as shown in Flashing below.

Manual CMake invocation
-------------------------
You can also drive CMake directly. Configure with the ``altera`` preset,
select the project/variant with ``PROJECT_DEFCONFIG`` (a path relative to
``projects/`` of the form ``<project>/<variant>.conf``), and pass the
compiler paths explicitly:

    .. code-block:: bash

        $ export ALTERA_NIOSV_REPO_DIR=/path/to/nios_v_reference_design
        $ cmake --preset altera -B build-hello_world-altera-altera \
              -DPROJECT_DEFCONFIG=hello_world/altera.conf \
              -DCMAKE_C_COMPILER=/path/to/riscv32-unknown-elf-gcc \
              -DCMAKE_CXX_COMPILER=/path/to/riscv32-unknown-elf-g++ \
              -DCMAKE_ASM_COMPILER=/path/to/riscv32-unknown-elf-gcc

Then build the project target (the target name is the project directory
name):

    .. code-block:: bash

        $ cmake --build build-hello_world-altera-altera --target hello_world

The build artifact (``hello_world.elf``) is placed in
``build-hello_world-altera-altera/build``.

Configuring options (menuconfig)
-----------------------------------
Kconfig options can be browsed and changed interactively after
configuring:

    .. code-block:: bash

        $ cmake --build build-hello_world-altera-altera --target menuconfig

Flashing
=========

Altera/Nios V does not use OpenOCD or J-Link. Flashing is done through
Intel's own ``niosv-download`` tool, resolved automatically from
``ALTERA_NIOSV_HOME/bin`` or ``PATH`` (override with
``-DNIOSV_DOWNLOAD_PATH=...``):

    .. code-block:: bash

        $ cmake --build build-hello_world-altera-altera --target hello_world
        $ cmake --build build-hello_world-altera-altera --target flash

The following optional CMake variables tune the ``niosv-download``
invocation:

    - ``ALTERA_NIOSV_DOWNLOAD_CPU`` — target CPU index (default: ``1``).
    - ``ALTERA_NIOSV_DOWNLOAD_CABLE`` — JTAG cable name (passed as
      ``--cable=...``).
    - ``ALTERA_NIOSV_DOWNLOAD_DEVICE`` — target device (passed as
      ``--device=...``).
    - ``ALTERA_NIOSV_DOWNLOAD_INSTANCE`` — target instance (passed as
      ``--instance=...``).

.. note::
   There is no GDB-based ``debug``/``debug_server`` target for this
   platform; console output is available over the BSP's JTAG UART instead
   (see the project's own README for the expected serial output).
