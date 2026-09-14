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

* **Data transactions** move Ethernet frames as a stream of fixed 64-byte
  data chunks (``OA_CHUNK_SIZE``). Every transmit chunk is prefixed with a
  4-byte data header and every receive chunk is suffixed with a 4-byte data
  footer, each protected by a parity bit and a CRC. The header/footer carry
  the framing bits (``DV`` data valid, ``SV``/``SWO`` start valid and offset,
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

The driver keeps two pools of frame buffers in the descriptor — one for
transmit, one for receive — each managed as a FIFO ring so frames are
serviced strictly in order. A single pump function, ``oa_tc6_thread()``,
performs one full service cycle: it frames every buffer in the
``OA_BUFF_TX_READY`` state out to SPI and de-frames the received chunks back
into free receive buffers. The caller invokes it from its main loop or in
response to the MAC-PHY's interrupt pin.

When the host has no data to transmit but the device has frames waiting, the
driver issues empty data chunks (data header with ``DV = 0``) purely to clock
receive chunks out of the MAC-PHY, so receive is never starved by an idle
transmit path.

OA-TC6 Driver Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

The caller must first bring up a ``no_os_spi_desc`` (SPI mode 0). The first
API to call is **oa_tc6_init**, passing that SPI descriptor in
``struct oa_tc6_init_param`` along with:

* ``bufst_polling`` — how transmit/receive credits are refreshed before a data
  transaction: ``OA_TC6_REG_POLL`` reads the ``BUFST`` (0x0B) register, while
  ``OA_TC6_FOOTER_POLL`` derives the credits from the footer of a data chunk
  (one fewer control transaction per cycle).
* ``prote_spi`` — set when the MAC-PHY is configured for protected
  (complement-checked) control transactions.

Make sure **oa_tc6_init** returns 0, which means the driver was initialized
correctly.

Register Access
~~~~~~~~~~~~~~~

**oa_tc6_reg_read**, **oa_tc6_reg_write** and **oa_tc6_reg_update** provide
32-bit access to the MMS-encoded register space (read-modify-write for the
update helper). Addresses are built with ``OA_MMS_REG(mms, reg)``.

Frame Transmit and Receive
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Frames are exchanged through borrow/return buffer handles rather than copies:

* **oa_tc6_get_tx_frame** hands out a free transmit buffer; the caller fills
  ``data``/``len`` and returns it with **oa_tc6_put_tx_frame**, which marks it
  ``OA_BUFF_TX_READY``.
* **oa_tc6_get_rx_frame** returns the oldest completed receive frame (or
  **oa_tc6_get_rx_frame_match_vs** to fetch one whose Vendor-Specific field
  matches). After consuming it, the caller returns the buffer to the pool with
  **oa_tc6_put_rx_frame**.
* **oa_tc6_thread** is the transfer pump described above and must be called
  periodically to make progress on both directions.

The ``struct oa_tc6_frame_buffer`` handle also exposes per-frame metadata:
``vs`` (Vendor Specific), ``tsc`` (transmit timestamp capture request),
``frame_drop`` (receive frame flagged invalid), and ``rtsa``/``rtsp``
(receive timestamp added / parity).

Events and Callbacks
~~~~~~~~~~~~~~~~~~~~

**oa_tc6_register_callback** installs a user callback that the driver invokes,
with an ``enum oa_tc6_event`` code, when notable conditions occur during a
service cycle:

* ``OA_TC6_EVENT_RX`` / ``OA_TC6_EVENT_TX`` — a frame was received / fully
  transmitted.
* ``OA_TC6_EVENT_TXPE`` — transmit protocol error.
* ``OA_TC6_EVENT_TXBOE`` / ``OA_TC6_EVENT_TXBUE`` — transmit buffer overflow /
  underflow.
* ``OA_TC6_EVENT_RXBOE`` — receive buffer overflow.
* ``OA_TC6_EVENT_LOFE`` — loss of frame.
* ``OA_TC6_EVENT_HDRE`` — header error.
* ``OA_TC6_EVENT_SYNCE`` — loss of synchronization.

The error events are decoded from the ``STATUS0`` register after the footer
latches ``EXST``.

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

* ``rx_frames`` / ``rx_bytes`` — frames and payload bytes de-framed to buffers.
* ``tx_frames`` / ``tx_bytes`` — frames and payload bytes framed out to SPI.
* ``rx_drop_fd`` — receive frames dropped because the footer flagged them
  invalid (``FD``).
* ``rx_drop_nobuf`` — receive frames dropped because the buffer pool was
  exhausted.
* ``exst_events`` — number of data-chunk footers that latched ``EXST``.

Compile-time Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The buffer pools and service limits are tunable through Kconfig (or by
overriding the macros before including the header):

* ``CONFIG_OA_TX_FRAME_BUFF_NUM`` (default 10) — transmit buffer pool depth.
* ``CONFIG_OA_RX_FRAME_BUFF_NUM`` (default 10) — receive buffer pool depth.
* ``CONFIG_OA_CHUNK_BUFFER_SIZE`` (default 1514) — per-frame payload capacity.
* ``CONFIG_OA_THREAD_RX_LIMIT`` (default 5) — maximum frames received per
  ``oa_tc6_thread`` call, bounding how long a single service cycle may run.
* ``CONFIG_OA_ZERO_SWO_ONLY`` (default 1) — only accept frames that start at
  chunk offset zero.

OA-TC6 Driver Initialization Example
------------------------------------

.. code-block:: bash

	struct oa_tc6_desc *oa_desc;
	struct no_os_spi_desc *spi_desc;

	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct oa_tc6_init_param oa_ip = {
		.bufst_polling = OA_TC6_FOOTER_POLL,
		.prote_spi = false,
	};

	ret = no_os_spi_init(&spi_desc, &spi_ip);
	if (ret)
		goto error;

	oa_ip.comm_desc = spi_desc;

	ret = oa_tc6_init(&oa_desc, &oa_ip);
	if (ret)
		goto error;
