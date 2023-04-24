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

#define ADIS_SYNC_DEFAULT	0
#define ADIS_SYNC_DIRECT	1
#define ADIS_SYNC_SCALED	2
#define ADIS_SYNC_OUTPUT	3

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Supported device ids
 */
enum adis_device_id {
	ADIS16505_1,
	ADIS16505_2,
	ADIS16505_3,
	ADIS16575_2,
	ADIS16575_3,
	ADIS16576_2,
	ADIS16576_3,
	ADIS16577_2,
	ADIS16577_3,
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
	uint8_t x_axis_gyro_failure	:1;
	/** Y-Axis gyroscope failure. */
	uint8_t y_axis_gyro_failure	:1;
	/** Z-Axis gyroscope failure. */
	uint8_t z_axis_gyro_failure	:1;
	/** X-Axis accelerometer failure. */
	uint8_t x_axis_accl_failure	:1;
	/** Y-Axis accelerometer failure. */
	uint8_t y_axis_accl_failure	:1;
	/** Z-Axis accelerometer failure. */
	uint8_t z_axis_accl_failure	:1;
	/** ADuC microcontroller fault. */
	uint8_t aduc_mcu_fault		:1;
	/** Checksum error.  */
	uint8_t checksum_err		:1;
	/** Flash memory write count exceeded. */
	uint8_t fls_mem_wr_cnt_exceed	:1;
};

/** @struct adis_timeout
 *  @brief ADIS chip timeouts
 */
struct adis_timeout {
	/** Wait time in milliseconds needed after a hardware reset. */
	uint16_t reset_ms;
	/** Wait time in milliseconds needed after a factory calibration restore
	 *  command is issued.
	 */
	uint16_t fact_calib_restore_ms;
	/** Wait time in milliseconds needed after a self test
	 * command is issued.
	 */
	uint16_t self_test_ms;
	/** Wait time in milliseconds needed after a flash memory update command
	 *  is issued.
	 */
	uint16_t fls_mem_update_ms;
	/** Wait time in milliseconds needed after a flash memory test command
	 *  is issued.
	 */
	uint16_t fls_mem_test_ms;
	/** Wait time in milliseconds needed after a software reset command is
	 * issued.
	 */
	uint16_t sw_reset_ms;
	/** Wait time in microseconds needed after a write command is issued for
	 *  the decimation rate field.
	 */
	uint16_t dec_rate_update_us;
	/** Wait time in microseconds needed after a write command is issued for
	 *  the filter size variable b field.
	 */
	uint16_t filt_size_var_b_update_us;
	/** Wait time in microseconds needed after a write command
	 *  is issued in the miscellaneous control register.
	 */
	uint16_t msc_reg_update_us;
	/** Wait time in milliseconds needed after a write command is issued to
	 *  change the internal sensor bandwidth field in the miscellaneous
	 *  control register.
	 */
	uint16_t sens_bw_update_ms;
};

/** @struct adis_clk_freq_limit
 *  @brief ADIS frequency limit for input synchronization clock
 */
struct adis_clk_freq_limit {
	/** Minimum allowed frequency for adis clocks in Hertz. */
	uint32_t min_freq;
	/** Maximum allowed frequency for adis clocks in Hertz. */
	uint32_t max_freq;
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
	/** Current device id, specified by the user */
	enum adis_device_id		dev_id;
	/** Current page to be accessed in register map. */
	uint32_t			current_page;
	/** Transmit buffer used in SPI transactions. */
	uint8_t				tx[10];
	/** Receive buffer used in SPI transactions. */
	uint8_t				rx[4];
	/** Internal clock frequency in Hertz. */
	uint32_t 			int_clk;
	/** External clock frequency in Hertz. */
	uint32_t 			ext_clk;
	/** Set to true if device fifo is enabled. */
	bool				fifo_enabled;
};

/** @struct adis_init_param
 *  @brief ADIS device initialization parameters
 */
struct adis_init_param {
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

/** @struct adis_field
 *  @brief ADIS device field structure
 */
struct adis_field {
	/** Address of the register in which the field is found. */
	uint8_t 	reg_addr;
	/** Size of te register in which the field is found. */
	uint8_t 	reg_size;
	/** The mask of the field in a register. */
	uint32_t 	field_mask;
};

/** @struct adis_data_field_map_def
 *  @brief ADIS filed map definition structure
 */
struct adis_data_field_map_def {
	/** Status/Error Flag Indicators register. */
	struct adis_field diag_stat;
	/** Sensor initialization failure bit mask. */
	uint16_t diag_snsr_init_failure_mask;
	/** Data path overrun bit mask. */
	uint16_t diag_data_path_overrun_mask;
	/** Flash memory update failure bit mask. */
	uint16_t diag_fls_mem_update_failure_mask;
	/** SPI communication error bit mask. */
	uint16_t diag_spi_comm_err_mask;
	/** Standby mode bit mask. */
	uint16_t diag_standby_mode_mask;
	/** Sensor failure bit mask. */
	uint16_t diag_snsr_failure_mask;
	/** Memory failure bit mask. */
	uint16_t diag_mem_failure_mask;
	/** Clock error bit mask. */
	uint16_t diag_clk_err_mask;
	/** Gyroscope 1 failure bit mask. */
	uint16_t diag_gyro1_failure_mask;
	/** Gyroscope 2 failure bit mask. */
	uint16_t diag_gyro2_failure_mask;
	/** Accelerometer failure bit mask. */
	uint16_t diag_accl_failure_mask;
	/** X-Axis gyroscope failure bit mask. */
	uint16_t diag_x_axis_gyro_failure_mask;
	/** Y-Axis gyroscope failure bit mask. */
	uint16_t diag_y_axis_gyro_failure_mask;
	/** Z-Axis gyroscope failure bit mask. */
	uint16_t diag_z_axis_gyro_failure_mask;
	/** X-Axis accelerometer failure bit mask. */
	uint16_t diag_x_axis_accl_failure_mask;
	/** Y-Axis accelerometer failure bit mask. */
	uint16_t diag_y_axis_accl_failure_mask;
	/** Z-Axis accelerometer failure bit mask. */
	uint16_t diag_z_axis_accl_failure_mask;
	/** ADuC microcontroller fault bit mask. */
	uint16_t diag_aduc_mcu_fault_mask;

	/** X-axis gyroscope raw data. */
	struct adis_field x_gyro;
	/** Y-axis gyroscope raw data. */
	struct adis_field y_gyro;
	/** Z-axis gyroscope raw data. */
	struct adis_field z_gyro;
	/** X-axis accelerometer raw data. */
	struct adis_field x_accl;
	/** Y-axis accelerometer raw data. */
	struct adis_field y_accl;
	/** Z-axis accelerometer raw data. */
	struct adis_field z_accl;
	/** Temperature raw data. */
	struct adis_field temp_out;

	/** Time stamp raw data. */
	struct adis_field time_stamp;
	/** Data update counter. */
	struct adis_field data_cntr;

	/** X-axis delta angle raw data. */
	struct adis_field x_deltang;
	/** Y-axis delta angle raw data. */
	struct adis_field y_deltang;
	/** Z-axis delta angle raw data. */
	struct adis_field z_deltang;
	/** X-axis delta velocity raw data. */
	struct adis_field x_deltvel;
	/** Y-axis delta velocity raw data. */
	struct adis_field y_deltvel;
	/** Z-axis delta velocity raw data. */
	struct adis_field z_deltvel;

	/** FIFO sample count.  */
	struct adis_field fifo_cnt;
	/** SPI dynamic checksum.  */
	struct adis_field spi_chksum;

	/** X-axis gyroscope offset correction. */
	struct adis_field xg_bias;
	/** Y-axis gyroscope offset correction. */
	struct adis_field yg_bias;
	/** Z-axis gyroscope offset correction. */
	struct adis_field zg_bias;
	/** X-axis accelerometer offset correction. */
	struct adis_field xa_bias;
	/** Y-axis accelerometer offset correction. */
	struct adis_field ya_bias;
	/** Z-axis accelerometer offset correction. */
	struct adis_field za_bias;

	/** FIFO mode enable. */
	struct adis_field fifo_en;
	/** FIFO overflow behavior. */
	struct adis_field fifo_overflow;
	/** FIFO watermark interrupt enable. */
	struct adis_field fifo_wm_int_en;
	/** FIFO watermark interrupt polarity. */
	struct adis_field fifo_wm_int_pol;
	/** FIFO watermark threshold level. */
	struct adis_field fifo_wm_lvl;

	/** Filter size variable B. */
	struct adis_field filt_size_var_b;

	/** Gyroscope measurement range. */
	struct adis_field gyro_meas_range;

	/** Data ready polarity. */
	struct adis_field dr_polarity;
	/** SYNC signal polarity. */
	struct adis_field sync_polarity;
	/** SYNC mode select. */
	struct adis_field sync_mode;
	/** Internal sensor bandwidth. */
	struct adis_field sens_bw;
	/** Point of percussion alignment enable bit. */
	struct adis_field pt_of_perc_algnmt;
	/** Linear acceleration compensation enable bit. */
	struct adis_field linear_accl_comp;
	/** Burst read output array selection. */
	struct adis_field burst_sel;
	/** 32-bit burst enable bit. */
	struct adis_field burst32;
	/** 32-bit timestamp enable bit. */
	struct adis_field timestamp32;
	/** 4khz internal sync enable bit. */
	struct adis_field sync_4khz;

	/** External clock scale factor. */
	struct adis_field up_scale;
	/** Decimation rate. */
	struct adis_field dec_rate;

	/** Bias correction time base control. */
	struct adis_field bias_corr_tbc;
	/** X-axis gyroscope bias correction enable. */
	struct adis_field bias_corr_en_xg;
	/** Y-axis gyroscope bias correction enable. */
	struct adis_field bias_corr_en_yg;
	/** Z-axis gyroscope bias correction enable. */
	struct adis_field bias_corr_en_zg;
	/** X-axis accelerometer bias correction enable. */
	struct adis_field bias_corr_en_xa;
	/** Y-axis accelerometer bias correction enable. */
	struct adis_field bias_corr_en_ya;
	/** Z-axis accelerometer bias correction enable. */
	struct adis_field bias_corr_en_za;

	/** Bias correction update command bit. */
	struct adis_field bias_corr_update;
	/** Factory calibration restore command bit. */
	struct adis_field fact_calib_restore;
	/** Sensor self test command bit. */
	struct adis_field snsr_self_test;
	/** Flash memory update command bit. */
	struct adis_field fls_mem_update;
	/** Flash memory test command bit. */
	struct adis_field fls_mem_test;
	/** FIFO flush command bit. */
	struct adis_field fifo_flush;
	/** Software reset command bit. */
	struct adis_field sw_res;

	/** Firmware revision. */
	struct adis_field firm_rev;
	/** Factory configuration day. */
	struct adis_field firm_d;
	/** Factory configuration month. */
	struct adis_field firm_m;
	/** Factory configuration year. */
	struct adis_field firm_y;
	/** Product identification. */
	struct adis_field prod_id;
	/** Serial number. */
	struct adis_field serial_num;

	/** User Scratch Pad Register 1. */
	struct adis_field usr_scr_1;
	/** User Scratch Pad Register 2. */
	struct adis_field usr_scr_2;
	/** User Scratch Pad Register 3. */
	struct adis_field usr_scr_3;

	/** Flash memory endurance counter. */
	struct adis_field fls_mem_wr_cntr;
};

/** @struct adis_chip_info
 *  @brief ADIS specific chip information structure
 */
struct adis_chip_info {
	/** Chip specific initialization parameters. */
	const struct adis_init_param 		*ip;
	/** Chip specific field map configuration. */
	const struct adis_data_field_map_def 	*field_map;
	/** Chip specific synchronization clock frequency limits. */
	const struct adis_clk_freq_limit	*sync_clk_freq_limits;
	/** Chip specific sampling clock frequency limits. */
	const struct adis_clk_freq_limit	sampling_clk_limits;
	/** Chip specific timeouts. */
	const struct adis_timeout 		*timeouts;
	/** Chip specific read delay for SPI transactions. */
	uint32_t 				read_delay;
	/** Chip specific write delay for SPI transactions. */
	uint32_t 				write_delay;
	/** Chip specific chip select change delay for SPI transactions. */
	uint32_t 				cs_change_delay;
	/** Chip specific flag to specify whether the SPI transaction addressing
	 *  supports paging.
	 */
	bool 					has_paging;
	/** Chip specific flag to specify wether a burst request is needed
	 *  before reading burst data.
	 */
	bool					burst_request;
	/** Chip specific filter size variable B field maximum allowed value. */
	uint16_t 				filt_size_var_b_max;
	/** Chip specific decimation rate field maximum allowed	value. */
	uint16_t 				dec_rate_max;
	/** Chip specific sync mode select field maximum allowed value. */
	uint8_t 				sync_mode_max;
	/** Chip specific flash memory write counter maximum allowed value. */
	uint32_t				fls_mem_wr_cntr_max;
	/** Chip specific bias correction time base control maximum allowed
	 *  encoded value.
	 */
	uint8_t 				bias_corr_tbc_max;
	/** Chip specific internal clock frequency in Hertz. */
	uint32_t 				int_clk;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis device. */
int adis_init(struct adis_dev **adis, const struct adis_chip_info *info);
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

/*! Range identifier: read gyroscope measurement range value. */
int adis_read_gyro_meas_range(struct adis_dev *adis, uint32_t *gyro_meas_range);

/*! Miscellaneous control: read data ready polarity encoded value. */
int adis_read_dr_polarity(struct adis_dev *adis, uint32_t *dr_polarity);
/*! Miscellaneous control: write data ready polarity encoded value. */
int adis_write_dr_polarity(struct adis_dev *adis, uint32_t dr_polarity);
/*! Miscellaneous control: read sync polarity encoded value. */
int adis_read_sync_polarity(struct adis_dev *adis, uint32_t *sync_polarity);
/*! Miscellaneous control: write sync polarity encoded value. */
int adis_write_sync_polarity(struct adis_dev *adis, uint32_t sync_polarity);
/*! Miscellaneous control: read synchronization mode encoded value. */
int adis_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode);
/*! Miscellaneous control: write synchronization mode encoded value. */
int adis_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
			 uint32_t ext_clk);
/*! Miscellaneous control: read internal sensor bandwidth encoded value. */
int adis_read_sens_bw(struct adis_dev *adis, uint32_t *sens_bw);
/*! Miscellaneous control: write internal sensor bandwidth encoded value. */
int adis_write_sens_bw(struct adis_dev *adis, uint32_t sens_bw);
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

/*! Device identification data: read firmware revision. */
int adis_read_firm_rev(struct adis_dev *adis, uint32_t *firm_rev);
/*! Device identification data: read firmware revision day. */
int adis_read_firm_d(struct adis_dev *adis, uint32_t *firm_d);
/*! Device identification data: read firmware revision month. */
int adis_read_firm_m(struct adis_dev *adis, uint32_t *firm_m);
/*! Device identification data: read firmware revision year. */
int adis_read_firm_y(struct adis_dev *adis, uint32_t *firm_y);
/*! Device identification data: read product identification. */
int adis_read_prod_id(struct adis_dev *adis, uint32_t *prod_id);
/*! Device identification data: read serial number. */
int adis_read_serial_num(struct adis_dev *adis, uint32_t *serial_num);

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

/*! Flash counter: read flash memory write cycle counter value. */
int adis_read_fls_mem_wr_cntr(struct adis_dev *adis, uint32_t *fls_mem_wr_cntr);

/*! Read burst data */
int adis_read_burst_data(struct adis_dev *adis, uint8_t burst_data_size,
			 uint16_t *burst_data, uint8_t burst_size_selection);

/*! Update external clock frequency. */
int adis_update_ext_clk_freq(struct adis_dev *adis, uint32_t clk_freq);

/*! Read adis synchronization clock frequency value in Hertz. */
int adis_get_sync_clk_freq(struct adis_dev *adis, uint32_t *clk_freq);

#endif
