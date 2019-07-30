// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad917x_reg.c
 *
 * \brief Contains AD917x APIs for SPI Register configuration and control
 *
 * Release 1.1.X
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "AD917x.h"
#include "ad917x_reg.h"
#include "api_errors.h"
#include <stddef.h>


#define IN_OUT_BUFF_SZ 3

int32_t ad917x_register_write(ad917x_handle_t *h,
			      const uint16_t address, const uint8_t data)
{
	int32_t err = API_ERROR_INVALID_HANDLE_PTR;
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

int32_t ad917x_register_read(ad917x_handle_t *h,
			     const uint16_t address, uint8_t *data)
{
	int32_t err = API_ERROR_INVALID_HANDLE_PTR;
	uint8_t inData[IN_OUT_BUFF_SZ];
	uint8_t outData[IN_OUT_BUFF_SZ];

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->dev_xfer == NULL)
		return API_ERROR_INVALID_XFER_PTR;
	/* set the 'read' bit. */
	inData[0] = (((address | 0x8000) >> 8) & 0xFF) ;
	inData[1] = ((address >> 0) & 0xFF);
	err = h->dev_xfer(h->user_data, inData, outData, IN_OUT_BUFF_SZ);
	if (err == 0)
		*data = outData[2];
	else
		return API_ERROR_SPI_XFER;
	return API_ERROR_OK;
}

int32_t ad917x_register_read_block(ad917x_handle_t *h,
				   const uint16_t address, uint8_t *data, uint32_t count)
{
	int32_t err;
	uint16_t i = 0;

	for (i = 0; i < count; i++) {
		err = ad917x_register_read(h, (address + i), &data[i]);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}

int32_t ad917x_register_write_tbl(ad917x_handle_t *h, struct
				  adi_reg_data *tbl, uint32_t count)
{
	uint16_t i = 0;
	int32_t err;

	for (i = 0; i < count; i++) {
		err = ad917x_register_write(h, tbl[i].reg, tbl[i].val);
		if (err != API_ERROR_OK)
			return err;
	}

	return API_ERROR_OK;
}
