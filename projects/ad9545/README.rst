AD9545 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD9545 <https://www.analog.com/EVAL-AD9545>`_

Overview
--------

The EVAL-AD9545 evaluation board is designed for testing the AD9545
clock generator IC, notable for its high-precision multi-output clock
generation. It includes two digital cleaning PLL cores to optimize clock
synchronization. The board offers up to five differential clock outputs,
configurable through a serial port, and optionally supports an external
EEPROM for settings retention and power management. Applications range
from telecommunications to industrial automation, requiring precise
timing solutions.

Applications
------------

* Global positioning system (GPS), PTP (IEEE 1588), and synchronous
  Ethernet (SyncE) jitter cleanup and synchronization
* Optical transport networks (OTN), synchronous digital hierarchy (SDH),
  and macro and small cell base stations
* Small base station clocking, including baseband and radio
* Stratum 2, Stratum 3e, and Stratum 3 holdover, jitter cleanup, and
  phase transient control
* JESD204B support for analog-to-digital converter (ADC) and
  digital-to-analog converter (DAC) clocking
* Cable infrastructures
* Carrier Ethernet

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD9545 evaluation board requires a 6V power connection via a
wall adapter to activate its on-board LDO voltage regulators. These
regulators provide key voltage outputs of 1.8V and 3.3V, crucial for the
board's operation. Critical components, such as the ADP2384 and ADP7104
regulators, manage these voltage levels to support the board's
functionalities, including generating up to five differential clock
outputs and utilizing EEPROM for settings retention. Additionally, power
is supplied via USB for communication with a PC, enabling configuration
through the provided Windows-based software.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector             | Description           | Functionality         |
+=======================+=======================+=======================+
| J300                  | SMB Connector         | Reference input for   |
|                       |                       | single-ended or       |
|                       |                       | differential signals  |
+-----------------------+-----------------------+-----------------------+
| J301                  | SMB Connector         | Reference input for   |
|                       |                       | single-ended or       |
|                       |                       | differential signals  |
+-----------------------+-----------------------+-----------------------+
| J400                  | SMA Connector         | System clock input    |
|                       |                       | option for external   |
|                       |                       | sources               |
+-----------------------+-----------------------+-----------------------+
| P605                  | Jumper Block          | GPIO configuration    |
|                       |                       | for clock and data    |
|                       |                       | lines                 |
+-----------------------+-----------------------+-----------------------+
| P504                  | Jumper Block          | Serial data line      |
|                       |                       | configuration         |
+-----------------------+-----------------------+-----------------------+
| P511                  | Jumper Block          | Serial data line      |
|                       |                       | configuration         |
+-----------------------+-----------------------+-----------------------+

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher
that calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9545/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9545/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes and configures the AD9545 clock synthesizer
end-to-end. It sets up the system reference clock (52 MHz crystal), two
PLLs (PLL0 targeting 1.4 GHz, PLL1 targeting 1.75 GHz), an NCO, four
output clocks (Q0A/Q0B at 10 MHz, Q1A at 25 MHz, Q1B at 10 MHz), and an
auxiliary TDC, then recalibrates the APLLs. Status and any errors are
printed to standard output.

The communication interface (SPI or I2C) is selected by the ``COMM_TYPE``
macro in
`src/platform/linux-userspace/parameters.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9545/src/platform/linux-userspace/parameters.h>`__
(defaults to SPI). Tunable clock parameters are in
`src/examples/basic_example/basic_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9545/src/examples/basic_example/basic_example.c>`__.

No-OS Supported Platforms
-------------------------

Linux Userspace
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD9545 <https://www.analog.com/EVAL-AD9545>`_
* A Linux host with a SPI or I2C bus exposed via kernel device nodes
  (e.g. a Raspberry Pi, Beaglebone, or any single-board computer running
  Linux with spidev/i2c-dev enabled).

Connections
^^^^^^^^^^^

The AD9545 communicates over SPI or I2C. Connect the EVAL-AD9545 to the
Linux host's bus header:

**SPI (default)**

=========== =========================================
Signal      Linux spidev node / pin
=========== =========================================
SCLK        SPI bus clock (``/dev/spidev0.0`` by default)
MOSI        SPI MOSI
MISO        SPI MISO
CS          SPI chip select 0
3.3V / GND  Host 3.3V rail and ground
=========== =========================================

The SPI bus and chip-select indices are controlled by ``SPI_DEVICE_ID``
and ``SPI_CS`` in ``parameters.h`` (defaults: device 0, CS 0).

**I2C (alternative)**

Set ``COMM_TYPE`` to ``I2C`` in ``parameters.h`` and wire the SDA/SCL
lines. The I2C bus index is set by ``device_id`` in ``parameters.c``
(default: 0, mapping to ``/dev/i2c-0``).

No UART console is needed — the example prints directly to ``stdout``.

Build Command
^^^^^^^^^^^^^

The Linux userspace platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic_example``.
Available boards: ``rpi4``.

No toolchain environment variable is required — the system ``gcc`` is
used automatically.

.. code-block:: bash

   cd no-OS

   # Build the basic example for Raspberry Pi 4
   python tools/scripts/no_os_build.py build \
      --project ad9545 --variant basic_example --board rpi4

The resulting executable is placed at:

.. code-block:: bash

   build/ad9545-basic_example-rpi4/build/ad9545

Run it directly on the target Linux system:

.. code-block:: bash

   sudo ./build/ad9545-basic_example-rpi4/build/ad9545
