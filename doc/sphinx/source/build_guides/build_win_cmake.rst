******************************************
Windows Build Prerequisites (CMake)
******************************************

This guide describes how to build a no-OS project for a **Windows** host
using the **CMake** build system, which is the only supported build flow
for this platform.

.. note::

   **This is a build guide for the Windows platform itself** — i.e. for
   producing executables that run *on* Windows. It is not a guide for
   cross-building any other no-OS platform *from* a Windows host. No
   cross-compiling is supported: the toolchain set up here only produces
   Windows executables, and only for **x86_64**. There is no ARM64 Windows
   build support (unlike the macOS platform, which supports both Intel and
   Apple Silicon natively — see the
   `macOS build guide <build_mac_cmake.html>`_).

The Windows platform targets applications that run natively on a Windows
PC. For projects that use the USB2PMB2 FTDI adapter (``win-usb2pmb2``
board), the ftd2xx library must be set up separately — see the
`ftd2xx build guide <build_ftd2xx_cmake.html>`_.

All commands below are for **PowerShell**. No MSYS2 shell is required
for building — MSYS2 is installed only to obtain the GCC toolchain.

Supported boards
================

.. list-table::
   :header-rows: 1

   * - Preset / Board
     - Description
   * - ``win-host``
     - Native Windows host
   * - ``win-usb2pmb2``
     - Windows host with USB2PMB2 adapter (FT2232HQ, ftd2xx library)

Prerequisites
=============

All one-time setup steps below only need to be performed once per machine.

Git
---

.. code-block:: powershell

   winget install Git.Git

Close and reopen PowerShell after installation so that ``git`` is on
``PATH``.

CMake and Ninja
---------------

.. code-block:: powershell

   winget install Kitware.CMake
   winget install Ninja-build.Ninja

Verify:

.. code-block:: powershell

   cmake --version
   ninja --version

Python 3
--------

.. note::

   The Python installer requires elevation. Run PowerShell **as
   Administrator** for this step.

.. code-block:: powershell

   winget install Python.Python.3.13

Verify (in a new PowerShell window):

.. code-block:: powershell

   python --version

The build system creates a virtual environment under ``.no_os_venv`` on
first configure and installs the required dependencies (``kconfiglib``)
automatically.

Compiler (MinGW-w64 GCC)
-------------------------

Install `MSYS2 <https://www.msys2.org/>`_ using its official installer,
then open the **MSYS2 MinGW64** shell and run:

.. code-block:: bash

   pacman -S mingw-w64-x86_64-gcc

Add the MinGW-w64 ``bin`` directory to your **system** ``PATH``
permanently via *System Properties → Advanced → Environment Variables*:

.. code-block:: text

   C:\msys64\mingw64\bin

Verify from a new PowerShell window:

.. code-block:: powershell

   gcc --version

FTDI CDM driver (USB2PMB2 boards only)
---------------------------------------

The CDM (Combined Driver Model) package provides the Windows USB driver
for FTDI chips. Download ``CDM-v2.12.36.20-WHQL-Certified.zip`` from
the FTDI website, extract it, then install:

1. Plug in the USB2PMB2 board.
2. Open **Device Manager** (``devmgmt.msc``).
3. Locate the FTDI device under *Other devices* or *Universal Serial Bus
   devices*.
4. Right-click → **Update driver** → **Browse my computer for drivers**.
5. Point to the extracted CDM folder → **Next**.

After installation the device appears as *USB Serial Converter A* and
*USB Serial Converter B*.

Visual C++ 2013 Runtime (USB2PMB2 boards only)
-----------------------------------------------

The prebuilt ``libmpsse.dll`` shipped by FTDI requires the Visual C++
2013 Runtime (``MSVCR120.dll``). Install it with:

.. code-block:: powershell

   winget install Microsoft.VCRedist.2013.x64

.. note::

   Without this runtime the executable exits silently with no output.

Listing build combinations
==========================

From the no-OS repository root, list all valid Windows combinations:

.. code-block:: powershell

   python tools\scripts\no_os_build.py list --board win-host
   python tools\scripts\no_os_build.py list --board win-usb2pmb2

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be
fed directly to the ``build`` subcommand.

Building a project
==================

Recommended: the build helper
------------------------------

.. code-block:: powershell

   cd no-OS

   python tools\scripts\no_os_build.py build `
       --project <project> --variant <variant> --board win-usb2pmb2

Replace ``win-usb2pmb2`` with ``win-host`` for projects that do not use
the FTDI adapter.

Useful options:

- ``--clean`` / ``--fresh`` — remove or refresh the build directory.
- ``-j N`` / ``--jobs N`` — parallel compile jobs.
- ``--dry-run`` — print the ``cmake`` commands without running them.

The executable is placed in
``build\<project>-<variant>-<board>\build\<project>.exe``.

.. note::

   For ``win-usb2pmb2`` builds, the required FTDI runtime DLLs
   (``FTD2XX.dll`` and ``libmpsse.dll``) are copied automatically next
   to the executable by the build system.

Manual CMake invocation
-----------------------

You can also drive CMake directly from PowerShell. Ensure ``cmake``,
``ninja``, and ``gcc`` are all on ``PATH``:

.. code-block:: powershell

   cmake --preset win-usb2pmb2 `
         -B build-<project>-<variant>-win-usb2pmb2 `
         -DPROJECT_DEFCONFIG=<project>/<variant>.conf

   cmake --build build-<project>-<variant>-win-usb2pmb2 --target <project>

The built executable is placed in
``build-<project>-<variant>-win-usb2pmb2\build\<project>.exe``.

Running
=======

Run the executable directly from PowerShell:

.. code-block:: powershell

   .\build\<project>-<variant>-<board>\build\<project>.exe

IIO projects
------------

Projects built with the ``iio_example`` variant are not supported on
the ``win-usb2pmb2`` board — there is no Windows socket backend in the
ftd2xx platform. Use the ``win-host`` board for IIO projects that do not
require FTDI hardware.

Example: building max31827-evkit basic
=======================================

.. note::

   This example uses the ``win-usb2pmb2`` board which requires the ftd2xx
   library. Set it up first following the
   `ftd2xx build guide <build_ftd2xx_cmake.html>`_.

.. code-block:: powershell

   cd no-OS

   python tools\scripts\no_os_build.py build `
       --project max31827-evkit --variant basic --board win-usb2pmb2

   .\build\max31827-evkit-basic-win-usb2pmb2\build\max31827-evkit.exe
