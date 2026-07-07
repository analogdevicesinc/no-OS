EVAL-LTC4306 no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-LTC4306 <https://www.analog.com/EVAL-LTC4306>`_

Overview
--------

The EVAL-LTC4306 features the LTC4306, a 4-channel, 2-wire I2C bus and
SMBus-compatible multiplexer with bus buffers that provide capacitive
isolation between the upstream bus and downstream buses. This evaluation
board provides 6-pin PMOD connectors for upstream and downstream
connection for compatibility with PMOD form factors such as
EVAL-ADICUP3029.

The EVAL-LTC4306 comes with PMOD connectors and 10 kOhm pull-up
resistors on both the upstream and downstream side, and 0.1 uF bypass
capacitor on the upstream side. The user also has the option to add a
6-pin, 2.54 mm pitch pin header for connection to the GPIO pins for
added functionality.

Applications
------------

* Nested addressing
* 5 V/3.3 V level translator
* Capacitance buffer/bus extender

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

Each bus can be pulled up to a supply voltage ranging from 2.7 V to
5.5 V, independent of the LTC4306 supply voltage. Jumper configuration
(JP1-JP2) allows individual power supplies per downstream bus, shared
upstream voltage, or level shifting between different voltage domains.
The board includes 0.1 uF bypass capacitors and optional 0805 footprint
capacitor locations for additional decoupling.

Board Connectors
~~~~~~~~~~~~~~~~

* **P1** - 6-pin PMOD upstream connector (SDA, SCL, VCC, GND, ENABLE)
* **P5-P11** - 6-pin PMOD downstream connectors for four buses
* **P3** - GPIO pin header (not pre-soldered)
* **JP6-JP8** - Address configuration headers (ADR0-ADR2)
* **RDY LED** - Lights when upstream is disconnected from all downstream
  buses; turns off when at least one connection is established

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ltc4306/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ltc4306/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes an LTC4306 I2C bus buffer and two MAX5380
DACs, blinks two GPIO-controlled LEDs for status indication, and
cyclically enables downstream I2C channels to sequentially set and cycle
voltages on each DAC. The example showcases downstream channel switching
and multi-DAC control, running continuously while providing error
handling and status output through logs.

This is the only example in this project, so it is selected with the
``basic`` variant (see the Build Command sections below).

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-LTC4306 <https://www.analog.com/EVAL-LTC4306>`_
* MAX32650FTHR or compatible Maxim feather board

Connections
^^^^^^^^^^^

The Maxim feather boards do not have a PMOD interface, but you may use
Dupont female-female cables to make the required connections. The table
below shows the connection between EVAL-LTC4306 upstream connector (P1)
and the Maxim feather board.

+-----------------+-----------------+-----------------+
| EVAL-LTC4306    | Maxim FTHR Pin  | Function        |
| Pin (P1)        |                 |                 |
+-----------------+-----------------+-----------------+
| VCC             | 3V3             | 3.3 V Supply    |
+-----------------+-----------------+-----------------+
| GND             | GND             | Board Ground    |
+-----------------+-----------------+-----------------+
| SCL             | SCL             | Serial Clock    |
+-----------------+-----------------+-----------------+
| SDA             | SDA             | Serial Data     |
+-----------------+-----------------+-----------------+

Use the PMOD connectors on the EVAL-LTC4306 to connect downstream
devices.

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``max32650fthr``, ``max32655fthr``, ``max32660fthr``, ``max32665fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the Maxim SDK libraries (only if not auto-detected)
   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the basic example on the MAX32650FTHR
   python tools/scripts/no_os_build.py build \
      --project eval-ltc4306 --variant basic --board max32650fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-ltc4306 --variant basic --board max32650fthr \
      --probe openocd --flash

ADuCM Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-LTC4306 <https://www.analog.com/EVAL-LTC4306>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Connections
^^^^^^^^^^^

Connect the EVAL-LTC4306 board to the EVAL-ADICUP3029 through the PMOD
I2C header (P9). Use the PMOD connectors on the EVAL-LTC4306 to connect
downstream devices.

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3

   cd no-OS

   # build the basic example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project eval-ltc4306 --variant basic --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-ltc4306 --variant basic --board eval-adicup3029 \
      --probe openocd --flash
