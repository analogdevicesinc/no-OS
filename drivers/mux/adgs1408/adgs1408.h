/***************************************************************************//**
 *   @file   adgs1408.h
 *   @brief  Header file of ADGS1408 Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef ADGS1408_H_
#define ADGS1408_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADGS1408_REG_SW_DATA		        0x01
#define ADGS1408_REG_ERR_CONFIG		        0x02
#define ADGS1408_REG_ERR_FLAGS		        0x03
#define ADGS1408_REG_BURST_EN		        0x05
#define ADGS1408_REG_ROUND_ROBIN_EN	        0x06
#define ADGS1408_REG_RROBIN_CHANNEL_CONFIG	0x07
#define ADGS1408_REG_CNV_EDGE_SEL	        0x09
#define ADGS1408_REG_SOFT_RESETB	        0x0B

/* ADGS1408_REG_SW_DATA for ADGS1408 */
#define ADGS1408_SW1_EN                 0x01
#define ADGS1408_SW2_EN                 0x03
#define ADGS1408_SW3_EN                 0x05
#define ADGS1408_SW4_EN                 0x07
#define ADGS1408_SW5_EN                 0x09
#define ADGS1408_SW6_EN                 0x0B
#define ADGS1408_SW7_EN                 0x0D
#define ADGS1408_SW8_EN                 0x0F

/* ADGS1408_REG_SW_DATA for ADGS1409 */
#define ADGS1409_SW1_EN			0x01
#define ADGS1409_SW2_EN			0x03
#define ADGS1409_SW3_EN			0x05
#define ADGS1409_SW4_EN			0x07

/* ADGS1408_REG_ERR_CONFIG */
#define ADGS1408_RW_ERR_EN		(1 << 2)
#define ADGS1408_SCLK_ERR_EN		(1 << 1)
#define ADGS1408_CRC_ERR_EN		(1 << 0)

/* ADGS1408_REG_ERR_FLAGS */
#define ADGS1408_RW_ERR_FLAG		(1 << 2)
#define ADGS1408_SCLK_ERR_FLAG		(1 << 1)
#define ADGS1408_CRC_ERR_FLAG		(1 << 0)
#define ADGS1408_CLR_1			0x6C
#define ADGS1408_CLR_2			0xA9

/* ADGS1408_REG_BURST_EN */
#define ADGS1408_BURST_MODE_EN		(1 << 0)

/* ADGS1408_REG_ROUND_ROBIN_EN */
#define ADGS1408_ROUND_ROBIN_EN		(1 << 0)

/* ADGS1408_REG_RROBIN_CHANNEL_CONFIG  for ADGS1408 */
#define ADGS1408_RROBIN_SW1(x)	(((x) & 0x1) << 0)
#define ADGS1408_RROBIN_SW2(x)	(((x) & 0x1) << 1)
#define ADGS1408_RROBIN_SW3(x)	(((x) & 0x1) << 2)
#define ADGS1408_RROBIN_SW4(x)	(((x) & 0x1) << 3)
#define ADGS1408_RROBIN_SW5(x)	(((x) & 0x1) << 4)
#define ADGS1408_RROBIN_SW6(x)	(((x) & 0x1) << 5)
#define ADGS1408_RROBIN_SW7(x)	(((x) & 0x1) << 6)
#define ADGS1408_RROBIN_SW8(x)	(((x) & 0x1) << 7)

/* ADGS1408_REG_RROBIN_CHANNEL_CONFIG  for ADGS1409 */
#define ADGS1408_RROBIN_SW1(x)	(((x) & 0x1) << 0)
#define ADGS1408_RROBIN_SW2(x)	(((x) & 0x1) << 1)
#define ADGS1408_RROBIN_SW3(x)	(((x) & 0x1) << 2)
#define ADGS1408_RROBIN_SW4(x)	(((x) & 0x1) << 3)

/* ADGS1408_REG_CNV_EDGE_SEL */
#define ADGS1408_CNV_EDGE_RISING        (1 << 0)

/* ADGS1408_REG_SOFT_RESETB */
#define ADGS1408_SOFT_RESETB(x)		(((x) & 0xFF) << 0)
#define ADGS1408_RESET_1		0xA3
#define ADGS1408_RESET_2		0x05

#define ADGS1408_DAISY_CHAIN_1		0x25
#define ADGS1408_DAISY_CHAIN_2		0x00

/* ADGS1408 exit Round Robin*/
#define ADGS1408_RROBIN_EXIT_1		0xA3
#define ADGS1408_RROBIN_EXIT_2		0x18
#define ADGS1408_RROBIN_EXIT_3		0xE3
#define ADGS1408_RROBIN_EXIT_4		0xB4

#define ADGS1408_ALIGNMENT		0x25

#define ADGS1408_CRC8_POLY		0x07

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adgs1408_type {
	ID_ADGS1408,
	ID_ADGS1409,
};

enum adgs1408_state {
	ADGS1408_ENABLE,
	ADGS1408_DISABLE,
};

struct adgs1408_rrobin_config {
	bool rrobin_sw1;
	bool rrobin_sw2;
	bool rrobin_sw3;
	bool rrobin_sw4;
	bool rrobin_sw5;
	bool rrobin_sw6;
	bool rrobin_sw7;
	bool rrobin_sw8;
};

struct adgs1408_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* Device Settings */
	enum adgs1408_state	crc_en;
	enum adgs1408_state	burst_mode_en;
	enum adgs1408_state	daisy_chain_en;
	enum adgs1408_state	round_robin_en;
	struct adgs1408_rrobin_config rrobin_sw_config;
	enum adgs1408_type act_device;
};

struct adgs1408_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* Device Settings */
	enum adgs1408_state	crc_en;
	enum adgs1408_state	burst_mode_en;
	enum adgs1408_state	daisy_chain_en;
	enum adgs1408_state	round_robin_en;
	struct adgs1408_rrobin_config rrobin_sw_config;
	enum adgs1408_type act_device;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Compute CRC8 checksum. */
uint8_t adgs1408_compute_crc8(uint8_t *data,
			      uint8_t data_size);
/* SPI register read from device. */
int32_t adgs1408_spi_reg_read(struct adgs1408_dev *dev,
			      uint8_t reg_addr,
			      uint8_t *reg_data);
/* SPI register write to device. */
int32_t adgs1408_spi_reg_write(struct adgs1408_dev *dev,
			       uint8_t reg_addr,
			       uint8_t reg_data);
/* SPI register read from device using a mask. */
int32_t adgs1408_spi_reg_read_mask(struct adgs1408_dev *dev,
				   uint8_t reg_addr,
				   uint8_t mask,
				   uint8_t *data);
/* SPI internal register write to device using a mask. */
int32_t adgs1408_spi_reg_write_mask(struct adgs1408_dev *dev,
				    uint8_t reg_addr,
				    uint8_t mask,
				    uint8_t data);
/* Do a software reset. */
int32_t adgs1408_do_soft_reset(struct adgs1408_dev *dev);
/* Clear the Error Flags Register. */
int32_t adgs1408_clear_err_flags(struct adgs1408_dev *dev);
/* Enter Daisy-Chain Mode. */
int32_t adgs1408_enter_daisy_chain(struct adgs1408_dev *dev);
/* Send Daisy-Chain commands. */
int32_t adgs1408_send_daisy_chain_cmds(struct adgs1408_dev *dev,
				       uint8_t *cmds,
				       uint8_t cmds_size);
/* Enter round robin mode. */
int32_t adgs1408_enter_round_robin(struct adgs1408_dev *dev);
/* Configure Round Robin Mode. */
int32_t adgs1408_configure_round_robin(struct adgs1408_dev *dev,
				       uint8_t cnv_polarity);
/* Exit Round Robin Mode. */
int32_t adgs1408_exit_round_robin(struct adgs1408_dev *dev);
/* Initialize the device. */
int32_t adgs1408_init(struct adgs1408_dev **device,
		      struct adgs1408_init_param init_param);
/* Free the resources allocated by adgs1408_init(). */
int32_t adgs1408_remove(struct adgs1408_dev *dev);
#endif // ADGS1408_H_
