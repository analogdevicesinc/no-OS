/***************************************************************************//**
 *   @file   adis1657x.c
 *   @brief  Implementation of adis1657x.c
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "adis.h"
#include "adis1657x.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const struct adis_data_field_map_def adis1657x_def = {
	.x_gyro 		 = {.reg_addr = 0x04, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_gyro 		 = {.reg_addr = 0x08, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_gyro 		 = {.reg_addr = 0x0C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_accl 		 = {.reg_addr = 0x10, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_accl 		 = {.reg_addr = 0x14, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_accl 		 = {.reg_addr = 0x18, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.temp_out 		 = {.reg_addr = 0x1C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.time_stamp 		 = {.reg_addr = 0x1E, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.data_cntr 		 = {.reg_addr = 0x22, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.x_deltang 		 = {.reg_addr = 0x24, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltang 		 = {.reg_addr = 0x28, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltang 		 = {.reg_addr = 0x2C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_deltvel 		 = {.reg_addr = 0x30, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltvel 		 = {.reg_addr = 0x34, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltvel 		 = {.reg_addr = 0x38, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.fifo_cnt		 = {.reg_addr = 0x3C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.spi_chksum		 = {.reg_addr = 0x3E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.xg_bias 		 = {.reg_addr = 0x40, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.yg_bias 		 = {.reg_addr = 0x44, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.zg_bias 		 = {.reg_addr = 0x48, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xa_bias 		 = {.reg_addr = 0x4C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.ya_bias 		 = {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.za_bias 		 = {.reg_addr = 0x54, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.fifo_en		 = {.reg_addr = 0x5A, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.fifo_overflow		 = {.reg_addr = 0x5A, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.fifo_wm_int_en		 = {.reg_addr = 0x5A, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.fifo_wm_int_pol	 = {.reg_addr = 0x5A, .reg_size = 0x02, .field_mask = NO_OS_BIT(3)},
	.fifo_wm_lvl		 = {.reg_addr = 0x5A, .reg_size = 0x02, .field_mask = 0x0000FFF0},
	.filt_size_var_b 	 = {.reg_addr = 0x5C, .reg_size = 0x02, .field_mask = 0x00000007},
	.up_scale 		 = {.reg_addr = 0x62, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.dec_rate 		 = {.reg_addr = 0x64, .reg_size = 0x02, .field_mask = 0x000007FF},
	.gyro_meas_range 	 = {.reg_addr = 0x5E, .reg_size = 0x02, .field_mask = 0x0000000C},
	.firm_rev 		 = {.reg_addr = 0x6C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.firm_d 		 = {.reg_addr = 0x6E, .reg_size = 0x02, .field_mask = 0x000000FF},
	.firm_m 		 = {.reg_addr = 0x6E, .reg_size = 0x02, .field_mask = 0x0000FF00},
	.firm_y 		 = {.reg_addr = 0x70, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.prod_id 		 = {.reg_addr = 0x72, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.serial_num 		 = {.reg_addr = 0x74, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_1 		 = {.reg_addr = 0x76, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_2 		 = {.reg_addr = 0x78, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_3 		 = {.reg_addr = 0x7A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.fls_mem_wr_cntr	 = {.reg_addr = 0x7C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.dr_polarity 		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.sync_polarity 		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.sync_mode 		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = 0x0000000C},
	.pt_of_perc_algnmt 	 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(6)},
	.linear_accl_comp 	 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(7)},
	.burst_sel 		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.burst32 		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(9)},
	.timestamp32	         = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(10)},
	.sync_4khz		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.sens_bw		 = {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(12)},
	.bias_corr_tbc		 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = 0x0000000F},
	.bias_corr_en_xg	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.bias_corr_en_yg	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(9)},
	.bias_corr_en_zg	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(10)},
	.bias_corr_en_xa	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.bias_corr_en_ya	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(12)},
	.bias_corr_en_za	 = {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(13)},
	.bias_corr_update        = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.fact_calib_restore 	 = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.snsr_self_test 	 = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.fls_mem_update 	 = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(3)},
	.fls_mem_test 		 = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(4)},
	.fifo_flush              = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(5)},
	.sw_res 		 = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(7)},
	.diag_stat 		 = {.reg_addr = 0x02, .reg_size = 0x02, .field_mask = 0x0000BFFF},
	.diag_snsr_init_failure_mask  		= NO_OS_BIT(0),
	.diag_data_path_overrun_mask  		= NO_OS_BIT(1),
	.diag_fls_mem_update_failure_mask 	= NO_OS_BIT(2),
	.diag_spi_comm_err_mask 	 	= NO_OS_BIT(3),
	.diag_standby_mode_mask 	 	= NO_OS_BIT(4),
	.diag_snsr_failure_mask 	 	= NO_OS_BIT(5),
	.diag_mem_failure_mask	 		= NO_OS_BIT(6),
	.diag_clk_err_mask 	 		= NO_OS_BIT(7),
	.diag_x_axis_gyro_failure_mask  	= NO_OS_BIT(8),
	.diag_y_axis_gyro_failure_mask  	= NO_OS_BIT(9),
	.diag_z_axis_gyro_failure_mask  	= NO_OS_BIT(10),
	.diag_x_axis_accl_failure_mask  	= NO_OS_BIT(11),
	.diag_y_axis_accl_failure_mask  	= NO_OS_BIT(12),
	.diag_z_axis_accl_failure_mask  	= NO_OS_BIT(13),
	.diag_aduc_mcu_fault_mask 		= NO_OS_BIT(15),
};

static const struct adis_timeout adis1657x_timeouts = {
	.reset_ms 			= 350,
	.fact_calib_restore_ms		= 150,
	.self_test_ms 			= 30,
	.fls_mem_update_ms		= 70,
	.fls_mem_test_ms		= 30,
	.sw_reset_ms 			= 350,
	.filt_size_var_b_update_us 	= 30,
	.dec_rate_update_us		= 30,
	.msc_reg_update_us 		= 200,
	.sens_bw_update_ms 		= 250,
};

static const struct adis_clk_freq_limit adis1657x_sync_clk_freq_limits[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 1900, 4100 },
	[ADIS_SYNC_SCALED] = { 1, 400 },
	[ADIS_SYNC_OUTPUT] = { },
};

static const struct adis_clk_freq_limit adis1657x_sampling_clk_limits = {
	.min_freq = 1900,
	.max_freq = 4100,
};

struct adis_chip_info adis1657x_chip_info = {
	.field_map		= &adis1657x_def,
	.sync_clk_freq_limits	= adis1657x_sync_clk_freq_limits,
	.sampling_clk_limits	= adis1657x_sampling_clk_limits,
	.timeouts 		= &adis1657x_timeouts,
	.read_delay 		= 5,
	.write_delay 		= 0,
	.cs_change_delay 	= 5,
	.burst_request		= true,
	.filt_size_var_b_max 	= 6,
	.dec_rate_max 		= 1999,
	.sync_mode_max 		= ADIS_SYNC_OUTPUT,
	.fls_mem_wr_cntr_max 	= 100000,
	.int_clk		= 2000,
	.bias_corr_tbc_max	= 12,
};
