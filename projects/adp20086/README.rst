ADP20086 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* ADP20086 Customer Evaluation Board

Overview
--------

The ADP20086 evaluation board carries one :adi:`ADP20086` quad-channel camera
power protector and exposes all four protected outputs on screw terminals with
matching test points. The board is designed to pair directly with a Feather
form-factor controller: a 28-pin header accepts a
`MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_,
which supplies the 3.3 V logic rail and drives the I2C bus, the EN pin, and the
INTB fault interrupt. No hand wiring is required between the two boards.

The ADP20086 delivers up to 800 mA of constant load current on each of its four
output channels from a 3 V to 17 V camera supply, with each output individually
protected against short-to-battery, short-to-ground, and overcurrent
conditions. Each channel has an independently adjustable current limit from
52 mA to 832 mA accurate to ±5%, selectable soft-start and soft-shutdown
current ramps, and integrated output discharge resistors. An on-chip 8-bit ADC
reports the input voltage, the internal VDD rail, and the voltage and current
of every output channel over I2C.

On the board, the SDA, SCL, INTB, and EN lines are pulled up to the VDDIO rail
through 4.99 kΩ resistors (R5-R8), and a red LED (DS1) indicates an asserted
INT. An M24C64 EEPROM (U3) sits on the same I2C bus for board identification.
The ADDR resistor is fitted as 6.8 kΩ (R11), which latches the device at I2C
address 0x2A.

Applications
------------

* Power-over-Coax (PoC) protection for camera systems
* Automotive camera power distribution
* Fault diagnostics
* ASIL B safety applications

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The board takes two supplies. The camera supply feeding the protection
switches is applied to the VIN terminal and must sit between 3 V and 17 V; the
IN pins tolerate up to 17.6 V. The logic and analog control supply is applied
to the VCC terminal and accepts 2.7 V to 5.5 V.

The 3.3 V logic rail for the I2C bus comes from the MAX32655FTHR through the
Feather header, so the controller board can be powered over its own USB
connector. The VDDIO rail that the SDA, SCL, INTB, and EN pull-ups reference is
selected by J1, between the 3.3 V rail from the Feather and the device VCC.
Set it to match the logic level of the controller driving the bus.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

=============== =========================================================
Connector       Function
=============== =========================================================
U2              28-pin Feather header; accepts the MAX32655FTHR
VIN             Camera supply input, 3 V to 17 V
VCC             Logic and analog control supply, 2.7 V to 5.5 V
VDDIO           I2C and control pull-up rail
OUT1 - OUT4     Protected camera supply outputs
GND1 - GND7     Ground turrets
J1              VDDIO rail source select (3.3 V from Feather, or VCC)
J2              I2C address select
J3              EN source select (Feather GPIO, or VCC)
=============== =========================================================

Test points are provided on VIN, EN, INT, SDA, SCL, and each of the four
outputs.

**I2C address selection (J2).** The ADDR pin latches the I2C address at
power-up from the resistor to ground selected by J2:

============================ =============
ADDR setting                 I2C address
============================ =============
Short to GND                 0x28
6.8 kΩ (R11, as fitted)      0x2A
Open                         0x2F
============================ =============

The examples are configured for ``ADP20086_6P8KOHM_ADDR`` (0x2A), matching the
fitted 6.8 kΩ resistor. Change ``.slave_address`` in
``src/common/common_data.c`` if you move J2.

**EN source selection (J3).** J3 selects whether the device EN pin is driven by
the Feather's GPIO or tied to VCC. The examples expect the GPIO position
(pins 1-3), which lets the driver control EN in software. If J3 is set to tie
EN to VCC instead, comment out the ``.en_gpio_ip`` assignment in
``src/common/common_data.c`` so the driver does not try to claim a pin that is
not connected.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp20086/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp20086/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example brings the device up, prints the part ID, revision, and
whether PEC is enabled, then configures the protection thresholds and streams
telemetry to the UART console.

It sets an input protection window of 4 V to 16 V, a 4.8 V output undervoltage
threshold on every channel, unmasks the default set of fault interrupts, then
enables all four outputs and arms the INTB interrupt. Once running, it prints
VIN and VDD plus the output voltage and current of each channel once per
second, and reports any fault latched on INTB with the offending channel and
cause. Press Enter to stop; the example then masks the interrupts, brings the
outputs down, releases EN, and exits cleanly.

The thresholds, the sample period, and the console polling granularity are
defined at the top of
``src/examples/basic_example/basic_example.c`` as
``ADP20086_EXAMPLE_UVIN_UV``, ``ADP20086_EXAMPLE_OVIN_UV``,
``ADP20086_EXAMPLE_UVOUT_UV``, ``ADP20086_EXAMPLE_PERIOD_MS``, and
``ADP20086_EXAMPLE_POLL_MS``.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the board so that a host can
connect over the UART and read telemetry, configure every per-output
threshold, and inspect the latched fault status through standard IIO
attributes. INTB is armed and serviced once per IIO loop iteration, so faults
are detected while the server runs; the fault callback is deliberately left
empty, because the UART carries the libiio protocol in this example and
printing from the handler would corrupt the client's connection.

Note that this example relies on the device's reset and OTP configuration for
its startup state: ``adp20086_init()`` drives EN high, and CONFIG resets with
``EN[4:1] = 1111``, so the outputs come up live. Write the per-channel ``en``
attribute after connecting to change that.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look
at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* ADP20086 Customer Evaluation Board
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

Connections
^^^^^^^^^^^

The MAX32655FTHR plugs directly into the board's 28-pin Feather header (U2),
so there is no wiring to do. The header fixes the following mapping:

+---------------------+------------+--------------------------------+------------+--------------------+
| ADP20086 Board Net  | Mnemonic   | Function                       | Mnemonic   | MAX32655FTHR Pin   |
+=====================+============+================================+============+====================+
| SDA_FTHR            | SDA        | I2C serial data                | I2C2_SDA   | P0.31              |
+---------------------+------------+--------------------------------+------------+--------------------+
| SCL_FTHR            | SCL        | I2C serial clock               | I2C2_SCL   | P0.30              |
+---------------------+------------+--------------------------------+------------+--------------------+
| INT_FTHR            | INTB       | Active-low fault interrupt     | GPIO       | P1.8               |
+---------------------+------------+--------------------------------+------------+--------------------+
| EN_FTHR             | EN         | Output enable (via J3)         | GPIO       | P1.7               |
+---------------------+------------+--------------------------------+------------+--------------------+
| 3V3FTHR             | 3V3        | Logic rail for the board       |            | 3V3                |
+---------------------+------------+--------------------------------+------------+--------------------+
| GND                 | GND        | Ground                         |            | GND                |
+---------------------+------------+--------------------------------+------------+--------------------+

Set the three jumpers as follows before powering the board. These are the
positions both examples are written against; see `On-board Connectors`_ for
the other options.

========= ========================= ==================================================
Jumper    Expected position         Why
========= ========================= ==================================================
J1        VDDIO tied to 3V3FTHR     References the SDA, SCL, INTB, and EN pull-ups to
                                    the MAX32655FTHR 3.3 V rail, matching the logic
                                    level the Feather drives the bus at.
J2        6.8 kΩ (R11, as fitted)   Latches I2C address 0x2A, which is the
                                    ``ADP20086_6P8KOHM_ADDR`` the examples open.
J3        Pins 1-3 (Feather GPIO)   Routes EN to P1.7 so the driver can control it in
                                    software, as ``.en_gpio_ip`` expects.
========= ========================= ==================================================

Apply the camera supply to the VIN terminal and the control supply to the VCC
terminal, and connect the loads to OUT1 through OUT4. The UART console is
available at 115200 baud over the MAX32655FTHR USB connector.

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script.

Available variants: ``basic_example``, ``iio_example``.
Available boards: ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32655fthr board)
   python tools/scripts/no_os_build.py build \
      --project adp20086 --variant basic_example --board max32655fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adp20086 --variant basic_example --board max32655fthr \
      --probe openocd --flash
