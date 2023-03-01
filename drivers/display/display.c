/***************************************************************************//**
 *   @file   display.c
 *   @brief  Implementation of display Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include "display.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include <string.h>

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the display peripheral.
 *
 * @param device     - The device structure.
 * @param param      - The structure that contains the device initial
 * 		               parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_init(struct display_dev **device,
		     const struct display_init_param *param)
{
	struct display_dev *dev;
	uint32_t ret;

	if (!device || !param)
		return -EINVAL;

	dev = (struct display_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;
	dev->cols_nb = param->cols_nb;
	dev->rows_nb = param->rows_nb;
	dev->controller_ops = param->controller_ops;
	dev->extra = param->extra;

	ret = dev->controller_ops->init(dev);
	if (ret != 0) {
		no_os_free(dev);
		return -1;
	}

	*device = dev;

	return ret;
}

/****************************************************************************//**
 * @brief Frees the resources allocated by display_init().
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_remove(struct display_dev *device)
{
	int32_t ret;

	if (!device)
		return -EINVAL;

	ret = device->controller_ops->remove(device);
	if (ret != 0)
		return -1;
	no_os_free(device);

	return ret;
}

/***************************************************************************//**
 * @brief Turns display on.
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_on(struct display_dev *device)
{
	if (!device)
		return -EINVAL;

	return device->controller_ops->display_on_off(device, true);
}

/***************************************************************************//**
 * @brief Turns display off.
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_off(struct display_dev *device)
{
	if (!device)
		return -EINVAL;

	return device->controller_ops->display_on_off(device, false);
}

/***************************************************************************//**
 * @brief Moves cursor to desired position.
 *
 * @param device - The device structure.
 * @param row    - row
 * @param column - column
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_move_cursor(struct display_dev *device, uint8_t row,
			    uint8_t column)
{
	if (!device)
		return -EINVAL;

	return device->controller_ops->move_cursor(device, row, column);
}

/***************************************************************************//**
 * @brief Clears data on display.
 *
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_clear(struct display_dev *device)
{
	int32_t ret;
	uint8_t i, j;

	if (!device)
		return -EINVAL;

	for(i = 0; i < device->rows_nb; i++)
		for(j = 0; j < device->cols_nb; j++) {
			ret = device->controller_ops->print_char(device, ' ', i, j);
			if (ret != 0)
				return -1;
		}
	return ret;
}

/***************************************************************************//**
 * @brief Prints char string at selected position.
 *
 * @param device - The device structure.
 * @param msg    - char string pointer
 * @param row    - row
 * @param column - column
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_print_string(struct display_dev *device, char *msg,
			     uint8_t row, uint8_t column)
{
	int32_t ret;
	int32_t len;
	int32_t i;
	int32_t r = row;
	int32_t c = column;

	if (!device || !msg)
		return -EINVAL;

	len = strlen(msg);
	for(i = 0; i < len; i++) {
		if(r < device->rows_nb) {
			if(c < device->cols_nb) {
				ret = device->controller_ops->print_char(device, msg[i], r, c);
				if (ret != 0)
					return -1;
				c++;
			} else {
				c=0U;
				r++;
				ret = device->controller_ops->print_char(device, msg[i], r, c);
				if (ret != 0)
					return -1;
				c++;
			}
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Prints single char at selected position.
 *
 * @param device - The device structure.
 * @param chr    - char to be printed
 * @param row    - row
 * @param column - column
 * @return Returns 0 in case of success or negative error code otherwise.
*******************************************************************************/
int32_t display_print_char(struct display_dev *device, char chr,
			   uint8_t row, uint8_t column)
{
	if (!device)
		return -EINVAL;

	return device->controller_ops->print_char(device, chr, row, column);
}
