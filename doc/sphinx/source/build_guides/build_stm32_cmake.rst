*********************************
STM32 Build Prerequisites (CMake)
*********************************

This guide describes how to build a no-OS project for the STM32 platform using
the **CMake** build system. For the legacy Makefile flow, see
:doc:`build_stm32`.

The CMake build is driven by board *presets* (defined in
``board_configs/stm32/CMakePresets.json``) and project *defconfigs* (Kconfig
fragments inside each project). A small helper script,
``tools/scripts/no_os_build.py``, discovers the valid project/variant/board
combinations and orchestrates the ``cmake`` configure and build steps for you.

Unlike most platforms, STM32 generates its HAL drivers from a CubeMX **.ioc**
file at configure time. The build invokes STM32CubeMX to produce the HAL/CMSIS
sources, startup file and linker script, patches the generated project, and
links it into the no-OS executable. This happens automatically — you only need
a valid ``.ioc`` for the board.

Supported boards
================

The following STM32 boards (presets) are available out of the box:

    .. list-table::
       :header-rows: 1

       * - Preset / Board
         - Target chip
       * - ``stm32f756_nucleo``
         - STM32F756 (Cortex-M7)
       * - ``sdp-ck1z``
         - stm32f469 (Cortex-M4)
       * - ``adin1110ebz``
         - STM32L4S5 (Cortex-M4)

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** (the presets use the Ninja generator).
- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed in
  ``tools/scripts/requirements.txt`` (``kconfiglib``, ``pylink-square`` and,
  on Windows, ``windows-curses``).
- **STM32CubeIDE** — provides the ARM cross toolchain
  (``arm-none-eabi-gcc``) and the OpenOCD used for flashing and debugging.
- **STM32CubeMX** — used to generate the HAL drivers from the project ``.ioc``
  file.

Install the STM32 tools
-----------------------

Install both `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_
and `STM32CubeMX <https://www.st.com/en/development-tools/stm32cubemx.html>`_.

The build locates them automatically in the common install locations
(``/opt/st``, ``$HOME/st``, ``C:/ST``, ``$ProgramFiles/STMicroelectronics``,
``/Applications`` on macOS, etc.). If they are installed elsewhere, point the
build at them with environment variables:

    .. code-block:: bash

        # Linux / macOS — set to the install location
        export STM32CUBEMX=/path/to/STM32CubeMX
        export STM32CUBEIDE=/path/to/stm32cubeide

    .. code-block:: bash

        # Windows (Git Bash)
        export STM32CUBEMX=/c/ST/STM32CubeMX
        export STM32CUBEIDE=/c/ST/STM32CubeIDE

.. note::
   The toolchain and OpenOCD are taken from the GCC and OpenOCD bundled inside
   STM32CubeIDE, so a separate ARM toolchain install is not required. If a
   bundled OpenOCD is not found, a system ``openocd`` on ``PATH`` is used as a
   fallback.

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
STM32 boards:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list --board sdp-ck1z

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
              --project iio_demo --variant iio --board sdp-ck1z

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

        $ cmake -B iio_demo --preset sdp-ck1z \
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

The .ioc file
-------------
The HAL drivers are generated from a CubeMX ``.ioc`` file. By default the build
looks for ``projects/<project>/<board>.ioc``. To use a different file, set the
``CONFIG_STM32_IOC_PATH`` Kconfig option (relative paths are resolved from the
project directory):

    .. code-block:: bash

        $ cmake -B iio_demo --preset sdp-ck1z \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf \
              -DCONFIG_STM32_IOC_PATH=sdp-ck1z-usb-cdc-acm.ioc

The CubeMX project is regenerated only when the ``.ioc`` file or the CubeMX
executable changes (tracked via a stamp file), so subsequent builds are fast.

Configuring options (menuconfig)
--------------------------------
Kconfig options can be browsed and changed interactively after configuring:

    .. code-block:: bash

        $ cmake --build iio_demo --target menuconfig

Flashing and Debugging
======================

Flashing requires a debug probe to be selected at **configure** time via
``-DPROBE=...``. Two probes are supported:

    - ``openocd`` (default) — uses the OpenOCD bundled with STM32CubeIDE.
    - ``jlink`` — uses a SEGGER J-Link via the ``pylink-square`` Python package.

Configure with the desired probe, build, then run the ``flash`` target:

    .. code-block:: bash

        $ cmake -B iio_demo --preset sdp-ck1z \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf -DPROBE=openocd
        $ cmake --build iio_demo --target iio_demo
        $ cmake --build iio_demo --target flash

Or, with the helper script in one step:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py build \
              --project iio_demo --variant iio --board sdp-ck1z \
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

The OpenOCD interface and target configuration are derived automatically from
the ``.ioc`` file (MCU family and part number), falling back to the preset's
``TARGET`` when the ``.ioc`` is unavailable.

Windows
=======

The CMake flow works under Git Bash. Install STM32CubeIDE and STM32CubeMX, then
export ``STM32CUBEIDE`` and ``STM32CUBEMX`` if they are not in a default
location (see above). Make sure ``cmake``, ``ninja`` and ``python`` are
available on ``PATH`` in your Git Bash session, then follow the same
configure/build/flash steps as on Linux.
