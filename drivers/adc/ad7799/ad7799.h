/***************************************************************************//**
 *   @file   ad7799.h
 *   @brief  Header file of AD7798/AD7799 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#ifndef AD7799_H_
#define AD7799_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Types Declarations *************************/
/******************************************************************************/

/*AD7799 Registers*/
#define AD7799_REG_COMM		0x0 /* Communications Register(WO, 8-bit) */
#define AD7799_REG_STAT	    	0x0 /* Status Register (RO, 8-bit) */
#define AD7799_REG_MODE	    	0x1 /* Mode Register (RW, 16-bit) */
#define AD7799_REG_CONF	    	0x2 /* Configuration Register (RW, 16-bit)*/
#define AD7799_REG_DATA	    	0x3 /* Data Register (RO, 16-/24-bit) */
#define AD7799_REG_ID	    	0x4 /* ID Register (RO, 8-bit) */
#define AD7799_REG_IO	    	0x5 /* IO Register (RO, 8-bit) */
#define AD7799_REG_OFFSET   	0x6 /* Offset Register (RW, 24-bit) */
#define AD7799_REG_FULLSCALE	0x7 /* Full-Scale Register (RW, 24-bit) */

/* AD7799 Polarity */
#define AD7799_BIPOLAR 		0x0 /* Bipolar bit */
#define AD7799_UNIPOLAR		0x1 /* Unipolar bit */

/* Communications Register Bit Designations (AD7799_REG_COMM) */
#define AD7799_COMM_WEN		0x80 /* Write Enable */
#define AD7799_COMM_WRITE	0x00 /* Write Operation */
#define AD7799_COMM_READ    	0x40 /* Read Operation */
#define AD7799_COMM_ADDR(x)	(((x) & 0x7) << 3) /* Register Address */
#define AD7799_COMM_CREAD	0x04 /* Continuous Read */

/* Status Register Bit Designations (AD7799_REG_STAT) */
#define AD7799_STAT_RDY		0x80 /* Ready */
#define AD7799_STAT_ERR		0x40 /* Error (Overrange, Underrange) */
#define AD7799_STAT_CH3		0x04 /* Channel 3 */
#define AD7799_STAT_CH2		(1 << 1) /* Channel 2 */
#define AD7799_STAT_CH1		(1 << 0) /* Channel 1 */

/* Mode Register Bit Designations (AD7799_REG_MODE) */
#define AD7799_MODE_SEL(x)	(((x) & 0x7) << 13) /* Operation Mode Select */
#define AD7799_MODE_PSW(x)	0x1000 /* Power Switch Control Bit */
#define AD7799_MODE_RATE(x)	((x) & 0xF) /* Filter Update Rate */

/* AD7799_MODE_SEL(x) options */
#define AD7799_MODE_CONT	 0x0 /* Continuous Conversion Mode */
#define AD7799_MODE_SINGLE	 0x1 /* Single Conversion Mode */
#define AD7799_MODE_IDLE	 0x2 /* Idle Mode */
#define AD7799_MODE_PWRDN	 0x3 /* Power-Down Mode */
#define AD7799_MODE_CAL_INT_ZERO 0x4 /* Internal Zero-Scale Calibration */
#define AD7799_MODE_CAL_INT_FULL 0x5 /* Internal Full-Scale Calibration */
#define AD7799_MODE_CAL_SYS_ZERO 0x6 /* System Zero-Scale Calibration */
#define AD7799_MODE_CAL_SYS_FULL 0x7 /* System Full-Scale Calibration */

/* Configuration Register Bit Designations (AD7799_REG_CONF) */
#define AD7799_CONF_BO_EN	 0x2000 /* Burnout Current */
#define AD7799_CONF_POLARITY(x)  (((x) & 0x1) << 12) /* Unipolar/Bipolar */
#define AD7799_CONF_GAIN(x)	 (((x) & 0x7) << 8) /* Gain Select */
#define AD7799_CONF_REFDET(x)    (((x) & 0x1) << 5) /* Reference detect */
#define AD7799_CONF_BUF		 0x10 /* Buffered Mode Enable */
#define AD7799_CONF_CHAN(x)	 ((x) & 0x7) /* Channel select */

/* AD7799_CONF_GAIN(x) options */
#define AD7799_GAIN_1       	 0x0
#define AD7799_GAIN_2       	 0x1
#define AD7799_GAIN_4       	 0x2
#define AD7799_GAIN_8       	 0x3
#define AD7799_GAIN_16      	 0x4
#define AD7799_GAIN_32      	 0x5
#define AD7799_GAIN_64      	 0x6
#define AD7799_GAIN_128     	 0x7

/* AD7799 Register size */
#define AD7799_REG_SIZE_1B	 0x1
#define AD7799_REG_SIZE_2B	 0x2
#define AD7799_REG_SIZE_3B	 0x3

/* AD7799_CONF_REFDET(x) options */
#define AD7799_REFDET_ENA   	 0x1
#define AD7799_REFDET_DIS   	 0x0

/* AD7799_CONF_CHAN(x) options */
#define AD7799_CH_AIN1P_AIN1M	 0x0 /* AIN1(+) - AIN1(-) */
#define AD7799_CH_AIN2P_AIN2M	 0x1 /* AIN2(+) - AIN2(-) */
#define AD7799_CH_AIN3P_AIN3M	 0x2 /* AIN3(+) - AIN3(-) */
#define AD7799_CH_AIN1M_AIN1M	 0x3 /* AIN1(-) - AIN1(-) */
#define AD7799_CH_AVDD_MONITOR	 0x7 /* AVDD Monitor */

/* ID Register Bit Designations (AD7799_REG_ID) */
#define AD7799_ID_MASK		 0xF

/* AD7799 Configuration Mask */
#define AD7799_REG_MASK		 0xF

/* IO (Excitation Current Sources) Register Bit Designations (AD7799_REG_IO) */
#define AD7799_IOEN		 0x40
#define AD7799_IO1(x)		 (((x) & 0x1) << 4)
#define AD7799_IO2(x)		 (((x) & 0x1) << 5)

/* AD7799 Timeout */
#define AD7799_TIMEOUT		 0xFFFF

/* AD7799 Reset Sequence */
#define AD7799_RESET_DATA	 0xFF

/**
 * @enum ad7799_type
 * @brief Device type (AD7798/AD7799)
 */
enum ad7799_type {
	/** AD7798 device*/
	ID_AD7798 = 0x8,
	/** AD7799 device */
	ID_AD7799 = 0x9
};

enum ad7799_precision {
	/** ADC channel precision in mV */
	AD7799_PRECISION_MV,
	/** ADC channel precision in uV */
	AD7799_PRECISION_UV,
};

/**
 * @struct ad7799_dev
 * @brief AD7798/AD7799 Device description
 */
struct ad7799_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
	/** Chip type (AD7798/AD7799) */
	uint8_t chip_type;
	/** Register size */
	const uint8_t *reg_size;
	/** Gain */
	uint8_t gain;
	/** Unipolar/Bipolar Coding */
	bool polarity;
	/** Reference Voltage in mV */
	uint32_t vref_mv;
	/** ADC channel precision (mV/uV) **/
	enum ad7799_precision precision;
};

/**
 * @struct ad7799_init_param
 * @brief AD7798/AD7799 Device initialization parameters
 */
struct ad7799_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param spi_init;
	/** Chip type (AD7798/AD7799) */
	enum ad7799_type chip_type;
	/** Gain */
	uint8_t gain;
	/** Unipolar/Bipolar Coding */
	bool polarity;
	/** Reference Voltage in mV */
	uint32_t vref_mv;
	/** ADC channel precision (mV/uV) **/
	enum ad7799_precision precision;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read device register. */
int32_t ad7799_read(struct ad7799_dev *device, uint8_t reg_addr,
		    uint32_t *reg_data);

/* Write device register */
int32_t ad7799_write(struct ad7799_dev *device, uint8_t reg_addr,
		     uint32_t reg_data);

/* Software reset of the device. */
int32_t ad7799_reset(struct ad7799_dev *device);

/* Set the device mode. */
int32_t ad7799_set_mode(struct ad7799_dev *device, uint8_t mode);

/* Select the ADC channel. */
int32_t ad7799_set_channel(struct ad7799_dev *device, uint8_t ch);

/* Read specific ADC channel. */
int32_t ad7799_get_channel(struct ad7799_dev *device, uint8_t ch,
			   uint32_t *reg_data);

/* Read specific ADC channel data with the specified precision. */
int32_t ad7799_read_channel(struct ad7799_dev *device, uint8_t ch,
			    int32_t *data_scaled);

/* Set the ADC gain. */
int32_t ad7799_set_gain(struct ad7799_dev *device, uint8_t gain);

/* Get the ADC gain. */
int32_t ad7799_get_gain(struct ad7799_dev *device, uint8_t *gain);

/* Enable or disable the reference detect function. */
int32_t ad7799_set_refdet(struct ad7799_dev *device, uint8_t ref_en);

/* Set ADC polarity. */
int32_t ad7799_set_polarity(struct ad7799_dev *device, uint8_t polarity);

/* Check the status of the device. */
int32_t ad7799_dev_ready(struct ad7799_dev *device);

/* Initialize the device. */
int32_t ad7799_init(struct ad7799_dev **device,
		    const struct ad7799_init_param *init_param);

/* Remove the device and release resources. */
int32_t ad7799_remove(struct ad7799_dev *device);

#endif /* AD7799_H_ */
