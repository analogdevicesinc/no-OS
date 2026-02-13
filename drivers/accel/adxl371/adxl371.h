/***************************************************************************//**
 *   @file   adxl371.h
 *   @brief  Header file for adxl371 Driver.
 *   @author Brandon Hurst (brandon.hurst@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef ADXL371_H_
#define ADXL371_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"

/*
 * ADXL371 registers definition
 */
#define ADXL371_DEVID	0x00u   /* Analog Devices, Inc., accelerometer ID */
#define ADXL371_DEVID_MST	0x01u   /* Analog Devices MEMS device ID */
#define ADXL371_PARTID		0x02u   /* Device ID */
#define ADXL371_REVID		0x03u   /* product revision ID*/
#define ADXL371_STATUS_1	0x04u   /* Status register 1 */
#define ADXL371_STATUS_2	0x05u   /* Status register 2 */
#define ADXL371_FIFO_ENTRIES_2	0x06u   /* Valid data samples in the FIFO */
#define ADXL371_FIFO_ENTRIES_1	0x07u   /* Valid data samples in the FIFO */
#define ADXL371_X_DATA_H	0x08u   /* X-axis acceleration data [11:4] */
#define ADXL371_X_DATA_L	0x09u   /* X-axis acceleration data [3:0] */
#define ADXL371_Y_DATA_H	0x0Au   /* Y-axis acceleration data [11:4] */
#define ADXL371_Y_DATA_L	0x0Bu   /* Y-axis acceleration data [3:0] */
#define ADXL371_Z_DATA_H	0x0Cu   /* Z-axis acceleration data [11:4] */
#define ADXL371_Z_DATA_L	0x0Du   /* Z-axis acceleration data [3:0] */
#define ADXL371_X_MAXPEAK_H	0x15u   /* X-axis MaxPeak acceleration data [15:8] */
#define ADXL371_X_MAXPEAK_L	0x16u   /* X-axis MaxPeak acceleration data [7:0] */
#define ADXL371_Y_MAXPEAK_H	0x17u   /* Y-axis MaxPeak acceleration data [15:8] */
#define ADXL371_Y_MAXPEAK_L	0x18u   /* Y-axis MaxPeak acceleration data [7:0] */
#define ADXL371_Z_MAXPEAK_H	0x19u   /* Z-axis MaxPeak acceleration data [15:8] */
#define ADXL371_Z_MAXPEAK_L	0x1Au   /* Z-axis MaxPeak acceleration data [7:0] */
#define ADXL371_OFFSET_X	0x20u   /* X axis offset */
#define ADXL371_OFFSET_Y	0x21u   /* Y axis offset */
#define ADXL371_OFFSET_Z	0x22u   /* Z axis offset */
#define ADXL371_X_THRESH_ACT_H	0x23u   /* X axis Activity Threshold [15:8] */
#define ADXL371_X_THRESH_ACT_L	0x24u   /* X axis Activity Threshold [7:0] */
#define ADXL371_Y_THRESH_ACT_H	0x25u   /* Y axis Activity Threshold [15:8] */
#define ADXL371_Y_THRESH_ACT_L 	0x26u   /* Y axis Activity Threshold [7:0] */
#define ADXL371_Z_THRESH_ACT_H	0x27u   /* Z axis Activity Threshold [15:8] */
#define ADXL371_Z_THRESH_ACT_L	0x28u   /* Z axis Activity Threshold [7:0] */
#define ADXL371_TIME_ACT	0x29u   /* Activity Time */
#define ADXL371_X_THRESH_INACT_H	0x2Au   /* X axis Inactivity Threshold [15:8] */
#define ADXL371_X_THRESH_INACT_L	0x2Bu   /* X axis Inactivity Threshold [7:0] */
#define ADXL371_Y_THRESH_INACT_H    	0x2Cu   /* Y axis Inactivity Threshold [15:8] */
#define ADXL371_Y_THRESH_INACT_L    	0x2Du   /* Y axis Inactivity Threshold [7:0] */
#define ADXL371_Z_THRESH_INACT_H	0x2Eu   /* Z axis Inactivity Threshold [15:8] */
#define ADXL371_Z_THRESH_INACT_L	0x2Fu   /* Z axis Inactivity Threshold [7:0] */
#define ADXL371_TIME_INACT_H       	0x30u   /* Inactivity Time [15:8] */
#define ADXL371_TIME_INACT_L        	0x31u   /* Inactivity Time [7:0] */
#define ADXL371_X_THRESH_ACT2_H     	0x32u   /* X axis Activity2 Threshold [15:8] */
#define ADXL371_X_THRESH_ACT2_L		0x33u   /* X axis Activity2 Threshold [7:0] */
#define ADXL371_Y_THRESH_ACT2_H     	0x34u   /* Y axis Activity2 Threshold [15:8] */
#define ADXL371_Y_THRESH_ACT2_L     	0x35u   /* Y axis Activity2 Threshold [7:0] */
#define ADXL371_Z_THRESH_ACT2_H		0x36u   /* Z axis Activity2 Threshold [15:8] */
#define ADXL371_Z_THRESH_ACT2_L		0x37u   /* Z axis Activity2 Threshold [7:0] */
#define ADXL371_HPF			0x38u   /* High Pass Filter */
#define ADXL371_FIFO_SAMPLES        	0x39u   /* FIFO Samples */
#define ADXL371_FIFO_CTL	    	0x3Au   /* FIFO Control */
#define ADXL371_INT1_MAP	0x3Bu   /* Interrupt 1 mapping control */
#define ADXL371_INT2_MAP        0x3Cu   /* Interrupt 2 mapping control */
#define ADXL371_TIMING	       	0x3Du   /* Timing */
#define ADXL371_MEASURE		0x3Eu   /* Measure */
#define ADXL371_POWER_CTL       0x3Fu   /* Power control */
#define ADXL371_SELF_TEST       0x40u   /* Self Test */
#define ADXL371_RESET           0x41u   /* Reset */
#define ADXL371_FIFO_DATA	0x42u   /* FIFO Data */

#define ADXL371_DEVID_VAL       0xADu   /* Analog Devices, Inc., accelerometer ID */
#define ADXL371_MST_DEVID_VAL   0x1Du   /* Analog Devices MEMS device ID */
#define ADXL371_PARTID_VAL      0xFAu   /* Device ID */
#define ADXL371_REVID_VAL       0x02u   /* product revision ID*/
#define ADXL371_RESET_CODE	0x52u	/* Writing code 0x52 resets the device */

#define ADXL371_REG_READ(x)	(((x & 0xFF) << 1) | 0x01)
#define ADXL371_REG_WRITE(x)	((x & 0xFF) << 1)

/* ADXL371_POWER_CTL */
#define ADXL371_POWER_CTL_INSTANT_ON_TH_MSK	NO_OS_BIT(5)
#define ADXL371_POWER_CTL_INSTANT_ON_TH_MODE(x)	(((x) & 0x1) << 5)
#define ADXL371_POWER_CTL_FIL_SETTLE_MSK	NO_OS_BIT(4)
#define ADXL371_POWER_CTL_FIL_SETTLE_MODE(x)	(((x) & 0x1) << 4)
#define ADXL371_POWER_CTL_LPF_DIS_MSK		NO_OS_BIT(3)
#define ADXL371_POWER_CTL_LPF_DIS_MODE(x)	(((x) & 0x1) << 3)
#define ADXL371_POWER_CTL_HPF_DIS_MSK		NO_OS_BIT(2)
#define ADXL371_POWER_CTL_HPF_DIS_MODE(x)	(((x) & 0x1) << 2)
#define ADXL371_POWER_CTL_MODE_MSK		NO_OS_GENMASK(1, 0)
#define ADXL371_POWER_CTL_MODE(x)		(((x) & 0x3) << 0)

/* ADXL371_MEASURE */
#define ADXL371_MEASURE_AUTOSLEEP_MSK		NO_OS_BIT(6)
#define ADXL371_MEASURE_AUTOSLEEP_MODE(x)	(((x) & 0x1) << 6)
#define ADXL371_MEASURE_LINKLOOP_MSK		NO_OS_GENMASK(5, 4)
#define ADXL371_MEASURE_LINKLOOP_MODE(x)	(((x) & 0x3) << 4)
#define ADXL371_MEASURE_LOW_NOISE_MSK		NO_OS_BIT(3)
#define ADXL371_MEASURE_LOW_NOISE_MODE(x)	(((x) & 0x1) << 3)
#define ADXL371_MEASURE_BANDWIDTH_MSK		NO_OS_GENMASK(2, 0)
#define ADXL371_MEASURE_BANDWIDTH_MODE(x)	(((x) & 0x7) << 0)

/* ADXL371_TIMING */
#define ADXL371_TIMING_ODR_MSK			NO_OS_GENMASK(7, 5)
#define ADXL371_TIMING_ODR_MODE(x)		(((x) & 0x7) << 5)
#define ADXL371_TIMING_WAKE_UP_RATE_MSK		NO_OS_GENMASK(4, 2)
#define ADXL371_TIMING_WAKE_UP_RATE_MODE(x)	(((x) & 0x7) << 2)
#define ADXL371_TIMING_EXT_CLK_MSK		NO_OS_BIT(1)
#define ADXL371_TIMING_EXT_CLK_MODE(x)		(((x) & 0x1) << 1)
#define ADXL371_TIMING_EXT_SYNC_MSK		NO_OS_BIT(0)
#define ADXL371_TIMING_EXT_SYNC_MODE(x)		(((x) & 0x1) << 0)

/* ADXL371_FIFO_CTL */
#define ADXL371_FIFO_CTL_FORMAT_MSK		NO_OS_GENMASK(5, 3)
#define ADXL371_FIFO_CTL_FORMAT_MODE(x)		(((x) & 0x7) << 3)
#define ADXL371_FIFO_CTL_MODE_MSK		NO_OS_GENMASK(2, 1)
#define ADXL371_FIFO_CTL_MODE_MODE(x)		(((x) & 0x3) << 1)
#define ADXL371_FIFO_CTL_SAMPLES_MSK		NO_OS_BIT(0)
#define ADXL371_FIFO_CTL_SAMPLES_MODE(x)	(((x) > 0xFF) ? 1 : 0)

/* ADXL371_STATUS_1 */
#define ADXL371_STATUS_1_DATA_RDY(x)		(((x) >> 0) & 0x1)
#define ADXL371_STATUS_1_FIFO_RDY(x)		(((x) >> 1) & 0x1)
#define ADXL371_STATUS_1_FIFO_FULL(x)		(((x) >> 2) & 0x1)
#define ADXL371_STATUS_1_FIFO_OVR(x)		(((x) >> 3) & 0x1)
#define ADXL371_STATUS_1_USR_NVM_BUSY(x)	(((x) >> 5) & 0x1)
#define ADXL371_STATUS_1_AWAKE(x)		(((x) >> 6) & 0x1)
#define ADXL371_STATUS_1_ERR_USR_REGS(x)	(((x) >> 7) & 0x1)

/* ADXL371_STATUS_2 */
#define ADXL371_STATUS_2_INACT(x)			(((x) >> 4) & 0x1)
#define ADXL371_STATUS_2_ACTIVITY(x)		(((x) >> 5) & 0x1)
#define ADXL371_STATUS_2_ACTIVITY2(x)		(((x) >> 6) & 0x1)

/* ADXL371_INT1_MAP */
#define ADXL371_INT1_MAP_DATA_RDY_MSK		NO_OS_BIT(0)
#define ADXL371_INT1_MAP_DATA_RDY_MODE(x)	(((x) & 0x1) << 0)
#define ADXL371_INT1_MAP_FIFO_RDY_MSK		NO_OS_BIT(1)
#define ADXL371_INT1_MAP_FIFO_RDY_MODE(x)	(((x) & 0x1) << 1)
#define ADXL371_INT1_MAP_FIFO_FULL_MSK		NO_OS_BIT(2)
#define ADXL371_INT1_MAP_FIFO_FULL_MODE(x)	(((x) & 0x1) << 2)
#define ADXL371_INT1_MAP_FIFO_OVR_MSK		NO_OS_BIT(3)
#define ADXL371_INT1_MAP_FIFO_OVR_MODE(x)	(((x) & 0x1) << 3)
#define ADXL371_INT1_MAP_INACT_MSK		NO_OS_BIT(4)
#define ADXL371_INT1_MAP_INACT_MODE(x)		(((x) & 0x1) << 4)
#define ADXL371_INT1_MAP_ACT_MSK		NO_OS_BIT(5)
#define ADXL371_INT1_MAP_ACT_MODE(x)		(((x) & 0x1) << 5)
#define ADXL371_INT1_MAP_AWAKE_MSK		NO_OS_BIT(6)
#define ADXL371_INT1_MAP_AWAKE_MODE(x)		(((x) & 0x1) << 6)
#define ADXL371_INT1_MAP_LOW_MSK		NO_OS_BIT(7)
#define ADXL371_INT1_MAP_LOW_MODE(x)		(((x) & 0x1) << 7)

/* ADXL371_INT2_MAP */
#define ADXL371_INT2_MAP_DATA_RDY_MSK		NO_OS_BIT(0)
#define ADXL371_INT2_MAP_DATA_RDY_MODE(x)	(((x) & 0x1) << 0)
#define ADXL371_INT2_MAP_FIFO_RDY_MSK		NO_OS_BIT(1)
#define ADXL371_INT2_MAP_FIFO_RDY_MODE(x)	(((x) & 0x1) << 1)
#define ADXL371_INT2_MAP_FIFO_FULL_MSK		NO_OS_BIT(2)
#define ADXL371_INT2_MAP_FIFO_FULL_MODE(x)	(((x) & 0x1) << 2)
#define ADXL371_INT2_MAP_FIFO_OVR_MSK		NO_OS_BIT(3)
#define ADXL371_INT2_MAP_FIFO_OVR_MODE(x)	(((x) & 0x1) << 3)
#define ADXL371_INT2_MAP_INACT_MSK		NO_OS_BIT(4)
#define ADXL371_INT2_MAP_INACT_MODE(x)		(((x) & 0x1) << 4)
#define ADXL371_INT2_MAP_ACT_MSK		NO_OS_BIT(5)
#define ADXL371_INT2_MAP_ACT_MODE(x)		(((x) & 0x1) << 5)
#define ADXL371_INT2_MAP_AWAKE_MSK		NO_OS_BIT(6)
#define ADXL371_INT2_MAP_AWAKE_MODE(x)		(((x) & 0x1) << 6)
#define ADXL371_INT2_MAP_LOW_MSK		NO_OS_BIT(7)
#define ADXL371_INT2_MAP_LOW_MODE(x)		(((x) & 0x1) << 7)

static const int32_t adxl371_th_reg_addr_h[3][3] = {
	{
		ADXL371_X_THRESH_ACT_H,
		ADXL371_Y_THRESH_ACT_H,
		ADXL371_Z_THRESH_ACT_H,
	}, {
		ADXL371_X_THRESH_ACT2_H,
		ADXL371_Y_THRESH_ACT2_H,
		ADXL371_Z_THRESH_ACT2_H,
	}, {
		ADXL371_X_THRESH_INACT_H,
		ADXL371_Y_THRESH_INACT_H,
		ADXL371_Z_THRESH_INACT_H,
	}
};

static const int32_t adxl371_th_reg_addr_l[3][3] = {
	{
		ADXL371_X_THRESH_ACT_L,
		ADXL371_Y_THRESH_ACT_L,
		ADXL371_Z_THRESH_ACT_L,
	}, {
		ADXL371_X_THRESH_ACT2_L,
		ADXL371_Y_THRESH_ACT2_L,
		ADXL371_Z_THRESH_ACT2_L,
	}, {
		ADXL371_X_THRESH_INACT_L,
		ADXL371_Y_THRESH_INACT_L,
		ADXL371_Z_THRESH_INACT_L,
	}
};

enum adxl371_axis {
	ADXL371_X_AXIS,
	ADXL371_Y_AXIS,
	ADXL371_Z_AXIS
};

enum adxl371_op_mode {
	ADXL371_STANDBY,
	ADXL371_WAKE_UP,
	ADXL371_INSTANT_ON,
	ADXL371_FULL_BW_MEASUREMENT
};

enum adxl371_bandwidth {
	ADXL371_BW_160HZ,
	ADXL371_BW_320HZ,
	ADXL371_BW_640HZ,
	ADXL371_BW_1280HZ,
	ADXL371_BW_2560HZ
};

enum adxl371_act_proc_mode {
	ADXL371_DEFAULT,
	ADXL371_LINKED,
	ADXL371_LOOPED
};

enum adxl371_odr {
	ADXL371_ODR_320HZ,
	ADXL371_ODR_640HZ,
	ADXL371_ODR_1280HZ,
	ADXL371_ODR_2560HZ,
	ADXL371_ODR_5120HZ
};

enum adxl371_instant_on_th_mode {
	ADXL371_INSTANT_ON_LOW_TH,
	ADXL371_INSTANT_ON_HIGH_TH
};

enum adxl371_wakeup_rate {
	ADXL371_WUR_65ms,
	ADXL371_WUR_130ms,
	ADXL371_WUR_260ms,
	ADXL371_WUR_640ms,
	ADXL371_WUR_2560ms,
	ADXL371_WUR_5120ms,
	ADXL371_WUR_10240ms,
	ADXL371_WUR_30720ms
};

enum adxl371_th_activity {
	ADXL371_ACTIVITY,
	ADXL371_ACTIVITY2,
	ADXL371_INACTIVITY
};

enum adxl371_filter_settle {
	ADXL371_FILTER_SETTLE_462p5,
	ADXL371_FILTER_SETTLE_4divODR
};

enum adxl371_fifo_format {
	ADXL371_XYZ_FIFO,
	ADXL371_X_FIFO,
	ADXL371_Y_FIFO,
	ADXL371_XY_FIFO,
	ADXL371_Z_FIFO,
	ADXL371_XZ_FIFO,
	ADXL371_YZ_FIFO,
	ADXL371_XYZ_PEAK_FIFO,
};

enum adxl371_fifo_mode {
	ADXL371_FIFO_BYPASSED,
	ADXL371_FIFO_STREAMED,
	ADXL371_FIFO_TRIGGERED,
	ADXL371_FIFO_OLD_SAVED
};

enum adxl371_comm_type {
	ADXL371_SPI,
	ADXL371_I2C,
};

struct adxl371_fifo_config {
	enum adxl371_fifo_mode fifo_mode;
	enum adxl371_fifo_format fifo_format;
	uint16_t fifo_samples;
};

struct adxl371_activity_threshold {
	uint16_t thresh;
	bool referenced;
	bool enable;
};

struct adxl371_xyz_accel_data {
	int16_t x;
	int16_t y;
	int16_t z;
} ;

struct adxl371_irq_config {
	bool data_rdy;
	bool fifo_rdy;
	bool fifo_full;
	bool fifo_ovr;
	bool inactivity;
	bool activity;
	bool awake;
	bool low_operation;
};

struct adxl371_dev;

typedef int32_t (*adxl371_reg_read_func)(struct adxl371_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data);
typedef int32_t (*adxl371_reg_write_func)(struct adxl371_dev *dev,
		uint8_t reg_addr,
		uint8_t reg_data);
typedef int32_t (*adxl371_reg_read_multi_func)(struct adxl371_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data,
		uint16_t count);

struct adxl371_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* I2C */
	struct no_os_i2c_desc		*i2c_desc;
	/* GPIO */
	struct no_os_gpio_desc		*gpio_int1;
	struct no_os_gpio_desc		*gpio_int2;
	/* Device Settings */
	adxl371_reg_read_func		reg_read;
	adxl371_reg_write_func		reg_write;
	adxl371_reg_read_multi_func	reg_read_multiple;
	enum adxl371_bandwidth		bw;
	enum adxl371_odr			odr;
	enum adxl371_wakeup_rate	wur;
	enum adxl371_act_proc_mode	act_proc_mode;
	enum adxl371_instant_on_th_mode	th_mode;
	struct adxl371_fifo_config	fifo_config;
	enum adxl371_comm_type		comm_type;
	/* FIFO Buffer */
	uint8_t fifo_raw[1024]; // 512 samples * 2 bytes/sample = 1024 bytes
};

struct adxl371_init_param {
	/* SPI */
	struct no_os_spi_init_param			spi_init;
	/* I2C */
	struct no_os_i2c_init_param			i2c_init;
	/* GPIO */
	struct no_os_gpio_init_param		gpio_int1;
	struct no_os_gpio_init_param		gpio_int2;
	/* Device Settings */
	enum adxl371_bandwidth				bw;
	enum adxl371_odr					odr;
	enum adxl371_wakeup_rate			wur;
	enum adxl371_act_proc_mode			act_proc_mode;
	enum adxl371_instant_on_th_mode		th_mode;
	struct adxl371_activity_threshold	activity_th;
	struct adxl371_activity_threshold	activity2_th;
	struct adxl371_activity_threshold	inactivity_th;
	uint8_t								activity_time;
	uint16_t							inactivity_time;
	enum adxl371_filter_settle			filter_settle;
	struct adxl371_fifo_config			fifo_config;
	struct adxl371_irq_config			int1_config;
	struct adxl371_irq_config			int2_config;
	enum adxl371_op_mode				op_mode;
	enum adxl371_comm_type				comm_type;
};

int adxl371_read_reg(struct adxl371_dev *dev, uint8_t reg_addr,
		     uint8_t *reg_data);
int adxl371_write_reg(struct adxl371_dev *dev, uint8_t reg_addr,
		      uint8_t reg_data);
int32_t adxl371_spi_reg_read(struct adxl371_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl371_spi_reg_read_multiple(struct adxl371_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
int32_t adxl371_spi_reg_write(struct adxl371_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl371_i2c_reg_read(struct adxl371_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl371_i2c_reg_write(struct adxl371_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl371_i2c_reg_read_multiple(struct adxl371_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
int adxl371_write_mask(struct adxl371_dev *dev,
		       uint8_t reg_addr,
		       uint32_t mask,
		       uint8_t data);
int adxl371_set_activity_threshold(struct adxl371_dev *dev,
				   enum adxl371_th_activity act,
				   uint16_t thresh,
				   bool referenced,
				   bool enable);
int adxl371_get_op_mode(struct adxl371_dev *dev,
			enum adxl371_op_mode *op_mode);
int adxl371_set_op_mode(struct adxl371_dev *dev,
			enum adxl371_op_mode op_mode);
int adxl371_set_autosleep(struct adxl371_dev *dev, bool enable);
int adxl371_set_bandwidth(struct adxl371_dev *dev,
			  enum adxl371_bandwidth bw);
int adxl371_set_act_proc_mode(struct adxl371_dev *dev,
			      enum adxl371_act_proc_mode mode);
int adxl371_set_odr(struct adxl371_dev *dev,
		    enum adxl371_odr odr);
int adxl371_set_instant_on_th(struct adxl371_dev *dev,
			      enum adxl371_instant_on_th_mode mode);
int adxl371_set_wakeup_rate(struct adxl371_dev *dev,
			    enum adxl371_wakeup_rate wur);
int adxl371_set_activity_time(struct adxl371_dev *dev, uint8_t time);
int adxl371_set_inactivity_time(struct adxl371_dev *dev, uint16_t time);
int adxl371_interrupt_config(struct adxl371_dev *dev,
			     struct adxl371_irq_config int1,
			     struct adxl371_irq_config int2);
int adxl371_set_filter_settle(struct adxl371_dev *dev,
			      enum adxl371_filter_settle mode);
int adxl371_get_status(struct adxl371_dev *dev,
		       uint8_t *status1,
		       uint8_t *status2,
		       uint16_t *fifo_entries);
int adxl371_reset(struct adxl371_dev *dev);
int adxl371_configure_fifo(struct adxl371_dev *dev,
			   enum adxl371_fifo_mode mode,
			   enum adxl371_fifo_format format,
			   uint16_t fifo_samples,
			   enum adxl371_op_mode op_mode);
int adxl371_init(struct adxl371_dev **device,
		 struct adxl371_init_param init_param);
int adxl371_remove(struct adxl371_dev *dev);
int adxl371_set_external_sync(struct adxl371_dev *dev);
int adxl371_set_internal_sync(struct adxl371_dev *dev);
int adxl371_get_internal_sync(struct adxl371_dev *dev, bool *internal_sync);
int adxl371_get_fifo_xyz_data_workaround(struct adxl371_dev *dev,
		struct adxl371_xyz_accel_data *fifo_data,
		uint16_t cnt);
int	adxl371_service_fifo_ev(struct adxl371_dev *dev,
				struct adxl371_xyz_accel_data *fifo_data,
				uint16_t *fifo_entries);
int adxl371_get_highest_peak_data(struct adxl371_dev *dev,
				  struct adxl371_xyz_accel_data *max_peak);

#endif // ADXL371_H_
