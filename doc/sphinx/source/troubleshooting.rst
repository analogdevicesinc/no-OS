*********************
Troubleshooting & FAQ
*********************

This page collects the problems people most often hit when building and running
no-OS, and the questions that come up repeatedly, in one place. It is organized
by *when* the problem shows up — cloning, building, flashing, running — plus a
general FAQ at the end.

Each entry points at the guide with the full story: :doc:`getting_started` for
the end-to-end workflow, :doc:`build_guide` for per-platform toolchain setup,
:doc:`configuration_guide` for the Kconfig/defconfig system, and
:doc:`architecture` for how the pieces fit together.

.. contents:: Table of Contents
    :depth: 2

Clone and setup
===============

Submodules are empty / vendor headers not found
-----------------------------------------------

**Symptom:** the build fails with missing headers from a vendor SDK or a
library directory, or directories under ``libraries/`` are empty.

**Cause:** the repository was cloned without its submodules.

**Fix:** clone with ``--recursive``, or initialize the submodules in an existing
clone:

.. code-block:: bash

   git submodule update --recursive --init

The build utility complains it cannot find the repo root
--------------------------------------------------------

**Symptom:** ``no_os_build.py`` exits with *"Could not find no-OS repo root"*.

**Cause:** the script locates the repository by walking up for a
``CMakePresets.json`` next to a ``projects/`` directory.

**Fix:** run it from inside your no-OS checkout (``cd no-OS`` first). Invoke it
as ``python tools/scripts/no_os_build.py ...`` from the repo root, exactly as in
:doc:`getting_started`.

``kconfiglib`` / ``pylink`` not installed
-----------------------------------------

**Symptom:** a ``ModuleNotFoundError`` for ``kconfiglib`` (during configure) or
``pylink`` (during a J-Link flash).

**Cause:** the build utility's Python dependencies are not installed.

**Fix:** install them once:

.. code-block:: bash

   pip install -r tools/scripts/requirements.txt

On Windows this also pulls in ``windows-curses`` (needed by ``menuconfig``).

Build
=====

The build utility cannot find the compiler / SDK
------------------------------------------------

**Symptom:** configure fails because the toolchain or a vendor SDK path cannot
be located.

**Cause:** the vendor SDK is not installed, or its environment variable is not
set. The utility auto-detects default install locations, so this usually means a
non-standard install path.

**Fix:** set the environment variable for your platform (see the matching page
under :doc:`build_guide` for the exact value):

.. list-table::
   :header-rows: 1
   :widths: 20 40 40

   * - Platform
     - Environment variable
     - Points at
   * - Maxim
     - ``MAXIM_LIBRARIES``
     - ``.../MaximSDK/Libraries``
   * - STM32
     - ``STM32CUBEMX`` / ``STM32CUBEIDE``
     - the CubeMX / CubeIDE install
   * - ADuCM3029
     - ``CCES_HOME``
     - the CrossCore Embedded Studio install
   * - Xilinx
     - (Vivado/Vitis ``settings`` sourced)
     - the Xilinx tools install

A variant/board combination is rejected
---------------------------------------

**Symptom:** ``no_os_build.py build`` errors that the combination is invalid, or
your board/variant is missing.

**Cause:** not every variant is available on every board — the valid set is
discovered from the project's ``.conf`` variants and ``boards/`` directory.

**Fix:** list the valid combinations and pick a row that exists:

.. code-block:: bash

   python tools/scripts/no_os_build.py list --project iio_demo

The table it prints is the source of truth (see :doc:`getting_started`, Step 2).

An option I set in a ``.conf`` did not take effect
--------------------------------------------------

**Symptom:** you edited a defconfig but the build behaves as before.

**Cause:** the configuration is resolved at *configure* time and cached in the
build directory.

**Fix:** rebuild with ``--clean`` (or delete the ``build-*`` directory) so the
configuration is regenerated:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr --clean

See :doc:`configuration_guide` for how the layered defconfigs are merged.

Interface enabled but backend missing (link errors)
---------------------------------------------------

**Symptom:** undefined references to a peripheral's platform functions at link
time, even though the interface option is on.

**Cause:** you enabled the *abstract* interface (e.g. ``CONFIG_SPI``) in the
variant config, but the *board* config does not enable the platform backend
(e.g. ``CONFIG_SPI_MAXIM``). Interfaces come from the variant; backends come
from the board defconfig.

**Fix:** enable the backend in the board config, or use a board whose defconfig
already provides it. This layering is explained in :doc:`configuration_guide`.

A stale build behaves strangely after switching branches or configs
-------------------------------------------------------------------

**Symptom:** inexplicable build or link errors after changing branch, variant,
or options.

**Fix:** do a clean configure. ``--clean`` removes the build directory before
configuring; ``--fresh`` passes CMake's ``--fresh`` to drop ``CMakeCache.txt``
and ``CMakeFiles/`` while keeping other artifacts. When in doubt, delete the
``build-<project>-<variant>-<board>`` directory and rebuild.

Flash and debug
===============

Nothing to flash / flashing fails
---------------------------------

**Symptom:** ``--flash`` does nothing, or the flash step errors.

**Causes and fixes:**

* You did not select a probe. ``--flash`` **requires** ``--probe``:

  .. code-block:: bash

     python tools/scripts/no_os_build.py build \
        --project iio_demo --variant iio --board max32650fthr \
        --probe openocd --flash

* The board is not powered or the debug probe is not connected. Confirm the USB
  connection and that the on-board debugger's drivers are installed on the host.
* For J-Link, ensure ``pylink-square`` is installed (see the setup section
  above).

The build succeeds but the board does nothing
---------------------------------------------

**Symptom:** firmware flashes but there is no serial output or IIO context.

**Checks:**

* Confirm you are opening the right serial device at the right baud rate (see
  the running section below).
* Some boards expose several USB serial interfaces; make sure you opened the one
  the firmware uses.
* Rebuild with debug info (``CMAKE_BUILD_TYPE=Debug``) and attach a debugger to
  confirm ``main()`` is reached. The default build type is ``RelWithDebInfo``,
  so the ELF already contains symbols for debugging.

Run and interact
================

The IIO tools cannot open the serial port
-----------------------------------------

**Symptom:** ``iio_readdev`` / ``iio_writedev`` fail to connect.

**Fixes:**

* Verify the device name — ``ls /dev/ttyUSB* /dev/ttyACM*`` on Linux, or the
  ``COMx`` name on Windows — and the baud rate in the URI, e.g.
  ``serial:/dev/ttyACM0,57600,8n1n``.
* On Linux, add your user to the ``dialout`` group to access serial devices
  without ``sudo`` (log out and back in for it to take effect).
* Make sure no other program (a serial monitor, a previous session) is holding
  the port open.

See the *Interacting with the device* steps in :doc:`getting_started`.

I see trailing bytes or garbage at the end of a capture
-------------------------------------------------------

**Symptom:** buffered reads end with unexpected padding or repeated bytes.

**Checks:** confirm the sample count (``-s``) and buffer length (``-b``) match
what you expect, and that the channel scan format matches the client's. Behavior
that is specific to one board port usually points at that platform's UART/IRQ
timing rather than the shared IIO layer — reproduce against another transport or
the ``linux`` platform to isolate it.

Frequently asked questions
==========================

Which branch should I use?
--------------------------

Use **main** — it is the development branch and carries the newest drivers and
fixes.

Do I need real hardware to try no-OS?
-------------------------------------

Not to get started. The IIO Demo uses software loopback devices, so any
supported board reachable over USB is enough — no sensor wiring required. See
:doc:`getting_started`.

Which platforms are supported?
------------------------------

Xilinx, Intel/Altera, Maxim, STM32, ADuCM3029, Raspberry Pi Pico, Lattice, and
Linux, among others. Each has a setup page under :doc:`build_guide`, and the
platform backends live under ``drivers/platform/`` (see :doc:`architecture`).

Can the same driver run on a different MCU?
-------------------------------------------

Yes — that is the core design. A driver talks only to the ``no_os_*``
abstraction layer, never to a vendor SDK, so moving to another MCU changes only
the platform backend selected at configuration time. The mechanism (the
``platform_ops`` tables) is described in :doc:`architecture`.

How do I turn a driver or feature on or off?
--------------------------------------------

Through the Kconfig system: edit the project's ``.conf`` defconfig, or use the
``menuconfig`` target to browse options interactively. See
:doc:`configuration_guide`.

How do I add support for a new device?
--------------------------------------

Write a driver against the ``no_os_*`` API following the conventions in
:doc:`drivers_guide` (init_param/descriptor structures, ``_init``/``_remove``
functions, coding style), then submit it per :doc:`contributing`.

What do the return codes mean?
------------------------------

no-OS functions return standard negative ``errno`` values on failure
(``-EINVAL``, ``-ENOMEM``, ``-ENOSYS``, ...); ``0`` means success. The helpers
in ``include/no_os_error.h`` (``NO_OS_IS_ERR_VALUE``, ``NO_OS_PTR_ERR``, ...)
test and cast these. There is also a no-OS-specific ``NO_OS_EOVERRUN`` for
circular-buffer overruns.

Where is the per-function API reference?
----------------------------------------

In the auto-generated
`No-OS Doxygen Documentation <https://analogdevicesinc.github.io/no-OS/doxygen>`_,
which has the exact signature and fields of every ``no_os_*`` function and
structure.

Where can I ask for help?
-------------------------

On the ADI `EngineerZone
<https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a>`_
microcontroller / no-OS forum.

Still stuck?
============

* Re-read the relevant guide: :doc:`getting_started`, :doc:`build_guide`,
  :doc:`configuration_guide`, or :doc:`architecture`.
* Reproduce with a clean build (``--clean``) to rule out stale artifacts.
* If the behavior looks platform-specific, try the same variant on another
  board or the ``linux`` platform to isolate whether it is in the shared code or
  the platform port.
* Search or ask on `EngineerZone
  <https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a>`_.
