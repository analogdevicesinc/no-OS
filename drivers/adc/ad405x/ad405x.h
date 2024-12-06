/***************************************************************************//**
 *   @file   ad405x.h
 *   @brief  Header file of AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
 *   @author Ribhu Das Purkayastha (ribhu.daspurkayastha@analog.com)
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2024~2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __AD405X_H__
#define __AD405X_H__

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_i3c.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/* Buffer set to ad405x_set_threshold() required size */
#define AD405X_BUFF_SIZE		7

/* SPI commands */
#define AD405X_SPI_READ        		NO_OS_BIT(7)

/* AD405X Register Map */
#define AD405X_REG_INTF_CONFIG_A	0x00
#define AD405X_REG_DEVICE_CONFIG	0x02
#define AD405X_REG_PROD_ID_0		0x04
#define AD405X_REG_PROD_ID_1		0x05
#define AD405X_REG_SCRATCH_PAD		0x0A
#define AD405X_REG_VENDOR_L		0x0C
#define AD405X_REG_VENDOR_H		0x0D
#define AD405X_REG_STREAM_MODE		0x0E
#define AD405X_REG_INTF_STATUS		0x11
#define AD405X_REG_ADC_MODES		0x21
#define AD405X_REG_ADC_CONFIG		0x22
#define AD405X_REG_AVG_CONFIG		0x23
#define AD405X_REG_GPIO_CONFIG		0x24
#define AD405X_REG_INTR_CONFIG		0x25
#define AD405X_REG_CHOP_CONFIG		0x26
#define AD405X_REG_TIMER_CONFIG		0x27
#define AD405X_REG_MAX_THRESH_REG(x)	(0x28 + x)
#define AD405X_REG_MIN_THRESH_REG(x)	(0x30 + x)
#define AD405X_REG_MAX_HYST_REG		0x2C
#define AD405X_REG_MIN_HYST_REG		0x2D
#define AD405X_REG_MON_VAL_REG(x)	(0x2E + x)
#define AD405X_REG_FUSE_CRC		0x40
#define AD405X_REG_DEVICE_STATUS	0x41
#define AD405X_REG_MAX_INTR(x)		(0x42 + x)
#define AD405X_REG_MIN_INTR(x)		(0x43 + x)
/* AD405X SPI Only Register Map */
#define AD405X_REG_MODE_SET		0x20
/* AD405X I3C Only Register Map */
#define AD405X_REG_INTF_IBI_EN		0x30
#define AD405X_REG_ADC_IBI_EN		0x31
#define AD405X_REG_TGT_ADDR		0x46
#define AD405X_REG_GRP_ADDR		0x47
#define AD405X_REG_IBI_STATUS		0x48
#define AD405X_REG_CONV_READ(x)		(0x50 + x)
#define AD405X_REG_CONV_TRIGGER(x)	(0x54 + x)
#define AD405X_REG_CONV_AUTO	        0x58

/* AD405X_REG_INTF_STATUS/AD405X_REG_STATUS Bit definition */
#define AD405X_STATUS_FLAGS_MSK(x)      (0x2F & x)
#define AD405X_INTF_STATUS_FLAGS_MSK(x) (0x9F & x)

/* AD405X_REG_ADC_MODES Bit definition */
#define AD405X_ADC_DATA_FORMAT_MSK      NO_OS_BIT(7)
#define AD405X_ADC_MODES_MSK		NO_OS_GENMASK(1, 0)
#define AD405X_ADC_MODE		      	0x00
#define AD405X_BURST_AVERAGING_MODE	NO_OS_BIT(0)
#define AD405X_AVERAGING_MODE      	NO_OS_BIT(1)
#define AD405X_PERSISTENT_AUTO_MODE	NO_OS_GENMASK(1, 0)
#define AD405X_NONPERSISTENT_AUTO_MODE	NO_OS_GENMASK(1, 0)

/* AD405X_REG_MODE_SET Bit definition */
#define AD405X_ENTER_ADC_MODE_MSK	NO_OS_BIT(0)
#define AD405X_ENTER_ADC_MODE     	NO_OS_BIT(0)
#define AD405X_ENTER_AUTO_MODE_MSK	NO_OS_BIT(2)
#define AD405X_ENTER_AUTO_MODE     	NO_OS_BIT(2)

/* AD405X_REG_TIMER_CONFIG Bit Definitions */
#define AD405X_FS_BURST_AUTO_MSK	NO_OS_GENMASK(7, 4)
#define AD405X_TIMER_PWR_ON_MSK		NO_OS_GENMASK(3, 0)

/* AD405X_REG_AVG_CONFIG Bit Definitions */
#define AD405X_AVG_WIN_LEN_MSK		NO_OS_GENMASK(3, 0)

/* AD405X_REG_GPIO_CONFIG Bit Definitions */
#define AD405X_DEV_EN_POL_MSK		NO_OS_BIT(7)
#define AD405X_GP1_MODE_MSK		NO_OS_GENMASK(6, 4)
#define AD405X_INVERT_ON_CHOP_MSK	NO_OS_BIT(3)
#define AD405X_GP0_MODE_MSK		NO_OS_GENMASK(2, 0)

#define AD405X_CLEAR_RESET_FLAG		NO_OS_BIT(6)
#define AD405X_CLEAR_NOT_RDY_FLAG	NO_OS_BIT(7)

/* Register reset values */
#define AD405X_PROD_ID_0_RESET(x)	(0x70 + x)
#define AD405X_PROD_ID_1_RESET		0x00
#define AD405X_VENDOR_SPI_L_RESET	0x56
#define AD405X_VENDOR_SPI_H_RESET	0x04
#define AD405X_VENDOR_I3C_L_RESET	0x77
#define AD405X_VENDOR_I3C_H_RESET	0x01

/* Reset code for AD405X_REG_INTF_CONFIG_A */
#define AD405X_RESET_CODE		0x81

#define AD405X_I3C_BCR			0x26
#define AD405X_I3C_DCR			0x00
/* Used on the common data */
#define AD405X_I3C_PID_			0x02ee00700000
#define AD405X_I3C_PID_INSTANCE_ID(x)	(((uint64_t)x & 0x7) << 12)
#define AD405X_I3C_PID(x)		(AD405X_I3C_PID_ | AD405X_I3C_PID_INSTANCE_ID(x))

/* Getter for AD405X_REG_INTF_STATUS flags */
#define AD405X_ISTS_NOT_RDY_ERROR(x)		(((x) >> 7) & 1)
/* Getters for AD405X_REG_STATUS flags */
#define AD405X_STS_DEVICE_RESET(x)		(((x) >> 6) & 1)
#define AD405X_STS_DEVICE_READY(x)		(((x) >> 7) & 1)
/* Setters for AD405X_REG_*_IBI_EN flags */
#define AD405X_IBI_STS_EN(x)			((x) & 1)
#define AD405X_IBI_EN_MIN_SET(x)		(((x) & 1) << 8)
#define AD405X_IBI_EN_MAX_SET(x)		(((x) & 1) << 9)
#define AD405X_IBI_EN_CONV_TRIGGER(x)		(((x) & 1) << 10)
/* Getter for AD405X_REG_*_IBI_EN flags */
#define AD405X_IBI_EN_CONV_TRIGGER_GET(x)	(((x) >> 10) & 1)

/** Available AD405X-AD406X parts */
enum ad405x_type {
	ID_AD4050,
	ID_AD4052,
	ID_AD4056,
	ID_AD4058,
	ID_AD4060,
	ID_AD4062,
};

/** Variants comm options */
enum ad405x_comm_type {
	AD405X_SPI_COMM,
	AD405X_I3C_COMM
};

/** Variants comm initialization parameter */
union ad405x_comm_init_param {
	/** I3C Initialization structure. */
	struct no_os_i3c_init_param *i3c_init;
	/** SPI Initialization structure. */
	struct no_os_spi_init_param *spi_init;
};

/**
 * @struct ad405x_init_param
 * @brief Structure holding the parameters for AD405X device initialization.
 */
struct ad405x_init_param {
	/* Device Communication initialization structure: either SPI or I3C */
	union ad405x_comm_init_param comm_init;
	/* Device Communication type: AD405X_SPI_COMM, AD405X_I3C_COMM */
	enum ad405x_comm_type comm_type;
	/* Device type */
	enum ad405x_type dev_type;
	/** CNV GPIO descriptor (SPI only)*/
	struct no_os_gpio_init_param *gpio_cnv;
	/** GPIO0 GPIO descriptor */
	struct no_os_gpio_init_param *gpio_gpio0;
	/** GPIO1 GPIO descriptor */
	struct no_os_gpio_init_param *gpio_gpio1;
};

/** Holds the comm descriptors */
union ad405x_comm_desc {
	/* I3C Descriptor */
	struct no_os_i3c_desc *i3c_desc;
	/* SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
};

/** AD405X modes of operations */
enum ad405x_operation_mode {
	AD405X_ADC_MODE_OP = 0,
	AD405X_BURST_AVERAGING_MODE_OP = 1,
	AD405X_AVERAGING_MODE_OP = 2,
	AD405X_CONFIG_MODE_OP,
	AD405X_PERSISTENT_AUTO_MODE_OP,
	AD405X_NONPERSISTENT_AUTO_MODE_OP
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

/** AD405X DEV_EN power-on timer setting */
enum ad405x_dev_en_timer_pwr_on {
	AD405X_DEV_EN_TIME_250_NS,
	AD405X_DEV_EN_TIME_1_US,
	AD405X_DEV_EN_TIME_3_3_US,
	AD405X_DEV_EN_TIME_10_US,
	AD405X_DEV_EN_TIME_33_US,
	AD405X_DEV_EN_TIME_100_US,
	AD405X_DEV_EN_TIME_333_US,
	AD405X_DEV_EN_TIME_1_MS,
	AD405X_DEV_EN_TIME_2_MS,
	AD405X_DEV_EN_TIME_3_MS,
	AD405X_DEV_EN_TIME_4_MS,
	AD405X_DEV_EN_TIME_5_MS,
	AD405X_DEV_EN_TIME_6_MS,
	AD405X_DEV_EN_TIME_7_MS,
	AD405X_DEV_EN_TIME_8_MS,
	AD405X_DEV_EN_TIME_9_MS
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

/**
 * @struct ad405x_thresh
 * @brief AD405X threshold configuration values.
 * Entry positions matter.
 */
struct ad405x_thresh {
	/* Max threshold hyst */
	uint8_t max_thresh_hyst;
	/* Min threshold hyst */
	uint8_t min_thresh_hyst;
	/* Min threshold value */
	uint16_t min_thresh_val;
	/* Max threshold value */
	uint16_t max_thresh_val;
};

/**
 * @struct ad405x_i3c_extra
 * @brief AD405X Device I3C structure.
 */
struct ad405x_i3c_extra {
	/* Current address */
	uint8_t addr_ptr;
	/* IBI Enable Flags */
	uint16_t ibi_en_mask;
	/* IBI Status Flags */
	uint8_t ibi_sts;
};

/**
 * @struct ad405x_spi_extra
 * @brief AD405X Device SPI structure.
 */
struct ad405x_spi_extra {
	/* CNV GPIO descriptor */
	struct no_os_gpio_desc *gpio_cnv;
};

/** Variants extra structure */
union ad405x_extra {
	/** I3C Initialization structure. */
	struct ad405x_i3c_extra i3c_extra;
	/** SPI Initialization structure. */
	struct ad405x_spi_extra spi_extra;
};

/**
 * @struct ad405x_dev
 * @brief AD405X Device structure.
 */
struct ad405x_dev {
	/* Device type */
	enum ad405x_type dev_type;
	/* Device communication descriptor */
	union ad405x_comm_desc com_desc;
	/* Device communication type: AD405X_SPI_COMM, AD405X_I3C_COMM */
	enum ad405x_comm_type comm_type;
	/* Mode of operations */
	enum ad405x_operation_mode operation_mode;
	/* GPIO0 GPIO descriptor */
	struct no_os_gpio_desc *gpio_gpio0;
	/* GPIO1 GPIO descriptor */
	struct no_os_gpio_desc *gpio_gpio1;
	/* Sample rate for Burst and Autonomous Modes */
	enum ad405x_sample_rate rate;
	/* Averaging filter window length */
	enum ad405x_avg_filter_l filter_length;
	/* DEV_EN signal polarity */
	enum ad405x_dev_en_polarity polarity;
	/* DEV_EN power-on timer setting */
	enum ad405x_dev_en_timer_pwr_on timer_pwr_on;
	/* GP1 mode */
	enum ad405x_gp_mode gp1_mode;
	/* INVERT_ON_CHOP setting */
	enum ad405x_invert_on_chop invert_on_chop_status;
	/* GP0 mode */
	enum ad405x_gp_mode gp0_mode;
	/* Output data format */
	enum ad405x_out_data_format data_format;
	/* Variants extra structure */
	union ad405x_extra extra;
};

/** Initialize the device */
int ad405x_init(struct ad405x_dev **device,
		struct ad405x_init_param init_param);

/** Remove device and free resources allocated by ad405x_init() */
int ad405x_remove(struct ad405x_dev *dev);

/** Soft reset the device */
int ad405x_soft_reset(struct ad405x_dev *dev);

/** Clear reset flags of device */
int ad405x_clear_reset_flags(struct ad405x_dev *dev);

/** Read device registers */
int ad405x_read(struct ad405x_dev *dev, uint8_t addr,
		uint8_t *data, uint8_t size);

/** Write device registers */
int ad405x_write(struct ad405x_dev *dev, uint8_t addr,
		 uint8_t *data, uint8_t size);

/** Get formatted ADC reading */
int ad405x_get_adc(struct ad405x_dev *dev, int32_t *value);

/** Get raw ADC reading */
int ad405x_get_raw(struct ad405x_dev *dev, uint32_t *raw);

/** Reads the interface status and status registers */
int ad405x_get_sts_reg(struct ad405x_dev *dev,
		       uint8_t *status_flags,
		       uint8_t *intf_status_flags);

/** Write to clear the interface status and status registers */
int ad405x_clear_sts_reg(struct ad405x_dev *dev,
			 uint8_t *status_flags,
			 uint8_t *intf_status_flags);

/** Test scratch register pad register */
int ad405x_test_scratch_pad(struct ad405x_dev *dev);

/** Enter ADC Mode */
int ad405x_set_adc_mode(struct ad405x_dev *dev);

/** Enter Burst Averaging Mode */
int ad405x_set_burst_averaging_mode(struct ad405x_dev *dev);

/** Enter Persistent Auto Mode */
int ad405x_set_persistent_auto_mode(struct ad405x_dev *dev);

/** Enter Non Persistent Auto Mode */
int ad405x_set_nonpersistent_auto_mode(struct ad405x_dev *dev);

/** Set operation mode */
int ad405x_set_operation_mode(struct ad405x_dev *dev,
			      enum ad405x_operation_mode mode);

/** Select sample rate for Burst and Autonomous Modes */
int ad405x_set_sample_rate(struct ad405x_dev *dev,
			   enum ad405x_sample_rate rate);

/** Get sample rate for Burst and Autonomous Modes */
enum ad405x_sample_rate ad405x_get_sample_rate(struct ad405x_dev *dev);

/** Set averaging filter window length */
int ad405x_set_avg_filter_length(struct ad405x_dev *dev,
				 enum ad405x_avg_filter_l length);

/** Get averaging filter window length */
enum ad405x_avg_filter_l ad405x_get_avg_filter_length(struct ad405x_dev *dev);

/** Set DEV_EN signal polarity */
int ad405x_set_dev_en_polarity(struct ad405x_dev *dev,
			       enum ad405x_dev_en_polarity polarity);

/** Get DEV_EN signal polarity */
enum ad405x_dev_en_polarity ad405x_get_dev_en_polarity(struct ad405x_dev *dev);

/** Set DEV_EN TIMER_PWR_ON */
int ad405x_set_dev_en_timer_pwr_on(struct ad405x_dev *dev,
				   enum ad405x_dev_en_timer_pwr_on time);

/** Get DEV_EN TIMER_PWR_ON */
enum ad405x_dev_en_timer_pwr_on ad405x_get_dev_en_timer_pwr_on(
	struct ad405x_dev *dev);

/** Enable INVERT_ON_CHOP */
int ad405x_enable_invert_on_chop(struct ad405x_dev *dev);

/** Disable INVERT_ON_CHOP */
int ad405x_disable_invert_on_chop(struct ad405x_dev *dev);

/** Get INVERT_ON_CHOP setting */
enum ad405x_invert_on_chop ad405x_get_invert_on_chop_state(
	struct ad405x_dev *dev);

/** Set GP mode */
int ad405x_set_gp_mode(struct ad405x_dev *dev,
		       enum ad405x_gp_select gp, enum ad405x_gp_mode mode);

/** Get GP mode setting */
enum ad405x_gp_mode ad405x_get_gp_mode(struct ad405x_dev *dev,
				       enum ad405x_gp_select gp);

/** Set ADC output data format */
int ad405x_set_data_format(struct ad405x_dev *dev,
			   enum ad405x_out_data_format data_format);

/** Set ADC output data format as straight binary */
int ad405x_set_data_format_straight_binary(struct ad405x_dev *dev);

/** Set ADC output data format as two's complement */
int ad405x_set_data_format_twos_complement(struct ad405x_dev *dev);

/** Get ADC output data format */
enum ad405x_out_data_format ad405x_get_data_format(struct ad405x_dev *dev);

/** Set threshold configuration */
int ad405x_set_threshold(struct ad405x_dev *dev,
			 struct ad405x_thresh *thresh);

/** Get threshold configuration */
int ad405x_get_threshold(struct ad405x_dev *dev,
			 struct ad405x_thresh *thresh);

/** Send EXIT command (SPI only) */
int ad405x_exit_command(struct ad405x_dev *dev);

/** Trigger an ADC conversion (SPI only) */
int ad405x_trigger_adc_conv(struct ad405x_dev *dev);

/** Enter Averaging Mode (SPI only) */
int ad405x_set_averaging_mode(struct ad405x_dev *dev);

/** Enter Configuration Mode (SPI only) */
int ad405x_set_config_mode(struct ad405x_dev *dev,
			   enum ad405x_operation_mode *mode);

/** Test common CCCs (I3C only)*/
int ad405x_test_common_ccc(struct ad405x_dev *dev);

/** Pool IBI status and config registers (I3C only) */
int ad405x_pool_ibi_config(struct ad405x_dev *dev);

/** Enable/disable IBI (I3C only) */
int ad405x_set_ibi_enable(struct ad405x_dev *dev,
			  bool enable);

/** Set IBI mask (I3C only) */
int ad405x_set_ibi_mask(struct ad405x_dev *dev,
			uint16_t en_mask);

#endif /* __AD405X_H__ */
