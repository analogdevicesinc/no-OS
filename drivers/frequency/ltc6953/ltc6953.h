/***************************************************************************//**
 *   @file   ltc6953.h
 *   @brief  Implementation of LTC6953 Driver.
 *   @author MTinaco (mariel.tinaco@analog.com)
********************************************************************************
 * Copyright 2023-2024(c) Analog Devices, Inc.
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

#ifndef __LTC6953_H__
#define __LTC6953_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define LTC6953_NUM_REGADDR		57

/* Register address macro */
#define LTC6953_REG_VCO_STATUS		0x00
#define LTC6953_REG_STAT		0x01
#define LTC6953_REG_PD_CTL		0x02
#define LTC6953_REG_CHAN_POWER_DOWN(x)	0x03 + (x / 4)
#define LTC6953_REG_TEMP		0x05
#define LTC6953_REG_SYNC_CONFIG		0x0B
#define LTC6953_REG_OUTPUT_DIVIDER(x)	0x0C + (x * 4)
#define LTC6953_REG_OUTPUT_CONFIG(x)	0x0D + (x * 4)
#define LTC6953_REG_DDEL_HIGH(x)	0x0D + (x * 4)
#define LTC6953_REG_DDEL_LOW(x)		0x0E + (x * 4)
#define LTC6953_REG_ADEL(x)		0x0F + (x * 4)
#define LTC6953_REG_CHIP_INFO		0x38

/* LTC6952_REG0 */
#define LTC6953_VCOOK_MSK		NO_OS_BIT(2)
#define LTC6953_NVCOOK_MSK		NO_OS_BIT(3)

/* LTC6952_REG1 */
#define LTC6953_INVSTAT_MSK		NO_OS_BIT(7)
#define LTC6953_STAT_OUT_MSK		NO_OS_GENMASK(6, 0)

/* LTC6952_REG2 */
#define LTC6953_PDALL_MSK		NO_OS_BIT(7)
#define LTC6953_PDVCOPK_MSK		NO_OS_BIT(5)
#define LTC6953_FILTV_MSK		NO_OS_BIT(1)
#define LTC6953_POR_MSK			NO_OS_BIT(0)

/* LTC6953 REG3, 4, 5 */
#define LTC6953_PD_MSK(ch)		NO_OS_GENMASK( \
						((ch) & NO_OS_GENMASK(1, 0)) * 2 + 1, \
						((ch) & NO_OS_GENMASK(1, 0)) * 2)
#define LTC6953_PD(ch, x)		no_os_field_prep(LTC6953_PD_MSK(ch), x)

/* LTC6953 REG5 */
#define LTC6953_TEMPO_MSK		NO_OS_BIT(7)

/* LTC6953 REG11 */
#define LTC6953_EZMD_MSK		NO_OS_BIT(4)
#define LTC6953_SRQMD_MSK		NO_OS_BIT(3)
#define LTC6953_SYSCT_MSK		NO_OS_GENMASK(2, 1)
#define LTC6953_SSRQ_MSK		NO_OS_BIT(0)

/* LTC6953 REG12,16,20,24,28,32,36,40,44,48,52 */
#define LTC6953_MP_MSK			NO_OS_GENMASK(7, 3)
#define LTC6953_MD_MSK			NO_OS_GENMASK(2, 0)

/* LTC6953_REG13,17,21,25,29,33,37,41,45,49,53 */
#define LTC6953_SRQEN_MSK		NO_OS_BIT(7)
#define LTC6953_MODE_MSK		NO_OS_GENMASK(6, 5)
#define LTC6953_OINV_MSK		NO_OS_BIT(4)
#define LTC6953_DDEL_HIGH_MSK		NO_OS_GENMASK(3, 0)

/* LTC6953_REG14,18,22,26,30,34,38,42,46,50,54 */
#define LTC6953_DDEL_LOW_MSK		NO_OS_GENMASK(7, 0)

/* LTC6953_REG15,19,23,27,31,35,39,43,47,51,55 */
#define LTC6953_ADEL_MSK		NO_OS_GENMASK(5, 0)

/* LTC6953_REG56 */
#define LTC6953_REV_MSK			NO_OS_GENMASK(7, 4)
#define LTC6953_PART_MSK		NO_OS_GENMASK(3, 0)

#define LTC6953_NUM_CHAN		11

#define LTC6953_OUT_DIV_MIN		1
#define LTC6953_OUT_DIV_MAX		1048576
#define LTC6953_X_MAX			127

#define LTC6953_ADDRX 			0
#define LTC6953_DXMSB 			1
#define LTC6953_NUMBITS 		2
#define LTC6953_R_ONLY 			3

/* Specifications */
#define LTC6953_SPI_WRITE_CMD		0x00
#define LTC6953_SPI_READ_CMD		0x01
#define LTC6953_SPI_ADDR_CMD(x)		((x) << 1)
#define LTC6953_BUFF_SIZE_BYTES		2
#define LTC6953_DUMMY_BYTES		0x00


/******************************************************************************/
/*************************** Types Definitions ********************************/
/******************************************************************************/

/**
 * @struct ltc6953_channel_spec
 * @brief Output channel configuration
 */
struct ltc6953_channel_spec {
	uint8_t		num;
	uint8_t		out_divider;
	uint8_t		mp;
	uint8_t		md;
	uint8_t		digital_delay;
	uint8_t		analog_delay;
	uint8_t		sysref_mode;
	uint8_t		power_down_mode;
	int8_t   	extended_name[16];
};

/**
 * @struct ltc6953_init_param
 * @brief LTC6953 Initialization Parameters structure.
 */
struct ltc6953_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	spi_init;
	struct ltc6953_channel_spec chans[LTC6953_NUM_CHAN];
	float vco_frequency;
};

/**
 * @struct ltc6953_dev
 * @brief LTC6953 Device Descriptor.
 */
struct ltc6953_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	struct ltc6953_channel_spec chans[LTC6953_NUM_CHAN];
	float vco_frequency;
};

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/

/** LTC6953 SPI Initialization **/
int ltc6953_init(struct ltc6953_dev **dev,
		 struct ltc6953_init_param *init_param);

/** LTC6953 Resource deallocation **/
int ltc6953_remove(struct ltc6953_dev *dev);

/** LTC6953 Power-on reset **/
int ltc6953_reset(struct ltc6953_dev *dev);

/** LTC6953 SPI Single byte Write register**/
int ltc6953_write(struct ltc6953_dev *dev, uint8_t addr,
		  uint16_t data);

/** LTC6953 SPI Single byte Read register**/
int ltc6953_read(struct ltc6953_dev *dev, uint8_t addr,
		 uint8_t *data);

/** LTC6953 SPI Single byte Update register **/
int ltc6953_update(struct ltc6953_dev *dev, uint8_t reg_addr,
		   uint8_t mask, uint8_t data);

/** LTC6953 Power down all channels  **/
int ltc6953_power_down_all(struct ltc6953_dev *dev, bool is_pwdn);

/** LTC6953 Enable input filter **/
int ltc6953_enable_filter(struct ltc6953_dev *dev, bool is_en);

/** LTC6953 Power Down VCO input **/
int ltc6953_power_down_vco(struct ltc6953_dev *dev, bool is_pwdn);

/** LTC6953 Set single channel output divider **/
int ltc6953_set_output_divider(struct ltc6953_dev *dev, uint32_t channel,
			       uint32_t divider);

/** LTC6953 Set single channel power mode **/
int ltc6953_power_mode(struct ltc6953_dev *dev, uint32_t channel,
		       int32_t mode);

/** LTC6953 Enable single channel sync **/
int ltc6953_enable_sync(struct ltc6953_dev *dev, uint32_t channel,
			bool enable);

/** LTC6953 Set single channel digital delay **/
int ltc6953_set_digital_delay(struct ltc6953_dev *dev, uint32_t channel,
			      uint16_t delay);

/** LTC6953 Set single channel analog delay **/
int ltc6953_set_analog_delay(struct ltc6953_dev *dev, uint32_t channel,
			     uint16_t delay);

/** LTC6953 Set single channel mode **/
int ltc6953_set_mode(struct ltc6953_dev *dev, uint32_t channel,
		     uint8_t mode);

/** LTC6953 Enable single channel output inversion **/
int ltc6953_invert_output(struct ltc6953_dev *dev, uint32_t channel,
			  bool is_invert);

/** LTC6953 Set EZSYNC mode **/
int ltc6953_ezsync_mode(struct ltc6953_dev *dev, bool is_en);

/** LTC6953 Set SRQ mode **/
int ltc6953_sync_mode(struct ltc6953_dev *dev, bool is_en);

/** LTC6953 Set SSRQ **/
int ltc6953_ssrq_mode(struct ltc6953_dev *dev, bool is_en);

/** LTC6953 Set SYSCT **/
int ltc6953_num_pulse(struct ltc6953_dev *dev, uint8_t num_pulse);

/** LTC6953 Enable Temperature Measurement from STAT pin **/
int ltc6953_enable_temp_stat(struct ltc6953_dev *dev, bool is_en);

/** LTC6953 Check VCO Input status **/
int ltc6953_vco_status(struct ltc6953_dev *dev, bool *is_ok);

/** LTC6953 Get INVSTAT bit **/
int ltc6953_get_invstat(struct ltc6953_dev *dev, bool *status);

/** LTC6953 Set INVSTAT bit **/
int ltc6953_set_invstat(struct ltc6953_dev *dev, bool status);

/** LTC6953 Set X **/
int ltc6953_set_x(struct ltc6953_dev *dev, uint8_t x);

/** LTC6953 Get X **/
int ltc6953_get_x(struct ltc6953_dev *dev, uint8_t *x);

/** LTC6953 Get Revision Number **/
int ltc6953_read_rev(struct ltc6953_dev *dev, uint8_t *rev);

/** LTC6953 Get Part Number **/
int ltc6953_read_part(struct ltc6953_dev *dev, uint8_t *part);

#endif // __LTC6953_H__
