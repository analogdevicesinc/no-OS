/***************************************************************************//**
 *   @file   iio_adis_internals.h
 *   @brief  Internal include file used for specific chip IIO driver definitions
 *           which are not useful for the user.
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#ifndef IIO_ADIS_INTERNALS_H
#define IIO_ADIS_INTERNALS_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"
#include <errno.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** @struct adis_iio_chan_type
 *  @brief ADIS IIO channels enumeration
 */
enum adis_iio_chan_type {
	ADIS_GYRO_X,
	ADIS_GYRO_Y,
	ADIS_GYRO_Z,
	ADIS_ACCEL_X,
	ADIS_ACCEL_Y,
	ADIS_ACCEL_Z,
	ADIS_TEMP,
	ADIS_DELTA_ANGL_X,
	ADIS_DELTA_ANGL_Y,
	ADIS_DELTA_ANGL_Z,
	ADIS_DELTA_VEL_X,
	ADIS_DELTA_VEL_Y,
	ADIS_DELTA_VEL_Z,
	ADIS_NUM_CHAN,
};

/** @struct adis_iio_debug_attrs
 *  @brief ADIS IIO debug attributes enumeration
 */
enum adis_iio_debug_attrs {
	ADIS_DIAG_SNSR_INIT_FAILURE,
	ADIS_DIAG_DATA_PATH_OVERRUN,
	ADIS_DIAG_FLS_MEM_UPDATE_FAILURE,
	ADIS_DIAG_SPI_COMM_ERR,
	ADIS_DIAG_STANDBY_MODE,
	ADIS_DIAG_SNSR_FAILURE,
	ADIS_DIAG_MEM_FAILURE,
	ADIS_DIAG_CLK_ERR,
	ADIS_DIAG_GYRO1_FAILURE,
	ADIS_DIAG_GYRO2_FAILURE,
	ADIS_DIAG_ACCL_FAILURE,
	ADIS_DIAG_X_AXIS_GYRO_FAILURE,
	ADIS_DIAG_Y_AXIS_GYRO_FAILURE,
	ADIS_DIAG_Z_AXIS_GYRO_FAILURE,
	ADIS_DIAG_X_AXIS_ACCL_FAILURE,
	ADIS_DIAG_Y_AXIS_ACCL_FAILURE,
	ADIS_DIAG_Z_AXIS_ACCL_FAILURE,
	ADIS_DIAG_ADUC_MCU_FAULT,
	ADIS_DIAG_CONFIG_CALIB_CRC_ERR,
	ADIS_DIAG_OVERRANGE,
	ADIS_DIAG_TEMP_ERR,
	ADIS_DIAG_POWER_SUPPLY_FAILURE,
	ADIS_DIAG_BOOT_MEMORY_FAILURE,
	ADIS_DIAG_REG_NVM_ERR,
	ADIS_DIAG_WDG_TIMER_FLAG,
	ADIS_DIAG_INT_PROC_SUPPLY_ERR,
	ADIS_DIAG_EXT_5V_SUPPLY_ERR,
	ADIS_DIAG_INT_SNSR_SUPPLY_ERR,
	ADIS_DIAG_INT_REG_ERR,
	ADIS_DIAG_CHECKSUM_ERR,
	ADIS_DIAG_FLS_MEM_WR_CNT_EXCEED,
	ADIS_DIAG_LOST_SAMPLES_COUNT,

	ADIS_TIME_STAMP,
	ADIS_DATA_CNTR,

	ADIS_FIFO_CNT,
	ADIS_SPI_CHKSUM,

	ADIS_FIFO_EN,
	ADIS_FIFO_OVERFLOW,
	ADIS_FIFO_WM_INT_EN,
	ADIS_FIFO_WM_INT_POL,
	ADIS_FIFO_WM_LVL,

	ADIS_FILT_SIZE_VAR_B,
	ADIS_GYRO_MEAS_RANGE,

	ADIS_DR_ENABLE,
	ADIS_DR_POLARITY,
	ADIS_DR_LINE_SEL,
	ADIS_SYNC_POLARITY,
	ADIS_SYNC_LINE_SEL,
	ADIS_SYNC_MODE,
	ADIS_ALARM_IND_ENABLE,
	ADIS_ALARM_IND_POLARITY,
	ADIS_ALARM_IND_LINE_SEL,
	ADIS_DIO_1_DIR,
	ADIS_DIO_2_DIR,
	ADIS_DIO_3_DIR,
	ADIS_DIO_4_DIR,
	ADIS_DIO_1_LVL,
	ADIS_DIO_2_LVL,
	ADIS_DIO_3_LVL,
	ADIS_DIO_4_LVL,
	ADIS_SENS_BW,
	ADIS_PT_OF_PERC_ALGNMT,
	ADIS_LINEAR_ACCL_COMP,
	ADIS_BURST_SEL,
	ADIS_BURST32,
	ADIS_TIMESTAMP32,
	ADIS_SYNC_4KHZ,
	ADIS_GYRO_FIR_ENABLE,
	ADIS_ACCL_FIR_ENABLE,
	ADIS_FIR_EN_XG,
	ADIS_FIR_EN_YG,
	ADIS_FIR_EN_ZG,
	ADIS_FIR_EN_XA,
	ADIS_FIR_EN_YA,
	ADIS_FIR_EN_ZA,
	ADIS_FIR_BANK_SEL_XG,
	ADIS_FIR_BANK_SEL_YG,
	ADIS_FIR_BANK_SEL_ZG,
	ADIS_FIR_BANK_SEL_XA,
	ADIS_FIR_BANK_SEL_YA,
	ADIS_FIR_BANK_SEL_ZA,

	ADIS_UP_SCALE,
	ADIS_DEC_RATE,

	ADIS_BIAS_CORR_TBC,
	ADIS_BIAS_CORR_EN_XG,
	ADIS_BIAS_CORR_EN_YG,
	ADIS_BIAS_CORR_EN_ZG,
	ADIS_BIAS_CORR_EN_XA,
	ADIS_BIAS_CORR_EN_YA,
	ADIS_BIAS_CORR_EN_ZA,

	ADIS_CMD_BIAS_CORR_UPDATE,
	ADIS_CMD_FACT_CALIB_RESTORE,
	ADIS_CMD_SNSR_SELF_TEST,
	ADIS_CMD_FLS_MEM_UPDATE,
	ADIS_CMD_FLS_MEM_TEST,
	ADIS_CMD_FIFO_FLUSH,
	ADIS_CMD_SW_RES,
	ADIS_CMD_WRITE_LOCK,

	ADIS_PROC_REV,
	ADIS_BOOT_REV,
	ADIS_FIRM_REV,
	ADIS_FIRM_DATE,
	ADIS_PROD_ID,
	ADIS_SERIAL_NUM,
	ADIS_LOT_NUM,
	ADIS_USR_SCR_1,
	ADIS_USR_SCR_2,
	ADIS_USR_SCR_3,
	ADIS_USR_SCR_4,
	ADIS_FLS_MEM_WR_CNTR,
	ADIS_EXT_CLK_FREQ,
};

/** @struct adis_iio_scale_fractional
 *  @brief ADIS IIO fractional scale format structure; scale = dividend/divisor
 */
struct adis_iio_scale_fractional {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale divisor. */
	uint32_t divisor;
};

/** @struct adis_iio_scale_fractional_log2
 *  @brief ADIS IIO fractional log2 scale format structure; scale = dividend/2^power
 */
struct adis_iio_scale_fractional_log2 {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale 2's power. */
	uint32_t power;
};

/** @struct adis_iio_dev
 *  @brief ADIS IIO device descriptor structure
 */
struct adis_iio_dev {
	/** ADIS device structure. */
	struct adis_dev *adis_dev;
	/** IIO device structure. */
	struct iio_device *iio_dev;
	/** Number of lost samples for the current buffer reading. */
	uint16_t samples_lost;
	/** Current data counter for the current buffer reading.*/
	uint32_t data_cntr;
	/** ADIS sampling frequency. */
	uint32_t sampling_frequency;
	/** Current setting for adis burst size. */
	uint32_t burst_size;
	/** Current setting for adis burst data selection. */
	uint32_t burst_sel;
	/** Current setting for adis sync mode. */
	uint32_t sync_mode;
	/** Data buffer to store one sample-set. */
	uint16_t data[26];
	/** True if iio device offers FIFO support for buffer reading. */
	bool has_fifo;
	/** Gyroscope measurement range value in text. */
	const char *rang_mdl_txt;
	struct iio_hw_trig *hw_trig_desc;
};

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_ACCEL_CHAN(mod, idx, adis_nb, attr) { \
	.ch_type = IIO_ACCEL, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_accel_scan_type,  \
        .attributes = attr,  \
}

#define ADIS_GYRO_CHAN(mod, idx, adis_nb, attr) { \
	.ch_type = IIO_ANGL_VEL, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_anglvel_scan_type,  \
        .attributes = attr,  \
}

#define ADIS_DELTA_ANGL_CHAN(mod, idx, adis_nb, attr) { \
	.ch_type = IIO_DELTA_ANGL, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_angl_scan_type,  \
        .attributes = attr,  \
}

#define ADIS_DELTA_ANGL_CHAN_NO_SCAN(mod, idx, attr) { \
	.ch_type = IIO_DELTA_ANGL, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
        .attributes = attr,  \
}

#define ADIS_DELTA_VEL_CHAN(mod, idx, adis_nb, attr) { \
	.ch_type = IIO_DELTA_VELOCITY, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_vel_scan_type,  \
        .attributes = attr,  \
}

#define ADIS_DELTA_VEL_CHAN_NO_SCAN(mod, idx, attr) { \
	.ch_type = IIO_DELTA_VELOCITY, \
	.modified = true, \
	.channel2 = IIO_MOD_##mod, \
	.address = idx, \
        .attributes = attr,  \
}

#define ADIS_TEMP_CHAN(idx, adis_nb, attr) { \
	.ch_type = IIO_TEMP, \
	.channel = 0, \
	.indexed = true, \
	.address = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_temp_scan_type,  \
        .attributes = attr,  \
}

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/

extern struct iio_attribute adis_dev_attrs[];
extern struct iio_attribute adis_iio_anglvel_attrs[];
extern struct iio_attribute adis_iio_delta_angl_attrs[];
extern struct iio_attribute adis_iio_delta_vel_attrs[];
extern struct iio_attribute adis_iio_accel_attrs[];
extern struct iio_attribute adis_iio_temp_attrs[];
extern struct iio_trigger adis_iio_trig_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! API to be called before trigger is enabled. */
int adis_iio_pre_enable(void* dev, uint32_t mask);
/*! API to be called before trigger is disabled. */
int adis_iio_post_disable(void* dev, uint32_t mask);
/*! Read adis iio samples for the active channels. */
int adis_iio_read_samples(void* dev, int* buff, uint32_t samples);
/*! Callback for adis iio data ready trigger. */
int adis_iio_trigger_handler(struct iio_device_data *dev_data);
int adis_iio_trigger_handler_with_fifo(struct iio_device_data *dev_data);

/*! Callback for adis iio debug attributes reading. */
int adis_iio_read_debug_attrs(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv);
/*! Callback for adis iio debug attributes writing. */
int adis_iio_write_debug_attrs(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);

/*! Wrapper for reading adis register. */
int adis_iio_read_reg(struct adis_iio_dev *device, uint32_t reg,
		      uint32_t *readval);
/*! Wrapper for writing adis register. */
int adis_iio_write_reg(struct adis_iio_dev *device, uint32_t reg,
		       uint32_t writeval);

/*! Handles the read request for raw attribute. */
int adis_iio_read_raw(void *dev, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for scale attribute. */
int adis_iio_read_scale(void *dev, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for offset attribute. */
int adis_iio_read_offset(void *dev, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for calibbias attribute. */
int adis_iio_read_calibbias(void *dev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for calibbias attribute. */
int adis_iio_write_calibbias(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for calibscale attribute. */
int adis_iio_read_calibscale(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for calibscale attribute. */
int adis_iio_write_calibscale(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for sampling frequency attribute. */
int adis_iio_write_sampling_freq(void *dev, char *buf,
				 uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for sampling frequency attribute. */
int adis_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the read request for lpf attribute. */
int adis_iio_read_lpf(void *dev, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv);
/*! Handles the write request for lpf attribute. */
int adis_iio_write_lpf(void *dev, char *buf, uint32_t len,
		       const struct iio_ch_info *channel, intptr_t priv);
#endif
