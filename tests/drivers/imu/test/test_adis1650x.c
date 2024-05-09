/***************************************************************************//**
 *   @file   test_adis1650x.c
 *   @brief  Implementation of test_adis1650x.c
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

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "unity.h"
#include "adis.h"
#include "adis1650x.h"
#include "test_adis.c"
#include "mock_no_os_delay.h"
#include "mock_no_os_util.h"
#include "mock_no_os_i2c.h"
#include "mock_no_os_gpio.h"
#include "mock_no_os_spi.h"
#include "mock_no_os_alloc.h"
#include <errno.h>

/*******************************************************************************
 *    PUBLIC DATA
 ******************************************************************************/

const struct adis_chip_info *adis_chip_info = &adis1650x_chip_info;
enum adis_device_id adis_dev_id = ADIS16505_2;

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void)
{
}

void tearDown(void)
{
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_adis1650x_init(void)
{
	test_adis_init_1();
	test_adis_init_2();
	test_adis_init_4();
	test_adis_init_5();
}

void test_adis1650x_remove(void)
{
	test_adis_remove_1();
	test_adis_remove_2();
}

void test_adis1650x_initial_startup(void)
{
	test_adis_initial_startup_1();
	test_adis_initial_startup_2();
	test_adis_initial_startup_3();
}

void test_adis1650x_read_reg(void)
{
	test_adis_read_reg();
}

void test_adis1650x_read_field_s32(void)
{
	test_adis_read_field_s32();
}

void test_adis1650x_write_reg(void)
{
	test_adis_write_reg_1();
	test_adis_write_reg_2();
}

void test_adis1650x_read_diag_data_path_overrun(void)
{
	test_adis_read_diag_data_path_overrun_1();
	test_adis_read_diag_data_path_overrun_2();
	test_adis_read_diag_data_path_overrun_3();
}

void test_adis1650x_read_diag_fls_mem_update_failure(void)
{
	test_adis_read_diag_fls_mem_update_failure_1();
	test_adis_read_diag_fls_mem_update_failure_2();
	test_adis_read_diag_fls_mem_update_failure_3();
}

void test_adis1650x_read_diag_spi_comm_err(void)
{
	test_adis_read_diag_spi_comm_err_1();
	test_adis_read_diag_spi_comm_err_2();
	test_adis_read_diag_spi_comm_err_3();
}

void test_adis1650x_read_diag_standby_mode(void)
{
	test_adis_read_diag_standby_mode_1();
	test_adis_read_diag_standby_mode_2();
	test_adis_read_diag_standby_mode_3();
}

void test_adis1650x_read_diag_snsr_failure(void)
{
	test_adis_read_diag_snsr_failure_1();
	test_adis_read_diag_snsr_failure_2();
	test_adis_read_diag_snsr_failure_3();
}

void test_adis1650x_read_diag_mem_failure(void)
{
	test_adis_read_diag_mem_failure_1();
	test_adis_read_diag_mem_failure_2();
	test_adis_read_diag_mem_failure_3();
}

void test_adis1650x_read_diag_clk_err(void)
{
	test_adis_read_diag_clk_err_1();
	test_adis_read_diag_clk_err_2();
	test_adis_read_diag_clk_err_3();
}

void test_adis1650x_read_diag_gyro2_failure(void)
{
	test_adis_read_diag_gyro2_failure_1();
	test_adis_read_diag_gyro2_failure_2();
	test_adis_read_diag_gyro2_failure_3();
}

void test_adis1650x_read_diag_gyro1_failure(void)
{
	test_adis_read_diag_gyro1_failure_1();
	test_adis_read_diag_gyro1_failure_2();
	test_adis_read_diag_gyro1_failure_3();
}

void test_adis1650x_read_diag_accl_failure(void)
{
	test_adis_read_diag_accl_failure_1();
	test_adis_read_diag_accl_failure_2();
	test_adis_read_diag_accl_failure_3();
}

void test_adis1650x_read_diag_checksum_err(void)
{
	test_adis_read_diag_checksum_err_1();
	test_adis_read_diag_checksum_err_2();
}

void test_adis1650x_read_diag_fls_mem_wr_cnt_exceed(void)
{
	test_adis_read_diag_fls_mem_wr_cnt_exceed_1();
	test_adis_read_diag_fls_mem_wr_cnt_exceed_2();
}

void test_adis1650x_read_x_gyro(void)
{
	test_adis_read_x_gyro_3();
}

void test_adis1650x_read_y_gyro(void)
{
	test_adis_read_y_gyro_3();
}

void test_adis1650x_read_z_gyro(void)
{
	test_adis_read_z_gyro_3();
}

void test_adis1650x_read_x_accl(void)
{
	test_adis_read_x_accl_3();
}

void test_adis1650x_read_y_accl(void)
{
	test_adis_read_y_accl_3();
}

void test_adis1650x_read_z_accl(void)
{
	test_adis_read_z_accl_3();
}

void test_adis1650x_read_temp_out(void)
{
	test_adis_read_temp_out_3();
}

void test_adis1650x_read_time_stamp(void)
{
	test_adis_read_time_stamp_size16();
}

void test_adis1650x_read_data_cntr(void)
{
	test_adis_read_data_cntr();
}

void test_adis1650x_read_x_deltang(void)
{
	test_adis_read_x_deltang_3();
}

void test_adis1650x_read_y_deltang(void)
{
	test_adis_read_y_deltang_3();
}

void test_adis1650x_read_z_deltang(void)
{
	test_adis_read_z_deltang_3();
}

void test_adis1650x_read_x_deltvel(void)
{
	test_adis_read_x_deltvel_3();
}

void test_adis1650x_read_y_deltvel(void)
{
	test_adis_read_y_deltvel_3();
}

void test_adis1650x_read_z_deltvel(void)
{
	test_adis_read_z_deltvel_3();
}

void test_adis1650x_read_xg_bias(void)
{
	test_adis_read_xg_bias();
}

void test_adis1650x_write_xg_bias(void)
{
	test_adis_write_xg_bias();
}

void test_adis1650x_read_yg_bias(void)
{
	test_adis_read_yg_bias();
}

void test_adis1650x_write_yg_bias(void)
{
	test_adis_write_yg_bias();
}

void test_adis1650x_read_zg_bias(void)
{
	test_adis_read_zg_bias();
}

void test_adis1650x_write_zg_bias(void)
{
	test_adis_write_zg_bias();
}

void test_adis1650x_read_xa_bias(void)
{
	test_adis_read_xa_bias();
}

void test_adis1650x_write_xa_bias(void)
{
	test_adis_write_xa_bias();
}

void test_adis1650x_read_ya_bias(void)
{
	test_adis_read_ya_bias();
}

void test_adis1650x_write_ya_bias(void)
{
	test_adis_write_ya_bias();
}

void test_adis1650x_read_za_bias(void)
{
	test_adis_read_za_bias();
}

void test_adis1650x_write_za_bias(void)
{
	test_adis_write_za_bias();
}


void test_adis1650x_read_filt_size_var_b(void)
{
	test_adis_read_filt_size_var_b();
}

void test_adis1650x_write_filt_size_var_b(void)
{
	test_adis_write_filt_size_var_b_1();
	test_adis_write_filt_size_var_b_2();
	test_adis_write_filt_size_var_b_3();
}

void test_adis1650x_read_gyro_meas_range(void)
{
	test_adis_read_gyro_meas_range();
}

void test_adis1650x_read_dr_polarity(void)
{
	test_adis_read_dr_polarity();
}

void test_adis1650x_write_dr_polarity(void)
{
	test_adis_write_dr_polarity_1();
	test_adis_write_dr_polarity_2();
	test_adis_write_dr_polarity_3();
}

void test_adis1650x_read_sync_polarity(void)
{
	test_adis_read_sync_polarity();
}

void test_adis1650x_write_sync_polarity(void)
{
	test_adis_write_sync_polarity_1();
	test_adis_write_sync_polarity_2();
	test_adis_write_sync_polarity_3();
}

void test_adis1650x_read_sync_mode(void)
{
	test_adis_read_sync_mode();
}

void test_adis1650x_write_sync_mode(void)
{
	test_adis_write_sync_mode_1();
	test_adis_write_sync_mode_2();
	test_adis_write_sync_mode_3();
	test_adis_write_sync_mode_4();
	test_adis_write_sync_mode_5();
	test_adis_write_sync_mode_6();
	test_adis_write_sync_mode_7();
	test_adis_write_sync_mode_8();
	test_adis_write_sync_mode_9();
}

void test_adis1650x_read_sens_bw(void)
{
	test_adis_read_sens_bw();
}

void test_adis1650x_write_sens_bw(void)
{
	test_adis_write_sens_bw_1();
	test_adis_write_sens_bw_2();
}

void test_adis1650x_read_pt_of_perc_algnmt(void)
{
	test_adis_read_pt_of_perc_algnmt();
}

void test_adis1650x_write_pt_of_perc_algnmt(void)
{
	test_adis_write_pt_of_perc_algnmt_1();
	test_adis_write_pt_of_perc_algnmt_2();
}

void test_adis1650x_read_linear_accl_comp(void)
{
	test_adis_read_linear_accl_comp();
}

void test_adis1650x_write_linear_accl_comp(void)
{
	test_adis_write_linear_accl_comp_1();
	test_adis_write_linear_accl_comp_2();
}

void test_adis1650x_read_burst_sel(void)
{
	test_adis_read_burst_sel_1();
	test_adis_read_burst_sel_2();
}

void test_adis1650x_write_burst_sel(void)
{
	test_adis_write_burst_sel_1();
	test_adis_write_burst_sel_2();
}

void test_adis1650x_read_burst32(void)
{
	test_adis_read_burst32_1();
	test_adis_read_burst32_2();

}

void test_adis1650x_write_burst32(void)
{
	test_adis_write_burst32_1();
	test_adis_write_burst32_2();
}

void test_adis1650x_read_up_scale(void)
{
	test_adis_read_up_scale();
}

void test_adis1650x_write_up_scale(void)
{
	test_adis_write_up_scale_1();
	test_adis_write_up_scale_2();
	test_adis_write_up_scale_3();
	test_adis_write_up_scale_4();
}

void test_adis1650x_read_dec_rate(void)
{
	test_adis_read_dec_rate();
}

void test_adis1650x_write_dec_rate(void)
{
	test_adis_write_dec_rate_1();
	test_adis_write_dec_rate_2();
	test_adis_write_dec_rate_3();
}

void test_adis1650x_cmd_fact_calib_restore(void)
{
	test_adis_cmd_fact_calib_restore_1();
	test_adis_cmd_fact_calib_restore_2();
}

void test_adis1650x_cmd_snsr_self_test()
{
	test_adis_cmd_snsr_self_test_1();
	test_adis_cmd_snsr_self_test_2();
}

void test_adis1650x_cmd_fls_mem_update(void)
{
	test_adis_cmd_fls_mem_update_1();
	test_adis_cmd_fls_mem_update_2();
}

void test_adis1650x_cmd_fls_mem_test(void)
{
	test_adis_cmd_fls_mem_test_1();
	test_adis_cmd_fls_mem_test_2();
}

void test_adis1650x_cmd_sw_res(void)
{
	test_adis_cmd_sw_res_1();
	test_adis_cmd_sw_res_2();
}

void test_adis1650x_read_firm_rev(void)
{
	test_adis_read_firm_rev();
}

void test_adis1650x_read_firm_d(void)
{
	test_adis_read_firm_d();
}

void test_adis1650x_read_firm_m(void)
{
	test_adis_read_firm_m();
}

void test_adis1650x_read_firm_y(void)
{
	test_adis_read_firm_y();
}

void test_adis1650x_read_prod_id(void)
{
	test_adis_read_prod_id();
}

void test_adis1650x_read_serial_num(void)
{
	test_adis_read_serial_num();
}

void test_adis1650x_read_usr_scr_1(void)
{
	test_adis_read_usr_scr_1();
}

void test_adis1650x_write_usr_scr_1(void)
{
	test_adis_write_usr_scr_1();
}

void test_adis1650x_read_usr_scr_2(void)
{
	test_adis_read_usr_scr_2();
}

void test_adis1650x_write_usr_scr_2(void)
{
	test_adis_write_usr_scr_2();
}

void test_adis1650x_read_usr_scr_3(void)
{
	test_adis_read_usr_scr_3();
}

void test_adis1650x_write_usr_scr_3(void)
{
	test_adis_write_usr_scr_3();
}

void test_adis1650x_read_fls_mem_wr_cntr(void)
{
	test_adis_read_fls_mem_wr_cntr_1();
	test_adis_read_fls_mem_wr_cntr_2();
	test_adis_read_fls_mem_wr_cntr_3();
}

void test_adis1650x_read_burst_data(void)
{
	test_adis_read_burst_data_1();
	test_adis_read_burst_data_2();
	test_adis_read_burst_data_3();
	test_adis_read_burst_data_6();
}

void test_adis1650x_update_ext_clk_freq(void)
{
	test_adis_update_ext_clk_freq_1();
	test_adis_update_ext_clk_freq_2();
	test_adis_update_ext_clk_freq_3();
	test_adis_update_ext_clk_freq_4();
	test_adis_update_ext_clk_freq_5();
	test_adis_update_ext_clk_freq_6();
}

void test_adis1650x_get_sync_clk_freq(void)
{
	test_adis_get_sync_clk_freq_1();
	test_adis_get_sync_clk_freq_2();
	test_adis_get_sync_clk_freq_3();
	test_adis_get_sync_clk_freq_4();
}

void test_adis1650x_get_anglvel_scale(void)
{
	test_adis_get_anglvel_scale_1();
	test_adis_get_anglvel_scale_2();
	test_adis_get_anglvel_scale_3();
	test_adis_get_anglvel_scale_4();
}

void test_adis1650x_get_accl_scale(void)
{
	test_adis_get_accl_scale_1();
	test_adis_get_accl_scale_2();
	test_adis_get_accl_scale_3();
	test_adis_get_accl_scale_4();
}

void test_adis1650x_get_deltaangl_scale(void)
{
	test_adis_get_deltaangl_scale_1();
	test_adis_get_deltaangl_scale_2();
	test_adis_get_deltaangl_scale_3();
	test_adis_get_deltaangl_scale_4();
}

void test_adis1650x_get_deltavelocity_scale(void)
{
	test_adis_get_deltavelocity_scale_1();
	test_adis_get_deltavelocity_scale_2();
	test_adis_get_deltavelocity_scale_3();
	test_adis_get_deltavelocity_scale_4();
}

void test_adis1650x_get_temp_scale(void)
{
	test_adis_get_temp_scale_1();
	test_adis_get_temp_scale_2();
	test_adis_get_temp_scale_3();
	test_adis_get_temp_scale_4();
}
