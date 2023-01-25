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
#define ADIS_SYNC_PULSE		5

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Structure which maps the read burst data
 */
struct adis_burst_data {
	uint16_t 	diag_stat;
	int 		x_gyro;
	int 		y_gyro;
	int 		z_gyro;
	int 		x_accl;
	int 		y_accl;
	int 		z_accl;
	int 		temp_out;
	uint16_t 	data_cntr;
};

/**
 * @brief Bitfield struct which maps on the diagnosis register
 */
struct _adis_diag_flags {
	uint8_t DATA_PATH_OVERRUN	:1;
	uint8_t SPI_COMM_ERR		:1;
	uint8_t CHECKSUM_ERR		:1;
	uint8_t STANDBY_MODE		:1;
	uint8_t CLOCK_ERR		:1;
	uint8_t FLS_MEM_UPDATE_ERR	:1;
	uint8_t FLS_MEM_TEST_ERR	:1;
	uint8_t FLS_MEM_WR_CNT_EXCEED	:1;
	uint8_t SNSR_SELF_TEST_ERR	:1;
	uint8_t GYRO1_SELF_TEST_ERR	:1;
	uint8_t GYRO2_SELF_TEST_ERR	:1;
	uint8_t ACCL_SELF_TEST_ERR	:1;
};

/**
 * @brief Union for device diagnosis flags
 */
union adis_diag_flags {
	struct _adis_diag_flags adis_diag_flags_bits;
	uint16_t 		value;
};

/**
 * @brief ADIS chip timeouts
 */
struct adis_timeout {
	uint16_t reset_ms;
	uint16_t sw_reset_ms;
	uint16_t self_test_ms;
	uint16_t filter_update_us;
	uint16_t msc_reg_update_us;
	uint16_t sens_bw_update_ms;
};

/**
 * @brief ADIS sync rate min and max limits
 */
struct adis_sync_rate_limit {
	unsigned int sync_min_rate;
	unsigned int sync_max_rate;
};

/**
 * @brief ADIS device descriptor structure
 */
struct adis_dev {
	struct no_os_spi_desc	*spi_desc;
	struct no_os_gpio_desc	*gpio_reset;
	unsigned int		current_page;
	uint8_t			tx[10];
	uint8_t			rx[4];
	const struct adis_data  *data;
	union adis_diag_flags 	diag_flags;
	unsigned int 		burst_size;
	unsigned int   		burst_sel;
	unsigned int 		clk_freq;
	unsigned int 		ext_clk;
	unsigned int		sync_mode;
};

/**
 * @brief ADIS device initialization parameters
 */
struct adis_init_param {
	struct no_os_spi_init_param 	*spi_init;
	struct no_os_gpio_init_param	*gpio_reset;
	unsigned int 			ext_clk;
	unsigned int			sync_mode;
};

/**
 * @brief ADIS device register structure
 */
struct adis_reg {
	uint8_t 	addr;
	uint8_t 	size;
	uint32_t 	mask;
};

/**
 * @brief ADIS memory map data definition structure
 */
struct adis_data_def {
	/* Measured data */
	struct adis_reg x_gyro;
	struct adis_reg y_gyro;
	struct adis_reg z_gyro;
	struct adis_reg x_accl;
	struct adis_reg y_accl;
	struct adis_reg z_accl;
	struct adis_reg temp_out;

	/* Timing data */
	struct adis_reg time_stamp;
	struct adis_reg data_cntr;

	/* Delta measured data */
	struct adis_reg x_deltang;
	struct adis_reg y_deltang;
	struct adis_reg z_deltang;
	struct adis_reg x_deltvel;
	struct adis_reg y_deltvel;
	struct adis_reg z_deltvel;

	/* Calibration data */
	struct adis_reg xg_bias;
	struct adis_reg yg_bias;
	struct adis_reg zg_bias;
	struct adis_reg xa_bias;
	struct adis_reg ya_bias;
	struct adis_reg za_bias;

	/* Filter data */
	struct adis_reg filt_ctrl;
	struct adis_reg up_scale;
	struct adis_reg dec_rate;

	/* Identification data */
	struct adis_reg rang_mdl;
	struct adis_reg firm_rev;
	struct adis_reg firm_dm;
	struct adis_reg firm_y;
	struct adis_reg prod_id;
	struct adis_reg serial_num;
	struct adis_reg usr_scr_1;
	struct adis_reg usr_scr_2;
	struct adis_reg usr_scr_3;
	struct adis_reg flshcnt;

	/* Configuration data */
	struct adis_reg dr_polarity;
	struct adis_reg sync_polarity;
	struct adis_reg sync_mode;
	struct adis_reg sens_bw;
	struct adis_reg pt_of_perc_algnmt;
	struct adis_reg linear_accl_comp;
	struct adis_reg burst_sel;
	struct adis_reg burst_size;

	/* Commands */
	struct adis_reg sw_res;
	struct adis_reg fls_mem_test;
	struct adis_reg fls_mem_update;
	struct adis_reg snsr_self_test;
	struct adis_reg fact_calib_restore;

	/* Diagnosis data */
	struct adis_reg diag_stat;

	/* Diagnosis flags masks */
	uint16_t data_path_overrun_mask;
	uint16_t fls_mem_update_mask;
	uint16_t spi_comm_err_mask;
	uint16_t standby_mode_mask;
	uint16_t snsr_self_test_mask;
	uint16_t fls_mem_test_mask;
	uint16_t clock_err_mask;
	uint16_t gyro1_self_test_mask;
	uint16_t gyro2_self_test_mask;
	uint16_t accl_self_test_mask;
};

/**
 * @brief ADIS data definition structure
 */
struct adis_data {
	bool 					has_paging;

	/* Timings */
	unsigned int 				read_delay;
	unsigned int 				write_delay;
	unsigned int 				cs_change_delay;
	const struct adis_timeout 		*timeouts;

	/* Registers */
	const struct adis_data_def 		*reg_map;

	/* Maximum values */
	uint16_t 				filt_ctrl_max;
	uint16_t 				dec_rate_max;
	uint8_t 				burst_size_max;
	uint8_t 				burst_sel_max;
	uint8_t					sens_bw_max;
	uint8_t 				sync_mode_max;
	uint16_t				flshcnt_max;
	uint8_t 				pt_of_perc_algnmt_max;
	uint8_t 				linear_accl_comp_max;
	uint32_t 				int_clk;

	unsigned int 				prod_id;
	const struct adis_init_param 		*adis_ip;
	const struct adis_sync_rate_limit	*sync_rate_limit;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initialize adis device. */
int adis_init(struct adis_dev **adis, const struct adis_data *data);
/*! Remove adis device. */
void adis_remove(struct adis_dev *adis);
/*! Device initial setup. */
int adis_initial_startup(struct adis_dev *adis);
/*! Read N bytes from register. */
int adis_read_reg(struct adis_dev *adis,  unsigned int reg,
		  unsigned int *val, unsigned int size);
/*! Write N bytes to register. */
int adis_write_reg(struct adis_dev *adis, unsigned int reg,
		   unsigned int value, unsigned int size);
/*! Update the desired bits of reg in accordance with mask and val. */
int adis_update_bits_base(struct adis_dev *adis, unsigned int reg,
			  const unsigned int mask,
			  const unsigned int val, uint8_t size);

/*! Read burst data */
int adis_read_burst_data(struct adis_dev *adis,
			 struct adis_burst_data *burst_data);

/*! Read diag status register and update device diag flags. */
int adis_read_diag_stat(struct adis_dev *adis,
			union adis_diag_flags *diag_flags);
/*! Diagnosis: read checksum error flag value. */
void adis_read_diag_checksum_err(struct adis_dev *adis,
				 unsigned int *checksum_err);
/*! Diagnosis: read flash memory write counts exceeded flag value. */
void adis_read_diag_fls_mem_wr_cnt_exceed(struct adis_dev *adis,
		unsigned int *fls_mem_wr_cnt_exceed);
/*! Diagnosis: read accelerometer self test error flag value. */
int adis_read_diag_accl_self_test_err(struct adis_dev *adis,
				      unsigned int *accl_self_test_err);
/*! Diagnosis: read gyroscope2 self test error flag value. */
int adis_read_diag_gyro2_self_test_err(struct adis_dev *adis,
				       unsigned int *gyro2_self_test_err);
/*! Diagnosis: read gyroscope1 self test error flag value. */
int adis_read_diag_gyro1_self_test_err(struct adis_dev *adis,
				       unsigned int *gyro1_self_test_err);
/*! Diagnosis: read clock error flag value. */
int adis_read_diag_clock_err(struct adis_dev *adis, unsigned int *clock_err);
/*! Diagnosis: read flash memory test error flag value. */
int adis_read_diag_fls_mem_test_err(struct adis_dev *adis,
				    unsigned int *fls_mem_test_err);
/*! Diagnosis: read sensor self test error flag value. */
int adis_read_diag_snsr_self_test_err(struct adis_dev *adis,
				      unsigned int *snsr_self_test_err);
/*! Diagnosis: read standby mode flag value. */
int adis_read_diag_standby_mode(struct adis_dev *adis,
				unsigned int *standby_mode);
/*! Diagnosis: read spi communication error flag value. */
int adis_read_diag_spi_comm_err(struct adis_dev *adis,
				unsigned int *spi_comm_err);
/*! Diagnosis: read flash memory update error flag value. */
int adis_read_diag_fls_mem_update_err(struct adis_dev *adis,
				      unsigned int *fls_mem_update_err);
/*! Diagnosis: read data path overrun flag value. */
int adis_read_diag_data_path_overrun(struct adis_dev *adis,
				     unsigned int *data_path_overrun_err);

/*! Gyroscope data: read raw gyroscope data on x axis. */
int adis_read_x_gyro(struct adis_dev *adis, int *x_gyro);
/*! Gyroscope data: read raw gyroscope data on y axis. */
int adis_read_y_gyro(struct adis_dev *adis, int *y_gyro);
/*! Gyroscope data: read raw gyroscope data on z axis. */
int adis_read_z_gyro(struct adis_dev *adis, int *z_gyro);

/*! Acceleration data: read raw acceleration data on x axis. */
int adis_read_x_accl(struct adis_dev *adis, int *x_accl);
/*! Acceleration data: read raw acceleration data on y axis. */
int adis_read_y_accl(struct adis_dev *adis, int *y_accl);
/*! Acceleration data: read raw acceleration data on z axis. */
int adis_read_z_accl(struct adis_dev *adis, int *z_accl);

/*! Temperature data: read raw temperature data. */
int adis_read_temp_out(struct adis_dev *adis, int *temp_out);

/*! Synchronization data: read raw time stamp data. */
int adis_read_time_stamp(struct adis_dev *adis, unsigned int *time_stamp);
/*! Synchronization data: read raw data counter data. */
int adis_read_data_cntr(struct adis_dev *adis, unsigned int *data_cntr);

/*! Delta angle data: read raw delta angle data on x axis. */
int adis_read_x_deltang(struct adis_dev *adis, int *x_deltang);
/*! Delta angle data: read raw delta angle data on y axis. */
int adis_read_y_deltang(struct adis_dev *adis, int *y_deltang);
/*! Delta angle data: read raw delta angle data on z axis. */
int adis_read_z_deltang(struct adis_dev *adis, int *z_deltang);

/*! Delta velocity data: read raw delta velocity data on x axis. */
int adis_read_x_deltvel(struct adis_dev *adis, int *x_deltvel);
/*! Delta velocity data: read raw delta velocity data on y axis. */
int adis_read_y_deltvel(struct adis_dev *adis, int *y_deltvel);
/*! Delta velocity data: read raw delta velocity data on z axis. */
int adis_read_z_deltvel(struct adis_dev *adis, int *z_deltvel);

/*! Gyroscope calibration offset: read raw gyroscope offset correction on x axis. */
int adis_read_xg_bias(struct adis_dev *adis, int *xg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on x axis. */
int adis_write_xg_bias(struct adis_dev *adis, int xg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on y axis. */
int adis_read_yg_bias(struct adis_dev *adis, int *yg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on y axis. */
int adis_write_yg_bias(struct adis_dev *adis, int yg_bias);
/*! Gyroscope calibration offset: read raw gyroscope offset correction on z axis. */
int adis_read_zg_bias(struct adis_dev *adis, int *zg_bias);
/*! Gyroscope calibration offset: write raw gyroscope offset correction on z axis. */
int adis_write_zg_bias(struct adis_dev *adis, int zg_bias);

/*! Acceleration calibration offset: read raw acceleration offset correction on x axis. */
int adis_read_xa_bias(struct adis_dev *adis, int *xa_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on x axis. */
int adis_write_xa_bias(struct adis_dev *adis, int xa_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on y axis. */
int adis_read_ya_bias(struct adis_dev *adis, int *ya_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on y axis. */
int adis_write_ya_bias(struct adis_dev *adis, int ya_bias);
/*! Acceleration calibration offset: read raw acceleration offset correction on z axis. */
int adis_read_za_bias(struct adis_dev *adis, int *za_bias);
/*! Acceleration calibration offset: write raw acceleration offset correction on z axis. */
int adis_write_za_bias(struct adis_dev *adis, int za_bias);

/*! Filter control: read filter size variable B value. */
int adis_read_filt_ctrl(struct adis_dev *adis, unsigned int *filt_ctrl);
/*! Filter control: write filter size variable B value. */
int adis_write_filt_ctrl(struct adis_dev *adis, unsigned int filt_ctrl);

/*! Range identifier: read gyroscope measurement range value. */
int adis_read_rang_mdl(struct adis_dev *adis, unsigned int *rang_mdl);

/*! Miscellaneous control: read burst size encoded value. */
int adis_read_burst_size(struct adis_dev *adis, unsigned int *burst_size);
/*! Miscellaneous control: write burst size encoded value. */
int adis_write_burst_size(struct adis_dev *adis, unsigned int burst_size);
/*! Miscellaneous control: read burst selection encoded value. */
int adis_read_burst_sel(struct adis_dev *adis, unsigned int *burst_sel);
/*! Miscellaneous control: write burst selection encoded value. */
int adis_write_burst_sel(struct adis_dev *adis, unsigned int burst_sel);
/*! Miscellaneous control: read linear acceleration compensation encoded value. */
int adis_read_linear_accl_comp(struct adis_dev *adis,
			       unsigned int *linear_accl_comp);
/*! Miscellaneous control: write linear acceleration compensation encoded value. */
int adis_write_linear_accl_comp(struct adis_dev *adis,
				unsigned int linear_accl_comp);
/*! Miscellaneous control: read point of percussion alignment encoded value. */
int adis_read_pt_of_perc_algnmt(struct adis_dev *adis,
				unsigned int *pt_of_perc_algnmt);
/*! Miscellaneous control: write point of percussion alignment encoded value. */
int adis_write_pt_of_perc_algnmt(struct adis_dev *adis,
				 unsigned int pt_of_perc_algnmt);
int adis_read_sens_bw(struct adis_dev *adis, unsigned int *sens_bw);
int adis_write_sens_bw(struct adis_dev *adis, unsigned int sens_bw);
int adis_read_sync_mode(struct adis_dev *adis, unsigned int *sync_mode);
int adis_write_sync_mode(struct adis_dev *adis, unsigned int sync_mode);
int adis_read_sync_polarity(struct adis_dev *adis,
			    unsigned int *sync_polarity);
int adis_write_sync_polarity(struct adis_dev *adis,
			     unsigned int sync_polarity);
int adis_read_dr_polarity(struct adis_dev *adis,
			  unsigned int *dr_polarity);
int adis_write_dr_polarity(struct adis_dev *adis,
			   unsigned int dr_polarity);

/* Sync input frequency multiplier read/write */
int adis_read_up_scale(struct adis_dev *adis, unsigned int *up_scale);
int adis_write_up_scale(struct adis_dev *adis, unsigned int up_scale);
/* Decimation rate read/write */
int adis_read_dec_rate(struct adis_dev *adis, unsigned int *dec_rate);
int adis_write_dec_rate(struct adis_dev *adis, unsigned int dec_rate);

/* Commands */
int adis_cmd_sw_res(struct adis_dev *adis);
int adis_cmd_fls_mem_test(struct adis_dev *adis);
int adis_cmd_fls_mem_update(struct adis_dev *adis);
int adis_cmd_snsr_self_test(struct adis_dev *adis);
int adis_cmd_fact_calib_restore(struct adis_dev *adis);

/* Device identification data */
int adis_read_firm_rev(struct adis_dev *adis, unsigned int *firm_rev);
int adis_read_firm_dm(struct adis_dev *adis, unsigned int *firm_dm);
int adis_read_firm_y(struct adis_dev *adis, unsigned int *firm_y);
int adis_read_prod_id(struct adis_dev *adis, unsigned int *prod_id);
int adis_read_serial_num(struct adis_dev *adis, unsigned int *serial_num);

/* Scratch pad registers read/write */
int adis_read_usr_scr_1(struct adis_dev *adis, unsigned int *usr_scr_1);
int adis_write_usr_scr_1(struct adis_dev *adis, unsigned int usr_scr_1);
int adis_read_usr_scr_2(struct adis_dev *adis, unsigned int *usr_scr_2);
int adis_write_usr_scr_2(struct adis_dev *adis, unsigned int usr_scr_2);
int adis_read_usr_scr_3(struct adis_dev *adis, unsigned int *usr_scr_3);
int adis_write_usr_scr_3(struct adis_dev *adis, unsigned int usr_scr_3);

/* Flash update counter read */
int adis_read_flshcnt(struct adis_dev *adis, unsigned int *flshcnt);

#endif
