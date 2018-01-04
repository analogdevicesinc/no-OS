/***************************************************************************//**
 *   @file   ad738x.h
 *   @brief  Header file for AD738x Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

#ifndef SRC_AD738X_H_
#define SRC_AD738X_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h.
 */
#define GENMASK(h, l) \
		(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (31 - (h))))

/*
 * AD738X registers definition
 */
#define AD738X_REG_NOP                  0x00
#define AD738X_REG_CONFIG1              0x01
#define AD738X_REG_CONFIG2              0x02
#define AD738X_REG_ALERT                0x03
#define AD738X_REG_ALERT_LOW_TH         0x04
#define AD738X_REG_ALERT_HIGH_TH        0x05

/*
 * AD738X_REG_CONFIG1
 */
#define AD738X_CONFIG1_OS_MODE_MSK      BIT(9)
#define AD738X_CONFIG1_OS_MODE(x)       (((x) & 0x1) << 9)
#define AD738X_CONFIG1_OSR_MSK          GENMASK(8, 6)
#define AD738X_CONFIG1_OSR(x)           (((x) & 0x7) << 6)
#define AD738X_CONFIG1_CRC_W_MSK        BIT(5)
#define AD738X_CONFIG1_CRC_W(x)         (((x) & 0x1) << 5)
#define AD738X_CONFIG1_CRC_R_MSK        BIT(4)
#define AD738X_CONFIG1_CRC_R(x)         (((x) & 0x1) << 4)
#define AD738X_CONFIG1_ALERTEN_MSK      BIT(3)
#define AD738X_CONFIG1_ALERTEN(x)       (((x) & 0x1) << 3)
#define AD738X_CONFIG1_RES_MSK          BIT(2)
#define AD738X_CONFIG1_RES(x)           (((x) & 0x1) << 2)
#define AD738X_CONFIG1_REFSEL_MSK       BIT(1)
#define AD738X_CONFIG1_REFSEL(x)        (((x) & 0x1) << 1)
#define AD738X_CONFIG1_PMODE_MSK        BIT(0)
#define AD738X_CONFIG1_PMODE(x)         (((x) & 0x1) << 0)

/*
 * AD738X_REG_CONFIG2
 */
#define AD738X_CONFIG2_SDO2_MSK         BIT(8)
#define AD738X_CONFIG2_SDO2(x)          (((x) & 0x1) << 8)
#define AD738X_CONFIG2_SDO4_MSK         GENMASK(9, 8)
#define AD738X_CONFIG2_SDO4(x)          (((x) & 0x3) << 8)
#define AD738X_CONFIG2_RESET_MSK        GENMASK(7, 0)
#define AD738X_CONFIG2_RESET(x)         (((x) & 0xFF) << 0)

/*
 * AD738X_REG_ALERT_LOW_TH
 */
#define AD738X_ALERT_LOW_MSK            GENMASK(11, 0)
#define AD738X_ALERT_LOW(x)             (((x) & 0xFFF) << 0)

/*
 * AD738X_REG_ALERT_HIGH_TH
 */
#define AD738X_ALERT_HIGH_MSK           GENMASK(11, 0)
#define AD738X_ALERT_HIGH(x)            (((x) & 0xFFF) << 0)

/* Write to register x */
#define AD738X_REG_WRITE(x)             ((1 << 7) | ((x & 0x7) << 4))
/* Read from register x */
#define AD738X_REG_READ(x)              ((x & 0x7) << 4)

/* Size of an array in bytes */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	TWO_WIRE_MODE,
	ONE_WIRE_MODE
} ad738x_conv_mode;

typedef enum {
	NORMAL_OS_MODE,
	ROLLING_OS_MODE
} ad738x_os_mode;

typedef enum {
	OSR_DISABLED,
	OSR_X2,
	OSR_X4,
	OSR_X8,
	OSR_X16,
	OSR_X32,
} ad738x_os_ratio;

typedef enum {
	RES_16_BIT,
	RES_18_BIT
} ad738x_resolution;

typedef enum {
	SOFT_RESET,
	HARD_RESET
} ad738x_reset_type;

typedef enum {
	NORMAL_PWDM,
	FULL_PWDM
} ad738x_pwd_mode;

typedef enum {
	INT_REF,
	EXT_REF
} ad738x_ref_sel;

typedef struct {
	/* SPI */
	spi_desc		*spi_desc;
	/* Device Settings */
	ad738x_conv_mode 	conv_mode;
	ad738x_resolution 	resolution;
} ad738x_dev;

typedef struct {
	/* SPI */
	spi_init_param		spi_init;
	/* Device Settings */
	ad738x_conv_mode	conv_mode;
	ad738x_ref_sel		ref_sel;
} ad738x_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad738x_init(ad738x_dev **device,
		    ad738x_init_param init_param);
int32_t ad738x_remove(ad738x_dev *dev);
int32_t ad738x_spi_reg_read(ad738x_dev *dev,
			    uint8_t reg_addr,
			    uint16_t *reg_data);
int32_t ad738x_spi_reg_write(ad738x_dev *dev,
			     uint8_t reg_addr,
			     uint16_t reg_data);
int32_t ad738x_spi_single_conversion(ad738x_dev *dev,
				     uint16_t *adc_data);
int32_t ad738x_spi_write_mask(ad738x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t mask,
			      uint16_t data);
int32_t ad738x_set_conversion_mode(ad738x_dev *dev,
				   ad738x_conv_mode mode);
int32_t ad738x_reset(ad738x_dev *dev,
		     ad738x_reset_type reset);
int32_t ad738x_oversampling_config(ad738x_dev *dev,
				   ad738x_os_mode os_mode,
				   ad738x_os_ratio os_ratio,
				   ad738x_resolution res);
int32_t ad738x_power_down_mode(ad738x_dev *dev,
			       ad738x_pwd_mode pmode);
int32_t ad738x_reference_sel(ad738x_dev *dev,
			     ad738x_ref_sel ref_sel);
void mdelay(uint32_t msecs);
#endif /* SRC_AD738X_H_ */
