/***************************************************************************//**
 *   @file   adxl372.h
 *   @brief  Header file for adxl372 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#ifndef ADXL372_H_
#define ADXL372_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * ADXL372 registers definition
 */
#define ADXL372_DEVID	0x00u   /* Analog Devices, Inc., accelerometer ID */
#define ADXL372_DEVID_MST	0x01u   /* Analog Devices MEMS device ID */
#define ADXL372_PARTID		0x02u   /* Device ID */
#define ADXL372_REVID		0x03u   /* product revision ID*/
#define ADXL372_STATUS_1	0x04u   /* Status register 1 */
#define ADXL372_STATUS_2	0x05u   /* Status register 2 */
#define ADXL372_FIFO_ENTRIES_2	0x06u   /* Valid data samples in the FIFO */
#define ADXL372_FIFO_ENTRIES_1	0x07u   /* Valid data samples in the FIFO */
#define ADXL372_X_DATA_H	0x08u   /* X-axis acceleration data [11:4] */
#define ADXL372_X_DATA_L	0x09u   /* X-axis acceleration data [3:0] */
#define ADXL372_Y_DATA_H	0x0Au   /* Y-axis acceleration data [11:4] */
#define ADXL372_Y_DATA_L	0x0Bu   /* Y-axis acceleration data [3:0] */
#define ADXL372_Z_DATA_H	0x0Cu   /* Z-axis acceleration data [11:4] */
#define ADXL372_Z_DATA_L	0x0Du   /* Z-axis acceleration data [3:0] */
#define ADXL372_X_MAXPEAK_H	0x15u   /* X-axis MaxPeak acceleration data [15:8] */
#define ADXL372_X_MAXPEAK_L	0x16u   /* X-axis MaxPeak acceleration data [7:0] */
#define ADXL372_Y_MAXPEAK_H	0x17u   /* Y-axis MaxPeak acceleration data [15:8] */
#define ADXL372_Y_MAXPEAK_L	0x18u   /* Y-axis MaxPeak acceleration data [7:0] */
#define ADXL372_Z_MAXPEAK_H	0x19u   /* Z-axis MaxPeak acceleration data [15:8] */
#define ADXL372_Z_MAXPEAK_L	0x1Au   /* Z-axis MaxPeak acceleration data [7:0] */
#define ADXL372_OFFSET_X	0x20u   /* X axis offset */
#define ADXL372_OFFSET_Y	0x21u   /* Y axis offset */
#define ADXL372_OFFSET_Z	0x22u   /* Z axis offset */
#define ADXL372_X_THRESH_ACT_H	0x23u   /* X axis Activity Threshold [15:8] */
#define ADXL372_X_THRESH_ACT_L	0x24u   /* X axis Activity Threshold [7:0] */
#define ADXL372_Y_THRESH_ACT_H	0x25u   /* Y axis Activity Threshold [15:8] */
#define ADXL372_Y_THRESH_ACT_L 	0x26u   /* Y axis Activity Threshold [7:0] */
#define ADXL372_Z_THRESH_ACT_H	0x27u   /* Z axis Activity Threshold [15:8] */
#define ADXL372_Z_THRESH_ACT_L	0x28u   /* Z axis Activity Threshold [7:0] */
#define ADXL372_TIME_ACT	0x29u   /* Activity Time */
#define ADXL372_X_THRESH_INACT_H	0x2Au   /* X axis Inactivity Threshold [15:8] */
#define ADXL372_X_THRESH_INACT_L	0x2Bu   /* X axis Inactivity Threshold [7:0] */
#define ADXL372_Y_THRESH_INACT_H    	0x2Cu   /* Y axis Inactivity Threshold [15:8] */
#define ADXL372_Y_THRESH_INACT_L    	0x2Du   /* Y axis Inactivity Threshold [7:0] */
#define ADXL372_Z_THRESH_INACT_H	0x2Eu   /* Z axis Inactivity Threshold [15:8] */
#define ADXL372_Z_THRESH_INACT_L	0x2Fu   /* Z axis Inactivity Threshold [7:0] */
#define ADXL372_TIME_INACT_H       	0x30u   /* Inactivity Time [15:8] */
#define ADXL372_TIME_INACT_L        	0x31u   /* Inactivity Time [7:0] */
#define ADXL372_X_THRESH_ACT2_H     	0x32u   /* X axis Activity2 Threshold [15:8] */
#define ADXL372_X_THRESH_ACT2_L		0x33u   /* X axis Activity2 Threshold [7:0] */
#define ADXL372_Y_THRESH_ACT2_H     	0x34u   /* Y axis Activity2 Threshold [15:8] */
#define ADXL372_Y_THRESH_ACT2_L     	0x35u   /* Y axis Activity2 Threshold [7:0] */
#define ADXL372_Z_THRESH_ACT2_H		0x36u   /* Z axis Activity2 Threshold [15:8] */
#define ADXL372_Z_THRESH_ACT2_L		0x37u   /* Z axis Activity2 Threshold [7:0] */
#define ADXL372_HPF			0x38u   /* High Pass Filter */
#define ADXL372_FIFO_SAMPLES        	0x39u   /* FIFO Samples */
#define ADXL372_FIFO_CTL	    	0x3Au   /* FIFO Control */
#define ADXL372_INT1_MAP	0x3Bu   /* Interrupt 1 mapping control */
#define ADXL372_INT2_MAP        0x3Cu   /* Interrupt 2 mapping control */
#define ADXL372_TIMING	       	0x3Du   /* Timing */
#define ADXL372_MEASURE		0x3Eu   /* Measure */
#define ADXL372_POWER_CTL       0x3Fu   /* Power control */
#define ADXL372_SELF_TEST       0x40u   /* Self Test */
#define ADXL372_RESET           0x41u   /* Reset */
#define ADXL372_FIFO_DATA	0x42u   /* FIFO Data */

#define ADXL372_DEVID_VAL       0xADu   /* Analog Devices, Inc., accelerometer ID */
#define ADXL372_MST_DEVID_VAL   0x1Du   /* Analog Devices MEMS device ID */
#define ADXL372_PARTID_VAL      0xFAu   /* Device ID */
#define ADXL372_REVID_VAL       0x02u   /* product revision ID*/
#define ADXL372_RESET_CODE	0x52u	/* Writing code 0x52 resets the device */

#define ADXL372_REG_READ(x)	(((x & 0xFF) << 1) | 0x01)
#define ADXL372_REG_WRITE(x)	((x & 0xFF) << 1)

/* ADXL372_POWER_CTL */
#define ADXL372_POWER_CTL_INSTANT_ON_TH_MSK	NO_OS_BIT(5)
#define ADXL372_POWER_CTL_INSTANT_ON_TH_MODE(x)	(((x) & 0x1) << 5)
#define ADXL372_POWER_CTL_FIL_SETTLE_MSK	NO_OS_BIT(4)
#define ADXL372_POWER_CTL_FIL_SETTLE_MODE(x)	(((x) & 0x1) << 4)
#define ADXL372_POWER_CTL_LPF_DIS_MSK		NO_OS_BIT(3)
#define ADXL372_POWER_CTL_LPF_DIS_MODE(x)	(((x) & 0x1) << 3)
#define ADXL372_POWER_CTL_HPF_DIS_MSK		NO_OS_BIT(2)
#define ADXL372_POWER_CTL_HPF_DIS_MODE(x)	(((x) & 0x1) << 2)
#define ADXL372_POWER_CTL_MODE_MSK		NO_OS_GENMASK(1, 0)
#define ADXL372_POWER_CTL_MODE(x)		(((x) & 0x3) << 0)

/* ADXL372_MEASURE */
#define ADXL372_MEASURE_AUTOSLEEP_MSK		NO_OS_BIT(6)
#define ADXL372_MEASURE_AUTOSLEEP_MODE(x)	(((x) & 0x1) << 6)
#define ADXL372_MEASURE_LINKLOOP_MSK		NO_OS_GENMASK(5, 4)
#define ADXL372_MEASURE_LINKLOOP_MODE(x)	(((x) & 0x3) << 4)
#define ADXL372_MEASURE_LOW_NOISE_MSK		NO_OS_BIT(3)
#define ADXL372_MEASURE_LOW_NOISE_MODE(x)	(((x) & 0x1) << 3)
#define ADXL372_MEASURE_BANDWIDTH_MSK		NO_OS_GENMASK(2, 0)
#define ADXL372_MEASURE_BANDWIDTH_MODE(x)	(((x) & 0x7) << 0)

/* ADXL372_TIMING */
#define ADXL372_TIMING_ODR_MSK			NO_OS_GENMASK(7, 5)
#define ADXL372_TIMING_ODR_MODE(x)		(((x) & 0x7) << 5)
#define ADXL372_TIMING_WAKE_UP_RATE_MSK		NO_OS_GENMASK(4, 2)
#define ADXL372_TIMING_WAKE_UP_RATE_MODE(x)	(((x) & 0x7) << 2)
#define ADXL372_TIMING_EXT_CLK_MSK		NO_OS_BIT(1)
#define ADXL372_TIMING_EXT_CLK_MODE(x)		(((x) & 0x1) << 1)
#define ADXL372_TIMING_EXT_SYNC_MSK		NO_OS_BIT(0)
#define ADXL372_TIMING_EXT_SYNC_MODE(x)		(((x) & 0x1) << 0)

/* ADXL372_FIFO_CTL */
#define ADXL372_FIFO_CTL_FORMAT_MSK		NO_OS_GENMASK(5, 3)
#define ADXL372_FIFO_CTL_FORMAT_MODE(x)		(((x) & 0x7) << 3)
#define ADXL372_FIFO_CTL_MODE_MSK		NO_OS_GENMASK(2, 1)
#define ADXL372_FIFO_CTL_MODE_MODE(x)		(((x) & 0x3) << 1)
#define ADXL372_FIFO_CTL_SAMPLES_MSK		NO_OS_BIT(0)
#define ADXL372_FIFO_CTL_SAMPLES_MODE(x)	(((x) > 0xFF) ? 1 : 0)

/* ADXL372_STATUS_1 */
#define ADXL372_STATUS_1_DATA_RDY(x)		(((x) >> 0) & 0x1)
#define ADXL372_STATUS_1_FIFO_RDY(x)		(((x) >> 1) & 0x1)
#define ADXL372_STATUS_1_FIFO_FULL(x)		(((x) >> 2) & 0x1)
#define ADXL372_STATUS_1_FIFO_OVR(x)		(((x) >> 3) & 0x1)
#define ADXL372_STATUS_1_USR_NVM_BUSY(x)	(((x) >> 5) & 0x1)
#define ADXL372_STATUS_1_AWAKE(x)		(((x) >> 6) & 0x1)
#define ADXL372_STATUS_1_ERR_USR_REGS(x)	(((x) >> 7) & 0x1)

/* ADXL372_INT1_MAP */
#define ADXL372_INT1_MAP_DATA_RDY_MSK		NO_OS_BIT(0)
#define ADXL372_INT1_MAP_DATA_RDY_MODE(x)	(((x) & 0x1) << 0)
#define ADXL372_INT1_MAP_FIFO_RDY_MSK		NO_OS_BIT(1)
#define ADXL372_INT1_MAP_FIFO_RDY_MODE(x)	(((x) & 0x1) << 1)
#define ADXL372_INT1_MAP_FIFO_FULL_MSK		NO_OS_BIT(2)
#define ADXL372_INT1_MAP_FIFO_FULL_MODE(x)	(((x) & 0x1) << 2)
#define ADXL372_INT1_MAP_FIFO_OVR_MSK		NO_OS_BIT(3)
#define ADXL372_INT1_MAP_FIFO_OVR_MODE(x)	(((x) & 0x1) << 3)
#define ADXL372_INT1_MAP_INACT_MSK		NO_OS_BIT(4)
#define ADXL372_INT1_MAP_INACT_MODE(x)		(((x) & 0x1) << 4)
#define ADXL372_INT1_MAP_ACT_MSK		NO_OS_BIT(5)
#define ADXL372_INT1_MAP_ACT_MODE(x)		(((x) & 0x1) << 5)
#define ADXL372_INT1_MAP_AWAKE_MSK		NO_OS_BIT(6)
#define ADXL372_INT1_MAP_AWAKE_MODE(x)		(((x) & 0x1) << 6)
#define ADXL372_INT1_MAP_LOW_MSK		NO_OS_BIT(7)
#define ADXL372_INT1_MAP_LOW_MODE(x)		(((x) & 0x1) << 7)

/* ADXL372_INT2_MAP */
#define ADXL372_INT2_MAP_DATA_RDY_MSK		NO_OS_BIT(0)
#define ADXL372_INT2_MAP_DATA_RDY_MODE(x)	(((x) & 0x1) << 0)
#define ADXL372_INT2_MAP_FIFO_RDY_MSK		NO_OS_BIT(1)
#define ADXL372_INT2_MAP_FIFO_RDY_MODE(x)	(((x) & 0x1) << 1)
#define ADXL372_INT2_MAP_FIFO_FULL_MSK		NO_OS_BIT(2)
#define ADXL372_INT2_MAP_FIFO_FULL_MODE(x)	(((x) & 0x1) << 2)
#define ADXL372_INT2_MAP_FIFO_OVR_MSK		NO_OS_BIT(3)
#define ADXL372_INT2_MAP_FIFO_OVR_MODE(x)	(((x) & 0x1) << 3)
#define ADXL372_INT2_MAP_INACT_MSK		NO_OS_BIT(4)
#define ADXL372_INT2_MAP_INACT_MODE(x)		(((x) & 0x1) << 4)
#define ADXL372_INT2_MAP_ACT_MSK		NO_OS_BIT(5)
#define ADXL372_INT2_MAP_ACT_MODE(x)		(((x) & 0x1) << 5)
#define ADXL372_INT2_MAP_AWAKE_MSK		NO_OS_BIT(6)
#define ADXL372_INT2_MAP_AWAKE_MODE(x)		(((x) & 0x1) << 6)
#define ADXL372_INT2_MAP_LOW_MSK		NO_OS_BIT(7)
#define ADXL372_INT2_MAP_LOW_MODE(x)		(((x) & 0x1) << 7)

static const int adxl372_th_reg_addr_h[3][3] = {
	{
		ADXL372_X_THRESH_ACT_H,
		ADXL372_Y_THRESH_ACT_H,
		ADXL372_Z_THRESH_ACT_H,
	}, {
		ADXL372_X_THRESH_ACT2_H,
		ADXL372_Y_THRESH_ACT2_H,
		ADXL372_Z_THRESH_ACT2_H,
	}, {
		ADXL372_X_THRESH_INACT_H,
		ADXL372_Y_THRESH_INACT_H,
		ADXL372_Z_THRESH_INACT_H,
	}
};

static const int adxl372_th_reg_addr_l[3][3] = {
	{
		ADXL372_X_THRESH_ACT_L,
		ADXL372_Y_THRESH_ACT_L,
		ADXL372_Z_THRESH_ACT_L,
	}, {
		ADXL372_X_THRESH_ACT2_L,
		ADXL372_Y_THRESH_ACT2_L,
		ADXL372_Z_THRESH_ACT2_L,
	}, {
		ADXL372_X_THRESH_INACT_L,
		ADXL372_Y_THRESH_INACT_L,
		ADXL372_Z_THRESH_INACT_L,
	}
};

enum adxl372_axis {
	ADXL372_X_AXIS,
	ADXL372_Y_AXIS,
	ADXL372_Z_AXIS
};

enum adxl372_op_mode {
	ADXL372_STANDBY,
	ADXL372_WAKE_UP,
	ADXL372_INSTANT_ON,
	ADXL372_FULL_BW_MEASUREMENT
};

enum adxl372_bandwidth {
	ADXL372_BW_200HZ,
	ADXL372_BW_400HZ,
	ADXL372_BW_800HZ,
	ADXL372_BW_1600HZ,
	ADXL372_BW_3200HZ
};

enum adxl372_act_proc_mode {
	ADXL372_DEFAULT,
	ADXL372_LINKED,
	ADXL372_LOOPED
};

enum adxl372_odr {
	ADXL372_ODR_400HZ,
	ADXL372_ODR_800HZ,
	ADXL372_ODR_1600HZ,
	ADXL372_ODR_3200HZ,
	ADXL372_ODR_6400HZ
};

enum adxl372_instant_on_th_mode {
	ADXL372_INSTANT_ON_LOW_TH,
	ADXL372_INSTANT_ON_HIGH_TH
};

enum adxl372_wakeup_rate {
	ADXL372_WUR_52ms,
	ADXL372_WUR_104ms,
	ADXL372_WUR_208ms,
	ADXL372_WUR_512ms,
	ADXL372_WUR_2048ms,
	ADXL372_WUR_4096ms,
	ADXL372_WUR_8192ms,
	ADXL372_WUR_24576ms
};

enum adxl372_th_activity {
	ADXL372_ACTIVITY,
	ADXL372_ACTIVITY2,
	ADXL372_INACTIVITY
};

enum adxl372_filter_settle {
	ADXL372_FILTER_SETTLE_370,
	ADXL372_FILTER_SETTLE_16
};

enum adxl372_fifo_format {
	ADXL372_XYZ_FIFO,
	ADXL372_X_FIFO,
	ADXL372_Y_FIFO,
	ADXL372_XY_FIFO,
	ADXL372_Z_FIFO,
	ADXL372_XZ_FIFO,
	ADXL372_YZ_FIFO,
	ADXL372_XYZ_PEAK_FIFO,
};

enum adxl372_fifo_mode {
	ADXL372_FIFO_BYPASSED,
	ADXL372_FIFO_STREAMED,
	ADXL372_FIFO_TRIGGERED,
	ADXL372_FIFO_OLD_SAVED
};

enum adxl372_comm_type {
	SPI,
	I2C,
};

struct adxl372_fifo_config {
	enum adxl372_fifo_mode fifo_mode;
	enum adxl372_fifo_format fifo_format;
	uint16_t fifo_samples;
};

struct adxl372_activity_threshold {
	uint16_t thresh;
	bool referenced;
	bool enable;
};

struct adxl372_xyz_accel_data {
	uint16_t x;
	uint16_t y;
	uint16_t z;
} ;

struct adxl372_irq_config {
	bool data_rdy;
	bool fifo_rdy;
	bool fifo_full;
	bool fifo_ovr;
	bool inactivity;
	bool activity;
	bool awake;
	bool low_operation;
};

struct adxl372_dev;

typedef int32_t (*adxl372_reg_read_func)(struct adxl372_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data);
typedef int32_t (*adxl372_reg_write_func)(struct adxl372_dev *dev,
		uint8_t reg_addr,
		uint8_t reg_data);
typedef int32_t (*adxl372_reg_read_multi_func)(struct adxl372_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data,
		uint16_t count);

struct adxl372_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* I2C */
	struct no_os_i2c_desc			*i2c_desc;
	/* GPIO */
	struct no_os_gpio_desc		*gpio_int1;
	struct no_os_gpio_desc		*gpio_int2;
	/* Device Settings */
	adxl372_reg_read_func		reg_read;
	adxl372_reg_write_func		reg_write;
	adxl372_reg_read_multi_func	reg_read_multiple;
	enum adxl372_bandwidth		bw;
	enum adxl372_odr		odr;
	enum adxl372_wakeup_rate	wur;
	enum adxl372_act_proc_mode	act_proc_mode;
	enum adxl372_instant_on_th_mode	th_mode;
	struct adxl372_fifo_config	fifo_config;
	enum adxl372_comm_type		comm_type;
};

struct adxl372_init_param {
	/* SPI */
	struct no_os_spi_init_param		spi_init;
	/* I2C */
	struct no_os_i2c_init_param			i2c_init;
	/* GPIO */
	struct no_os_gpio_init_param			gpio_int1;
	struct no_os_gpio_init_param			gpio_int2;
	/* Device Settings */
	enum adxl372_bandwidth			bw;
	enum adxl372_odr			odr;
	enum adxl372_wakeup_rate		wur;
	enum adxl372_act_proc_mode		act_proc_mode;
	enum adxl372_instant_on_th_mode		th_mode;
	struct adxl372_activity_threshold	activity_th;
	struct adxl372_activity_threshold	activity2_th;
	struct adxl372_activity_threshold	inactivity_th;
	uint8_t					activity_time;
	uint16_t				inactivity_time;
	enum adxl372_filter_settle		filter_settle;
	struct adxl372_fifo_config		fifo_config;
	struct adxl372_irq_config		int1_config;
	struct adxl372_irq_config		int2_config;
	enum adxl372_op_mode			op_mode;
	enum adxl372_comm_type			comm_type;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t adxl372_spi_reg_read(struct adxl372_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl372_spi_reg_read_multiple(struct adxl372_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
int32_t adxl372_spi_reg_write(struct adxl372_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl372_i2c_reg_read(struct adxl372_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl372_i2c_reg_write(struct adxl372_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl372_i2c_reg_read_multiple(struct adxl372_dev *dev,
				      uint8_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
int32_t adxl372_write_mask(struct adxl372_dev *dev,
			   uint8_t reg_addr,
			   uint32_t mask,
			   uint8_t data);
int32_t adxl372_set_activity_threshold(struct adxl372_dev *dev,
				       enum adxl372_th_activity act,
				       uint16_t thresh,
				       bool referenced,
				       bool enable);
int32_t adxl372_set_op_mode(struct adxl372_dev *dev,
			    enum adxl372_op_mode op_mode);
int32_t adxl372_set_autosleep(struct adxl372_dev *dev, bool enable);
int32_t adxl372_set_bandwidth(struct adxl372_dev *dev,
			      enum adxl372_bandwidth bw);
int32_t adxl372_set_act_proc_mode(struct adxl372_dev *dev,
				  enum adxl372_act_proc_mode mode);
int32_t adxl372_set_odr(struct adxl372_dev *dev,
			enum adxl372_odr odr);
int32_t adxl372_set_instant_on_th(struct adxl372_dev *dev,
				  enum adxl372_instant_on_th_mode mode);
int32_t adxl372_set_wakeup_rate(struct adxl372_dev *dev,
				enum adxl372_wakeup_rate wur);
int32_t adxl372_set_activity_time(struct adxl372_dev *dev, uint8_t time);
int32_t adxl372_set_inactivity_time(struct adxl372_dev *dev, uint16_t time);
int32_t adxl372_interrupt_config(struct adxl372_dev *dev,
				 struct adxl372_irq_config int1,
				 struct adxl372_irq_config int2);
int32_t adxl372_set_filter_settle(struct adxl372_dev *dev,
				  enum adxl372_filter_settle mode);
int32_t adxl372_get_status(struct adxl372_dev *dev,
			   uint8_t *status1,
			   uint8_t *status2,
			   uint16_t *fifo_entries);
int32_t adxl372_reset(struct adxl372_dev *dev);
int32_t adxl372_configure_fifo(struct adxl372_dev *dev,
			       enum adxl372_fifo_mode mode,
			       enum adxl372_fifo_format format,
			       uint16_t fifo_samples);
int32_t adxl372_get_fifo_xyz_data(struct adxl372_dev *dev,
				  struct adxl372_xyz_accel_data *fifo_data,
				  uint16_t cnt);
int32_t adxl372_service_fifo_ev(struct adxl372_dev *dev,
				struct adxl372_xyz_accel_data *fifo_data,
				uint16_t *fifo_entries);
int32_t adxl372_get_highest_peak_data(struct adxl372_dev *dev,
				      struct adxl372_xyz_accel_data *max_peak);
int32_t adxl372_get_accel_data(struct adxl372_dev *dev,
			       struct adxl372_xyz_accel_data *accel_data);
int32_t adxl372_init(struct adxl372_dev **device,
		     struct adxl372_init_param init_param);

#endif // ADXL372_H_
