/***************************************************************************//**
 *   @file   nhd_c12832a1z.h
 *   @brief  Implementation of nhd_c12832a1z.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#ifndef __NHD_C12832A1Z_H__
#define __NHD_C12832A1Z_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define NHD_C12832A1Z_RST_ON     	0U
#define NHD_C12832A1Z_RST_OFF    	1U
#define NHD_C12832A1Z_DC_DATA    	1U
#define NHD_C12832A1Z_DC_CMD     	0U
#define NHD_C12832A1Z_DISP_ON    	0xAFU
#define NHD_C12832A1Z_DISP_OFF   	0xAEU
#define NDH_C12832A1Z_ADC_NORMAL	0xA0U
#define NDH_C12832A1Z_ADC_REVERSE	0xA1U
#define NDH_C12832A1Z_COM_NORMAL	0xC0U
#define NDH_C12832A1Z_COM_REVERSE	0xC8U
#define NDH_C12832A1Z_LCD_BIAS		0xA2U
#define NDH_C12832A1Z_PWR_CTRL		0x2FU
#define NDH_C12832A1Z_RES_RATIO		0x21U
#define NDH_C12832A1Z_ELECTRIC_VOL	0x81U
#define NDH_C12832A1Z_ELECTRIC_VAL	0x20U

#define NHD_C12832A1Z_REVERSE		0x1
#define NHD_C12832A1Z_BLINK		0x2

#define NHD_C12832A1Z_BLINK_INTERVAL	500

#define FB_FLUSH_DELAY			30

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct nhd_c12832a1z_extra
 * @brief Extra parameters needed for nhd_c12832a1z usage.
 */

struct nhd_c12832a1z_dev {
	/** Data/Command pin gpio desc */
	struct no_os_gpio_desc		*dc_pin;
	/** RESET pin gpio desc*/
	struct no_os_gpio_desc     	*reset_pin;
	/* SPI descriptor*/
	struct no_os_spi_desc		*spi_desc;
};

struct nhd_c12832a1z_init_param {
	/** Data/Command pin gpio initial param */
	struct no_os_gpio_init_param	*dc_pin_ip;
	/** RESET pin gpio initial param */
	struct no_os_gpio_init_param	*reset_pin_ip;
	/* SPI initial param */
	struct no_os_spi_init_param 	*spi_ip;

};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int nhd_c12832a1z_write_cmd(struct nhd_c12832a1z_dev *dev, uint8_t cmd);

int nhd_c12832a1z_write_data(struct nhd_c12832a1z_dev *dev, uint8_t data);

int nhd_c12832a1z_clear_lcd(struct nhd_c12832a1z_dev *dev);

/** Initialize the nhd_c12832a1z peripheral for display operation. */
int nhd_c12832a1z_init(struct nhd_c12832a1z_dev **device, struct nhd_c12832a1z_init_param init_param);

/** nhd_c12832a1z turns display on/off. */
int nhd_c12832a1z_display_on_off(struct nhd_c12832a1z_dev *device, uint8_t on_off);

/*  Moves cursor to desired row/column. */
int nhd_c12832a1z_move_cursor(struct nhd_c12832a1z_dev *device, uint8_t row,
			     uint8_t column);

/** Print string */
int nhd_c12832a1z_print_string(struct nhd_c12832a1z_dev *dev, char *msg, int len);

/** Removes resources allocated by device. */
int nhd_c12832a1z_remove(struct nhd_c12832a1z_dev *device);

#endif //__NHD_C12832A1Z_H__