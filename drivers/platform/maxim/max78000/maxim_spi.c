/***************************************************************************//**
 *   @file   maxim_spi.c
 *   @brief  Implementation of SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "spi.h"
#include "mxc_errors.h"
#include "mxc_pins.h"
#include "maxim_spi.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

#define SPI_MASTER_MODE	1
#define SPI_SINGLE_MODE	0

#define MAX_DELAY_SCLK	255
#define NS_PER_US	1000

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Set the closest first and last SCLK delays to what was requested
 * @param desc - SPI descriptor
 * @param msg - The message for which the delays will be set
 * @return void
 */
static void _max_delay_config(struct no_os_spi_desc *desc,
			      struct no_os_spi_msg *msg)
{
	struct max_spi_state *st = desc->extra;
	uint32_t sstime_cache;
	uint32_t ticks_delay;
	mxc_spi_regs_t *spi;
	uint32_t clk_rate;
	uint32_t ticks_ns;

	if (msg->cs_delay_first == st->cs_delay_first &&
	    msg->cs_delay_last == st->cs_delay_last)
		return;

	spi = MXC_SPI_GET_SPI(desc->device_id);
	sstime_cache = spi->sstime;
	clk_rate = MXC_SPI_GetPeripheralClock(spi);
	ticks_ns = NO_OS_DIV_ROUND_CLOSEST(NANO, clk_rate);

	if (msg->cs_delay_first != st->cs_delay_first) {
		/**
		 * The minimum number of delay ticks is 1. If 0 is written to the
		 * sstime register, there would be a delay of 256 ticks.
		 */
		if (msg->cs_delay_first == 0)
			ticks_delay = 1;
		else
			ticks_delay = msg->cs_delay_first * NS_PER_US / ticks_ns;

		if (ticks_delay > MAX_DELAY_SCLK) {
			pr_warning("cs_delay_first value is too high\n");
			goto error;
		}

		spi->sstime &= ~MXC_F_SPI_SSTIME_PRE;
		spi->sstime |= no_os_field_prep(MXC_F_SPI_SSTIME_PRE, ticks_delay);
	}
	if (msg->cs_delay_last != st->cs_delay_last) {
		/**
		 * The minimum number of delay ticks is 1. If 0 is written to the
		 * sstime register, there would be a delay of 256 ticks.
		 */
		if (msg->cs_delay_first == 0)
			ticks_delay = 1;
		else
			ticks_delay = msg->cs_delay_last * NS_PER_US / ticks_ns;

		if (ticks_delay > MAX_DELAY_SCLK) {
			pr_warning("cs_delay_last value is too high\n");
			goto error;
		}

		spi->sstime &= ~MXC_F_SPI_SSTIME_POST;
		spi->sstime |= no_os_field_prep(MXC_F_SPI_SSTIME_POST, ticks_delay);
	}

	st->cs_delay_first = msg->cs_delay_first;
	st->cs_delay_last = msg->cs_delay_last;

	return;

error:
	spi->sstime = sstime_cache;
}

static int _max_spi_config(struct no_os_spi_desc *desc)
{
	struct max_spi_init_param *eparam;
	struct max_spi_state *st;
	mxc_spi_mode_t mode;
	int32_t ret;

	st = desc->extra;
	eparam = st->init_param;

	mxc_spi_pins_t spi_pins_config = {
		.clock = true,
		.ss0 = (desc->chip_select == 0) ? true : false,
		.ss1 = (desc->chip_select == 1) ? true : false,
		.ss2 = (desc->chip_select == 2) ? true : false,
		.miso = true,
		.mosi = true,
		.sdio2 = false,
		.sdio3 = false,
		.vddioh = eparam->vssel,
	};

	ret = MXC_SPI_Init(MXC_SPI_GET_SPI(desc->device_id), SPI_MASTER_MODE,
			   SPI_SINGLE_MODE,
			   eparam->num_slaves, eparam->polarity, desc->max_speed_hz, spi_pins_config);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	/* For Maxim Platforms SPI Mode 1 and 2 are reversed */
	switch (desc->mode) {
	case NO_OS_SPI_MODE_1:
		mode = SPI_MODE_2;
		break;
	case NO_OS_SPI_MODE_2:
		mode = SPI_MODE_1;
		break;
	case NO_OS_SPI_MODE_0:
	/* fallthrough */
	case NO_OS_SPI_MODE_3:
		mode = (mxc_spi_mode_t)desc->mode;
		break;
	default:
		return -EINVAL;
	}

	ret = MXC_SPI_SetMode(MXC_SPI_GET_SPI(desc->device_id), mode);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetWidth(MXC_SPI_GET_SPI(desc->device_id), SPI_WIDTH_STANDARD);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetDataSize(MXC_SPI_GET_SPI(desc->device_id), 8);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	return 0;
err_init:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));

	return ret;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_init(struct no_os_spi_desc **desc,
		     const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct max_spi_state *st;
	struct no_os_spi_desc *descriptor;
	struct max_spi_init_param *eparam;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	st = no_os_calloc(1, sizeof(*st));
	if (!st) {
		ret = -ENOMEM;
		goto err;
	}

	eparam = param->extra;
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;

	st->init_param = eparam;
	descriptor->extra = st;

	if (descriptor->device_id >= MXC_SPI_INSTANCES) {
		ret = -EINVAL;
		goto err;
	}

	ret = _max_spi_config(descriptor);
	if (ret)
		goto err_init;

	*desc = descriptor;

	return 0;

err_init:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(descriptor->device_id));
err:
	no_os_free(st);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_transfer(struct no_os_spi_desc *desc,
			 struct no_os_spi_msg *msgs,
			 uint32_t len)
{
	mxc_spi_regs_t *spi = MXC_SPI_GET_SPI(desc->device_id);
	static uint32_t last_slave_id[MXC_SPI_INSTANCES];
	uint32_t tx_cnt = 0;
	uint32_t rx_cnt = 0;
	bool rx_done = true;
	bool tx_done = true;
	uint32_t slave_id;
	size_t i = 0;
	int32_t ret;

	if (!desc || !msgs)
		return -EINVAL;

	slave_id = desc->chip_select;
	if (slave_id != last_slave_id[desc->device_id]) {
		ret = _max_spi_config(desc);
		if (ret)
			return ret;

		last_slave_id[desc->device_id] = slave_id;
	}

	/* Assert CS desc->chip_select when the SPI transaction is started */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_ACTIVE;
	spi->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_ACTIVE,
				       NO_OS_BIT(desc->chip_select));

	for (i = 0; i < len; i++) {
		/* Flush the RX and TX FIFOs */
		spi->dma |= MXC_F_SPI_DMA_RX_FLUSH | MXC_F_SPI_DMA_TX_FLUSH;
		/* Enable SPI */
		spi->intfl |= MXC_F_SPI_INTFL_MST_DONE;
		spi->ctrl1 = 0;

		if (msgs[i].cs_change)
			spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;
		else
			spi->ctrl0 |= MXC_F_SPI_CTRL0_SS_CTRL;

		_max_delay_config(desc, &msgs[i]);

		if (msgs[i].tx_buff) {
			/* Set the transfer size in the TX direction */
			spi->ctrl1 = msgs->bytes_number;
			tx_done = false;
			/* Enable the TX FIFO */
			spi->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;
			tx_cnt += MXC_SPI_WriteTXFIFO(spi, &msgs[i].tx_buff[tx_cnt],
						      msgs[i].bytes_number - tx_cnt);
			tx_done = (tx_cnt == msgs[i].bytes_number) ? true : false;
		}
		if (msgs[i].rx_buff) {
			/* Set the transfer size in the RX direction */
			spi->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
						       msgs->bytes_number);
			/* Enable the RX FIFO */
			spi->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;
			rx_done = false;
		}

		/* Start the transaction */
		spi->ctrl0 |= MXC_F_SPI_CTRL0_START;

		while (!(rx_done && tx_done)) {
			if (msgs[i].tx_buff && tx_cnt < msgs[i].bytes_number) {
				tx_cnt += MXC_SPI_WriteTXFIFO(spi, &msgs[i].tx_buff[tx_cnt],
							      msgs[i].bytes_number - tx_cnt);
				tx_done = (tx_cnt == msgs[i].bytes_number) ? true : false;
			}
			if (msgs[i].rx_buff && rx_cnt < msgs[i].bytes_number) {
				rx_cnt += MXC_SPI_ReadRXFIFO(spi, &msgs[i].rx_buff[rx_cnt],
							     msgs[i].bytes_number - rx_cnt);
				rx_done = (rx_cnt == msgs[i].bytes_number) ? true : false;
			}
		}

		/* Wait for the RX and TX FIFOs to empty */
		while (!(spi->intfl & MXC_F_SPI_INTFL_MST_DONE));

		/* End the transaction */
		spi->ctrl0 &= ~MXC_F_SPI_CTRL0_START;

		/* Disable the RX and TX FIFOs */
		spi->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);

		no_os_udelay(msgs[i].cs_change_delay);
	}

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_write_and_read(struct no_os_spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number)
{
	struct no_os_spi_msg xfer = {
		.rx_buff = data,
		.tx_buff = data,
		.bytes_number = bytes_number,
		.cs_change = 1,
	};

	return max_spi_transfer(desc, &xfer, 1);
}

/**
 * @brief maxim platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops max_spi_ops = {
	.init = &max_spi_init,
	.write_and_read = &max_spi_write_and_read,
	.transfer = &max_spi_transfer,
	.remove = &max_spi_remove
};
