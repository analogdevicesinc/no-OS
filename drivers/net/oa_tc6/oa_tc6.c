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

#define OA_TC6_FCS_LEN 4

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
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_get_tx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer **buffer)
{
	struct oa_tc6_frame_buffer *slot;
	struct oa_tc6_queue *q;

	if (!desc)
		return -EINVAL;

	q = &desc->tx_q[0];

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

/*
 * Events are only recorded while the bus lock is held and delivered by
 * oa_tc6_dispatch_events() after it's released, so that the user callback
 * may call back into the driver.
 */
static void oa_tc6_queue_event(struct oa_tc6_desc *desc, uint32_t event)
{
	desc->pending_events |= NO_OS_BIT(event);
}

static void oa_tc6_dispatch_events(struct oa_tc6_desc *desc, uint32_t events)
{
	uint32_t event;

	if (!desc->callback)
		return;

	for (event = 0; events; event++, events >>= 1) {
		if (events & 1)
			desc->callback(desc, event, desc->callback_arg);
	}
}

/**
 * @brief Convert frames in the OA_BUFF_TX_READY state to chunks.
 * Configure empty chunks if we need to receive more then transmit.
 * @param desc - the OA TC6 descriptor
 * @param tx_buffer - the buffer containing the chunks
 * @param tx_credit - the number of chunks available for transmission
 * @param rx_nchunks - the number of chunks available for reception
 * @param tx_written - the number of chunks written in the buffer
 * @param tx_done - set if the last chunk of at least one frame was written
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_tx_frame_to_chunks(struct oa_tc6_desc *desc,
				     uint8_t *tx_buffer,
				     uint32_t tx_credit, uint32_t rx_nchunks,
				     uint32_t *tx_written, bool *tx_done)
{
	struct oa_tc6_queue *q = &desc->tx_q[0];
	uint32_t spi_buffer_index = 0;
	uint32_t spi_buff_max_chunks;
	uint32_t chunks_written = 0;
	uint32_t chunks_limit;
	uint32_t remaining;
	uint32_t copy_len;
	uint32_t header;
	int ret;

	struct oa_tc6_frame_buffer *frame_buffer;
	spi_buff_max_chunks = OA_SPI_BUFF_LEN / (OA_CHUNK_SIZE + OA_HEADER_LEN);

	/* The maximum number of chunks we can potentially send, given the size of our SPI buffer. */
	chunks_limit = no_os_min(spi_buff_max_chunks, tx_credit);
	*tx_done = false;

	/*
	 * A frame that doesn't fit in the remaining space is sent partially. Its
	 * index is kept, and the next call resumes it without setting SV.
	 */
	while (chunks_written < chunks_limit) {
		ret = oa_tc6_get_first_tx_frame(desc, q, &frame_buffer);
		if (ret)
			break;

		remaining = frame_buffer->len - frame_buffer->index;
		if (!remaining)
			break;

		copy_len = no_os_min(remaining, OA_CHUNK_SIZE);

		header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
		header |= no_os_field_prep(OA_DATA_HEADER_DV_MASK, 1);
		header |= no_os_field_prep(OA_DATA_HEADER_VS_MASK, frame_buffer->vs);

		if (!frame_buffer->index) {
			header |= no_os_field_prep(OA_DATA_HEADER_SV_MASK, 1);
			header |= no_os_field_prep(OA_DATA_HEADER_TSC_MASK, frame_buffer->tsc);
		}

		if (remaining <= OA_CHUNK_SIZE) {
			header |= no_os_field_prep(OA_DATA_HEADER_EV_MASK, 1);
			header |= no_os_field_prep(OA_DATA_HEADER_EBO_MASK, remaining - 1);
		}

		header |= oa_tc6_crc1(header);

		no_os_put_unaligned_be32(header, &tx_buffer[spi_buffer_index]);
		spi_buffer_index += OA_HEADER_LEN;
		memcpy(&tx_buffer[spi_buffer_index], &frame_buffer->data[frame_buffer->index],
		       copy_len);
		frame_buffer->index += copy_len;
		spi_buffer_index += OA_CHUNK_SIZE;
		chunks_written++;

		if (frame_buffer->index == frame_buffer->len) {
			desc->stats.tx_frames++;
			desc->stats.tx_bytes += frame_buffer->len;

			capi_mutex_lock(desc->data_lock);
			frame_buffer->len = 0;
			frame_buffer->index = 0;
			frame_buffer->state = OA_BUFF_FREE;
			q->tail = (q->tail + 1) % q->pool_size;
			q->count--;
			capi_mutex_unlock(desc->data_lock);
			*tx_done = true;
		}
	}

	/*
	 * The TX queue may be empty, there is no space in the SPI buffer,
	 * or we're out of tx credits.
	 * If rx_chunks > tx_chunks, we need to add dummy chunks (DV = 0) as long
	 * as there is enough room in the buffer. These don't consume TX credits.
	 */
	while ((rx_nchunks > chunks_written) &&
	       (chunks_written < spi_buff_max_chunks)) {
		header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
		no_os_put_unaligned_be32(header, &tx_buffer[spi_buffer_index]);
		spi_buffer_index += OA_CHUNK_SIZE + OA_HEADER_LEN;
		chunks_written++;
	}

	*tx_written = spi_buffer_index;

	return 0;
}

static int oa_tc6_handle_exst(struct oa_tc6_desc *desc)
{
	uint32_t status0;
	uint32_t status1;
	int ret;

	ret = oa_tc6_reg_read(desc, OA_TC6_STATUS0_REG, &status0);
	if (ret)
		return ret;

	ret = oa_tc6_reg_read(desc, OA_TC6_STATUS1_REG, &status1);
	if (ret)
		return ret;

	if (no_os_field_get(OA_TC6_IMASK0_TXPEM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_TXPE);

	if (no_os_field_get(OA_TC6_IMASK0_TXBOEM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_TXBOE);

	if (no_os_field_get(OA_TC6_IMASK0_TXBUEM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_TXBUE);

	if (no_os_field_get(OA_TC6_IMASK0_RXBOEM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_RXBOE);

	if (no_os_field_get(OA_TC6_IMASK0_LOFEM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_LOFE);

	if (no_os_field_get(OA_TC6_IMASK0_HDREM, status0))
		oa_tc6_queue_event(desc, OA_TC6_EVENT_HDRE);

	ret = oa_tc6_reg_write(desc, OA_TC6_STATUS0_REG, status0);
	if (ret)
		return ret;

	return oa_tc6_reg_write(desc, OA_TC6_STATUS1_REG, status1);
}

/*
 * Service a STATUS0 read/clear owed by a prior data pass (EXST/HDRB). Caller
 * must hold the bus with no data transfer in progress.
 */
static void oa_tc6_service_status0(struct oa_tc6_desc *desc)
{
	if (!desc->status0_pending)
		return;

	oa_tc6_handle_exst(desc);
	desc->status0_pending = false;
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

	/* FD flags the closing frame as bad: still forward it, letting the drop
	 * flag decide. */
	buf->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
	if (buf->frame_drop)
		desc->stats.rx_drop_fd++;

	/*
	 * The MAC-PHY never strips the FCS, so it is dropped from the delivered
	 * length. The minimum length is the MAC's to enforce; here a frame only
	 * has to hold its FCS. A runt frees the slot and hands back the same
	 * buffer so it is never delivered.
	 */
	if (buf->len < OA_TC6_FCS_LEN) {
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
		oa_tc6_queue_event(desc, OA_TC6_EVENT_RX);
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
	oa_tc6_queue_event(desc, OA_TC6_EVENT_SYNCE);
}

/* Pass-scoped state threaded through one RX transaction's footers. */
struct oa_tc6_rx_pass {
	uint32_t last_good_footer; /* newest parity-valid footer (for RCA) */
	bool have_good_footer;
	bool status_pending;  /* a STATUS0 read is owed (EXST/HDRB) */
};

/*
 * Inspect one chunk's footer and apply every non-payload side effect: parity,
 * error draining, SYNC loss, EXST/HDRB. All
 * footer flags are decoded here so the caller sees a clean decision.
 *
 * Returns true if the chunk carries deliverable payload (parity OK, SYNC set,
 * DV=1, not HDRB, not mid-drain) and the caller should place its bytes; false
 * to skip the chunk.
 */
static bool oa_tc6_rx_footer_usable(struct oa_tc6_desc *desc, uint32_t footer,
				    struct oa_tc6_frame_buffer *frame_buffer,
				    struct oa_tc6_rx_pass *pass)
{
	bool sv, dv;

	/*
	 * Parity guards every other footer field, so validate it first:
	 * oa_tc6_crc1() is non-zero on a bad word. Drop the frame in progress
	 * and resync, without trusting this footer's credit or RCA.
	 */
	if (oa_tc6_crc1(footer)) {
		desc->stats.rx_footer_parity_err++;
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
		oa_tc6_rx_drop_in_progress(desc, frame_buffer);
		return false;
	}

	pass->last_good_footer = footer;
	pass->have_good_footer = true;

	sv = !!(footer & OA_DATA_FOOTER_SV_MASK);
	dv = !!(footer & OA_DATA_FOOTER_DV_MASK);

	/* Latch the transfer flags, even on DV=0. */
	desc->xfer_flags.flags_valid = true;
	desc->xfer_flags.sync = !!(footer & OA_DATA_FOOTER_SYNC_MASK);

	/* An unconfigured MAC-PHY returns no RX data (7.6). */
	if (!desc->xfer_flags.sync) {
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
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_rx_chunk_to_frame(struct oa_tc6_desc *desc, uint8_t *chunks,
				    uint32_t len)
{
	struct oa_tc6_rx_pass pass = { 0 };
	struct oa_tc6_frame_buffer *frame_buffer;
	uint32_t footer;
	uint32_t i;
	int ret;

	ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, false);
	if (ret == -ENOBUFS)
		frame_buffer = &desc->rx_sink;
	else if (ret)
		return ret;

	for (i = 0; i < len; i++) {
		footer = no_os_get_unaligned_be32(&chunks[desc->chunk_size]);

		if (oa_tc6_rx_footer_usable(desc, footer, frame_buffer, &pass)) {
			ret = oa_tc6_rx_place_data(desc, &frame_buffer, chunks,
						   footer);
			if (ret)
				return ret;
		}

		chunks += desc->chunk_size + OA_FOOTER_LEN;
	}

	/*
	 * A status read is owed (EXST/HDRB). Defer it to a single read once the
	 * data pass is over, where the bus is free.
	 */
	if (pass.status_pending)
		desc->status0_pending = true;

	/* The credits come from the last parity-valid footer of this
	 * transaction; keep the prior estimate if none was. */
	if (pass.have_good_footer) {
		desc->data_rx_credit = no_os_field_get(OA_DATA_FOOTER_RCA_MASK,
						       pass.last_good_footer);
		desc->data_tx_credit = no_os_field_get(OA_DATA_FOOTER_TXC_MASK,
						       pass.last_good_footer);
	}

	return 0;
}

/**
 * @brief Update the number of RX and TX credit available in the MAC FIFOs.
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

	desc->data_tx_credit = no_os_field_get(OA_TC6_BUFSTS_TXC_MASK, reg_val);
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

/* Clock len bytes of ctrl_chunks. */
static int oa_tc6_ctrl_transceive(struct oa_tc6_desc *desc, uint32_t len)
{
	struct capi_spi_transfer xfer = { 0 };

	xfer.tx_buf = desc->ctrl_chunks;
	xfer.rx_buf = desc->ctrl_chunks;
	xfer.tx_size = len;
	xfer.rx_size = len;

	return capi_spi_transceive(desc->comm_desc, &xfer);
}

static void oa_tc6_add_empty_chunk(struct oa_tc6_desc *desc)
{
	uint32_t header;

	header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
	header |= no_os_field_prep(OA_DATA_HEADER_DV_MASK, 0);
	header |= oa_tc6_crc1(header);

	no_os_put_unaligned_be32(header, desc->data_chunks);
}

/**
 * @brief Transmit all the frames in the OA_BUFF_TX_READY state and receive the
 * frames in the OA_BUFF_RX_COMPLETE state.
 * @param desc - the OA TC6 descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_thread(struct oa_tc6_desc *desc)
{
	uint32_t tx_chunks_avail = 0;
	uint32_t rx_limit = 0;
	uint32_t bytes_total;
	uint32_t events;
	bool tx_done;
	int ret = 0;
	int tx_ret;

	struct oa_tc6_frame_buffer *frame_buffer;
	struct capi_spi_transfer xfer = {0};

	if (!desc)
		return -ENODEV;

	/*
	 * Hold the bus lock across the data transfers, so a register access from
	 * another task cannot collide with them. The register reads nested inside
	 * re-enter ctrl_lock, which must therefore be recursive.
	 */
	capi_mutex_lock(desc->ctrl_lock);

	if (desc->bufst_polling == OA_TC6_REG_POLL) {
		ret = oa_tc6_update_stats(desc);
		if (ret)
			goto unlock;
	}

	tx_ret = oa_tc6_get_first_tx_frame(desc, &desc->tx_q[0], &frame_buffer);
	if (!tx_ret)
		tx_chunks_avail = frame_buffer->len;

	if (desc->bufst_polling == OA_TC6_REG_POLL) {
		if (!desc->data_rx_credit && !tx_chunks_avail)
			goto unlock;
	}

	do {
		oa_tc6_tx_frame_to_chunks(desc, desc->data_chunks, desc->data_tx_credit,
					  desc->data_rx_credit, &bytes_total, &tx_done);
		if (!bytes_total) {
			oa_tc6_add_empty_chunk(desc);
			bytes_total = OA_CHUNK_SIZE + OA_HEADER_LEN;
		}

		xfer.tx_buf = desc->data_chunks;
		xfer.rx_buf = desc->data_chunks;
		xfer.tx_size = bytes_total;
		xfer.rx_size = bytes_total;

		ret = capi_spi_transceive(desc->comm_desc, &xfer);
		if (ret) {
			memset(desc->data_chunks, 0, bytes_total);

			goto unlock;
		}

		if (tx_done)
			oa_tc6_queue_event(desc, OA_TC6_EVENT_TX);

		ret = oa_tc6_rx_chunk_to_frame(desc, desc->data_chunks,
					       bytes_total / (desc->chunk_size + OA_HEADER_LEN));
		if (ret)
			goto unlock;

		tx_ret = oa_tc6_get_first_tx_frame(desc, &desc->tx_q[0],
						   &frame_buffer);
		if (!tx_ret)
			tx_chunks_avail = frame_buffer->len;
		else
			tx_chunks_avail = 0;

		rx_limit++;

		if (rx_limit > CONFIG_OA_THREAD_RX_LIMIT)
			break;
	} while (desc->data_rx_credit || tx_chunks_avail);

unlock:
	/* The bus is free here: service any STATUS0 read owed by the pass. */
	if (!ret)
		oa_tc6_service_status0(desc);
	events = desc->pending_events;
	desc->pending_events = 0;
	capi_mutex_unlock(desc->ctrl_lock);

	oa_tc6_dispatch_events(desc, events);

	return ret;
}

static int oa_tc6_setup_config0(struct oa_tc6_desc *desc,
				struct oa_tc6_init_param *param)
{
	uint32_t new_reg_val =
		no_os_field_prep(OA_TC6_CONFIG0_CPS_MASK, OA_TC6_CONFIG0_CPS_64);
	bool want_prote;
	int ret;

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

int oa_tc6_register_callback(struct oa_tc6_desc *desc,
			     void (*callback)(struct oa_tc6_desc *, uint32_t, void *),
			     void *arg)
{
	if (!desc)
		return -EINVAL;

	desc->callback = callback;
	desc->callback_arg = arg;

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

/* Wire up the TX queue from the init parameters. */
static int oa_tc6_setup_tx_queues(struct oa_tc6_desc *desc,
				  struct oa_tc6_init_param *param)
{
	struct oa_tc6_queue *q = &desc->tx_q_storage;

	if (!param->tx_pool || !param->tx_pool_size)
		return -EINVAL;

	q->pool = param->tx_pool;
	q->pool_size = param->tx_pool_size;
	oa_tc6_pool_reset(q->pool, q->pool_size);

	desc->tx_q = q;
	desc->tx_q_count = 1;

	return 0;
}

/**
 * @brief Allocate resources for the OA TC6 driver.
 * @param desc - the device descriptor to be initialized
 * @param param - the device's parameter
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_init(struct oa_tc6_desc *descriptor, struct oa_tc6_init_param *param)
{
	int ret;

	if (!descriptor || !param || !param->rx_pool || !param->rx_pool_size)
		return -EINVAL;

	memset(descriptor, 0, sizeof(*descriptor));

	descriptor->rx_pool = param->rx_pool;
	descriptor->rx_pool_size = param->rx_pool_size;
	oa_tc6_pool_reset(descriptor->rx_pool, descriptor->rx_pool_size);

	ret = oa_tc6_setup_tx_queues(descriptor, param);
	if (ret)
		return ret;

	descriptor->chunk_size = OA_CHUNK_SIZE;
	descriptor->comm_desc = param->comm_desc;
	/* The reset request is framed for the protection the part boots in. */
	descriptor->prote_spi = param->prote_spi;

	/*
	 * Without a mutex config (bare-metal single-threaded), each lock is the
	 * shared no-op sentinel. ctrl_lock and data_lock MUST be independent.
	 */
	if (param->ctrl_mutex_cfg) {
		ret = capi_mutex_init(&descriptor->ctrl_lock, param->ctrl_mutex_cfg);
		if (ret)
			return ret;
	} else {
		descriptor->ctrl_lock = &oa_noop_mutex;
	}

	if (param->data_mutex_cfg) {
		ret = capi_mutex_init(&descriptor->data_lock, param->data_mutex_cfg);
		if (ret)
			goto free_ctrl_lock;
	} else {
		descriptor->data_lock = &oa_noop_mutex;
	}

	ret = oa_tc6_sw_reset(descriptor);
	if (ret)
		goto free_lock;

	ret = oa_tc6_setup_config0(descriptor, param);
	if (ret)
		goto free_lock;

	ret = oa_tc6_reg_write(descriptor, OA_TC6_STATUS0_REG, 0xFFFF);
	if (ret)
		goto free_lock;

	descriptor->bufst_polling = param->bufst_polling;

	return 0;

free_lock:
	capi_mutex_deinit(descriptor->data_lock);
free_ctrl_lock:
	capi_mutex_deinit(descriptor->ctrl_lock);

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

	capi_mutex_deinit(desc->data_lock);
	capi_mutex_deinit(desc->ctrl_lock);

	return 0;
}
