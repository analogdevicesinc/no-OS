/***************************************************************************//**
 *   @file   adgs5412.h
 *   @brief  Header file of ADGS5412 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef ADGS5412_H_
#define ADGS5412_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADGS5412_REG_SW_DATA		0x01
#define ADGS5412_REG_ERR_CONFIG		0x02
#define ADGS5412_REG_ERR_FLAGS		0x03
#define ADGS5412_REG_BURST_EN		0x05
#define ADGS5412_REG_SOFT_RESETB	0x0B

/* ADGS5412_REG_SW_DATA for ADGS5412 */
#define ADGS5412_SW4_EN			(1 << 3)
#define ADGS5412_SW3_EN			(1 << 2)
#define ADGS5412_SW2_EN			(1 << 1)
#define ADGS5412_SW1_EN			(1 << 0)

/* ADGS5412_REG_SW_DATA for ADGS5414 */
#define ADGS5414_SW7_EN			(1 << 7)
#define ADGS5414_SW6_EN			(1 << 6)
#define ADGS5414_SW5_EN			(1 << 5)
#define ADGS5414_SW4_EN			(1 << 4)
#define ADGS5414_SW3_EN			(1 << 3)
#define ADGS5414_SW2_EN			(1 << 2)
#define ADGS5414_SW1_EN			(1 << 1)
#define ADGS5414_SW0_EN			(1 << 0)

/* ADGS5412_REG_ERR_CONFIG */
#define ADGS5412_RW_ERR_EN		(1 << 2)
#define ADGS5412_SCLK_ERR_EN		(1 << 1)
#define ADGS5412_CRC_ERR_EN		(1 << 0)

/* ADGS5412_REG_ERR_FLAGS */
#define ADGS5412_RW_ERR_FLAG		(1 << 2)
#define ADGS5412_SCLK_ERR_FLAG		(1 << 1)
#define ADGS5412_CRC_ERR_FLAG		(1 << 0)
#define ADGS5412_CLR_1			0x6C
#define ADGS5412_CLR_2			0xA9

/* ADGS5412_REG_BURST_EN */
#define ADGS5412_BURST_MODE_EN		(1 << 0)

/* ADGS5412_REG_SOFT_RESETB */
#define ADGS5412_SOFT_RESETB(x)		(((x) & 0xFF) << 0)
#define ADGS5412_RESET_1		0xA3
#define ADGS5412_RESET_2		0x05

#define ADGS5412_DAISY_CHAIN_1		0x25
#define ADGS5412_DAISY_CHAIN_2		0x00

#define ADGS5412_ALIGNMENT		0x25

#define ADGS5412_CRC8_POLY		0x07

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	ADGS5412_ENABLE,
	ADGS5412_DISABLE,
} adgs5412_state;

typedef struct {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* Device Settings */
	adgs5412_state	crc_en;
	adgs5412_state	burst_mode_en;
	adgs5412_state	daisy_chain_en;
} adgs5412_dev;

typedef struct {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	adgs5412_state	crc_en;
	adgs5412_state	burst_mode_en;
	adgs5412_state	daisy_chain_en;
} adgs5412_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Compute CRC8 checksum. */
uint8_t adgs5412_compute_crc8(uint8_t *data,
			      uint8_t data_size);
/* SPI register read from device. */
int32_t adgs5412_spi_reg_read(adgs5412_dev *dev,
			      uint8_t reg_addr,
			      uint8_t *reg_data);
/* SPI register write to device. */
int32_t adgs5412_spi_reg_write(adgs5412_dev *dev,
			       uint8_t reg_addr,
			       uint8_t reg_data);
/* SPI register read from device using a mask. */
int32_t adgs5412_spi_reg_read_mask(adgs5412_dev *dev,
				   uint8_t reg_addr,
				   uint8_t mask,
				   uint8_t *data);
/* SPI internal register write to device using a mask. */
int32_t adgs5412_spi_reg_write_mask(adgs5412_dev *dev,
				    uint8_t reg_addr,
				    uint8_t mask,
				    uint8_t data);
/* Do a software reset. */
int32_t adgs5412_do_soft_reset(adgs5412_dev *dev);
/* Clear the Error Flags Register. */
int32_t adgs5412_clear_err_flags(adgs5412_dev *dev);
/* Enter Daisy-Chain Mode. */
int32_t adgs5412_enter_daisy_chain(adgs5412_dev *dev);
/* Send Daisy-Chain commands. */
int32_t adgs5412_send_daisy_chain_cmds(adgs5412_dev *dev,
				       uint8_t *cmds,
				       uint8_t cmds_size);
/* Initialize the device. */
int32_t adgs5412_init(adgs5412_dev **device,
		      adgs5412_init_param init_param);
/* Free the resources allocated by adgs5412_init(). */
int32_t adgs5412_remove(adgs5412_dev *dev);
#endif // ADGS5412_H_
