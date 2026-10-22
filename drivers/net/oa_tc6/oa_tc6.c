/*******************************************************************************
 *   @file   oa_tc6.c
 *   @brief  Implementation for the Open Alliance TC6 SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "capi_mutex.h"
#include "capi_time.h"
#include "oa_tc6.h"

/*
 * Built-in no-op mutex, shared by every descriptor that gets no
 * ctrl_mutex_cfg / data_mutex_cfg (bare-metal single-threaded). Keeps
 * ctrl_lock/data_lock always non-NULL so lock/unlock never need a NULL
 * guard; the ops all ignore the handle, so sharing one sentinel across
 * descriptors is safe.
 */
static int noop_mutex_nop(struct capi_mutex *h)
{
	(void)h;
	return 0;
}

static const struct capi_mutex_ops noop_mutex_ops = {
	.deinit = noop_mutex_nop,
	.lock = noop_mutex_nop,
	.unlock = noop_mutex_nop,
};

static struct capi_mutex oa_noop_mutex = { .ops = &noop_mutex_ops };

uint8_t oa_tc6_crc1(uint32_t header)
{
	/* Branchless odd-parity over 32 bits: fold by halvings then invert. */
	header ^= header >> 16;
	header ^= header >> 8;
	header ^= header >> 4;
	header ^= header >> 2;
	header ^= header >> 1;
	return (~header) & 0x1;
}

/* Ethernet minimum frame length on the wire, excluding the 4-byte FCS. */
#define OA_TC6_ETH_MIN_LEN 60
#define OA_TC6_FCS_LEN 4

/* CONFIG0 fields owned by oa_tc6_set_config() */
#define OA_TC6_CONFIG0_RUNTIME_MASK                                  \
	(OA_TC6_CONFIG0_TXFCSVE_MASK | OA_TC6_CONFIG0_CSARFE_MASK |  \
	 OA_TC6_CONFIG0_ZARFE_MASK | OA_TC6_CONFIG0_TXCTHRESH_MASK | \
	 OA_TC6_CONFIG0_TXCTE_MASK | OA_TC6_CONFIG0_RXCTE_MASK |     \
	 OA_TC6_CONFIG0_FTSE_MASK | OA_TC6_CONFIG0_FTSS_MASK)

/**
 * @brief Compute the Ethernet FCS (CRC-32) over a frame.
 *
 * Reflected CRC-32 (poly 0xEDB88320, init 0xFFFFFFFF, final inversion), the
 * standard IEEE 802.3 FCS. Returned in host byte order; append little-endian.
 *
 * @param data - frame bytes.
 * @param len - number of bytes to checksum.
 * @return the 32-bit FCS.
 */
static uint32_t oa_tc6_fcs_crc32(const uint8_t *data, uint32_t len)
{
	uint32_t crc = 0xFFFFFFFFu;
	uint32_t i;
	int bit;

	for (i = 0; i < len; i++) {
		crc ^= data[i];
		for (bit = 0; bit < 8; bit++)
			crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1));
	}

	return ~crc;
}

/*
 * Build an OA-TC6 control header: MMS+ADDR, LEN = count - 1, AID set only
 * for a single register (count == 1), WNR if writing.
 */
static uint32_t oa_tc6_ctrl_header(uint32_t addr, uint8_t count, bool write)
{
	uint32_t h = no_os_field_prep(OA_CTRL_ADDR_MMS_MASK, addr);

	h |= no_os_field_prep(OA_CTRL_LEN_MASK, count - 1);
	h |= no_os_field_prep(OA_CTRL_AID_MASK, count == 1);
	if (write)
		h |= no_os_field_prep(OA_CTRL_WNR_MASK, 1);

	return h | oa_tc6_crc1(h);
}

/* True if data is the PROTE ones'-complement of inv. */
static bool oa_tc6_prote_ok(uint32_t data, uint32_t inv)
{
	return data == (inv ^ NO_OS_GENMASK(31, 0));
}

static int oa_tc6_ctrl_transceive(struct oa_tc6_desc *desc, uint32_t len);

/*
 * One control transaction of count registers, built in ctrl_chunks.
 *
 * MOSI: the header, then for a write the count data words, each followed by
 * its ones'-complement under PROTE, then one trailing word. A read sends the
 * same length with the data slots left zero.
 *
 * The MAC-PHY echoes the sent stream on MISO shifted by one header word:
 * ctrl_chunks[HDR:2*HDR) carries the echoed header, then come the read data
 * (with complements under PROTE) or, for a write, the echoed data words,
 * which are checked against what was sent.
 */
static int oa_tc6_ctrl_xfer(struct oa_tc6_desc *desc, uint32_t addr,
			    uint32_t *val, uint8_t count, bool write)
{
	uint32_t word_stride = desc->prote_spi ? 2 : 1; /* words per register */
	uint32_t data_bytes = count * OA_REG_LEN * word_stride;
	uint8_t *buf = desc->ctrl_chunks;
	uint32_t header, total, off, comp;
	uint8_t i;
	int ret;

	if (!count || count > OA_CTRL_MAX_REGS)
		return -EINVAL;

	total = 2 * OA_HEADER_LEN + data_bytes;
	header = oa_tc6_ctrl_header(addr, count, write);

	memset(buf, 0, total);
	no_os_put_unaligned_be32(header, buf);

	if (write) {
		off = OA_HEADER_LEN;
		for (i = 0; i < count; i++) {
			no_os_put_unaligned_be32(val[i], &buf[off]);
			if (desc->prote_spi)
				no_os_put_unaligned_be32(
					val[i] ^ NO_OS_GENMASK(31, 0),
					&buf[off + OA_REG_LEN]);
			off += OA_REG_LEN * word_stride;
		}
	}

	ret = oa_tc6_ctrl_transceive(desc, total);
	if (ret)
		return ret;

	/* A mismatched echo means the control path is desynced. */
	if (no_os_get_unaligned_be32(&buf[OA_HEADER_LEN]) != header)
		return -EIO;

	off = 2 * OA_HEADER_LEN;
	for (i = 0; i < count; i++) {
		if (write) {
			if (no_os_get_unaligned_be32(&buf[off]) != val[i])
				return -EIO;
			if (desc->prote_spi &&
			    no_os_get_unaligned_be32(&buf[off + OA_REG_LEN]) !=
				    (val[i] ^ NO_OS_GENMASK(31, 0)))
				return -EIO;
		} else {
			val[i] = no_os_get_unaligned_be32(&buf[off]);
			if (desc->prote_spi) {
				comp = no_os_get_unaligned_be32(
					&buf[off + OA_REG_LEN]);
				if (!oa_tc6_prote_ok(val[i], comp))
					return -EINVAL;
			}
		}
		off += OA_REG_LEN * word_stride;
	}

	return 0;
}

static int __oa_tc6_reg_read(struct oa_tc6_desc *desc, uint32_t addr,
			     uint32_t *val)
{
	return oa_tc6_ctrl_xfer(desc, addr, val, 1, false);
}

static int __oa_tc6_reg_write(struct oa_tc6_desc *desc, uint32_t addr,
			      uint32_t val)
{
	return oa_tc6_ctrl_xfer(desc, addr, &val, 1, true);
}

/**
 * @brief Read a register value.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @param val - Register value.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_reg_read(struct oa_tc6_desc *desc, uint32_t addr, uint32_t *val)
{
	int ret;

	if (!desc)
		return -ENODEV;

	capi_mutex_lock(desc->ctrl_lock);
	ret = __oa_tc6_reg_read(desc, addr, val);
	capi_mutex_unlock(desc->ctrl_lock);

	return ret;
}

/**
 * @brief Write a register value.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @param val - Register value.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_reg_write(struct oa_tc6_desc *desc, uint32_t addr, uint32_t val)
{
	int ret;

	if (!desc)
		return -ENODEV;

	capi_mutex_lock(desc->ctrl_lock);
	ret = __oa_tc6_reg_write(desc, addr, val);
	capi_mutex_unlock(desc->ctrl_lock);

	return ret;
}

static int __oa_tc6_reg_update(struct oa_tc6_desc *desc, uint32_t addr,
			       uint32_t val, uint32_t mask)
{
	uint32_t reg_val;
	int ret;

	ret = __oa_tc6_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= val & mask;

	return __oa_tc6_reg_write(desc, addr, reg_val);
}

/**
 * @brief Update a field inside a register.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @param val - Field value.
 * @param mask - Bit mask corresponding to the register field.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_reg_update(struct oa_tc6_desc *desc, uint32_t addr, uint32_t val,
		      uint32_t mask)
{
	int ret;

	if (!desc)
		return -ENODEV;

	capi_mutex_lock(desc->ctrl_lock);
	ret = __oa_tc6_reg_update(desc, addr, val, mask);
	capi_mutex_unlock(desc->ctrl_lock);

	return ret;
}

/* Resolve a wire VS value to its TX queue, falling back to queue 0. */
static struct oa_tc6_queue *oa_tc6_txq_for_vs(struct oa_tc6_desc *desc,
					      uint8_t vs)
{
	int8_t idx = -1;

	if (vs < NO_OS_ARRAY_SIZE(desc->vs_to_txq))
		idx = desc->vs_to_txq[vs];

	return &desc->tx_q[idx < 0 ? 0 : idx];
}

/* Queue whose credit the footer of a chunk stamped vs will report. */
static int8_t oa_tc6_chunk_credit_q(struct oa_tc6_desc *desc, uint8_t vs)
{
	if (desc->tx_credit_reserve)
		return desc->tx_sv_q;

	return oa_tc6_txq_for_vs(desc, vs) - desc->tx_q;
}

/*
 * May queue qi clock its next chunk (sv: one that opens a frame)? Normally
 * any credit will do. With a reserve, a chunk must leave the reserve free,
 * except an SV to a queue other than tx_sv_q, which may use it. That SV must
 * not leave tx_sv_q below the reserve: no footer reports a queue again until
 * it gets an SV, and that SV needs the reserve.
 */
static bool oa_tc6_tx_credit_ok(struct oa_tc6_desc *desc, uint8_t qi, bool sv)
{
	uint8_t reserve = desc->tx_credit_reserve;
	int8_t cur = desc->tx_sv_q;

	if (!reserve || !sv || cur == qi)
		return desc->tx_q[qi].tx_credit > reserve;

	if (cur >= 0 && desc->tx_q[cur].tx_credit < reserve)
		return false;

	return desc->tx_q[qi].tx_credit >= reserve;
}

/*
 * Claim a caller-owned frame pool. The pools are typically static storage the
 * caller reuses across init cycles, so a slot can still carry the state a
 * previous instance left behind -- and only a FREE slot is ever allocated.
 */
static void oa_tc6_pool_reset(struct oa_tc6_frame_buffer *pool, uint32_t size)
{
	uint32_t i;

	for (i = 0; i < size; i++) {
		pool[i].state = OA_BUFF_FREE;
		pool[i].index = 0;
		pool[i].len = 0;
	}
}

/**
 * @brief Get a frame buffer that can be filled by user.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame to be transmitted.
 * @param prio - TX queue priority to allocate from (0 for single-queue).
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_tx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer **buffer, uint8_t prio)
{
	struct oa_tc6_frame_buffer *slot;
	struct oa_tc6_queue *q;

	if (!desc)
		return -EINVAL;

	q = oa_tc6_txq_for_vs(desc, prio);

	/*
	 * Serialise concurrent callers so two threads can't both see
	 * count < pool_size and advance head to the same slot. Leaf-level: no
	 * SPI, no ctrl_lock. No-op in bare-metal single-threaded builds.
	 */
	capi_mutex_lock(desc->data_lock);

	if (q->count >= q->pool_size) {
		capi_mutex_unlock(desc->data_lock);
		return -ENOBUFS;
	}

	slot = &q->pool[q->head];
	if (slot->state != OA_BUFF_FREE) {
		capi_mutex_unlock(desc->data_lock);
		return -ENOBUFS;
	}

	slot->state = OA_BUFF_TX_BUSY;
	slot->index = 0;
	slot->vs = q->vs;
	q->head = (q->head + 1) % q->pool_size;
	q->count++;

	capi_mutex_unlock(desc->data_lock);

	/*
	 * Zero the payload outside the lock -- the slot is now exclusively
	 * owned by this thread (state = TX_BUSY; poll only picks TX_READY).
	 */
	memset(slot->data, 0, CONFIG_OA_CHUNK_BUFFER_SIZE);

	*buffer = slot;

	return 0;
}

/**
 * @brief Mark a frame buffer as ready to be transmitted.
 *
 * On error the slot stays owned by the caller, who must fix ->len and put it
 * again: the ring is FIFO, so an abandoned slot blocks its queue.
 *
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame to be transmitted.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_put_tx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer *buffer)
{
	if (!desc || !buffer || !buffer->len)
		return -EINVAL;

	if (buffer->len > CONFIG_OA_CHUNK_BUFFER_SIZE)
		return -EMSGSIZE;

	/*
	 * Host-computed FCS: pad to the 60-byte Ethernet minimum unless
	 * tx_no_pad (the slot was zero-filled by oa_tc6_get_tx_frame, so the pad
	 * bytes are already 0), then append the CRC-32 little-endian. The MAC's
	 * own append was disabled at init (CONFIG2.HOST_CRC_APPEND).
	 */
	if (desc->sw_fcs) {
		uint32_t crc;

		/* The 4-byte FCS is appended in place; reject a frame that leaves
		 * no room for it rather than overrun buffer->data[]. */
		if (buffer->len > CONFIG_OA_CHUNK_BUFFER_SIZE - OA_TC6_FCS_LEN)
			return -EMSGSIZE;

		if (!desc->tx_no_pad && buffer->len < OA_TC6_ETH_MIN_LEN)
			buffer->len = OA_TC6_ETH_MIN_LEN;
		crc = oa_tc6_fcs_crc32(buffer->data, buffer->len);
		buffer->data[buffer->len++] = crc & 0xFF;
		buffer->data[buffer->len++] = (crc >> 8) & 0xFF;
		buffer->data[buffer->len++] = (crc >> 16) & 0xFF;
		buffer->data[buffer->len++] = (crc >> 24) & 0xFF;
	}

	/* ->index is the TX resume offset (bytes already sent); a freshly
	 * queued frame starts from the beginning. */
	buffer->index = 0;
	buffer->state = OA_BUFF_TX_READY;

	return 0;
}

/**
 * @brief Get the oldest ready TX frame in a queue's ring.
 *
 * The pool is a FIFO ring: the oldest frame is always at q->tail and stays
 * there (non-zero ->index = bytes already sent) until fully transmitted, so
 * a frame split across several SPI transactions (oa_tc6_tx_frame_to_chunks())
 * is always resumed first -- OA-TC6's ban on interleaving frames on the bus
 * is honoured for free.
 *
 * Head-of-line blocking: if the tail slot is still TX_BUSY (not yet
 * TX_READY), nothing is returned rather than skipping ahead and reordering.
 *
 * @param desc - the OA TC6 descriptor.
 * @param q - the queue to inspect.
 * @param buffer - buffer to be filled with the frame to be transmitted.
 * @return 0 in case of success, negative error code otherwise
 */
/*
 * Peek the frame `offset` slots past the queue's tail, without touching tail
 * or count -- lets a pass look ahead at not-yet-committed frames (see
 * oa_tc6_tx_commit()) the same way it would look at the real tail.
 */
static int oa_tc6_peek_tx_frame(struct oa_tc6_desc *desc,
				struct oa_tc6_queue *q, uint32_t offset,
				struct oa_tc6_frame_buffer **buffer)
{
	struct oa_tc6_frame_buffer *frame;
	uint32_t count;

	capi_mutex_lock(desc->data_lock);
	count = q->count;
	frame = &q->pool[(q->tail + offset) % q->pool_size];
	capi_mutex_unlock(desc->data_lock);

	if (offset >= count)
		return -ENOENT;

	if (frame->state != OA_BUFF_TX_READY)
		return -ENOENT;

	*buffer = frame;

	return 0;
}

static int oa_tc6_get_first_tx_frame(struct oa_tc6_desc *desc,
				     struct oa_tc6_queue *q,
				     struct oa_tc6_frame_buffer **buffer)
{
	return oa_tc6_peek_tx_frame(desc, q, 0, buffer);
}

/* How many chunks fit in one SPI transaction (header + payload per chunk). */
static uint32_t oa_tc6_spi_max_chunks(struct oa_tc6_desc *desc)
{
	return OA_SPI_BUFF_LEN / (desc->chunk_size + OA_HEADER_LEN);
}

/*
 * Index of the queue whose tail frame is open on the wire -- partially clocked
 * (->index != 0, SV sent, no EV yet). At most one frame is ever open, since an
 * incomplete frame ends the transaction and is closed before any other frame
 * opens. Returns -1 if none is open.
 */
static int oa_tc6_open_frame_qi(struct oa_tc6_desc *desc)
{
	struct oa_tc6_frame_buffer *frame;
	uint8_t i;

	for (i = 0; i < desc->tx_q_count; i++) {
		if (oa_tc6_get_first_tx_frame(desc, &desc->tx_q[i], &frame))
			continue;
		if (frame->index)
			return i;
	}

	return -1;
}

/**
 * @brief Check whether the RX pool has no room left for a fresh frame --
 * the exact condition under which oa_tc6_get_empty_rx_buff() would return
 * -ENOBUFS for a new buffer. Used to assert NORX on outgoing TX headers so
 * the MAC-PHY stops returning RX data (DV=0) before the pool is actually
 * exhausted.
 * @param desc - the OA TC6 descriptor.
 * @return true if the pool is full, false otherwise
 */
static bool oa_tc6_rx_pool_full(struct oa_tc6_desc *desc)
{
	struct oa_tc6_frame_buffer *slot;
	uint32_t next;
	bool full;

	capi_mutex_lock(desc->data_lock);

	slot = &desc->rx_pool[desc->rx_head];
	if (slot->state == OA_BUFF_FREE) {
		full = false;
	} else if (slot->state == OA_BUFF_RX_IN_PROGRESS &&
		   desc->rx_pool_size == 1) {
		/* The only slot holds an open frame: NORX would stall its rest. */
		full = false;
	} else {
		next = (desc->rx_head + 1) % desc->rx_pool_size;
		full = desc->rx_pool[next].state != OA_BUFF_FREE;
	}

	capi_mutex_unlock(desc->data_lock);

	return full;
}

/**
 * @brief Get an empty RX buffer.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer to be filled with the frame received.
 * @param new_buffer - if true, get a new buffer, otherwise get
 * the buffer that is currently being written.
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_get_empty_rx_buff(struct oa_tc6_desc *desc,
				    struct oa_tc6_frame_buffer **buffer,
				    bool new_buffer)
{
	struct oa_tc6_frame_buffer *slot;
	uint32_t next;
	int ret = 0;

	/*
	 * Guards rx_head against a concurrent put_rx_frame() (which reads
	 * rx_head while advancing rx_tail). Leaf-level: no SPI, no ctrl_lock.
	 * The caller fills the returned head slot outside this lock -- safe,
	 * since put_rx_frame()'s tail advance stops at rx_head.
	 */
	capi_mutex_lock(desc->data_lock);

	slot = &desc->rx_pool[desc->rx_head];

	/*
	 * The head slot is the one currently being written. When a frame spans
	 * several SPI transactions it stays at the head as OA_BUFF_RX_IN_PROGRESS;
	 * unless the caller wants a fresh buffer, keep writing into it.
	 */
	if (!new_buffer && slot->state == OA_BUFF_RX_IN_PROGRESS) {
		*buffer = slot;
		capi_mutex_unlock(desc->data_lock);

		return 0;
	}

	/*
	 * Need a fresh buffer. If the head slot is still FREE (nothing written
	 * yet -- the common start-of-transaction case), reuse it in place. Only
	 * once it holds a frame do we advance the ring, leaving the finished
	 * frame in [rx_tail, rx_head) for in-order delivery.
	 */
	if (slot->state != OA_BUFF_FREE) {
		next = (desc->rx_head + 1) % desc->rx_pool_size;
		slot = &desc->rx_pool[next];
		if (slot->state != OA_BUFF_FREE) {
			ret = -ENOBUFS; /* ring full -- oldest frames kept */
			goto out;
		}

		desc->rx_head = next;
	}

	slot->index = 0;
	slot->len = 0;
	*buffer = slot;

out:
	capi_mutex_unlock(desc->data_lock);

	return ret;
}

/**
 * @brief Get a frame buffer that is ready to be read by the user.
 * The VS field in the chunk footer fields should match what is provided
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame received.
 * @param vs - the value of the VS field to match.
 * @param mask - the mask to apply to the VS field.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_rx_frame_match_vs(struct oa_tc6_desc *desc,
				 struct oa_tc6_frame_buffer **buffer,
				 uint8_t vs, uint8_t mask)
{
	uint32_t i;
	int ret = -ENOENT;

	/*
	 * NOTE: matching by VS pulls a frame out of the middle of the ring, so
	 * it does NOT preserve receive order -- use oa_tc6_get_rx_frame() for
	 * ordinary in-order delivery. put_rx_frame() reclaims the hole this
	 * leaves without reordering what's left.
	 *
	 * Guards the scan/state-flip against the transfer function; leaf-level,
	 * no SPI.
	 */
	capi_mutex_lock(desc->data_lock);

	for (i = 0; i < desc->rx_pool_size; i++) {
		if (desc->rx_pool[i].state == OA_BUFF_RX_COMPLETE &&
		    (vs & mask) == (desc->rx_pool[i].vs & mask)) {
			*buffer = &desc->rx_pool[i];
			desc->rx_pool[i].state = OA_BUFF_RX_USER_OWNED;
			ret = 0;
			break;
		}
	}

	capi_mutex_unlock(desc->data_lock);

	return ret;
}

/**
 * @brief Get a frame buffer that is ready to be read by the user.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame received.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_rx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer **buffer)
{
	struct oa_tc6_frame_buffer *tail;

	/*
	 * Deliver strictly from the tail so frames leave in receive order --
	 * report nothing rather than skip ahead if the tail isn't complete yet
	 * (even if a younger slot is). A slot picked out of order by
	 * get_rx_frame_match_vs() (RX_USER_OWNED, not COMPLETE) likewise blocks
	 * the tail until returned.
	 *
	 * Guards rx_tail/state against the transfer function's rx_head advance
	 * (same lock); leaf-level, no SPI.
	 */
	capi_mutex_lock(desc->data_lock);

	tail = &desc->rx_pool[desc->rx_tail];
	if (tail->state != OA_BUFF_RX_COMPLETE) {
		capi_mutex_unlock(desc->data_lock);
		return -ENOENT;
	}

	tail->state = OA_BUFF_RX_USER_OWNED;

	capi_mutex_unlock(desc->data_lock);

	*buffer = tail;

	return 0;
}

/**
 * @brief Mark a frame buffer as used and ready to be rewritten.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame read by the user.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_put_rx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer *buffer)
{
	if (!desc || !buffer || buffer->state != OA_BUFF_RX_USER_OWNED)
		return -EINVAL;

	buffer->index = 0;
	buffer->len = 0;
	buffer->state = OA_BUFF_FREE;

	/*
	 * Reclaim ring space from the tail. In-order delivery frees the tail
	 * slot itself; out-of-order VS matching may free a slot ahead of the
	 * tail, leaving a FREE hole that is only reclaimed once the tail reaches
	 * it. Advance the tail over any contiguous freed slots, stopping at the
	 * head (the active write slot), so a hole never blocks reuse but the
	 * order of the frames still queued is preserved.
	 *
	 * Guard the rx_tail advance against the transfer function's rx_head
	 * reads/writes; leaf-level, no SPI.
	 */
	capi_mutex_lock(desc->data_lock);

	while (desc->rx_tail != desc->rx_head &&
	       desc->rx_pool[desc->rx_tail].state == OA_BUFF_FREE)
		desc->rx_tail = (desc->rx_tail + 1) % desc->rx_pool_size;

	capi_mutex_unlock(desc->data_lock);

	return 0;
}

/* Running state of one TX packing pass into the SPI buffer. */
struct oa_tc6_tx_pack {
	uint8_t *buf;		 /* SPI TX buffer being filled */
	uint32_t spi_index;	 /* bytes written so far */
	uint32_t chunks_written; /* chunks written so far */
	uint32_t max_chunks;	 /* chunks this pass may clock at most */
};

/*
 * Emit one DV=0 chunk: it carries no TX data (consumes no TX credit) and exists
 * only to clock the bus -- to drain RX, solicit credit, or surface a pending
 * error. `vs` names the queue whose credit the returned footer should refresh.
 */
static void oa_tc6_emit_empty_chunk(struct oa_tc6_desc *desc,
				    struct oa_tc6_tx_pack *pack, uint32_t norx,
				    uint8_t vs)
{
	uint32_t header;

	header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
	header |= no_os_field_prep(OA_DATA_HEADER_VS_MASK, vs);
	header |= norx;
	header |= oa_tc6_crc1(header);

	no_os_put_unaligned_be32(header, &pack->buf[pack->spi_index]);
	pack->spi_index += desc->chunk_size + OA_HEADER_LEN;
	desc->tx_chunk_credit_q[pack->chunks_written] =
		oa_tc6_chunk_credit_q(desc, vs);
	desc->tx_chunk_eof_q[pack->chunks_written] = -1;
	pack->chunks_written++;
}

/*
 * Fill any remaining bus room with DV=0 chunks so more RX chunks than we clocked
 * out as TX can still be received. Bounded by the physical SPI buffer and the RX
 * count only -- NOT by TX credit, which dummy chunks do not consume; gating this
 * on credit would stall RX whenever TX credit is low though the bus is ready.
 * Legal with a frame open: DV=0 carries no frame data (7.3.8.1, 7.7).
 */
static void oa_tc6_pack_rx_drain(struct oa_tc6_desc *desc,
				 struct oa_tc6_tx_pack *pack,
				 uint32_t rx_nchunks, uint32_t norx,
				 uint8_t force_vs)
{
	while (rx_nchunks > pack->chunks_written &&
	       pack->chunks_written < pack->max_chunks)
		oa_tc6_emit_empty_chunk(desc, pack, norx, force_vs);
}

/* First queue to service: the round-robin cursor, or under strict priority
 * the highest-index (highest-priority) queue. */
static uint8_t oa_tc6_tx_lead_queue(struct oa_tc6_desc *desc)
{
	if (desc->tx_sched == OA_TC6_TX_SCHED_STRICT_PRIO && desc->tx_q_count)
		return desc->tx_q_count - 1;

	return desc->tx_rr_cursor;
}

/* The k-th queue to service after start: upwards under round-robin,
 * downwards (towards lower priority) under strict priority. */
static uint8_t oa_tc6_tx_nth_queue(struct oa_tc6_desc *desc, uint8_t start,
				   uint8_t k)
{
	if (desc->tx_sched == OA_TC6_TX_SCHED_STRICT_PRIO)
		return (start + desc->tx_q_count - k) % desc->tx_q_count;

	return (start + k) % desc->tx_q_count;
}

/*
 * Which queue to service first this pass, advancing the round-robin cursor so
 * the next transaction leads with a different queue and the MAC's TX arbiter,
 * not driver order, decides egress.
 */
static uint8_t oa_tc6_tx_pick_start_queue(struct oa_tc6_desc *desc)
{
	uint8_t start = oa_tc6_tx_lead_queue(desc);

	if (desc->tx_sched == OA_TC6_TX_SCHED_ROUND_ROBIN && desc->tx_q_count)
		desc->tx_rr_cursor =
			(desc->tx_rr_cursor + 1) % desc->tx_q_count;

	return start;
}

/*
 * Plan for opening the next same-queue frame inside the leftover bytes of the
 * chunk that closes the current frame (SWO packing), instead of wasting them on
 * a zero-pad. ->frame is NULL when no packing applies.
 */
struct oa_tc6_swo_plan {
	struct oa_tc6_frame_buffer *frame; /* next frame to open, or NULL */
	uint32_t off_bytes; /* word-aligned start within the chunk */
	uint32_t avail;	    /* leading bytes of it copied into the chunk */
};

static void oa_tc6_plan_swo_pack(struct oa_tc6_desc *desc,
				 struct oa_tc6_queue *q, uint8_t qi,
				 uint32_t chunk_bytes, bool first_chunk,
				 struct oa_tc6_swo_plan *plan)
{
	struct oa_tc6_frame_buffer *next;
	uint32_t off, avail;

	plan->frame = NULL;
	plan->off_bytes = 0;
	plan->avail = 0;

	/*
	 * SV is one bit: a chunk that both opens and closes this frame
	 * (first_chunk) has already spent it, so it cannot also open the next.
	 */
	if (desc->zero_swo || first_chunk)
		return;

	off = (chunk_bytes + 3u) & ~3u;
	avail = desc->chunk_size - off;
	if (avail < 4)
		return;

	if (oa_tc6_peek_tx_frame(desc, q, desc->tx_pending_done[qi] + 1, &next))
		return;

	/*
	 * One header closes at most one frame (a single EV/EBO field), so the
	 * packed frame must NOT also fit entirely in the leftover room.
	 */
	if (next->len <= avail)
		return;

	plan->frame = next;
	plan->off_bytes = off;
	plan->avail = avail;
}

/* Base data-chunk header for one chunk of an outgoing frame (no SWO/CRC). */
static uint32_t oa_tc6_frame_chunk_header(struct oa_tc6_frame_buffer *frame,
					  bool first_chunk, bool last_chunk,
					  uint32_t chunk_bytes, uint32_t norx)
{
	uint32_t header;

	header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
	header |= no_os_field_prep(OA_DATA_HEADER_DV_MASK, 1);
	header |= no_os_field_prep(OA_DATA_HEADER_VS_MASK, frame->vs);
	header |= norx;

	if (first_chunk) {
		header |= no_os_field_prep(OA_DATA_HEADER_SV_MASK, 1);
		header |= no_os_field_prep(OA_DATA_HEADER_TSC_MASK, frame->tsc);
	}

	if (last_chunk) {
		header |= no_os_field_prep(OA_DATA_HEADER_EV_MASK, 1);
		header |= no_os_field_prep(OA_DATA_HEADER_EBO_MASK,
					   chunk_bytes - 1);
	}

	return header;
}

enum oa_tc6_pack_result {
	OA_PACK_COMPLETE,   /* the whole frame was clocked (SV..EV) */
	OA_PACK_INCOMPLETE, /* ran out of credit or SPI room before EV */
};

/*
 * Clock as many of one frame's chunks as TX credit and SPI-buffer room allow,
 * starting at *frame_offset (the resume point) and advancing it. SV marks the
 * first chunk, EV the last; a frame that runs out of either is left INCOMPLETE
 * and resumed next pass. Unless swo_frame is NULL, a chunk that closes the
 * frame with spare room may also open the next same-queue frame (SWO packing),
 * handed back via *swo_frame and *swo_offset for the caller's next iteration.
 */
static enum oa_tc6_pack_result
oa_tc6_pack_frame(struct oa_tc6_desc *desc, struct oa_tc6_tx_pack *pack,
		  struct oa_tc6_queue *q, uint8_t qi,
		  struct oa_tc6_frame_buffer *frame, uint32_t *frame_offset,
		  uint32_t norx, struct oa_tc6_frame_buffer **swo_frame,
		  uint32_t *swo_offset)
{
	while (*frame_offset < frame->len &&
	       pack->chunks_written < pack->max_chunks &&
	       oa_tc6_tx_credit_ok(desc, qi, *frame_offset == 0)) {
		struct oa_tc6_swo_plan swo;
		uint32_t chunk_bytes =
			no_os_min(desc->chunk_size, frame->len - *frame_offset);
		bool first_chunk = (*frame_offset == 0);
		bool last_chunk = (*frame_offset + chunk_bytes == frame->len);
		uint32_t header;
		uint8_t *dst;

		header = oa_tc6_frame_chunk_header(
			frame, first_chunk, last_chunk, chunk_bytes, norx);

		swo.frame = NULL;
		if (last_chunk && swo_frame) {
			oa_tc6_plan_swo_pack(desc, q, qi, chunk_bytes,
					     first_chunk, &swo);
			if (swo.frame) {
				header |= no_os_field_prep(
					OA_DATA_HEADER_SV_MASK, 1);
				header |= no_os_field_prep(
					OA_DATA_HEADER_SWO_MASK,
					swo.off_bytes / 4);
				header |= no_os_field_prep(
					OA_DATA_HEADER_TSC_MASK,
					swo.frame->tsc);
			}
		}

		header |= oa_tc6_crc1(header);
		no_os_put_unaligned_be32(header, &pack->buf[pack->spi_index]);
		pack->spi_index += OA_HEADER_LEN;

		dst = &pack->buf[pack->spi_index];
		memcpy(dst, &frame->data[*frame_offset], chunk_bytes);
		if (swo.frame) {
			/* Zero the word-align gap, then open the next frame in
			 * the leftover bytes. */
			if (swo.off_bytes > chunk_bytes)
				memset(dst + chunk_bytes, 0,
				       swo.off_bytes - chunk_bytes);
			memcpy(dst + swo.off_bytes, swo.frame->data, swo.avail);
			*swo_frame = swo.frame;
			*swo_offset = swo.avail;
		} else if (chunk_bytes < desc->chunk_size) {
			/* A chunk is always chunk_size on the wire; zero-pad a
			 * short end chunk (EBO marks the valid length) so no
			 * stale SPI-buffer bytes are clocked out. */
			memset(dst + chunk_bytes, 0,
			       desc->chunk_size - chunk_bytes);
		}
		pack->spi_index += desc->chunk_size;

		if (first_chunk)
			desc->tx_sv_q = qi;
		desc->tx_chunk_credit_q[pack->chunks_written] =
			oa_tc6_chunk_credit_q(desc, frame->vs);
		desc->tx_chunk_eof_q[pack->chunks_written] =
			last_chunk ? (int8_t)qi : -1;
		pack->chunks_written++;
		q->tx_credit--;
		*frame_offset += chunk_bytes;
	}

	return (*frame_offset < frame->len) ? OA_PACK_INCOMPLETE
					    : OA_PACK_COMPLETE;
}

/*
 * Pack as many of queue qi's ready frames into the transaction as credit and
 * SPI-buffer room allow (only its first when one_frame), counting each into
 * tx_pending_done[qi] (applied to the real ring only once the transceive
 * succeeds -- see oa_tc6_tx_commit()).
 *
 * Returns true if a frame was left OPEN on the wire (SV clocked, no EV): no
 * other queue may be serviced until it closes, since a second SV before its EV
 * is a Repeated Frame Start (TXPE) and the MAC would drop both frames. A DV=0
 * chunk is not affected -- the MAC-PHY ignores its payload and never runs it
 * through the protocol check -- so RX can still be drained and the open
 * queue's credit solicited mid-frame.
 */
static bool oa_tc6_pack_queue(struct oa_tc6_desc *desc,
			      struct oa_tc6_tx_pack *pack, uint8_t qi,
			      uint32_t norx, bool one_frame)
{
	struct oa_tc6_queue *q = &desc->tx_q[qi];
	struct oa_tc6_frame_buffer *swo_frame = NULL;
	uint32_t swo_offset = 0;

	for (;;) {
		struct oa_tc6_frame_buffer *frame;
		uint32_t frame_offset;
		enum oa_tc6_pack_result res;

		if (oa_tc6_peek_tx_frame(desc, q, desc->tx_pending_done[qi],
					 &frame))
			return false;

		/*
		 * Resume point. ->index is the real ring state, untouched until
		 * commit; a frame this same pass already opened via SWO packing
		 * has ->index still 0, so its true offset is carried separately.
		 */
		frame_offset = frame->index;
		if (frame == swo_frame) {
			frame_offset = swo_offset;
			swo_frame = NULL;
		}

		res = oa_tc6_pack_frame(desc, pack, q, qi, frame, &frame_offset,
					norx, one_frame ? NULL : &swo_frame,
					&swo_offset);

		if (res == OA_PACK_INCOMPLETE) {
			/*
			 * Record the resume offset -- applied to the real frame
			 * only when oa_tc6_tx_commit() confirms this pass
			 * succeeded, so a failed transfer resends from the true
			 * state. If any chunk was clocked the frame is now open
			 * (end the transaction); if none was (fresh frame, zero
			 * credit) leave it queued so the DV=0 paths can refresh
			 * credit from the returned footer.
			 */
			desc->tx_pending_incomplete_frame = frame;
			desc->tx_pending_incomplete_index = frame_offset;
			return frame_offset > 0;
		}

		/* Fully packed: count it and look past it for more, still from
		 * the real (unmoved) tail. */
		desc->tx_pending_done[qi]++;
		if (one_frame)
			return false;
	}
}

/**
 * @brief Convert queued OA_BUFF_TX_READY frames to chunks; append DV=0
 * dummy chunks if more RX is available than TX.
 *
 * Queues are serviced from the tx_sched start queue (strict priority by default,
 * the highest-index queue first under strict priority), each up to its own
 * footer-tracked credit -- decremented locally per chunk here, reconciled from
 * the footer once the transaction completes.
 *
 * @param desc - the OA TC6 descriptor
 * @param tx_buffer - the buffer containing the chunks
 * @param rx_nchunks - the number of chunks available for reception
 * @param force_empty - if nothing else would be clocked, emit one empty
 *                      (DV=0) chunk anyway (surfaces a pending error)
 * @param solicit - lead the transaction with one empty (DV=0) chunk to solicit
 *                  force_vs's queue credit, whatever else it carries
 * @param force_vs - VS to stamp on the empty chunks
 * @param tx_written - the number of chunks written in the buffer
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_tx_frame_to_chunks(struct oa_tc6_desc *desc,
				     uint8_t *tx_buffer, uint32_t rx_nchunks,
				     bool force_empty, bool solicit,
				     uint8_t force_vs, uint32_t *tx_written)
{
	struct oa_tc6_tx_pack pack = {
		.buf = tx_buffer,
		.max_chunks = oa_tc6_spi_max_chunks(desc),
	};
	uint8_t drain_vs = force_vs;
	int open_qi = -1;
	uint32_t norx;
	uint8_t start;
	uint8_t qi;
	uint8_t k;

	/*
	 * Pool occupancy cannot change between here and this pass's footer
	 * processing (no allocation happens in between), so one check covers
	 * every chunk header built below. NORX tells the MAC-PHY not to return
	 * RX data this pass (footer DV=0); RCA/TXC stay valid regardless, so
	 * credit bookkeeping is unaffected.
	 */
	norx = no_os_field_prep(OA_DATA_HEADER_NORX_MASK,
				oa_tc6_rx_pool_full(desc));

	/*
	 * Start this pass with nothing pending -- whatever the previous pass
	 * left here has already been committed (oa_tc6_tx_commit(), on success)
	 * or abandoned (on failure, where the real ring state was never
	 * touched). Either way it must not carry into this pass's count.
	 */
	memset(desc->tx_pending_done, 0, sizeof(desc->tx_pending_done));
	desc->tx_pending_incomplete_frame = NULL;
	desc->tx_pass_sv_q = desc->tx_sv_q;

	/*
	 * Solicit first, before the queues that do have credit claim the bus
	 * room: a starved queue's credit is reported only in the footer of a
	 * chunk stamped with its own VS, so as long as a sending queue fills
	 * every chunk of every pass with its own VS the starved one never learns
	 * it may send. This DV=0 chunk consumes no credit, and is legal even
	 * between an open frame's SV and EV: the MAC-PHY ignores a DV=0 payload
	 * and never runs the chunk through its transmit protocol check.
	 */
	if (solicit)
		oa_tc6_emit_empty_chunk(desc, &pack, norx, force_vs);

	/*
	 * Close the frame the last pass left open before any other frame opens,
	 * then service queues from the round-robin cursor; stop as soon as one
	 * leaves a frame open on the wire. Resuming only that one frame keeps a
	 * backlogged queue, which ends most passes mid-frame, from holding the
	 * lead. No frame data while the MAC-PHY is unconfigured: it would
	 * ignore it (7.6).
	 */
	start = oa_tc6_tx_pick_start_queue(desc);
	if (!desc->sync_lost) {
		open_qi = oa_tc6_open_frame_qi(desc);
		if (open_qi >= 0 &&
		    !oa_tc6_pack_queue(desc, &pack, open_qi, norx, true))
			open_qi = -1;
	}
	for (k = 0; k < desc->tx_q_count && !desc->sync_lost && open_qi < 0;
	     k++) {
		qi = oa_tc6_tx_nth_queue(desc, start, k);
		if (oa_tc6_pack_queue(desc, &pack, qi, norx, false))
			open_qi = qi;
	}
	if (open_qi >= 0)
		drain_vs = desc->tx_q[open_qi].vs;

	/*
	 * Backfill DV=0 chunks to drain RX, stamped with the open frame's queue
	 * (else force_vs) so their footers refresh the credit it is waiting on.
	 * If nothing at all was clocked but the caller forced a pass, emit a
	 * single empty chunk so the MAC still returns a footer (surfaces EXST,
	 * or solicits credit for a starved queue).
	 */
	oa_tc6_pack_rx_drain(desc, &pack, rx_nchunks, norx, drain_vs);
	if (force_empty && pack.chunks_written == 0)
		oa_tc6_emit_empty_chunk(desc, &pack, norx, force_vs);

	desc->tx_pass_chunks = pack.chunks_written;
	*tx_written = pack.spi_index;

	return 0;
}

/*
 * Apply the TX progress of the pass just clocked, up to the first of its
 * `accepted` chunks the MAC-PHY ignored: 0 for a transfer that failed or was
 * aborted, fewer than the pass after a header parity error (7.5.1) or a reset
 * (7.6). A frame whose EV chunk was accepted is released -- the MAC-PHY
 * processes completed frames even when a later header is bad. The frame being
 * received at the cut is dropped by the MAC-PHY, which then ignores MOSI until
 * SV=1, so it restarts at offset 0 rather than resuming mid-frame.
 */
static void oa_tc6_tx_commit(struct oa_tc6_desc *desc, uint32_t accepted)
{
	uint32_t done[NO_OS_ARRAY_SIZE(desc->tx_pending_done)] = { 0 };
	struct oa_tc6_frame_buffer *f;
	struct oa_tc6_queue *q;
	bool sent = false;
	uint32_t j;
	int qi;

	accepted = no_os_min(accepted, desc->tx_pass_chunks);
	for (j = 0; j < accepted; j++)
		if (desc->tx_chunk_eof_q[j] >= 0)
			done[desc->tx_chunk_eof_q[j]]++;

	for (qi = 0; qi < desc->tx_q_count; qi++) {
		if (!done[qi])
			continue;

		q = &desc->tx_q[qi];
		sent = true;
		capi_mutex_lock(desc->data_lock);
		for (j = 0; j < done[qi]; j++) {
			f = &q->pool[(q->tail + j) % q->pool_size];
			desc->stats.tx_frames++;
			desc->stats.tx_bytes += f->len;
			f->len = 0;
			f->index = 0;
			f->state = OA_BUFF_FREE;
		}
		q->tail = (q->tail + done[qi]) % q->pool_size;
		q->count -= done[qi];
		capi_mutex_unlock(desc->data_lock);
	}

	if (accepted == desc->tx_pass_chunks) {
		if (desc->tx_pending_incomplete_frame)
			desc->tx_pending_incomplete_frame->index =
				desc->tx_pending_incomplete_index;
	} else {
		qi = oa_tc6_open_frame_qi(desc);
		if (qi >= 0 &&
		    !oa_tc6_get_first_tx_frame(desc, &desc->tx_q[qi], &f))
			f->index = 0;
		/* The MAC-PHY saw only the accepted chunks' SVs. */
		if (desc->tx_credit_reserve)
			desc->tx_sv_q =
				accepted ? desc->tx_chunk_credit_q[accepted - 1]
					 : desc->tx_pass_sv_q;
	}

	memset(desc->tx_pending_done, 0, sizeof(desc->tx_pending_done));
	desc->tx_pending_incomplete_frame = NULL;
	desc->tx_pass_chunks = 0;

	if (sent && desc->event_cb)
		desc->event_cb(OA_TC6_EVENT_TX, NULL, desc->event_cb_arg);
}

/* Map each status-register error bit to the event reported to the user. */
struct oa_tc6_status_error {
	uint32_t mask;
	enum oa_tc6_event event;
};

static const struct oa_tc6_status_error oa_tc6_status0_errors[] = {
	{ OA_TC6_STATUS0_TXPE, OA_TC6_EVENT_ERR_TX_PROTOCOL },
	{ OA_TC6_STATUS0_TXBOE, OA_TC6_EVENT_ERR_TX_OVERFLOW },
	{ OA_TC6_STATUS0_TXBUE, OA_TC6_EVENT_ERR_TX_UNDERFLOW },
	{ OA_TC6_STATUS0_RXBOE, OA_TC6_EVENT_ERR_RX_OVERFLOW },
	{ OA_TC6_STATUS0_LOFE, OA_TC6_EVENT_ERR_LOSS_OF_FRAME },
	{ OA_TC6_STATUS0_HDRE, OA_TC6_EVENT_ERR_HEADER },
	{ OA_TC6_STATUS0_TXFCSE, OA_TC6_EVENT_ERR_TX_FCS },
	{ OA_TC6_STATUS0_CDPE, OA_TC6_EVENT_ERR_CTRL_DATA_PROT },
	{ OA_TC6_STATUS0_RESETC, OA_TC6_EVENT_RESET },
};

static const struct oa_tc6_status_error oa_tc6_status1_errors[] = {
	{ OA_TC6_STATUS1_RX_IFG_ERR, OA_TC6_EVENT_ERR_RX_IFG },
	{ OA_TC6_STATUS1_MAX_RR_ERR, OA_TC6_EVENT_ERR_TX_MAX_RETRY },
	{ OA_TC6_STATUS1_SPI_ERR, OA_TC6_EVENT_ERR_SPI },
	{ OA_TC6_STATUS1_RX_ECC_ERR, OA_TC6_EVENT_ERR_RX_ECC },
	{ OA_TC6_STATUS1_TX_ECC_ERR, OA_TC6_EVENT_ERR_TX_ECC },
	{ OA_TC6_STATUS1_RD_ACC_ERR, OA_TC6_EVENT_ERR_READ_ACCESS },
};

/* Report each set error bit in status against its map; returns true if any
 * fired. */
static bool oa_tc6_report_status(struct oa_tc6_desc *desc, uint32_t status,
				 const struct oa_tc6_status_error *map,
				 uint32_t count, void *detail)
{
	bool reported = false;
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (status & map[i].mask) {
			desc->event_cb(map[i].event, detail,
				       desc->event_cb_arg);
			reported = true;
		}
	}

	return reported;
}

/*
 * Handle an error signalled in a data-chunk footer (EXST/HDRB). Read STATUS0
 * and STATUS1, clear only the bits that were set by writing the read value back
 * (write-1-to-clear), then report each set error bit to the user callback as its
 * own decoded event. A STATUS0 bit that maps to no known event is reported as
 * the generic OA_TC6_EVENT_ERROR; STATUS1's non-fault bits (link/RX-ready/VS)
 * never raise it, and neither do STATUS0's TTSCA{A,B,C} capture flags.
 * CONFIG0.SYNC is deliberately NOT re-asserted here: on a real config loss the
 * host must reconfigure the MAC-PHY before setting SYNC (7.6), which is
 * signalled separately as OA_TC6_EVENT_RESET.
 */
static int oa_tc6_handle_flags(struct oa_tc6_desc *desc)
{
	uint32_t status0 = 0;
	uint32_t status1 = 0;
	bool reported = false;
	int ret;

	ret = oa_tc6_reg_read(desc, OA_TC6_STATUS0_REG, &status0);
	if (ret)
		status0 = 0;
	ret = oa_tc6_reg_read(desc, OA_TC6_STATUS1_REG, &status1);
	if (ret)
		status1 = 0;

	/*
	 * Write-1-to-clear: only the bits that were set are cleared, minus the
	 * egress-timestamp-capture-available bits. Those are application state,
	 * not errors: clearing them here would discard a capture the application
	 * has not read yet and make its poll time
	 * out. Unknown bits are still cleared, so a latched one cannot wedge EXST.
	 */
	status0 &= ~OA_TC6_STATUS0_TTSCA_MASK;

	if (status0)
		oa_tc6_reg_write(desc, OA_TC6_STATUS0_REG, status0);
	if (status1)
		oa_tc6_reg_write(desc, OA_TC6_STATUS1_REG, status1);

	if (desc->event_cb) {
		struct oa_tc6_error_detail detail = {
			.status0 = status0,
			.status1 = status1,
		};

		reported = oa_tc6_report_status(
			desc, status0, oa_tc6_status0_errors,
			NO_OS_ARRAY_SIZE(oa_tc6_status0_errors), &detail);
		reported |= oa_tc6_report_status(
			desc, status1, oa_tc6_status1_errors,
			NO_OS_ARRAY_SIZE(oa_tc6_status1_errors), &detail);

		/* STATUS0 had a bit set that maps to no known error event --
		 * report it generically. A clean STATUS0 (status0 == 0) means EXST
		 * was already cleared or reflected a non-fault STATUS1 bit alone;
		 * nothing happened on the data path, so stay silent rather than
		 * misreport an error. */
		if (!reported && status0)
			desc->event_cb(OA_TC6_EVENT_ERROR, &detail,
				       desc->event_cb_arg);
	}

	return 0;
}

/*
 * Service a STATUS0 read/clear owed by a prior data pass (EXST/HDRB). Caller
 * must hold the bus with no async DMA in flight, so the ctrl read inside cannot
 * spin in oa_tc6_ctrl_transceive (see the sync thread and the async IDLE
 * branch).
 */
static void oa_tc6_service_status0(struct oa_tc6_desc *desc)
{
	if (!desc->status0_pending)
		return;

	oa_tc6_handle_flags(desc);
	desc->status0_pending = false;
}

void oa_tc6_register_callback(struct oa_tc6_desc *desc,
			      void (*cb)(enum oa_tc6_event, void *, void *),
			      void *arg)
{
	desc->event_cb = cb;
	desc->event_cb_arg = arg;
}

/* True if the queue has a ready frame waiting to be transmitted. */
static bool oa_tc6_queue_ready(struct oa_tc6_desc *desc,
			       struct oa_tc6_queue *queue)
{
	struct oa_tc6_frame_buffer *frame;

	return oa_tc6_get_first_tx_frame(desc, queue, &frame) == 0;
}

/*
 * True if the queue has a ready frame that can make forward progress now: one
 * chunk of credit is enough, since a frame is clocked as far as credit allows
 * and resumed once the footer reports more. A DV=0 chunk mid-frame is harmless
 * (7.3.8.1 lists no such TXPE cause).
 */
static bool oa_tc6_queue_sendable(struct oa_tc6_desc *desc,
				  struct oa_tc6_queue *queue)
{
	struct oa_tc6_frame_buffer *frame;

	if (desc->sync_lost || oa_tc6_get_first_tx_frame(desc, queue, &frame))
		return false;

	return oa_tc6_tx_credit_ok(desc, queue - desc->tx_q, !frame->index);
}

/* Abandon the frame in progress and resync on the next SV=1 chunk. The active
 * slot is not user-owned, so resetting its offsets cannot lose a delivered
 * frame. */
static void oa_tc6_rx_drop_in_progress(struct oa_tc6_desc *desc,
				       struct oa_tc6_frame_buffer *frame_buffer)
{
	desc->skip_current_frame = true;
	frame_buffer->index = 0;
	frame_buffer->len = 0;
	frame_buffer->state = OA_BUFF_FREE;
}

/*
 * A transaction whose RX chunks were never parsed (failed, errored or aborted)
 * may have carried part of the frame in progress: drop it rather than splice
 * the rest onto the gap. Its footers were lost too, so re-probe: nINT may still
 * be asserted, with no new edge to come.
 */
static void oa_tc6_rx_xfer_lost(struct oa_tc6_desc *desc)
{
	struct oa_tc6_frame_buffer *head;

	capi_mutex_lock(desc->data_lock);
	head = &desc->rx_pool[desc->rx_head];
	if (head->state == OA_BUFF_RX_IN_PROGRESS)
		oa_tc6_rx_drop_in_progress(desc, head);
	capi_mutex_unlock(desc->data_lock);

	oa_tc6_rx_drop_in_progress(desc, &desc->rx_sink);
	desc->irq_pending = true;
}

/**
 * @brief Copy one contiguous slice of a data chunk into the active RX
 * frame buffer, applying start/end-of-frame side effects as needed.
 *
 * @param desc - the OA TC6 descriptor
 * @param fb - in/out: active RX frame buffer; on closes, updated to the
 *             fresh buffer fetched for what comes next
 * @param chunk - the chunk's data (chunk-relative offsets, not the footer)
 * @param footer - this chunk's footer word
 * @param start - first byte offset copied from chunk (inclusive)
 * @param end - last byte offset copied from chunk (inclusive)
 * @param opens - this slice is the start of a frame (SV): latch
 *                rtsa/rtsp/vs and mark the buffer IN_PROGRESS
 * @param closes - this slice is the end of a frame (EV): latch FD from the
 *                 footer, notify on a drop, mark the buffer COMPLETE and fetch
 *                 a fresh one for what follows.
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_rx_copy_slice(struct oa_tc6_desc *desc,
				struct oa_tc6_frame_buffer **fb,
				const uint8_t *chunk, uint32_t footer,
				uint32_t start, uint32_t end, bool opens,
				bool closes)
{
	struct oa_tc6_frame_buffer *buf = *fb;
	uint32_t off, crc = 0, rx_fcs = 0;
	bool runt;
	int ret;

	/* A frame start (SV=1) resets the write cursor: bytes already in the slot
	 * are a prior frame that never closed (EV lost to error/resync) and must
	 * be discarded, not prepended to this one. */
	if (opens) {
		buf->index = 0;
		buf->len = 0;
	}

	/* The rest of a frame whose start was dropped or never seen. */
	if (!opens && buf->state != OA_BUFF_RX_IN_PROGRESS)
		return 0;

	/* A well-formed frame fits the slot, but a stream that loses its EV (or a
	 * corrupted SWO/EBO) would keep appending past the end. Drop the whole
	 * oversized frame rather than overrun buf->data[] -- the next SV
	 * restarts accumulation. */
	if (buf->index + (end - start + 1) > CONFIG_OA_CHUNK_BUFFER_SIZE) {
		oa_tc6_rx_drop_in_progress(desc, buf);
		return 0;
	}

	memcpy(&buf->data[buf->index], &chunk[start], end - start + 1);
	buf->index += end - start + 1;
	buf->len = buf->index;

	if (opens) {
		buf->rtsa = !!(footer & OA_DATA_FOOTER_RTSA_MASK);
		buf->rtsp = !!(footer & OA_DATA_FOOTER_RTSP_MASK);
		buf->vs = no_os_field_get(OA_DATA_FOOTER_VS_MASK, footer);
		buf->state = OA_BUFF_RX_IN_PROGRESS;
	}

	if (!closes)
		return 0;

	/* FD flags the closing frame as bad: notify the app but still forward it
	 * letting the drop flag decide. */
	buf->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
	if (buf->frame_drop) {
		desc->stats.rx_drop_fd++;
		if (desc->event_cb)
			desc->event_cb(OA_TC6_EVENT_ERR_RX_FRAME_DROP, NULL,
				       desc->event_cb_arg);
	}

	/*
	 * The MAC-PHY never strips the FCS, so it is dropped from the delivered
	 * length in both modes. The minimum length is the MAC's to enforce
	 * (MAC_RX_MIN_LEN); here a frame only has to hold its FCS. A host FCS is
	 * checked too (little-endian on the wire). A runt or a mismatch frees
	 * the slot and hands back the same buffer so the bad frame is never
	 * delivered. A receive timestamp (RTSA) sits ahead of the MAC
	 * header and is not covered by the FCS.
	 */
	off = buf->rtsa ? desc->rx_ts_len : 0;
	runt = buf->len < off + OA_TC6_FCS_LEN;
	if (desc->sw_fcs && !runt) {
		crc = oa_tc6_fcs_crc32(&buf->data[off],
				       buf->len - off - OA_TC6_FCS_LEN);
		rx_fcs = buf->data[buf->len - 4] |
			 (buf->data[buf->len - 3] << 8) |
			 (buf->data[buf->len - 2] << 16) |
			 ((uint32_t)buf->data[buf->len - 1] << 24);
	}

	if (runt || crc != rx_fcs) {
		buf->state = OA_BUFF_FREE;
		buf->index = 0;
		buf->len = 0;
		ret = oa_tc6_get_empty_rx_buff(desc, fb, true);
		if (ret == -ENOBUFS) {
			*fb = &desc->rx_sink;
			return 0;
		}
		return ret;
	}

	buf->len -= OA_TC6_FCS_LEN;

	buf->state = OA_BUFF_RX_COMPLETE;
	if (buf != &desc->rx_sink) {
		desc->stats.rx_frames++;
		desc->stats.rx_bytes += buf->len;
		if (desc->event_cb)
			desc->event_cb(OA_TC6_EVENT_RX, NULL, desc->event_cb_arg);
	} else {
		desc->stats.rx_drop_nobuf++;
	}

	ret = oa_tc6_get_empty_rx_buff(desc, fb, true);
	if (ret == -ENOBUFS) {
		*fb = &desc->rx_sink;
		return 0;
	}

	return ret;
}

/* SYNC=0 in a footer means the MAC-PHY reset and lost its configuration (7.6).
 * Tell the app to reconfigure, once per loss. CONFIG0.SYNC is deliberately NOT
 * re-asserted here: the host must reconfigure first, then set it. */
static void oa_tc6_rx_note_sync_loss(struct oa_tc6_desc *desc)
{
	if (desc->sync_lost)
		return;

	desc->sync_lost = true;
	desc->tx_sv_q = -1;
	if (desc->event_cb)
		desc->event_cb(OA_TC6_EVENT_RESET, NULL, desc->event_cb_arg);
}

/* Pass-scoped state threaded through one RX transaction's footers. */
struct oa_tc6_rx_pass {
	uint32_t last_good_footer; /* newest parity-valid footer (for RCA) */
	bool have_good_footer;
	bool status_pending;  /* a STATUS0 read is owed (EXST/HDRB) */
	uint32_t tx_accepted; /* leading chunks whose TX the MAC-PHY took */
};

/*
 * Inspect one chunk's footer and apply every non-payload side effect: parity,
 * per-queue credit reconciliation, error draining, SYNC loss, EXST/HDRB. All
 * footer flags are decoded here so the caller sees a clean decision.
 *
 * Returns true if the chunk carries deliverable payload (parity OK, SYNC set,
 * DV=1, not HDRB, not mid-drain) and the caller should place its bytes; false
 * to skip the chunk.
 */
static bool oa_tc6_rx_footer_usable(struct oa_tc6_desc *desc, uint32_t footer,
				    uint32_t chunk_idx,
				    struct oa_tc6_frame_buffer *frame_buffer,
				    struct oa_tc6_rx_pass *pass)
{
	bool sv, dv;
	int8_t qi;

	/*
	 * Parity guards every other footer field, so validate it first:
	 * oa_tc6_crc1() is non-zero on a bad word. Drop the frame in progress
	 * and resync, without trusting this footer's credit or RCA.
	 */
	if (oa_tc6_crc1(footer)) {
		desc->stats.rx_footer_parity_err++;
		if (desc->event_cb)
			desc->event_cb(OA_TC6_EVENT_ERR_RX_PARITY, NULL,
				       desc->event_cb_arg);
		oa_tc6_rx_drop_in_progress(desc, frame_buffer);
		return false;
	}

	/*
	 * HDRB: from here on MISO carries the 0xC0000001 constant, not a footer
	 * (7.5.1), so none of its other fields -- SYNC, TXC, RCA -- are real.
	 * The MAC-PHY ignored this chunk's header and every one after it, and
	 * terminated the RX frame in progress.
	 */
	if (footer & OA_DATA_FOOTER_HDRB_MASK) {
		desc->xfer_flags.flags_valid = true;
		desc->xfer_flags.hdrb = true;
		pass->status_pending = true;
		pass->tx_accepted = no_os_min(pass->tx_accepted, chunk_idx);
		oa_tc6_rx_drop_in_progress(desc, frame_buffer);
		return false;
	}

	pass->last_good_footer = footer;
	pass->have_good_footer = true;

	sv = !!(footer & OA_DATA_FOOTER_SV_MASK);
	dv = !!(footer & OA_DATA_FOOTER_DV_MASK);

	/*
	 * TXC is the credit for the queue recorded in tx_chunk_credit_q, valid
	 * after this transaction -- NOT the footer's VS, which reports the
	 * received frame's source with a different mapping. A chunk whose SV
	 * switches tx_sv_q already reports the new queue.
	 */
	qi = desc->tx_chunk_credit_q[chunk_idx];
	if (qi >= 0)
		desc->tx_q[qi].tx_credit =
			no_os_field_get(OA_DATA_FOOTER_TXC_MASK, footer);

	/* Latch the transfer flags, even on DV=0. */
	desc->xfer_flags.flags_valid = true;
	desc->xfer_flags.sync = !!(footer & OA_DATA_FOOTER_SYNC_MASK);

	/* An unconfigured MAC-PHY ignores TX data chunks (7.6). */
	if (!desc->xfer_flags.sync) {
		pass->tx_accepted = no_os_min(pass->tx_accepted, chunk_idx);
		oa_tc6_rx_note_sync_loss(desc);
		oa_tc6_rx_drop_in_progress(desc, frame_buffer);
		return false;
	}
	desc->sync_lost = false;

	/*
	 * EXST means "a STATUS0 read is owed", NOT "this frame is invalid" --
	 * deliver it anyway. The read/clear is deferred to a single call after
	 * all of this transaction's chunks are processed (servicing it inline
	 * per chunk would starve the data path under sustained LOFE).
	 */
	if (footer & OA_DATA_FOOTER_EXST_MASK) {
		desc->stats.exst_events++;
		pass->status_pending = true;
	}

	/* Draining the rest of an already-dropped frame: discard until SV=1. */
	if (desc->skip_current_frame && dv && !sv)
		return false;
	desc->skip_current_frame = false;

	return dv;
}

/*
 * Place a deliverable chunk's payload into the RX frame(s), driven by its SV/EV
 * boundaries (SWO/EBO give the byte offsets). A chunk may start a frame, end
 * one, do both (optionally spanning two frames when SWO wraps past EBO), or
 * carry only the middle of a frame already in progress.
 */
static int oa_tc6_rx_place_data(struct oa_tc6_desc *desc,
				struct oa_tc6_frame_buffer **frame_buffer,
				const uint8_t *chunk, uint32_t footer)
{
	uint32_t sbo = no_os_field_get(OA_DATA_FOOTER_SWO_MASK, footer) * 4;
	uint32_t ebo = no_os_field_get(OA_DATA_FOOTER_EBO_MASK, footer);
	bool sv = !!(footer & OA_DATA_FOOTER_SV_MASK);
	bool ev = !!(footer & OA_DATA_FOOTER_EV_MASK);
	int ret;

	/* SWO is 4 bits and EBO 6, so either can point past a payload smaller
	 * than 64 bytes. Parity only catches an odd number of flipped bits, so
	 * treat an out-of-bounds offset as a corrupt footer: dropping the frame
	 * and resyncing on the next SV beats copying from past the chunk. */
	if ((sv && sbo >= desc->chunk_size) ||
	    (ev && ebo >= desc->chunk_size)) {
		oa_tc6_rx_drop_in_progress(desc, *frame_buffer);
		return 0;
	}

	if (sv && ev) {
		if (sbo > ebo) {
			/* Two frames in this chunk: finish the one in progress
			 * at [0, ebo], then start the next at [sbo, end]. */
			ret = oa_tc6_rx_copy_slice(desc, frame_buffer, chunk,
						   footer, 0, ebo, false, true);
			if (ret)
				return ret;

			return oa_tc6_rx_copy_slice(
				desc, frame_buffer, chunk, footer, sbo,
				desc->chunk_size - 1, true, false);
		}

		/* A single frame, entirely within this chunk. */
		return oa_tc6_rx_copy_slice(desc, frame_buffer, chunk, footer,
					    sbo, ebo, true, true);
	}

	if (ev)
		/* Chunk holds only the end of the frame in progress. */
		return oa_tc6_rx_copy_slice(desc, frame_buffer, chunk, footer,
					    0, ebo, false, true);

	if (sv)
		/* Chunk holds only the start of a frame, at offset SWO. */
		return oa_tc6_rx_copy_slice(desc, frame_buffer, chunk, footer,
					    sbo, desc->chunk_size - 1, true,
					    false);

	/* Neither boundary: the whole chunk is data for the frame in progress. */
	return oa_tc6_rx_copy_slice(desc, frame_buffer, chunk, footer, 0,
				    desc->chunk_size - 1, false, false);
}

/**
 * @brief Convert the received chunks into frames.
 * @param desc - the OA TC6 descriptor
 * @param chunks - array containing chunks received from the MAC
 * @param len - the number of chunks
 * @param tx_accepted - out: leading chunks whose TX the MAC-PHY accepted
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_rx_chunk_to_frame(struct oa_tc6_desc *desc, uint8_t *chunks,
				    uint32_t len, uint32_t *tx_accepted)
{
	struct oa_tc6_rx_pass pass = { .tx_accepted = len };
	struct oa_tc6_frame_buffer *frame_buffer;
	uint32_t footer;
	uint32_t i;
	int ret;

	*tx_accepted = len;
	ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, false);
	if (ret == -ENOBUFS)
		frame_buffer = &desc->rx_sink;
	else if (ret)
		return ret;

	/* Snapshot per-queue credit to detect 0->non-zero recovery below. */
	for (i = 0; i < desc->tx_q_count; i++)
		desc->tx_q[i].prev_tx_credit = desc->tx_q[i].tx_credit;

	for (i = 0; i < len; i++) {
		footer = no_os_get_unaligned_be32(&chunks[desc->chunk_size]);

		if (oa_tc6_rx_footer_usable(desc, footer, i, frame_buffer,
					    &pass)) {
			ret = oa_tc6_rx_place_data(desc, &frame_buffer, chunks,
						   footer);
			if (ret) {
				*tx_accepted = pass.tx_accepted;
				return ret;
			}
		}

		chunks += desc->chunk_size + OA_FOOTER_LEN;
	}

	*tx_accepted = pass.tx_accepted;

	/*
	 * A status read is owed (EXST/HDRB) but MUST NOT be issued here: this
	 * runs from the async completion path with async_state still
	 * DATA_PENDING, so a ctrl read would spin in oa_tc6_ctrl_transceive to
	 * the watchdog and wedge the RX DMA. Defer it to the state machine's IDLE
	 * branch, where the bus is genuinely free (see oa_tc6_thread_async).
	 */
	if (pass.status_pending)
		desc->status0_pending = true;

	/* RCA is aggregate, from the last footer of this transaction; per-queue
	 * TXC was already reconciled per-footer above. Harvest only from a
	 * parity-valid footer; keep the prior estimate if none was. */
	if (pass.have_good_footer)
		desc->data_rx_credit = no_os_field_get(OA_DATA_FOOTER_RCA_MASK,
						       pass.last_good_footer);

	if (desc->event_cb) {
		/* A footer reports RX chunks waiting -- notify the caller. */
		if (pass.have_good_footer && desc->data_rx_credit)
			desc->event_cb(OA_TC6_EVENT_RX_READY, NULL,
				       desc->event_cb_arg);

		/* Any queue's credit recovered from zero -- TX can resume. */
		for (i = 0; i < desc->tx_q_count; i++) {
			struct oa_tc6_queue *q = &desc->tx_q[i];

			if (!q->prev_tx_credit && q->tx_credit) {
				desc->event_cb(OA_TC6_EVENT_TX_CREDITS, NULL,
					       desc->event_cb_arg);
				break;
			}
		}
	}

	return 0;
}

/**
 * @brief Bootstrap the aggregate RX credit from the BUFST register.
 *
 * Per-queue TX credit is not register-readable (footer-only), so it is seeded
 * from each queue's init_credit at init; only RCA is read here.
 * @param desc - the OA TC6 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_update_stats(struct oa_tc6_desc *desc)
{
	uint32_t reg_val;
	int ret;

	ret = oa_tc6_reg_read(desc, OA_TC6_BUFST_REG, &reg_val);
	if (ret)
		return ret;

	desc->data_rx_credit = no_os_field_get(OA_TC6_BUFSTS_RCA_MASK, reg_val);

	return 0;
}

/**
 * @brief Gets the latched transfer flags that are read from the data chunk
 * footer. Optionally clears the latched values
 * @param desc - the OA TC6 descriptor
 * @param flags - Storage location for flag values
 * @param clear - If set, clears the latch and valid
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_xfer_flags(struct oa_tc6_desc *desc, struct oa_tc6_flags *flags,
			  bool clear)
{
	if (!desc || !flags)
		return -EINVAL;

	memcpy(flags, &desc->xfer_flags, sizeof(struct oa_tc6_flags));

	if (clear)
		memset(&desc->xfer_flags, 0, sizeof(struct oa_tc6_flags));

	return 0;
}

/*
 * Watchdog: has a transfer launched at start_us overrun its deadline? Detects a
 * lost completion, which serialization alone cannot. False when disabled or no
 * clock is available.
 */
static bool oa_tc6_xfer_timed_out(uint64_t start_us, bool clock_ok)
{
	uint64_t now = 0;

	if (!CONFIG_OA_ASYNC_XFER_TIMEOUT_US || !clock_ok)
		return false;

	if (capi_uptime(&now))
		return false;

	/* A backward clock step would underflow (now - start) and fire the
	 * watchdog spuriously -- treat it as not-yet-timed-out. */
	if (now <= start_us)
		return false;

	return (now - start_us) > CONFIG_OA_ASYNC_XFER_TIMEOUT_US;
}

static bool oa_tc6_async_xfer_timed_out(struct oa_tc6_desc *desc)
{
	return oa_tc6_xfer_timed_out(desc->async_start_us,
				     desc->async_clock_ok);
}

/*
 * Tear down a wedged async data transfer: abort the DMA, reset the state
 * machine and drop the ctrl_lock hold the transfer took at launch.
 */
static void oa_tc6_async_abort(struct oa_tc6_desc *desc)
{
	bool held = desc->async_state == OA_ASYNC_DATA_PENDING;

	capi_spi_abort_async(desc->comm_desc);
	oa_tc6_tx_commit(desc, 0);
	oa_tc6_rx_xfer_lost(desc);
	desc->async_xfer_done = false;
	desc->async_xfer_error = false;
	desc->async_state = OA_ASYNC_IDLE;

	if (held)
		capi_mutex_unlock(desc->ctrl_lock);
}

/*
 * Block until any in-flight async data transfer completes -- ctrl and data
 * transactions share the same SPI bus. Under an RTOS another task never gets
 * here mid-transfer: ctrl_lock is held for the whole in-flight transfer (see
 * oa_tc6_thread_async), so it blocks on the mutex instead. The poll task itself
 * (recursive lock) and single-threaded bare-metal (no-op lock) rely on this
 * bounded spin. The watchdog breaks it if the completion is genuinely lost.
 */
static void oa_tc6_async_bus_wait(struct oa_tc6_desc *desc)
{
	if (!desc->async_mode)
		return;

	while (desc->async_state == OA_ASYNC_DATA_PENDING &&
	       !desc->async_xfer_done) {
		if (oa_tc6_async_xfer_timed_out(desc)) {
			oa_tc6_async_abort(desc);
			break;
		}
	}
}

/*
 * Clock len bytes of ctrl_chunks. In async mode the transfer goes through the
 * DMA path like a data transfer, under the same watchdog, and the caller waits
 * for its completion.
 */
static int oa_tc6_ctrl_transceive(struct oa_tc6_desc *desc, uint32_t len)
{
	struct capi_spi_transfer xfer = { 0 };
	uint64_t start_us = 0;
	bool clock_ok;
	int ret;

	oa_tc6_async_bus_wait(desc);

	xfer.tx_buf = desc->ctrl_chunks;
	xfer.rx_buf = desc->ctrl_chunks;
	xfer.tx_size = len;
	xfer.rx_size = len;

	if (!desc->async_mode)
		return capi_spi_transceive(desc->comm_desc, &xfer);

	desc->ctrl_xfer_done = false;
	desc->ctrl_xfer_error = false;
	desc->ctrl_pending = true;
	clock_ok = !capi_uptime(&start_us);

	ret = capi_spi_transceive_async(desc->comm_desc, &xfer);
	while (!ret && !desc->ctrl_xfer_done) {
		if (oa_tc6_xfer_timed_out(start_us, clock_ok)) {
			capi_spi_abort_async(desc->comm_desc);
			ret = -ETIMEDOUT;
		}
	}
	desc->ctrl_pending = false;

	if (!ret && desc->ctrl_xfer_error)
		ret = -EIO;

	return ret;
}

/*
 * Per-queue TX credit is reported only in the footer of a chunk stamped with
 * that queue's VS; there is no credit register. Return the VS of the first
 * queue with a frame waiting but no credit to send it, so the caller
 * can stamp it on the transaction's DV=0 chunks and refresh that credit.
 * Without this a queue that clocks no data of its own -- the high-priority queue
 * behind a backlogged low-priority stream, or during a bidirectional RX stream
 * -- never learns it has credit and stalls.
 */
static bool oa_tc6_credit_solicit_vs(struct oa_tc6_desc *desc, uint8_t *vs)
{
	struct oa_tc6_queue *q;
	int open_qi;
	uint8_t start;
	uint8_t qi;
	uint8_t k;

	/* A queue with a frame open on the wire is the one that has to be
	 * credited first -- nothing else may be clocked until it closes -- so
	 * scan from it. Otherwise scan in service order, so that under
	 * round-robin several starved queues each get their footer solicited
	 * in turn. */
	open_qi = oa_tc6_open_frame_qi(desc);
	start = open_qi >= 0 ? (uint8_t)open_qi : oa_tc6_tx_lead_queue(desc);

	for (k = 0; k < desc->tx_q_count; k++) {
		qi = oa_tc6_tx_nth_queue(desc, start, k);
		q = &desc->tx_q[qi];
		if (oa_tc6_queue_ready(desc, q) &&
		    !oa_tc6_queue_sendable(desc, q)) {
			*vs = q->vs;
			return true;
		}
	}

	return false;
}

/*
 * Decide whether a data transaction should run and how to prime it.
 *
 * A transaction is worth clocking when RX is available, when some queue is
 * sendable (ready frame + credit), when a queue is waiting on credit, or when
 * the caller forced one (interrupt). Returns true if a transaction should run;
 * *force primes a forced empty chunk, *solicit a leading one stamped
 * *solicit_vs (also stamped on any RX-drain chunks).
 */
static bool oa_tc6_tx_should_run(struct oa_tc6_desc *desc, bool *force,
				 bool *solicit, uint8_t *solicit_vs)
{
	bool sendable = false;
	uint8_t i;

	*solicit = false;
	*solicit_vs = 0;

	/* A queue with any credit can send now. */
	for (i = 0; i < desc->tx_q_count; i++)
		if (oa_tc6_queue_sendable(desc, &desc->tx_q[i]))
			sendable = true;

	/*
	 * A queue waiting on credit gets a DV=0 chunk stamped with its VS, the
	 * only way the MAC-PHY reports that credit (there is no per-queue credit
	 * register). Solicited even when another queue can send: that queue's
	 * own chunks would otherwise carry every footer of every pass, starving
	 * the waiting queue for as long as the sending one has frames -- a
	 * backlogged low-priority stream would keep the high-priority queue at
	 * zero credit until it ran dry.
	 */
	*solicit = oa_tc6_credit_solicit_vs(desc, solicit_vs);

	/* Otherwise run only to drain RX or service a pending IRQ. */
	return sendable || *solicit || *force || desc->data_rx_credit;
}

/*
 * Common data-pass prologue for both the sync and async datapaths: consume a
 * pending interrupt into a forced empty chunk, and bootstrap the aggregate RX
 * credit once from BUFST (later kept current from data-chunk footers).
 */
static int oa_tc6_data_pass_begin(struct oa_tc6_desc *desc, bool *force)
{
	int ret;

	if (!desc->credit_valid) {
		ret = oa_tc6_update_stats(desc);
		if (ret)
			return ret;
		desc->credit_valid = true;
	}

	*force = desc->irq_pending;
	desc->irq_pending = false;

	return 0;
}

/*
 * Decide whether a data transaction should run, and if so pack it into
 * desc->data_chunks and fill *xfer. *bytes_total == 0 means nothing to clock.
 * *force is cleared after packing so a forced chunk emits only once per pass.
 */
static int oa_tc6_pack_xfer(struct oa_tc6_desc *desc, bool *force,
			    struct capi_spi_transfer *xfer,
			    uint32_t *bytes_total)
{
	uint8_t force_vs;
	bool solicit;

	if (!oa_tc6_tx_should_run(desc, force, &solicit, &force_vs)) {
		*bytes_total = 0;
		return 0;
	}

	oa_tc6_tx_frame_to_chunks(desc, desc->data_chunks, desc->data_rx_credit,
				  *force, solicit, force_vs, bytes_total);
	*force = false;

	xfer->tx_buf = desc->data_chunks;
	xfer->rx_buf = desc->data_chunks;
	xfer->tx_size = *bytes_total;
	xfer->rx_size = *bytes_total;

	return 0;
}

/*
 * Turn a completed data transaction's chunks into frame(s) and advance
 * *rx_limit. Deliberately does not service status0_pending -- that stays
 * deferred to the async IDLE branch, never the completion path (a ctrl read
 * here would collide with an in-flight transfer on the shared bus).
 */
static int oa_tc6_process_rx(struct oa_tc6_desc *desc, uint32_t bytes_total,
			     uint32_t *rx_limit)
{
	uint32_t accepted = 0;
	int ret;

	ret = oa_tc6_rx_chunk_to_frame(
		desc, desc->data_chunks,
		bytes_total / (desc->chunk_size + OA_HEADER_LEN), &accepted);
	oa_tc6_tx_commit(desc, accepted);
	if (ret)
		return ret;

	(*rx_limit)++;

	return 0;
}

/*
 * Issue data-path SPI transactions until credits are exhausted or the RX-limit
 * trip. Per-queue TX credit is harvested from data-transaction footers; a
 * synchronous BUFST register read is only done once to bootstrap the aggregate
 * RX credit (no footer seen yet).
 */
static int oa_tc6_run_data(struct oa_tc6_desc *desc)
{
	uint32_t rx_limit = 0;
	uint32_t bytes_total;
	bool force;
	int ret;

	struct capi_spi_transfer xfer = { 0 };

	ret = oa_tc6_data_pass_begin(desc, &force);
	if (ret)
		return ret;

	do {
		oa_tc6_pack_xfer(desc, &force, &xfer, &bytes_total);

		/*
		 * Nothing to clock this pass -- no credit and no RX, so no
		 * transaction can make progress. Stop and let a later poll retry
		 * once the MAC has advertised fresh credit.
		 */
		if (!bytes_total)
			break;

		ret = capi_spi_transceive(desc->comm_desc, &xfer);
		if (ret) {
			memset(desc->data_chunks, 0, bytes_total);
			oa_tc6_tx_commit(desc, 0);
			oa_tc6_rx_xfer_lost(desc);
			return ret;
		}

		ret = oa_tc6_process_rx(desc, bytes_total, &rx_limit);
		if (ret)
			return ret;
	} while (rx_limit <= CONFIG_OA_THREAD_RX_LIMIT);

	return 0;
}

int oa_tc6_thread(struct oa_tc6_desc *desc)
{
	int ret;

	/*
	 * Hold the bus lock across the whole synchronous data pass so register
	 * access from another task cannot collide with a data transfer on the
	 * shared SPI bus. update_stats()/reg reads nested inside re-enter ctrl_lock
	 * recursively; the transfers themselves keep data_lock leaf-level.
	 */
	capi_mutex_lock(desc->ctrl_lock);
	ret = oa_tc6_run_data(desc);
	/*
	 * Bus is free here in sync mode (no async DMA in flight), so a ctrl read
	 * cannot spin in oa_tc6_ctrl_transceive. Service any STATUS0 read owed
	 * by the data pass -- the async path does this from its IDLE branch
	 * instead.
	 */
	if (!ret)
		oa_tc6_service_status0(desc);
	capi_mutex_unlock(desc->ctrl_lock);

	return ret;
}

/*
 * SPI completion callback registered with the CAPI SPI controller.
 * Called from the DMA (or interrupt) ISR when a transfer started by
 * oa_tc6_thread_async() completes or errors. Sets the done flag so the
 * next main-loop call to oa_tc6_thread_async() can advance the state machine.
 */
static void oa_tc6_spi_callback(enum capi_async_event event, void *arg,
				int event_extra)
{
	struct oa_tc6_desc *desc = arg;

	(void)event_extra;

	if (desc->ctrl_pending) {
		if (event != CAPI_SPI_EVENT_XFR_DONE)
			desc->ctrl_xfer_error = true;
		desc->ctrl_xfer_done = true;
		return;
	}

	if (event != CAPI_SPI_EVENT_XFR_DONE)
		desc->async_xfer_error = true;

	desc->async_xfer_done = true;

	if (desc->async_state == OA_ASYNC_DATA_PENDING &&
	    desc->async_xfer_done_cb)
		desc->async_xfer_done_cb(desc->async_xfer_done_cb_arg);
}

/*
 * OA_ASYNC_IDLE handler: pack the next batch of chunks and kick off a DMA
 * transfer, transitioning to OA_ASYNC_DATA_PENDING. If there is nothing to
 * transfer, stay in OA_ASYNC_IDLE.
 *
 * Credit (TXC/RCA) is kept current from data-chunk footers, so a BUFST read is
 * only needed on the very first call (credit_valid == false). When TX is queued
 * but the MAC last advertised no credit, an empty chunk is clocked instead so
 * its footer refreshes the credit counts before the next poll sends.
 *
 * Returns 0 on success (transfer started or nothing to do), negative on error.
 */
static int oa_tc6_async_start(struct oa_tc6_desc *desc)
{
	struct capi_spi_transfer xfer = { 0 };
	uint32_t bytes_total;
	bool force;
	int ret;

	desc->async_state = OA_ASYNC_IDLE;

	ret = oa_tc6_data_pass_begin(desc, &force);
	if (ret)
		return ret;

	oa_tc6_pack_xfer(desc, &force, &xfer, &bytes_total);
	if (!bytes_total)
		return 0; /* nothing to clock -- stay idle */

	desc->async_bytes_total = bytes_total;
	desc->async_xfer_done = false;
	desc->async_xfer_error = false;

	desc->async_state = OA_ASYNC_DATA_PENDING;

	/* Stamp launch time for the watchdog; disable it if no clock. */
	desc->async_clock_ok = (capi_uptime(&desc->async_start_us) == 0);

	ret = capi_spi_transceive_async(desc->comm_desc, &xfer);
	if (ret) {
		desc->async_state = OA_ASYNC_IDLE;
		desc->irq_pending = true;
		return ret;
	}

	return 0;
}

/*
 * OA_ASYNC_DATA_PENDING handler: called once the DMA transfer completes.
 * Turns the received chunks into frames and reports whether the caller should
 * try to start another transfer this round (true) or stop (false, hit the
 * per-poll RX limit).
 *
 * Returns 0 and sets *again; negative on error (state reset to idle by caller).
 */
static int oa_tc6_async_complete(struct oa_tc6_desc *desc, bool *again)
{
	int ret;

	*again = false;

	ret = oa_tc6_process_rx(desc, desc->async_bytes_total,
				&desc->async_rx_limit);
	if (ret)
		return ret;

	/*
	 * Keep draining while the last footer reports RCA > 0, bounded by the
	 * safety cap: a fixed per-poll count isn't enough, since at line rate
	 * the RX FIFO can refill faster than a few transactions drain it and
	 * overflow into LOFE.
	 */
	*again = desc->data_rx_credit &&
		 desc->async_rx_limit <= CONFIG_OA_THREAD_RX_LIMIT;

	return 0;
}

/**
 * @brief Non-blocking variant of oa_tc6_thread() for DMA-capable SPI.
 *
 * A two-state machine driven once per main-loop poll:
 *
 *   OA_ASYNC_IDLE            start a DMA transfer (OA_ASYNC_DATA_PENDING),
 *                            or stay idle if there is nothing to send/receive.
 *   OA_ASYNC_DATA_PENDING    wait for the DMA completion flag; once set,
 *                            process the received chunks and either start the
 *                            next transfer (staying PENDING) or go back to IDLE
 *                            after CONFIG_OA_THREAD_RX_LIMIT rounds.
 *
 * On any error the state is reset to OA_ASYNC_IDLE so the next poll retries.
 *
 * @param desc - the OA TC6 descriptor
 * @return 0 on success (including "DMA still running, try again later"),
 *         negative error code on SPI or protocol error.
 */
int oa_tc6_thread_async(struct oa_tc6_desc *desc)
{
	bool again;
	int ret;

	/*
	 * Bus-lock discipline: ctrl_lock is held for exactly as long as a
	 * transfer is in flight (async_state == OA_ASYNC_DATA_PENDING), across
	 * every poll iteration that spans -- taken when a transfer starts below
	 * and released only once state falls back to IDLE. This mutually
	 * excludes register access on the shared bus without a busy-spin; the
	 * poll task both takes and releases it, as FreeRTOS ownership requires.
	 */
	switch (desc->async_state) {
	case OA_ASYNC_DATA_PENDING:
		/* DMA still in flight -- yield to the main loop, bus still held. */
		if (!desc->async_xfer_done) {
			/* Watchdog: a lost completion would wedge here with the
			 * bus lock held -- abort, release it, report the timeout. */
			if (oa_tc6_async_xfer_timed_out(desc)) {
				oa_tc6_async_abort(desc);
				return -ETIMEDOUT;
			}
			return 0;
		}

		/* async_xfer_done stays set until the next launch, so a ctrl
		 * access from event_cb below does not wait on this transfer. */
		if (desc->async_xfer_error) {
			desc->async_xfer_error = false;
			desc->async_state = OA_ASYNC_IDLE;
			oa_tc6_tx_commit(desc, 0);
			oa_tc6_rx_xfer_lost(desc);
			capi_mutex_unlock(desc->ctrl_lock);
			return -EIO;
		}

		ret = oa_tc6_async_complete(desc, &again);
		if (ret || !again) {
			desc->async_state = OA_ASYNC_IDLE;
			capi_mutex_unlock(desc->ctrl_lock);
			return ret;
		}
		/*
		 * Room for more this round -- start the next transfer below with the
		 * bus still held (state stays DATA_PENDING on a successful restart).
		 */
		break;

	case OA_ASYNC_IDLE:
	default:
		desc->async_rx_limit = 0;
		/* Acquire the bus before starting a transfer. */
		capi_mutex_lock(desc->ctrl_lock);
		/*
		 * Bus is free here (state IDLE, no DMA in flight, lock held), so a
		 * ctrl read cannot spin in oa_tc6_ctrl_transceive. This is the only
		 * safe point to service a STATUS0 read/clear owed by a prior
		 * transaction.
		 */
		oa_tc6_service_status0(desc);
		/*
		 * Do NOT read BUFST here -- RCA is kept current from every
		 * data-chunk footer, so BUFST is only for the one-time bootstrap
		 * (credit_valid == false) inside async_start(). RX arriving while
		 * idle is handled by the nINT/RX_RDY interrupt (irq_pending),
		 * which makes async_start() force a DV=0 probe to refresh RCA.
		 */
		break;
	}

	ret = oa_tc6_async_start(desc);

	/*
	 * async_start() leaves the state at OA_ASYNC_DATA_PENDING only when it
	 * actually launched a transfer; on "nothing to send" or a start error it
	 * leaves (or resets) it to OA_ASYNC_IDLE. Release the bus whenever no
	 * transfer is in flight; keep holding it across the DMA otherwise.
	 */
	if (desc->async_state != OA_ASYNC_DATA_PENDING)
		capi_mutex_unlock(desc->ctrl_lock);

	return ret;
}

/*
 * Reject a chunk payload the part cannot be configured for: CONFIG0.CPS would
 * keep its old value while the host framed every transaction at the requested
 * size, leaving the footer unfindable (7.6). STDCAP.MINCPS uses the same
 * 1 << N encoding as CPS; 0 is reserved, so treat it as "no constraint" rather
 * than failing a part that does not populate the field.
 */
static int oa_tc6_check_cps(struct oa_tc6_desc *desc, uint8_t cps)
{
	uint32_t stdcap;
	uint8_t mincps;
	int ret;

	ret = oa_tc6_reg_read(desc, OA_TC6_STDCAP_REG, &stdcap);
	if (ret)
		return ret;

	mincps = no_os_field_get(OA_TC6_STDCAP_MINCPS_MASK, stdcap);

	return (mincps && cps < mincps) ? -ENOTSUP : 0;
}

static int oa_tc6_setup_config0(struct oa_tc6_desc *desc,
				struct oa_tc6_init_param *param)
{
	uint32_t new_reg_val =
		no_os_field_prep(OA_TC6_CONFIG0_CPS_MASK, param->cps);
	bool want_prote;
	int ret;

	ret = oa_tc6_check_cps(desc, param->cps);
	if (ret)
		return ret;

	want_prote = param->prote_spi;

	if (want_prote)
		new_reg_val |= OA_TC6_CONFIG0_PROTE_MASK;

	/*
	 * PROTE only takes effect on transactions issued AFTER the write that
	 * sets it, so the CONFIG0 write that turns protection on must itself be
	 * framed for the chip's CURRENT protection state, not the target one.
	 * desc->prote_spi already tracks that current state -- false after
	 * oa_tc6_sw_reset(), as a soft reset clears PROTE -- so leave it untouched
	 * for the write and adopt the target only once the write has landed.
	 *
	 * Getting this wrong is a hard error: framing the enabling write as
	 * protected (header, val, ~val) against a still-unprotected MAC-PHY makes
	 * it echo only header+val then idle MISO high, so the complement slot
	 * reads 0xFFFFFFFF and __oa_tc6_reg_write()'s echo check rejects it with
	 * -EIO; the converse (unprotected framing to a protected part) trips the
	 * part's own control-data protection check.
	 */
	ret = __oa_tc6_reg_write(desc, OA_TC6_CONFIG0_REG, new_reg_val);
	if (ret)
		return ret;

	desc->prote_spi = want_prote;

	/*
	 * SYNC goes in a SECOND write, once CPS has landed. The chunk payload
	 * size must be configured before the configuration is synchronised and
	 * must not change afterwards without a MAC-PHY reset, so setting both in
	 * one write leaves it to the part whether CPS is latched on the same
	 * cycle SYNC rises. The hazard is invisible at the 64-byte default --
	 * CPS already holds its reset value, so that write does not change it --
	 * and only bites for any other chunk size.
	 */
	return __oa_tc6_reg_write(desc, OA_TC6_CONFIG0_REG,
				  new_reg_val | OA_TC6_CONFIG0_SYNC_MASK);
}

/**
 * @brief Soft reset the MAC-PHY and wait for STATUS0.RESETC.
 * @param desc - the OA TC6 descriptor.
 * @return 0 in case of success, -ETIMEDOUT on timeout, negative error
 *         otherwise.
 */
int oa_tc6_sw_reset(struct oa_tc6_desc *desc)
{
	uint32_t timeout = 1000;
	uint32_t reg_val;
	int ret;

	ret = oa_tc6_reg_write(desc, OA_TC6_RESET_REG, OA_TC6_RESET_SWRESET);
	if (ret)
		return ret;

	/*
	 * A soft reset clears CONFIG0.PROTE to its reset default (0), so the
	 * MAC-PHY is UNPROTECTED coming out of reset regardless of how it booted
	 * -- an AD3306 that powers up protected included (verified on the wire:
	 * post-reset STATUS0 reads carry RESETC in the data word but do NOT drive
	 * the PROTE complement). Frame the RESETC poll unprotected and leave
	 * prote_spi=false so the CONFIG0 write in oa_tc6_setup_config0() -- the
	 * write that re-enables protection -- is itself framed unprotected, as
	 * PROTE only takes effect on the transaction AFTER it.
	 */
	desc->prote_spi = false;

	do {
		/*
		 * Tolerate transient read errors while the part is still coming
		 * out of reset (e.g. a control-data complement that is not yet
		 * valid); only RESETC or the timeout ends the loop.
		 */
		ret = oa_tc6_reg_read(desc, OA_TC6_STATUS0_REG, &reg_val);
		if (!ret && no_os_field_get(OA_TC6_STATUS0_RESETC, reg_val))
			return 0;

		capi_wait_ms(1);
	} while (--timeout);

	return -ETIMEDOUT;
}

/**
 * @brief Program the runtime CONFIG0 options and the host state that follows
 *        them (TX SWO packing, RX timestamp prepend width).
 * @param desc - the device descriptor
 * @param cfg - the options to apply
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_set_config(struct oa_tc6_desc *desc, const struct oa_tc6_config *cfg)
{
	uint32_t val;
	int ret;

	if (!desc || !cfg || cfg->txcthresh > 3)
		return -EINVAL;

	val = no_os_field_prep(OA_TC6_CONFIG0_TXCTHRESH_MASK, cfg->txcthresh);
	if (cfg->csarfe)
		val |= OA_TC6_CONFIG0_CSARFE_MASK;
	if (cfg->zarfe)
		val |= OA_TC6_CONFIG0_ZARFE_MASK;
	if (cfg->txcte)
		val |= OA_TC6_CONFIG0_TXCTE_MASK;
	if (cfg->rxcte)
		val |= OA_TC6_CONFIG0_RXCTE_MASK;
	if (cfg->ftse)
		val |= OA_TC6_CONFIG0_FTSE_MASK;
	if (cfg->ftss)
		val |= OA_TC6_CONFIG0_FTSS_MASK;

	ret = oa_tc6_reg_update(desc, OA_TC6_CONFIG0_REG, val,
				OA_TC6_CONFIG0_RUNTIME_MASK);
	if (ret)
		return ret;

	desc->zero_swo = cfg->zarfe;
	desc->rx_ts_len = cfg->ftse ? (cfg->ftss ? 8 : 4) : 0;

	return 0;
}

/**
 * @brief Read back the runtime CONFIG0 options.
 * @param desc - the device descriptor
 * @param cfg - filled with the current options
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_config(struct oa_tc6_desc *desc, struct oa_tc6_config *cfg)
{
	uint32_t val;
	int ret;

	if (!desc || !cfg)
		return -EINVAL;

	ret = oa_tc6_reg_read(desc, OA_TC6_CONFIG0_REG, &val);
	if (ret)
		return ret;

	cfg->txcthresh = no_os_field_get(OA_TC6_CONFIG0_TXCTHRESH_MASK, val);
	cfg->csarfe = !!(val & OA_TC6_CONFIG0_CSARFE_MASK);
	cfg->zarfe = !!(val & OA_TC6_CONFIG0_ZARFE_MASK);
	cfg->txcte = !!(val & OA_TC6_CONFIG0_TXCTE_MASK);
	cfg->rxcte = !!(val & OA_TC6_CONFIG0_RXCTE_MASK);
	cfg->ftse = !!(val & OA_TC6_CONFIG0_FTSE_MASK);
	cfg->ftss = !!(val & OA_TC6_CONFIG0_FTSS_MASK);

	return 0;
}

/**
 * @brief Wire up the TX queues, replacing any previously configured set.
 * @param desc - the device descriptor
 * @param tx_q - array of queues, each with its pool and wire VS assigned
 * @param count - number of queues in the array
 * @return 0 in case of success, negative error code otherwise
 *
 * Resets each queue's ring to empty, seeds its live credit from init_credit and
 * rebuilds the VS -> queue lookup. Callers that only learn how many queues the
 * part has after the transport is up (the queue count is a chip variant
 * property) can re-run this once the variant is known; it discards anything
 * still queued, so call it before frames are submitted. Each queue MUST have a
 * distinct VS; on error the previous queue set is left untouched.
 */
int oa_tc6_set_tx_queues(struct oa_tc6_desc *desc, struct oa_tc6_queue *tx_q,
			 uint8_t count)
{
	int8_t map[NO_OS_ARRAY_SIZE(desc->vs_to_txq)];
	struct oa_tc6_queue *q;
	uint8_t i;

	if (!desc || !tx_q || !count)
		return -EINVAL;

	/* Distinct VS values also bound count to the VS space. */
	memset(map, -1, sizeof(map));
	for (i = 0; i < count; i++) {
		q = &tx_q[i];
		if (!q->pool || !q->pool_size ||
		    q->vs >= NO_OS_ARRAY_SIZE(map) || map[q->vs] >= 0)
			return -EINVAL;
		map[q->vs] = i;
	}

	for (i = 0; i < count; i++) {
		q = &tx_q[i];
		oa_tc6_pool_reset(q->pool, q->pool_size);
		q->head = 0;
		q->tail = 0;
		q->count = 0;
		/* An empty FIFO has the reserve free. */
		q->tx_credit =
			no_os_max(q->init_credit, desc->tx_credit_reserve);
		q->prev_tx_credit = 0;
	}

	memcpy(desc->vs_to_txq, map, sizeof(map));
	desc->tx_q = tx_q;
	desc->tx_q_count = count;
	desc->tx_rr_cursor = 0;
	desc->tx_sv_q = -1;

	return 0;
}

/**
 * @brief Keep reserve chunks of credit free in every TX FIFO, for a MAC-PHY
 * whose footer TXC reports only the FIFO that last received an SV.
 * @param desc - the OA TC6 descriptor
 * @param reserve - chunks to keep free; 0 restores per-chunk VS credit
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_set_tx_credit_reserve(struct oa_tc6_desc *desc, uint8_t reserve)
{
	uint8_t i;

	if (!desc)
		return -EINVAL;

	desc->tx_credit_reserve = reserve;
	desc->tx_sv_q = -1;
	/* Called before frames are submitted, so every FIFO is empty. */
	for (i = 0; i < desc->tx_q_count; i++)
		desc->tx_q[i].tx_credit =
			no_os_max(desc->tx_q[i].tx_credit, reserve);

	return 0;
}

/*
 * Wire up the TX queues from the init parameters. The caller passes either an
 * array of queues (tx_q) or a single plain pool (tx_pool), the latter wrapped
 * in the built-in single queue (VS 0).
 */
static int oa_tc6_setup_tx_queues(struct oa_tc6_desc *desc,
				  struct oa_tc6_init_param *param)
{
	struct oa_tc6_queue *q;

	desc->tx_sched = param->tx_sched;

	if (param->tx_q_count)
		return oa_tc6_set_tx_queues(desc, param->tx_q,
					    param->tx_q_count);

	if (!param->tx_pool || !param->tx_pool_size)
		return -EINVAL;

	q = &desc->tx_q_storage;
	q->pool = param->tx_pool;
	q->pool_size = param->tx_pool_size;
	q->vs = 0;
	q->init_credit = 0;

	return oa_tc6_set_tx_queues(desc, q, 1);
}

/**
 * @brief Allocate resources for the OA TC6 driver.
 * @param desc - the device descriptor to be initialized
 * @param param - the device's parameter
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_init(struct oa_tc6_desc *desc, struct oa_tc6_init_param *param)
{
	int ret;

	if (!desc || !param || !param->rx_pool || !param->rx_pool_size)
		return -EINVAL;

	if (param->cps == 0)
		param->cps = OA_TC6_CONFIG0_CPS_64;
	if (param->cps < OA_TC6_CONFIG0_CPS_8 ||
	    param->cps > OA_TC6_CONFIG0_CPS_64)
		return -EINVAL;

	memset(desc, 0, sizeof(*desc));

	desc->chunk_size = 1u << param->cps;
	desc->sw_fcs = param->sw_fcs;
	desc->tx_no_pad = param->tx_no_pad;
	desc->comm_desc = param->comm_desc;
	desc->async_xfer_done_cb = param->async_xfer_done_cb;
	desc->async_xfer_done_cb_arg = param->async_xfer_done_cb_arg;
	desc->rx_pool = param->rx_pool;
	desc->rx_pool_size = param->rx_pool_size;
	oa_tc6_pool_reset(desc->rx_pool, desc->rx_pool_size);

	ret = oa_tc6_setup_tx_queues(desc, param);
	if (ret)
		return ret;

	/*
	 * ctrl_lock is always non-NULL: a real mutex config (RTOS context) or
	 * the shared no-op sentinel, so lock/unlock never need a NULL guard.
	 */
	if (param->ctrl_mutex_cfg) {
		ret = capi_mutex_init(&desc->ctrl_lock, param->ctrl_mutex_cfg);
		if (ret)
			return ret;
	} else {
		desc->ctrl_lock = &oa_noop_mutex;
	}

	/*
	 * data_lock is provisioned exactly like ctrl_lock but MUST be an
	 * independent mutex (its own config): it guards the frame-buffer rings
	 * and is held separately from ctrl_lock. NULL cfg => no-op lock.
	 */
	if (param->data_mutex_cfg) {
		ret = capi_mutex_init(&desc->data_lock, param->data_mutex_cfg);
		if (ret)
			goto err_ctrl_mutex;
	} else {
		desc->data_lock = &oa_noop_mutex;
	}

	/* The reset request is framed for the protection the part boots in. */
	desc->prote_spi = param->prote_spi;

	ret = oa_tc6_sw_reset(desc);
	if (ret)
		goto err_mutex;

	ret = oa_tc6_setup_config0(desc, param);
	if (ret)
		goto err_mutex;

	/*
	 * Host computes the FCS, so disable the MAC's automatic append/check
	 * (CONFIG2.HOST_CRC_APPEND, reset 1) -- the host now appends on TX and
	 * validates on RX.
	 */
	if (desc->sw_fcs) {
		ret = oa_tc6_reg_update(desc, OA_TC6_CONFIG2_REG, 0,
					OA_TC6_CONFIG2_HOST_CRC_APPEND_MASK);
		if (ret)
			goto err_mutex;
	}

	ret = oa_tc6_reg_write(desc, OA_TC6_STATUS0_REG, 0xFFFF);
	if (ret)
		goto err_mutex;

	/* Last: there is no unregister to unwind a later failure with. Control
	 * transfers stay synchronous until the callback is in place. */
	if (param->async_mode) {
		ret = capi_spi_register_callback(param->comm_desc->controller,
						 oa_tc6_spi_callback, desc);
		if (ret)
			goto err_mutex;
		desc->async_mode = true;
	}

	return 0;

err_mutex:
	capi_mutex_deinit(desc->data_lock);
err_ctrl_mutex:
	capi_mutex_deinit(desc->ctrl_lock);
	return ret;
}

/**
 * @brief Free a device descriptor
 * @param desc - the device descriptor to be removed.
 * @return 0
 */
int oa_tc6_remove(struct oa_tc6_desc *desc)
{
	if (!desc)
		return -ENODEV;

	/* A transfer still in flight would complete into a descriptor nobody
	 * owns, and leave the controller busy for whoever initializes next. */
	capi_mutex_lock(desc->ctrl_lock);
	oa_tc6_async_abort(desc);
	capi_mutex_unlock(desc->ctrl_lock);

	capi_mutex_deinit(desc->data_lock);
	capi_mutex_deinit(desc->ctrl_lock);

	return 0;
}

/**
 * @brief Get a snapshot of the software statistics counters.
 * @param desc - the device descriptor.
 * @param stats - storage for the snapshot.
 * @return 0 in case of success, negative error code otherwise.
 */
int oa_tc6_get_stats(struct oa_tc6_desc *desc, struct oa_tc6_stats *stats)
{
	if (!desc || !stats)
		return -EINVAL;

	*stats = desc->stats;

	return 0;
}

/**
 * @brief Reset the software statistics counters to zero.
 * @param desc - the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int oa_tc6_reset_stats(struct oa_tc6_desc *desc)
{
	if (!desc)
		return -EINVAL;

	memset(&desc->stats, 0, sizeof(desc->stats));

	return 0;
}
