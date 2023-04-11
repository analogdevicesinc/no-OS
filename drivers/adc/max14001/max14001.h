/***************************************************************************//**
 *   @file   max14001.h
 *   @brief  Implementation of max14001.h
 *   @author NAlteza (nathaniel.alteza@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __MAX14001_H__
#define __MAX14001_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX14001_REG_READ(x)		(((x) << 11) & NO_OS_GENMASK(15, 11))
#define MAX14001_REG_WRITE(x,y)		(MAX14001_REG_READ(x) | \
					NO_OS_BIT(10) | \
					(y & MAX14001_REG_DATA_MASK))
#define MAX14001_REG_DATA_MASK		NO_OS_GENMASK(9, 0)
#define VERIFICATION_REG(x)		((x) + 0x10)
#define MAX14001_SPI_REG_WRITE_ENABLE	0x294
#define MAX14001_SPI_REG_WRITE_DISABLE	0x000

/*
 * MAX14001 registers definition
 */
#define MAX14001_ADC_REG        	0x00
#define MAX14001_FADC_REG       	0x01
#define MAX14001_FLAGS_REG      	0x02
#define MAX14001_FLTEN_REG      	0x03
#define MAX14001_THL_REG        	0x04
#define MAX14001_THU_REG        	0x05
#define MAX14001_INRR_REG       	0x06
#define MAX14001_INRT_REG       	0x07
#define MAX14001_INRP_REG       	0x08
#define MAX14001_CFG_REG        	0x09
#define MAX14001_ENBL_REG       	0x0A
#define MAX14001_ACT_REG        	0x0B
#define MAX14001_WEN_REG        	0x0C
#define MAX14001_FLTV_REG       	0x13
#define MAX14001_THLV_REG       	0x14
#define MAX14001_THUV_REG       	0x15
#define MAX14001_INRRV_REG      	0x16
#define MAX14001_INRTV_REG      	0x17
#define MAX14001_INRPV_REG      	0x18
#define MAX14001_CFGV_REG       	0x19
#define MAX14001_ENBLV_REG      	0x1A

/*
 * MAX14001_ACT_REG
 */

/** Reset. Has the same effect as a power on reset. */
#define MAX14001_RSET_MASK		NO_OS_BIT(7)

/** Software reset. Restores all registers to their POR value. */
#define MAX14001_SRES_MASK              NO_OS_BIT(6)

/** Trigger an inrush current pulse. Has no effect when ENA = 0. */
#define MAX14001_INPLS_MASK             NO_OS_BIT(9)

/*
 * MAX14001_FLAGS_REG
 */
#define MAX14001_ADC_FLAG_MASK          NO_OS_BIT(1)
#define MAX14001_INRD_FLAG_MASK         NO_OS_BIT(2)
#define MAX14001_SPI_FLAG_MASK          NO_OS_BIT(3)
#define MAX14001_COM_FLAG_MASK          NO_OS_BIT(4)
#define MAX14001_CRCL_FLAG_MASK         NO_OS_BIT(5)
#define MAX14001_CRCF_FLAG_MASK         NO_OS_BIT(6)
#define MAX14001_FET_FLAG_MASK          NO_OS_BIT(7)
#define MAX14001_MV_FLAG_MASK           NO_OS_BIT(8)

/*
 * MAX14001_FLTEN_REG
 */
#define MAX14001_DYEN_FLTEN_MASK        NO_OS_BIT(0)
#define MAX14001_EADC_FLTEN_MASK        NO_OS_BIT(1)
#define MAX14001_EINRD_FLTEN_MASK       NO_OS_BIT(2)
#define MAX14001_ESPI_FLTEN_MASK        NO_OS_BIT(3)
#define MAX14001_ECOM_FLTEN_MASK        NO_OS_BIT(4)
#define MAX14001_ECRCL_FLTEN_MASK       NO_OS_BIT(5)
#define MAX14001_ECRCF_FLTEN_MASK       NO_OS_BIT(6)
#define MAX14001_EFET_FLTEN_MASK        NO_OS_BIT(7)
#define MAX14001_EMV_FLTEN_MASK         NO_OS_BIT(8)

/*
 * MAX14001_INRP_REG
 */
#define MAX14001_IINR_INRP_MASK         NO_OS_GENMASK(9, 6)
#define MAX14001_IINR_INRP_MODE(a)      ((MAX14001_IINR(a)) << 6)
#define MAX14001_TINR_INRP_MASK         NO_OS_GENMASK(5, 2)
#define MAX14001_TINR_INRP_MODE(a)	((MAX14001_TINR(a)) << 2)
#define MAX14001_DU_INRP_MASK           NO_OS_GENMASK(1, 0)
#define MAX14001_DU_INRP_MODE(a)  	(((a) & 0x3) << 0)

/*
 * MAX14001_CFG_REG
 */
#define MAX14001_IBIAS_CFG_MASK         NO_OS_GENMASK(9, 6)
#define MAX14001_IBIAS_CFG_MODE(a)	((MAX14001_IBIAS(a)) << 6)
#define MAX14001_EXRF_CFG_MASK          NO_OS_BIT(5)
#define MAX14001_EXTI_CFG_MASK          NO_OS_BIT(4)
#define MAX14001_FT_CFG_MASK            NO_OS_GENMASK(3, 2)
#define MAX14001_FT_CFG_MODE(a)		(((a) & 0x3) << 2)
#define MAX14001_FAST_CFG_MASK          NO_OS_BIT(1)
#define MAX14001_IRAW_CFG_MASK          NO_OS_BIT(0)

/*
 * MAX14001_ENBL_REG
 */
#define MAX14001_ENA_ENBL_MASK          NO_OS_BIT(4)

/*
 * 16-bit bitswap
 */
#define REVERSE_UINT16(x)		((((x >> 0) & 1) << 15) | \
					(((x >> 1) & 1 ) << 14) | \
					(((x >> 2) & 1 ) << 13) | \
					(((x >> 3) & 1 ) << 12) | \
					(((x >> 4) & 1 ) << 11) | \
					(((x >> 5) & 1 ) << 10) | \
					(((x >> 6) & 1 ) << 9) | \
					(((x >> 7) & 1 ) << 8) | \
					(((x >> 8) & 1 ) << 7) | \
					(((x >> 9) & 1 ) << 6) | \
					(((x >> 10) & 1) << 5) | \
					(((x >> 11) & 1) << 4) | \
					(((x >> 12) & 1) << 3) | \
					(((x >> 13) & 1) << 2) | \
					(((x >> 14) & 1) << 1) | \
					(((x >> 15) & 1) << 0))

/*
 * MAX14001 Quantization of configuration inputs
 */
#define MAX14001_CFG_MIN              	0
#define MAX14001_CFG_MAX              	0xF
#define MAX14001_CFG_Q(x,y)		(no_os_clamp( \
					NO_OS_DIV_ROUND_CLOSEST(x,y), \
                    			(MAX14001_CFG_MIN),(MAX14001_CFG_MAX)))
/*
 * MAX14001 TINR input to bits equivelent
 */
#define MAX14001_TINR_INC		8	//in terms of ms
#define MAX14001_TINR(x)		MAX14001_CFG_Q(x, MAX14001_TINR_INC)

/*
 * MAX14001 IINR input to bits equivelent
 */
#define MAX14001_IINR_INC		7	//in terms of mA
#define MAX14001_IINR(x)		MAX14001_CFG_Q(x, MAX14001_IINR_INC)

/*
 * MAX14001 IBIAS input to bits equivelent
 */
#define MAX14001_MUL			100
#define MAX14001_IBIAS_INC		25	//increment of 0.25mA
#define MAX14001_IBIAS(x)		MAX14001_CFG_Q((int)(x*MAX14001_MUL), \
					MAX14001_IBIAS_INC)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum max14001_du
 * @brief MAX14001 list of possible duty cycle modes
 */
enum max14001_du {
	DUTY_OFF,	//default, duty cycle limiting function off
	DUTY_1P6,
	DUTY_3P1,
	DUTY_6P3,
};

/**
 * @enum max14001_ft
 * @brief MAX14001 list of possible filtering modes
 */
enum max14001_ft {
	FILTER_OFF,	//default, filtering is off
	AVERAGE_2_READINGS,
	AVERAGE_4_READINGS,
	AVERAGE_8_READINGS,
};

struct max14001_dev {
	/** SPI layer handler*/
	struct no_os_spi_desc   	*spi_desc;
};

struct max14001_init_param {
	/** SPI device descriptor*/
	struct no_os_spi_init_param     spi_init;
};

/** Read from device. */
int max14001_read(struct max14001_dev *dev,
		  uint8_t reg_addr,
		  uint16_t *reg_data);

/** Write to device. */
int max14001_write(struct max14001_dev *dev,
		   uint8_t reg_addr,
		   uint16_t reg_data);

/** SPI write to device using a mask. */
int max14001_reg_update(struct max14001_dev *dev,
			uint8_t reg_addr,
			uint16_t mask,
			uint16_t data);

/** Write to config registers then to corresponding verification register. */
int max14001_write_config_verify(struct max14001_dev *dev,
				 uint8_t reg_addr,
				 uint16_t mask,
				 uint16_t data);

/** Initialize the device. */
int max14001_init(struct max14001_dev **device,
		  struct max14001_init_param init_param);

/** Initialize the configurations of device. */
int max14001_init_config(struct max14001_dev *dev);

/** Enable write to registers. */
int max14001_wen(struct max14001_dev *dev, bool write_enable);

/** Full reset, same effect as power on reset. */
int max14001_full_reset(struct max14001_dev *dev);

/** Software reset. Restores all registers to their POR value. */
int max14001_reg_reset(struct max14001_dev *dev);

/** Trigger an inrush current pulse.*/
int max14001_inpls_reset(struct max14001_dev *dev);

/** Enable/Disable fast inrush mode. */
int max14001_fast_config(struct max14001_dev *dev,
			 bool fast);

/** Selects Inrush comparator input multiplexer. */
int max14001_iraw_config(struct max14001_dev *dev,
			 bool raw_data);

/** Enable MV fault. */
int max14001_emv_config(struct max14001_dev *dev,
			bool mode);

/** Enable FET fault. */
int max14001_efet_config(struct max14001_dev *dev,
			 bool mode);

/** Enable CRCF fault. */
int max14001_ecrcf_config(struct max14001_dev *dev,
			  bool mode);

/** Enable CRCL fault. */
int max14001_ecrcl_config(struct max14001_dev *dev,
			  bool mode);

/** Enable COM fault. */
int max14001_ecom_config(struct max14001_dev *dev,
			 bool mode);

/** Enable SPI fault. */
int max14001_espi_config(struct max14001_dev *dev,
			 bool mode);

/** Enable INRD fault. */
int max14001_einrd_config(struct max14001_dev *dev,
			  bool mode);

/** Enable ADC fault. */
int max14001_eadc_config(struct max14001_dev *dev,
			 bool mode);

/** Enable dynamic FAULT signal. */
int max14001_dyen_config(struct max14001_dev *dev,
			 bool mode);

/** Enable/Disable field-side current sink. */
int max14001_ena_config(struct max14001_dev *dev,
			bool enable);

/** Selects the voltage reference source for the ADC. */
int max14001_exrf_config(struct max14001_dev *dev,
			 bool mode);

/** Current source connection. */
int max14001_exti_config(struct max14001_dev *dev,
			 bool mode);

/** Configure 4-bit inrush time. */
int max14001_tinr_config(struct max14001_dev *dev,
			 int mode);

/** Configure 4-bit inrush current. */
int max14001_iinr_config(struct max14001_dev *dev,
			 int mode);

/** Set maximum duty cycle for inrush current over the last 10 seconds. */
int max14001_du_config(struct max14001_dev *dev,
		       enum max14001_du mode);

/** Configure 4-bit bias current. */
int max14001_ibias_config(struct max14001_dev *dev,
			  float mode);

/** Set number of readings averaged in the ADC filter. */
int max14001_ft_config(struct max14001_dev *dev,
		       enum max14001_ft mode);

/** Get adc data. */
int max14001_get_data_raw(struct max14001_dev *dev, uint16_t *data);

/** Get filtered adc data. */
int max14001_get_data_filtered(struct max14001_dev *dev, uint16_t *data);

/** Free the resources allocated by max14001_init(). */
int max14001_remove(struct max14001_dev *dev);

#endif /* __MAX14001_H__ */
