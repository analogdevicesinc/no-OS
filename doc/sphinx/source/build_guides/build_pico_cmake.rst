*********************************************
Raspberry Pi Pico Build Prerequisites (CMake)
*********************************************

This guide describes how to build a no-OS project for the Raspberry Pi Pico
(RP2040) platform using the **CMake** build system, which is the only supported
build flow for the Pico platform.

The CMake build is driven by board *presets* (defined in
``board_configs/pico/CMakePresets.json``) and project *defconfigs* (Kconfig
fragments inside each project). The ``rpi-pico`` preset selects the board and
toolchain, while ``PROJECT_DEFCONFIG`` selects the project and variant to
build.

The Pico SDK is fetched automatically at configure time (cloned once, in-tree
under ``libraries/pico-sdk``), so no external SDK install or submodule init is
required. no-OS
uses the Pico SDK's own CMake (``pico_sdk_init()``) to provide the SDK sources,
headers, the second-stage bootloader, the linker script and the libc/soft-float
wrapping, linking them into the no-OS executable automatically.

.. note::
   Pico platform builds are supported on Linux only. The build targets the
   Raspberry Pi Pico (RP2040).

Supported boards
================

The following Pico board (preset) is available out of the box:

    .. list-table::
       :header-rows: 1

       * - Preset / Board
         - Target chip
       * - ``rpi-pico``
         - rp2040

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** (the presets use the Ninja generator).
- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed in
  ``tools/scripts/requirements.txt`` (``kconfiglib``, ``pylink-square`` and,
  on Windows, ``windows-curses``).
- An **ARM cross toolchain** (``arm-none-eabi-gcc``) available on ``PATH``.
  Unlike the Maxim and STM32 platforms, no vendor toolchain is used: the Pico
  SDK is vendored in-tree and the build uses the ``arm-none-eabi`` GCC found on
  ``PATH``.
- **OpenOCD** 0.12.0 or newer for flashing and debugging (RP2040 support is
  included from 0.12.0):

    .. code-block:: bash

        $ sudo apt install openocd

Pico SDK resolution
-------------------
The Pico SDK is resolved automatically at configure time, so no manual step is
required. The build clones the SDK once (non-recursively — an RP2040 no-OS build
does not need the SDK's own submodules such as tinyusb, cyw43 or lwip) in-tree at
``libraries/pico-sdk`` and reuses it thereafter.

.. note::
   Resolution priority for ``PICO_SDK_PATH``: the ``NO_OS_PICO_SDK_DIR`` override
   (CMake var or environment) → in-tree ``libraries/pico-sdk`` (cloned once by
   default) → a version-keyed shared store when ``NO_OS_CACHE_DIR`` is set. Any
   externally exported ``PICO_SDK_PATH`` is ignored so a stale value cannot break
   the build.

Install picotool (optional)
---------------------------
Install `picotool <https://github.com/raspberrypi/picotool>`_ to generate the
``.uf2`` image used for USB Mass Storage flashing. When picotool is unavailable,
the ``.elf`` and ``.hex`` are still produced and the ``.uf2`` step is skipped
with a status message.

picotool is a separate tool from the Pico SDK and is not vendored, so it has to
be built from source. Install its build dependencies (``cmake``, a host C/C++
compiler and ``libusb-1.0`` development headers), then clone and build it
against the vendored Pico SDK (use the same version as the SDK):

    .. code-block:: bash

        # Build dependencies (Debian/Ubuntu)
        $ sudo apt install cmake build-essential pkg-config libusb-1.0-0-dev

        # Clone and build picotool against the Pico SDK
        $ git clone https://github.com/raspberrypi/picotool.git
        $ cd picotool
        $ mkdir build && cd build
        $ PICO_SDK_PATH=/path/to/no-OS/libraries/pico-sdk cmake ..
        $ make -j$(nproc)

Make the resulting ``picotool`` binary available to the no-OS build either by
adding its directory to ``PATH`` or by passing ``-DPICOTOOL_PATH=...`` at
configure time.

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

     or from VS Code via ``File > Open Workspace from File...``.

  VS Code does not always prompt to open the workspace on its own (the
  notification is shown at most once per folder and is suppressed once
  dismissed), so open it explicitly as shown above.

Listing build combinations
==========================

From the no-OS repository root, list the valid combinations and filter to the
Pico board:

    .. code-block:: bash

        $ python tools/scripts/no_os_build.py list --board rpi-pico

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
              --project iio_demo --variant iio --board rpi-pico

Useful options:

    - ``--clean`` — remove the build directory before configuring.
    - ``-j N`` / ``--jobs N`` — parallel compile jobs.
    - ``--parallel`` — build different boards in parallel.
    - ``--probe {openocd,jlink}`` — select the debug probe (needed for flashing).
    - ``--flash`` — flash after a successful build (requires ``--probe``).
    - ``--dry-run`` — print the ``cmake`` commands without running them.

Each combination is built into its own directory named
``build-<project>-<variant>-<board>`` at the repo root (override the location
with ``--build-dir``). The build artifacts (``.elf``, ``.hex``, ``.bin`` and,
when picotool is available, ``.uf2``) are placed in ``<build-dir>/build``.

Manual CMake invocation
-----------------------
You can also drive CMake directly. Configure with the board preset and select
the project/variant with ``PROJECT_DEFCONFIG`` (a path relative to ``projects/``
of the form ``<project>/<variant>.conf``):

    .. code-block:: bash

        $ cmake --preset rpi-pico -B build-iio_demo-iio-rpi-pico \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf

Then build the project target (the target name is the project directory name):

    .. code-block:: bash

        $ cmake --build build-iio_demo-iio-rpi-pico --target iio_demo

The build artifacts (``.elf``, ``.hex``, ``.bin`` and, when picotool is
available, ``.uf2``) are placed in ``build-iio_demo-iio-rpi-pico/build``.

.. note::
   When the project provides board variants under
   ``projects/<project>/boards/<variant>/<board>.conf``, the matching board
   config is layered on top automatically based on the preset's ``BOARD``
   value.

.. note::
   If no ``CMAKE_BUILD_TYPE`` is given, the build defaults to
   ``RelWithDebInfo`` so the ELF contains debug info. Use
   ``-DCMAKE_BUILD_TYPE=Debug`` for an unoptimized build, or ``MinSizeRel`` /
   ``Release`` for size/speed.

.. note::
   The Pico flash region size defaults to 2 MiB (the Raspberry Pi Pico). Pass
   ``-DPICO_FLASH_SIZE_BYTES=...`` to target a board with a different flash size.

Configuring options (menuconfig)
--------------------------------
Kconfig options can be browsed and changed interactively after configuring:

    .. code-block:: bash

        $ cmake --build build-iio_demo-iio-rpi-pico --target menuconfig

Flashing and Debugging
======================

Flashing requires a debug probe to be selected at **configure** time via
``-DPROBE=...``. Two probes are supported:

    - ``openocd`` (default) — a second Raspberry Pi Pico running the
      ``debugprobe``/``picoprobe`` firmware (a CMSIS-DAP probe), driven by
      OpenOCD. OpenOCD's cmsis-dap driver handles the RP2040 SWD multidrop
      correctly.
    - ``jlink`` — a SEGGER J-Link via the ``pylink-square`` Python package.

Configure with the desired probe, build, then run the ``flash`` target:

    .. code-block:: bash

        $ cmake --preset rpi-pico -B build-iio_demo-iio-rpi-pico \
              -DPROJECT_DEFCONFIG=iio_demo/iio.conf -DPROBE=openocd
        $ cmake --build build-iio_demo-iio-rpi-pico --target iio_demo
        $ cmake --build build-iio_demo-iio-rpi-pico --target flash

When the OpenOCD probe is selected, the following additional targets are
available:

    - ``flash`` — program the ``.elf`` to the target and reset.
    - ``erase`` — erase the device flash.
    - ``debug`` — start OpenOCD and attach GDB (Linux/macOS).
    - ``debug_server`` — start OpenOCD and wait for a GDB connection.

    .. code-block:: bash

        $ cmake --build build-iio_demo-iio-rpi-pico --target debug

The OpenOCD configuration for the RP2040 (cmsis-dap interface and
``target/rp2040.cfg``) is generated automatically into ``openocd.cfg`` in the
project build directory.

CMSIS-DAP / picoprobe probe
---------------------------
The default ``openocd`` probe expects a CMSIS-DAP debugger. Flash a spare
Raspberry Pi Pico with the ``debugprobe`` firmware to turn it into one: download
``debugprobe_on_pico.uf2`` from the `debugprobe releases
<https://github.com/raspberrypi/debugprobe/releases>`_, hold the BOOTSEL button
while connecting the probe Pico over USB, and copy the ``.uf2`` onto the mounted
USB Mass Storage device. Then wire the probe Pico to the target board's SWD pads:

    ===============  ===========================
    Probe Pico       Target Pico
    ===============  ===========================
    GP2              SWCLK (left SWD pad)
    GP3              SWDIO (right SWD pad)
    GND              GND (middle SWD pad)
    VSYS / 3V3       VSYS / 3V3 (to power target)
    ===============  ===========================

J-Link probe
------------
OpenOCD's J-Link driver fails the RP2040 SWD multidrop selection
(``Failed to connect multidrop rp2040.dap0``), so the ``jlink`` probe uses the
native SEGGER tooling via ``pylink-square`` instead. Install the
`J-Link software <https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack>`_
and wire the J-Link's SWD pins to the target Pico's SWD pads (SWDIO, SWCLK,
GND); power the Pico separately over USB or ``VSYS``.

Flashing via .uf2
-----------------
If picotool is available, the generated ``.uf2`` image can also be flashed
without a probe by holding the BOOTSEL button while connecting the board over
USB and copying the ``.uf2`` file onto the mounted USB Mass Storage device.

To debug graphically from VS Code, open the generated ``no-os.code-workspace``
(see the install step above) and launch the debug configuration from there;
opening the no-OS folder without the workspace will not load the debug setup.
