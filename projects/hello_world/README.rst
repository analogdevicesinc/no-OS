Hello World no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Overview
--------

This is the simplest possible no-OS project: it initializes the board's UART
peripheral and prints a "Hello World" counter message over the serial port once
per second. It is intended as a **quick-start template** for engineers who are
new to no-OS and want to understand how to build, flash, and run a bare-metal
embedded application on a Xilinx board.

What it demonstrates:

- The no-OS peripheral descriptor pattern (``no_os_uart_init`` / ``no_os_uart_remove``)
- The no-OS logging macros (``pr_info``, ``pr_err``, ...)
- The no-OS delay API (``no_os_mdelay``)
- Proper error handling (checking return values, cleanup on failure)

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example.
Platform-specific init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/hello_world/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

Initializes the board's UART peripheral and prints a "Hello World" counter
message over the serial port once per second. No external hardware is required
beyond the board itself.

This example is built by selecting the ``basic`` variant (see the Build Command
section below).

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* Any Xilinx FPGA carrier board that exposes a PS UART (ZedBoard, ZC706,
  ZCU102, Cora Z7, and others). No eval board is required.

Connections
^^^^^^^^^^^

No external hardware connections are required beyond the carrier board's USB
cable. The application uses only the ARM Processing System (PS) and its
built-in UART — no custom PL logic is needed. Any XSA that targets your
board will work, even one borrowed from a different no-OS or HDL project.

The UART console appears on the board's USB-UART adapter at **115200 baud,
8N1, no flow control**:

- **Linux**: ``/dev/ttyUSB0`` (ZedBoard/Cora Z7) or ``/dev/ttyUSB1`` (ZCU102)
- **Windows**: a COM port listed in Device Manager under *Ports (COM & LPT)*
- **macOS**: ``/dev/tty.usbserial-*``

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your Vitis install)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "<path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # build the basic example on the ZedBoard (requires a .xsa hardware file)
   python tools/scripts/no_os_build.py build \
      --project hello_world --variant basic --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project hello_world --variant basic --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

Where to get an XSA
^^^^^^^^^^^^^^^^^^^

**Option 1 — Reuse an existing XSA (easiest).**

If you already have an XSA from any other no-OS or Vivado project that targets
your board, pass it directly with ``--hardware``::

   python tools/scripts/no_os_build.py build \
      --project hello_world --variant basic --board zed \
      --hardware /path/to/existing/system_top.xsa

**Option 2 — Build an XSA from the ADI HDL repository.**

The `ADI HDL repository <https://github.com/analogdevicesinc/hdl>`__ contains
reference designs for many boards. Follow the
`ADI HDL build guide <https://analogdevicesinc.github.io/hdl/user_guide/build_hdl.html>`__
to generate a ``system_top.xsa`` for your board.

Expected Output
---------------

After programming the board and opening a serial terminal at 115200 baud, you
should see:

.. code-block:: text

   *** no-OS Hello World ***

   Hello World #1
   Hello World #2
   Hello World #3
   ...

A new line is printed every second. If nothing appears, check that you are
connected to the correct serial device and that the baud rate is exactly 115200.
