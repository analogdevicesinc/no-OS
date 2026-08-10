***************
Getting Started
***************

This tutorial takes you from a fresh clone of no-OS to a running application
that streams data off an evaluation board — no external sensor wiring required.
Read it top to bottom the first time; afterwards, the :doc:`build_guide`, the
:doc:`cmake_cheatsheet`, and the per-project pages under :doc:`projects_doc`
are your references.

We use the **IIO Demo** project throughout. It runs an IIO (Industrial I/O)
server on the target and exposes software ADC/DAC devices that loop back to
each other, so any supported board connected over USB is enough to see it work
end to end. That makes it the go-to project for bringing up a new board.

.. contents:: Table of Contents
    :depth: 2

What is no-OS?
==============

no-OS is a framework for building bare-metal (no operating system) firmware for
Analog Devices parts. An application is composed from three layers:

* **Drivers** — device-specific code for an ADI part (e.g. ``adxl355``), talking
  to hardware only through the no-OS abstractions.
* **no-OS platform layer** — a thin hardware-abstraction API (``no_os_spi_*``,
  ``no_os_gpio_*``, ``no_os_i2c_*``, ``no_os_uart_*``, ...), implemented
  separately for each supported MCU family.
* **Projects** — example applications under ``projects/`` that wire a driver to a
  platform and a ``main()``.

Because drivers depend on the abstraction layer rather than a specific MCU, the
same driver builds and runs across every supported platform (Xilinx, Maxim,
STM32, ADuCM, Raspberry Pi Pico, linux-userspace, win, and mac). For the full picture —
``platform_ops`` dispatch, the init_param/descriptor idiom, and the source-tree
map — see :doc:`architecture`.

Prerequisites
=============

You need three things: the source, host build tools, and the vendor SDK for
your board.

Host build tools
----------------

no-OS builds through CMake with the Ninja generator and is driven by a Python
utility (``tools/scripts/no_os_build.py``), so you need **Git**, **Python 3.8+**,
**CMake 3.27+**, and **Ninja**:

* On Debian/Ubuntu: ``sudo apt install git cmake ninja-build python3``
* On macOS with Homebrew: ``brew install git cmake ninja python``
* On Windows: ``winget install Git.Git Python.Python.3 Kitware.CMake Ninja-build.Ninja``

Ensure ``git``, ``python``, ``cmake`` and ``ninja`` are all on your ``PATH``:

.. code-block:: bash

   cmake --version     # 3.27+
   ninja --version

Board SDK
---------

Each platform needs its vendor SDK and, usually, one environment variable
pointing at it. Find your board's row, then follow the linked CMake build guide
for the full install steps, exact variable, and supported presets:

.. list-table::
   :header-rows: 1
   :widths: 16 24 26 34

   * - Platform
     - Example board
     - Required SDK / toolchain
     - CMake build guide
   * - Maxim
     - MAX32650FTHR
     - Maxim MSDK (``MAXIM_LIBRARIES``)
     - :doc:`build_guides/build_maxim_cmake`
   * - STM32
     - SDP-K1, NUCLEO-F413ZH
     - STM32CubeIDE + STM32CubeMX (``STM32CUBEIDE`` / ``STM32CUBEMX``)
     - :doc:`build_guides/build_stm32_cmake`
   * - ADuCM3029
     - EVAL-ADICUP3029
     - CrossCore Embedded Studio (``CCES_HOME``)
     - :doc:`build_guides/build_aducm3029_cmake`
   * - Raspberry Pi Pico
     - Raspberry Pi Pico
     - ``arm-none-eabi-gcc`` (Pico SDK fetched automatically)
     - :doc:`build_guides/build_pico_cmake`
   * - Xilinx
     - ZedBoard, ZCU102
     - Vitis (``XILINX_VITIS``) + board ``.xsa`` file (``--hardware``)
     - :doc:`build_guides/build_xilinx_cmake`
   * - Linux userspace
     - Any Linux host (spidev / i2c-dev / gpio-cdev)
     - System GCC — no vendor SDK required
     - :doc:`build_guides/build_linux_userspace_cmake`
   * - Windows
     - Windows host (x86_64)
     - MSYS2 GCC — no vendor SDK required
     - :doc:`build_guides/build_win_cmake`
   * - macOS
     - macOS host (Apple Silicon or Intel)
     - Xcode Command Line Tools — no vendor SDK required
     - :doc:`build_guides/build_mac_cmake`

The build utility auto-detects default SDK install locations, so you only export
a variable when your SDK lives somewhere non-standard, e.g.:

.. code-block:: bash

   export MAXIM_LIBRARIES=~/MaximSDK/Libraries

Debug probe (for flashing)
--------------------------

Flashing needs a **J-Link** or **OpenOCD**-compatible probe. Many evaluation
boards have an on-board debugger, so a single USB cable is often enough. You can
build without a probe — you just cannot flash without one.

Step 1 — Get the source
=======================

.. code-block:: bash

   git clone https://github.com/analogdevicesinc/no-OS
   cd no-OS

External libraries (lwIP, FreeRTOS, mbedTLS, LVGL, ...) are cloned automatically
at configure time, so nothing extra is fetched up front. To override a
dependency's location or share a download cache, see :doc:`configuration_guide`.

Step 2 — Find your build combination
====================================

A build is identified by a **project**, a **variant**, and a **board**. List
every valid combination for IIO Demo:

.. code-block:: bash

   python tools/scripts/no_os_build.py list --project iio_demo

.. code-block::

   PROJECT   VARIANT                BOARD             PLATFORM
   ────────  ─────────────────────  ────────────────  ─────────
   iio_demo  iio                    max32650fthr      maxim
   iio_demo  iio                    sdp-ck1z          stm32
   iio_demo  iio                    eval-adicup3029   aducm3029
   iio_demo  iio                    rpi-pico          pico
   iio_demo  iio_usb_uart           sdp-ck1z          stm32
   ...

Read a row as *"the* ``iio`` *variant of* ``iio_demo`` *runs on the*
``max32650fthr`` *board via the* ``maxim`` *platform."* Pick the row for the
board you own; this tutorial uses ``--variant iio --board max32650fthr``
throughout — substitute your own.

.. note::

   Not every variant exists for every board. ``iio`` is the plain serial/UART
   build and the best starting point. Others add a USB CDC-ACM console
   (``iio_usb_uart``), a hardware-timer trigger (``iio_timer_trigger``), or a
   Wi-Fi transport (``iio_wifi``).

Step 3 — Build
==============

.. code-block:: bash

   python tools/scripts/no_os_build.py build --project iio_demo --variant iio --board max32650fthr

Output lands in ``build/iio_demo-iio-max32650fthr/`` — the firmware image
(``.elf``, plus ``.hex`` or ``.bin`` depending on the platform).

Useful flags while iterating:

* ``--clean`` — wipe the build directory first (use after changing config).
* ``--dry-run`` — print the underlying CMake commands without running them.

Step 4 — Flash the board
========================

Connect the board over USB, then build and flash in one step:

.. code-block:: bash

   python tools/scripts/no_os_build.py build --project iio_demo --variant iio --board max32650fthr --probe openocd --flash

Use ``--probe jlink`` for a J-Link. ``--flash`` requires ``--probe``. When it
finishes, the firmware is running and the IIOD server is listening on the
board's serial console.

Step 5 — Debug the firmware
===========================

Debugging needs a probe selected at configure time (the same ``--probe`` used
for flashing). You can debug from the command line or graphically in VS Code.

Command line (GDB)
------------------

A project configured with a probe exposes debug targets. Start the debug server
and attach GDB:

.. code-block:: bash

   cmake --build build/iio_demo-iio-max32650fthr --target debug

On Windows this splits into ``debug_server`` and ``debug_gdb`` (run in separate
terminals). For an unoptimized build, configure with
``-DCMAKE_BUILD_TYPE=Debug``; the default ``RelWithDebInfo`` already includes
symbols. See your platform's CMake build guide (:doc:`build_guide`) for the exact
targets.

Graphical (VS Code)
-------------------

The build generates ``no-os.code-workspace`` at the repository root, with the
debug launch configuration and include paths.

.. important::

   Open the generated ``no-os.code-workspace`` file, **not** the no-OS folder —
   the launch configuration and include paths live in the workspace and are not
   picked up otherwise.

.. code-block:: bash

   code no-os.code-workspace

or use ``File > Open Workspace from File...``. The build utility can open it for
you after a successful build with ``--open``:

.. code-block:: bash

   python tools/scripts/no_os_build.py build --project iio_demo --variant iio --board max32650fthr --probe openocd --open

Install the **Cortex-Debug** extension, then start the debug session from the
Run and Debug view.

Step 6 — Talk to the running device
===================================

The board presents an IIO context over its serial port. Interact with it using
the `libiio <https://github.com/analogdevicesinc/libiio>`_ command-line tools
(``iio_info``, ``iio_readdev``, ``iio_writedev``) or the
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_
/ `Scopy <https://github.com/analogdevicesinc/scopy>`_ GUIs — the same tools
introduced in :doc:`iio_framework`.

First find the serial device: ``/dev/ttyUSB0`` or ``/dev/ttyACM0`` on Linux, a
``COMx`` port on Windows.

**Enumerate** the context to confirm the connection and see the ``adc_demo`` and
``dac_demo`` devices with their channels and attributes:

.. code-block:: bash

   iio_info -u serial:/dev/ttyACM0,57600,8n1n

**Read** a buffer of samples from the demo ADC and inspect it as a hex dump:

.. code-block:: bash

   iio_readdev -u serial:/dev/ttyACM0,57600,8n1n -s 100 adc_demo voltage0 voltage1 > sample.dat
   hexdump -C sample.dat

**Write** the shipped sine-wave file to the demo DAC (which loops back to the
ADC) so the loopback carries a known pattern:

.. code-block:: bash

   cat projects/iio_demo/sample_sine.dat | iio_writedev -u serial:/dev/ttyACM0,57600,8n1n -s 100 dac_demo

**Read** the ADC back and hex-dump it again:

.. code-block:: bash

   iio_readdev -u serial:/dev/ttyACM0,57600,8n1n -s 100 adc_demo voltage0 voltage1 > sample.dat
   hexdump -C sample.dat

The demo ADC and DAC share a loopback buffer, so the second dump differs from
the first — it now carries the sine-wave pattern you wrote. That round trip
confirms the whole stack (transport, IIO server, device model) works without any
external hardware.

On Windows, use the ``COM`` name in the URI and ``Format-Hex`` for the hex view:

.. code-block:: powershell

   iio_readdev -u serial:COM9,57600,8n1n -s 100 adc_demo voltage0 voltage1 > sample.dat
   Format-Hex sample.dat

Step 7 — Make it your own
=========================

Natural next steps once the demo runs:

* **Change the configuration.** Each variant is a ``.conf`` defconfig in
  ``projects/iio_demo/``. Edit its values (buffer sizes, enabled examples, Wi-Fi
  credentials for ``iio_wifi``) and rebuild with ``--clean``.
* **Read the project sources.** ``src/common`` holds the device init data and
  ``src/platform`` holds the per-platform macros — the pattern every no-OS
  project follows.
* **Swap in a real driver.** Browse :doc:`projects_doc` for a project targeting
  an ADI part you have, and repeat Steps 2–6; the workflow is identical.
* **Write your own driver.** :doc:`drivers_guide` documents the required
  structures, functions, and conventions; :doc:`contributing` covers submission.

Troubleshooting
===============

**A dependency fails to clone during configure.**
   Dependencies are fetched over the network. Check your connection or proxy and
   re-run the build. To reuse a local working copy or a shared download cache,
   see :doc:`configuration_guide`.

**The build utility cannot find the compiler / SDK.**
   The vendor SDK is not installed or its environment variable is not set.
   Re-check the platform's build guide (see the table in Prerequisites) and
   confirm the relevant variable (``MAXIM_LIBRARIES``, ``STM32CUBEMX``,
   ``CCES_HOME``, ...) points at the right path.

**Nothing to flash / flashing fails.**
   Confirm the board is powered and its probe is connected, and that you passed
   both ``--probe`` and ``--flash``. Some boards need their on-board debugger
   drivers installed on the host first.

**The IIO tools cannot open the serial port.**
   Verify the device name (``ls /dev/ttyUSB* /dev/ttyACM*`` on Linux) and the
   baud rate in the URI.

**A variant/board combination is rejected.**
   Re-run ``python tools/scripts/no_os_build.py list --project iio_demo`` — not
   every variant exists on every board, and the table is the source of truth.
