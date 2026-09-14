MAX78000 no-OS Example Project
==============================

.. no-os-doxygen::

..contents::
        :depth: 3

Supported Evaluation Boards
---------------------------

* `MAX78000FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max78000fthr.html>`_

Overview
--------

The MAX78000FTHR is a rapid development platform for the MAX78000, an
ultra-low-power Arm Cortex-M4F microcontroller with an integrated
convolutional neural network (CNN) accelerator and an auxiliary RISC-V
co-processor. The board provides an on-board DAPLink debugger with a
USB-to-serial bridge, making it easy to build, flash, and interact with
firmware over UART.

This project is a minimal board bring-up for no-OS: it targets the MAX78000
directly (not an external peripheral) and demonstrates the base platform
features (clocks, UART stdio, GPIO, IRQ). RISC-V co-processor support is not
included in this project yet.

Applications
------------

* Board bring-up / smoke test
* Reference for no-OS ports to the MAX78000

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The MAX78000FTHR is powered over its USB connector (5V).

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from **Project Common Data Path**.

The macros used in Common Data are defined in platform specific files found in **Project Platform Configuration Path**.

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the UART and repeatedly prints a
"Hello world" message to the console. It exercises the base platform
(clock/UART bring-up) and is the recommended starting point when validating a
new setup.

This is the only example in this project, so it builds by default.

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX78000FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max78000fthr.html>`_

The basic example uses UART0, which the MAX78000FTHR routes to the on-board
DAPLink USB-to-serial bridge (no external wiring required). Open the resulting
serial port at 115200 8N1 to see the output.

**Build Command**

Available variants: ``basic``.
Available boards: ``max78000fthr``.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the project (basic example on the max78000fthr board)
   python tools/scripts/no_os_build.py build \
      --project max78000 --variant basic --board max78000fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max78000 --variant basic --board max78000fthr \
      --probe openocd --flash
