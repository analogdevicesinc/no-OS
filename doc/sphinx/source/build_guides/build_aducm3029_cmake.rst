*************************************
ADuCM3029 Build Prerequisites (CMake)
*************************************

This guide describes how to build a no-OS project for the ADuCM3029 platform
using the **CMake** build system.

The CMake build is driven by board *presets* (defined in
``board_configs/aducm3029/CMakePresets.json``) and project *defconfigs* (Kconfig
fragments inside each project). A small helper script,
``tools/scripts/no_os_build.py``, discovers the valid project/variant/board
combinations and orchestrates the ``cmake`` configure and build steps for you.

Unlike most platforms, ADuCM3029 generates its project startup, pin
multiplexing, RTE configuration and linker script from CrossCore Embedded
Studio (CCES) at configure time. The build invokes the CCES headless tools to
create the project, regenerate the device sources, and links them into the
no-OS executable. This happens automatically — you only need a valid
``pinmux_config.c`` for the board.

Supported boards
================

The following ADuCM3029 boards (presets) are available out of the box:

    .. list-table::
       :header-rows: 1

       * - Preset / Board
         - Target chip
       * - ``eval-adicup3029``
         - ADuCM3029 (Cortex-M3)

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** (the presets use the Ninja generator).
- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed in
  ``tools/scripts/requirements.txt`` (``kconfiglib``, ``pylink-square`` and,
  on Windows, ``windows-curses``).
- **CrossCore Embedded Studio (CCES)** — provides the ARM cross toolchain
  (``arm-none-eabi-gcc``), the OpenOCD used for flashing and debugging, and the
  headless tools used to generate the ADuCM3029 project.
- **ADuCM302x Device Family Pack** and the **ARM.CMSIS pack**, installed via the
  CCES pack manager.

Install the ADuCM3029 tools
---------------------------

Install `CrossCore Embedded Studio
<https://www.analog.com/en/resources/evaluation-hardware-and-software/software/adswt-cces.html>`_,
then install the **ADuCM302x Device Family Pack** and the **ARM.CMSIS pack**
through the CCES pack manager.

.. important::
   Make sure you don't have multiple versions of the ADuCM302x DFP and ARM CMSIS
   packs installed.

The build locates CCES automatically under the common install locations
(``/opt/analog/cces/*`` on Linux, ``C:/Analog Devices/CrossCore Embedded
Studio*`` on Windows), selecting the highest version found. If it is installed
elsewhere, or you want to pin a specific version, point the build at it with the
``CCES_HOME`` environment variable:

    .. code-block:: bash

        # Linux
        export CCES_HOME=/opt/analog/cces/3.0.3

    .. code-block:: bash

        # Windows (Git Bash) — install to a path without whitespaces
        export CCES_HOME=/c/ADI/cces3.0.3

You can also pass it explicitly to CMake with ``-DCCES_HOME=...``.

.. note::
   The ARM toolchain and OpenOCD are taken from the GCC and OpenOCD bundled
   inside CCES, so a separate ARM toolchain install is not required. To use a
   compiler already on ``PATH`` instead of the CCES-bundled one, configure with
   ``-DUSE_VENDOR_TOOLCHAIN=OFF``.

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

From the no-OS repository root, list the valid combinations and filter to the
ADuCM3029 boards:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list --board eval-adicup3029

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
              --project iio_demo --variant iio --board eval-adicup3029

Useful options:

    - ``--clean`` — remove the build directory before configuring.
    - ``-j N`` / ``--jobs N`` — parallel compile jobs.
    - ``--parallel`` — build different boards in parallel.
    - ``--probe {openocd,jlink}`` — select the debug probe (needed for flashing).
    - ``--flash`` — flash after a successful build (requires ``--probe``).
    - ``--dry-run`` — print the ``cmake`` commands without running them.

Each combination is built into its own directory named
``build-<project>-<variant>-<board>`` at the repo root (override the location
with ``--build-dir``). The build artifacts (``.elf``, ``.hex``, ``.bin``) are
placed in ``<build-dir>/build``.

Manual CMake invocation
-----------------------
You can also drive CMake directly. Configure with the board preset and select
the project/variant with ``PROJECT_DEFCONFIG``:

    .. code-block:: bash

        $ cmake -B iio_demo --preset eval-adicup3029 \
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

The pinmux_config.c file
------------------------
The ADuCM3029 project sources are generated by CCES from a ``pinmux_config.c``
"hardware" file, which describes the pin multiplexing. By default the build
looks for ``projects/<project>/pinmux_config.c``. To use a different file, set
the ``CONFIG_ADUCM_PINMUX_PATH`` Kconfig option (relative paths are resolved
from the project directory):

    .. code-block:: bash

        $ cmake -B iio_demo --preset eval-adicup3029 \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf \
              -DCONFIG_ADUCM_PINMUX_PATH=pinmux_config.c

The CCES project is regenerated only when the ``pinmux_config.c`` file or the
CCES installation changes (tracked via a stamp file), so subsequent builds are
fast.

Configuring options (menuconfig)
--------------------------------
Kconfig options can be browsed and changed interactively after configuring:

    .. code-block:: bash

        $ cmake --build iio_demo --target menuconfig

Flashing and Debugging
======================

Flashing requires a debug probe to be selected at **configure** time via
``-DPROBE=...``. Two probes are supported:

    - ``openocd`` (default) — uses the OpenOCD bundled with CCES.
    - ``jlink`` — uses a SEGGER J-Link via the ``pylink-square`` Python package.

Configure with the desired probe, build, then run the ``flash`` target:

    .. code-block:: bash

        $ cmake -B iio_demo --preset eval-adicup3029 \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf -DPROBE=openocd
        $ cmake --build iio_demo --target iio_demo
        $ cmake --build iio_demo --target flash

Or, with the helper script in one step:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py build \
              --project iio_demo --variant iio --board eval-adicup3029 \
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

.. note::
   The ADuCM3029 OpenOCD target configuration (``target/aducm3029.cfg``) ships
   with the ADuCM302x Device Family Pack. The build adds the DFP's OpenOCD
   scripts directory to the search path automatically, so no extra setup is
   needed.

Windows
=======

The CMake flow works under Git Bash. Install CrossCore Embedded Studio (to a
path without whitespaces) and the ADuCM302x DFP / ARM.CMSIS packs, then export
``CCES_HOME`` if it is not in a default location (see above). Make sure
``cmake``, ``ninja`` and ``python`` are available on ``PATH`` in your Git Bash
session, then follow the same configure/build/flash steps as on Linux.
