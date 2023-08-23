#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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

int oa_ctrl_tx_chunk_to_buffer(struct adin1110_desc *adin1110,
			       struct oa_tx_ctrl_chunk *chunk)
{
	struct oa_ctrl_header hdr = chunk->header;
	uint32_t header_val = 0;
	uint32_t len;

	header_val |= no_os_field_prep(OA_CTRL_HDRB_MASK, chunk->header.hdrb);
	header_val |= no_os_field_prep(OA_CTRL_WNR_MASK, chunk->header.wnr);
	header_val |= no_os_field_prep(OA_CTRL_AID_MASK, chunk->header.aid);
	header_val |= no_os_field_prep(OA_CTRL_MMS_MASK, chunk->header.mms);
	header_val |= no_os_field_prep(OA_CTRL_ADDR_MASK, chunk->header.addr);
	header_val |= no_os_field_prep(OA_CTRL_LEN_MASK, chunk->header.len);
	chunk->header.p = oa_crc1(header_val);
	header_val |= no_os_field_prep(OA_CTRL_P_MASK, chunk->header.p);

	no_os_put_unaligned_be32(no_os_bswap_constant_32(header_val), adin1110->data);

	memcpy(adin1110->data[OA_HEADER_LEN], chunk->data, OA_CHUNK_MAX_LEN);

	return 0;
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

// int oa_frame_to_chunks()
