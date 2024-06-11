/***************************************************************************//**
 *   @file   adis_internals.h
 *   @brief  Internal include file used for specific chip driver definitions,
 *           which are not useful for the user.
 *   @author RBolboac (ramona.gradnariu@analog.com)
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

#ifndef __ADIS_INTERNALS_H__
#define __ADIS_INTERNALS_H__

#define ADIS_HAS_BURST32		NO_OS_BIT(0)
#define ADIS_HAS_BURST_DELTA_DATA	NO_OS_BIT(1)
#define ADIS_HAS_FIFO			NO_OS_BIT(2)

#define ADIS_READ_BURST_DATA_CMD_MSB   0x68
#define ADIS_READ_BURST_DATA_CMD_LSB   0x00
#define ADIS_READ_BURST_DATA_CMD_SIZE  2  /* in bytes */

#define ADIS_PAGE_SIZE			0x80
#define ADIS_REG_PAGE_ID		0x00

#define ADIS_WRITE_REG(reg)		((NO_OS_BIT(7) | (reg)))
#define ADIS_READ_REG(reg)		((reg) & NO_OS_GENMASK(6,0))

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

/** @struct adis_scale_members
 *  @brief ADIS generic scale members structure.
 */
struct adis_scale_members {
	uint32_t scale_m1;
	uint32_t scale_m2;
};

/** @struct adis_field
 *  @brief ADIS device field structure
 */
struct adis_field {
	/** Address of the register in which the field is found. */
	uint16_t 	reg_addr;
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
	uint32_t diag_snsr_init_failure_mask;
	/** Data path overrun bit mask. */
	uint32_t diag_data_path_overrun_mask;
	/** Flash memory update failure bit mask. */
	uint32_t diag_fls_mem_update_failure_mask;
	/** SPI communication error bit mask. */
	uint32_t diag_spi_comm_err_mask;
	/** Standby mode bit mask. */
	uint32_t diag_standby_mode_mask;
	/** Sensor failure bit mask. */
	uint32_t diag_snsr_failure_mask;
	/** Memory failure bit mask. */
	uint32_t diag_mem_failure_mask;
	/** Clock error bit mask. */
	uint32_t diag_clk_err_mask;
	/** Gyroscope 1 failure bit mask. */
	uint32_t diag_gyro1_failure_mask;
	/** Gyroscope 2 failure bit mask. */
	uint32_t diag_gyro2_failure_mask;
	/** Accelerometer failure bit mask. */
	uint32_t diag_accl_failure_mask;
	/** X-Axis gyroscope failure bit mask. */
	uint32_t diag_x_axis_gyro_failure_mask;
	/** Y-Axis gyroscope failure bit mask. */
	uint32_t diag_y_axis_gyro_failure_mask;
	/** Z-Axis gyroscope failure bit mask. */
	uint32_t diag_z_axis_gyro_failure_mask;
	/** X-Axis accelerometer failure bit mask. */
	uint32_t diag_x_axis_accl_failure_mask;
	/** Y-Axis accelerometer failure bit mask. */
	uint32_t diag_y_axis_accl_failure_mask;
	/** Z-Axis accelerometer failure bit mask. */
	uint32_t diag_z_axis_accl_failure_mask;
	/** ADuC microcontroller fault bit mask. */
	uint32_t diag_aduc_mcu_fault_mask;
	/** Configuration and/or calibration CRC error bit mask. */
	uint32_t diag_config_calib_crc_error_mask;
	/** Overrange event occurred bit mask. */
	uint32_t diag_overrange_mask;
	/** Temperature error bit mask. */
	uint32_t diag_temp_err_mask;
	/** Power supply failure bit mask. */
	uint32_t diag_power_supply_failure_mask;
	/** Power supply failure bit mask. */
	uint32_t diag_boot_memory_failure_mask;
	/** Register NVM error bit mask. */
	uint32_t diag_reg_nvm_err_mask;
	/** Watchdog timer flag bit mask. */
	uint32_t diag_wdg_timer_flag_mask;
	/** Internal processor supply error bit mask. */
	uint32_t diag_int_proc_supply_err_mask;
	/** External 5V supply error bit mask. */
	uint32_t diag_ext_5v_supply_err_mask;
	/** Internal sensor supply error bit mask. */
	uint32_t diag_int_snsr_supply_err_mask;
	/** Internal regulator error bit mask. */
	uint32_t diag_int_reg_err_mask;

	/** Status/Error Flag Indicators register. */
	struct adis_field temp_flags;

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

	/** X-axis gyroscope scale adjustment. */
	struct adis_field xg_scale;
	/** Y-axis gyroscope scale adjustment. */
	struct adis_field yg_scale;
	/** Z-axis gyroscope scale adjustment. */
	struct adis_field zg_scale;
	/** X-axis accelerometer scale adjustment. */
	struct adis_field xa_scale;
	/** Y-axis accelerometer scale adjustment. */
	struct adis_field ya_scale;
	/** Z-axis accelerometer scale adjustment. */
	struct adis_field za_scale;

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

	/** Data ready line selection. */
	struct adis_field dr_selection;
	/** Data ready polarity. */
	struct adis_field dr_polarity;
	/** Data ready enable. */
	struct adis_field dr_enable;
	/** SYNC signal input line selection. */
	struct adis_field sync_selection;
	/** SYNC signal polarity. */
	struct adis_field sync_polarity;
	/** SYNC mode select. */
	struct adis_field sync_mode;
	/** Alarm indicator select. */
	struct adis_field alarm_selection;
	/** Alarm indicator polarity. */
	struct adis_field alarm_polarity;
	/** Alarm indicator enable. */
	struct adis_field alarm_enable;
	/** General-purpose input and output control. */
	struct adis_field gpio_ctrl;
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
	/** Accelerometer FIR filter control bit. */
	struct adis_field accl_fir_enable;
	/** Gyroscope FIR filter control bit. */
	struct adis_field gyro_fir_enable;

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

	/** Write lock command bit. */
	struct adis_field write_lock;
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

	/** Processor revision number. */
	struct adis_field proc_rev;
	/** Firmware revision. */
	struct adis_field firm_rev;
	/** Factory configuration day. */
	struct adis_field firm_d;
	/** Factory configuration month. */
	struct adis_field firm_m;
	/** Factory configuration year. */
	struct adis_field firm_y;
	/** Boot loader revision. */
	struct adis_field boot_rev;
	/** Product identification. */
	struct adis_field prod_id;
	/** Serial number. */
	struct adis_field serial_num;
	/** Lot specific number. */
	struct adis_field lot_num;

	/** User Scratch Pad Register 1. */
	struct adis_field usr_scr_1;
	/** User Scratch Pad Register 2. */
	struct adis_field usr_scr_2;
	/** User Scratch Pad Register 3. */
	struct adis_field usr_scr_3;
	/** User Scratch Pad Register 4. */
	struct adis_field usr_scr_4;

	/** Flash memory endurance counter. */
	struct adis_field fls_mem_wr_cntr;

	/* FIR Filter Coefficient C0 */
	struct adis_field coeff_c0;
	/** X-axis gyroscope filter enable. */
	struct adis_field fir_en_xg;
	/** Y-axis gyroscope filter enable. */
	struct adis_field fir_en_yg;
	/** Z-axis gyroscope filter enable. */
	struct adis_field fir_en_zg;
	/** X-axis accelerometer filter enable. */
	struct adis_field fir_en_xa;
	/** Y-axis accelerometer filter enable. */
	struct adis_field fir_en_ya;
	/** Z-axis accelerometer filter enable. */
	struct adis_field fir_en_za;
	/** X-axis gyroscope filter bank selection. */
	struct adis_field fir_bank_sel_xg;
	/** Y-axis gyroscope filter bank selection. */
	struct adis_field fir_bank_sel_yg;
	/** Z-axis gyroscope filter bank selection. */
	struct adis_field fir_bank_sel_zg;
	/** X-axis accelerometer filter bank selection. */
	struct adis_field fir_bank_sel_xa;
	/** Y-axis accelerometer filter bank selection. */
	struct adis_field fir_bank_sel_ya;
	/** Z-axis accelerometer filter bank selection. */
	struct adis_field fir_bank_sel_za;
	/* FIR Filter Coefficient Bank A */
	struct adis_field coeff_bank_a;
	/* FIR Filter Coefficient Bank B */
	struct adis_field coeff_bank_b;
	/* FIR Filter Coefficient Bank C */
	struct adis_field coeff_bank_c;
	/* FIR Filter Coefficient Bank D */
	struct adis_field coeff_bank_d;
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
	/** Chip specific flags. */
	const uint32_t flags;
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
	/** Chip specific flag to specify whether a write lock setting is available.*/
	bool					has_lock;
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
	/** Maximum FIR filter coefficient id. */
	uint8_t					fir_coef_idx_max;
	/** Chip specific internal clock frequency in Hertz. */
	uint32_t 				int_clk;
	/** Chip specific implementation to obtain the channel scale members. */
	int (*get_scale)(struct adis_dev *adis,
			 uint32_t *scale_m1, uint32_t *scale_m2,
			 enum adis_chan_type chan_type);
	/** Chip specific implementation for reading a register.  */
	int (*read_reg)(struct adis_dev *adis,  uint32_t reg, uint32_t *val,
			uint32_t size);
	/** Chip specific implementation for writing a register.  */
	int (*write_reg)(struct adis_dev *adis,  uint32_t reg, uint32_t val,
			 uint32_t size);
	/** Chip specifc implementation for reading burst data. */
	int (*read_burst_data)(struct adis_dev *adis,struct adis_burst_data *data,
			       bool burst32, uint8_t burst_sel, bool fifo_pop, bool crc_check);
	/** Chip specific implementation for reading channel offset. */
	int (*get_offset)(struct adis_dev *adis,
			  int *offset,
			  enum adis_chan_type chan_type);
	/** Chip specific implementation for reading sync_mode. */
	int (*read_sync_mode)(struct adis_dev *adis, uint32_t *sync_mode);
	/** Chip specific implementation for writing sync_mode. */
	int (*write_sync_mode)(struct adis_dev *adis, uint32_t sync_mode,
			       uint32_t ext_clk);
	/** Chip specific implementation for writing the low pass filter frequency. */
	int (*write_lpf)(struct adis_dev *adis, enum adis_chan_type chan,
			 enum adis_axis_type axis, uint32_t freq);
	/** Chip specific implementation for reading the low pass filter frequency. */
	int (*read_lpf)(struct adis_dev *adis, enum adis_chan_type chan,
			enum adis_axis_type axis, uint32_t *freq);
};

/*! Check if the checksum for burst data is correct. */
bool adis_validate_checksum(uint8_t *buffer, uint8_t size, uint8_t idx);
/*! Update device diagnosis flags according to the received parameter. */
void adis_update_diag_flags(struct adis_dev *adis, uint32_t diag_stat);
/*! Update temperature flags. */
void adis_update_temp_flags(struct adis_dev *adis, uint16_t temp_reg);
/*! Write adis 32-bit unsigned field. */
int adis_write_field_u32(struct adis_dev *adis, struct adis_field field,
			 uint32_t field_val);
/*! Read adis 32-bit signed field. */
int adis_read_field_s32(struct adis_dev *adis, struct adis_field field,
			int32_t *field_val);
/*! Read adis 32-bit unsigned field. */
int adis_read_field_u32(struct adis_dev *adis, struct adis_field field,
			uint32_t *field_val);

#endif
