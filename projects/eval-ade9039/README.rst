EVAL-ADE9039 no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* :adi:`EVAL-ADE9039`

Overview
--------

The ADE9039 is a high performance, polyphase energy and power quality
measurement IC intended for use with di/dt (Rogowski) current sensors. It
provides total and fundamental active, reactive, and apparent energy, current
and voltage RMS, power factor, THD, line period, and phase angle measurements
per phase, and communicates over a 10 MHz SPI port from a single 3.3 V supply.

This project runs on the AD-APARD32690-SL platform board (MAX32690) and
demonstrates a minimal SPI read example: it initializes the ADE9039, starts the
metrology DSP, and in a loop reads and prints the Phase A RMS voltage and
current, active/reactive/apparent power, energy, power factor, and the on-chip
temperature over the UART console.

Applications
------------

* Advanced metering infrastructure (AMI)
* Data center power distribution units (PDU)
* Sub-meters
* Smart circuit breakers
* Home energy management devices

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADE9039 operates from a single 3.3 V supply. On the evaluation setup, the
AD-APARD32690-SL carrier board is powered over its USB-C connector, and the
ADE9039 board is supplied from the carrier through the interconnect.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

The ADE9039 is connected to the MAX32690 on the AD-APARD32690-SL through the
SPI bus and three control GPIOs. The default pin assignment used by this
project is:

============ ============ ===================================================
Signal       MCU Pin      Function
============ ============ ===================================================
SPI (ID 1)   SPI1         ADE9039 register access, MODE 0, 1 MHz
PSM0         P2.8         ADE9039 power mode select 0
PSM1         P2.6         ADE9039 power mode select 1
RESET        P2.7         ADE9039 hardware reset (active low)
LED          P2.1         User activity LED
UART (ID 0)  UART0        Console, 115200 baud, 8-N-1
============ ============ ===================================================

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow. Selecting
a variant at build time (``--variant <name>``) chooses which application is
compiled.

The initialization data used in the example is taken out from:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ade9039/src/platform>`_

ADE9039 Example
~~~~~~~~~~~~~~~

The ``ade9039_example`` variant initializes the ADE9039 over SPI, applies the
default measurement configuration, and starts the metrology DSP. In its main
loop it polls the ``STATUS0`` register and, when the corresponding ready flags
are set, reads and prints over the UART console:

* Phase A RMS voltage (mV) and RMS current (mA)
* Phase A total and fundamental power factor
* Phase A active, reactive, and apparent power, and the accumulated energy
* The on-chip temperature in degrees Celsius

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* :adi:`EVAL-ADE9039`
* :adi:`AD-APARD32690-SL`

Connections
^^^^^^^^^^^

Connect the ADE9039 board to the AD-APARD32690-SL using the SPI bus and the
PSM0, PSM1, and reset control lines as listed in the On-board Connectors
table above. The UART console is exposed over the AD-APARD32690-SL USB-C
connection and runs at 115200 baud, 8 data bits, no parity, 1 stop bit.

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``ade9039_example``. Available boards:
``ad-apard32690-sl``.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # PowerShell (Windows) equivalent:
   #   $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the example on the target board
   python tools/scripts/no_os_build.py build \
      --project eval-ade9039 --variant ade9039_example --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-ade9039 --variant ade9039_example --board ad-apard32690-sl \
      --probe openocd --flash
