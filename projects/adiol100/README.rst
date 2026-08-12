ADIOL100 no-OS Example Project
===============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* ADIOL100EVKIT
* `MAXREFDES171# <https://www.analog.com/en/resources/reference-designs/maxrefdes171.html>`__
  — IO-Link distance sensor, used as the target device in the basic example

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

EVKIT Jumper Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following jumper settings are required when using the ADIOL100EVKIT
with the MAX78000FTHR via the Pmod connector (J3):

==============  ===========  =============================================
Jumper          Position     Function
==============  ===========  =============================================
J4 (VLED)       2-3          LED supply = 3.3 V (V33)
J5 (VL)         2-3          Logic supply = 3.3 V (V33)
JMP1 (LDOIN)    1-3          LDO input = 5 V (V5)
J10 (ADRSEL)    3-4          SPI device address = 01 (``chip_addr = 0x01``)
==============  ===========  =============================================

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
   ┌─────────────────┐      ┌─────▼────┐      ┌───────────▼───────────────┐
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

IO-Link FIFO Framing
^^^^^^^^^^^^^^^^^^^^^

All IO-Link communication goes through the ADIOL100's per-channel FIFO.
The application writes a frame into the TxFIFO and reads the device's
response from the RxFIFO. The actual SPI command byte (chip address +
register + R/W bit) is handled by the driver, the application only deals
with the FIFO payload.

**Write frame** (master to device, via ``adiol100_load_and_send_msg``):

.. code-block:: text

   ┌───────┬─────────┬─────────┬──────┬──────┬──────┐
   │ MsgID │ RxBytes │ TxBytes │  MC  │  CKT │ Data │
   └───────┴─────────┴─────────┴──────┴──────┴──────┘
    driver   caller    caller  ├── IO-Link payload ─┤
    adds     provides  provides  (caller provides)

* **MsgID** — Auto-incrementing message ID (prepended by the driver).
* **RxBytes** — Number of response bytes to expect (including CKS).
* **TxBytes** — Number of IO-Link payload bytes that follow.
* **MC** — M-sequence Control byte. Encodes the operation, IO-Link
  communication channel, and address in a single byte:

  .. code-block:: text

     Bit   7      6:5       4:0
         ┌────┬─────────┬─────────┐
         │R/W │ Channel │ Address │
         └────┴─────────┴─────────┘

  * **R/W** — 1 = read from device, 0 = write to device.
  * **Channel** — IO-Link communication channel:
    0 = Process, 1 = Page, 2 = Diagnosis, 3 = ISDU.
  * **Address** — Register or page address within the selected channel.

  When the Channel is set to Page (1), the Address selects a register in
  the Direct Parameter Page (DPP), defined in the IO-Link spec Table B.1.
  DPP registers 0x00–0x0D contain standard device parameters such as
  MinCycleTime, MSeqCapability, VendorID, DeviceID, and MasterCommand.

  The example uses the ``IOL_MC(rw, ch, addr)`` macro to build this byte.
  For example, reading DPP address 0x02 (MinCycleTime) on the Page channel:
  ``IOL_MC(IOL_READ, IOL_CH_PAGE, 0x02)`` = ``0xA2``.

* **CKT** — Checksum/Type byte. Bits [7:6] select the M-sequence type,
  which determines the frame structure (how many OD and PD bytes are
  exchanged):

  * **TYPE_0** (``0x00``) — 1 OD byte, no PD. Used in STARTUP for
    single register reads and writes.
  * **TYPE_1** (``0x40``) — Variable OD bytes, no PD. Used in
    PREOPERATE for ISDU transfers and multi-byte parameter writes.
  * **TYPE_2** (``0x80``) — OD + PD bytes. Used in OPERATE for cyclic
    process data exchange.

  Bits [5:0] carry the 6-bit IO-Link checksum. When the framer's checksum
  insertion is enabled (``ADIOL100_CHKS_EN``), the chip computes and
  inserts the checksum automatically — the application only needs to set
  the type bits.

* **Data** — On-request Data (OD) bytes. Content depends on the operation
  (e.g. register value for a write, ISDU command for an ISDU request).

**Read frame** (device response, via ``adiol100_read_msg``):

.. code-block:: text

   ┌───────┬────────────┬──────┬─────┐
   │ MsgID │ RxBytesAct │ Data │ CKS │
   └───────┴────────────┴──────┴─────┘
   ├─── FIFO header  ──┤├─ returned ─┤
    (stripped by driver)   to caller

* **MsgID** — Echoed message ID (stripped by the driver).
* **RxBytesAct** — Actual number of response bytes (stripped by the driver).
* **Data** — OD bytes from the device (register value, ISDU response, or
  process data depending on the M-sequence type).
* **CKS** — Checksum/Status byte: bit 7 = Event flag, bit 6 = PD valid,
  bits [5:0] = 6-bit checksum.

Example Flow
^^^^^^^^^^^^^

With this framing, IO-Link communication is a sequence of write-frame /
read-response pairs. Each protocol phase uses a different M-sequence type:

1. **STARTUP** — Configure channel A: enable L+ supply, set CQ driver to
   push-pull mode, enable framer and checksum insertion. Then run
   EstablishCommunication, which probes the sensor at COM3, COM2, and COM1
   until a baud rate is established. Read required DPP parameters from the
   sensor (MinCycleTime, MSeqCap, PDIn, PDOut) using TYPE_0 M-sequences.
   Read optional identification parameters (VendorID, DeviceID, FunctionID).
   Send the MasterIdent and PreOperate commands.

2. **PREOPERATE** — Perform ISDU reads (serial number, vendor name,
   product name, etc.) using TYPE_1 M-sequences. ISDU reads follow a
   3-step pattern: the master sends the read command to the ISDU Start
   address, polls until the device responds (it returns "No Service"
   while busy), then closes the transaction by reading the ISDU Idle
   address. Set the cycle time. Send the Operate command.

3. **OPERATE** — Start cyclic process data exchange using TYPE_2
   M-sequences with ``ADIOL100_KEEP_MSG``. The cycle timer automatically
   re-transmits the master message each cycle. The example polls and
   prints 20 cycles of sensor data.

The example code uses two helper functions built on top of the driver:

* ``iol_send_frame`` — Packs MC + CKT + OD into the IO-Link payload and
  calls ``adiol100_load_and_send_msg``.
* ``iol_send_frame_and_read`` — Calls ``iol_send_frame`` followed by
  ``adiol100_read_msg`` to retrieve the device response.

For more information about the IO-Link protocol, see the
`IO-Link specification <https://io-link.com/downloads>`__.

Example Output
^^^^^^^^^^^^^^^

.. code-block:: bash

   -----ADIOL100 Basic Example-----
   Configuring transceiver...
   Sending EstablishCommunication...
   Slave responded at COM3
   Reading DPP parameters...
   MinCycleTime=0x08  MSeqCap=0x2B  PDIn=0xC6  PDOut=0x00
   PreOp OD=8  Operate OD=2  PDIn=7  PDOut=0
   MasterIdent sent
   DPP 0x07 VendorID MSB     = 0x01
   DPP 0x08 VendorID LSB     = 0xDE
   DPP 0x09 DeviceID b1      = 0x02
   DPP 0x0A DeviceID b2      = 0x9D
   DPP 0x0B DeviceID b3      = 0x47
   DPP 0x0C FunctionID MSB   = 0x00
   DPP 0x0D FunctionID LSB   = 0x00
   SetPreOp sent
   ISDU reads...
     ISDU 21 (len=16): 00000000
     ISDU 16 (len=16): Maxim Integrated
     ISDU 17 (len=23): www.maximintegrated.com
     ISDU 18 (len=16): MAXREFDES171-TMG
     ISDU 19 (len=16): MAXREFDES171-TMG
     ISDU 20 (len=54): MAX32660 Sample Application with LASER Distance Sensor
   SetCycleTime=0x08
   SetOperate sent
   Starting cyclic process data exchange...
   Cyclic #0 dist=39mm raw(10): 00 00 00 27 3B 18 00 18 00 10
   Cyclic #1 dist=38mm raw(10): 00 00 00 26 3B 08 00 18 00 10
   ...
   Cyclic #19 dist=38mm raw(10): 00 00 00 26 3B 00 00 18 00 10

No-OS Supported Platforms
--------------------------

Maxim
~~~~~~

Used Hardware
^^^^^^^^^^^^^^

* MAX78000FTHR
* ADIOL100EVKIT

Connections
^^^^^^^^^^^^

===============  ========  =====================  ===================
ADIOL100 Pin     Signal    Function               MAX78000FTHR Pin
===============  ========  =====================  ===================
SCLK             SCLK      SPI clock              P0.5
SDI              MOSI      SPI master out         P0.6
SDO              MISO      SPI master in          P0.7
CSB              CS        SPI chip select        P0.11
IRQA             IRQA      Channel A interrupt    P0.19
IRQB             IRQB      Channel B interrupt    Not used
IRQG             IRQG      Global interrupt       Not used
GND              GND       Ground                 GND
VL               VL        Logic supply           3V3
===============  ========  =====================  ===================

UART console output is on UART0 at 115200 baud.

Build Command
^^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``.
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

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adiol100 --variant basic --board max78000fthr \
      --probe openocd --flash

