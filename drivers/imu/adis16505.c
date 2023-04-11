/***************************************************************************//**
 *   @file   adis16505.c
 *   @brief  Implementation of adis16505.c
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
#include "adis16505.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const struct adis_data_def adis16505_def = {
	.x_gyro 		= {.addr = 0x04, .size = 0x04, .mask = 0xFFFFFFFF},
	.y_gyro 		= {.addr = 0x08, .size = 0x04, .mask = 0xFFFFFFFF},
	.z_gyro 		= {.addr = 0x0C, .size = 0x04, .mask = 0xFFFFFFFF},
	.x_accl 		= {.addr = 0x10, .size = 0x04, .mask = 0xFFFFFFFF},
	.y_accl 		= {.addr = 0x14, .size = 0x04, .mask = 0xFFFFFFFF},
	.z_accl 		= {.addr = 0x18, .size = 0x04, .mask = 0xFFFFFFFF},
	.temp_out 		= {.addr = 0x1C, .size = 0x02, .mask = 0x0000FFFF},
	.time_stamp 		= {.addr = 0x1E, .size = 0x02, .mask = 0x0000FFFF},
	.data_cntr 		= {.addr = 0x22, .size = 0x02, .mask = 0x0000FFFF},
	.x_deltang 		= {.addr = 0x24, .size = 0x04, .mask = 0xFFFFFFFF},
	.y_deltang 		= {.addr = 0x28, .size = 0x04, .mask = 0xFFFFFFFF},
	.z_deltang 		= {.addr = 0x2C, .size = 0x04, .mask = 0xFFFFFFFF},
	.x_deltvel 		= {.addr = 0x30, .size = 0x04, .mask = 0xFFFFFFFF},
	.y_deltvel 		= {.addr = 0x34, .size = 0x04, .mask = 0xFFFFFFFF},
	.z_deltvel 		= {.addr = 0x38, .size = 0x04, .mask = 0xFFFFFFFF},
	.xg_bias 		= {.addr = 0x40, .size = 0x04, .mask = 0xFFFFFFFF},
	.yg_bias 		= {.addr = 0x44, .size = 0x04, .mask = 0xFFFFFFFF},
	.zg_bias 		= {.addr = 0x48, .size = 0x04, .mask = 0xFFFFFFFF},
	.xa_bias 		= {.addr = 0x4C, .size = 0x04, .mask = 0xFFFFFFFF},
	.ya_bias 		= {.addr = 0x50, .size = 0x04, .mask = 0xFFFFFFFF},
	.za_bias 		= {.addr = 0x54, .size = 0x04, .mask = 0xFFFFFFFF},
	.filt_ctrl 		= {.addr = 0x5C, .size = 0x02, .mask = 0x00000007},
	.up_scale 		= {.addr = 0x62, .size = 0x02, .mask = 0x0000FFFF},
	.dec_rate 		= {.addr = 0x64, .size = 0x02, .mask = 0x000007FF},
	.rang_mdl 		= {.addr = 0x5E, .size = 0x02, .mask = 0x0000000C},
	.firm_rev 		= {.addr = 0x6C, .size = 0x02, .mask = 0x0000FFFF},
	.firm_dm 		= {.addr = 0x6E, .size = 0x02, .mask = 0x0000FFFF},
	.firm_y 		= {.addr = 0x70, .size = 0x02, .mask = 0x0000FFFF},
	.prod_id 		= {.addr = 0x72, .size = 0x02, .mask = 0x0000FFFF},
	.serial_num 		= {.addr = 0x74, .size = 0x02, .mask = 0x0000FFFF},
	.usr_scr_1 		= {.addr = 0x76, .size = 0x02, .mask = 0x0000FFFF},
	.usr_scr_2 		= {.addr = 0x78, .size = 0x02, .mask = 0x0000FFFF},
	.usr_scr_3 		= {.addr = 0x7A, .size = 0x02, .mask = 0x0000FFFF},
	.flshcnt		= {.addr = 0x7C, .size = 0x04, .mask = 0xFFFFFFFF},
	.dr_polarity 		= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(0)},
	.sync_polarity 		= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(1)},
	.sync_mode 		= {.addr = 0x60, .size = 0x02, .mask = 0x0000000C},
	.sens_bw		= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(4)},
	.pt_of_perc_algnmt 	= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(6)},
	.linear_accl_comp 	= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(7)},
	.burst_sel 		= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(8)},
	.burst_size 		= {.addr = 0x60, .size = 0x02, .mask = NO_OS_BIT(9)},
	.sw_res 		= {.addr = 0x68, .size = 0x02, .mask = NO_OS_BIT(7)},
	.fls_mem_test 		= {.addr = 0x68, .size = 0x02, .mask = NO_OS_BIT(4)},
	.fls_mem_update 	= {.addr = 0x68, .size = 0x02, .mask = NO_OS_BIT(3)},
	.snsr_self_test 	= {.addr = 0x68, .size = 0x02, .mask = NO_OS_BIT(2)},
	.fact_calib_restore 	= {.addr = 0x68, .size = 0x02, .mask = NO_OS_BIT(1)},
	.diag_stat 		= {.addr = 0x02, .size = 0x02, .mask = 0x000007FE},
	.data_path_overrun_mask = NO_OS_BIT(1),
	.fls_mem_update_mask 	= NO_OS_BIT(2),
	.spi_comm_err_mask 	= NO_OS_BIT(3),
	.standby_mode_mask 	= NO_OS_BIT(4),
	.snsr_self_test_mask 	= NO_OS_BIT(5),
	.fls_mem_test_mask	= NO_OS_BIT(6),
	.clock_err_mask 	= NO_OS_BIT(7),
	.gyro1_self_test_mask 	= NO_OS_BIT(8),
	.gyro2_self_test_mask 	= NO_OS_BIT(9),
	.accl_self_test_mask 	= NO_OS_BIT(10),
};

static const struct adis_timeout adis16505_timeouts = {
	.reset_ms 		= 260,
	.sw_reset_ms 		= 260,
	.self_test_ms 		= 20,
	.filter_update_us 	= 30,
	.msc_reg_update_us 	= 200,
	.sens_bw_update_ms 	= 250,
};

static const struct adis_sync_rate_limit adis16505_sync_mode[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 1900, 2100 },
	[ADIS_SYNC_SCALED] = { 1, 128 },
	[ADIS_SYNC_OUTPUT] = { },
};

struct adis_data adis16505_data = {
	.cs_change_delay 	= 16,
	.read_delay 		= 0,
	.write_delay 		= 0,
	.timeouts 		= &adis16505_timeouts,
	.reg_map 		= &adis16505_def,
	.filt_ctrl_max 		= 6,
	.dec_rate_max 		= 1999,
	.sync_mode_max 		= ADIS_SYNC_OUTPUT,
	.sens_bw_max 		= 1,
	.burst_sel_max		= 1,
	.burst_size_max		= 1,
	.prod_id 		= 0x4079,
	.has_paging 		= false,
	.flshcnt_max 		= 10000,
	.int_clk		= 2000,
	.sync_rate_limit	= adis16505_sync_mode,
	.linear_accl_comp_max	= 1,
	.pt_of_perc_algnmt_max	= 1,
};
