/***************************************************************************//**
 *   @file   iio_adis.h
 *   @brief  Implementation of iio_adis.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
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

#ifndef IIO_ADIS_H
#define IIO_ADIS_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio.h"
#include <errno.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** @struct adis_chan_type
 *  @brief ADIS channels enumeration
 */
enum adis_chan_type {
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
	ADIS_DATA_COUNTER,
	ADIS_NUM_CHAN,
};

/** @struct adis_debug_attrs
 *  @brief ADIS debug attributes enumeration
 */
enum adis_debug_attrs {
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

	ADIS_DR_POLARITY,
	ADIS_SYNC_POLARITY,
	ADIS_SYNC_MODE,
	ADIS_SENS_BW,
	ADIS_PT_OF_PERC_ALGNMT,
	ADIS_LINEAR_ACCL_COMP,
	ADIS_BURST_SEL,
	ADIS_BURST32,
	ADIS_TIMESTAMP32,
	ADIS_SYNC_4KHZ,

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

	ADIS_FIRM_REV,
	ADIS_FIRM_DATE,
	ADIS_PROD_ID,
	ADIS_SERIAL_NUM,
	ADIS_USR_SCR_1,
	ADIS_USR_SCR_2,
	ADIS_USR_SCR_3,
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
	uint16_t data_cntr;
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
	/** Gyroscope fractional scale. */
	struct adis_iio_scale_fractional gyro_scale;
	/** Accelerometer fractional scale. */
	struct adis_iio_scale_fractional accl_scale;
	/** Rotation angle fractional log2 scale. */
	struct adis_iio_scale_fractional_log2 rot_scale;
	/** Linear velocity fractional log2 scale. */
	struct adis_iio_scale_fractional_log2 vel_scale;
	/** Temperature fractional scale. */
	struct adis_iio_scale_fractional temp_scale;
	/** Gyroscope measurement range value in text. */
	const char *rang_mdl_txt;
};

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_ACCEL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ACCEL, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_accel_scan_type,  \
        .attributes = adis_iio_accel_attrs,  \
}

#define ADIS_GYRO_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ANGL_VEL, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_anglvel_scan_type,  \
        .attributes = adis_iio_anglvel_attrs,  \
}

#define ADIS_DELTA_ANGL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_ROT, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_angl_scan_type,  \
        .attributes = adis_iio_delta_angl_attrs,  \
}

#define ADIS_DELTA_VEL_CHAN(mod, idx, adis_nb) { \
	.ch_type = IIO_VELOCITY, \
	.modified = true, \
	.channel = idx, \
	.channel2 = IIO_MOD_##mod, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_delta_vel_scan_type,  \
        .attributes = adis_iio_delta_vel_attrs,  \
}

#define ADIS_TEMP_CHAN(idx, adis_nb) { \
	.ch_type = IIO_TEMP, \
	.channel = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_temp_scan_type,  \
        .attributes = adis_iio_temp_attrs,  \
}

#define ADIS_DATA_COUNTER_CHAN(idx, adis_nb) { \
	.ch_type = IIO_COUNT, \
	.channel = idx, \
	.scan_index = idx, \
	.scan_type = &adis##adis_nb##_iio_data_counter_scan_type,  \
        .attributes = NULL,  \
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
#endif
