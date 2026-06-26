/***************************************************************************/ /**
                                                                               *   @file   adxl37x.h
                                                                               *   @brief  Header file for adxl371 Driver.
                                                                               *   @author Brandon Hurst (brandon.hurst@analog.com)
                                                                               ********************************************************************************
                                                                               * Copyright 2026(c) Analog Devices, Inc.
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

#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include <stdint.h>

/*
 * ADXL371 registers definition
 */
#define ADXL371_DEVID 0x00u     /* Analog Devices, Inc., accelerometer ID */
#define ADXL371_DEVID_MST 0x01u /* Analog Devices MEMS device ID */
#define ADXL371_PARTID 0x02u    /* Device ID */
#define ADXL371_REVID 0x03u     /* product revision ID*/
#define ADXL371_STATUS_1 0x04u  /* Status register 1 */
#define ADXL371_STATUS_2 0x05u  /* Status register 2 */
#define ADXL371_FIFO_ENTRIES_2 0x06u /* Valid data samples in the FIFO */
#define ADXL371_FIFO_ENTRIES_1 0x07u /* Valid data samples in the FIFO */
#define ADXL371_X_DATA_H 0x08u       /* X-axis acceleration data [11:4] */
#define ADXL371_X_DATA_L 0x09u       /* X-axis acceleration data [3:0] */
#define ADXL371_Y_DATA_H 0x0Au       /* Y-axis acceleration data [11:4] */
#define ADXL371_Y_DATA_L 0x0Bu       /* Y-axis acceleration data [3:0] */
#define ADXL371_Z_DATA_H 0x0Cu       /* Z-axis acceleration data [11:4] */
#define ADXL371_Z_DATA_L 0x0Du       /* Z-axis acceleration data [3:0] */
#define ADXL371_X_MAXPEAK_H 0x15u /* X-axis MaxPeak acceleration data [15:8]   \
                                   */
#define ADXL371_X_MAXPEAK_L 0x16u /* X-axis MaxPeak acceleration data [7:0] */
#define ADXL371_Y_MAXPEAK_H 0x17u /* Y-axis MaxPeak acceleration data [15:8]   \
                                   */
#define ADXL371_Y_MAXPEAK_L 0x18u /* Y-axis MaxPeak acceleration data [7:0] */
#define ADXL371_Z_MAXPEAK_H 0x19u /* Z-axis MaxPeak acceleration data [15:8]   \
                                   */
#define ADXL371_Z_MAXPEAK_L 0x1Au /* Z-axis MaxPeak acceleration data [7:0] */
#define ADXL371_OFFSET_X 0x20u    /* X axis offset */
#define ADXL371_OFFSET_Y 0x21u    /* Y axis offset */
#define ADXL371_OFFSET_Z 0x22u    /* Z axis offset */
#define ADXL371_X_THRESH_ACT_H 0x23u   /* X axis Activity Threshold [15:8] */
#define ADXL371_X_THRESH_ACT_L 0x24u   /* X axis Activity Threshold [7:0] */
#define ADXL371_Y_THRESH_ACT_H 0x25u   /* Y axis Activity Threshold [15:8] */
#define ADXL371_Y_THRESH_ACT_L 0x26u   /* Y axis Activity Threshold [7:0] */
#define ADXL371_Z_THRESH_ACT_H 0x27u   /* Z axis Activity Threshold [15:8] */
#define ADXL371_Z_THRESH_ACT_L 0x28u   /* Z axis Activity Threshold [7:0] */
#define ADXL371_TIME_ACT 0x29u         /* Activity Time */
#define ADXL371_X_THRESH_INACT_H 0x2Au /* X axis Inactivity Threshold [15:8]   \
                                        */
#define ADXL371_X_THRESH_INACT_L 0x2Bu /* X axis Inactivity Threshold [7:0] */
#define ADXL371_Y_THRESH_INACT_H 0x2Cu /* Y axis Inactivity Threshold [15:8]   \
                                        */
#define ADXL371_Y_THRESH_INACT_L 0x2Du /* Y axis Inactivity Threshold [7:0] */
#define ADXL371_Z_THRESH_INACT_H 0x2Eu /* Z axis Inactivity Threshold [15:8]   \
                                        */
#define ADXL371_Z_THRESH_INACT_L 0x2Fu /* Z axis Inactivity Threshold [7:0] */
#define ADXL371_TIME_INACT_H 0x30u     /* Inactivity Time [15:8] */
#define ADXL371_TIME_INACT_L 0x31u     /* Inactivity Time [7:0] */
#define ADXL371_X_THRESH_ACT2_H 0x32u  /* X axis Activity2 Threshold [15:8] */
#define ADXL371_X_THRESH_ACT2_L 0x33u  /* X axis Activity2 Threshold [7:0] */
#define ADXL371_Y_THRESH_ACT2_H 0x34u  /* Y axis Activity2 Threshold [15:8] */
#define ADXL371_Y_THRESH_ACT2_L 0x35u  /* Y axis Activity2 Threshold [7:0] */
#define ADXL371_Z_THRESH_ACT2_H 0x36u  /* Z axis Activity2 Threshold [15:8] */
#define ADXL371_Z_THRESH_ACT2_L 0x37u  /* Z axis Activity2 Threshold [7:0] */
#define ADXL371_HPF 0x38u              /* High Pass Filter */
#define ADXL371_FIFO_SAMPLES 0x39u     /* FIFO Samples */
#define ADXL371_FIFO_CTL 0x3Au         /* FIFO Control */
#define ADXL371_INT1_MAP 0x3Bu         /* Interrupt 1 mapping control */
#define ADXL371_INT2_MAP 0x3Cu         /* Interrupt 2 mapping control */
#define ADXL371_TIMING 0x3Du           /* Timing */
#define ADXL371_MEASURE 0x3Eu          /* Measure */
#define ADXL371_POWER_CTL 0x3Fu        /* Power control */
#define ADXL371_SELF_TEST 0x40u        /* Self Test */
#define ADXL371_RESET 0x41u            /* Reset */
#define ADXL371_FIFO_DATA 0x42u        /* FIFO Data */

#define ADXL371_DEVID_VAL 0xADu     /* Analog Devices, Inc., accelerometer ID */
#define ADXL371_MST_DEVID_VAL 0x1Du /* Analog Devices MEMS device ID */
#define ADXL371_PARTID_VAL 0xFAu    /* Device ID */
#define ADXL371_REVID_VAL 0x02u     /* product revision ID*/
#define ADXL371_RESET_CODE 0x52u    /* Writing code 0x52 resets the device */

#define ADXL371_REG_READ(x) (((x & 0xFF) << 1) | 0x01)
#define ADXL371_REG_WRITE(x) ((x & 0xFF) << 1)

/* ADXL371_POWER_CTL */
#define ADXL371_POWER_CTL_INSTANT_ON_TH_MSK NO_OS_BIT(5)
#define ADXL371_POWER_CTL_INSTANT_ON_TH_MODE(x) (((x) & 0x1) << 5)
#define ADXL371_POWER_CTL_FIL_SETTLE_MSK NO_OS_BIT(4)
#define ADXL371_POWER_CTL_FIL_SETTLE_MODE(x) (((x) & 0x1) << 4)
#define ADXL371_POWER_CTL_LPF_DIS_MSK NO_OS_BIT(3)
#define ADXL371_POWER_CTL_LPF_DIS_MODE(x) (((x) & 0x1) << 3)
#define ADXL371_POWER_CTL_HPF_DIS_MSK NO_OS_BIT(2)
#define ADXL371_POWER_CTL_HPF_DIS_MODE(x) (((x) & 0x1) << 2)
#define ADXL371_POWER_CTL_MODE_MSK NO_OS_GENMASK(1, 0)
#define ADXL371_POWER_CTL_MODE(x) (((x) & 0x3) << 0)

/* ADXL371_MEASURE */
#define ADXL371_MEASURE_AUTOSLEEP_MSK NO_OS_BIT(6)
#define ADXL371_MEASURE_AUTOSLEEP_MODE(x) (((x) & 0x1) << 6)
#define ADXL371_MEASURE_LINKLOOP_MSK NO_OS_GENMASK(5, 4)
#define ADXL371_MEASURE_LINKLOOP_MODE(x) (((x) & 0x3) << 4)
#define ADXL371_MEASURE_LOW_NOISE_MSK NO_OS_BIT(3)
#define ADXL371_MEASURE_LOW_NOISE_MODE(x) (((x) & 0x1) << 3)
#define ADXL371_MEASURE_BANDWIDTH_MSK NO_OS_GENMASK(2, 0)
#define ADXL371_MEASURE_BANDWIDTH_MODE(x) (((x) & 0x7) << 0)

/* ADXL371_TIMING */
#define ADXL371_TIMING_ODR_MSK NO_OS_GENMASK(7, 5)
#define ADXL371_TIMING_ODR_MODE(x) (((x) & 0x7) << 5)
#define ADXL371_TIMING_WAKE_UP_RATE_MSK NO_OS_GENMASK(4, 2)
#define ADXL371_TIMING_WAKE_UP_RATE_MODE(x) (((x) & 0x7) << 2)
#define ADXL371_TIMING_EXT_CLK_MSK NO_OS_BIT(1)
#define ADXL371_TIMING_EXT_CLK_MODE(x) (((x) & 0x1) << 1)
#define ADXL371_TIMING_EXT_SYNC_MSK NO_OS_BIT(0)
#define ADXL371_TIMING_EXT_SYNC_MODE(x) (((x) & 0x1) << 0)

/* ADXL371_FIFO_CTL */
#define ADXL371_FIFO_CTL_FORMAT_MSK NO_OS_GENMASK(5, 3)
#define ADXL371_FIFO_CTL_FORMAT_MODE(x) (((x) & 0x7) << 3)
#define ADXL371_FIFO_CTL_MODE_MSK NO_OS_GENMASK(2, 1)
#define ADXL371_FIFO_CTL_MODE_MODE(x) (((x) & 0x3) << 1)
#define ADXL371_FIFO_CTL_SAMPLES_MSK NO_OS_BIT(0)
#define ADXL371_FIFO_CTL_SAMPLES_MODE(x) (((x) > 0xFF) ? 1 : 0)

/* ADXL371_STATUS_1 */
#define ADXL371_STATUS_1_DATA_RDY(x) (((x) >> 0) & 0x1)
#define ADXL371_STATUS_1_FIFO_RDY(x) (((x) >> 1) & 0x1)
#define ADXL371_STATUS_1_FIFO_FULL(x) (((x) >> 2) & 0x1)
#define ADXL371_STATUS_1_FIFO_OVR(x) (((x) >> 3) & 0x1)
#define ADXL371_STATUS_1_USR_NVM_BUSY(x) (((x) >> 5) & 0x1)
#define ADXL371_STATUS_1_AWAKE(x) (((x) >> 6) & 0x1)
#define ADXL371_STATUS_1_ERR_USR_REGS(x) (((x) >> 7) & 0x1)

/* ADXL371_STATUS_2 */
#define ADXL371_STATUS_2_INACT(x) (((x) >> 4) & 0x1)
#define ADXL371_STATUS_2_ACTIVITY(x) (((x) >> 5) & 0x1)
#define ADXL371_STATUS_2_ACTIVITY2(x) (((x) >> 6) & 0x1)

/* ADXL371_INT1_MAP */
#define ADXL371_INT1_MAP_DATA_RDY_MSK NO_OS_BIT(0)
#define ADXL371_INT1_MAP_DATA_RDY_MODE(x) (((x) & 0x1) << 0)
#define ADXL371_INT1_MAP_INACT_MSK NO_OS_BIT(4)
#define ADXL371_INT1_MAP_INACT_MODE(x) (((x) & 0x1) << 4)
#define ADXL371_INT1_MAP_ACT_MSK NO_OS_BIT(5)
#define ADXL371_INT1_MAP_ACT_MODE(x) (((x) & 0x1) << 5)
#define ADXL371_INT1_MAP_AWAKE_MSK NO_OS_BIT(6)
#define ADXL371_INT1_MAP_AWAKE_MODE(x) (((x) & 0x1) << 6)
#define ADXL371_INT1_MAP_LOW_MSK NO_OS_BIT(7)
#define ADXL371_INT1_MAP_LOW_MODE(x) (((x) & 0x1) << 7)

/* ADXL371_INT2_MAP */
#define ADXL371_INT2_MAP_DATA_RDY_MSK NO_OS_BIT(0)
#define ADXL371_INT2_MAP_DATA_RDY_MODE(x) (((x) & 0x1) << 0)
#define ADXL371_INT2_MAP_FIFO_RDY_MSK NO_OS_BIT(1)
#define ADXL371_INT2_MAP_FIFO_RDY_MODE(x) (((x) & 0x1) << 1)
#define ADXL371_INT2_MAP_FIFO_FULL_MSK NO_OS_BIT(2)
#define ADXL371_INT2_MAP_FIFO_FULL_MODE(x) (((x) & 0x1) << 2)
#define ADXL371_INT2_MAP_FIFO_OVR_MSK NO_OS_BIT(3)
#define ADXL371_INT2_MAP_FIFO_OVR_MODE(x) (((x) & 0x1) << 3)
#define ADXL371_INT2_MAP_INACT_MSK NO_OS_BIT(4)
#define ADXL371_INT2_MAP_INACT_MODE(x) (((x) & 0x1) << 4)
#define ADXL371_INT2_MAP_ACT_MSK NO_OS_BIT(5)
#define ADXL371_INT2_MAP_ACT_MODE(x) (((x) & 0x1) << 5)
#define ADXL371_INT2_MAP_AWAKE_MSK NO_OS_BIT(6)
#define ADXL371_INT2_MAP_AWAKE_MODE(x) (((x) & 0x1) << 6)
#define ADXL371_INT2_MAP_LOW_MSK NO_OS_BIT(7)
#define ADXL371_INT2_MAP_LOW_MODE(x) (((x) & 0x1) << 7)

//specific for ADXL372 INT1 and INT2 registers
#define ADXL371_INT1_MAP_FIFO_RDY_MSK NO_OS_BIT(1)
#define ADXL371_INT1_MAP_FIFO_RDY_MODE(x) (((x) & 0x1) << 1)
#define ADXL371_INT1_MAP_FIFO_FULL_MSK NO_OS_BIT(2)
#define ADXL371_INT1_MAP_FIFO_FULL_MODE(x) (((x) & 0x1) << 2)
#define ADXL371_INT1_MAP_FIFO_OVR_MSK NO_OS_BIT(3)
#define ADXL371_INT1_MAP_FIFO_OVR_MODE(x) (((x) & 0x1) << 3)

enum adxl37x_axis { ADXL37x_X_AXIS, ADXL37x_Y_AXIS, ADXL37x_Z_AXIS };

enum adxl37x_op_mode {
	ADXL37x_STANDBY,
	ADXL37x_WAKE_UP,
	ADXL37x_INSTANT_ON,
	ADXL37x_FULL_BW_MEASUREMENT
};


enum adxl37x_bandwidth {
	ADXL371_BW_160HZ  = 0, ADXL372_BW_200HZ  = 0,
	ADXL371_BW_320HZ  = 1, ADXL372_BW_400HZ  = 1,
	ADXL371_BW_640HZ  = 2, ADXL372_BW_800HZ  = 2,
	ADXL371_BW_1280HZ = 3, ADXL372_BW_1600HZ = 3,
	ADXL371_BW_2560HZ = 4, ADXL372_BW_3200HZ = 4,
};

enum adxl37x_act_proc_mode { ADXL37x_DEFAULT, ADXL37x_LINKED, ADXL37x_LOOPED };

enum adxl37x_odr {
	ADXL371_ODR_320HZ=0,
	ADXL372_ODR_400HZ=0,

	ADXL371_ODR_640HZ=1,
	ADXL372_ODR_800HZ=1,

	ADXL371_ODR_1280HZ=2,
	ADXL372_ODR_1600HZ=2,

	ADXL371_ODR_2560HZ=3,
	ADXL372_ODR_3200HZ=3,

	ADXL371_ODR_5120HZ=4,
	ADXL372_ODR_6400HZ=4

};

enum adxl37x_instant_on_th_mode {
	ADXL37x_INSTANT_ON_LOW_TH,
	ADXL37x_INSTANT_ON_HIGH_TH
};

enum adxl37x_wakeup_rate {
	ADXL371_WUR_65ms    = 0, ADXL372_WUR_52ms    = 0,
	ADXL371_WUR_130ms   = 1, ADXL372_WUR_104ms   = 1,
	ADXL371_WUR_260ms   = 2, ADXL372_WUR_208ms   = 2,
	ADXL371_WUR_640ms   = 3, ADXL372_WUR_512ms   = 3,
	ADXL371_WUR_2560ms  = 4, ADXL372_WUR_2048ms  = 4,
	ADXL371_WUR_5120ms  = 5, ADXL372_WUR_4096ms  = 5,
	ADXL371_WUR_10240ms = 6, ADXL372_WUR_8192ms  = 6,
	ADXL371_WUR_30720ms = 7, ADXL372_WUR_24576ms = 7,
};

enum adxl37x_th_activity {
	ADXL37x_ACTIVITY,
	ADXL37x_ACTIVITY2,
	ADXL37x_INACTIVITY
};

enum adxl37x_filter_settle {
	ADXL371_FILTER_SETTLE_462p5   = 0, ADXL372_FILTER_SETTLE_370 = 0,
	ADXL371_FILTER_SETTLE_4divODR = 1, ADXL372_FILTER_SETTLE_16  = 1,
};

enum adxl37x_fifo_format {
	ADXL37x_XYZ_FIFO,
	ADXL37x_X_FIFO,
	ADXL37x_Y_FIFO,
	ADXL37x_XY_FIFO,
	ADXL37x_Z_FIFO,
	ADXL37x_XZ_FIFO,
	ADXL37x_YZ_FIFO,
	ADXL37x_XYZ_PEAK_FIFO,
};

enum adxl37x_fifo_mode {
	ADXL37x_FIFO_BYPASSED,
	ADXL37x_FIFO_STREAMED,
	ADXL37x_FIFO_TRIGGERED,
	ADXL37x_FIFO_OLD_SAVED
};

enum adxl37x_type {
	ADXL371,
	ADXL372
};

struct adxl37x_chip_info {
	int8_t wur;
	int8_t odr;
	int8_t bw;
	int8_t filter_settle;
};

enum adxl37x_comm_type {
	ADXL37x_SPI,
	ADXL37x_I2C,
};

struct adxl37x_fifo_config {
	enum adxl37x_fifo_mode   fifo_mode;
	enum adxl37x_fifo_format fifo_format;
	uint16_t fifo_samples;
};

struct adxl37x_activity_threshold {
	uint16_t thresh;
	bool referenced;
	bool enable;
};

struct adxl37x_xyz_accel_data {
	int16_t x;
	int16_t y;
	int16_t z;
};

struct adxl37x_irq_config {
	/* fifo_rdy/full/ovr not used by adxl371 for INT1 and INT2 */
	bool data_rdy;
	bool fifo_rdy;
	bool fifo_full;
	bool fifo_ovr;
	bool inactivity;
	bool activity;
	bool awake;
	bool low_operation;
};

struct adxl37x_dev;

typedef int32_t (*adxl37x_reg_read_func)(struct adxl37x_dev *dev,
		uint8_t reg_addr, uint8_t *reg_data);
typedef int32_t (*adxl37x_reg_write_func)(struct adxl37x_dev *dev,
		uint8_t reg_addr, uint8_t reg_data);
typedef int32_t (*adxl37x_reg_read_multi_func)(struct adxl37x_dev *dev,
		uint8_t reg_addr,
		uint8_t *reg_data,
		uint16_t count);

//array of chip info structs

static const struct adxl37x_chip_info chip_info[] = {
	[ADXL371] = {
		.wur = ADXL371_WUR_65ms,
		.odr = ADXL371_ODR_320HZ,
		.bw = ADXL371_BW_320HZ,
		.filter_settle = ADXL371_FILTER_SETTLE_462p5
	},
	[ADXL372] = {
		.wur = ADXL372_WUR_104ms,
		.odr = ADXL372_ODR_400HZ,
		.bw = ADXL372_BW_200HZ,
		.filter_settle = ADXL372_FILTER_SETTLE_370
	}
};

struct adxl37x_dev {
	/*ADXL37x*/
	enum adxl37x_type adxl_type;
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* I2C */
	struct no_os_i2c_desc *i2c_desc;
	/* GPIO */
	struct no_os_gpio_desc *gpio_int1;
	struct no_os_gpio_desc *gpio_int2;
	/* Device Settings */
	adxl37x_reg_read_func reg_read;
	adxl37x_reg_write_func reg_write;
	adxl37x_reg_read_multi_func reg_read_multiple;
	enum adxl37x_bandwidth bw;
	enum adxl37x_odr odr;
	enum adxl37x_wakeup_rate wur;
	enum adxl37x_act_proc_mode act_proc_mode;
	enum adxl37x_instant_on_th_mode th_mode;
	struct adxl37x_fifo_config fifo_config;
	enum adxl37x_comm_type comm_type;
	/* FIFO Buffer */
	uint8_t fifo_raw[1024]; // 512 samples * 2 bytes/sample = 1024 bytes
};



struct adxl37x_init_param {
	/*ADXL37x*/
	enum adxl37x_type adxl_type;
	/* SPI */
	struct no_os_spi_init_param spi_init;
	/* I2C */
	struct no_os_i2c_init_param i2c_init;
	/* GPIO */
	struct no_os_gpio_init_param gpio_int1;
	struct no_os_gpio_init_param gpio_int2;
	/* Device Settings */
	enum adxl37x_act_proc_mode act_proc_mode;

	enum adxl37x_instant_on_th_mode th_mode;
	struct adxl37x_activity_threshold activity_th;
	struct adxl37x_activity_threshold activity2_th;
	struct adxl37x_activity_threshold inactivity_th;
	uint8_t activity_time;
	uint16_t inactivity_time;
	struct adxl37x_fifo_config fifo_config;
	struct adxl37x_irq_config int1_config;
	struct adxl37x_irq_config int2_config;
	enum adxl37x_op_mode op_mode;
	enum adxl37x_comm_type comm_type;
};

int adxl37x_read_reg(struct adxl37x_dev *dev, uint8_t reg_addr,
		     uint8_t *reg_data);
int adxl37x_write_reg(struct adxl37x_dev *dev, uint8_t reg_addr,
		      uint8_t reg_data);
int32_t adxl37x_spi_reg_read(struct adxl37x_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl37x_spi_reg_read_multiple(struct adxl37x_dev *dev, uint8_t reg_addr,
				      uint8_t *reg_data, uint16_t count);
int32_t adxl37x_spi_reg_write(struct adxl37x_dev *dev, uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl37x_i2c_reg_read(struct adxl37x_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t adxl37x_i2c_reg_write(struct adxl37x_dev *dev, uint8_t reg_addr,
			      uint8_t reg_data);
int32_t adxl37x_i2c_reg_read_multiple(struct adxl37x_dev *dev, uint8_t reg_addr,
				      uint8_t *reg_data, uint16_t count);
int adxl37x_write_mask(struct adxl37x_dev *dev, uint8_t reg_addr, uint32_t mask,
		       uint8_t data);
int adxl37x_set_activity_threshold(struct adxl37x_dev *dev,
				   enum adxl37x_th_activity act,
				   uint16_t thresh, bool referenced,
				   bool enable);
int adxl37x_get_op_mode(struct adxl37x_dev *dev, enum adxl37x_op_mode *op_mode);
int adxl37x_set_op_mode(struct adxl37x_dev *dev, enum adxl37x_op_mode op_mode);
int adxl37x_set_autosleep(struct adxl37x_dev *dev, bool enable);
int adxl37x_set_bandwidth(struct adxl37x_dev *dev, enum adxl37x_bandwidth bw);
int adxl37x_set_act_proc_mode(struct adxl37x_dev *dev,
			      enum adxl37x_act_proc_mode mode);
int adxl37x_set_odr(struct adxl37x_dev *dev, enum adxl37x_odr odr);
int adxl37x_set_instant_on_th(struct adxl37x_dev *dev,
			      enum adxl37x_instant_on_th_mode mode);
int adxl37x_set_wakeup_rate(struct adxl37x_dev *dev,
			    enum adxl37x_wakeup_rate wur);
int adxl37x_set_activity_time(struct adxl37x_dev *dev, uint8_t time);
int adxl37x_set_inactivity_time(struct adxl37x_dev *dev, uint16_t time);
int adxl37x_interrupt_config(struct adxl37x_dev *dev,
			     struct adxl37x_irq_config int1,
			     struct adxl37x_irq_config int2);
int adxl37x_set_filter_settle(struct adxl37x_dev *dev,
			      enum adxl37x_filter_settle mode);
int adxl37x_get_status(struct adxl37x_dev *dev, uint8_t *status1,
		       uint8_t *status2, uint16_t *fifo_entries);
int adxl37x_reset(struct adxl37x_dev *dev);
int adxl37x_configure_fifo(struct adxl37x_dev *dev,
			   enum adxl37x_fifo_mode mode,
			   enum adxl37x_fifo_format format,
			   uint16_t fifo_samples,
			   enum adxl37x_op_mode op_mode);
int adxl37x_init(struct adxl37x_dev **device,
		 struct adxl37x_init_param init_param);
int adxl37x_remove(struct adxl37x_dev *dev);
int adxl37x_set_external_sync(struct adxl37x_dev *dev);
int adxl37x_set_internal_sync(struct adxl37x_dev *dev);
int adxl37x_get_internal_sync(struct adxl37x_dev *dev, bool *internal_sync);
int adxl37x_get_fifo_xyz_data_workaround(
	struct adxl37x_dev *dev, struct adxl37x_xyz_accel_data *fifo_data,
	uint16_t cnt);
int adxl37x_service_fifo_ev(struct adxl37x_dev *dev,
			    struct adxl37x_xyz_accel_data *fifo_data,
			    uint16_t *fifo_entries);
int adxl37x_get_highest_peak_data(struct adxl37x_dev *dev,
				  struct adxl37x_xyz_accel_data *max_peak);

#endif // ADXL371_H_
