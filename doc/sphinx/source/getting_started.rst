***************
Getting Started
***************

This tutorial takes you from a fresh clone of no-OS to a running application
that streams data off an evaluation board. It is meant to be read top to
bottom the first time you use no-OS; once you are comfortable, the
:doc:`build_guide` and the per-project pages under :doc:`projects_doc` are the
references you will come back to.

We use the **IIO Demo** project as the vehicle. It runs an IIO (Industrial
I/O) server on the target and exposes software ADC/DAC devices that loop data
back to each other, so **no external sensor wiring is required** — any
supported board connected over USB is enough to see it work end to end. That
also makes it the project most people use to bring up a new board.

.. contents:: Table of Contents
    :depth: 2

What is no-OS?
==============

no-OS is a framework for building bare-metal (no operating system) firmware
that drives Analog Devices parts. A no-OS application is composed from three
layers:

* **Drivers** — device-specific code for an ADI part (e.g. ``adxl355``). A
  driver talks to hardware only through the no-OS abstractions, never directly.
* **The no-OS platform layer** — a thin hardware-abstraction API
  (``no_os_spi_*``, ``no_os_gpio_*``, ``no_os_i2c_*``, ``no_os_uart_*``, ...).
  Each supported MCU family provides an implementation of this API.
* **Projects** — example applications under ``projects/`` that wire a driver to
  a platform and a ``main()``.

Because drivers depend on the abstraction layer and not on a specific MCU, the
same driver builds and runs across every supported platform (Xilinx, Intel,
Maxim, STM32, ADuCM, Raspberry Pi Pico, Lattice, and Linux).

For a fuller treatment of how these layers fit together — the ``platform_ops``
dispatch mechanism, the init_param/descriptor idiom, and the source-tree map —
see :doc:`architecture`.

Prerequisites
=============

Before you build anything you need three things: the source, the host build
tools (Git, Python, CMake, Ninja), and the vendor SDK for your board's MCU
family.

1. Host build tools
------------------

* **Git** — to clone the repository and its submodules.
* **Python 3.8+** — the unified build utility
  (``tools/scripts/no_os_build.py``) is a Python script. Install its helper
  dependencies once with:

  .. code-block:: bash

     pip install -r tools/scripts/requirements.txt

* **CMake** (3.27+) and **Ninja** — no-OS builds through CMake using the Ninja
  generator, so both must be installed and on your ``PATH``. Verify with:

  .. code-block:: bash

     cmake --version
     ninja --version

  On Debian/Ubuntu: ``sudo apt install cmake ninja-build``. On macOS with
  Homebrew: ``brew install cmake ninja``. On Windows, install both and make sure
  ``cmake``, ``ninja`` and ``python`` are available on your ``PATH``.

1. A supported board and its SDK
-------------------------------

Pick the row that matches the hardware you have. Each platform requires its
vendor SDK/toolchain to be installed and, in most cases, one environment
variable pointing at it. The **CMake build guide** for your platform has the
full install steps, the exact environment variable, and the list of supported
boards/presets:

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
     - ``arm-none-eabi-gcc`` (Pico SDK is vendored in-tree)
     - :doc:`build_guides/build_pico_cmake`

For example, the Maxim SDK exposes its libraries through ``MAXIM_LIBRARIES``:

.. code-block:: bash

   export MAXIM_LIBRARIES=~/MaximSDK/Libraries

The build utility auto-detects default SDK install locations, so you only need
to export a variable when your SDK lives somewhere non-standard. Follow the
CMake build guide linked above for your platform before continuing.

.. note::

   The Xilinx and Intel/Altera FPGA platforms use their own vendor tool flows
   rather than the CMake utility used throughout this tutorial. See
   :doc:`build_guides/build_xilinx` and :doc:`build_guides/build_intel` for
   those.

3. A debug probe (for flashing)
------------------------------

To load firmware onto the board you need a debug probe supported by the build
utility — either a **J-Link** or an **OpenOCD**-compatible probe. Many
evaluation boards have an on-board debugger, so a single USB cable is often all
you need. You can build without a probe; you just cannot flash without one.

Step 1 — Get the source
=======================

Clone with ``--recursive`` so the submodules (vendor HALs, libraries) come
down with it:

.. code-block:: bash

   git clone --recursive https://github.com/analogdevicesinc/no-OS
   cd no-OS

If you already cloned without ``--recursive``, initialize the submodules in
place:

.. code-block:: bash

   git submodule update --recursive --init

Step 2 — Find your build combination
====================================

no-OS builds are identified by a **project**, a **variant**, and a **board**.
List every valid combination for the IIO Demo project:

.. code-block:: bash

   python tools/scripts/no_os_build.py list --project iio_demo

You will get a table like this (abridged):

.. code-block::

   PROJECT   VARIANT                BOARD             PLATFORM
   ────────  ─────────────────────  ────────────────  ─────────
   iio_demo  iio                    max32650fthr      maxim
   iio_demo  iio                    sdp-ck1z          stm32
   iio_demo  iio                    eval-adicup3029   aducm3029
   iio_demo  iio                    rpi-pico          pico
   iio_demo  iio_usb_uart           sdp-ck1z          stm32
   ...

Read a row as: *"the* ``iio`` *variant of* ``iio_demo`` *runs on the*
``max32650fthr`` *board using the* ``maxim`` *platform."* Pick the row whose
board you own. The rest of this tutorial uses ``--variant iio --board
max32650fthr`` as the running example — substitute your own board name
throughout.

.. note::

   Not every variant exists for every board. The ``iio`` variant is the plain
   serial/UART build and is the best one to start with. Other variants add a
   USB CDC-ACM console (``iio_usb_uart``), a hardware-timer trigger
   (``iio_timer_trigger``), or a Wi-Fi transport (``iio_wifi``).

Step 3 — Build
==============

Build the combination you picked:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr

This configures and compiles the project into a dedicated directory named
``build-iio_demo-iio-max32650fthr/`` at the repository root. On success it
contains the firmware image (``.elf`` and, depending on the platform, ``.hex``
or ``.bin``).

Useful flags while iterating:

* ``--clean`` — remove the build directory before configuring (use after
  changing configuration).
* ``--dry-run`` — print the underlying CMake commands without running them, so
  you can see exactly what the utility does.

Step 4 — Flash the board
========================

Connect the board over USB, then build and flash in one step by adding a probe
type and ``--flash``:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr \
      --probe openocd --flash

Use ``--probe jlink`` instead if you are using a J-Link. ``--flash`` requires
``--probe``. After it completes, the firmware is running on the board and the
IIOD server is listening on the board's serial console.

Step 5 — Debug the firmware
===========================

To step through the firmware with a debugger you need a probe selected at
configure time (the same ``--probe`` used for flashing). There are two ways to
debug: from the command line, or graphically in VS Code.

Command line (GDB)
------------------

When a project has been configured with a probe, the CMake build exposes debug
targets. Build the debug target to start the debug server and attach GDB:

.. code-block:: bash

   cmake --build build-iio_demo-iio-max32650fthr --target debug

On Windows this is split into ``debug_server`` (run in one terminal) and
``debug_gdb`` (run in another). For an unoptimized build, configure with
``-DCMAKE_BUILD_TYPE=Debug``; the default ``RelWithDebInfo`` already includes
symbols. Your platform's CMake build guide (see :doc:`build_guide`) lists the
exact targets available for its probe.

Graphical (VS Code)
-------------------

The build generates a VS Code workspace file, ``no-os.code-workspace``, at the
repository root, along with the debug launch configuration and include paths.

.. important::

   You **must** open the generated ``no-os.code-workspace`` file — not just the
   no-OS folder. The debug launch configuration and include paths live in that
   workspace; opening the folder on its own will not pick them up.

Open it from the command line:

.. code-block:: bash

   code no-os.code-workspace

or from VS Code via ``File > Open Workspace from File...``. The build utility
can also open it for you after a successful build by passing ``--open``:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr \
      --probe openocd --open

Install the **Cortex-Debug** VS Code extension for the debug launch
configuration to work, then start the debug session from the Run and Debug view.

Step 6 — Talk to the running device
===================================

The board now presents an IIO context over its serial port. Use the
`libiio <https://github.com/analogdevicesinc/libiio>`_ command-line tools
(``iio_info``, ``iio_readdev`` / ``iio_writedev``) or the
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_
or `Scopy <https://github.com/analogdevicesinc/scopy>`_ GUIs on your host to
interact with it. These are the same commands introduced in
:doc:`iio_framework`.

First identify the serial device the board enumerated as — ``/dev/ttyUSB0`` or
``/dev/ttyACM0`` on Linux, a ``COMx`` port on Windows.

Enumerate the context to confirm the connection and see the ``adc_demo`` and
``dac_demo`` devices with their channels and attributes:

.. code-block:: bash

   iio_info -u serial:/dev/ttyACM0,57600,8n1n

Read a buffer of samples from the demo ADC:

.. code-block:: bash

   iio_readdev -u serial:/dev/ttyACM0,57600,8n1n -s 100 adc_demo > sample.dat

Write a buffer to the demo DAC (which loops back to the ADC):

.. code-block:: bash

   cat sample.dat | iio_writedev -u serial:/dev/ttyACM0,57600,8n1n -s 100 dac_demo

Because the demo ADC and DAC share a loopback buffer, data you write to the DAC
can be read back from the ADC — a self-contained round trip that confirms the
whole stack (transport, IIO server, device model) is working without any
external hardware.

On Windows the serial URI uses the ``COM`` name:

.. code-block:: bash

   iio_readdev -u serial:COM9,57600,8n1n -s 100 adc_demo voltage0 voltage1

Step 7 — Make it your own
=========================

Once the demo runs, the natural next steps are:

* **Change the configuration.** Each variant is defined by a ``.conf``
  defconfig in the project directory (``projects/iio_demo/``). Editing these
  values — buffer sizes, enabled examples, Wi-Fi credentials for the
  ``iio_wifi`` variant — and rebuilding with ``--clean`` is the fastest way to
  see the build system respond to configuration.
* **Read the project sources.** ``projects/iio_demo/src/common`` holds the
  device initialization data and ``projects/iio_demo/src/platform`` holds the
  per-platform macro definitions. This is the pattern every no-OS project
  follows: common application code plus a thin per-platform configuration
  layer.
* **Swap in a real driver.** Browse :doc:`projects_doc` for a project that
  targets an ADI part you have, and repeat Steps 2–6 with its project and
  board name. The build, flash, and interaction workflow is identical.
* **Write your own driver.** When you are ready to add support for a new part,
  the :doc:`drivers_guide` documents the required structures, functions, and
  coding conventions, and :doc:`contributing` covers how to submit it.

Troubleshooting
===============

**Submodules are empty / headers not found.**
   You cloned without ``--recursive``. Run
   ``git submodule update --recursive --init``.

**The build utility cannot find the compiler / SDK.**
   The vendor SDK is not installed or its environment variable is not set.
   Re-check the platform's build guide (see the table in Prerequisites) and
   confirm the relevant variable (``MAXIM_LIBRARIES``, ``STM32CUBEMX``,
   ``CCES_HOME``, ...) points at the right path.

**Nothing to flash / flashing fails.**
   Confirm the board is powered and its debug probe is connected, and that you
   passed both ``--probe`` and ``--flash``. Some boards need their on-board
   debugger drivers installed on the host first.

**The IIO tools cannot open the serial port.**
   Verify the device name (``ls /dev/ttyUSB* /dev/ttyACM*`` on Linux) and the
   baud rate in the URI.

**A variant/board combination is rejected.**
   Re-run ``python tools/scripts/no_os_build.py list --project iio_demo`` — not
   every variant exists on every board, and the table is the source of truth.
