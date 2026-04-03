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

Flash Example
~~~~~~~~~~~~~

The flash example initializes the UART, IRQ controller, and flash
peripherals, then reads from and writes to flash memory at specific
addresses (0x3E000 and 0x3E004). The results of each read/write
operation are printed to the serial console.

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

.. code-block:: bash

   # remove sp characters from the build directory
   make reset
   # build the project
   make PLATFORM=aducm3029
   # flash the code
   make run
