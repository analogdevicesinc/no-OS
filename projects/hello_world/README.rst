Hello World no-OS Example Project
==================================

Overview
--------

This is the simplest possible no-OS project: it initialises the board's UART
peripheral and prints a "Hello World" counter message over the serial port once
per second.  It is intended as a **quick-start template** for engineers who are
new to no-OS and want to understand how to build, flash, and run a bare-metal
embedded application on a Xilinx board.

What it demonstrates:

- How a no-OS project is structured (``Makefile``, ``src.mk``, platform tree)
- The no-OS peripheral descriptor pattern (``no_os_uart_init`` / ``no_os_uart_remove``)
- The no-OS logging macros (``pr_info``, ``pr_err``, …)
- The no-OS delay API (``no_os_mdelay``)
- Proper error handling (checking return values, cleanup on failure)

Supported Platforms
-------------------

Xilinx
^^^^^^

The project is generic — no board-specific code is required.  The same source
builds correctly for:

- **Zynq-7000**: Cora Z7, ZED, ZC706, and any other Zynq-7000 carrier
- **ZynqMP (Zynq UltraScale+)**: ZCU102, and any other ZynqMP carrier

The target architecture (Cortex-A9 for Zynq-7000, Cortex-A53 for ZynqMP) is
auto-detected from the XSA file you provide — see `Hardware Requirements`_
below.

Project Layout
--------------

::

   no-OS/projects/hello_world/
   ├── Makefile
   ├── README.rst
   ├── builds.json
   ├── src.mk
   └── src/
       ├── app/
       │   ├── hello_world.c      ← application logic (platform-agnostic)
       │   └── hello_world.h
       └── platform/
           ├── platform_includes.h
           └── xilinx/
               ├── main.c         ← Xilinx entry point (cache enable)
               ├── parameters.c   ← UART init parameters
               ├── parameters.h   ← UART peripheral IDs
               └── platform_src.mk

Hardware Requirements
---------------------

Xilinx builds require an **XSA file** (hardware description exported from
Vivado) to be present in the project directory before running ``make``.  The
build system uses it to generate the BSP (board support package), linker
script, and the ``xparameters.h`` header that provides peripheral base
addresses.

hello_world uses only the ARM Processing System (PS) and its built-in UART —
no custom PL logic is needed.  **Any XSA that targets your board will work**,
even one borrowed from a different no-OS or HDL project.

Where to get an XSA
^^^^^^^^^^^^^^^^^^^

**Option 1 — Reuse an existing XSA (easiest).**

If you already have an XSA from any other no-OS or Vivado project that targets
your board, copy it here::

   cp /path/to/existing/system_top.xsa .

For example, if you have a ZCU102 build of ``adrv9001``::

   cp ../adrv9001/system_top.xsa .

**Option 2 — Build an XSA from the ADI HDL repository.**

The `ADI HDL repository <https://github.com/analogdevicesinc/hdl>`__ contains
reference designs for many boards.  Follow the
`ADI HDL build guide <https://analogdevicesinc.github.io/hdl/user_guide/build_hdl.html>`__
to generate a ``system_top.xsa`` for your board, then copy it here.

Build
-----

.. code-block:: bash

   # 1. Enter the project directory
   cd no-OS/projects/hello_world/

   # 2. Place your XSA here (see Hardware Requirements above)
   cp /path/to/your/system_top.xsa .

   # 3. Build
   make

Useful build variants:

.. code-block:: bash

   # Build with full debug symbols and -O0 optimisation (recommended for stepping through code)
   make DEBUG=1

   # Remove all generated files and start fresh
   make reset

   # Open the project in Vitis for GUI-based editing or debugging
   make sdkopen

Flash and Run
-------------

From the command line
^^^^^^^^^^^^^^^^^^^^^

After a successful ``make``, program the board via JTAG and run the
application:

.. code-block:: bash

   make run

This calls ``xsct`` (Xilinx System Console Tool) to download and execute the
ELF on the connected board.

From Vitis (GUI)
^^^^^^^^^^^^^^^^

Open the generated Vitis workspace with ``make sdkopen``, then follow the
no-OS build guide for your Vitis version to run or debug the application:

https://analogdevicesinc.github.io/no-OS/build_guide.html

Viewing UART Output
-------------------

The application prints over the board's USB-UART bridge at **115200 baud,
8N1, no flow control**.

Connect the board to your PC with a USB cable before powering it on.  The
USB-UART adapter appears as a serial device on the host:

- **Linux**: ``/dev/ttyUSB0`` (Cora Z7) or ``/dev/ttyUSB1`` (ZCU102 — the
  second of the four USB-serial interfaces exposed by the FTDI chip)
- **Windows**: a COM port listed in Device Manager under *Ports (COM & LPT)*
- **macOS**: ``/dev/tty.usbserial-*``

Using picocom (Linux)
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   sudo picocom /dev/ttyUSB0 -b 115200

Press **Ctrl-A Ctrl-X** to exit picocom.

Using screen (Linux/macOS)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   sudo screen /dev/ttyUSB0 115200

Press **Ctrl-A K** to exit screen.

Using a GUI terminal (Windows / cross-platform)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configure any serial terminal application (PuTTY, TeraTerm, CoolTerm) with:

=============  =======
Setting        Value
=============  =======
Baud rate      115200
Data bits      8
Parity         None
Stop bits      1
Flow control   None
=============  =======

Expected Output
---------------

After programming the board and opening the serial terminal, you should see:

.. code-block:: text

   *** no-OS Hello World ***

   Hello World #1
   Hello World #2
   Hello World #3
   ...

A new line is printed every second.  If nothing appears, check that:

- You are connected to the correct serial device.
- The baud rate is exactly 115200.
- The board has been powered on and the ELF has been programmed successfully
  (``make run`` completed without errors).

Build Flags Reference
---------------------

.. list-table::
   :header-rows: 1

   * - Flag
     - Default
     - Description
   * - ``DEBUG=1``
     - not set
     - Build with ``-O0`` optimisation and full debug symbols (``-g3``)

Adding Support for Other Platforms
-----------------------------------

The project is structured so that adding a new platform (STM32, Maxim, etc.)
requires only a new ``src/platform/<platform>/`` directory containing
``main.c``, ``parameters.h``, ``parameters.c``, and ``platform_src.mk``.
Use the existing ``src/platform/xilinx/`` directory as a reference.

no-OS Build Guide
-----------------

For a full description of the no-OS build system, prerequisites, and
toolchain setup, refer to the official build guide:

https://analogdevicesinc.github.io/no-OS/build_guide.html
