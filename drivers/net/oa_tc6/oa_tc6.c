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

int oa_rx_chunk_to_frame(struct oa_tc6_desc *desc, uint8_t *chunks,
			 uint32_t len);

/**
 * @brief Compute the CRC value of the header/footer.
 * @param header - value of the header/footer field in a chunk.
 * @return CRC value.
 */
static uint8_t oa_tc6_crc1(uint32_t header)
{
	header ^= header >> 16;
	header ^= header >> 8;
	header ^= header >> 4;
	header ^= header >> 2;
	header ^= header >> 1;

	return 1 ^ (header & 1);
}

/**
 * @brief Prepare a register read control chunk.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @return the control header.
 */
static uint32_t oa_tc6_prepare_rx_ctrl(struct oa_tc6_desc *desc, uint32_t addr)
{
	uint32_t header;

	header = no_os_field_prep(OA_CTRL_ADDR_MMS_MASK, addr);
	header |= no_os_field_prep(OA_CTRL_AID_MASK, 1);
	header |= oa_tc6_crc1(header);

	no_os_put_unaligned_be32(header, desc->ctrl_chunks);
	desc->ctrl_rx_credit++;

	return header;
}

/**
 * @brief Prepare a register write control chunk.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @param val - Register value.
 * @return the control header.
 */
static uint32_t oa_tc6_prepare_tx_ctrl(struct oa_tc6_desc *desc, uint32_t addr,
				       uint32_t val)
{
	uint32_t header;

	header = no_os_field_prep(OA_CTRL_ADDR_MMS_MASK, addr);
	header |= no_os_field_prep(OA_CTRL_AID_MASK, 1);
	header |= no_os_field_prep(OA_CTRL_WNR_MASK, 1);
	header |= oa_tc6_crc1(header);

	no_os_put_unaligned_be32(header, desc->ctrl_chunks);
	no_os_put_unaligned_be32(val, &desc->ctrl_chunks[OA_HEADER_LEN]);

	if (desc->prote_spi) {
		no_os_put_unaligned_be32(val ^ NO_OS_GENMASK(31, 0),
					 &desc->ctrl_chunks[OA_HEADER_LEN + OA_REG_LEN]);
	}

	desc->ctrl_tx_credit++;

	return header;
}

static int oa_tc6_do_ctrl_transfer(struct oa_tc6_desc *desc)
{
	struct capi_spi_transfer xfer = {0};

	if (desc->ctrl_rx_credit || desc->ctrl_tx_credit) {
		xfer.tx_buf = desc->ctrl_chunks;
		xfer.rx_buf = desc->ctrl_chunks;

		if (desc->prote_spi)
			xfer.tx_size = 2 * (OA_HEADER_LEN + OA_REG_LEN);
		else
			xfer.tx_size = 2 * OA_HEADER_LEN + OA_REG_LEN;
		xfer.rx_size = xfer.tx_size;

		return capi_spi_transceive(desc->comm_desc, &xfer);
	}

	return 0;
}

/*
 * The MAC-PHY echoes the control header one word later. A mismatch means the
 * header was rejected (HDRB) or the control transfer is corrupted.
 */
static bool oa_tc6_ctrl_echo_ok(struct oa_tc6_desc *desc, uint32_t header)
{
	return no_os_get_unaligned_be32(&desc->ctrl_chunks[OA_HEADER_LEN]) == header;
}

static int __oa_tc6_reg_read(struct oa_tc6_desc *desc, uint32_t addr,
			     uint32_t *val)
{
	uint32_t comp_val;
	uint32_t header;
	int ret;

	if (!desc)
		return -ENODEV;

	header = oa_tc6_prepare_rx_ctrl(desc, addr);
	ret = oa_tc6_do_ctrl_transfer(desc);
	desc->ctrl_rx_credit = 0;
	if (ret)
		return ret;

	if (!oa_tc6_ctrl_echo_ok(desc, header))
		return -EIO;

	*val = no_os_get_unaligned_be32(&desc->ctrl_chunks[2 * OA_HEADER_LEN]);

	if (desc->prote_spi) {
		comp_val = no_os_get_unaligned_be32(&desc->ctrl_chunks[3 * OA_HEADER_LEN]);
		if (*val != (comp_val ^ NO_OS_GENMASK(31, 0)))
			return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write a register value.
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

static int __oa_tc6_reg_write(struct oa_tc6_desc *desc, uint32_t addr,
			      uint32_t val)
{
	uint32_t header;
	uint8_t *echo;
	int ret;

	if (!desc)
		return -ENODEV;

	echo = &desc->ctrl_chunks[2 * OA_HEADER_LEN];
	header = oa_tc6_prepare_tx_ctrl(desc, addr, val);
	ret = oa_tc6_do_ctrl_transfer(desc);
	desc->ctrl_tx_credit = 0;
	if (ret)
		return ret;

	if (!oa_tc6_ctrl_echo_ok(desc, header) ||
	    no_os_get_unaligned_be32(echo) != val)
		return -EIO;

	if (desc->prote_spi &&
	    no_os_get_unaligned_be32(&echo[OA_REG_LEN]) != (val ^ NO_OS_GENMASK(31, 0)))
		return -EIO;

	return 0;
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

int __oa_tc6_reg_update(struct oa_tc6_desc *desc, uint32_t addr,
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
int oa_tc6_reg_update(struct oa_tc6_desc *desc, uint32_t addr,
		      uint32_t val, uint32_t mask)
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
			desc->stats.rx_drop_nobuf++;
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

/**
 * @brief Find the last footer with a valid parity in a transfer.
 * @param chunks - array containing chunks received from the MAC
 * @param len - the number of chunks
 * @param footer - the footer found
 * @return true if a valid footer was found, false otherwise
 */
static bool oa_tc6_last_valid_footer(uint8_t *chunks, uint32_t len,
				     uint32_t *footer)
{
	while (len--) {
		*footer = no_os_get_unaligned_be32(&chunks[len * (OA_CHUNK_SIZE +
						   OA_FOOTER_LEN) + OA_CHUNK_SIZE]);
		if (!oa_tc6_crc1(*footer))
			return true;
	}

	return false;
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
	uint32_t footer = 0;
	uint32_t sbo;
	uint32_t ebo;
	uint32_t ev;
	uint32_t sv;
	int ret;

	struct oa_tc6_frame_buffer *frame_buffer = NULL;
	uint8_t *start = chunks;

	ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, false);
	if (ret)
		goto out;

	for (uint32_t i = 0; i < len; i++) {
		footer = no_os_get_unaligned_be32(&chunks[OA_CHUNK_SIZE]);

		/* None of the footer fields can be trusted on a parity error */
		if (oa_tc6_crc1(footer)) {
			desc->stats.rx_footer_parity_err++;
			if (frame_buffer->state == OA_BUFF_RX_IN_PROGRESS) {
				frame_buffer->index = 0;
				frame_buffer->len = 0;
				frame_buffer->state = OA_BUFF_FREE;
			}

			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		ev = footer & OA_DATA_FOOTER_EV_MASK;
		sv = footer & OA_DATA_FOOTER_SV_MASK;
		ebo = no_os_field_get(OA_DATA_FOOTER_EBO_MASK, footer);

		/*
		 * The footer contains an SWO (Start WORD offset). Convert to a start
		 * BYTE offset to be used directly in buffer access and math
		 */
		sbo = no_os_field_get(OA_DATA_FOOTER_SWO_MASK, footer) * 4;

		/* Always update the transfer flags, even if DV=0 */
		desc->xfer_flags.flags_valid = true;
		if (footer & OA_DATA_FOOTER_EXST_MASK) {
			desc->xfer_flags.exst = 1; /* Latched */
			desc->stats.exst_events++;
			ret = oa_tc6_handle_exst(desc);
			if (ret)
				goto out;
		}
		desc->xfer_flags.hdrb |= !!(footer & OA_DATA_FOOTER_HDRB_MASK); /* Latched */
		desc->xfer_flags.sync = !!(footer &
					   OA_DATA_FOOTER_SYNC_MASK);  /* Instantaneous */

		if (!(footer & OA_DATA_FOOTER_DV_MASK)) {
			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (sv && ev) {
			if (sbo > ebo) {
				/*
				 * There are 2 frames in the current chunk. Finish the existing
				 * one, if it was started.
				 */
				if (frame_buffer->state == OA_BUFF_RX_IN_PROGRESS) {
					memcpy(&frame_buffer->data[frame_buffer->index], chunks,
					       ebo + 1);
					frame_buffer->index += ebo + 1;
					frame_buffer->len = frame_buffer->index;
					frame_buffer->state = OA_BUFF_RX_COMPLETE;

					desc->stats.rx_frames++;
					desc->stats.rx_bytes += frame_buffer->len;

					/* Flags valid when EV=1 Only */
					frame_buffer->frame_drop = !!(footer &
								      OA_DATA_FOOTER_FD_MASK);
					if (frame_buffer->frame_drop)
						desc->stats.rx_drop_fd++;

					oa_tc6_queue_event(desc, OA_TC6_EVENT_RX);

					/* Now get a new buffer for the second frame */
					ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
					if (ret)
						goto out;
				}

				/*
				 * Overwrite the EBO to be the end of the chunk so the next
				 * block of code is generic
				 */
				ebo = OA_CHUNK_SIZE - 1;

				/* Next frame will be in progress */
				frame_buffer->state = OA_BUFF_RX_IN_PROGRESS;
			} else {
				/* A single frame in current chunk. It will be completed */
				frame_buffer->state = OA_BUFF_RX_COMPLETE;

				frame_buffer->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
				if (frame_buffer->frame_drop)
					desc->stats.rx_drop_fd++;

				oa_tc6_queue_event(desc, OA_TC6_EVENT_RX);
			}

			/*
			 * At this point it is either a single frame encapsulated in the
			 * chunk, or the start of the 2nd Frame. EBO should be set
			 * accordingly above. A start of frame drops any frame left open.
			 */
			memcpy(frame_buffer->data, &chunks[sbo], ebo - sbo + 1);
			frame_buffer->index = ebo - sbo + 1;
			frame_buffer->len = frame_buffer->index;

			//Flags valid when SV=1
			frame_buffer->rtsa = !!(footer & OA_DATA_FOOTER_RTSA_MASK);
			frame_buffer->rtsp = !!(footer & OA_DATA_FOOTER_RTSP_MASK);

			frame_buffer->vs = no_os_field_get(OA_DATA_FOOTER_VS_MASK, footer);

			if (frame_buffer->state == OA_BUFF_RX_COMPLETE) {
				desc->stats.rx_frames++;
				desc->stats.rx_bytes += frame_buffer->len;

				/* Get a new buffer for the next iteration */
				ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
				if (ret)
					goto out;
			}

			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (ev) {
			/* Drop the end of a frame that was not started */
			if (frame_buffer->state != OA_BUFF_RX_IN_PROGRESS) {
				chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
				continue;
			}

			ebo = no_os_field_get(OA_DATA_FOOTER_EBO_MASK, footer);

			/* The current chunk may be shorter than chunk_size, since it contains the end of a frame */
			memcpy(&(frame_buffer->data[frame_buffer->index]), chunks, ebo + 1);
			frame_buffer->len = frame_buffer->index + ebo + 1;
			frame_buffer->state = OA_BUFF_RX_COMPLETE;

			desc->stats.rx_frames++;
			desc->stats.rx_bytes += frame_buffer->len;

			oa_tc6_queue_event(desc, OA_TC6_EVENT_RX);

			/* Flags valid when EV=1 Only */
			frame_buffer->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
			if (frame_buffer->frame_drop)
				desc->stats.rx_drop_fd++;

			/* Get a new buffer for the next iteration */
			ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
			if (ret)
				goto out;

			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (sv) {
			/*
			 * The current chunk contains the start of a frame at offset SWO.
			 * It drops any frame left open.
			 */
			memcpy(frame_buffer->data, &chunks[sbo], OA_CHUNK_SIZE - sbo);

			frame_buffer->index = OA_CHUNK_SIZE - sbo;
			frame_buffer->len = frame_buffer->index;
			frame_buffer->state = OA_BUFF_RX_IN_PROGRESS;

			//Flags valid when SV=1
			frame_buffer->rtsa = !!(footer & OA_DATA_FOOTER_RTSA_MASK);
			frame_buffer->rtsp = !!(footer & OA_DATA_FOOTER_RTSP_MASK);

			frame_buffer->vs = no_os_field_get(OA_DATA_FOOTER_VS_MASK, footer);
			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if ((!ev && !sv) && (frame_buffer->state == OA_BUFF_RX_IN_PROGRESS)) {
			/*
			 * The current chunk does not contain either the start or end of a frame,
			 * and the contains valid data for each byte
			 */
			memcpy(&frame_buffer->data[frame_buffer->index], chunks, OA_CHUNK_SIZE);
			frame_buffer->index += OA_CHUNK_SIZE;
			frame_buffer->len = frame_buffer->index;
		}

		chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
	}

out:
	/* The credits reported by the last footer replace the earlier ones */
	if (oa_tc6_last_valid_footer(start, len, &footer)) {
		desc->data_rx_credit = no_os_field_get(OA_DATA_FOOTER_RCA_MASK, footer);
		desc->data_tx_credit = no_os_field_get(OA_DATA_FOOTER_TXC_MASK, footer);
	}

	return ret;
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
					       bytes_total / (OA_CHUNK_SIZE + OA_HEADER_LEN));
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
	events = desc->pending_events;
	desc->pending_events = 0;
	capi_mutex_unlock(desc->ctrl_lock);

	oa_tc6_dispatch_events(desc, events);

	return ret;
}

/**
 * @brief Trigger a soft reset of the MAC-PHY via the OA-TC6 RESET register
 *        and wait for completion. Mirrors the linux oa_tc6_sw_reset_macphy:
 *        write SWRESET, poll STATUS0.RESETC every 1ms up to 1s, then RW1C
 *        clear the reset-complete status.
 * @param desc - the OA TC6 descriptor.
 * @return 0 in case of success, -ENODEV on timeout, negative error otherwise.
 */
int oa_tc6_sw_reset(struct oa_tc6_desc *desc)
{
	uint32_t val;
	int ret;
	int i;

	ret = oa_tc6_reg_write(desc, OA_TC6_RESET_REG, OA_TC6_RESET_SWRESET);
	if (ret)
		return ret;

	for (i = 0; i < 1000; i++) {
		/* The control echo is not valid until the MAC-PHY is out of reset */
		ret = oa_tc6_reg_read(desc, OA_TC6_STATUS0_REG, &val);
		if (!ret && (val & OA_TC6_STATUS0_RESETC))
			break;
		capi_wait_ms(1);
	}
	if (i == 1000)
		return -ENODEV;

	return oa_tc6_reg_write(desc, OA_TC6_STATUS0_REG, OA_TC6_STATUS0_RESETC);
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

	descriptor->comm_desc = param->comm_desc;
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

#if CONFIG_OA_ZERO_SWO_ONLY
	/* For now, we'll only support receiving frames with SWO = 0 */
	ret = oa_tc6_reg_update(descriptor, OA_TC6_CONFIG0_REG,
				OA_TC6_CONFIG0_ZARFE_MASK,
				OA_TC6_CONFIG0_ZARFE_MASK);
	if (ret)
		goto free_lock;
#endif

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
