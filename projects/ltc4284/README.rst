Evaluating the LTC4284
======================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `DC2470A <https://www.analog.com/DC2470A>`_ — LTC4284 High-Power
  Negative-Voltage Hot Swap Controller Evaluation Kit

Overview
--------

The DC2470A demonstration circuit carries the LTC4284, a high-power
negative-voltage hot swap controller with an integrated I²C energy
monitor and on-die EEPROM. It targets -48 V distributed-power systems
used in telecom infrastructure, data centers, and server power shelves,
providing hot-swap MOSFET drive, voltage/current/power/energy telemetry,
overcurrent protection with programmable foldback and retry, and a
fault-log capture path.

The DC2470A default configuration uses 312 µΩ of sense resistance
(5 mΩ ‖ 6×2 mΩ), 40:1 dividers on the VPWR and DRAIN networks, and
straps the I²C address to ``0x16`` (``LTC4284_I2C_ADDR_6``). The
board's I²C bus operates at LTC4284 INTVCC (5 V) and must be level
shifted to interface to a 3.3 V MCU.

Applications
------------

* -48 V power-distribution front ends (data centers, telecom)
* Server power shelves and rack-mount PSUs
* Backplane / hot-plug slot cards
* High-power negative-supply hot-swap and inrush control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* **Input bus**: -48 V DC nominal (LTC4284 -43 V UV lockout threshold;
  a sagging bench PSU under load can false-trip UV, reported as
  ``FAULT=0x22``).
* **Load**: electronic load or resistive dummy load on the DC2470A
  output. Choose the load so peak current stays below the programmed
  V\ :sub:`ILIM` × 1 / R\ :sub:`SENSE` trip.
* **Level shifter (I²C)**: PCA9603 or equivalent, VCCA = 3.3 V (MCU
  side), VCCB = 5 V (DC2470A INTVCC side). Required because the
  DC2470A I²C bus is pulled to INTVCC.
* **MCU carrier**: powered by USB (also supplies the UART console).

On-board Configuration (DC2470A defaults)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* I²C address straps: ADR1 = H (INTVCC), ADR0 = H (INTVCC) ⇒
  ``LTC4284_I2C_ADDR_6`` (``0x16``).
* Sense resistance R\ :sub:`SENSE` = 312 µΩ (5 mΩ ‖ 6×2 mΩ,
  datasheet p. 49 worked example).
* VPWR divider = 40:1 (390 kΩ / 10 kΩ). DRAIN divider = 40:1.
* Default V\ :sub:`ILIM` = 18 mV (EEPROM-altered from the 15 mV
  reset default).

These values are captured in ``src/common/common_data.c``. Change them
there if you carry the LTC4284 on a different board.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher
that calls the selected example's ``<name>_example_main()`` entry point.
Shared initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4284/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4284/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

Production-safe telemetry loop on the Arm core. Verifies the programmed
current limit against the hardware rating before enabling the FET,
polls VIN / IIN / VOUT / power on a 1 Hz cadence, cross-checks the
chip's power register against V × I, and applies bounded retry on
transient faults (OC/UV/OV) while latching off on hardware faults
(``FET_BAD``, ``FET_SHORT``). Runs for 1 hour then exits cleanly.

Overcurrent Example
~~~~~~~~~~~~~~~~~~~

Programs the OC profile (V\ :sub:`ILIM`, foldback, retry policy) and
narrates fault decode + retry loop transitions on the console. Build
it with ``--variant overcurrent``. The OC profile is set by macros at
the top of ``src/examples/overcurrent/overcurrent_example.c``:

.. code-block:: c

   #define OC_ILIM_MV       15                /* V_ILIM in mV, 15..30 */
   #define OC_FOLDBACK      LTC4284_FB_50     /* Startup foldback */
   #define OC_RETRY_POLICY  LTC4284_RETRY_1   /* 1 retry then latch */

**Wiring for the OC trip:** with ``OC_ILIM_MV = 15`` and DC2470A
``RSENSE = 312 uohm`` the steady-state trip is ~48 A and the fast
(short-circuit) trip is ~96 A. Use **10 AWG minimum ring-lug leads**;
alligator clips will burn. Connect the load between DC2470A RTN and
-48VOUT (not -48VIN, which bypasses the FET).

IIO Example
~~~~~~~~~~~

Publishes LTC4284 telemetry over an IIO server so an IIO client
(IIO-Oscilloscope, ``iio_readdev``) can read VIN / IIN / VOUT /
power / energy. **Not hardware-verified**: the project builds and the
firmware boots, but the client-side interaction has not been exercised
yet. Use with caution and expect to iterate on the channel scaling.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take
a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `DC2470A <https://www.analog.com/DC2470A>`_ — LTC4284 Evaluation Kit
* `MAX32690EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32690evkit.html>`_
* PCA9603 (or equivalent) 3.3 V ↔ 5 V I²C level shifter

Connections
^^^^^^^^^^^

I²C:

+----------------------+--------------------------------+---------------------------+
| DC2470A (5 V side)   | MAX32690EVKIT (3.3 V side)     | Description               |
+======================+================================+===========================+
| SDA                  | P2.7  (via PCA9603 SDB → SDA)  | I²C serial data           |
+----------------------+--------------------------------+---------------------------+
| SCL                  | P2.8  (via PCA9603 SCB → SCL)  | I²C serial clock          |
+----------------------+--------------------------------+---------------------------+
| GND                  | GND                            | Common ground             |
+----------------------+--------------------------------+---------------------------+

Level-shifter power: PCA9603 VCCA = 3.3 V (from MAX32690EVKIT),
VCCB = 5 V (from DC2470A INTVCC). Do not power VCCB from the MCU rail.

UART console: MAX32690EVKIT CN2 (FTDI USB-to-UART bridge) using the
MCU's UART2 at 115200 baud, 8N1. On Linux the port appears as
``/dev/ttyUSB0`` (or similar); on Windows as a COM port.

.. code-block:: bash

   screen /dev/ttyUSB0 115200

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake / Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``,
``iio_example``, ``overcurrent``. Available boards: ``max32690evkit``.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32690evkit board)
   python tools/scripts/no_os_build.py build \
      --project ltc4284 --variant basic --board max32690evkit

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ltc4284 --variant basic --board max32690evkit \
      --probe openocd --flash

Replace ``--variant`` with ``overcurrent`` or ``iio_example`` to build
the other examples on the same board.

Troubleshooting
---------------

**I²C communication failure (init returns -9 / EBADF):**
Usually the level shifter is unpowered — VCCA / VCCB must both be up
before the MCU issues the first transaction. Also verify pull-ups
(4.7 kΩ to INTVCC on the DC2470A side) and that DC2470A has -48 V
applied.

**UV false-trip under load (``FAULT=0x22`` or ``0x26``, UV +
POWER_FAILED, sometimes with a spurious OC bit):**
The bench PSU sags below the LTC4284's -43 V UV threshold when the
load engages — commonly around 500 mA on a small bench supply with
alligator clips. The signature is a visible VIN sag in the log
before the trip. A proper >60 A PSU with 10 AWG ring-lug leads keeps
VIN stable and lets the real OC trip fire at ~48 A as programmed.
This is not an overcurrent event and not a code problem.
