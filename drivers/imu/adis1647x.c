/***************************************************************************//**
 *   @file   adis1647x.c
 *   @brief  Implementation of adis1647x.c
 *   @author RBolboac (ramona.gradinariu@analog.com)
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
#include "adis_internals.h"
#include "adis1647x.h"
#include "no_os_units.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const struct adis_data_field_map_def adis1647x_def = {
	.x_gyro 		= {.reg_addr = 0x04, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_gyro 		= {.reg_addr = 0x08, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_gyro 		= {.reg_addr = 0x0C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_accl 		= {.reg_addr = 0x10, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_accl 		= {.reg_addr = 0x14, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_accl 		= {.reg_addr = 0x18, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.temp_out 		= {.reg_addr = 0x1C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.time_stamp 		= {.reg_addr = 0x1E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.data_cntr 		= {.reg_addr = 0x22, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.x_deltang 		= {.reg_addr = 0x24, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltang 		= {.reg_addr = 0x28, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltang 		= {.reg_addr = 0x2C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_deltvel 		= {.reg_addr = 0x30, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltvel 		= {.reg_addr = 0x34, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltvel 		= {.reg_addr = 0x38, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xg_bias 		= {.reg_addr = 0x40, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.yg_bias 		= {.reg_addr = 0x44, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.zg_bias 		= {.reg_addr = 0x48, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xa_bias 		= {.reg_addr = 0x4C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.ya_bias 		= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.za_bias 		= {.reg_addr = 0x54, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.filt_size_var_b 	= {.reg_addr = 0x5C, .reg_size = 0x02, .field_mask = 0x00000007},
	.up_scale 		= {.reg_addr = 0x62, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.dec_rate 		= {.reg_addr = 0x64, .reg_size = 0x02, .field_mask = 0x000007FF},
	.gyro_meas_range 	= {.reg_addr = 0x5E, .reg_size = 0x02, .field_mask = 0x0000000C},
	.firm_rev 		= {.reg_addr = 0x6C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.firm_d 		= {.reg_addr = 0x6E, .reg_size = 0x02, .field_mask = 0x000000FF},
	.firm_m 		= {.reg_addr = 0x6E, .reg_size = 0x02, .field_mask = 0x0000FF00},
	.firm_y 		= {.reg_addr = 0x70, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.prod_id 		= {.reg_addr = 0x72, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.serial_num 		= {.reg_addr = 0x74, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_1 		= {.reg_addr = 0x76, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_2 		= {.reg_addr = 0x78, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_3 		= {.reg_addr = 0x7A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.fls_mem_wr_cntr	= {.reg_addr = 0x7C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.dr_polarity 		= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.sync_polarity 		= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.sync_mode 		= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = 0x0000001C},
	.pt_of_perc_algnmt 	= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(6)},
	.linear_accl_comp 	= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(7)},
	.burst_sel 		= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.burst32 		= {.reg_addr = 0x60, .reg_size = 0x02, .field_mask = NO_OS_BIT(9)},
	.bias_corr_tbc		= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = 0x0000000F},
	.bias_corr_en_xg	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.bias_corr_en_yg	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(9)},
	.bias_corr_en_zg	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(10)},
	.bias_corr_en_xa	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.bias_corr_en_ya	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(12)},
	.bias_corr_en_za	= {.reg_addr = 0x66, .reg_size = 0x02, .field_mask = NO_OS_BIT(13)},
	.bias_corr_update       = {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.fact_calib_restore 	= {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.snsr_self_test 	= {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.fls_mem_update 	= {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(3)},
	.fls_mem_test 		= {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(4)},
	.sw_res 		= {.reg_addr = 0x68, .reg_size = 0x02, .field_mask = NO_OS_BIT(7)},
	.diag_stat 		= {.reg_addr = 0x02, .reg_size = 0x02, .field_mask = 0x000000FE},
	.diag_data_path_overrun_mask 		= NO_OS_BIT(1),
	.diag_fls_mem_update_failure_mask 	= NO_OS_BIT(2),
	.diag_spi_comm_err_mask 		= NO_OS_BIT(3),
	.diag_standby_mode_mask 		= NO_OS_BIT(4),
	.diag_snsr_failure_mask 		= NO_OS_BIT(5),
	.diag_mem_failure_mask			= NO_OS_BIT(6),
	.diag_clk_err_mask 			= NO_OS_BIT(7),
};

static const struct adis_timeout adis1647x_timeouts = {
	.reset_ms 			= 310,
	.fact_calib_restore_ms		= 136,
	.self_test_ms 			= 24,
	.fls_mem_update_ms		= 70,
	.fls_mem_test_ms		= 30,
	.sw_reset_ms 			= 255,
	.filt_size_var_b_update_us 	= 30,
	.dec_rate_update_us		= 30,
	.msc_reg_update_us 		= 200,
};

static const struct adis_clk_freq_limit adis1647x_sync_clk_freq_limits[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 1900, 2100 },
	[ADIS_SYNC_SCALED] = { 1, 128 },
	[ADIS_SYNC_OUTPUT] = { },
	[ADIS_SYNC_PULSE] = { 1000, 2100 },
};

static const struct adis_clk_freq_limit adis1647x_sampling_clk_limits = {
	.min_freq = 1900,
	.max_freq = 2100,
};

/* Values from datasheet for 32-bit data */
static const struct adis_scale_members adis1647x_anglvel_scale[] = {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = {1, RAD_TO_DEGREE(160 << 16)},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = {1, RAD_TO_DEGREE(40 << 16)},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = {1, RAD_TO_DEGREE(10 << 16)},
};

static const struct adis_scale_members adis1647x_accl_scale[] = {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = {1, M_S_2_TO_G(800 << 16)},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = {1, M_S_2_TO_G(800 << 16)},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = {1, M_S_2_TO_G(800 << 16)},
};

static const struct adis_scale_members adis1647x_deltaangl_scale[] = {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = {DEGREE_TO_RAD(360), 31},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = {DEGREE_TO_RAD(720), 31},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = {DEGREE_TO_RAD(2160), 31},
};

static const struct adis_scale_members adis1647x_deltavelocity_scale[]
	= {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = {400, 31},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = {400, 31},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = {400, 31},
};

/* Milli-degrees Celsius for temperature */
static const struct adis_scale_members adis1647x_temp_scale[] = {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = {1 * MILLIDEGREE_PER_DEGREE, 10},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = {1 * MILLIDEGREE_PER_DEGREE, 10},
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = {1 * MILLIDEGREE_PER_DEGREE, 10},
};

static int adis1647x_get_scale(struct adis_dev *adis,
			       uint32_t *scale_m1, uint32_t *scale_m2,
			       enum adis_chan_type chan_type)
{
	switch(chan_type) {
	case ADIS_ACCL_CHAN:
		*scale_m1 = adis1647x_accl_scale[ADIS1647X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1647x_accl_scale[ADIS1647X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_GYRO_CHAN:
		*scale_m1 = adis1647x_anglvel_scale[ADIS1647X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1647x_anglvel_scale[ADIS1647X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_TEMP_CHAN:
		*scale_m1 = adis1647x_temp_scale[ADIS1647X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1647x_temp_scale[ADIS1647X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAANGL_CHAN:
		*scale_m1 = adis1647x_deltaangl_scale[ADIS1647X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1647x_deltaangl_scale[ADIS1647X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAVEL_CHAN:
		*scale_m1 = adis1647x_deltavelocity_scale[ADIS1647X_ID_NO_OFFSET(
					adis->dev_id)].scale_m1;
		*scale_m2 = adis1647x_deltavelocity_scale[ADIS1647X_ID_NO_OFFSET(
					adis->dev_id)].scale_m2;
		return 0;
	default:
		return -EINVAL;
	}
}

const struct adis_chip_info adis1647x_chip_info = {
	.cs_change_delay 	= 16,
	.read_delay 		= 8,
	.write_delay 		= 0,
	.timeouts 		= &adis1647x_timeouts,
	.field_map 		= &adis1647x_def,
	.filt_size_var_b_max 	= 6,
	.dec_rate_max 		= 1999,
	.sync_mode_max 		= ADIS_SYNC_PULSE,
	.fls_mem_wr_cntr_max 	= 10000,
	.int_clk		= 2000,
	.sync_clk_freq_limits	= adis1647x_sync_clk_freq_limits,
	.sampling_clk_limits	= adis1647x_sampling_clk_limits,
	.flags			= ADIS_HAS_BURST32 | ADIS_HAS_BURST_DELTA_DATA,
	.get_scale		= &adis1647x_get_scale,
};
