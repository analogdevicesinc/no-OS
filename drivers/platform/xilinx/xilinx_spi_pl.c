/*******************************************************************************
 *   @file   xilinx/xilinx_spi_pl.c
 *   @brief  Driver for spi pl device using registers
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#include <xparameters.h>

#ifdef XPAR_XSPI_NUM_INSTANCES

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xspi.h>
#include <stdlib.h>
#include <stdbool.h>
#include "spi.h"
#include "error.h"
#include "delay.h"
#include "util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define XSP_DEFAULT_SR_VALUE	0x25
#define XSP_DUMMY_DATA		0xFF

/*
 * Used to store current spi device configuration in order to not update it for
 * each spi transaction if not needed.
 */
static struct spi_desc g_spi_desc[XPAR_XSPI_NUM_INSTANCES];

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Xilinx PL spi specific data */
struct xspi_desc {
	uint32_t base_addr;
	uint32_t disable_slaves_mask;
	uint32_t fifo_depth;
	uint8_t  cs_asserted;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
static void _update_mode(struct spi_desc *desc)
{
	uint32_t		val;
	uint32_t		mask;

	if (desc->mode == g_spi_desc[desc->device_id].mode &&
	    desc->bit_order == g_spi_desc[desc->device_id].bit_order)
		return ;

	/* Update mode */
	val = desc->mode | SPI_CPOL ? XSP_CR_CLK_POLARITY_MASK : 0;
	val |= desc->mode | SPI_CPHA ? XSP_CR_CLK_PHASE_MASK : 0;
	mask = XSP_CR_CLK_POLARITY_MASK | XSP_CR_CLK_PHASE_MASK;

	val |= desc->bit_order == SPI_BIT_ORDER_LSB_FIRST ?
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
	mdelay(1);

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

	return SUCCESS;
}

/* Initialize spi_desc structure and device */
static int32_t xil_spi_init_pl(struct spi_desc **desc,
			       const struct spi_init_param *param)
{
	struct spi_desc		*ldesc;
	struct xspi_desc	*xdesc;
	int32_t			err;
	XSpi_Config		*cfg;

	if (!desc || !param || param->device_id >= XPAR_XSPI_NUM_INSTANCES)
		return -EINVAL;

	cfg = XSpi_LookupConfig(param->device_id);
	if (!cfg) {
		err = -EINVAL;
		goto error;
	}

	if (param->chip_select > cfg->NumSlaveBits)
		return -EINVAL;

	ldesc = (struct spi_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc) {
		err = -ENOMEM;
		goto error;
	}

	xdesc = (struct xspi_desc *)calloc(1, sizeof(*xdesc));
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
	if (IS_ERR_VALUE(err))
		goto error;

	ldesc->extra = xdesc;
	*desc = ldesc;

	return SUCCESS;

error:
	free(xdesc);
	free(ldesc);

	return err;
}

/* Remove allocated resources */
static int32_t xil_spi_remove_pl(struct spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	free(desc->extra);
	free(desc);

	return SUCCESS;
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
	to_write = min(tx_available, len);
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
	to_read = min(rx_available, len);
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
static void _xil_spi_start_transfer(struct spi_desc *desc, uint8_t start)
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
static int32_t xil_spi_write_and_read_pl(struct spi_desc *desc, uint8_t *data,
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

	return SUCCESS;
}

const struct spi_platform_ops xil_spi_reg_ops_pl = {
	.init = xil_spi_init_pl,
	.remove = xil_spi_remove_pl,
	.write_and_read = xil_spi_write_and_read_pl
};

#endif // XPAR_XSPI_NUM_INSTANCES
