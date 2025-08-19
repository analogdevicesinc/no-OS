/*******************************************************************************
 *   @file   oa_tc6.c
 *   @brief  Implementation for the Open Alliance TC6 SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "no_os_alloc.h"
#include "oa_tc6.h"

int oa_rx_chunk_to_frame(struct oa_tc6_desc *desc, uint8_t *chunks,
			 uint32_t len);

/**
 * @brief Compute the CRC value of the header/footer.
 * @param header - value of the header/footer field in a chunk.
 * @return CRC value.
 */
static uint8_t oa_tc6_crc1(uint32_t header)
{
	uint8_t p = 1;

	while (header) {
		p ^= header & 0x1;
		header >>= 1;
	}

	return p;
}

/**
 * @brief Prepare a register read control chunk.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 */
static void oa_tc6_prepare_rx_ctrl(struct oa_tc6_desc *desc, uint32_t addr)
{
	uint32_t header;

	header = no_os_field_prep(OA_CTRL_ADDR_MMS_MASK, addr);
	header |= no_os_field_prep(OA_CTRL_AID_MASK, 1);
	header |= oa_tc6_crc1(header);

	no_os_put_unaligned_be32(header, desc->ctrl_chunks);
	desc->ctrl_rx_credit++;
}

/**
 * @brief Prepare a register write control chunk.
 * @param desc - the OA TC6 descriptor.
 * @param addr - Register address.
 * @param val - Register value.
 */
static void oa_tc6_prepare_tx_ctrl(struct oa_tc6_desc *desc, uint32_t addr,
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
	uint32_t comp_val;
	int ret;

	if (!desc)
		return -ENODEV;

	oa_tc6_prepare_rx_ctrl(desc, addr);
	ret = oa_tc6_thread(desc);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(&desc->ctrl_chunks[2 * OA_HEADER_LEN]);
	desc->ctrl_rx_credit = 0;

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
int oa_tc6_reg_write(struct oa_tc6_desc *desc, uint32_t addr, uint32_t val)
{
	int ret;

	if (!desc)
		return -ENODEV;

	oa_tc6_prepare_tx_ctrl(desc, addr, val);
	ret = oa_tc6_thread(desc);
	if (ret)
		return ret;

	desc->ctrl_tx_credit = 0;

	return 0;
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
	uint32_t reg_val;
	int ret;

	ret = oa_tc6_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= val & mask;

	return oa_tc6_reg_write(desc, addr, reg_val);
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
	if (!desc)
		return -EINVAL;

	for (int i = 0; i < OA_TX_FRAME_BUFF_NUM; i++) {
		if (desc->user_tx_frame_buffer[i].state == OA_BUFF_FREE) {
			*buffer = &desc->user_tx_frame_buffer[i];
			desc->user_tx_frame_buffer[i].state = OA_BUFF_TX_BUSY;
			memset(desc->user_tx_frame_buffer[i].data, 0,
			       CONFIG_OA_CHUNK_BUFFER_SIZE);

			return 0;
		}
	}

	return -ENOBUFS;
}

/**
 * @brief Mark a frame buffer as ready to be transmitted.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer containing the frame to be transmitted.
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_put_tx_frame(struct oa_tc6_desc *desc,
			struct oa_tc6_frame_buffer *buffer)
{
	if (!desc || !buffer)
		return -EINVAL;

	buffer->state = OA_BUFF_TX_READY;

	return 0;
}

/**
 * @brief Get the first TX frame buffer that is ready to be transmitted.
 * @param desc - the OA TC6 descriptor.
 * @param buffer - buffer to be filled with the frame to be transmitted.
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_get_first_tx_frame(struct oa_tc6_desc *desc,
				     struct oa_tc6_frame_buffer **buffer)
{
	for (int i = 0; i < OA_TX_FRAME_BUFF_NUM; i++) {
		if (desc->user_tx_frame_buffer[i].state == OA_BUFF_TX_READY) {
			*buffer = &desc->user_tx_frame_buffer[i];

			return 0;
		}
	}

	return -ENOENT;
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
	if (!new_buffer) {
		for (int i = 0; i < OA_RX_FRAME_BUFF_NUM; i++) {
			if (desc->user_rx_frame_buffer[i].state == OA_BUFF_RX_IN_PROGRESS) {
				*buffer = &desc->user_rx_frame_buffer[i];

				return 0;
			}
		}
	}

	for (int i = 0; i < OA_RX_FRAME_BUFF_NUM; i++) {
		if (desc->user_rx_frame_buffer[i].state == OA_BUFF_FREE) {
			*buffer = &desc->user_rx_frame_buffer[i];
			desc->user_rx_frame_buffer[i].index = 0;
			desc->user_rx_frame_buffer[i].len = 0;

			return 0;
		}
	}

	return -ENOBUFS;
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
	for (int i = 0; i < OA_RX_FRAME_BUFF_NUM; i++) {
		if (desc->user_rx_frame_buffer[i].state == OA_BUFF_RX_COMPLETE &&
		    (vs & mask) == (desc->user_rx_frame_buffer[i].vs & mask)) {
			*buffer = &desc->user_rx_frame_buffer[i];
			desc->user_rx_frame_buffer[i].state = OA_BUFF_RX_USER_OWNED;

			return 0;
		}
	}

	return -ENOENT;
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
	for (int i = 0; i < OA_RX_FRAME_BUFF_NUM; i++) {
		if (desc->user_rx_frame_buffer[i].state == OA_BUFF_RX_COMPLETE) {
			*buffer = &desc->user_rx_frame_buffer[i];
			desc->user_rx_frame_buffer[i].state = OA_BUFF_RX_USER_OWNED;

			return 0;
		}
	}

	return -ENOENT;
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

	return 0;
}

/**
 * @brief Convert frames in the OA_BUFF_TX_READY state to chunks.
 * Configure empty chunks if we need to receive more then transmit.
 * @param desc - the OA TC6 descriptor
 * @param tx_buffer - the buffer containing the chunks
 * @param tx_credit - the number of chunks available for transmission
 * @param rx_nchunks - the number of chunks available for reception
 * @param tx_written - the number of chunks written in the buffer
 * @return 0 in case of success, negative error code otherwise
 */
static int oa_tc6_tx_frame_to_chunks(struct oa_tc6_desc *desc,
				     uint8_t *tx_buffer,
				     uint32_t tx_credit, uint32_t rx_nchunks,
				     uint32_t *tx_written)
{
	uint32_t spi_buffer_index = 0;
	uint32_t tx_frame_num_chunks;
	uint32_t spi_buff_max_chunks;
	uint32_t chunks_written = 0;
	uint32_t frame_offset = 0;
	uint32_t chunks_limit;
	uint32_t frame_len;
	uint32_t header;
	uint32_t i;
	int ret;

	struct oa_tc6_frame_buffer *frame_buffer;
	spi_buff_max_chunks = OA_SPI_BUFF_LEN / (OA_CHUNK_SIZE + OA_HEADER_LEN);

	/* The maximum number of chunks we can potentially send, given the size of our SPI buffer. */
	chunks_limit = no_os_min(spi_buff_max_chunks, tx_credit);

	do {
		ret = oa_tc6_get_first_tx_frame(desc, &frame_buffer);
		if (ret)
			break;

		frame_len = frame_buffer->len;
		tx_frame_num_chunks = NO_OS_DIV_ROUND_UP(frame_len, OA_CHUNK_SIZE);

		/* Check if we can fit the current frame into the SPI buffer (as a whole). */
		if (!frame_len || ((chunks_written + tx_frame_num_chunks) > chunks_limit))
			break;

		frame_offset = 0;
		for (i = 0; i < tx_frame_num_chunks; i++) {
			header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
			header |= no_os_field_prep(OA_DATA_HEADER_DV_MASK, 1);
			header |= no_os_field_prep(OA_DATA_HEADER_VS_MASK, frame_buffer->vs);

			if (!i)
				header |= no_os_field_prep(OA_DATA_FOOTER_SV_MASK, 1);

			if (i == tx_frame_num_chunks - 1) {
				header |= no_os_field_prep(OA_DATA_HEADER_EV_MASK, 1);
				header |= no_os_field_prep(OA_DATA_HEADER_EBO_MASK, frame_len - 1);
			}

			header |= oa_tc6_crc1(header);

			no_os_put_unaligned_be32(header, &tx_buffer[spi_buffer_index]);
			spi_buffer_index += OA_HEADER_LEN;
			memcpy(&tx_buffer[spi_buffer_index], &frame_buffer->data[frame_offset],
			       OA_CHUNK_SIZE);
			frame_offset += OA_CHUNK_SIZE;
			spi_buffer_index += OA_CHUNK_SIZE;

			frame_len -= OA_CHUNK_SIZE;
		}
		chunks_written += tx_frame_num_chunks;

		frame_buffer->len = 0;
		frame_buffer->index = 0;
		frame_buffer->state = OA_BUFF_FREE;
	} while (1);

	/*
	 * The TX queue may be empty, there is no space in the SPI buffer,
	 * or we're out of tx credits.
	 * If rx_chunks > tx_chunks, we need to add dummy chunks (DV = 0) as long
	 * as there is enough room in the buffer.
	 */
	while ((rx_nchunks > chunks_written) && (chunks_written < chunks_limit)) {
		header = no_os_field_prep(OA_DATA_HEADER_DNC_MASK, 1);
		no_os_put_unaligned_be32(header, &tx_buffer[spi_buffer_index]);
		spi_buffer_index += OA_CHUNK_SIZE + OA_HEADER_LEN;
		chunks_written++;
	}

	*tx_written = spi_buffer_index;

	return 0;
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

	struct oa_tc6_frame_buffer *frame_buffer;

	ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, false);
	if (ret)
		return ret;

	for (uint32_t i = 0; i < len; i++) {
		footer = no_os_get_unaligned_be32(&chunks[OA_CHUNK_SIZE]);

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
		desc->xfer_flags.exst |= !!(footer & OA_DATA_FOOTER_EXST_MASK); /* Latched */
		desc->xfer_flags.hdrb |= !!(footer & OA_DATA_FOOTER_HDRB_MASK); /* Latched */
		desc->xfer_flags.sync = !!(footer &
					   OA_DATA_FOOTER_SYNC_MASK);  /* Instantaneous */

		if (!(footer & OA_DATA_FOOTER_DV_MASK)) {
			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (sv && ev) {
			if (sbo > ebo) {
				/* There are 2 frames in the current chunk. Finish the existing. */
				memcpy(&(frame_buffer->data[frame_buffer->index]), chunks, ebo + 1);
				frame_buffer->index += ebo + 1;
				frame_buffer->len = frame_buffer->index;
				frame_buffer->state = OA_BUFF_RX_COMPLETE;

				/* Flags valid when EV=1 Only */
				frame_buffer->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
				frame_buffer->rtsa = !!(footer & OA_DATA_FOOTER_RTSA_MASK);
				frame_buffer->rtsp = !!(footer & OA_DATA_FOOTER_RTSP_MASK);

				/* Now get a new buffer for the second frame */
				ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
				if (ret)
					return ret;

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
			}

			/*
			 * At this point it is either a single frame encapsulated in the
			 * chunk, or the start of the 2nd Frame. EBO should be set
			 * accordingly above.
			 */
			memcpy(&frame_buffer->data[frame_buffer->index], &chunks[sbo],
			       ebo - sbo + 1);
			frame_buffer->index += ebo - sbo + 1;
			frame_buffer->len = frame_buffer->index;
			frame_buffer->vs = no_os_field_get(OA_DATA_FOOTER_VS_MASK, footer);

			if (frame_buffer->state == OA_BUFF_RX_COMPLETE) {
				/* Get a new buffer for the next iteration */
				ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
				if (ret)
					return ret;
			}

			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (ev) {
			ebo = no_os_field_get(OA_DATA_FOOTER_EBO_MASK, footer);

			/* The current chunk may be shorter than chunk_size, since it contains the end of a frame */
			memcpy(&(frame_buffer->data[frame_buffer->index]), chunks, ebo + 1);
			frame_buffer->len = frame_buffer->index + ebo + 1;
			frame_buffer->state = OA_BUFF_RX_COMPLETE;

			/* Flags valid when EV=1 Only */
			frame_buffer->frame_drop = !!(footer & OA_DATA_FOOTER_FD_MASK);
			frame_buffer->rtsa = !!(footer & OA_DATA_FOOTER_RTSA_MASK);
			frame_buffer->rtsp = !!(footer & OA_DATA_FOOTER_RTSP_MASK);

			/* Get a new buffer for the next iteration */
			ret = oa_tc6_get_empty_rx_buff(desc, &frame_buffer, true);
			if (ret)
				return ret;

			frame_buffer->state = OA_BUFF_RX_IN_PROGRESS;

			chunks += OA_CHUNK_SIZE + OA_FOOTER_LEN;
			continue;
		}

		if (sv) {
			/* The current chunk contains the start of a frame at offset SWO */
			memcpy(&frame_buffer->data[frame_buffer->index], &chunks[sbo],
			       OA_CHUNK_SIZE - sbo);

			frame_buffer->index += OA_CHUNK_SIZE - sbo;
			frame_buffer->len = frame_buffer->index;
			frame_buffer->state = OA_BUFF_RX_IN_PROGRESS;
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

	desc->data_rx_credit = no_os_field_get(OA_DATA_FOOTER_RCA_MASK, footer);
	desc->data_tx_credit = no_os_field_get(OA_DATA_FOOTER_TXC_MASK, footer);

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
	int ret;

	struct oa_tc6_frame_buffer *frame_buffer;
	struct no_os_spi_msg xfer = {0};

	if (desc->ctrl_rx_credit || desc->ctrl_tx_credit) {
		xfer.tx_buff = desc->ctrl_chunks;
		xfer.rx_buff = desc->ctrl_chunks;
		xfer.cs_change = 1;

		if (desc->prote_spi)
			xfer.bytes_number = 2 * (OA_HEADER_LEN + OA_REG_LEN);
		else
			xfer.bytes_number = 2 * OA_HEADER_LEN + OA_REG_LEN;

		return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	}

	ret = oa_tc6_update_stats(desc);
	if (ret)
		return ret;

	if (desc->data_tx_credit) {
		ret = oa_tc6_get_first_tx_frame(desc, &frame_buffer);
		if (!ret)
			tx_chunks_avail = frame_buffer->len;
	}

	while (desc->data_rx_credit || tx_chunks_avail) {
		oa_tc6_tx_frame_to_chunks(desc, desc->data_chunks, desc->data_tx_credit,
					  desc->data_rx_credit, &bytes_total);

		xfer.tx_buff = desc->data_chunks;
		xfer.rx_buff = desc->data_chunks;
		xfer.bytes_number = bytes_total;

		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret) {
			memset(desc->data_chunks, 0, bytes_total);

			return ret;
		}

		ret = oa_tc6_rx_chunk_to_frame(desc, desc->data_chunks,
					       bytes_total / (OA_CHUNK_SIZE + OA_HEADER_LEN));
		if (ret)
			return ret;

		ret = oa_tc6_get_first_tx_frame(desc, &frame_buffer);
		if (!ret)
			tx_chunks_avail = frame_buffer->len;
		else
			tx_chunks_avail = 0;

		rx_limit++;

		if (rx_limit > CONFIG_OA_THREAD_RX_LIMIT)
			break;
	}

	return 0;
}

/**
 * @brief Allocate resources for the OA TC6 driver.
 * @param desc - the device descriptor to be initialized
 * @param param - the device's parameter
 * @return 0 in case of success, negative error code otherwise
 */
int oa_tc6_init(struct oa_tc6_desc **desc, struct oa_tc6_init_param *param)
{
	struct oa_tc6_desc *descriptor;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->comm_desc = param->comm_desc;
	descriptor->prote_spi = param->prote_spi;

#if CONFIG_OA_ZERO_SWO_ONLY
	/* For now, we'll only support receiving frames with SWO = 0 */
	int ret = oa_tc6_reg_update(descriptor, OA_TC6_CONFIG0_REG,
				    OA_TC6_CONFIG0_ZARFE_MASK,
				    OA_TC6_CONFIG0_ZARFE_MASK);
	if (ret) {
		no_os_free(descriptor);

		return ret;
	}
#endif

	*desc = descriptor;

	return 0;
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

	no_os_free(desc);

	return 0;
}
