/***************************************************************************//**
 *   @file   tx_generator.c
 *   @brief  Driver for the ADI tx_generator AXI IP.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_axi_io.h"
#include "tx_generator.h"

int32_t tx_generator_read(struct tx_generator_dev *dev,
			  uint32_t reg_addr, uint32_t *reg_data)
{
	no_os_axi_io_read(dev->base, reg_addr, reg_data);
	return 0;
}

int32_t tx_generator_write(struct tx_generator_dev *dev,
			   uint32_t reg_addr, uint32_t reg_data)
{
	no_os_axi_io_write(dev->base, reg_addr, reg_data);
	return 0;
}

int32_t tx_generator_init(struct tx_generator_dev **dev,
			  const struct tx_generator_init_param *init)
{
	struct tx_generator_dev *d;
	uint32_t reg_data, freq, ratio;

	d = (struct tx_generator_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->name = init->name;
	d->base = init->base;
	d->num_channels = init->num_channels;

	tx_generator_write(d, TX_GEN_REG_RESET_CTRL, 0);
	tx_generator_write(d, TX_GEN_REG_RESET_CTRL,
			   TX_GEN_MMCM_RSTN | TX_GEN_RSTN);

	no_os_mdelay(100);

	tx_generator_read(d, TX_GEN_REG_STATUS, &reg_data);
	if (!(reg_data & TX_GEN_STATUS_OK)) {
		printf("%s: Status error: 0x%08lx\n",
		       d->name, (unsigned long)reg_data);
		no_os_free(d);
		return -EIO;
	}

	tx_generator_read(d, TX_GEN_REG_CLK_COUNT, &freq);
	tx_generator_read(d, TX_GEN_REG_CLK_RATIO, &ratio);
	d->clock_hz = (uint64_t)freq * ratio;
	d->clock_hz = (d->clock_hz * 390625) >> 8;

	printf("%s: Successfully initialized (%" PRIu64 " Hz)\n",
	       d->name, d->clock_hz);

	*dev = d;
	return 0;
}

int32_t tx_generator_remove(struct tx_generator_dev *dev)
{
	no_os_free(dev);
	return 0;
}

int32_t tx_generator_cmd_write(struct tx_generator_dev *dev,
			       uint16_t cmd_addr, uint8_t cmd_numwords,
			       enum tx_gen_cmd_op cmd_op, uint32_t cmd_data)
{
	uint32_t ctrl;

	tx_generator_write(dev, TX_GEN_REG_CMD_DATA, cmd_data);

	ctrl = no_os_field_prep(TX_GEN_CMD_ADDR_MSK, cmd_addr) |
	       no_os_field_prep(TX_GEN_CMD_NUMWORDS_MSK, cmd_numwords) |
	       no_os_field_prep(TX_GEN_CMD_OP_MSK, cmd_op) |
	       TX_GEN_CMD_START;

	tx_generator_write(dev, TX_GEN_REG_CMD_CTRL, ctrl);

	return 0;
}

int32_t tx_generator_cmd_set_stream_source(struct tx_generator_dev *dev,
					   bool dma)
{
	uint32_t reg;

	tx_generator_read(dev, TX_GEN_REG_CMD_CTRL, &reg);

	if (dma)
		reg |= TX_GEN_CMD_STREAM_SRC;
	else
		reg &= ~TX_GEN_CMD_STREAM_SRC;

	reg &= ~TX_GEN_CMD_START;

	tx_generator_write(dev, TX_GEN_REG_CMD_CTRL, reg);

	return 0;
}

int32_t tx_generator_load_custom_data(struct tx_generator_dev *dev,
				      const uint32_t *custom_data_iq,
				      uint32_t custom_tx_count,
				      uint32_t address)
{
	uint32_t index, index_mem = 0;
	uint8_t chan;
	uint8_t num_tx_channels = dev->num_channels / 2;

	if (num_tx_channels == 0)
		num_tx_channels = 1;

	for (index = 0; index < custom_tx_count; index++) {
		for (chan = 0; chan < num_tx_channels; chan++) {
			no_os_axi_io_write(address,
					   index_mem * sizeof(uint32_t),
					   custom_data_iq[index]);
			index_mem++;
		}
	}

	return 0;
}
