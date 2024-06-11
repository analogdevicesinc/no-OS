/***************************************************************************//**
 *   @file   adis1654x.c
 *   @brief  Implementation of adis1654x.c
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "adis.h"
#include "adis_internals.h"
#include "adis1654x.h"
#include "no_os_units.h"
#include <string.h>
#include "no_os_util.h"
#include "no_os_delay.h"
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADIS1654X_CRC32_SEED		0xFFFFFFFF

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const struct adis_data_field_map_def adis1654x_def = {
	/* Page 0 */
	.data_cntr 		= {.reg_addr = 0x04, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.diag_stat 		= {.reg_addr = 0x08, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.diag_boot_memory_failure_mask		= NO_OS_BIT(1),
	.diag_mem_failure_mask			= NO_OS_BIT(2),
	.diag_spi_comm_err_mask			= NO_OS_BIT(3),
	.diag_snsr_failure_mask			= NO_OS_BIT(5),
	.diag_fls_mem_update_failure_mask	= NO_OS_BIT(6),
	.diag_data_path_overrun_mask		= NO_OS_BIT(7),
	.diag_clk_err_mask			= NO_OS_BIT(8),
	.diag_wdg_timer_flag_mask		= NO_OS_BIT(15),
	.diag_x_axis_gyro_failure_mask  	= NO_OS_BIT(16),
	.diag_y_axis_gyro_failure_mask  	= NO_OS_BIT(17),
	.diag_z_axis_gyro_failure_mask  	= NO_OS_BIT(18),
	.diag_x_axis_accl_failure_mask  	= NO_OS_BIT(19),
	.diag_y_axis_accl_failure_mask  	= NO_OS_BIT(20),
	.diag_z_axis_accl_failure_mask  	= NO_OS_BIT(21),
	.temp_out 		= {.reg_addr = 0x0E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.x_gyro 		= {.reg_addr = 0x10, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_gyro 		= {.reg_addr = 0x14, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_gyro 		= {.reg_addr = 0x18, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_accl 		= {.reg_addr = 0x1C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_accl 		= {.reg_addr = 0x20, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_accl 		= {.reg_addr = 0x24, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.time_stamp 		= {.reg_addr = 0x28, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.x_deltang 		= {.reg_addr = 0x40, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltang 		= {.reg_addr = 0x44, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltang 		= {.reg_addr = 0x48, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.x_deltvel 		= {.reg_addr = 0x4C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.y_deltvel 		= {.reg_addr = 0x50, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.z_deltvel 		= {.reg_addr = 0x54, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.prod_id 		= {.reg_addr = 0x7E, .reg_size = 0x02, .field_mask = 0x0000FFFF},

	/* Page 2 */
	.xg_scale		= {.reg_addr = 0x104, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.yg_scale		= {.reg_addr = 0x106, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.zg_scale		= {.reg_addr = 0x108, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.xa_scale		= {.reg_addr = 0x10A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.ya_scale		= {.reg_addr = 0x10C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.za_scale		= {.reg_addr = 0x10E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.xg_bias 		= {.reg_addr = 0x110, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.yg_bias 		= {.reg_addr = 0x114, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.zg_bias 		= {.reg_addr = 0x118, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.xa_bias 		= {.reg_addr = 0x11C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.ya_bias 		= {.reg_addr = 0x120, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.za_bias 		= {.reg_addr = 0x124, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.usr_scr_1 		= {.reg_addr = 0x174, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_2 		= {.reg_addr = 0x176, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_3 		= {.reg_addr = 0x178, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.usr_scr_4 		= {.reg_addr = 0x17A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.fls_mem_wr_cntr	= {.reg_addr = 0x17C, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},

	/* Page 3 */
	.sw_res 		= {.reg_addr = 0x182, .reg_size = 0x02, .field_mask = NO_OS_BIT(7)},
	.fact_calib_restore 	= {.reg_addr = 0x182, .reg_size = 0x02, .field_mask = NO_OS_BIT(6)},
	.fls_mem_update 	= {.reg_addr = 0x182, .reg_size = 0x02, .field_mask = NO_OS_BIT(3)},
	.snsr_self_test 	= {.reg_addr = 0x182, .reg_size = 0x02, .field_mask = NO_OS_BIT(1)},
	.bias_corr_update	= {.reg_addr = 0x182, .reg_size = 0x02, .field_mask = NO_OS_BIT(0)},
	.dr_selection		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = 0x00000003},
	.dr_polarity		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.dr_enable		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = NO_OS_BIT(3)},
	.sync_selection		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = 0x00000030},
	.sync_polarity		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = NO_OS_BIT(6)},
	.sync_mode		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = 0x000000C0},
	.alarm_selection	= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = 0x00000300},
	.alarm_polarity		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.alarm_enable		= {.reg_addr = 0x186, .reg_size = 0x02, .field_mask = NO_OS_BIT(12)},
	.gpio_ctrl		= {.reg_addr = 0x188, .reg_size = 0x02, .field_mask = 0x000000FF},
	.pt_of_perc_algnmt 	= {.reg_addr = 0x18A, .reg_size = 0x02, .field_mask = NO_OS_BIT(6)},
	.burst_sel 		= {.reg_addr = 0x18A, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.dec_rate 		= {.reg_addr = 0x18C, .reg_size = 0x02, .field_mask = 0x00000FFF},
	.bias_corr_tbc		= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = 0x0000000F},
	.bias_corr_en_xg	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.bias_corr_en_yg	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(9)},
	.bias_corr_en_zg	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(10)},
	.bias_corr_en_xa	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.bias_corr_en_ya	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(12)},
	.bias_corr_en_za	= {.reg_addr = 0x18E, .reg_size = 0x02, .field_mask = NO_OS_BIT(13)},
	.up_scale 		= {.reg_addr = 0x190, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.gyro_meas_range 	= {.reg_addr = 0x192, .reg_size = 0x02, .field_mask = 0x0000000C},
	.fir_en_xg		= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.fir_en_yg		= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = NO_OS_BIT(5)},
	.fir_en_zg		= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = NO_OS_BIT(8)},
	.fir_en_xa		= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = NO_OS_BIT(11)},
	.fir_en_ya		= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = NO_OS_BIT(14)},
	.fir_en_za		= {.reg_addr = 0x198, .reg_size = 0x02, .field_mask = NO_OS_BIT(2)},
	.fir_bank_sel_xg	= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = 0x00000003},
	.fir_bank_sel_yg	= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = 0x00000018},
	.fir_bank_sel_zg	= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = 0x000000C0},
	.fir_bank_sel_xa	= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = 0x00000600},
	.fir_bank_sel_ya	= {.reg_addr = 0x196, .reg_size = 0x02, .field_mask = 0x00000018},
	.fir_bank_sel_za	= {.reg_addr = 0x198, .reg_size = 0x02, .field_mask = 0x00003000},
	.firm_rev 		= {.reg_addr = 0x1F8, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.firm_d 		= {.reg_addr = 0x1FA, .reg_size = 0x02, .field_mask = 0x000000FF},
	.firm_m 		= {.reg_addr = 0x1FA, .reg_size = 0x02, .field_mask = 0x0000FF00},
	.firm_y 		= {.reg_addr = 0x1FC, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.boot_rev		= {.reg_addr = 0x1FE, .reg_size = 0x02, .field_mask = 0x0000FFFF},

	/* Page 4 */
	.serial_num 		= {.reg_addr = 0x220, .reg_size = 0x02, .field_mask = 0x0000FFFF},

	/* Page 5 */
	.coeff_bank_a		= {.reg_addr = 0x288, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	/* Page 7 */
	.coeff_bank_b		= {.reg_addr = 0x388, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	/* Page 9 */
	.coeff_bank_c		= {.reg_addr = 0x488, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	/* Page B */
	.coeff_bank_d		= {.reg_addr = 0x588, .reg_size = 0x02, .field_mask = 0x0000FFFF},
};

static const struct adis_timeout adis1654x_timeouts = {
	.reset_ms 			= 290,
	.fact_calib_restore_ms		= 1,
	.self_test_ms 			= 35,
	.fls_mem_update_ms		= 625,
	.sw_reset_ms 			= 250,
	.msc_reg_update_us 		= 20000,
};

static const struct adis_clk_freq_limit adis1654x_sync_clk_freq_limits[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 3000, 4500 },
	[ADIS_SYNC_SCALED] = { 1, 128 },
};

static const struct adis_clk_freq_limit adis1654x_sampling_clk_limits = {
	.min_freq = 3000,
	.max_freq = 4500,
};

/* Values from datasheet for 32-bit data TBD */
static const struct adis_scale_members adis1654x_anglvel_scale[] = {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = {DEGREE_TO_RAD(125), 20000 << 16},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = {DEGREE_TO_RAD(450), 20000 << 16},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = {DEGREE_TO_RAD(2000), 20000 << 16},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = {DEGREE_TO_RAD(125), 20000 << 16},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = {DEGREE_TO_RAD(450), 20000 << 16},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = {DEGREE_TO_RAD(2000), 20000 << 16},
};

static const struct adis_scale_members adis1654x_accl_scale[] = {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = {8, M_S_2_TO_G(32000 << 16)},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = {8, M_S_2_TO_G(32000 << 16)},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = {8, M_S_2_TO_G(32000 << 16)},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = {40, M_S_2_TO_G(32000 << 16)},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = {40, M_S_2_TO_G(32000 << 16)},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = {40, M_S_2_TO_G(32000 << 16)},
};

static const struct adis_scale_members adis1654x_deltaangl_scale[] = {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = {DEGREE_TO_RAD(360), 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = {DEGREE_TO_RAD(720), 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = {DEGREE_TO_RAD(2160), 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = {DEGREE_TO_RAD(360), 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = {DEGREE_TO_RAD(720), 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = {DEGREE_TO_RAD(2160), 31},
};

static const struct adis_scale_members adis1654x_deltavelocity_scale[]
	= {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = {100, 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = {100, 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = {100, 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = {400, 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = {400, 31},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = {400, 31},
};

/* Milli-degrees Celsius for temperature */
static const struct adis_scale_members adis1654x_temp_scale[] = {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = {1 * MILLIDEGREE_PER_DEGREE, 140},
};

/* Milli-degrees Celsius for temperature */
static const int adis1654x_temp_offset[] = {
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_1)] = 3500,
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_2)] = 3500,
	[ADIS1654X_ID_NO_OFFSET(ADIS16545_3)] = 3500,
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_1)] = 3500,
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_2)] = 3500,
	[ADIS1654X_ID_NO_OFFSET(ADIS16547_3)] = 3500,
};

static const uint32_t adis1654x_filt_freqs[] = {
	300,
	100,
	300,
	100,
};

static int adis1654x_get_scale(struct adis_dev *adis,
			       uint32_t *scale_m1, uint32_t *scale_m2,
			       enum adis_chan_type chan_type)
{
	switch(chan_type) {
	case ADIS_ACCL_CHAN:
		*scale_m1 = adis1654x_accl_scale[ADIS1654X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1654x_accl_scale[ADIS1654X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_GYRO_CHAN:
		*scale_m1 = adis1654x_anglvel_scale[ADIS1654X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1654x_anglvel_scale[ADIS1654X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_TEMP_CHAN:
		*scale_m1 = adis1654x_temp_scale[ADIS1654X_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis1654x_temp_scale[ADIS1654X_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAANGL_CHAN:
		*scale_m1 = adis1654x_deltaangl_scale[ADIS1654X_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis1654x_deltaangl_scale[ADIS1654X_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAVEL_CHAN:
		*scale_m1 = adis1654x_deltavelocity_scale[ADIS1654X_ID_NO_OFFSET(
					adis->dev_id)].scale_m1;
		*scale_m2 = adis1654x_deltavelocity_scale[ADIS1654X_ID_NO_OFFSET(
					adis->dev_id)].scale_m2;
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read adis device channel offset in integer.
 * @param adis      - The adis device.
 * @param offset    - The read channel offset.
 * @param chan_type - The channel type to read offset for.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1654x_get_offset(struct adis_dev *adis, int *offset,
				enum adis_chan_type chan_type)
{
	switch(chan_type) {
	case ADIS_TEMP_CHAN:
		*offset = adis1654x_temp_offset[ADIS1654X_ID_NO_OFFSET(adis->dev_id)];
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief CRC32 computation algorithm for adis1654x burst access.
 * @param len  - size of buff in bytes.
 * @param buff - data buffer to check.
 * @return computed crc.
 */
static uint32_t adis1654x_crc32_computation(uint32_t len, const uint8_t *buff)
{
	uint32_t crc = ADIS1654X_CRC32_SEED;
	unsigned int j;
	uint32_t poly = 0xEDB88320;

	while (len > 0) {
		crc ^= *buff;

		for (j = 8; j > 0; j--) {
			if (crc & 1)
				crc = crc >> 1 ^ poly;
			else
				crc = crc >> 1;
		}
		buff++;
		len--;
	}

	crc = crc ^ 0xFFFFFFFFU;

	return crc;
}

/**
 * @brief Read burst data.
 * @param adis      - The adis device.
 * @param data      - The burst read data structure to be populated.
 * @param burst32   - True if 32-bit data is requested for accel
 *		      and gyro (or delta angle and delta velocity)
 *		      measurements, false if 16-bit data is requested.
 * @param burst_sel - 0 if accel and gyro data is requested, 1
 *		      if delta angle and delta velocity is requested.
 * @param fifo_pop  - In case FIFO is present, will pop the fifo if
 * 		      true. Unused if FIFO is not present.
 * @param crc_check - If true CRC will be checked, if false check will be skipped.
 * @return 0 in case of success, error code otherwise.
 * -EAGAIN in case the request has to be sent again due to data being unavailable
 * at the time of the request.
 */
int adis1654x_read_burst_data(struct adis_dev *adis,
			      struct adis_burst_data *data,
			      bool burst32, uint8_t burst_sel, bool fifo_pop, bool crc_check)
{
	int ret;
	uint8_t burst_id = 0xA5;
	uint32_t computed_crc32, recv_crc32;
	uint8_t buffer[46], offset;

	if (!burst32)
		return -EINVAL;

	if (adis->info->flags & ADIS_HAS_BURST32) {
		if (adis->burst_sel != burst_sel) {
			ret = adis_write_burst_sel(adis, burst_sel);
			if (ret)
				return ret;
			/* If burst select has changed, wait for the next reading
			request to actually read the data, because the according data will be available
			only after the next data ready impulse. */
			return -EAGAIN;
		}
	}

	if (adis->current_page != 0) {
		adis->tx[0] = ADIS_WRITE_REG(ADIS_REG_PAGE_ID);
		adis->tx[1] = 0;
		ret = no_os_spi_write_and_read(adis->spi_desc, adis->tx, 2);
		if (ret)
			return ret;
		adis->current_page = 0;
	}

	if (burst_sel)
		burst_id = 0xC3;

	buffer[0] = 0x7C;
	buffer[1] = 0x00;

	ret = no_os_spi_write_and_read(adis->spi_desc, buffer,
				       NO_OS_ARRAY_SIZE(buffer));
	if (ret)
		return ret;

	for (offset = 0; offset < 10; offset ++) {
		if (buffer[offset] == burst_id && buffer[offset + 1] != burst_id) {
			offset ++;
			break;
		}
	}

	if (crc_check) {
		recv_crc32 = no_os_get_unaligned_be16(&buffer[offset + 30]) |
			     (no_os_get_unaligned_be16(&buffer[offset + 32]) << 16);
		uint8_t crc_buffer[30];
		memcpy(crc_buffer, &buffer[offset], 30);
		/* The CRC algorithm expects data to be LSB format, swap memory */
		no_os_memswap64(crc_buffer, 30, 2);
		computed_crc32 = adis1654x_crc32_computation(30, crc_buffer);
		if (recv_crc32 != computed_crc32)
			return -EIO;
	}

	adis->diag_flags.checksum_err = false;

	data->temp_msb = 0;
	/* Temp data */
	memcpy(&data->temp_lsb, &buffer[offset + 2], 2);

	memcpy(&data->x_gyro_lsb, &buffer[offset + 4], 2);
	memcpy(&data->x_gyro_msb, &buffer[offset + 6], 2);
	memcpy(&data->y_gyro_lsb, &buffer[offset + 8], 2);
	memcpy(&data->y_gyro_msb, &buffer[offset + 10], 2);
	memcpy(&data->z_gyro_lsb, &buffer[offset + 12], 2);
	memcpy(&data->z_gyro_msb, &buffer[offset + 14], 2);
	memcpy(&data->x_accel_lsb, &buffer[offset + 16], 2);
	memcpy(&data->x_accel_msb, &buffer[offset + 18], 2);
	memcpy(&data->y_accel_lsb, &buffer[offset + 20], 2);
	memcpy(&data->y_accel_msb, &buffer[offset + 22], 2);
	memcpy(&data->z_accel_lsb, &buffer[offset + 24], 2);
	memcpy(&data->z_accel_msb, &buffer[offset + 26], 2);

	/* Counter data - aligned */
	data->data_cntr_lsb = no_os_get_unaligned_be16(&buffer[offset + 28]);
	data->data_cntr_msb = 0;

	/* Update diagnosis flags at each reading */
	adis_update_diag_flags(adis, buffer[offset]);

	return 0;
}

/**
 * @brief Read synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1654x_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode)
{
	int ret;

	ret = adis_read_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
	if(ret)
		return ret;

	/*
	 * ADIS16550 does not follow the same logic for sync mode:
	 * SYNC_DIRECT = 1
	 * SYNC_SCALED = 3
	 */
	if(*sync_mode == 3)
		*sync_mode = ADIS_SYNC_SCALED;

	return 0;
}

/**
 * @brief Update synchronization mode.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value to update.
 * @param ext_clk   - The external clock frequency to update, will be ignored
 * if sync_mode is different from ADIS_SYNC_SCALED and ADIS_SYNC_DIRECT.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1654x_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
				     uint32_t ext_clk)
{
	int ret;

	if(sync_mode > adis->info->sync_mode_max)
		return -EINVAL;

	if (sync_mode != ADIS_SYNC_DEFAULT) {
		/* Sync pulse is external */
		if (ext_clk < adis->info->sync_clk_freq_limits[sync_mode].min_freq
		    || ext_clk > adis->info->sync_clk_freq_limits[sync_mode].max_freq)
			return -EINVAL;

		adis->ext_clk = ext_clk;

		if (sync_mode == ADIS_SYNC_SCALED) {
			sync_mode = 3;
			/*
			 * In sync scaled mode, the IMU sample rate is the
			 * clk_freq * sync_scale.
			 * Hence, default the IMU sample rate to the highest
			 * multiple of the input clock lower than the IMU max
			 * sample rate.
			 */
			ret = adis_write_up_scale(adis,
						  adis->info->sampling_clk_limits.max_freq / ext_clk);
			if (ret)
				return ret;
		}
	}

	return adis_write_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
}

/**
 * @brief Read configured filter frequency.
 * @param adis - The adis device.
 * @param chan - The adis channel.
 * @param axis - Tha adis channel axis.
 * @param freq - The filter frequency, in Hz.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1654x_read_lpf(struct adis_dev *adis, enum adis_chan_type chan,
			      enum adis_axis_type axis, uint32_t *freq)
{
	int ret;
	uint32_t fir_en, bank_sel;

	*freq = 0;

	switch(chan) {
	case ADIS_ACCL_CHAN:
		switch (axis) {
		case ADIS_X_AXIS:
			ret = adis_read_fir_en_xa(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_xa(adis, &bank_sel);
			break;
		case ADIS_Y_AXIS:
			ret = adis_read_fir_en_ya(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_ya(adis, &bank_sel);
			break;
		case ADIS_Z_AXIS:
			ret = adis_read_fir_en_za(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_za(adis, &bank_sel);
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADIS_GYRO_CHAN:
		switch (axis) {
		case ADIS_X_AXIS:
			ret = adis_read_fir_en_xg(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_xg(adis, &bank_sel);
			break;
		case ADIS_Y_AXIS:
			ret = adis_read_fir_en_yg(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_yg(adis, &bank_sel);
			break;
		case ADIS_Z_AXIS:
			ret = adis_read_fir_en_zg(adis, &fir_en);
			if (ret)
				return ret;
			ret = adis_read_fir_bank_sel_zg(adis, &bank_sel);
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	if (fir_en) {
		if (bank_sel > NO_OS_ARRAY_SIZE(adis1654x_filt_freqs))
			return -EINVAL;

		*freq = adis1654x_filt_freqs[bank_sel];
	}

	return 0;
}

/**
 * @brief Configure filter for the given filter frequency.
 * @param adis - The adis device.
 * @param chan - The adis channel.
 * @param axis - Tha adis channel axis.
 * @param freq - The filter frequency, in Hz.
 * @return 0 in case of success, error code otherwise.
 */
static int adis1654x_write_lpf(struct adis_dev *adis, enum adis_chan_type chan,
			       enum adis_axis_type axis, uint32_t freq)
{
	uint32_t fir_en = 0;
	uint32_t bank_sel = NO_OS_ARRAY_SIZE(adis1654x_filt_freqs);
	int ret;

	if (freq > 0)
		fir_en = 1;

	while (--bank_sel && freq) {
		if (adis1654x_filt_freqs[bank_sel] >= freq)
			break;
	}

	switch(chan) {
	case ADIS_ACCL_CHAN:
		switch (axis) {
		case ADIS_X_AXIS:
			ret = adis_write_fir_en_xa(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_xa(adis, bank_sel);
			break;
		case ADIS_Y_AXIS:
			ret = adis_write_fir_en_ya(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_ya(adis, bank_sel);
			break;
		case ADIS_Z_AXIS:
			ret = adis_write_fir_en_za(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_za(adis, bank_sel);
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADIS_GYRO_CHAN:
		switch (axis) {
		case ADIS_X_AXIS:
			ret = adis_write_fir_en_xg(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_xg(adis, bank_sel);
			break;
		case ADIS_Y_AXIS:
			ret = adis_write_fir_en_yg(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_yg(adis, bank_sel);
			break;
		case ADIS_Z_AXIS:
			ret = adis_write_fir_en_zg(adis, fir_en);
			if (ret)
				return ret;
			ret = adis_write_fir_bank_sel_zg(adis, bank_sel);
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	/* Typical delay needed to enable and select filter bank. */
	no_os_udelay(65);

	return 0;
}

const struct adis_chip_info adis1654x_chip_info = {
	.cs_change_delay 	= 5,
	.read_delay 		= 0,
	.write_delay 		= 0,
	.timeouts 		= &adis1654x_timeouts,
	.field_map 		= &adis1654x_def,
	.fir_coef_idx_max	= 119,
	.dec_rate_max 		= 3999,
	.sync_mode_max 		= ADIS_SYNC_OUTPUT,
	.fls_mem_wr_cntr_max 	= 100000,
	.int_clk		= 4000,
	.sync_clk_freq_limits	= adis1654x_sync_clk_freq_limits,
	.sampling_clk_limits	= adis1654x_sampling_clk_limits,
	.flags			= ADIS_HAS_BURST32 | ADIS_HAS_BURST_DELTA_DATA,
	.has_paging		= true,
	.get_scale		= &adis1654x_get_scale,
	.get_offset		= &adis1654x_get_offset,
	.read_burst_data	= &adis1654x_read_burst_data,
	.read_sync_mode		= &adis1654x_read_sync_mode,
	.write_sync_mode	= &adis1654x_write_sync_mode,
	.read_lpf		= &adis1654x_read_lpf,
	.write_lpf		= &adis1654x_write_lpf,
};

