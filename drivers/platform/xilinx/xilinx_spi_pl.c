/*******************************************************************************
 *   @file   xilinx/xilinx_spi_pl.c
 *   @brief  Driver for spi pl device using registers
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#include <xparameters.h>

#ifdef XPAR_XSPI_NUM_INSTANCES

#include <xspi.h>
#include <stdlib.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#define XPS_TYPE_WRITE		0x1
#define XPS_TYPE_READ		0x2
#define XSP_DEFAULT_SR_VALUE	0x25
#define XSP_DUMMY_DATA		0xFF

/*
 * Used to store current spi device configuration in order to not update it for
 * each spi transaction if not needed.
 */
static struct no_os_spi_desc g_spi_desc[XPAR_XSPI_NUM_INSTANCES];

/* Xilinx PL spi specific data */
struct xspi_desc {
	uint32_t base_addr;
	uint32_t disable_slaves_mask;
	uint32_t fifo_depth;
	uint8_t  cs_asserted;
};

/* Structure used to iterate over SPI messages */
struct msg_iter {
	struct no_os_spi_msg	*msgs;
	uint32_t	len;
	uint32_t	i;
	uint32_t	buff_i;
	uint8_t		is_wr;
};

/* Read device register */
static inline uint32_t _read(struct xspi_desc *xdesc, uint32_t reg)
{
	return Xil_In32(xdesc->base_addr + reg);
}

/* Write device register */
static inline void _write(struct xspi_desc *xdesc, uint32_t reg, uint32_t val)
{
	Xil_Out32(xdesc->base_addr + reg, val);
}

/* Update device register. Only bits set to 1 in mask will be modified */
static inline void _wr_mask(struct xspi_desc *xdesc, uint32_t reg,
			    uint32_t val, uint32_t mask)
{
	uint32_t tmp;

	tmp = _read(xdesc, reg);
	tmp &= ~mask;
	tmp |= val & mask;
	_write(xdesc, reg, tmp);
}

/* Update spi device configuration if needed */
static void _update_mode(struct no_os_spi_desc *desc)
{
	uint32_t		val;
	uint32_t		mask;

	if (desc->mode == g_spi_desc[desc->device_id].mode &&
	    desc->bit_order == g_spi_desc[desc->device_id].bit_order)
		return ;

	/* Update mode */
	val = desc->mode | NO_OS_SPI_CPOL ? XSP_CR_CLK_POLARITY_MASK : 0;
	val |= desc->mode | NO_OS_SPI_CPHA ? XSP_CR_CLK_PHASE_MASK : 0;
	mask = XSP_CR_CLK_POLARITY_MASK | XSP_CR_CLK_PHASE_MASK;

	val |= desc->bit_order == NO_OS_SPI_BIT_ORDER_LSB_FIRST ?
	       XSP_CR_LSB_MSB_FIRST_MASK : 0;
	mask |= XSP_CR_LSB_MSB_FIRST_MASK;

	_wr_mask(desc->extra, XSP_CR_OFFSET, val, mask);
	g_spi_desc[desc->device_id] = *desc;
}

/* Configure SPI PL device */
static int32_t _xil_spi_init_dev(struct xspi_desc *xdesc)
{
	uint32_t val;

	/* Software reset */
	_write(xdesc, XSP_SRR_OFFSET, XSP_SRR_RESET_MASK);
	no_os_mdelay(1);

	/* Check if success reset */
	val = _read(xdesc, XSP_SR_OFFSET);
	if (val != XSP_DEFAULT_SR_VALUE)
		return -ENODEV;

	/* Disable interrupts. Drivers only works with pooled mode */
	_write(xdesc, XSP_IIER_OFFSET, 0);

	/* Deasert chip selects */
	_write(xdesc, XSP_SSR_OFFSET, 0xFFFFFFFF);

	/* Get fifo depth */
	while (!(_read(xdesc, XSP_SR_OFFSET) & XSP_SR_TX_FULL_MASK))
		_write(xdesc, XSP_DTR_OFFSET, XSP_DUMMY_DATA);
	xdesc->fifo_depth = _read(xdesc, XSP_TFO_OFFSET) + 1;

	/* Configure spi device */
	val = XSP_CR_ENABLE_MASK |
	      XSP_CR_MANUAL_SS_MASK |
	      XSP_CR_TXFIFO_RESET_MASK |
	      XSP_CR_RXFIFO_RESET_MASK |
	      XSP_CR_MASTER_MODE_MASK |
	      XSP_CR_TRANS_INHIBIT_MASK;

	_write(xdesc, XSP_CR_OFFSET, val);

	return 0;
}

/* Initialize spi_desc structure and device */
static int32_t xil_spi_init_pl(struct no_os_spi_desc **desc,
			       const struct no_os_spi_init_param *param)
{
	struct no_os_spi_desc		*ldesc;
	struct xspi_desc	*xdesc;
	int32_t			err;
	XSpi_Config		*cfg;

	if (!desc || !param || param->device_id >= XPAR_XSPI_NUM_INSTANCES)
		return -EINVAL;

	cfg = XSpi_LookupConfig(param->device_id);
	if (!cfg)
		return -EINVAL;

	if (param->chip_select > cfg->NumSlaveBits)
		return -EINVAL;

	ldesc = (struct no_os_spi_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	xdesc = (struct xspi_desc *)no_os_calloc(1, sizeof(*xdesc));
	if (!xdesc) {
		err = -ENOMEM;
		goto error;
	}

	ldesc->bit_order = param->bit_order;
	ldesc->chip_select = param->chip_select;
	ldesc->mode = param->mode;
	xdesc->base_addr = cfg->BaseAddress;
	xdesc->disable_slaves_mask = (1 << cfg->NumSlaveBits) - 1;
	xdesc->cs_asserted = 0;

	err = _xil_spi_init_dev(xdesc);
	if (NO_OS_IS_ERR_VALUE(err))
		goto error;

	ldesc->extra = xdesc;
	*desc = ldesc;

	return 0;

error:
	no_os_free(xdesc);
	no_os_free(ldesc);

	return err;
}

/* Remove allocated resources */
static int32_t xil_spi_remove_pl(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/*
 * Fill fifo until fifo is full or no more data available.
 * Return the number of written bytes.
 */
static uint32_t _wr_fifo(struct xspi_desc *xdesc, uint8_t *data,
			 uint32_t len)
{
	uint32_t i;
	uint32_t to_write;
	uint32_t tx_available;

	tx_available = xdesc->fifo_depth - _read(xdesc, XSP_TFO_OFFSET);
	to_write = no_os_min(tx_available, len);
	for (i = 0; i < to_write; ++i)
		if (data)
			_write(xdesc, XSP_DTR_OFFSET, data[i]);
		else
			_write(xdesc, XSP_DTR_OFFSET, XSP_DUMMY_DATA);

	return to_write;
}

/*
 * Read fifo until fifo is empty or no more buffer available to read into
 * Returns the number of read bytes
 */
static uint32_t _rd_fifo(struct xspi_desc *xdesc, uint8_t *data,
			 uint32_t len)
{
	uint32_t rx_available;
	uint32_t to_read;
	uint32_t tmp;
	uint32_t i;

	tmp = _read(xdesc, XSP_SR_OFFSET);
	if (tmp & XSP_SR_RX_EMPTY_MASK)
		return 0;

	rx_available = _read(xdesc, XSP_RFO_OFFSET) + 1;
	to_read = no_os_min(rx_available, len);
	for (i = 0; i < to_read; ++i) {
		tmp = _read(xdesc, XSP_DRR_OFFSET);
		if (data)
			data[i] = tmp;
	}

	return to_read;
}

/* If start is 0 stop transfer, else start it.
 * CS will be asserted if needed. If different CS othere than the one needed
 * is asserted. Deassert it and assert correct one */
static void _xil_spi_start_transfer(struct no_os_spi_desc *desc, uint8_t start)
{
	struct xspi_desc	*xdesc;

	xdesc = desc->extra;
	if (start) {
		if (xdesc->cs_asserted) {
			if (desc->chip_select ==
			    g_spi_desc[desc->device_id].chip_select)
				return ;
			else
				/* Stop transfer to start again with new CS */
				_xil_spi_start_transfer(desc, 0);
		}
		_write(xdesc, XSP_SSR_OFFSET, ~(1 << desc->chip_select));
		_wr_mask(xdesc, XSP_CR_OFFSET, 0, XSP_CR_TRANS_INHIBIT_MASK);
	} else {
		_wr_mask(xdesc, XSP_CR_OFFSET, XSP_CR_TRANS_INHIBIT_MASK,
			 XSP_CR_TRANS_INHIBIT_MASK);
		_write(xdesc, XSP_SSR_OFFSET, xdesc->disable_slaves_mask);
	}
	xdesc->cs_asserted = start;
}


/* SPI polled transfer */
static int32_t xil_spi_write_and_read_pl(struct no_os_spi_desc *desc,
		uint8_t *data,
		uint16_t bytes_number)
{
	struct xspi_desc	*xdesc;
	uint32_t		rx;
	uint32_t		tx;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = desc->extra;
	_update_mode(desc);

	_xil_spi_start_transfer(desc, 1);
	rx = 0;
	tx = 0;
	while (rx < bytes_number) {
		if (tx < bytes_number)
			tx += _wr_fifo(xdesc, data + tx, bytes_number - tx);
		rx += _rd_fifo(xdesc, data + rx, bytes_number - rx);
	}
	_xil_spi_start_transfer(desc, 0);

	return 0;
}

/* Get type of message. It can be READ, WRITE or both */
static uint32_t _get_msg_type(struct no_os_spi_msg *msg)
{
	uint32_t	type;

	type = 0;
	if (msg->tx_buff)
		type |= XPS_TYPE_WRITE;
	if (msg->rx_buff)
		type |= XPS_TYPE_READ;

	return type;
}

/* Get information based on current iterator position */
static void _get_data(struct msg_iter *it, uint8_t **buff, uint32_t *len,
		      uint32_t *type)
{
	struct no_os_spi_msg *msg;

	if (it->i == it->len) {
		*len = 0;
		return ;
	}

	msg = it->msgs + it->i;
	if (it->is_wr)
		*buff = msg->tx_buff;
	else
		*buff = msg->rx_buff;

	if (*buff)
		*buff += it->buff_i;

	*len = msg->bytes_number - it->buff_i;
	if (type)
		*type = _get_msg_type(msg);
}

/*
 * Increment iterator position with desired length.
 * If the new position has a different message type, new_type will be set to 1
 * If a CS change is needed, cs_change is set
 */
static void _increment_iter(struct msg_iter *it, uint32_t len,
			    uint8_t *cs_change, uint8_t *new_type)
{
	struct no_os_spi_msg	*msg;
	uint32_t	type;
	uint32_t	extra_len;

	if (!len)
		return ;

	*cs_change = 0;
	*new_type = 0;
	msg = it->msgs + it->i;
	if (it->buff_i + len > msg->bytes_number)
		extra_len = it->buff_i + len - msg->bytes_number;
	else
		extra_len = 0;
	len -= extra_len;
	it->buff_i += len;
	if (it->buff_i == msg->bytes_number) {
		type = _get_msg_type(msg);
		++it->i;
		it->buff_i = 0;
		if (cs_change && msg->cs_change)
			*cs_change = 1;
		if (new_type)
			if (it->i == it->len ||
			    type != _get_msg_type(it->msgs + it->i))
				*new_type = 1;
	}
	if (extra_len)
		_increment_iter(it, extra_len, cs_change, new_type);
}

/*
 * SPI polling transfer of multiple messages.
 */
static int32_t xil_spi_transfer_pl(struct no_os_spi_desc *desc,
				   struct no_os_spi_msg *msgs,
				   uint32_t len)

{
	struct xspi_desc	*xdesc;
	uint8_t			cs_change;
	uint8_t			new_type;
	struct msg_iter		rx = {.msgs = msgs, .len = len, .is_wr = 0};
	struct msg_iter		tx = {.msgs = msgs, .len = len, .is_wr = 1};
	uint8_t			*buff;
	uint32_t		bytes;
	uint32_t		type;
	/* Bytes to increment rx when only a write transaction is executed */
	uint32_t		to_inc_rx;
	/* Difference between rx and tx iterators in bytes */
	uint32_t		tx_advance;
	/* Save bytes sent during a write loop */
	uint32_t		tx_sent;
	/* Used to loop if need to empty the RX FIFO */
	uint32_t		loop;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = desc->extra;
	_update_mode(desc);

	tx_advance = 0;
	to_inc_rx = 0;
	buff = NULL;
	type = 0;
	do {
		_xil_spi_start_transfer(desc, 1);

		tx_sent = 0;
		/* Writing loop */
		do {
			_get_data(&tx, &buff, &bytes, &type);
			if (type & XPS_TYPE_READ)
				/*
				 * Do not write more bytes than fifo_depth if
				 * they haven't been read.
				 */
				bytes = no_os_min(bytes,
						  xdesc->fifo_depth - tx_advance);
			bytes = _wr_fifo(xdesc, buff, bytes);
			tx_sent += bytes;
			_increment_iter(&tx, bytes, &cs_change, &new_type);
			tx_advance += bytes;
		} while (!cs_change && !new_type && bytes);

		if (type & XPS_TYPE_READ) {
			loop = 1;
			/*
			 * Loop only when no more data to write otherwise read
			 * only available data.
			 */
			while (loop) {
				if (!new_type)
					loop = 0;
				_get_data(&rx, &buff, &bytes, NULL);
				bytes = _rd_fifo(xdesc, buff, bytes);
				_increment_iter(&rx, bytes, &cs_change, &new_type);
				tx_advance -= bytes;
				if (loop && new_type)
					break;
			}
		} else {
			/* Clean RX FIFO if it was only a WRITE message */
			to_inc_rx += tx_sent;
			if (new_type) {
				/* Wait for TX to be sent */
				while (!(_read(xdesc, XSP_SR_OFFSET) &
					 XSP_SR_TX_EMPTY_MASK))
					;
				/* Reset fifo. Only write was performed */
				_wr_mask(xdesc, XSP_CR_OFFSET,
					 XSP_CR_RXFIFO_RESET_MASK,
					 XSP_CR_RXFIFO_RESET_MASK);

				_increment_iter(&rx, to_inc_rx, NULL, NULL);
				tx_advance -= to_inc_rx;
				to_inc_rx = 0;
			}
		}

		if ((cs_change && rx.i != len) || (!cs_change && rx.i == len))
			_xil_spi_start_transfer(desc, 0);
	} while (rx.i < len);

	return 0;
}

const struct no_os_spi_platform_ops xil_spi_pl_ops = {
	.init = xil_spi_init_pl,
	.remove = xil_spi_remove_pl,
	.write_and_read = xil_spi_write_and_read_pl,
	.transfer = xil_spi_transfer_pl
};

#endif // XPAR_XSPI_NUM_INSTANCES
