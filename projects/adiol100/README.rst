ADIOL100 no-OS Example Project
===============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* TO MODIFY AFTER RECEIVING EVAL-BOARD

Overview
--------

The ADIOL100 is a dual-channel IO-Link Master transceiver with an integrated
IO-Link framer, L+ sensor supply controller, and CQ line driver. It
communicates with the host MCU over SPI at up to 26 MHz. Each channel can
independently manage an IO-Link port: powering a sensor, establishing
communication, and exchanging cyclic process data.

For a detailed description of the device and the driver API, see the
`ADIOL100 driver documentation <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/io-link/adiol100>`__.

Applications
------------

* IO-Link master ports
* Factory automation sensor hubs
* Industrial fieldbus gateways
* Process instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADIOL100 requires a 9 V to 36 V supply on the V24 pin (typ 24 V).
The VL logic supply must be 1.62 V to 5.5 V, provided by the MCU board
(3.3 V from the MAX78000FTHR). The on-chip LDO generates the 1.8 V core
supply from a separate LDOIN input (3 V to 36 V).

Device Address (ADRSEL)
~~~~~~~~~~~~~~~~~~~~~~~~

The ADIOL100 SPI device address is set by the ADRSEL pin at power-up.
Ensure the ADRSEL pin state matches the ``chip_addr`` value in
``common_data.c``.

=================  ================
ADRSEL Pin         Device Address
=================  ================
LOW                0
HIGH               1
1 kohm pull-down   2
1 kohm pull-up     3
=================  ================

No-OS Supported Examples
-------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adiol100/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adiol100/src/platform>`__.

Basic Example
~~~~~~~~~~~~~~

This example demonstrates the full IO-Link communication flow, manually
driving the protocol state machine through all phases:

.. code-block:: text

                            ┌────────────┐
                            │STANDARD I/O│
                            └─────┬──────┘
                                  │
                              ┌───▼───┐            ┌──────────────┐
                              │STARTUP├───────────►│ Transceiver  │
                              └───┬───┘            │Configuration │
                                  │                │- CQ, LP      │
                                  │                │- Protections │
                                  │                │- Interrupts  │
                                  │                │- ...         │
                                  │                └──────┬───────┘
                                  │                       │
                                  │              ┌────────▼─────────────┐
                                  │              │EstablishCommunication│
                                  │              │  (probing the slave  │
                                  │              │   at every ComRate)  │
                                  │              └──────────────────────┘
                                  │                       │
   ┌─────────────────┐      ┌─────▼────┐      ┌──────────▼────────────────┐
   │   Master sets   │◄─────┤PREOPERATE│      │          Slave            │
   │ MasterCycleTime │      └─────┬────┘      │      Identification       │
   └────────┬────────┘            │           │ (reading slave parameters │
            │                     │           │required for communication │
   ┌────────▼────────┐            │           │   and identification)     │
   │   (Optional)    │            │           └────────────┬──────────────┘
   │Master reads ISDU│            │                        │
   │     pages       │            │             ┌──────────▼───────────┐
   └─────────────────┘            │             │       Sending        │
                                  │             │ MasterIdent command  │
                                  │             └──────────────────────┘
                                  │
                                  │
                              ┌───▼───┐
                              │OPERATE│
                              └───────┘
                       (live data every cycle)

The example walks through each phase:

1. **STARTUP** — Configure channel A: enable L+ supply, set CQ driver to
   push-pull mode, enable framer and checksum insertion. Then run
   EstablishCommunication, which probes the sensor at COM3, COM2, and COM1
   until a baud rate is established. Read required DPP parameters from the sensor
   (MinCycleTime, MSeqCap, PDIn, PDOut) using TYPE_0 M-sequences. Read
   optional identification parameters (VendorID, DeviceID, FunctionID).
   Send the MasterIdent command.

2. **PREOPERATE** — Send the PreOperate command. Perform ISDU reads
   (serial number, vendor name, product name, etc.) using the 3-step
   send/poll/close pattern with TYPE_1_V M-sequences. Set the cycle time.
   Send the Operate command.

3. **OPERATE** — Start cyclic process data exchange using TYPE_2
   M-sequences with ``ADIOL100_KEEP_MSG``. The cycle timer automatically
   re-transmits the master message each cycle. The example polls and
   prints 20 cycles of sensor data.

All IO-Link protocol steps (M-sequence framing, ISDU parsing, state
transitions) are implemented directly in the example code using the
driver's ``load_and_send_msg`` and ``read_msg`` functions.

i-link Stack Example (FreeRTOS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example integrates the `rt-labs i-link <https://github.com/rtlabs-com/i-link>`__
open-source IO-Link master stack, running on FreeRTOS. Instead of manually
driving the protocol like the basic example, the i-link stack handles the
full IO-Link state machine (DL, SM, CM, DS, AL layers) autonomously. The
ADIOL100 driver acts as the physical layer (PL) backend via a port layer
that implements the stack's ``iolink_hw_ops_t`` interface.

The example:

1. Initializes the ADIOL100 and the port layer with IRQ GPIO configuration.
2. Configures channel A as an SDCI (IO-Link) port and channel B as inactive.
3. Creates a FreeRTOS task that runs the i-link application handler, which
   manages device discovery, identification, and cyclic process data exchange.

Key components:

* **Port layer** (``adiol100_ilink_pl.c/h``) — Bridges the i-link stack
  to the ADIOL100 driver. Implements all 14 ``iolink_hw_ops_t`` callbacks:
  baudrate/cycletime get/set, mode switching, cycle timer control, FIFO
  messaging, EstablishCommunication, and interrupt handling. ISRs signal
  FreeRTOS event groups to wake the DL thread.

* **OSAL** (``osal_freertos.c``) — FreeRTOS implementation of the i-link
  OS abstraction layer (mutexes, events, threads, mailboxes, timers).

* **Application handler** (``app_handler.c``) — Adapted from the i-link
  sample application. Uses the SMI (System Management Interface) to
  interact with discovered devices. Vendor-specific device handling has
  been replaced with a generic handler that reads and prints process data.

Build with the ``ilink`` variant. Requires FreeRTOS (pulled in
automatically via Kconfig).

No-OS Supported Platforms
--------------------------

Maxim
~~~~~~

Used Hardware
^^^^^^^^^^^^^^

* MAX78000FTHR
* TO MODIFY AFTER RECEIVING EVAL-BOARD

Connections
^^^^^^^^^^^^

===============  ========  =====================  ===================
ADIOL100 Pin     Signal    Function               MAX78000FTHR Pin
===============  ========  =====================  ===================
SCLK             SCLK      SPI clock              P0.5
SDI              MOSI      SPI master out         P0.6
SDO              MISO      SPI master in          P0.7
CSB              CS        SPI chip select        P0.11
IRQA             IRQA      Channel A interrupt    TBD
IRQB             IRQB      Channel B interrupt    TBD
IRQG             IRQG      Global interrupt       TBD
GND              GND       Ground                 GND
VL               VL        Logic supply           3V3
===============  ========  =====================  ===================

UART console output is on UART0 at 115200 baud.

Build Command
^^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``ilink``.
Available boards: ``max78000fthr``.

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

.. code-block:: bash

   # set the path to the Maxim SDK libraries
   export MAXIM_LIBRARIES=/path/to/MaximSDK/Libraries
   # PowerShell (Windows) equivalent:
   #   $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the basic example on the MAX78000FTHR
   python tools/scripts/no_os_build.py build \
      --project adiol100 --variant basic --board max78000fthr

   # build the i-link stack example
   python tools/scripts/no_os_build.py build \
      --project adiol100 --variant ilink --board max78000fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adiol100 --variant basic --board max78000fthr \
      --probe openocd --flash
