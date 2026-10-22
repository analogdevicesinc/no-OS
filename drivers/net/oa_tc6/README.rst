OA-TC6 no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

The OA-TC6 layer is a transport driver rather than a device driver: it
implements the OPEN Alliance TC6 (10BASE-T1x MAC-PHY Serial Interface, or
"OA-SPI") protocol and is shared by the no-OS MAC-PHY drivers that speak it.

- :adi:`ADIN1110`
- :adi:`ADIN2111`
- :adi:`ADIN1140`

Overview
--------

OA-TC6 defines a chunk-based SPI protocol for exchanging Ethernet frames and
control registers with a MAC-PHY that has no parallel MII/RMII interface. All
traffic — frame data and register access alike — is multiplexed over the same
4-wire SPI link.

Two kinds of SPI transaction are used:

* **Control transactions** read and write the MAC-PHY register space. Each
  register lives in a Memory Map Selector (MMS) block and is addressed with
  the ``OA_MMS_REG(mms, reg)`` helper. Control transactions are optionally
  protected (``prote_spi``), in which case each register is transferred
  together with its bit-wise complement for integrity checking.

* **Data transactions** move Ethernet frames as a stream of data chunks of
  8, 16, 32 or 64 bytes (``CONFIG0.CPS``). Every transmit chunk is prefixed
  with a 4-byte data header and every receive chunk is suffixed with a 4-byte
  data footer, each protected by a parity bit. The header/footer carry the
  framing bits (``DV`` data valid, ``SV``/``SWO`` start valid and offset,
  ``EV``/``EBO`` end valid and offset) that let a frame span an arbitrary
  number of chunks.

Flow control is credit-based. The footer reports the number of receive chunks
available (``RCA``) and the remaining transmit credit (``TXC``); the driver
never sends more data chunks than the MAC-PHY has buffer space for, and reads
exactly the number of chunks the device has queued. The footer also latches
the ``EXST`` (extended status), ``HDRB`` (header bad) and ``SYNC`` bits that
surface device-level events.

Driver Architecture
-------------------

The caller allocates the descriptor and the frame buffer pools. The receive
pool and each transmit queue are FIFO rings, so frames are serviced strictly
in order. A pump function performs one service cycle: it frames the buffers in
the ``OA_BUFF_TX_READY`` state out to SPI and de-frames the received chunks
back into free receive buffers. **oa_tc6_thread** blocks for each SPI
transaction; **oa_tc6_thread_async** starts it with DMA and returns, and
processes the received chunks on a later call. The caller invokes the pump
from its main loop or in response to the MAC-PHY's interrupt pin, setting
``irq_pending`` in the descriptor so the next cycle reads a footer.

A transmit buffer is released only once the MAC-PHY accepted all its chunks.
A failed SPI transfer, a rejected header (``HDRB``) or a footer with
``SYNC = 0`` leaves the frame queued, and it is sent again.

When the host has no data to transmit but the device has frames waiting, the
driver issues empty data chunks (data header with ``DV = 0``) purely to clock
receive chunks out of the MAC-PHY, so receive is never starved by an idle
transmit path. When the receive pool is full, the driver sets ``NORX`` in the
headers.

Transmit Queues
~~~~~~~~~~~~~~~

Transmit frames go through one or more priority queues, each with its own
frame buffer ring and wire ``VS`` value. The transmit credit of a queue is only
reported in the footer of a chunk stamped with its ``VS``, so a queue waiting
for credit has an empty chunk with its ``VS`` sent to solicit it.

* ``tx_sched`` selects strict priority (``OA_TC6_TX_SCHED_STRICT_PRIO``, the
  highest-index queue first) or round-robin
  (``OA_TC6_TX_SCHED_ROUND_ROBIN``).
* **oa_tc6_set_tx_queues** replaces the queue set after init.
* **oa_tc6_set_tx_credit_reserve** keeps a number of chunks free in every
  transmit FIFO, for a MAC-PHY whose footer ``TXC`` reports only the FIFO that
  last received an ``SV``.

OA-TC6 Driver Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The caller must first bring up a CAPI SPI device (``struct capi_spi_device``,
SPI mode 0). The first API to call is **oa_tc6_init**, passing a descriptor
and ``struct oa_tc6_init_param`` with:

* ``comm_desc`` — the CAPI SPI device.
* ``rx_pool`` / ``rx_pool_size`` — the receive frame buffer pool.
* ``tx_pool`` / ``tx_pool_size`` for a single transmit queue, or ``tx_q`` /
  ``tx_q_count`` for priority queues, and ``tx_sched``.
* ``prote_spi`` — use protected (complement-checked) control transactions. A
  MAC-PHY that boots protected is detected and kept protected.
* ``cps`` — the chunk payload size, an ``OA_TC6_CONFIG0_CPS_*`` value
  (default 64 bytes).
* ``no_reset`` — skip the soft reset, for a MAC-PHY fresh out of a power-on
  reset.
* ``sw_fcs`` / ``tx_no_pad`` — compute and check the Ethernet FCS on the host,
  and optionally leave short transmit frames unpadded.
* ``async_mode`` / ``async_xfer_done_cb`` — enable **oa_tc6_thread_async**,
  with an optional callback run from the SPI completion interrupt.
* ``ctrl_mutex_cfg`` / ``data_mutex_cfg`` — CAPI mutexes for multi-threaded
  use. ``ctrl_lock`` must be recursive. Leave them NULL for bare-metal
  single-threaded use.

Make sure **oa_tc6_init** returns 0, which means the driver was initialized
correctly.

Register Access
~~~~~~~~~~~~~~~

**oa_tc6_reg_read**, **oa_tc6_reg_write** and **oa_tc6_reg_update** provide
32-bit access to the MMS-encoded register space (read-modify-write for the
update helper). Addresses are built with ``OA_MMS_REG(mms, reg)``.
**oa_tc6_reg_read_multiple** and **oa_tc6_reg_write_multiple** access up to
``OA_CTRL_MAX_REGS`` consecutive registers in one control transaction. The
echo of every control transaction is checked.

Runtime Configuration
~~~~~~~~~~~~~~~~~~~~~

**oa_tc6_set_config** and **oa_tc6_get_config** program and read back the
``CONFIG0`` fields that may change after init (``struct oa_tc6_config``):
``txcthresh``, cut-through (``txcte``/``rxcte``), receive frame alignment
(``csarfe``/``zarfe``) and frame timestamping (``ftse``/``ftss``). Unless
``zarfe`` is set, the chunk that ends a transmit frame also starts the next
frame of the same queue.

Frame Transmit and Receive
~~~~~~~~~~~~~~~~~~~~~~~~~~

Frames are exchanged through borrow/return buffer handles rather than copies:

* **oa_tc6_get_tx_frame** hands out a free transmit buffer of a queue; the
  caller fills ``data``/``len`` and returns it with **oa_tc6_put_tx_frame**,
  which marks it ``OA_BUFF_TX_READY``.
* **oa_tc6_get_rx_frame** returns the oldest completed receive frame (or
  **oa_tc6_get_rx_frame_match_vs** to fetch one whose Vendor-Specific field
  matches). After consuming it, the caller returns the buffer to the pool with
  **oa_tc6_put_rx_frame**. The FCS is not included in ``len``.
* **oa_tc6_thread** or **oa_tc6_thread_async** must be called periodically to
  make progress on both directions.

The ``struct oa_tc6_frame_buffer`` handle also exposes per-frame metadata:
``vs`` (Vendor Specific), ``tsc`` (transmit timestamp capture request),
``frame_drop`` (receive frame flagged invalid), and ``rtsa``/``rtsp``
(receive timestamp added / parity).

Events and Callbacks
~~~~~~~~~~~~~~~~~~~~

**oa_tc6_register_callback** installs a user callback that the driver invokes
from the pump, never from an interrupt, with an ``enum oa_tc6_event`` code, a
detail pointer and the user argument:

* ``OA_TC6_EVENT_RX`` / ``OA_TC6_EVENT_TX`` — a frame was received / the
  MAC-PHY accepted queued transmit frames, freeing their buffers.
* ``OA_TC6_EVENT_RX_READY`` — the MAC-PHY has receive chunks waiting.
* ``OA_TC6_EVENT_TX_CREDITS`` — transmit credit recovered from zero.
* ``OA_TC6_EVENT_RESET`` — the MAC-PHY reset and lost its configuration
  (``SYNC = 0`` or ``STATUS0.RESETC``). Reconfigure it; the driver does not set
  ``CONFIG0.SYNC`` again on its own.
* ``OA_TC6_EVENT_ERR_*`` — one event per error bit set in ``STATUS0`` /
  ``STATUS1``, with the detail pointing at ``struct oa_tc6_error_detail``, and
  the footer parity (``OA_TC6_EVENT_ERR_RX_PARITY``) and frame drop
  (``OA_TC6_EVENT_ERR_RX_FRAME_DROP``) errors.
* ``OA_TC6_EVENT_ERROR`` — a ``STATUS0`` bit set that maps to no other event.

``STATUS0`` and ``STATUS1`` are read and cleared once per service cycle after
a footer latches ``EXST`` or ``HDRB``.

Transfer Flags
~~~~~~~~~~~~~~

**oa_tc6_get_xfer_flags** returns the latched footer status (``exst``,
``hdrb``, ``sync``) and optionally clears the latch. ``sync`` reflects the
instantaneous data-synchronization state; ``exst`` and ``hdrb`` stay latched
until read so a transient event is not missed between service cycles.

Software Reset
~~~~~~~~~~~~~~

**oa_tc6_sw_reset** writes the ``RESET`` register and polls
``STATUS0.RESETC`` until the MAC-PHY reports the reset has completed.

Statistics
~~~~~~~~~~

The driver keeps software counters in ``struct oa_tc6_stats``, read with
**oa_tc6_get_stats** and cleared with **oa_tc6_reset_stats**:

* ``rx_frames`` / ``rx_bytes`` — frames and payload bytes delivered to the
  receive pool.
* ``tx_frames`` / ``tx_bytes`` — frames and payload bytes accepted by the
  MAC-PHY.
* ``rx_drop_fd`` — receive frames the footer flagged invalid (``FD``).
* ``rx_drop_nobuf`` — receive frames dropped because the buffer pool was full.
* ``exst_events`` — number of data-chunk footers that latched ``EXST``.
* ``rx_footer_parity_err`` — data-chunk footers with a parity error.

Compile-time Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

The service limits and buffer sizes are tunable through Kconfig (or by
overriding the macros before including the header):

* ``CONFIG_OA_CHUNK_BUFFER_SIZE`` (default 1530) — per-frame payload capacity.
* ``CONFIG_OA_SPI_BUFF_LEN`` (default 1632) — SPI data transaction buffer
  size, which bounds the chunks per transaction.
* ``CONFIG_OA_THREAD_RX_LIMIT`` (default 5) — maximum data transactions per
  service cycle while receive chunks are waiting.
* ``CONFIG_OA_ASYNC_XFER_TIMEOUT_US`` (default 50000) — watchdog for an
  asynchronous transfer; 0 disables it.

OA-TC6 Driver Initialization Example
------------------------------------

.. code-block:: c

	static struct oa_tc6_frame_buffer tx_pool[2];
	static struct oa_tc6_frame_buffer rx_pool[5];
	static struct oa_tc6_desc oa_desc;

	struct oa_tc6_init_param oa_ip = {
		.comm_desc = spi_dev,	/* CAPI SPI device, SPI mode 0 */
		.tx_pool = tx_pool,
		.tx_pool_size = NO_OS_ARRAY_SIZE(tx_pool),
		.rx_pool = rx_pool,
		.rx_pool_size = NO_OS_ARRAY_SIZE(rx_pool),
	};

	ret = oa_tc6_init(&oa_desc, &oa_ip);
	if (ret)
		goto error;
