*********************************
Maxim Build Prerequisites (CMake)
*********************************

This guide describes how to build a no-OS project for the Maxim platform using
the **CMake** build system. For the legacy Makefile flow, see
:doc:`build_maxim`.

The CMake build is driven by board *presets* (defined in
``board_configs/maxim/CMakePresets.json``) and project *defconfigs* (Kconfig
fragments inside each project). A small helper script,
``tools/scripts/no_os_build.py``, discovers the valid project/variant/board
combinations and orchestrates the ``cmake`` configure and build steps for you.

Supported boards
================

The following Maxim boards (presets) are available out of the box. All presets
use the vendor toolchain bundled with the Maxim SDK:

    .. list-table::
       :header-rows: 1

       * - Preset / Board
         - Target chip
       * - ``max78000_fthr``
         - max78000
       * - ``max32650_fthr``
         - max32650
       * - ``max32655_fthr``
         - max32655
       * - ``apard32690``
         - max32690
       * - ``ad-swiot1l-sl``
         - max32650

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** (the presets use the Ninja generator).
- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed in
  ``tools/scripts/requirements.txt`` (``kconfiglib``, ``pylink-square`` and,
  on Windows, ``windows-curses``).
- An **ARM cross toolchain** (``arm-none-eabi-gcc``). All Maxim presets use the
  vendor toolchain bundled with the Maxim SDK, so no separate toolchain install
  is required.
- The **Maxim SDK** (see below), which also provides OpenOCD for flashing and
  debugging.

Install the Maxim SDK
---------------------
The Maxim Microcontrollers SDK (MSDK) contains the register/startup files,
linker scripts, vendor toolchain and OpenOCD needed to build and flash firmware
for the MAX32xxx and MAX78xxx microcontrollers.

The build system locates the SDK in one of two ways (checked in this order):

1. **CodeFusion Studio (CFS)** — pass ``-DCFS_PATH=/path/to/cfs``, set the
   ``CFS_PATH`` environment variable, or place it at ``~/cfs``. When found, the
   libraries, vendor toolchain and OpenOCD are taken from inside the CFS tree.
2. **MAXIM_LIBRARIES** — set this environment variable to the
   ``MaximSDK/Libraries`` path:

    .. code-block:: bash

        # Linux (default install path)
        export MAXIM_LIBRARIES=~/MaximSDK/Libraries

    .. code-block:: bash

        # Windows (Git Bash), SDK installed to C:\MaximSDK
        export MAXIM_LIBRARIES=/c/MaximSDK/Libraries

If neither a CFS install nor ``MAXIM_LIBRARIES`` is found, configuration fails
with ``MAXIM_LIBRARIES is not set. Point it at the Maxim SDK Libraries
directory ...``.

- (Optional) For visual debugging, install Visual Studio Code and the
  Cortex-Debug extension. IDE project files are generated automatically as part
  of the build, including a ``no-os.code-workspace`` file at the repository
  root.

  .. important::

     To build, debug and run a project from VS Code you **must** open the
     generated ``no-os.code-workspace`` file, not just the no-OS folder. The
     debug launch configuration and include paths live in that workspace;
     opening the folder on its own will not pick them up. Open it with:

     .. code-block:: bash

         $ code no-os.code-workspace

     or from VS Code via ``File > Open Workspace from File...``. The build
     utility can also open it for you after a successful build by passing
     ``--open`` to ``no_os_build.py build``.

  VS Code does not always prompt to open the workspace on its own (the
  notification is shown at most once per folder and is suppressed once
  dismissed), so open it explicitly as shown above.

Listing build combinations
==========================

From the no-OS repository root, list the valid combinations for the Maxim
platform:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list

You can filter by project, variant or board:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list --project iio_demo
        $ python tools/scripts/no_os_build.py list --board max78000_fthr

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be fed back
to the ``build`` subcommand.

Building a project
==================

Recommended: the build helper
-----------------------------
The simplest way to configure and build is via ``no_os_build.py``, which selects
the right preset, defconfig and board config for you:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py build \
              --project iio_demo --variant iio --board max78000_fthr

Useful options:

    - ``--clean`` — remove the build directory before configuring.
    - ``-j N`` / ``--jobs N`` — parallel compile jobs.
    - ``--parallel`` — build different boards in parallel.
    - ``--probe {openocd,jlink}`` — select the debug probe (needed for flashing).
    - ``--flash`` — flash after a successful build (requires ``--probe``).
    - ``--dry-run`` — print the ``cmake`` commands without running them.

Each combination is built into its own directory named
``build-<project>-<variant>-<board>`` at the repo root (override the location
with ``--build-dir``). The build artifacts (``.elf``, ``.hex``, ``.bin``) are placed
in ``<build-dir>/build``.

Manual CMake invocation
-----------------------
You can also drive CMake directly. Configure with the board preset and select
the project/variant with ``PROJECT_DEFCONFIG``:

    .. code-block:: bash

        $ cmake -B iio_demo --preset max78000_fthr \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf

Then build the project target (the target name is the project directory name):

    .. code-block:: bash

        $ cmake --build iio_demo --target iio_demo

.. note::
   ``PROJECT_DEFCONFIG`` is a path relative to the ``projects/`` directory of
   the form ``<project>/<variant>.conf``. When the project provides board
   variants under ``projects/<project>/boards/<variant>/<board>.conf``, the
   matching board config is layered on top automatically based on the preset's
   ``BOARD`` value.

.. note::
   If no ``CMAKE_BUILD_TYPE`` is given, the build defaults to
   ``RelWithDebInfo`` so the ELF contains debug info. Use
   ``-DCMAKE_BUILD_TYPE=Debug`` for an unoptimized build, or ``MinSizeRel`` /
   ``Release`` for size/speed.

Configuring options (menuconfig)
--------------------------------
Kconfig options can be browsed and changed interactively after configuring:

    .. code-block:: bash

        $ cmake --build iio_demo --target menuconfig

Flashing and Debugging
======================

Flashing requires a debug probe to be selected at **configure** time via
``-DPROBE=...``. Two probes are supported:

    - ``openocd`` (default) — uses the OpenOCD that ships with the Maxim SDK.
    - ``jlink`` — uses a SEGGER J-Link via the ``pylink-square`` Python package.

Configure with the desired probe, build, then run the ``flash`` target:

    .. code-block:: bash

        $ cmake -B iio_demo --preset max78000_fthr \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf -DPROBE=openocd
        $ cmake --build iio_demo --target iio_demo
        $ cmake --build iio_demo --target flash

Or, with the helper script in one step:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py build \
              --project iio_demo --variant iio --board max78000_fthr \
              --probe openocd --flash

When the OpenOCD probe is selected, the following additional targets are
available:

    - ``flash`` — program the ``.elf`` to the target and reset.
    - ``erase`` — erase the device flash.
    - ``debug`` — start OpenOCD and attach GDB (Linux/macOS).
    - ``debug_server`` — start OpenOCD and wait for a GDB connection. On
      Windows, run ``debug_server`` in one terminal and ``debug_gdb`` in
      another.

    .. code-block:: bash

        $ cmake --build iio_demo --target debug

To debug graphically from VS Code, open the generated ``no-os.code-workspace``
(see the install step above) and launch the debug configuration from there;
opening the no-OS folder without the workspace will not load the debug setup.

Windows
=======

The CMake flow works under Git Bash. Install the Maxim SDK to a path without
spaces (e.g. ``C:\MaximSDK``) and export ``MAXIM_LIBRARIES`` as shown above.
Make sure ``cmake``, ``ninja`` and ``python`` are available on ``PATH`` in your
Git Bash session, then follow the same configure/build/flash steps as on Linux.
