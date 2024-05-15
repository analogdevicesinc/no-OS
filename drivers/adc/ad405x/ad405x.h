/*******************************************************************************
 *   @file   ad405x.h
 *   @brief  Header file of AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
 *	 @author Ribhu Das Purkayastha (ribhu.daspurkayastha@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __AD405X_H__
#define __AD405X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** Register Definition */
#define AD405X_REG_INTERFACE_CONFIG_A		0x00
#define AD405X_REG_INTERFACE_CONFIG_B		0x01
#define AD405X_REG_DEVICE_CONFIG		0x02
#define AD405X_REG_DEVICE_TYPE			0x03
#define AD405X_REG_PRODUCT_ID_L			0x04
#define AD405X_REG_PRODUCT_ID_H			0x05
#define AD405X_REG_CHIP_GRADE			0x06
#define AD405X_REG_SCRATCH_PAD			0x0A
#define AD405X_REG_SPI_REVISION			0x0B
#define AD405X_REG_VENDOR_L			0x0C
#define AD405X_REG_VENDOR_H			0x0D
#define AD405X_REG_STREAM_MODE			0x0E
#define AD405X_REG_TRANSFER_CONFIG		0x0F
#define AD405X_REG_INTERFACE_CONFIG_C		0x10
#define AD405X_REG_INTERFACE_STATUS		0x11
#define AD405X_REG_MODE_SET			0x20
#define AD405X_REG_ADC_MODES			0x21
#define AD405X_REG_ADC_CONFIG			0x22
#define AD405X_REG_AVG_CONFIG			0x23
#define AD405X_REG_GPIO_CONFIG			0x24
#define AD405X_REG_INTR_CONFIG			0x25
#define AD405X_REG_CHOP_CONFIG			0x26
#define AD405X_REG_TIMER_CONFIG			0x27
#define AD405X_REG_MAX_THRESH			0x28
#define AD405X_REG_MIN_THRESH			0x2A
#define AD405X_REG_MAX_HYST			0x2C
#define AD405X_REG_MIN_HYST			0x2D
#define AD405X_REG_MON_VAL			0x2E
#define AD405X_REG_FUSE_CRC			0x40
#define AD405X_REG_DEVICE_STATUS		0x41
#define AD405X_REG_MAX_INTR			0x42
#define AD405X_REG_MIN_INTR			0x44

/** AD405X_REG_INTERFACE_CONFIG_A Bit Definition */
#define AD405X_SW_RESET_MSK			NO_OS_BIT(7) | NO_OS_BIT(0)

/** AD405X_REG_INTERFACE_CONFIG_B Bit Definition */
#define AD405X_SINGLE_INST_MSK			NO_OS_BIT(7) /* Should be set to 0. */
#define AD405X_SHORT_INST_MSK			NO_OS_BIT(3)

/** AD405X_REG_INTERFACE_CONFIG_C Bit Definition */
#define AD405X_STRICT_REG_ACCESS_MSK		NO_OS_BIT(5)

/** AD405X_REG_ADC_MODES Bit definition */
#define AD405X_ADC_DATA_FORMAT_MSK      NO_OS_BIT(7)
#define AD405X_ADC_MODES_MSK			NO_OS_GENMASK(1, 0)
#define AD405X_SAMPLE_MODE      		0x00
#define AD405X_BURST_AVERAGING_MODE		NO_OS_BIT(0)
#define AD405X_AVERAGING_MODE      		NO_OS_BIT(1)
#define AD405X_AUTONOMOUS_MODE			NO_OS_GENMASK(1, 0)

/** AD405X_REG_MODE_SET Bit definition */
#define AD405X_ENTER_ADC_MODE_MSK		NO_OS_BIT(0)
#define AD405X_ENTER_ADC_MODE     		NO_OS_BIT(0)

/** AD405X_REG_TIMER_CONFIG Bit Definitions */
#define AD405X_FS_BURST_AUTO_MSK		NO_OS_GENMASK(7, 4)

/** AD405X_REG_AVG_CONFIG Bit Definitions */
#define AD405X_AVG_WIN_LEN_MSK			NO_OS_GENMASK(3, 0)

/** AD405X_REG_GPIO_CONFIG Bit Definitions */
#define AD405X_DEV_EN_POL_MSK				NO_OS_BIT(7)
#define AD405X_GP1_MODE_MSK				NO_OS_GENMASK(6, 4)
#define AD405X_INVERT_ON_CHOP_MSK			NO_OS_BIT(3)
#define AD405X_GP0_MODE_MSK				NO_OS_GENMASK(2, 0)

#define AD405X_DEV_EN_POL_LOW				~NO_OS_BIT(7)
#define AD405X_DEV_EN_POL_HIGH				NO_OS_BIT(7)

#define AD405X_INVERT_ON_CHOP_OFF			~NO_OS_BIT(3)
#define AD405X_INVERT_ON_CHOP_ON			NO_OS_BIT(3)

/** Miscellaneous Definitions */
#define AD405X_SW_RESET				NO_OS_BIT(7) | NO_OS_BIT(0)
#define AD405X_SPI_READ        			NO_OS_BIT(7)
#define BYTE_ADDR_H				NO_OS_GENMASK(15, 8)
#define BYTE_ADDR_L				NO_OS_GENMASK(7, 0)
#define AD405X_DEVICE_TYPE			NO_OS_GENMASK(2, 0)

/******************************************************************************/
/************************ Types Declarations **********************************/
/******************************************************************************/

/** AD405X modes of operations */
enum ad405x_operation_mode {
	AD405X_CONFIG_MODE_OP,
	AD405X_ADC_MODE_OP,
	AD405X_AVERAGING_MODE_OP,
	AD405X_BURST_AVERAGING_MODE_OP,
	AD405X_PERSISTENT_AUTO_MODE_OP,
	AD405X_NON_PERSISTENT_AUTO_MODE_OP
};

/** AD405X sample rate for burst and autonomous modes */
enum ad405x_sample_rate {
	AD405X_2_MSPS,
	AD405X_1_MSPS,
	AD405X_333_KSPS,
	AD405X_100_KSPS,
	AD405X_33_KSPS,
	AD405X_10_KSPS,
	AD405X_3_KSPS,
	AD405X_1_KSPS,
	AD405X_500_SPS,
	AD405X_333_SPS,
	AD405X_250_SPS,
	AD405X_200_SPS,
	AD405X_166_SPS,
	AD405X_140_SPS,
	AD405X_125_SPS,
	AD405X_111_SPS
};

/** AD405X averaging filter window length */
enum ad405x_avg_filter_l {
	AD405X_LENGTH_2,
	AD405X_LENGTH_4,
	AD405X_LENGTH_8,
	AD405X_LENGTH_16,
	AD405X_LENGTH_32,
	AD405X_LENGTH_64,
	AD405X_LENGTH_128,
	AD405X_LENGTH_256,
	AD405X_LENGTH_512,
	AD405X_LENGTH_1024,
	AD405X_LENGTH_2048,
	AD405X_LENGTH_4096
};

/** AD405X DEV_EN polarity */
enum ad405x_dev_en_polarity {
	AD405X_DEV_EN_ACTIVE_LOW,
	AD405X_DEV_EN_ACTIVE_HIGH
};

/** AD405X INVERT_ON_CHOP status */
enum ad405x_invert_on_chop {
	AD405X_INVERT_ON_CHOP_DISABLED,
	AD405X_INVERT_ON_CHOP_ENABLED
};

/** AD405X GP0, GP1 mode */
enum ad405x_gp_mode {
	AD405X_GP_MODE_HIGH_Z,
	AD405X_GP_MODE_INTR,
	AD405X_GP_MODE_DRDY,
	AD405X_GP_MODE_DEV_EN,
	AD405X_GP_MODE_CHOP,
	AD405X_GP_MODE_LOW,
	AD405X_GP_MODE_HIGH,
	AD405X_GP_MODE_DEV_RDY
};

/** AD405X GP selection */
enum ad405x_gp_select {
	AD405X_GP_0,
	AD405X_GP_1
};

/** AD405X output data format selection */
enum ad405x_out_data_format {
	AD405X_STRAIGHT_BINARY,
	AD405X_TWOS_COMPLEMENT
};

/** AD405X device type section */
enum ad405x_device_type {
	ID_AD4050,
	ID_AD4052
};

/** AD405X product ID */
enum ad405x_product_id {
	PROD_ID_AD4052 = 0x70
};

/**
 * @struct ad405x_dev
 * @brief AD405X Device structure.
 */
struct ad405x_dev {
	/** SPI */
	struct no_os_spi_desc	*spi_desc;
	/** Active device */
	enum ad405x_device_type active_device;
	/** Mode of operations */
	enum ad405x_operation_mode operation_mode;
	/** CNV GPIO descriptor */
	struct no_os_gpio_desc *gpio_cnv;
	/** GPIO0 GPIO descriptor */
	struct no_os_gpio_desc *gpio_gpio0;
	/** GPIO1 GPIO descriptor */
	struct no_os_gpio_desc *gpio_gpio1;
	/** Sample rate for Burst and Autonomous Modes */
	enum ad405x_sample_rate rate;
	/** Averaging filter window length */
	enum ad405x_avg_filter_l filter_length;
	/** DEV_EN signal polarity */
	enum ad405x_dev_en_polarity polarity;
	/** GP1 mode */
	enum ad405x_gp_mode gp1_mode;
	/** INVERT_ON_CHOP setting */
	enum ad405x_invert_on_chop invert_on_chop_status;
	/** GP0 mode */
	enum ad405x_gp_mode gp0_mode;
	/** Output data format */
	enum ad405x_out_data_format data_format;
};

/**
 * @struct AD405X_init_param
 * @brief AD405X Device initialization parameters.
 */
struct ad405x_init_param {
	/** SPI */
	struct no_os_spi_init_param	*spi_init;
	/** Active device */
	enum ad405x_device_type active_device;
	/** Mode of operations */
	enum ad405x_operation_mode operation_mode;
	/** CNV GPIO descriptor */
	struct no_os_gpio_init_param *gpio_cnv;
	/** GPIO0 GPIO descriptor */
	struct no_os_gpio_init_param *gpio_gpio0;
	/** GPIO1 GPIO descriptor */
	struct no_os_gpio_init_param *gpio_gpio1;
	/** sample rate for Burst and Autonomous Modes */
	enum ad405x_sample_rate rate;
	/** Averaging filter window length */
	enum ad405x_avg_filter_l filter_length;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Write data into a register */
int ad405x_write(struct ad405x_dev *dev,
		 uint8_t reg_addr,
		 uint8_t reg_val);

/** Read data from a register */
int ad405x_read(struct ad405x_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_val);

/** Update specific register bits */
int ad405x_update_bits(struct ad405x_dev *dev,
		       uint8_t reg_addr,
		       uint8_t mask,
		       uint8_t reg_val);

/** Send EXIT command. */
int ad405x_exit_command(struct ad405x_dev *dev);

/** Send RESET command. */
int ad405x_reset_pattern_command(struct ad405x_dev *dev);

/** Software Reset. */
int ad405x_soft_reset(struct ad405x_dev *dev);

/** Enter ADC Mode */
int ad405x_set_adc_mode(struct ad405x_dev *dev);

/** Enter Burst Averaging Mode */
int ad405x_set_burst_averaging_mode(struct ad405x_dev *dev);

/** Enter Config Mode */
int ad405x_set_config_mode(struct ad405x_dev *dev);

/** Set operation mode */
int ad405x_set_operation_mode(struct ad405x_dev *dev,
			      enum ad405x_operation_mode mode);

/** Toggle the CNV pin to start a conversion. */
int ad405x_convst(struct ad405x_dev *dev);

/** Read conversion data. */
int ad405x_spi_data_read(struct ad405x_dev *dev,
			 int32_t *data);

/** Trigger conversion adn read data. */
int ad405x_read_val(struct ad405x_dev *dev, int32_t *data);

/** Select sample rate for Burst and Autonomous Modes. */
int ad405x_set_sample_rate(struct ad405x_dev *dev,
			   enum ad405x_sample_rate rate);

/** Set averaging filter window length */
int ad405x_set_averaging_filter_length(struct ad405x_dev *dev,
				       enum ad405x_avg_filter_l length);

/** Get averaging filter window length */
enum ad405x_avg_filter_l ad405x_get_averaging_filter_length(
	struct ad405x_dev *dev);

/** Set DEV_EN signal polarity */
int ad405x_set_dev_en_polarity(struct ad405x_dev *dev,
			       enum ad405x_dev_en_polarity polarity);

/** Get DEV_EN signal polarity */
enum ad405x_dev_en_polarity ad405x_get_dev_en_polarity(struct ad405x_dev *dev);

/** Enable INVERT_ON_CHOP */
int ad405x_enable_invert_on_chop(struct ad405x_dev *dev);

/** Disable INVERT_ON_CHOP */
int ad405x_disable_invert_on_chop(struct ad405x_dev *dev);

/** Get INVERT_ON_CHOP setting */
enum ad405x_invert_on_chop ad405x_get_invert_on_chop_state(
	struct ad405x_dev *dev);

/** Set GP mode */
int ad405x_set_gp_mode(struct ad405x_dev *dev,
		       enum ad405x_gp_select gp,
		       enum ad405x_gp_mode mode);

/** Get GP mode setting */
enum ad405x_gp_mode ad405x_get_gp_mode(
	struct ad405x_dev *dev, enum ad405x_gp_select gp);

/** Set adc output data format */
int ad405x_set_data_format(struct ad405x_dev *dev,
			   enum ad405x_out_data_format data_format);

/** Initialize the device. */
int ad405x_init(struct ad405x_dev **device,
		struct ad405x_init_param init_param);

/** Remove the device and release resources. */
int ad405x_remove(struct ad405x_dev *dev);

#endif /* __AD405X_H__ */
