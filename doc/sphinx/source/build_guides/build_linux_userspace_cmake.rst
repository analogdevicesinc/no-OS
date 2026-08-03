*******************************************
Linux Userspace Build Prerequisites (CMake)
*******************************************

This guide describes how to build a no-OS project for the
**linux-userspace** platform using the **CMake** build system, which is
the only supported build flow for this platform.

The linux-userspace platform targets Linux userspace applications that
communicate with hardware directly through Linux kernel interfaces such
as ``spidev``, ``i2c-dev``, and ``gpio-cdev``. The resulting binary is a
native Linux executable that runs on the same machine as the build, with
no cross-compilation or debug probe required.

.. note::

   The linux-userspace platform requires a Linux host with the relevant
   kernel modules loaded (``spidev``, ``i2c-dev``, etc.) and access to
   the corresponding ``/dev`` nodes.  On most single-board computers
   (Raspberry Pi, BeagleBone, etc.) these are enabled by default.  On a
   standard desktop/server Linux you may need to load the modules and
   configure the device tree.

Supported boards
================

.. list-table::
   :header-rows: 1

   * - Preset / Board
     - Description
   * - ``linux-userspace``
     - Any Linux host with spidev / i2c-dev / gpio-cdev support

Prerequisites
=============

- **CMake** 3.28 or newer (presets v7 are used).
- **Ninja** build system.
- **GCC** (the system C compiler — no cross-toolchain needed).
  On Debian/Ubuntu:

  .. code-block:: bash

      $ sudo apt install build-essential

- **Python 3**. The build creates a private virtual environment under
  ``.no_os_venv`` on first configure and installs the dependencies listed
  in ``tools/scripts/requirements.txt`` (``kconfiglib`` and friends).

Install all prerequisites in one step on Debian/Ubuntu:

  .. code-block:: bash

      $ sudo apt install cmake ninja-build build-essential python3 python3-venv

Kernel modules
--------------

Ensure the required kernel modules are loaded before running the
application.  For SPI and I2C (the most common peripherals):

.. code-block:: bash

    $ sudo modprobe spidev
    $ sudo modprobe i2c-dev

To make them persistent across reboots, add them to ``/etc/modules``.

.. code-block:: bash

    $ echo -e "spidev\ni2c-dev" | sudo tee -a /etc/modules

Verify that the corresponding device nodes exist:

.. code-block:: bash

    $ ls /dev/spidev*   # e.g. /dev/spidev0.0
    $ ls /dev/i2c-*     # e.g. /dev/i2c-0

Listing build combinations
==========================

From the no-OS repository root, list all valid linux-userspace
combinations:

.. code-block:: bash

    $ python tools/scripts/no_os_build.py list --board linux-userspace

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be
fed back to the ``build`` subcommand.

Building a project
==================

Recommended: the build helper
------------------------------

The simplest way to configure and build is via ``no_os_build.py``, which
selects the correct preset, defconfig and board config automatically:

.. code-block:: bash

    $ python tools/scripts/no_os_build.py build \
          --project <project> --variant <variant> --board linux-userspace

Useful options:

- ``--clean`` — remove the build directory before configuring.
- ``-j N`` / ``--jobs N`` — parallel compile jobs.
- ``--parallel`` — build different boards in parallel.
- ``--dry-run`` — print the ``cmake`` commands without running them.

.. note::

   The ``--probe`` and ``--flash`` options are not available for the
   linux-userspace platform. There is nothing to flash — the output is a
   native executable that you run directly on the target.

Each combination is built into its own directory named
``build/<project>-<variant>-<board>`` at the repo root (override with
``--build-dir``). The executable is placed in
``<build-dir>/build/<project>``.

Manual CMake invocation
-----------------------

You can also drive CMake directly. Configure with the board preset and
select the project/variant with ``PROJECT_DEFCONFIG``:

.. code-block:: bash

    $ cmake --preset linux-userspace \
          -B build-<project>-<variant>-linux-userspace \
          -DPROJECT_DEFCONFIG=<project>/<variant>.conf

Then build the project target:

.. code-block:: bash

    $ cmake --build build-<project>-<variant>-linux-userspace \
          --target <project>

The built executable is placed in
``build-<project>-<variant>-linux-userspace/build/<project>``.

.. note::

   When the project provides board-specific config files under
   ``projects/<project>/boards/<variant>/linux-userspace.conf``, that
   file is layered on top of the variant config and the platform
   defconfig automatically.

.. note::

   If no ``CMAKE_BUILD_TYPE`` is given, the build defaults to
   ``RelWithDebInfo``. Use ``-DCMAKE_BUILD_TYPE=Debug`` for an
   unoptimized debug build.

Configuring options (menuconfig)
---------------------------------

Kconfig options can be browsed and changed interactively after
configuring:

.. code-block:: bash

    $ cmake --build build-<project>-<variant>-linux-userspace \
          --target menuconfig

Running
=======

Build machine vs. target machine
---------------------------------

The linux-userspace platform produces a **native ELF** for the
architecture of the machine that compiled it.  The binary must
therefore run on a machine with the **same CPU architecture** as the
build host.

The simplest workflow is to build directly on the target board (e.g.
log into a Raspberry Pi over SSH and run ``no_os_build.py`` there).
If you prefer to build on a separate development machine of the same
architecture, copy the resulting ELF to the target afterwards:

.. code-block:: bash

    $ scp build/<project>-<variant>-linux-userspace/build/<project> \
          user@<target-ip>:~/<project>

Alternatively, use a USB stick or SD card to transfer the file.

.. note::

   Building on an x86-64 desktop and running on an ARM board (e.g.
   Raspberry Pi) requires a cross-compiler and a matching sysroot —
   the host ``gcc`` alone is not sufficient.  For that workflow, set
   ``CMAKE_C_COMPILER`` to the cross-compiler in a custom toolchain
   file and point ``CMAKE_SYSROOT`` at the target sysroot.  The
   simplest alternative is always to build natively on the board
   itself.

Executing the binary
---------------------

Access to ``/dev`` nodes usually requires ``root`` or membership in
the ``spi`` / ``i2c`` / ``gpio`` groups:

.. code-block:: bash

    $ sudo ./build/<project>-<variant>-linux-userspace/build/<project>

To grant access without ``sudo``, add your user to the relevant groups:

.. code-block:: bash

    $ sudo usermod -aG spi,i2c,gpio $USER
    # Log out and back in for the change to take effect.

Example: building the ad9545 basic example
==========================================

.. code-block:: bash

    $ cd no-OS

    $ python tools/scripts/no_os_build.py build \
          --project ad9545 --variant basic_example --board linux-userspace

    $ sudo ./build/ad9545-basic_example-linux-userspace/build/ad9545
