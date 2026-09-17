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
that calls ``example_main()``, provided by the selected example.
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
narrates fault decode + retry loop transitions on the console. Built
into the ``basic`` variant (``src/examples/overcurrent/``); enable it
via ``CONFIG_LTC4284_OVERCURRENT_EXAMPLE=y`` if you want the
overcurrent-focused build instead of the safety-monitoring loop.

Dual-Core Example
~~~~~~~~~~~~~~~~~

Splits LTC4284 control across the MAX32690's Arm Cortex-M4F (CPU0)
and RISC-V RV32 (CPU1) cores. The RISC-V core owns the I²C bus and
the ALERT GPIO, programs the OC profile, and reacts to ALERT edges;
the Arm core boots the coprocessor, serves the UART console, and
polls telemetry from a shared IPC table backed by the MAX32690 SEMA
peripheral (doorbell + mailbox). Demonstrates the ``no_os_coprocessor``
and ``no_os_ipc`` subsystems end-to-end.

Architecture
^^^^^^^^^^^^

**ARM Cortex-M4F (CPU0) — Observer / Console**

* Boots the RISC-V coprocessor from the embedded flash region
* Owns the shared IPC table + UART console
* Polls RISC-V for telemetry and the OC config snapshot
* Prints the OC banner (uses live ``CONFIG_1`` / ``CONTROL_2`` bytes
  read from the chip after RISC-V programs it)
* Decodes ``FAULT`` bits into a human-readable string
* Auto-sends ``CMD_CLEAR_FAULTS`` on new faults so the retry loop
  stays observable
* Narrates ``FET turned off`` / ``Retry succeeded. FET back on.``
  transitions

**RISC-V RV32 (CPU1) — Real-Time Controller**

* Brings up I2C0 (device-id 0, 100 kHz, VDDIOH pads)
* Initializes the LTC4284 driver
* Programs the OC profile from local macros in
  ``dual_core_example_riscv.c``:

  .. code-block:: c

     #define OC_ILIM_MV       15                /* V_ILIM in mV, 15..30 */
     #define OC_FOLDBACK      LTC4284_FB_50     /* Startup foldback */
     #define OC_RETRY_POLICY  LTC4284_RETRY_1   /* 1 retry then latch */

* Clears faults and enables the FET
* Configures a rising-edge GPIO interrupt on the LTC4284 ALERT pin
  (P0.19)
* Rings the ARM doorbell on every ALERT edge (RISC-V is the sole owner
  of the ALERT wire; the ARM is only notified via IPC)
* Serves IPC commands from ARM: ``READ_TELEMETRY``,
  ``READ_CONFIG_REGS``, ``CLEAR_FAULTS``, ``ENABLE_FET``, ``NOP``

Inter-Core Communication
^^^^^^^^^^^^^^^^^^^^^^^^

**IPC hardware:** MAX32690 SEMA peripheral (doorbell semaphores).
The ARM rings ``irq0`` (RISC-V-side); RISC-V rings ``irq1``
(ARM-side).

**Shared memory:** IPC table at ``0x20110000`` in the
RISC-V-dedicated SRAM region (accessible by both cores).

**Commands (ARM → RISC-V):**

* ``LTC4284_CMD_NOP`` — heartbeat
* ``LTC4284_CMD_READ_TELEMETRY`` — populates the ``telemetry`` block
* ``LTC4284_CMD_CLEAR_FAULTS`` — writes the FAULT clear pattern
* ``LTC4284_CMD_ENABLE_FET`` — ``param1`` = 0/1
* ``LTC4284_CMD_READ_CONFIG_REGS`` — populates the ``config`` snapshot
  with ``CONFIG_1``, ``CONTROL_2``, and the programmed OC settings

**Handshake:** ARM writes ``cmd_opcode`` + ``cmd_param*``, increments
``cmd_sequence``, then rings the RISC-V doorbell. RISC-V processes
the command, writes ``rsp_error_code`` and sets ``rsp_sequence`` equal
to ``cmd_sequence``, then rings the ARM doorbell. ARM polls
``rsp_sequence == cmd_sequence`` to detect completion.

**RISC-V → ARM notifications:** OC ALERT edges from the LTC4284's
ALERT pin. RISC-V increments ``alert_count`` and rings the ARM
doorbell. ARM polls ``alert_count`` each loop and prints
``[RV32->ARM] OVERCURRENT ALERT ...`` when it advances.

Memory Layout
^^^^^^^^^^^^^

**ARM (CPU0)**

* Flash: ``0x10000000 - 0x10300000`` (3 MB ARM region)
* SRAM:  ``0x20000000 - 0x20100000`` (1 MB)

**RISC-V (CPU1)**

* Flash: ``0x10300000 - 0x10340000`` (256 KB, embedded in ARM ELF)
* SRAM:  ``0x20100000 - 0x20120000`` (128 KB, dedicated)

**IPC Shared Memory**

* Address: ``0x20110000``
* Size: 256 bytes (defined by ``ltc4284_ipc_table_t``)
* Located in RISC-V SRAM for dual-core access

Expected Console Output
^^^^^^^^^^^^^^^^^^^^^^^

Startup:

.. code-block:: text

   ====================================================
     LTC4284 Dual-Core Monitoring Example (MAX32690)
   ====================================================
   ARM Core:    User interface and telemetry display
   RISC-V Core: I2C control and OC monitoring
   ----------------------------------------------------

   [ARM] IPC table initialized at 0x20110000
   [ARM] Booting RISC-V coprocessor at 0x10300000...
   [ARM] RISC-V coprocessor booted
   [ARM] IPC initialized (doorbell + mailbox)
   [ARM] Waiting for RISC-V initialization...
   [ARM] RISC-V core ready

   ==== LTC4284 OC Config ====
     RSENSE          : 312 uohm
     CONFIG_1        : 0x04 (after programming)
     CONTROL_2       : 0x07 (after programming)
     V_ILIM          : 15 mV
     V_ILIM(FAST)    : 30 mV (auto 2x)
     Trip (steady)   : ~48076 mA
     Trip (fast)     : ~96152 mA
     OC retry policy : 1 retry then latch
   =========================

Steady-state telemetry:

.. code-block:: text

   [ARM] VIN=48.800V  IIN=0.000A  PIN=0.000W  VOUT=48.800V  VDS=0.000V  FET=1 PG=1  Alerts=0
   [ARM] VIN=48.800V  IIN=2.456A  PIN=119.865W  VOUT=48.795V  VDS=0.005V  FET=1 PG=1  Alerts=0

OC trip (steady-state exceed, single retry then latch):

.. code-block:: text

   [RV32->ARM] OVERCURRENT ALERT (RISC-V detected ALERT pin, notified ARM via IPC doorbell, count=1)
   [ARM] VIN=48.750V  IIN=48.100A  PIN=2344.611W  VOUT=0.150V  VDS=48.600V  FET=0 PG=0  Alerts=1
     ** FAULT 0x04: OC **
     [RV32] FET turned off. Waiting for auto-retry...
     --> FET is OFF.
   [ARM] VIN=48.800V  IIN=0.000A  ...  FET=1 PG=1  Alerts=1
     --> Retry succeeded. FET back on.

If the load stays above the trip current on the retry, the chip
latches off. A manual power cycle (or the loop's auto
``CMD_CLEAR_FAULTS`` combined with the load dropping below trip) is
required to recover.

Wiring for the OC Trip
^^^^^^^^^^^^^^^^^^^^^^

With ``OC_ILIM_MV = 15`` and DC2470A ``RSENSE = 312 uohm`` the
steady-state trip is ~48 A and the fast (short-circuit) trip is
~96 A. Use **10 AWG minimum ring-lug leads** — alligator clips will
burn.

Dual-Core Debugging
^^^^^^^^^^^^^^^^^^^

The RISC-V firmware is embedded in the ARM ELF via ``.incbin``. To
debug both cores at once you need two GDB sessions — one per core —
each backed by its own OpenOCD server on a different port. The
RISC-V symbols are in the separately-built ``riscv.elf`` inside the
build tree
(``build/ltc4284-dual_core-max32690evkit/projects/ltc4284/coprocessor/``).

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

I²C (single-core and dual-core variants):

+----------------------+--------------------------------+---------------------------+
| DC2470A (5 V side)   | MAX32690EVKIT (3.3 V side)     | Description               |
+======================+================================+===========================+
| SDA                  | P2.7  (via PCA9603 SDB → SDA)  | I²C serial data           |
+----------------------+--------------------------------+---------------------------+
| SCL                  | P2.8  (via PCA9603 SCB → SCL)  | I²C serial clock          |
+----------------------+--------------------------------+---------------------------+
| ALERT#               | P0.19 (dual-core variant)      | Overcurrent alert         |
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
``dual_core``, ``iio_example``. Available boards: ``max32690evkit``.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (dual-core example on the max32690evkit board)
   python tools/scripts/no_os_build.py build \
      --project ltc4284 --variant dual_core --board max32690evkit

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ltc4284 --variant dual_core --board max32690evkit \
      --probe openocd --flash

Replace ``--variant`` with ``basic`` or ``iio_example`` to build the
other examples on the same board.

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

**Dual-core RISC-V hangs at "Waiting for RISC-V initialization":**
The RISC-V core failed ``ltc4284_init()``; check the level shifter
power sequencing and the I²C wiring — the RISC-V owns the bus in
this variant. Also verify ``_riscv_boot`` exists in the ARM linker
map and that ``FCR->urvbootaddr`` is being programmed (should match
the ``[ARM] Booting RISC-V coprocessor at 0x…`` address).

**Dual-core banner prints but no telemetry:**
Watch for a stuck ``STATUS_ERROR`` bit in the IPC status before the
``[ARM] RISC-V core ready`` line — RISC-V may have failed the OC
programming step.

**Dual-core telemetry runs but no OC alert fires under load:**
Verify the ALERT wire from DC2470A to MAX32690EVKIT P0.19, confirm
the load actually exceeds the programmed trip current (see the OC
banner), and confirm the RISC-V is running (the ``[ARM] RISC-V
coprocessor booted`` line must appear at startup). Also verify the
``interrupt("machine")`` attribute is on the RISC-V IRQ handlers —
without it, ``mret`` is not emitted and interrupts stop after the
first firing.
