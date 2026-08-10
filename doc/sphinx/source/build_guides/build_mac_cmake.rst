******************************************
macOS Build Prerequisites (CMake)
******************************************

This guide describes how to build a no-OS project for a **macOS** host
using the **CMake** build system, which is the only supported build flow
for this platform.

.. note::

   **This is a build guide for the macOS platform itself** — i.e. for
   producing executables that run *on* macOS. It is not a guide for
   cross-building any other no-OS platform *from* a Mac host. No
   cross-compiling is supported: the toolchain set up here only produces
   macOS executables for the host's own architecture.

The macOS platform targets applications that run natively on a Mac. For
projects that use the USB2PMB2 FTDI adapter (``mac-usb2pmb2`` board),
the ftd2xx library must be set up separately — see the
`ftd2xx build guide <build_ftd2xx_cmake.html>`_.

Apple Silicon and Intel Macs
-----------------------------

No manual architecture selection is required. ``CMakeLists.txt`` does not
pin ``CMAKE_OSX_ARCHITECTURES`` or otherwise force a target architecture,
so CMake resolves the Xcode Command Line Tools' ``clang`` on ``PATH`` and
builds natively for whichever architecture the host is running — arm64 on
Apple Silicon, x86_64 on Intel.

Supported boards
================

.. list-table::
   :header-rows: 1

   * - Preset / Board
     - Description
   * - ``mac-host``
     - Native macOS host
   * - ``mac-usb2pmb2``
     - macOS host with USB2PMB2 adapter (FT2232HQ, ftd2xx library)

Prerequisites
=============

Xcode Command Line Tools
-------------------------

The Xcode Command Line Tools provide the Clang compiler and standard
build utilities. Install them by running:

.. code-block:: bash

   xcode-select --install

Follow the on-screen prompts. Verify the compiler is available:

.. code-block:: bash

   clang --version

CMake and Ninja
---------------

Install **CMake 3.28+** and **Ninja** via `Homebrew <https://brew.sh/>`_:

.. code-block:: bash

   brew install cmake ninja

Verify:

.. code-block:: bash

   cmake --version
   ninja --version

Python 3
--------

Python 3 is available via Homebrew or the official installer:

.. code-block:: bash

   brew install python3

The build system creates a virtual environment under ``.no_os_venv`` on
first configure and installs the required dependencies (``kconfiglib``)
automatically.

Listing build combinations
==========================

From the no-OS repository root, list all valid macOS combinations:

.. code-block:: bash

   python3 tools/scripts/no_os_build.py list --board mac-host
   python3 tools/scripts/no_os_build.py list --board mac-usb2pmb2

Each row is a ``PROJECT  VARIANT  BOARD  PLATFORM`` tuple that can be
fed directly to the ``build`` subcommand.

Building a project
==================

Recommended: the build helper
------------------------------

.. code-block:: bash

   cd no-OS

   python3 tools/scripts/no_os_build.py build \
       --project <project> --variant <variant> --board mac-usb2pmb2

Replace ``mac-usb2pmb2`` with ``mac-host`` for projects that do not use
the FTDI adapter.

Useful options:

- ``--clean`` / ``--fresh`` — remove or refresh the build directory.
- ``-j N`` / ``--jobs N`` — parallel compile jobs.
- ``--dry-run`` — print the ``cmake`` commands without running them.

The executable is placed in
``build/<project>-<variant>-<board>/build/<project>``.

Manual CMake invocation
-----------------------

You can also drive CMake directly from a Terminal:

.. code-block:: bash

   cmake --preset mac-usb2pmb2 \
         -B build-<project>-<variant>-mac-usb2pmb2 \
         -DPROJECT_DEFCONFIG=<project>/<variant>.conf

   cmake --build build-<project>-<variant>-mac-usb2pmb2 --target <project>

The built executable is placed in
``build-<project>-<variant>-mac-usb2pmb2/build/<project>``.

Running
=======

Run the executable directly from a Terminal:

.. code-block:: bash

   ./build/<project>-<variant>-<board>/build/<project>

IIO projects
------------

Projects built with the ``iio_example`` variant start an IIOD server on
the host. Connect to it from any IIO client using the IP backend:

.. code-block:: bash

   iio_info -u ip:127.0.0.1

Example: building max14916 basic
=================================

.. note::

   This example uses the ``mac-usb2pmb2`` board which requires the ftd2xx
   library. Set it up first following the
   `ftd2xx build guide <build_ftd2xx_cmake.html>`_.

.. code-block:: bash

   cd no-OS

   python3 tools/scripts/no_os_build.py build \
       --project max14916 --variant basic --board mac-usb2pmb2

   ./build/max14916-basic-mac-usb2pmb2/build/max14916
