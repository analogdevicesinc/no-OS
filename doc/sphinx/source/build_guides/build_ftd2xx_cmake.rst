*****************************************************
Building with the ftd2xx Library (USB2PMB2, CMake)
*****************************************************

This guide describes how to build a no-OS project that uses the **ftd2xx**
library (FTDI D2XX + LibMPSSE) with the **USB2PMB2** evaluation board
(FT2232HQ USB-to-MPSSE adapter) as a host-side peripheral bridge.

The ftd2xx library is supported on three host operating systems:

- **Linux** — ``usb2pmb2`` board preset (linux-userspace platform)
- **Windows** — ``win-usb2pmb2`` board preset (win platform)
- **macOS** — ``mac-usb2pmb2`` board preset (mac platform)

The resulting binary runs natively on the host PC and communicates with
the target evaluation board over USB via the USB2PMB2 adapter.

Supported boards
================

.. list-table::
   :header-rows: 1

   * - Preset / Board
     - Host OS
     - Description
   * - ``usb2pmb2``
     - Linux
     - Linux host with USB2PMB2 adapter (linux-userspace platform)
   * - ``win-usb2pmb2``
     - Windows
     - Windows host with USB2PMB2 adapter (win platform)
   * - ``mac-usb2pmb2``
     - macOS
     - macOS host with USB2PMB2 adapter (mac platform)

Prerequisites
=============

CMake and build tools
---------------------

All host OSes require **CMake 3.28+** and **Ninja**.

.. tab-set::

   .. tab-item:: Linux (Debian/Ubuntu)

      .. code-block:: bash

         sudo apt install cmake ninja-build build-essential python3 python3-venv

   .. tab-item:: Windows

      Install the following tools (ensure they are on ``PATH``):

      - `CMake <https://cmake.org/download/>`_ 3.28+
      - `Ninja <https://ninja-build.org/>`_
      - `MinGW-w64 <https://www.mingw-w64.org/>`_ (GCC for Windows) or
        MSVC (Visual Studio Build Tools)
      - `Python 3 <https://www.python.org/downloads/>`_

   .. tab-item:: macOS

      Install Xcode Command Line Tools and Homebrew, then:

      .. code-block:: bash

         brew install cmake ninja python3

ftd2xx library setup
--------------------

FTDI's website uses Cloudflare bot protection that blocks automated
downloads. The required archives must be downloaded manually first.

1. **Download LibMPSSE** for your host OS from FTDI's website:

   - Linux:   ``D2XX_MPSSE_1.0.7_Linux.zip``
   - macOS:   ``D2XX_MPSSE_1.0.7_MacOS.zip``
   - Windows: ``D2XX_MPSSE_1.0.7_Win.zip``

2. **Download libftd2xx** for your host OS and architecture from FTDI's
   `D2XX drivers page <https://ftdichip.com/drivers/d2xx-drivers/>`_.
   For Linux x86_64 the file is ``libftd2xx-linux-x86_64-1.4.35.tgz``.
   Windows uses ``CDM-v2.12.36.20-WHQL-Certified.zip``.

3. **Run the setup script** from the no-OS root directory:

   .. tab-set::

      .. tab-item:: Linux

         .. code-block:: bash

            python3 tools/scripts/config_ftd2xx.py \
                --local-mpsse D2XX_MPSSE_1.0.7_Linux.zip \
                --local-d2xx  libftd2xx-linux-x86_64-1.4.35.tgz

      .. tab-item:: Windows

         .. code-block:: bat

            python tools\scripts\config_ftd2xx.py ^
                --local-mpsse D2XX_MPSSE_1.0.7_Win.zip ^
                --local-d2xx  CDM-v2.12.36.20-WHQL-Certified.zip

      .. tab-item:: macOS

         .. code-block:: bash

            python3 tools/scripts/config_ftd2xx.py \
                --local-mpsse D2XX_MPSSE_1.0.7_MacOS.zip \
                --local-d2xx  D2XX1.4.35.dmg

The script installs both libraries to ``libraries/ftd2xx/release/``
(which is git-ignored). CMake finds them automatically from that location
— no ``FTD2XX_LIBRARY`` environment variable is needed.

Linux: kernel module conflict
------------------------------

The ``ftdi_sio`` kernel module claims FTDI USB devices automatically and
must be unloaded before running any ftd2xx application:

.. code-block:: bash

   sudo rmmod ftdi_sio usbserial

This must be repeated every time the adapter is replugged. To make it
permanent, blacklist the module:

.. code-block:: bash

   echo "blacklist ftdi_sio" | sudo tee /etc/modprobe.d/ftdi_sio.conf
   sudo update-initramfs -u

Listing build combinations
==========================

From the no-OS repository root, list all valid USB2PMB2 combinations:

.. code-block:: bash

   python3 tools/scripts/no_os_build.py list --board usb2pmb2

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be
fed directly to the ``build`` subcommand.

Building a project
==================

Recommended: the build helper
------------------------------

The simplest way to build is via ``no_os_build.py``:

.. tab-set::

   .. tab-item:: Linux

      .. code-block:: bash

         cd no-OS
         python3 tools/scripts/no_os_build.py build \
             --project <project> --variant <variant> --board usb2pmb2

   .. tab-item:: Windows

      .. code-block:: bat

         cd no-OS
         python tools\scripts\no_os_build.py build ^
             --project <project> --variant <variant> --board win-usb2pmb2

   .. tab-item:: macOS

      .. code-block:: bash

         cd no-OS
         python3 tools/scripts/no_os_build.py build \
             --project <project> --variant <variant> --board mac-usb2pmb2

Useful options:

- ``--clean`` / ``--fresh`` — remove or refresh the build directory.
- ``-j N`` / ``--jobs N`` — parallel compile jobs.
- ``--dry-run`` — print the ``cmake`` commands without running them.

The executable is placed in
``build/<project>-<variant>-<board>/build/<project>``.

Manual CMake invocation
-----------------------

You can also drive CMake directly:

.. code-block:: bash

   cmake --preset usb2pmb2 \
         -B build-<project>-<variant>-usb2pmb2 \
         -DPROJECT_DEFCONFIG=<project>/<variant>.conf

   cmake --build build-<project>-<variant>-usb2pmb2 --target <project>

Replace ``usb2pmb2`` with ``win-usb2pmb2`` or ``mac-usb2pmb2`` for
Windows and macOS respectively.

Running
=======

.. tab-set::

   .. tab-item:: Linux

      On Linux, ``libftd2xx`` accesses the USB device directly and
      typically requires root (or a udev rule granting access):

      .. code-block:: bash

         sudo ./build/<project>-<variant>-usb2pmb2/build/<project>

   .. tab-item:: Windows

      Run the executable from a Command Prompt or PowerShell.
      The FTDI D2XX driver is installed system-wide (CDM driver package)
      so no elevated privileges are normally required:

      .. code-block:: bat

         .\build\<project>-<variant>-win-usb2pmb2\build\<project>.exe

   .. tab-item:: macOS

      .. code-block:: bash

         ./build/<project>-<variant>-mac-usb2pmb2/build/<project>

IIO projects
------------

Projects built with the ``iio_example`` variant start an IIOD server
on the host. Connect to it from any IIO client using the IP backend:

.. code-block:: bash

   iio_info -u ip:127.0.0.1

Example: building max14916 basic on Linux
=========================================

.. code-block:: bash

   cd no-OS

   # Install ftd2xx libraries (one-time setup)
   python3 tools/scripts/config_ftd2xx.py \
       --local-mpsse D2XX_MPSSE_1.0.7_Linux.zip \
       --local-d2xx  libftd2xx-linux-x86_64-1.4.35.tgz

   # Unload the conflicting kernel module
   sudo rmmod ftdi_sio usbserial

   # Build
   python3 tools/scripts/no_os_build.py build \
       --project max14916 --variant basic --board usb2pmb2

   # Run
   sudo ./build/max14916-basic-usb2pmb2/build/max14916
