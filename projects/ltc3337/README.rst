LTC3337 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `DC2973A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2973a.html>`_

Overview
--------

The `LTC3337 <https://www.analog.com/LTC3337>`_ is a primary battery state of
health (SOH) monitor with a built-in precision coulomb counter. It is designed
to be placed in series with a primary battery with minimal associated series
voltage drop. The coulomb counter tallies all accumulated battery discharge and
stores it in an internal register accessible via an I2C interface. A discharge
alarm threshold is programmable, generating an interrupt at the IRQ pin when
reached.

The LTC3337 also monitors battery voltage, battery impedance, and die
temperature, all accessible via I2C. The battery input voltage range is 1.8 V
to 5.5 V with only 100 nA quiescent current. The peak input current limit is
pin-selectable from 5 mA to 100 mA (8 settings: 5, 10, 15, 20, 25, 50, 75,
100 mA).

Applications
------------

* Low power primary battery powered systems (e.g., 1x LiSOCl2, 2-3x Alkaline)
* Remote industrial sensors (e.g., meters, alarms)
* Asset trackers
* Electronic door locks
* Keep-alive supplies / battery backup

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Battery input voltage range: 1.8 V to 5.5 V
* Quiescent current: 100 nA
* 8 selectable peak input current limits: 5 mA, 10 mA, 15 mA, 20 mA,
  25 mA, 50 mA, 75 mA, 100 mA

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3337/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc3337/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the LTC3337 device and continuously reads
accumulated charge, battery input/output voltages (with iPeak on and off),
and die temperature, printing the results over UART.

This is the only example in this project, so it builds by default.

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `DC2973A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2973a.html>`_
* MAX32655 microcontroller

Connections
^^^^^^^^^^^

The DC2973A communicates via I2C. Connect the following pins between the
DC2973A and the MAX32655 board:

+----------+-------------------+
| DC2973A  | MAX32655          |
+==========+===================+
| SCL      | I2C2 SCL          |
+----------+-------------------+
| SDA      | I2C2 SDA          |
+----------+-------------------+
| GND      | GND               |
+----------+-------------------+

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32655fthr board)
   python tools/scripts/no_os_build.py build \
      --project ltc3337 --variant basic --board max32655fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ltc3337 --variant basic --board max32655fthr \
      --probe openocd --flash
