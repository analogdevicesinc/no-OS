/***************************************************************************//**
 *   @file   ad7616.h
 *   @brief  Header file of AD7616 Driver.
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
#ifndef AD7616_H_
#define AD7616_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7616_REG_CONFIG				0x02
#define AD7616_REG_CHANNEL				0x03
#define AD7616_REG_INPUT_RANGE_A1		0x04
#define AD7616_REG_INPUT_RANGE_A2		0x05
#define AD7616_REG_INPUT_RANGE_B1		0x06
#define AD7616_REG_INPUT_RANGE_B2		0x07
#define AD7616_REG_SEQUENCER_STACK(x)	(0x20 + (x))

/* AD7616_REG_CONFIG */
#define AD7616_SDEF						(1 << 7)
#define AD7616_BURSTEN					(1 << 6)
#define AD7616_SEQEN					(1 << 5)
#define AD7616_OS(x)					(((x) & 0x7) << 2)
#define AD7616_STATUSEN					(1 << 1)
#define AD7616_CRCEN					(1 << 0)

/* AD7616_REG_INPUT_RANGE */
#define AD7616_INPUT_RANGE(ch, x)		(((x) & 0x3) << (((ch) & 0x3) * 2))

/* AD7616_REG_SEQUENCER_STACK(x) */
#define AD7616_ADDR(x)					(((x) & 0x7F) << 9)
#define AD7616_SSREN					(1 << 8)
#define AD7616_BSEL(x)					(((x) & 0xF) << 4)
#define AD7616_ASEL(x)					(((x) & 0xF) << 0)

/* AD7616_REG_STATUS */
#define AD7616_STATUS_A(x)				(((x) & 0xF) << 12)
#define AD7616_STATUS_B(x)				(((x) & 0xF) << 8)
#define AD7616_STATUS_CRC(x)			(((x) & 0xFF) << 0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	AD7616_SW,
	AD7616_HW,
} ad7616_mode;

typedef enum {
	AD7616_SERIAL,
	AD7616_PARALLEL,
} ad7616_interface;

typedef enum {
	AD7616_VA0,
	AD7616_VA1,
	AD7616_VA2,
	AD7616_VA3,
	AD7616_VA4,
	AD7616_VA5,
	AD7616_VA6,
	AD7616_VA7,
	AD7616_VB0,
	AD7616_VB1,
	AD7616_VB2,
	AD7616_VB3,
	AD7616_VB4,
	AD7616_VB5,
	AD7616_VB6,
	AD7616_VB7,
} ad7616_ch;

typedef enum {
	AD7616_2V5 = 1,
	AD7616_5V  = 2,
	AD7616_10V = 3,
} ad7616_range;

typedef enum {
	AD7616_OSR_0,
	AD7616_OSR_2,
	AD7616_OSR_4,
	AD7616_OSR_8,
	AD7616_OSR_16,
	AD7616_OSR_32,
	AD7616_OSR_64,
	AD7616_OSR_128,
} ad7616_osr;

typedef struct {
	/* SPI */
	spi_device			spi_dev;
	/* GPIO */
	gpio_device			gpio_dev;
	int8_t				gpio_hw_rngsel0;
	int8_t				gpio_hw_rngsel1;
	int8_t				gpio_reset;
	int8_t				gpio_os0;
	int8_t				gpio_os1;
	int8_t				gpio_os2;
	/* Device Settings */
	ad7616_interface	interface;
	ad7616_mode			mode;
	ad7616_range		va[8];
	ad7616_range		vb[8];
	ad7616_osr			osr;
	adc_core			*core;
} ad7616_dev;

typedef struct {
	/* SPI */
	uint8_t				spi_chip_select;
	spi_mode			spi_mode;
	spi_type			spi_type;
	uint32_t			spi_device_id;
	/* GPIO */
	gpio_type			gpio_type;
	uint32_t			gpio_device_id;
	int8_t				gpio_hw_rngsel0;
	int8_t				gpio_hw_rngsel1;
	int8_t				gpio_reset;
	int8_t				gpio_os0;
	int8_t				gpio_os1;
	int8_t				gpio_os2;
	/* Device Settings */
	ad7616_mode			mode;
	ad7616_range		va[8];
	ad7616_range		vb[8];
	ad7616_osr			osr;
} ad7616_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* SPI read from device. */
int32_t ad7616_read(ad7616_dev *dev,
					uint8_t reg_addr,
					uint16_t *reg_data);
/* SPI write to device. */
int32_t ad7616_write(ad7616_dev *dev,
					 uint8_t reg_addr,
					 uint16_t reg_data);
/* SPI read from device using a mask. */
int32_t ad7616_read_mask(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t mask,
						 uint16_t *data);
/* SPI write to device using a mask. */
int32_t ad7616_write_mask(ad7616_dev *dev,
						  uint8_t reg_addr,
						  uint16_t mask,
						  uint16_t data);
/* SPI read from device. */
int32_t ad7616_spi_read(ad7616_dev *dev,
						uint8_t reg_addr,
						uint16_t *reg_data);
/* SPI write to device. */
int32_t ad7616_spi_write(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t reg_data);
/* PAR read from device. */
int32_t ad7616_par_read(ad7616_dev *dev,
						uint8_t reg_addr,
						uint16_t *reg_data);
/* PAR write to device. */
int32_t ad7616_par_write(ad7616_dev *dev,
						 uint8_t reg_addr,
						 uint16_t reg_data);
/* Perform a full reset of the device. */
int32_t ad7616_reset(ad7616_dev *dev);
/* Set the analog input range for the selected analog input channel. */
int32_t ad7616_set_range(ad7616_dev *dev,
						 ad7616_ch ch,
						 ad7616_range range);
/* Set the operation mode (software or hardware). */
int32_t ad7616_set_mode(ad7616_dev *dev,
						ad7616_mode mode);
/* Set the oversampling ratio. */
int32_t ad7616_set_oversampling_ratio(ad7616_dev *dev,
									  ad7616_osr osr);
/* Initialize the device. */
int32_t ad7616_setup(ad7616_dev **device,
					 adc_core *core,
					 ad7616_init_param init_param);
#endif
