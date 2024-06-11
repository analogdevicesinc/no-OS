/***************************************************************************//**
 *   @file   adis.h
 *   @brief  Implementation of adis.h
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

#ifndef __ADIS_H__
#define __ADIS_H__

#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"
#include "no_os_util.h"
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_4_BYTES_SIZE	4
#define ADIS_2_BYTES_SIZE	2
#define ADIS_1_BYTE_SIZE	1

#define ADIS_SYNC_DEFAULT	0
#define ADIS_SYNC_DIRECT	1
#define ADIS_SYNC_SCALED	2
#define ADIS_SYNC_OUTPUT	3
#define ADIS_SYNC_PULSE		5

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Supported device ids
 */
enum adis_device_id {
	ADIS16465_1,
	ADIS16465_2,
	ADIS16465_3,
	ADIS16467_1,
	ADIS16467_2,
	ADIS16467_3,
	ADIS16470,
	ADIS16475_1,
	ADIS16475_2,
	ADIS16475_3,
	ADIS16477_1,
	ADIS16477_2,
	ADIS16477_3,
	ADIS16500,
	ADIS16501,
	ADIS16505_1,
	ADIS16505_2,
	ADIS16505_3,
	ADIS16507_1,
	ADIS16507_2,
	ADIS16507_3,
	ADIS16545_1,
	ADIS16545_2,
	ADIS16545_3,
	ADIS16547_1,
	ADIS16547_2,
	ADIS16547_3,
	ADIS16550,
	ADIS16575_2,
	ADIS16575_3,
	ADIS16576_2,
	ADIS16576_3,
	ADIS16577_2,
	ADIS16577_3,
};

/**
 * @brief Supported channels
 */
enum adis_chan_type {
	ADIS_ACCL_CHAN,
	ADIS_GYRO_CHAN,
	ADIS_TEMP_CHAN,
	ADIS_DELTAANGL_CHAN,
	ADIS_DELTAVEL_CHAN,
};

/**
 * @brief Supported axes
 */
enum adis_axis_type {
	ADIS_X_AXIS,
	ADIS_Y_AXIS,
	ADIS_Z_AXIS,
};


/** @struct adis_diag_flags
 *  @brief Bitfield struct which maps on the diagnosis register
 */
struct adis_diag_flags {
	/** Sensor initialization failure. */
	uint8_t snsr_init_failure	:1;
	/** Data path overrun bit. */
	uint8_t data_path_overrun	:1;
	/** Flash memory update failure. */
	uint8_t fls_mem_update_failure	:1;
	/** SPI communication error. */
	uint8_t spi_comm_err		:1;
	/** Standby mode. */
	uint8_t standby_mode		:1;
	/** Sensor failure. */
	uint8_t snsr_failure		:1;
	/** Memory failure. */
	uint8_t mem_failure		:1;
	/** Clock error. */
	uint8_t clk_err			:1;
	/** Gyroscope 1 failure. */
	uint8_t gyro1_failure		:1;
	/** Gyroscope 2 failure. */
	uint8_t gyro2_failure		:1;
	/** Accelerometer failure. */
	uint8_t accl_failure		:1;
	/** X-Axis gyroscope failure. */
	uint8_t x_axis_gyro_failure	:2;
	/** Y-Axis gyroscope failure. */
	uint8_t y_axis_gyro_failure	:2;
	/** Z-Axis gyroscope failure. */
	uint8_t z_axis_gyro_failure	:2;
	/** X-Axis accelerometer failure. */
	uint8_t x_axis_accl_failure	:2;
	/** Y-Axis accelerometer failure. */
	uint8_t y_axis_accl_failure	:2;
	/** Z-Axis accelerometer failure. */
	uint8_t z_axis_accl_failure	:2;
	/** ADuC microcontroller fault. */
	uint8_t aduc_mcu_fault		:1;
	/** Configuration and/or calibration CRC error. */
	uint8_t config_calib_crc_error	:1;
	/** Overrange event occurred. */
	uint8_t overrange		:1;
	/** Temperature error. */
	uint8_t temp_err		:1;
	/** Power supply failure. */
	uint8_t power_supply_failure	:1;
	/** Power supply failure. */
	uint8_t boot_memory_failure	:1;
	/** Register NVM error. */
	uint8_t reg_nvm_err		:1;
	/** Watchdog timer flag. */
	uint8_t wdg_timer_flag		:1;
	/** Internal processor supply error. */
	uint8_t int_proc_supply_err	:1;
	/** External 5V supply error. */
	uint8_t ext_5v_supply_err	:1;
	/** Internal sensor supply error. */
	uint8_t int_snsr_supply_err	:1;
	/** Internal regulator error. */
	uint8_t int_reg_err		:1;
	/** Checksum error.  */
	uint8_t checksum_err		:1;
	/** Flash memory write count exceeded. */
	uint8_t fls_mem_wr_cnt_exceed	:1;
};

/** @struct adis_temp_flags
 *  @brief Bitfield struct which maps on the temperature fags from the temperature register
 */
struct adis_temp_flags {
	/** Accelerometer temperature flag for z-axis and x-axis. */
	uint8_t accl_temp_z_x	:1;
	/** Accelerometer temperature flag for y-axis and z-axis. */
	uint8_t accl_temp_y_z	:1;
	/** Accelerometer temperature flag for x-axis and y-axis. */
	uint8_t accl_temp_x_y	:1;
	/** Gyroscope2 temperature flag for z-axis. */
	uint8_t	gyro2_temp_z	:1;
	/** Gyroscope1 temperature flag for z-axis. */
	uint8_t gyro1_temp_z	:1;
	/** Gyroscope2 temperature flag for y-axis. */
	uint8_t	gyro2_temp_y	:1;
	/** Gyroscope1 temperature flag for y-axis. */
	uint8_t gyro1_temp_y	:1;
	/** Gyroscope2 temperature flag for x-axis. */
	uint8_t	gyro2_temp_x	:1;
	/** Gyroscope1 temperature flag for x-axis. */
	uint8_t gyro1_temp_x	:1;
};

/** @struct adis_scale_fractional
 *  @brief ADIS fractional scale format structure; scale = dividend/divisor
 */
struct adis_scale_fractional {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale divisor. */
	uint32_t divisor;
};

/** @struct adis_scale_fractional_log2
 *  @brief ADIS fractional log2 scale format structure; scale = dividend/2^power
 */
struct adis_scale_fractional_log2 {
	/** Scale dividend. */
	uint32_t dividend;
	/** Scale 2's power. */
	uint32_t power;
};


/** @struct adis_burst_data
 *  @brief ADIS burst data structure
 */
struct adis_burst_data {
	uint16_t temp_lsb;
	uint16_t temp_msb;
	uint16_t data_cntr_lsb;
	uint16_t data_cntr_msb;
	uint16_t x_gyro_lsb;
	uint16_t x_gyro_msb;
	uint16_t y_gyro_lsb;
	uint16_t y_gyro_msb;
	uint16_t z_gyro_lsb;
	uint16_t z_gyro_msb;
	uint16_t x_accel_lsb;
	uint16_t x_accel_msb;
	uint16_t y_accel_lsb;
	uint16_t y_accel_msb;
	uint16_t z_accel_lsb;
	uint16_t z_accel_msb;
};

/** @struct adis_dev
 *  @brief ADIS device descriptor structure
 */
struct adis_dev {
	/** SPI descriptor used for SPI communication. */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO descriptor used to handle the reset pin. */
	struct no_os_gpio_desc		*gpio_reset;
	/** Specific chip information. */
	const struct adis_chip_info  	*info;
	/** Current diagnosis flags values. */
	struct adis_diag_flags 		diag_flags;
	/** Current temperature flags values. */
	struct adis_temp_flags 		temp_flags;
	/** Current device id, specified by the user */
	enum adis_device_id		dev_id;
	/** Current page to be accessed in register map. */
	uint32_t			current_page;
	/** Transmit buffer used in SPI transactions. */
	uint8_t				tx[12];
	/** Receive buffer used in SPI transactions. */
	uint8_t				rx[8];
	/** Internal clock frequency in Hertz. */
	uint32_t 			int_clk;
	/** External clock frequency in Hertz. */
	uint32_t 			ext_clk;
	/** Set to true if device fifo is enabled. */
	bool				fifo_enabled;
	/** Set to true if device burst32 is enabled. */
	bool				burst32;
	/** Burst data selection: 0 for accel/gyro data; 1 for delta angle/ delta velocity data. */
	uint8_t				burst_sel;
	/** Device is locked, only data readings are allowed, no configuration allowed. */
	bool				is_locked;
};

/** @struct adis_init_param
 *  @brief ADIS device initialization parameters
 */
struct adis_init_param {
	/* Chip specific information. */
	const struct adis_chip_info *info;
	/** SPI initialization parameters. */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO initialization parameter for reset pin. */
	struct no_os_gpio_init_param	*gpio_reset;
	/** External clock frequency in Hertz to be configured at initialization
	 *  phase.
	 */
	uint32_t 			ext_clk;
	/** Desired synchronization mode to be configured at initialization
	 *  phase.
	 */
	uint32_t			sync_mode;
	/** Device id, specified by the user  */
	enum adis_device_id		dev_id;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis device. */
int adis_init(struct adis_dev **adis, const struct adis_init_param *ip);
/*! Remove adis device. */
void adis_remove(struct adis_dev *adis);
/*! Device initial setup. */
int adis_initial_startup(struct adis_dev *adis);

/*! Read N bytes from register. */
int adis_read_reg(struct adis_dev *adis,  uint32_t reg, uint32_t *val,
		  uint32_t size);
/*! Write N bytes to register. */
int adis_write_reg(struct adis_dev *adis, uint32_t reg, uint32_t value,
		   uint32_t size);
/*! Update the desired bits of reg in accordance with mask and val. */
int adis_update_bits_base(struct adis_dev *adis, uint32_t reg,
			  const uint32_t mask, const uint32_t val, uint8_t size);

/*! Read diag status register and update device diag flags. */
int adis_read_diag_stat(struct adis_dev *adis,
			struct adis_diag_flags *diag_flags);

/*! Read temperature register and update temperature flags. */
int adis_read_temp_flags(struct adis_dev *adis,
			 struct adis_temp_flags *temp_flags);

/*! Diagnosis: read sensor initialization failure flag value. */
int adis_read_diag_snsr_init_failure(struct adis_dev *adis,
				     uint32_t *snsr_init_failure);
/*! Diagnosis: read data path overrun flag value. */
int adis_read_diag_data_path_overrun(struct adis_dev *adis,
				     uint32_t *data_path_overrun_err);
/*! Diagnosis: read flash memory update error flag value. */
int adis_read_diag_fls_mem_update_failure(struct adis_dev *adis,
		uint32_t *fls_mem_update_failure);
/*! Diagnosis: read spi communication error flag value. */
int adis_read_diag_spi_comm_err(struct adis_dev *adis, uint32_t *spi_comm_err);
/*! Diagnosis: read standby mode flag value. */
int adis_read_diag_standby_mode(struct adis_dev *adis,
				uint32_t *standby_mode);
/*! Diagnosis: read sensor self test error flag value. */
int adis_read_diag_snsr_failure(struct adis_dev *adis, uint32_t *snsr_failure);
/*! Diagnosis: read flash memory test error flag value. */
int adis_read_diag_mem_failure(struct adis_dev *adis, uint32_t *mem_failure);
/*! Diagnosis: read clock error flag value. */
int adis_read_diag_clk_err(struct adis_dev *adis, uint32_t *clk_err);
/*! Diagnosis: read gyroscope1 self test error flag value. */
int adis_read_diag_gyro1_failure(struct adis_dev *adis,
				 uint32_t *gyro1_failure);
/*! Diagnosis: read gyroscope2 self test error flag value. */
int adis_read_diag_gyro2_failure(struct adis_dev *adis,
				 uint32_t *gyro2_failure);
/*! Diagnosis: read accelerometer self test error flag value. */
int adis_read_diag_accl_failure(struct adis_dev *adis, uint32_t *accl_failure);
/*! Diagnosis: read X-Axis Gyroscope failure flag value. */
int adis_read_diag_x_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *x_axis_gyro_failure);
/*! Diagnosis: read Y-Axis Gyroscope failure flag value. */
int adis_read_diag_y_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *y_axis_gyro_failure);
/*! Diagnosis: read Z-Axis Gyroscope failure flag value. */
int adis_read_diag_z_axis_gyro_failure(struct adis_dev *adis,
				       uint32_t *z_axis_gyro_failure);
/*! Diagnosis: read X-Axis Accelerometer failure flag value. */
int adis_read_diag_x_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *x_axis_accl_failure);
/*! Diagnosis: read Y-Axis Accelerometer failure flag value. */
int adis_read_diag_y_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *y_axis_accl_failure);
/*! Diagnosis: read Z-Axis Accelerometer failure flag value. */
int adis_read_diag_z_axis_accl_failure(struct adis_dev *adis,
				       uint32_t *z_axis_accl_failure);
/*! Diagnosis: read ADuC microcontroller fault flag value. */
int adis_read_diag_aduc_mcu_fault(struct adis_dev *adis,
				  uint32_t *aduc_mcu_fault);
/*! Diagnosis: read configuration and/or calibration CRC error flag value. */
int adis_read_diag_config_calib_crc_error(struct adis_dev *adis,
		uint32_t *config_calib_crc_error);
/*! Diagnosis: read overrange for inertial sensors flag value. */
int adis_read_diag_overrange(struct adis_dev *adis,
			     uint32_t *overrange);
/*! Diagnosis: read temperature error flag value. */
int adis_read_diag_temp_err(struct adis_dev *adis,
			    uint32_t *temp_err);
/*! Diagnosis: read power supply error flag value. */
int adis_read_diag_power_supply_failure(struct adis_dev *adis,
					uint32_t *power_supply_failure);
/*! Diagnosis: read boot memory failure error flag value. */
int adis_read_diag_boot_memory_failure(struct adis_dev *adis,
				       uint32_t *boot_memory_failure);
/*! Diagnosis: read register NVM error flag value. */
int adis_read_diag_reg_nvm_err(struct adis_dev *adis,
			       uint32_t *reg_nvm_err);
/*! Diagnosis: read watchdog timer flag value. */
int adis_read_diag_wdg_timer_flag(struct adis_dev *adis,
				  uint32_t *wdg_timer_flag);
/*! Diagnosis: read internal processor supply error flag value. */
int adis_read_diag_int_proc_supply_err(struct adis_dev *adis,
				       uint32_t *int_proc_supply_err);
/*! Diagnosis: read external 5V supply error flag value. */
int adis_read_diag_ext_5v_supply_err(struct adis_dev *adis,
				     uint32_t *ext_5v_supply_err);
/*!  Diagnosis: read internal sensor supply error flag value. */
int adis_read_diag_int_snsr_supply_err(struct adis_dev *adis,
				       uint32_t *int_snsr_supply_err);
/*! Diagnosis: read internal regulator error flag value. */
int adis_read_diag_int_reg_err(struct adis_dev *adis,
			       uint32_t *int_reg_err);
/*! Diagnosis: read checksum error flag value. */
void adis_read_diag_checksum_err(struct adis_dev *adis, uint32_t *checksum_err);
/*! Diagnosis: read flash memory write counts exceeded flag value. */
void adis_read_diag_fls_mem_wr_cnt_exceed(struct adis_dev *adis,
		uint32_t *fls_mem_wr_cnt_exceed);

/*! Gyroscope data: read raw gyroscope data on x axis. */
int adis_read_x_gyro(struct adis_dev *adis, int32_t *x_gyro);
/*! Gyroscope data: read raw gyroscope data on y axis. */
int adis_read_y_gyro(struct adis_dev *adis, int32_t *y_gyro);
/*! Gyroscope data: read raw gyroscope data on z axis. */
int adis_read_z_gyro(struct adis_dev *adis, int32_t *z_gyro);

/*! Acceleration data: read raw acceleration data on x axis. */
int adis_read_x_accl(struct adis_dev *adis, int32_t *x_accl);
/*! Acceleration data: read raw acceleration data on y axis. */
int adis_read_y_accl(struct adis_dev *adis, int32_t *y_accl);
/*! Acceleration data: read raw acceleration data on z axis. */
int adis_read_z_accl(struct adis_dev *adis, int32_t *z_accl);

/*! Temperature data: read raw temperature data. */
int adis_read_temp_out(struct adis_dev *adis, int32_t *temp_out);

/*! Synchronization data: read raw time stamp data. */
int adis_read_time_stamp(struct adis_dev *adis, uint32_t *time_stamp);
/*! Synchronization data: read raw data counter data. */
int adis_read_data_cntr(struct adis_dev *adis, uint32_t *data_cntr);

/*! Delta angle data: read raw delta angle data on x axis. */
int adis_read_x_deltang(struct adis_dev *adis, int32_t *x_deltang);
/*! Delta angle data: read raw delta angle data on y axis. */
int adis_read_y_deltang(struct adis_dev *adis, int32_t *y_deltang);
/*! Delta angle data: read raw delta angle data on z axis. */
int adis_read_z_deltang(struct adis_dev *adis, int32_t *z_deltang);

/*! Delta velocity data: read raw delta velocity data on x axis. */
int adis_read_x_deltvel(struct adis_dev *adis, int32_t *x_deltvel);
/*! Delta velocity data: read raw delta velocity data on y axis. */
int adis_read_y_deltvel(struct adis_dev *adis, int32_t *y_deltvel);
/*! Delta velocity data: read raw delta velocity data on z axis. */
int adis_read_z_deltvel(struct adis_dev *adis, int32_t *z_deltvel);

/*! Read output FIFO sample count. */
int adis_read_fifo_cnt(struct adis_dev *adis, uint32_t *fifo_cnt);
/*! Read current sample SPI transaction checksum. */
int adis_read_spi_chksum(struct adis_dev *adis, uint32_t *checksum);

/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  x axis.
 */
int adis_read_xg_bias(struct adis_dev *adis, int32_t *xg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  x axis.
 */
int adis_write_xg_bias(struct adis_dev *adis, int32_t xg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  y axis.
 */
int adis_read_yg_bias(struct adis_dev *adis, int32_t *yg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  y axis.
 */
int adis_write_yg_bias(struct adis_dev *adis, int32_t yg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on
 *  z axis.
 */
int adis_read_zg_bias(struct adis_dev *adis, int32_t *zg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on
 *  z axis.
 */
int adis_write_zg_bias(struct adis_dev *adis, int32_t zg_bias);

/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  x axis.
 */
int adis_read_xa_bias(struct adis_dev *adis, int32_t *xa_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  x axis.
 */
int adis_write_xa_bias(struct adis_dev *adis, int32_t xa_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  y axis.
 */
int adis_read_ya_bias(struct adis_dev *adis, int32_t *ya_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  y axis.
 */
int adis_write_ya_bias(struct adis_dev *adis, int32_t ya_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on
 *  z axis.
 */
int adis_read_za_bias(struct adis_dev *adis, int32_t *za_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on
 *  z axis.
 */
int adis_write_za_bias(struct adis_dev *adis, int32_t za_bias);

/*! Gyroscope scale adjustment: read raw gyroscope scale correction on
 *  x axis.
 */
int adis_read_xg_scale(struct adis_dev *adis, int32_t *xg_scale);
/*! Gyroscope scale adjustment: write raw gyroscope scale correction on
 *  x axis.
 */
int adis_write_xg_scale(struct adis_dev *adis, int32_t xg_scale);
/*! Gyroscope scale adjustment: read raw gyroscope scale correction on
 *  y axis.
 */
int adis_read_yg_scale(struct adis_dev *adis, int32_t *yg_scale);
/*! Gyroscope scale adjustment: write raw gyroscope scale correction on
 *  y axis.
 */
int adis_write_yg_scale(struct adis_dev *adis, int32_t yg_scale);
/*! Gyroscope scale adjustment: read raw gyroscope scale correction on
 *  z axis.
 */
int adis_read_zg_scale(struct adis_dev *adis, int32_t *zg_scale);
/*! Gyroscope scale adjustment: write raw gyroscope scale correction on
 *  z axis.
 */
int adis_write_zg_scale(struct adis_dev *adis, int32_t zg_scale);
/*! Acceleration scale adjustment: read raw acceleration scale correction on
 *  x axis.
 */
int adis_read_xa_scale(struct adis_dev *adis, int32_t *xa_scale);
/*! Acceleration scale adjustment: write raw acceleration scale correction on
 *  x axis.
 */
int adis_write_xa_scale(struct adis_dev *adis, int32_t xa_scale);
/*! Acceleration scale adjustment: read raw acceleration scale correction on
 *  y axis.
 */
int adis_read_ya_scale(struct adis_dev *adis, int32_t *ya_scale);
/*! Acceleration scale adjustment: write raw acceleration scale correction on
 *  y axis.
 */
int adis_write_ya_scale(struct adis_dev *adis, int32_t ya_scale);
/*! Acceleration scale adjustment: read raw acceleration scale correction on
 *  z axis.
 */
int adis_read_za_scale(struct adis_dev *adis, int32_t *za_scale);
/*! Acceleration scale adjustment: write raw acceleration scale correction on
 *  z axis.
 */
int adis_write_za_scale(struct adis_dev *adis, int32_t za_scale);

/*! FIFO control: read FIFO enable bit value. */
int adis_read_fifo_en(struct adis_dev *adis, uint32_t *fifo_en);
/*! FIFO control: write FIFO enable bit value. */
int adis_write_fifo_en(struct adis_dev *adis, uint32_t fifo_en);
/*! FIFO overflow: read FIFO enable bit value. */
int adis_read_fifo_overflow(struct adis_dev *adis, uint32_t *fifo_overflow);
/*! FIFO control: write FIFO overflow bit value. */
int adis_write_fifo_overflow(struct adis_dev *adis, uint32_t fifo_overflow);
/*! FIFO control: read FIFO watermark interrupt enable bit value. */
int adis_read_fifo_wm_int_en(struct adis_dev *adis, uint32_t *fifo_wm_int_en);
/*! FIFO control: write FIFO watermark interrupt enable bit value. */
int adis_write_fifo_wm_int_en(struct adis_dev *adis, uint32_t fifo_wm_int_en);
/*! FIFO control: read FIFO watermark interrupt polarity bit value. */
int adis_read_fifo_wm_int_pol(struct adis_dev *adis, uint32_t *fifo_wm_int_pol);
/*! FIFO control: write FIFO watermark interrupt polarity bit value. */
int adis_write_fifo_wm_int_pol(struct adis_dev *adis, uint32_t fifo_wm_int_pol);
/*! FIFO control: read FIFO watermark threshold level value. */
int adis_read_fifo_wm_lvl(struct adis_dev *adis, uint32_t *fifo_wm_lvl);
/*! FIFO control: write FIFO watermark threshold level value. */
int adis_write_fifo_wm_lvl(struct adis_dev *adis, uint32_t fifo_wm_lvl);

/*! Filter control: read filter size variable B value. */
int adis_read_filt_size_var_b(struct adis_dev *adis, uint32_t *filt_size_var_b);
/*! Filter control: write filter size variable B value. */
int adis_write_filt_size_var_b(struct adis_dev *adis, uint32_t filt_size_var_b);
/*! Filter control: read configured filter frequency. */
int adis_read_lpf(struct adis_dev *adis, enum adis_chan_type chan,
		  enum adis_axis_type axis, uint32_t *freq);
/*! Filter control: configure filter for the given filter frequency. */
int adis_write_lpf(struct adis_dev *adis, enum adis_chan_type chan,
		   enum adis_axis_type axis, uint32_t freq);
/*! Range identifier: read gyroscope measurement range value. */
int adis_read_gyro_meas_range(struct adis_dev *adis, uint32_t *gyro_meas_range);

/*! Filter control: Read x axis gyroscope filter enable bit value. */
int adis_read_fir_en_xg(struct adis_dev *adis, uint32_t *fir_en_xg);
/*! Filter control: Write x axis gyroscope filter enable bit value. */
int adis_write_fir_en_xg(struct adis_dev *adis, uint32_t fir_en_xg);
/*! Filter control: Read y axis gyroscope filter enable bit value. */
int adis_read_fir_en_yg(struct adis_dev *adis, uint32_t *fir_en_yg);
/*! Filter control: Write y axis gyroscope filter enable bit value. */
int adis_write_fir_en_yg(struct adis_dev *adis, uint32_t fir_en_yg);
/*! Filter control: Read z axis gyroscope filter enable bit value. */
int adis_read_fir_en_zg(struct adis_dev *adis, uint32_t *fir_en_zg);
/*! Filter control: Write z axis gyroscope filter enable bit value. */
int adis_write_fir_en_zg(struct adis_dev *adis, uint32_t fir_en_zg);
/*! Filter control: Read x axis accelerometer filter enable bit value. */
int adis_read_fir_en_xa(struct adis_dev *adis, uint32_t *fir_en_xa);
/*! Filter control: Write x axis accelerometer filter enable bit value. */
int adis_write_fir_en_xa(struct adis_dev *adis, uint32_t fir_en_xa);
/*! Filter control: Read y axis accelerometer filter enable bit value. */
int adis_read_fir_en_ya(struct adis_dev *adis, uint32_t *fir_en_ya);
/*! Filter control: Write y axis accelerometer filter enable bit value. */
int adis_write_fir_en_ya(struct adis_dev *adis, uint32_t fir_en_ya);
/*! Filter control: Read z axis accelerometer filter enable bit value. */
int adis_read_fir_en_za(struct adis_dev *adis, uint32_t *fir_en_za);
/*! Filter control: Write z axis accelerometer filter enable bit value. */
int adis_write_fir_en_za(struct adis_dev *adis, uint32_t fir_en_za);
/*! Filter control: read x axis gyroscope filter bank selection encoded value. */
int adis_read_fir_bank_sel_xg(struct adis_dev *adis, uint32_t *fir_bank_sel_xg);
/*! Filter control: write x axis gyroscope filter bank selection encoded value. */
int adis_write_fir_bank_sel_xg(struct adis_dev *adis, uint32_t fir_bank_sel_xg);
/*! Filter control: read y axis gyroscope filter bank selection encoded value. */
int adis_read_fir_bank_sel_yg(struct adis_dev *adis, uint32_t *fir_bank_sel_yg);
/*! Filter control: write y axis gyroscope filter bank selection encoded value. */
int adis_write_fir_bank_sel_yg(struct adis_dev *adis, uint32_t fir_bank_sel_yg);
/*! Filter control: read z axis gyroscope filter bank selection encoded value. */
int adis_read_fir_bank_sel_zg(struct adis_dev *adis, uint32_t *fir_bank_sel_zg);
/*! Filter control: write z axis gyroscope filter bank selection encoded value. */
int adis_write_fir_bank_sel_zg(struct adis_dev *adis, uint32_t fir_bank_sel_zg);
/*! Filter control: read x axis accelerometer filter bank selection encoded value. */
int adis_read_fir_bank_sel_xa(struct adis_dev *adis, uint32_t *fir_bank_sel_xa);
/*! Filter control: write x axis accelerometer filter bank selection encoded value. */
int adis_write_fir_bank_sel_xa(struct adis_dev *adis, uint32_t fir_bank_sel_xa);
/*! Filter control: read y axis accelerometer filter bank selection encoded value. */
int adis_read_fir_bank_sel_ya(struct adis_dev *adis, uint32_t *fir_bank_sel_ya);
/*! Filter control: write y axis accelerometer filter bank selection encoded value. */
int adis_write_fir_bank_sel_ya(struct adis_dev *adis, uint32_t fir_bank_sel_ya);
/*! Filter control: read z axis accelerometer filter bank selection encoded value. */
int adis_read_fir_bank_sel_za(struct adis_dev *adis, uint32_t *fir_bank_sel_za);
/*! Filter control: write z axis accelerometer filter bank selection encoded value. */
int adis_write_fir_bank_sel_za(struct adis_dev *adis, uint32_t fir_bank_sel_za);
/*! Filter control: read FIR Filter Coefficient Bank A idx value. */
int adis_read_fir_coef_bank_a(struct adis_dev *adis, uint8_t coef_idx,
			      uint32_t *coef);
/*! Filter control: write FIR Filter Coefficient Bank A idx value. */
int adis_write_fir_coef_bank_a(struct adis_dev *adis, uint8_t coef_idx,
			       uint32_t coef);
/*! Filter control: read FIR Filter Coefficient Bank B idx value. */
int adis_read_fir_coef_bank_b(struct adis_dev *adis, uint8_t coef_idx,
			      uint32_t *coef);
/*! Filter control: write FIR Filter Coefficient Bank B idx value. */
int adis_write_fir_coef_bank_b(struct adis_dev *adis, uint8_t coef_idx,
			       uint32_t coef);
/*! Filter control: read FIR Filter Coefficient Bank C idx value. */
int adis_read_fir_coef_bank_c(struct adis_dev *adis, uint8_t coef_idx,
			      uint32_t *coef);
/*! Filter control: write FIR Filter Coefficient Bank C idx value. */
int adis_write_fir_coef_bank_c(struct adis_dev *adis, uint8_t coef_idx,
			       uint32_t coef);
/*! Filter control: read FIR Filter Coefficient Bank D idx value. */
int adis_read_fir_coef_bank_d(struct adis_dev *adis, uint8_t coef_idx,
			      uint32_t *coef);
/*! Filter control: write FIR Filter Coefficient Bank D idx value. */
int adis_write_fir_coef_bank_d(struct adis_dev *adis, uint8_t coef_idx,
			       uint32_t coef);

/*! Miscellaneous control: read data ready selection encoded value. */
int adis_read_dr_selection(struct adis_dev *adis, uint32_t *dr_selection);
/*! Miscellaneous control: write data ready selection encoded value. */
int adis_write_dr_selection(struct adis_dev *adis, uint32_t dr_selection);
/*! Miscellaneous control: read data ready polarity encoded value. */
int adis_read_dr_polarity(struct adis_dev *adis, uint32_t *dr_polarity);
/*! Miscellaneous control: write data ready polarity encoded value. */
int adis_write_dr_polarity(struct adis_dev *adis, uint32_t dr_polarity);
/*! Miscellaneous control: read data ready enable encoded value. */
int adis_read_dr_enable(struct adis_dev *adis, uint32_t *dr_enable);
/*! Miscellaneous control: write data ready enable encoded value. */
int adis_write_dr_enable(struct adis_dev *adis, uint32_t dr_enable);
/*! Miscellaneous control: read sync selection encoded value. */
int adis_read_sync_selection(struct adis_dev *adis, uint32_t *sync_selection);
/*! Miscellaneous control: write sync selection encoded value. */
int adis_write_sync_selection(struct adis_dev *adis, uint32_t sync_selection);
/*! Miscellaneous control: read sync polarity encoded value. */
int adis_read_sync_polarity(struct adis_dev *adis, uint32_t *sync_polarity);
/*! Miscellaneous control: write sync polarity encoded value. */
int adis_write_sync_polarity(struct adis_dev *adis, uint32_t sync_polarity);
/*! Miscellaneous control: read synchronization mode encoded value. */
int adis_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode);
/*! Miscellaneous control: write synchronization mode encoded value. */
int adis_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
			 uint32_t ext_clk);
/*! Miscellaneous control: read alarm selection encoded value. */
int adis_read_alarm_selection(struct adis_dev *adis, uint32_t *alarm_selection);
/*! Miscellaneous control: write alarm selection encoded value. */
int adis_write_alarm_selection(struct adis_dev *adis, uint32_t alarm_selection);
/*! Miscellaneous control: read alarm polarity encoded value. */
int adis_read_alarm_polarity(struct adis_dev *adis, uint32_t *alarm_polarity);
/*! Miscellaneous control: write alarm polarity encoded value. */
int adis_write_alarm_polarity(struct adis_dev *adis, uint32_t alarm_polarity);
/*! Miscellaneous control: read alarm enable encoded value. */
int adis_read_alarm_enable(struct adis_dev *adis, uint32_t *alarm_enable);
/*! Miscellaneous control: write alarm enable encoded value. */
int adis_write_alarm_enable(struct adis_dev *adis, uint32_t alarm_enable);
/*! Miscellaneous control: read gpio configured direction. */
int adis_read_gpio_dir(struct adis_dev *adis, uint8_t dio_nb, uint32_t *dir);
/*! Miscellaneous control: write gpio direction configuration. */
int adis_write_gpio_dir(struct adis_dev *adis, uint8_t dio_nb, uint32_t dir);
/*! Miscellaneous control: read gpio configured data level. */
int adis_read_gpio_lvl(struct adis_dev *adis, uint8_t dio_nb, uint32_t *level);
/*! Miscellaneous control: write gpio level configuration. */
int adis_write_gpio_lvl(struct adis_dev *adis, uint8_t dio_nb, uint32_t level);
/*! Miscellaneous control: read internal sensor bandwidth encoded value. */
int adis_read_sens_bw(struct adis_dev *adis, uint32_t *sens_bw);
/*! Miscellaneous control: write internal sensor bandwidth encoded value. */
int adis_write_sens_bw(struct adis_dev *adis, uint32_t sens_bw);
/*! Miscellaneous control: read accelerometer FIR filter control bit value. */
int adis_read_accl_fir_enable(struct adis_dev *adis, uint32_t *accl_fir_enable);
/*! Miscellaneous control: write accelerometer FIR filter control bit value. */
int adis_write_accl_fir_enable(struct adis_dev *adis, uint32_t accl_fir_enable);
/*! Miscellaneous control: read gyroscope FIR filter control bit value. */
int adis_read_gyro_fir_enable(struct adis_dev *adis, uint32_t *gyro_fir_enable);
/*! Miscellaneous control: write gyroscope FIR filter control bit value. */
int adis_write_gyro_fir_enable(struct adis_dev *adis, uint32_t gyro_fir_enable);
/*! Miscellaneous control: read point of percussion alignment enable bit
 *  value.
 */
int adis_read_pt_of_perc_algnmt(struct adis_dev *adis,
				uint32_t *pt_of_perc_algnmt);
/*! Miscellaneous control: write point of percussion alignment enable bit
 *  value.
 */
int adis_write_pt_of_perc_algnmt(struct adis_dev *adis,
				 uint32_t pt_of_perc_algnmt);
/*! Miscellaneous control: read linear acceleration compensation enable bit
 *  value.
 */
int adis_read_linear_accl_comp(struct adis_dev *adis,
			       uint32_t *linear_accl_comp);
/*! Miscellaneous control: write linear acceleration compensation enable bit
 *  value.
 */
int adis_write_linear_accl_comp(struct adis_dev *adis,
				uint32_t linear_accl_comp);
/*! Miscellaneous control: read burst selection encoded value. */
int adis_read_burst_sel(struct adis_dev *adis, uint32_t *burst_sel);
/*! Miscellaneous control: write burst selection encoded value. */
int adis_write_burst_sel(struct adis_dev *adis, uint32_t burst_sel);
/*! Miscellaneous control: read burst32 enable bit value. */
int adis_read_burst32(struct adis_dev *adis, uint32_t *burst_size);
/*! Miscellaneous control: write burst32 enable bit value. */
int adis_write_burst32(struct adis_dev *adis, uint32_t burst_size);
/*! Miscellaneous control: read timestamp32 enable bit value. */
int adis_read_timestamp32(struct adis_dev *adis, uint32_t *timestamp32);
/*! Miscellaneous control: write timestamp32 enable bit value. */
int adis_write_timestamp32(struct adis_dev *adis, uint32_t timestamp32);
/*! Miscellaneous control: read 4khz internal sync enable bit value. */
int adis_read_sync_4khz(struct adis_dev *adis, uint32_t *sync_4khz);
/*! Miscellaneous control: write 4khz internal sync enable bit value. */
int adis_write_sync_4khz(struct adis_dev *adis, uint32_t sync_4khz);

/*! Sync input frequency multiplier: read external clock scale factor value. */
int adis_read_up_scale(struct adis_dev *adis, uint32_t *up_scale);
/*! Sync input frequency multiplier: write external clock scale factor value. */
int adis_write_up_scale(struct adis_dev *adis, uint32_t up_scale);

/*! Decimation filter: read decimation rate value.*/
int adis_read_dec_rate(struct adis_dev *adis, uint32_t *dec_rate);
/*! Decimation filter: write decimation rate value.*/
int adis_write_dec_rate(struct adis_dev *adis, uint32_t dec_rate);

/*! Continuous bias estimation: read time bias control. */
int adis_read_bias_corr_tbc(struct adis_dev *adis, uint32_t *bias_corr_tbc);
/*! Continuous bias estimation: write time bias control. */
int adis_write_bias_corr_tbc(struct adis_dev *adis, uint32_t bias_corr_tbc);
/*! Continuous bias estimation: read X-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_xg(struct adis_dev *adis, uint32_t *bias_corr_en_xg);
/*! Continuous bias estimation: write X-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_xg(struct adis_dev *adis, uint32_t bias_corr_en_xg);
/*! Continuous bias estimation: read Y-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_yg(struct adis_dev *adis, uint32_t *bias_corr_en_yg);
/*! Continuous bias estimation: write Y-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_yg(struct adis_dev *adis, uint32_t bias_corr_en_yg);
/*! Continuous bias estimation: read Z-axis gyroscope bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_zg(struct adis_dev *adis, uint32_t *bias_corr_en_zg);
/*! Continuous bias estimation: write Z-axis gyroscope bias correction enable
 *  bit.
 */
int adis_write_bias_corr_en_zg(struct adis_dev *adis, uint32_t bias_corr_en_zg);
/*! Continuous bias estimation: read X-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_xa(struct adis_dev *adis, uint32_t *bias_corr_en_xa);
/*! Continuous bias estimation: write X-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_xa(struct adis_dev *adis, uint32_t bias_corr_en_xa);
/*! Continuous bias estimation: read Y-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_ya(struct adis_dev *adis, uint32_t *bias_corr_en_ya);
/*! Continuous bias estimation: write Y-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_ya(struct adis_dev *adis, uint32_t bias_corr_en_ya);
/*! Continuous bias estimation: read Z-axis accelerometer bias correction enable
 *  bit.
 */
int adis_read_bias_corr_en_za(struct adis_dev *adis, uint32_t *bias_corr_en_za);
/*! Continuous bias estimation: write Z-axis accelerometer bias correction
 *  enable bit.
 */
int adis_write_bias_corr_en_za(struct adis_dev *adis, uint32_t bias_corr_en_za);

/*! Global commands: perform bias correction update command. */
int adis_cmd_bias_corr_update(struct adis_dev *adis);
/*! Global commands: perform factory calibration restore command. */
int adis_cmd_fact_calib_restore(struct adis_dev *adis);
/*! Global commands: perform sensor self test command. */
int adis_cmd_snsr_self_test(struct adis_dev *adis);
/*! Global commands: perform flash memory update command. */
int adis_cmd_fls_mem_update(struct adis_dev *adis);
/*! Global commands: perform flash memory test command. */
int adis_cmd_fls_mem_test(struct adis_dev *adis);
/*! Global commands: perform fifo flush command. */
int adis_cmd_fifo_flush(struct adis_dev *adis);
/*! Global commands: perform software reset command. */
int adis_cmd_sw_res(struct adis_dev *adis);
/*! Global commands: perform write lock command. */
int adis_cmd_write_lock(struct adis_dev *adis);

/*! Device identification data: read processor revision. */
int adis_read_proc_rev(struct adis_dev *adis, uint32_t *proc_rev);
/*! Device identification data: read firmware revision. */
int adis_read_firm_rev(struct adis_dev *adis, uint32_t *firm_rev);
/*! Device identification data: read firmware revision day. */
int adis_read_firm_d(struct adis_dev *adis, uint32_t *firm_d);
/*! Device identification data: read firmware revision month. */
int adis_read_firm_m(struct adis_dev *adis, uint32_t *firm_m);
/*! Device identification data: read firmware revision year. */
int adis_read_firm_y(struct adis_dev *adis, uint32_t *firm_y);
/*! Device identification data: read boot loader revision. */
int adis_read_boot_rev(struct adis_dev *adis, uint32_t *boot_rev);
/*! Device identification data: read product identification. */
int adis_read_prod_id(struct adis_dev *adis, uint32_t *prod_id);
/*! Device identification data: read serial number. */
int adis_read_serial_num(struct adis_dev *adis, uint32_t *serial_num);
/*! Device identification data: read lot specific number. */
int adis_read_lot_num(struct adis_dev *adis, uint32_t *lot_num);

/*! Scratch pad registers: read scratch register 1. */
int adis_read_usr_scr_1(struct adis_dev *adis, uint32_t *usr_scr_1);
/*! Scratch pad registers: write scratch register 1. */
int adis_write_usr_scr_1(struct adis_dev *adis, uint32_t usr_scr_1);
/*! Scratch pad registers: read scratch register 2. */
int adis_read_usr_scr_2(struct adis_dev *adis, uint32_t *usr_scr_2);
/*! Scratch pad registers: write scratch register 2. */
int adis_write_usr_scr_2(struct adis_dev *adis, uint32_t usr_scr_2);
/*! Scratch pad registers: read scratch register 3. */
int adis_read_usr_scr_3(struct adis_dev *adis, uint32_t *usr_scr_3);
/*! Scratch pad registers: write scratch register 3. */
int adis_write_usr_scr_3(struct adis_dev *adis, uint32_t usr_scr_3);
/*! Scratch pad registers: read scratch register 4. */
int adis_read_usr_scr_4(struct adis_dev *adis, uint32_t *usr_scr_4);
/*! Scratch pad registers: write scratch register 4. */
int adis_write_usr_scr_4(struct adis_dev *adis, uint32_t usr_scr_4);

/*! Flash counter: read flash memory write cycle counter value. */
int adis_read_fls_mem_wr_cntr(struct adis_dev *adis, uint32_t *fls_mem_wr_cntr);

/*! FIR Coefficients: read FIR Filter Coefficient C_coef_idx value. */
int adis_read_fir_coef(struct adis_dev *adis, uint8_t coef_idx, uint32_t *coef);
/*! FIR Coefficients: write FIR Filter Coefficient C_coef_idx value. */
int adis_write_fir_coef(struct adis_dev *adis, uint8_t coef_idx, uint32_t coef);

/*! Read burst data */
int adis_read_burst_data(struct adis_dev *adis,struct adis_burst_data *data,
			 bool burst32, uint8_t burst_sel, bool fifo_pop, bool crc_check);

/*! Update external clock frequency. */
int adis_update_ext_clk_freq(struct adis_dev *adis, uint32_t clk_freq);

/*! Read adis synchronization clock frequency value in Hertz. */
int adis_get_sync_clk_freq(struct adis_dev *adis, uint32_t *clk_freq);

/*! Read adis device gyroscope scale in fractional form. */
int adis_get_anglvel_scale(struct adis_dev *adis,
			   struct adis_scale_fractional *anglvel_scale);

/*! Read adis device acceleration scale in fractional form. */
int adis_get_accl_scale(struct adis_dev *adis,
			struct adis_scale_fractional *accl_scale);

/*! Read adis device delta angle scale in fractional form. */
int adis_get_deltaangl_scale(struct adis_dev *adis,
			     struct adis_scale_fractional_log2 *deltaangl_scale);

/*! Read adis device delta velocity scale in fractional form. */
int adis_get_deltavelocity_scale(struct adis_dev *adis,
				 struct adis_scale_fractional_log2 *deltavelocity_scale);

/*! Read adis device temperature scale in fractional form. */
int adis_get_temp_scale(struct adis_dev *adis,
			struct adis_scale_fractional *temp_scale);

/*! Read adis device temperature offset in integer form. */
int adis_get_temp_offset(struct adis_dev *adis, int *temp_offset);
#endif
