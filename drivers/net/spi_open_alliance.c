#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "adin1110/adin1110.h"
#include "no_os_irq.h"
#include "adin1110.h"
#include "spi_open_alliance.h"

#include "maxim_irq.h"
#include "maxim_gpio_irq.h"

static uint8_t open_alliance_crc1(uint32_t header)
{
	uint8_t p = 1;
	int i;

	while (header) {
		p ^= header & 0x1;
		header >>= 1;
	}

	return p;
}

int oa_reg_read(struct adin1110_desc *adin1110, uint32_t addr, uint32_t *val)
{
	struct oa_tx_ctrl_chunk tx_chunk;
	struct oa_rx_ctrl_chunk rx_chunk;
	uint32_t header;
	uint8_t txb[10];
	int ret;

	struct no_os_spi_msg xfer = {
		.tx_buff = adin1110->data,
		.rx_buff = adin1110->data,
		.cs_change = 1,
	};

	if (!adin1110)
		return -ENODEV;

	tx_chunk.header.len = 0;
	tx_chunk.header.hdrb = 0;
	tx_chunk.header.addr = addr;
	tx_chunk.header.wnr = 0;
	tx_chunk.header.mms = 0;
	tx_chunk.header.aid = 1;
	tx_chunk.header.p = 0;

	header = addr << 8;
	header |= 1 << 28;

	if (addr >= 0x30)
		header |= 1 << 24;

	header |= open_alliance_crc1(header);

	xfer.bytes_number = 2 * OA_HEADER_LEN + OA_REG_LEN;
	no_os_put_unaligned_be32(header, adin1110->data);
	ret = no_os_spi_transfer(adin1110->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be32(&adin1110->data[2 * OA_HEADER_LEN]);

	return ret;
}

int oa_reg_write(struct adin1110_desc *adin1110, uint32_t addr, uint32_t val)
{
	uint32_t header;
	struct no_os_spi_msg xfer = {
		.tx_buff = adin1110->data,
		.rx_buff = adin1110->data,
		.cs_change = 1,
	};
	int ret;

	if (!adin1110)
		return -ENODEV;

	header = addr << 8;
	header |= 1 << 28;
	header |= 1 << 29;

	if (addr >= 0x30)
		header |= 1 << 24;

	header |= open_alliance_crc1(header);

	xfer.bytes_number = 2 * OA_HEADER_LEN + OA_REG_LEN;
	no_os_put_unaligned_be32(header, adin1110->data);
	no_os_put_unaligned_be32(val, &adin1110->data[OA_HEADER_LEN]);

	return no_os_spi_transfer(adin1110->comm_desc, &xfer, 1);
}

int oa_irq_start(struct adin1110_desc *desc)
{
	return no_os_irq_enable(desc->gpio_irq, desc->int_gpio->number);
}

int oa_irq_stop(struct adin1110_desc *desc)
{
	return no_os_irq_disable(desc->gpio_irq, desc->int_gpio->number);
}

void oa_format_header(struct adin1110 *desc, uint8_t *header)
{
	uint32_t header_val = 0;

	header_val |= 1 << 31;
	header_val |= open_alliance_crc1(header_val);

	no_os_put_unaligned_be32(header_val, header);

	return 0;
}

uint32_t oa_queue_first_len(struct adin1110_desc *desc, struct oa_list_element *queue)
{
	if (!queue)
		return 0;

	return queue->length;
}

uint32_t oa_queue_tot_len(struct adin1110_desc *desc, struct oa_list_element *queue)
{
	uint32_t len = 0;

	if (!queue)
		return 0;

	while (queue) {
		len += queue->length;
		queue = queue->next;
	}

	return len;
}

int oa_queue_length(struct adin1110_desc *desc)
{
	struct oa_list_element *iter;
	int len = 0;

	iter = desc->rx_queue;

	while (iter) {
		len++;
		iter = iter->next;
	}

	return len;
}

int oa_get_chunk_size(struct adin1110_desc *desc, uint32_t *chunk_size)
{
	int ret;

	ret = oa_reg_read(desc, ADIN1110_CONFIG1_REG, chunk_size);
	if (ret)
		return ret;

	*chunk_size = no_os_field_get(NO_OS_GENMASK(2, 0), *chunk_size);
	*chunk_size = NO_OS_BIT(*chunk_size);

	return 0;
}

int oa_frame_len_to_chunks(struct adin1110_desc *desc, uint32_t len, uint32_t *chunks)
{
	uint32_t chunk_size;
	int ret;

	ret = oa_get_chunk_size(desc, &chunk_size);
	if (ret)
		return ret;

	*chunks = NO_OS_DIV_ROUND_UP(len, (chunk_size + OA_HEADER_LEN));

	return 0;
}

int oa_tx_frame_to_chunks(struct adin1110_desc *desc, uint8_t *tx_buffer,
			  uint32_t tx_credit, uint32_t rx_nchunks, uint32_t *tx_written)
{
	union oa_data_header header;
	uint32_t bytes_number = 0;
	uint32_t spi_buffer_index = 0;
	uint32_t tx_frame_num_chunks;
	uint32_t tx_queue_num_chunks;
	uint32_t spi_buff_max_chunks;
	uint32_t current_chunk_len;
	uint32_t chunks_limit;
	uint8_t *frame_data;
	uint32_t frame_offset = 0;
	uint32_t chunk_size;
	uint32_t chunks_written = 0;
	uint32_t frame_len;
	uint32_t i;
	int ret;

	/* Use the oa_spi_buffer instead */
	memset(tx_buffer, 0, OA_SPI_BUFF_LEN);

	/*
	 * Read frames from the TX FIFO and add them to tx_buffer in chunk format.
	 */
	ret = oa_get_chunk_size(desc, &chunk_size);
	if (ret)
		return ret;

	ret = oa_frame_len_to_chunks(desc, OA_SPI_BUFF_LEN, &spi_buff_max_chunks);
	if (ret)
		return ret;

	/* The maximum number of chunks we can potentially send. */
	chunks_limit = no_os_min(spi_buff_max_chunks, tx_credit);

	do {
		frame_len = oa_queue_first_len(desc, desc->tx_queue);
		tx_frame_num_chunks = NO_OS_DIV_ROUND_UP(frame_len, (chunk_size + OA_HEADER_LEN));

		/* Check if we can fit the current frame into the SPI buffer (as a whole). */
		if (!frame_len || chunks_written + tx_frame_num_chunks > chunks_limit)
			break;

		/* The frame is short enough to be transmitted in the current SPI transaction */
		ret = oa_dequeue(desc, &desc->tx_queue, frame_data, frame_len);
		if (ret)
			return ret;

		frame_offset = 0;
		header.val = 0;
		header.bits.dnc = 1;
		header.bits.dv = 1;
		for (i = 0; i < tx_frame_num_chunks; i++) {
			if (!i)
				header.bits.sv = 1;
			else
				header.bits.sv = 0;

			/* On 64 bytes received frames, the SV bit is not set (only EV), so this might be an issue */
			if (i == tx_frame_num_chunks - 1)
				header.bits.ev = 1;
			else
				header.bits.ev = 0;

			if (chunk_size > frame_len) {
				chunk_size = frame_data;
				header.bits.ebo = chunk_size;
			}
			no_os_put_unaligned_be32(header.val, &tx_buffer[spi_buffer_index]);
			spi_buffer_index += OA_HEADER_LEN;
			memcpy(&tx_buffer[spi_buffer_index], &frame_data[frame_offset], chunk_size);
			frame_offset += chunk_size + OA_HEADER_LEN;
			spi_buffer_index += chunk_size;
		}
		chunks_written += tx_frame_num_chunks;
		no_os_free(frame_data);
 
	} while (1);

	/* 
	 * The TX queue may be empty, there is no space in the SPI buffer, or we're out of tx credits.
	 * If rx_chunks > tx_chunks, we need to add dummy chunks (DV = 0). 
	 */
	header.val = 0;
	header.bits.dnc = 1;
	header.bits.dv = 0;
	for (i = 0; i < rx_nchunks - chunks_written; i++) {
		no_os_put_unaligned_be32(header.val, &tx_buffer[spi_buffer_index]);
		spi_buffer_index += chunk_size + OA_HEADER_LEN;
	}

	*tx_written = spi_buffer_index;

	return 0;
}

int oa_format_tx_chunks(struct adin1110_desc *desc, uint8_t *tx_buffer, uint32_t len)
{
	uint32_t chunk_size;
	uint32_t reg_val;
	int ret;
	int i;

	ret = oa_get_chunk_size(desc, &chunk_size);
	if (ret)
		return ret;

	for (i = 0; i < len; i++) {
		oa_format_header(desc, tx_buffer);
		tx_buffer += chunk_size + 4;
	}

	return 0;
}

int oa_rx_chunk_to_frame(struct adin1110_desc *desc, uint8_t *chunks, uint32_t len)
{
	union oa_data_footer footer;
	uint32_t frame_offset = 0;
	uint32_t frame_size = 0;
	uint32_t footer_index;
	uint32_t chunk_size;
	uint32_t val;
	int ret;
	int i;

	uint8_t *frame_buffer;

	ret = oa_get_chunk_size(desc, &chunk_size);
	if (ret)
		return ret;

	for (i = 0; i < len; i++) {
		footer_index = i * (chunk_size + OA_FOOTER_LEN) + chunk_size;
		footer.val = no_os_get_unaligned_be32(&chunks[footer_index]);

		if (!footer.bits.dv)
			continue;

		if (footer.bits.sv)
			frame_size += chunk_size - footer.bits.swo;
		else if (footer.bits.ev)
			frame_size += footer.bits.ebo;
		else
			frame_size += chunk_size;
	}

	frame_buffer = no_os_calloc(chunk_size, sizeof(*frame_buffer));
	if (!frame_buffer)
		return -ENOMEM;

	for (i = 0; i < len; i++) {
		footer.val = no_os_get_unaligned_be32(chunks + chunk_size);

		if (!footer.bits.dv) {
			chunks += chunk_size + OA_FOOTER_LEN;
			break;
		}

		if (footer.bits.sv) {
			/* The current chunk contains the start of a frame at offset SWO */
			memcpy(&frame_buffer[frame_offset], &chunks[footer.bits.swo],
			       chunk_size - footer.bits.swo);
			frame_offset += chunk_size - footer.bits.swo;
		} else if (footer.bits.ev) {
			/* The current chunk may be shorter than chunk_size, since it contains the end of a frame */
			memcpy(&frame_buffer[frame_offset], chunks, footer.bits.ebo);
			frame_offset += footer.bits.ebo;
		} else {
			/* The current chunk does not contain either the start or end of a frame,
			 * and the contains valid data for each byte
			 */
			memcpy(&frame_buffer[frame_offset], chunks,
			       chunk_size);
			frame_offset += chunk_size;
		}

		chunks += chunk_size + OA_FOOTER_LEN;
	}

	return oa_enqueue(desc, &desc->rx_queue, frame_buffer, frame_size);
}

int oa_enqueue(struct adin1110_desc *desc, struct oa_list_element **queue,
	       uint8_t *frame_data, uint32_t length)
{
	struct oa_list_element *frame;
	struct oa_list_element *iter;
	int ret;
	int i;

	frame = no_os_calloc(1, sizeof(*frame));
	if (!frame)
		return ret;

	frame->data = frame_data;
	frame->length = length;

	iter = *queue;
	if (!iter) {
		*queue = frame;
		return 0;
	}

	while (iter->next)
		iter = iter->next;

	iter->next = frame;

	return 0;

free_frame:
	no_os_free(frame);

	return ret;
}

int oa_dequeue(struct adin1110_desc *desc, struct oa_list_element **queue,
	       uint8_t *frame_data, uint32_t *length)
{
	struct oa_list_element *frame_ref;
	int ret;
	int i;

	if (!*queue)
		return -ENOENT;

	/* The frame data has to be copied, since  */
	frame_data = (*queue)->data;
	frame_ref = *queue;
	*length = (*queue)->length;
	*queue = (*queue)->next;

	no_os_free(frame_ref);

	return 0;
}

int oa_init(struct adin1110_desc *desc, struct adin1110_init_param *param)
{
	int ret;

	struct no_os_irq_init_param irq_global_param = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
		.extra = NULL,
	};

	struct no_os_irq_init_param irq_gpio_param = {
		.irq_ctrl_id = param->int_param.port,
		.platform_ops = &max_gpio_irq_ops,
		.extra = NULL,
	};

	ret = no_os_gpio_get(&desc->int_gpio, &param->int_param);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(desc->int_gpio);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&desc->nvic_irq, &irq_global_param);
	if (ret)
		goto free_int_gpio;

	ret = no_os_irq_ctrl_init(&desc->gpio_irq, &irq_gpio_param);
	if (ret)
		goto free_int_gpio;

	ret = no_os_irq_enable(desc->nvic_irq, 24);
	if (ret)
		return ret;

	return 0;

free_int_gpio:
	no_os_gpio_remove(desc->int_gpio);

	return ret;
}
