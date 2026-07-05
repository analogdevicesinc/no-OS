OA TC6 no-OS Driver
====================

Overview
--------

The OA TC6 driver implements the OPEN Alliance TC6 10BASE-T1x MAC-PHY
Serial Interface specification (v1.1). It provides the host-side SPI
protocol engine that carries both Ethernet frame data and register
control transactions over a single full-duplex SPI bus to a TC6-compliant
MAC-PHY device (e.g. ADIN1140, ADIN1110).

The specification defines two transaction types that share the same SPI
bus but are distinguished by the Data-Not-Control (DNC) bit in the first
32-bit header word:

- **Data transactions** — transfer Ethernet frames in 64-byte chunks
  (header + payload on MOSI, payload + footer on MISO).
- **Control transactions** — read/write 32-bit registers in the MAC-PHY
  register space, addressed via a Memory Map Selector (MMS) plus a
  16-bit offset.

The host must deassert CSn when switching between data and control
transactions. Multiple chunks may be transferred within a single data
transaction (CSn held low), and multiple control commands may be
chained within a single control transaction.

Protocol Background
-------------------

This section summarises the key TC6 protocol concepts that the driver
implements. Refer to the OPEN Alliance specification (Sections 7.2–7.8)
for the full normative text.

Chunk Structure
~~~~~~~~~~~~~~~

A **data chunk** is the atomic unit of Ethernet frame transfer. Each
chunk is composed of a 4-byte overhead (header or footer) plus a fixed
payload of ``OA_CHUNK_SIZE`` bytes (64 by default, matching the TC6
default chunk payload size).

**Transmit (host → MAC-PHY, MOSI):**

::

    ┌──────────────────┬──────────────────────────────────┐
    │  TX Data Header  │     TX Chunk Payload (64 B)      │
    │     (4 bytes)    │                                  │
    └──────────────────┴──────────────────────────────────┘

The TX data header carries the DNC, DV (Data Valid), SV (Start Valid),
EV (End Valid), SWO (Start Word Offset), EBO (End Byte Offset), VS
(Vendor Specific), TSC (Timestamp Capture), and a parity bit.

**Receive (MAC-PHY → host, MISO):**

::

    ┌──────────────────────────────────┬──────────────────┐
    │     RX Chunk Payload (64 B)      │  RX Data Footer  │
    │                                  │     (4 bytes)    │
    └──────────────────────────────────┴──────────────────┘

The RX data footer carries EXST (Extended Status), HDRB (Header Bad),
SYNC, RCA (Receive Chunks Available), TXC (Transmit Credits), DV, SV,
EV, SWO, EBO, FD (Frame Drop), VS, RTSA/RTSP (timestamps), and a
parity bit.

Credit-Based Flow Control
~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAC-PHY exposes two credit counters to the host in every receive
data footer:

- **TXC** (Transmit Credits) — the minimum number of data chunks the
  host can write with DV=1 without causing a transmit buffer overflow.
- **RCA** (Receive Chunks Available) — the minimum number of additional
  data chunks containing valid receive frame data available for reading.

These same values are also readable via the Buffer Status Register
(``BUFST``, MMS 0, offset 0x000B). The driver reads BUFST at the
beginning of each ``oa_tc6_thread()`` call to seed the initial credit
values, then updates them from the footer of every subsequent data
chunk exchange.

The host must respect TXC: sending data chunks with DV=1 when TXC=0
triggers a Transmit Buffer Overflow Error (TXBOE) in STATUS0, and the
frame data is dropped.

Configuration Synchronization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After reset, the MAC-PHY clears the SYNC bit in CONFIG0. While SYNC=0
the device ignores all transmit data chunks and does not transfer
Ethernet frames in receive data chunks. The host must:

1. Perform a software reset (``oa_tc6_sw_reset()``).
2. Configure registers as needed (chunk size, filters, etc.).
3. Set SYNC=1 in CONFIG0 to begin normal data operation.

The ``oa_tc6_init()`` function handles step 1 automatically. The
MAC-specific driver (e.g. ADIN1140) handles steps 2–3 during its own
initialisation.

Protected SPI (PROTE)
~~~~~~~~~~~~~~~~~~~~~~

When the PROTE bit is set in CONFIG0, each 32-bit register data word in
a control transaction is followed by its ones' complement. This allows
the receiver to detect single-bit errors by XORing the data word with
its complement — any zero bits in the result indicate corruption.

The driver supports protected SPI via the ``prote_spi`` flag in the init
parameters. When enabled, control reads validate the complement and
return ``-EINVAL`` on mismatch, and control writes append the complement
automatically.

IRQn and Interrupt Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAC-PHY asserts IRQn (active low) when CSn is deasserted and any
of the following become true:

- Receive chunks become available (RCA transitions from 0 to non-zero).
- Transmit credits exceed the configured threshold (TXC ≥ TXCTHRESH).
- An extended status event occurs (EXST set from STATUS0/STATUS1).

The host should respond to IRQn by initiating a data transaction
(calling ``oa_tc6_thread()``) to obtain a fresh footer. The IRQn
sources are automatically deasserted on reception of the first data
header after CSn is asserted.

Driver Architecture
-------------------

The OA TC6 driver sits between the platform SPI layer and a MAC-specific
driver. It has no knowledge of the MAC's register map beyond the
standard TC6 registers (MMS 0). The MAC driver (e.g. ``adin1140.c``)
uses the OA TC6 public API to access registers and exchange Ethernet
frames.

::

    ┌─────────────────────────────────────────────────┐
    │              Application / lwIP                 │
    ├─────────────────────────────────────────────────┤
    │           MAC Driver (adin1140.c)               │
    │  adin1140_write_fifo()  adin1140_read_fifo()    │
    │  adin1140_reg_read()    adin1140_reg_write()    │
    ├─────────────────────────────────────────────────┤
    │              OA TC6 Driver (oa_tc6.c)           │
    │  oa_tc6_reg_read()      oa_tc6_get_tx_frame()  │
    │  oa_tc6_reg_write()     oa_tc6_put_tx_frame()  │
    │  oa_tc6_reg_update()    oa_tc6_get_rx_frame()  │
    │  oa_tc6_thread()        oa_tc6_put_rx_frame()  │
    ├─────────────────────────────────────────────────┤
    │          no_os_spi (platform HAL)               │
    └─────────────────────────────────────────────────┘

Control Transaction Path
-------------------------

Control transactions implement register read and write access to the
MAC-PHY. They are used for device configuration, status polling, and
MDIO access.

SPI Wire Format
~~~~~~~~~~~~~~~~

Each control command consists of a 32-bit control header followed by
register data. The header encodes:

- **DNC=0** — identifies this as a control (not data) transaction.
- **WNR** — Write-Not-Read: 1 for write, 0 for read.
- **AID** — Address Increment Disable. The driver always sets AID=1
  (single-register access).
- **MMS** — Memory Map Selector (4 bits), selects which register bank.
- **ADDR** — 16-bit register address within the selected MMS.
- **LEN** — Number of registers minus one. The driver always uses LEN=0
  (single register).
- **P** — Odd parity over bits 31:1.

The MMS and ADDR are combined into a single 32-bit value using the
``OA_MMS_REG(mms, addr)`` macro, which packs MMS into bits [19:16] and
ADDR into bits [15:0] of the resulting value. The control header
preparation functions extract and place these into the appropriate
header bit positions.

**Control Read** (single register):

::

    MOSI: [ Control Header (4B) ] [ Ignored (4B)       ] [ Ignored (4B)          ]
    MISO: [ Discard (4B)        ] [ Echoed Header (4B)  ] [ Register Data (4B)    ]

    With PROTE: adds 4B complement after register data on both MOSI echo
    and MISO read data.

The first 32 bits on MISO are undefined (the MAC-PHY may output
residual receive data from its shift register before recognising DNC=0).
The driver discards these by using the same buffer for TX and RX.

**Control Write** (single register):

::

    MOSI: [ Control Header (4B) ] [ Register Data (4B)  ] [ Ignored (4B)          ]
    MISO: [ Discard (4B)        ] [ Echoed Header (4B)  ] [ Echoed Reg Data (4B)  ]

    With PROTE: register data is followed by its ones' complement on MOSI,
    and the echo includes the complemented data as well.

Implementation
~~~~~~~~~~~~~~~

Control transactions are serialised by ``ctrl_lock`` (a mutex). The
public API functions acquire the lock, delegate to internal (lockless)
helpers, and release the lock:

- ``oa_tc6_reg_read()`` → ``__oa_tc6_reg_read()``
- ``oa_tc6_reg_write()`` → ``__oa_tc6_reg_write()``
- ``oa_tc6_reg_update()`` → ``__oa_tc6_reg_update()`` (atomic
  read-modify-write under a single lock acquisition)

The internal flow is:

1. ``oa_tc6_prepare_rx_ctrl()`` or ``oa_tc6_prepare_tx_ctrl()`` fills
   the ``ctrl_chunks[]`` buffer with the header (and data + complement
   for writes).
2. ``oa_tc6_do_ctrl_transfer()`` performs a single full-duplex SPI
   transfer using ``no_os_spi_transfer()``. The same buffer is used for
   both TX and RX (in-place).
3. For reads, the register value is extracted from the echoed response
   at offset ``2 * OA_HEADER_LEN``. If PROTE is enabled, the complement
   is validated.

The ``ctrl_chunks[]`` buffer is ``OA_SPI_CTRL_LEN`` (16) bytes, which
accommodates the maximum protected single-register transfer:
2 headers × 4 bytes + 1 data × 4 bytes + 1 complement × 4 bytes = 16
bytes.

Data Transaction Path
----------------------

Data transactions transfer Ethernet frames between the host and the
MAC-PHY. Frames are segmented into 64-byte chunks for transmission and
reassembled from chunks on reception.

TX Path: Host → MAC-PHY
~~~~~~~~~~~~~~~~~~~~~~~~~

The transmit path converts Ethernet frames into TC6 data chunks and
sends them over SPI. The sequence is:

1. **Acquire a frame buffer** — ``oa_tc6_get_tx_frame()`` scans the TX
   buffer pool for a buffer in ``OA_BUFF_FREE`` state, transitions it
   to ``OA_BUFF_TX_BUSY``, and returns a pointer. The caller fills in
   the ``data[]`` array, sets ``len`` (frame length), and optionally
   sets ``vs`` (Vendor Specific) and ``tsc`` (Timestamp Capture).

2. **Submit the frame** — ``oa_tc6_put_tx_frame()`` transitions the
   buffer state to ``OA_BUFF_TX_READY``, marking it eligible for
   transmission.

3. **Transfer** — ``oa_tc6_thread()`` performs the actual SPI
   exchange. Internally:

   a. ``oa_tc6_update_stats()`` reads the BUFST register to get current
      TXC and RCA values.
   b. ``oa_tc6_tx_frame_to_chunks()`` iterates over
      ``OA_BUFF_TX_READY`` buffers and serialises them into
      ``data_chunks[]``:

      - For each frame, it computes the number of 64-byte chunks needed.
      - The first chunk gets SV=1 and SWO=0 (frame starts at byte 0).
      - The last chunk gets EV=1 and EBO pointing to the last frame byte.
      - Middle chunks have DV=1 with SV=0 and EV=0.
      - If RCA > 0 but there are no (or no more) frames to transmit,
        dummy chunks with DV=0 are appended so the MAC-PHY can deliver
        its pending receive data.

   c. A single SPI transfer sends all chunks and receives the
      corresponding RX chunks in-place.
   d. ``oa_tc6_rx_chunk_to_frame()`` processes the received chunks
      (see RX path below).
   e. The loop repeats while there are more TX frames or RX data
      available, up to ``CONFIG_OA_THREAD_RX_LIMIT`` iterations.

The ``data_chunks[]`` buffer is ``OA_SPI_BUFF_LEN`` (1632) bytes,
sized to hold one maximum-length Ethernet frame (1514 bytes) as 24
chunks × 68 bytes (64 payload + 4 header).

RX Path: MAC-PHY → Host
~~~~~~~~~~~~~~~~~~~~~~~~~

The receive path reassembles Ethernet frames from the TC6 data chunks
returned by the MAC-PHY in the MISO direction during data transactions.

**RX frames are not delivered via callback or return value.**
``oa_tc6_thread()`` silently populates the internal RX frame buffer pool
(``user_rx_frame_buffer[]``) during every SPI exchange. Completed frames
accumulate in the pool with state ``OA_BUFF_RX_COMPLETE`` until the
caller explicitly retrieves them with ``oa_tc6_get_rx_frame()`` (or
``oa_tc6_get_rx_frame_match_vs()``). After processing, the caller must
return each buffer with ``oa_tc6_put_rx_frame()`` to make it available
for the next incoming frame.

If the caller does not drain the RX pool fast enough,
``oa_tc6_rx_chunk_to_frame()`` will fail with ``-ENOBUFS`` when it
cannot find a free buffer, and frames in transit are lost. The pool size
is ``CONFIG_OA_RX_FRAME_BUFF_NUM`` (default 5).

The typical receive loop after an ``oa_tc6_thread()`` call is:

.. code-block:: C

    oa_tc6_thread(oa);
    while (!oa_tc6_get_rx_frame(oa, &frame)) {
        process(frame->data, frame->len);
        oa_tc6_put_rx_frame(oa, frame);
    }

**Chunk reassembly** — ``oa_tc6_rx_chunk_to_frame()`` processes each
received chunk by parsing the 32-bit footer at the end of the chunk
payload:

1. **Transfer flags** — EXST, HDRB, and SYNC are extracted from every
   footer (even DV=0 chunks) and latched into ``desc->xfer_flags``.
   EXST and HDRB are sticky (OR-latched); SYNC is instantaneous.

2. **DV=0** — No valid data; skip to the next chunk.

3. **SV=1 && EV=1** — The chunk contains a complete frame (or the
   boundary between two frames):

   - If SBO > EBO, two frames share this chunk: the tail of the
     previous frame ends at byte EBO, and a new frame starts at byte
     offset SBO. The previous frame buffer is marked
     ``OA_BUFF_RX_COMPLETE``.
   - If SBO ≤ EBO, a single short frame fits entirely within the chunk.

4. **EV=1 only** — End of a multi-chunk frame. Data up to byte EBO is
   copied, and the buffer is marked ``OA_BUFF_RX_COMPLETE``.

5. **SV=1 only** — Start of a new frame at offset SBO. Data from SBO
   to the end of the chunk is copied into a fresh buffer, which enters
   ``OA_BUFF_RX_IN_PROGRESS``.

6. **Neither SV nor EV** — Continuation of a multi-chunk frame. The
   entire 64-byte payload is appended to the in-progress buffer.

After processing all chunks, the final footer's RCA and TXC fields
update ``desc->data_rx_credit`` and ``desc->data_tx_credit`` for the
next iteration.

When to Call ``oa_tc6_thread()``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``oa_tc6_thread()`` is the central pump of the OA TC6 driver. It is a
**bidirectional** function: each call both transmits pending TX frames
and receives any available RX data in the same SPI transaction.
Even when the caller only wants to transmit, any RX chunks in the
MAC-PHY's MISO response are processed and buffered. When the caller only
wants to receive (no TX frames pending), the driver sends dummy chunks
(DV=0) so the MAC-PHY can deliver its pending RX data.

The function does not block waiting for data — it reads the current
credit values, performs the exchanges that the credits allow, and
returns. The caller must invoke it at the right times:

**1. After submitting a TX frame**

After ``oa_tc6_put_tx_frame()``, the frame sits in
``OA_BUFF_TX_READY`` state but has not touched the SPI bus.
``oa_tc6_thread()`` must be called to serialise it into chunks and
transfer it to the MAC-PHY. Example from ``adin1140_write_fifo()``:

.. code-block:: C

    oa_tc6_get_tx_frame(oa, &frame);
    /* fill frame->data, set frame->len */
    oa_tc6_put_tx_frame(oa, frame);
    oa_tc6_thread(oa);   /* actually sends the frame */

**2. In response to IRQn (interrupt-driven RX)**

The MAC-PHY asserts IRQn when receive data becomes available (RCA
transitions 0 → non-zero while CSn is deasserted), when transmit
credits become available, or on an extended status event. The interrupt
handler (or the task it wakes) should call ``oa_tc6_thread()`` to
perform the data exchange, then drain any completed RX frames.

In the ``iperf_lwip_freertos`` example, the flow is::

    GPIO IRQ fires → adin1140_irq_cb() notifies net_task
    net_task wakes  → no_os_lwip_step()
                    → adin1140_step()
                    → adin1140_poll()
                    → oa_tc6_thread()       ← SPI exchange
                    → adin1140_read_frames() loop
                       → oa_tc6_get_rx_frame()
                       → oa_tc6_put_rx_frame()

**3. Polling (no-interrupt operation)**

Without IRQn wired up, the application must call ``oa_tc6_thread()``
periodically in its main loop. Each call checks whether the MAC-PHY
has pending RX data (via the BUFST register) and whether there are TX
frames queued. This works but adds latency proportional to the polling
interval.

**What happens inside a single call:**

1. Read BUFST to get initial TXC and RCA credits.
2. Enter a loop (up to ``CONFIG_OA_THREAD_RX_LIMIT`` iterations):

   a. Serialise pending ``OA_BUFF_TX_READY`` frames into chunks
      (respecting TXC). If RCA > 0 but no TX frames are queued, emit
      dummy chunks (DV=0) so the MAC-PHY can return its RX data.
   b. Perform one SPI transfer (all chunks in a single CSn assertion).
   c. Parse the received RX chunks into frame buffers.
   d. Update TXC and RCA from the last footer.
   e. If more TX frames are queued or RCA is still non-zero, loop.

3. Return 0 on success, or a negative error code if an SPI transfer or
   chunk parsing failed.

Frame Buffer State Machine
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each frame buffer (``struct oa_tc6_frame_buffer``) tracks its lifecycle
via the ``state`` field:

**TX buffers:**

::

    ┌───────────┐  get_tx_frame()  ┌──────────────┐  put_tx_frame()  ┌──────────────┐
    │ BUFF_FREE ├─────────────────►│ BUFF_TX_BUSY ├─────────────────►│BUFF_TX_READY │
    └─────▲─────┘                  └──────────────┘                  └──────┬───────┘
          │                                                                 │
          │                    oa_tc6_tx_frame_to_chunks()                   │
          └─────────────────────────────────────────────────────────────────┘

**RX buffers:**

::

    ┌───────────┐  rx_chunk_to_frame()  ┌───────────────────┐  rx_chunk_to_frame()
    │ BUFF_FREE ├──────────────────────►│BUFF_RX_IN_PROGRESS├──────────────────────┐
    └─────▲─────┘                       └───────────────────┘                      │
          │                                                                        ▼
          │  put_rx_frame()  ┌───────────────────┐  get_rx_frame()  ┌──────────────────┐
          └──────────────────┤BUFF_RX_USER_OWNED │◄─────────────────┤ BUFF_RX_COMPLETE  │
                             └───────────────────┘                  └──────────────────┘

Memory Allocation
-----------------

All memory is allocated in ``oa_tc6_init()`` as a single
``no_os_calloc()`` of ``struct oa_tc6_desc``. There are no further
dynamic allocations during operation. The descriptor contains:

.. list-table::
   :header-rows: 1
   :widths: 35 15 50

   * - Field
     - Size
     - Purpose
   * - ``ctrl_chunks[OA_SPI_CTRL_LEN]``
     - 16 B
     - SPI buffer for control transactions (header + data + complement).
   * - ``data_chunks[OA_SPI_BUFF_LEN]``
     - 1632 B
     - SPI buffer for data transactions. Sized for 24 chunks × 68 B
       (one max-length Ethernet frame plus headers).
   * - ``user_tx_frame_buffer[N]``
     - N × ~1526 B
     - TX frame pool. N = ``CONFIG_OA_TX_FRAME_BUFF_NUM`` (default 2).
       Each entry holds a full Ethernet frame (``CONFIG_OA_CHUNK_BUFFER_SIZE``
       = 1514 B data) plus metadata.
   * - ``user_rx_frame_buffer[N]``
     - N × ~1526 B
     - RX frame pool. N = ``CONFIG_OA_RX_FRAME_BUFF_NUM`` (default 5).
   * - ``ctrl_lock``, ``data_lock``
     - platform-dependent
     - Mutexes for serialising control and data path access.

With default settings the descriptor is approximately **11 KB**. The
dominant cost is the 7 frame buffers (2 TX + 5 RX) at ~1.5 KB each.
For throughput-oriented builds these pools are typically enlarged (the
adin1140 iperf examples raise ``CONFIG_OA_RX_FRAME_BUFF_NUM`` to 10);
for memory-constrained targets, reduce ``CONFIG_OA_TX_FRAME_BUFF_NUM``
and ``CONFIG_OA_RX_FRAME_BUFF_NUM`` in the project build.

Compile-Time Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

The following macros may be defined before including ``oa_tc6.h`` (or
via compiler flags) to override defaults. In the CMake build,
``CONFIG_OA_RX_FRAME_BUFF_NUM`` and ``CONFIG_OA_THREAD_RX_LIMIT`` are
exposed as Kconfig options under the OA TC6 driver (see
``drivers/net/oa_tc6/Kconfig``) and can be set from a project defconfig:

.. list-table::
   :header-rows: 1
   :widths: 35 15 50

   * - Macro
     - Default
     - Description
   * - ``CONFIG_OA_TX_FRAME_BUFF_NUM``
     - 2
     - Number of TX frame buffers in the pool.
   * - ``CONFIG_OA_RX_FRAME_BUFF_NUM``
     - 5
     - Number of RX frame buffers in the pool.
   * - ``CONFIG_OA_CHUNK_BUFFER_SIZE``
     - 1514
     - Maximum Ethernet frame size (bytes) each buffer can hold.
   * - ``CONFIG_OA_THREAD_RX_LIMIT``
     - 5
     - Maximum number of SPI data exchanges per ``oa_tc6_thread()``
       call, preventing unbounded loop time.
   * - ``CONFIG_OA_ZERO_SWO_ONLY``
     - 1
     - When set, configures ZARFE in CONFIG0 so that received frames
       always start at SWO=0 (the beginning of the chunk payload).
       Simplifies RX parsing at a small performance cost.

Public API
----------

Lifecycle
~~~~~~~~~~

``oa_tc6_init(struct oa_tc6_desc **desc, struct oa_tc6_init_param *param)``
    Allocate the descriptor, initialise mutexes, perform a software
    reset of the MAC-PHY, and optionally enable ZARFE. The SPI
    descriptor must already be initialised by the caller. Returns 0
    on success.

``oa_tc6_remove(struct oa_tc6_desc *desc)``
    Free mutexes and the descriptor.

Register Access
~~~~~~~~~~~~~~~~

``oa_tc6_reg_read(desc, addr, &val)``
    Read a single 32-bit register. ``addr`` is an ``OA_MMS_REG(mms, offset)``
    value. Thread-safe (acquires ``ctrl_lock``).

``oa_tc6_reg_write(desc, addr, val)``
    Write a single 32-bit register. Thread-safe.

``oa_tc6_reg_update(desc, addr, val, mask)``
    Atomic read-modify-write: reads the register, clears bits in
    ``mask``, OR-s in ``val & mask``, and writes back. The entire
    sequence is performed under a single ``ctrl_lock`` acquisition.

``oa_tc6_sw_reset(desc)``
    Write SWRESET to the RESET register, poll STATUS0.RESETC every 1 ms
    for up to 1 second, then clear RESETC via write-1-to-clear.

Frame Transmission
~~~~~~~~~~~~~~~~~~~

``oa_tc6_get_tx_frame(desc, &buffer)``
    Acquire a free TX frame buffer. Returns ``-ENOBUFS`` if all buffers
    are in use. The caller fills ``buffer->data[]`` with the Ethernet
    frame, sets ``buffer->len``, and optionally sets ``buffer->vs``
    and ``buffer->tsc``.

``oa_tc6_put_tx_frame(desc, buffer)``
    Mark the buffer as ready for transmission (``OA_BUFF_TX_READY``).
    The frame is not yet on the wire — ``oa_tc6_thread()`` must be
    called to perform the actual SPI transfer.

``oa_tc6_thread(desc)``
    Perform the SPI data exchange: reads BUFST for credits, serialises
    pending TX frames into chunks, transfers them, and parses received
    RX chunks into frame buffers. This is the main driver pump — see
    `When to Call oa_tc6_thread()`_ for detailed guidance on when and
    how to invoke it.

Frame Reception
~~~~~~~~~~~~~~~~

``oa_tc6_get_rx_frame(desc, &buffer)``
    Retrieve the first completed RX frame buffer. Returns ``-ENOENT``
    if no complete frames are available. The buffer transitions to
    ``OA_BUFF_RX_USER_OWNED``. Frames are populated by
    ``oa_tc6_thread()`` — see `RX Path: MAC-PHY → Host`_ for details
    on how frames are buffered.

``oa_tc6_get_rx_frame_match_vs(desc, &buffer, vs, mask)``
    Like ``oa_tc6_get_rx_frame()`` but only returns a frame whose VS
    field matches ``(vs & mask)``. Useful for multi-port MAC-PHYs
    (e.g. ADIN2111) where VS identifies the ingress port.

``oa_tc6_put_rx_frame(desc, buffer)``
    Return the buffer to the free pool after the caller has finished
    processing the frame data. Failing to call this will eventually
    exhaust the RX pool (``CONFIG_OA_RX_FRAME_BUFF_NUM`` buffers),
    causing incoming frames to be dropped.

Transfer Flags
~~~~~~~~~~~~~~~

``oa_tc6_get_xfer_flags(desc, &flags, clear)``
    Retrieve the latched transfer flags (EXST, HDRB, SYNC) from the
    most recent data exchange. If ``clear`` is true, the latched values
    are reset. The EXST and HDRB flags are sticky (latched high until
    cleared); SYNC reflects the instantaneous value.

Usage From a MAC Driver
-----------------------

A MAC driver uses the OA TC6 API as follows:

.. code-block:: C

    #include "oa_tc6.h"

    /* Initialisation (called once) */
    struct oa_tc6_init_param oa_param = {
        .comm_desc = spi_desc,
        .prote_spi = true,
    };

    struct oa_tc6_desc *oa;
    ret = oa_tc6_init(&oa, &oa_param);

    /* Register access (configure MAC/PHY registers) */
    oa_tc6_reg_write(oa, OA_MMS_REG(1, 0x0010), 0x1234);
    oa_tc6_reg_read(oa, OA_MMS_REG(0, 0x0004), &val);
    oa_tc6_reg_update(oa, OA_MMS_REG(0, 0x0004), sync_bit, sync_bit);

    /* Transmit a frame */
    struct oa_tc6_frame_buffer *tx;
    ret = oa_tc6_get_tx_frame(oa, &tx);
    memcpy(tx->data, eth_frame, frame_len);
    tx->len = frame_len;
    oa_tc6_put_tx_frame(oa, tx);
    oa_tc6_thread(oa);   /* sends the frame and receives pending RX */

    /* Receive frames (after oa_tc6_thread populated RX buffers) */
    struct oa_tc6_frame_buffer *rx;
    while (!oa_tc6_get_rx_frame(oa, &rx)) {
        process_frame(rx->data, rx->len);
        oa_tc6_put_rx_frame(oa, rx);
    }

    /* IRQn handler (interrupt-driven operation) */
    void macphy_irq_handler(void *ctx)
    {
        oa_tc6_thread(oa);
        /* Then retrieve RX frames as above */
    }

    /* Cleanup */
    oa_tc6_remove(oa);

Concurrency
-----------

The driver provides two mutexes:

- ``ctrl_lock`` — serialises all control (register) transactions. The
  public ``oa_tc6_reg_read/write/update`` functions acquire and release
  this lock automatically.
- ``data_lock`` — serialises the data path inside ``oa_tc6_thread()``.

The frame buffer API functions (``oa_tc6_get_tx_frame``,
``oa_tc6_put_tx_frame``, ``oa_tc6_get_rx_frame``, ``oa_tc6_put_rx_frame``,
``oa_tc6_get_xfer_flags``) are **not** protected by any lock. In a
multi-threaded environment, the MAC driver must ensure that these
functions and ``oa_tc6_thread()`` are called from the same execution
context, or must provide its own external synchronisation.

On platforms without an RTOS the mutexes are no-ops (the default
``no_os_mutex.c`` stubs are empty). On FreeRTOS they map to binary
semaphores.
