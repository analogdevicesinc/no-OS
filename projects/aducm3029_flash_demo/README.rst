ADuCM3029 Flash Demo no-OS Example Project
===========================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Overview
--------

The ADuCM3029 Flash Demo is a no-OS example project that demonstrates
basic flash memory read and write operations on the ADuCM3029
microcontroller. The project initializes UART, interrupt controller, and
flash memory peripherals, then performs a series of flash read/write
operations at user-defined addresses. All results are reported via the
UART serial console at 115200 baud, providing a straightforward
demonstration of the no-OS flash memory API.

Applications
------------

* Flash memory read/write verification
* ADuCM3029 platform bring-up and validation
* Non-volatile storage development reference

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADICUP3029 board can be powered through the micro-USB connector
(P10), the DC barrel jack (P2, +7 V to +12 V DC input), or a battery
header (BT1). The USB connection provides power and also serves as the
programming and debug interface.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. The initialization
data is defined in ``src/common`` and the platform-specific macros in
``src/platform/aducm3029``.

Flash Example
~~~~~~~~~~~~~

The flash example (``flash`` variant, the default) initializes the
UART, IRQ controller, and flash peripherals, then reads from and writes to
flash memory at specific addresses (0x3E000 and 0x3E004). The results of each
read/write operation are printed to the serial console.

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADICUP3029 to a PC via the micro-USB connector (P10).
This provides power, programming, and serial communication. Ensure the
UART switch (S2) is set to the USB position for PC terminal access.

Build Command
^^^^^^^^^^^^^

Available variants: ``flash``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3
   # Windows (PowerShell): $env:CCES_HOME = "C:\analog\cces\3.0.3"

   cd no-OS

   # build the flash example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project aducm3029_flash_demo --variant flash --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project aducm3029_flash_demo --variant flash --board eval-adicup3029 \
      --probe openocd --flash
