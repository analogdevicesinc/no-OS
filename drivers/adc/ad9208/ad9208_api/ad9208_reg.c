// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_reg.c
 *
 * \brief Contains AD9208 APIs for ADC Register Configuration and Control
 *
 * Release 1.0.x
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "ad9208_api.h"
#include "ad9208_reg.h"
#include "api_errors.h"

#define IN_OUT_BUFF_SZ 3

int ad9208_register_write(ad9208_handle_t *h,
			  const uint16_t address, const uint8_t data)
{
	int err = API_ERROR_INVALID_HANDLE_PTR;
	uint8_t inData[IN_OUT_BUFF_SZ];
	uint8_t outData[IN_OUT_BUFF_SZ];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->dev_xfer == NULL)
		return API_ERROR_INVALID_XFER_PTR;
	inData[0] = ((address >> 8) & 0xFF);
	inData[1] = ((address >> 0) & 0xFF);
	inData[2] = data;
	err = h->dev_xfer(h->user_data, inData, outData, IN_OUT_BUFF_SZ);
	if (err != 0)
		return API_ERROR_SPI_XFER;
	return API_ERROR_OK;
}

int ad9208_register_read(ad9208_handle_t *h,
			 const uint16_t address, uint8_t *data)
{
	int err = API_ERROR_INVALID_HANDLE_PTR;
	uint8_t inData[IN_OUT_BUFF_SZ];
	uint8_t outData[IN_OUT_BUFF_SZ];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->dev_xfer == NULL)
		return API_ERROR_INVALID_XFER_PTR;
	/* set the 'read' bit. */
	inData[0] = (((address | 0x8000) >> 8) & 0xFF);
	inData[1] = ((address >> 0) & 0xFF);
	err = h->dev_xfer(h->user_data, inData, outData, IN_OUT_BUFF_SZ);
	if (err == 0)
		*data = outData[2];
	else
		return API_ERROR_SPI_XFER;
	return API_ERROR_OK;
}

int ad9208_register_read_block(ad9208_handle_t *h,
			       const uint16_t address, uint8_t *data,
			       uint32_t count)
{
	int err;
	uint16_t i = 0;

	for (i = 0; i < count; i++) {
		err = ad9208_register_read(h, (address + i), &data[i]);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int ad9208_register_write_tbl(ad9208_handle_t *h, struct
			      adi_reg_data * tbl, uint32_t count)
{
	uint16_t i = 0;
	int err;

	if (tbl == NULL)
		return API_ERROR_INVALID_PARAM;

	for (i = 0; i < count; i++) {
		err = ad9208_register_write(h, tbl[i].reg, tbl[i].val);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int ad9208_is_sync_spi_update_enabled(ad9208_handle_t *h, uint8_t *enabled)
{
	int err;
	uint8_t tmp_reg;

	if (enabled == NULL)
		return API_ERROR_INVALID_PARAM;

	err = ad9208_register_read(h, AD9208_DDC_SYNC_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	*enabled = (tmp_reg & AD9208_DDC_UPDATE_MODE) ? 0x1 : 0x0;

	return API_ERROR_OK;
}

int ad9208_register_chip_transfer(ad9208_handle_t *h)
{
	int err;

	err = ad9208_register_write(h,
				    AD9208_CHIP_SPI_XFER_REG,
				    AD9208_CHIP_TRIGGER_SPI_XFER);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}
