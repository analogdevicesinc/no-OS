// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI AXI_ADRV9102_SSI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************


#include "axi_adrv9102_ssi_regs.h"
#include "axi_adrv9102_ssi.h"


/*  register field read, axi_adrv9102_ssi_cmos_lvds_id */

int32_t axi_adrv9102_ssi_cmos_lvds_id_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_CMOS_LVDS_ID_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_control */

int32_t axi_adrv9102_ssi_reg_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_control */

int32_t axi_adrv9102_ssi_reg_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_num_of_lanes */

int32_t axi_adrv9102_ssi_num_of_lanes_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_NUM_OF_LANES_ADDR + addr_offset), AXI_ADRV9102_SSI_NUM_OF_LANES_POS, AXI_ADRV9102_SSI_NUM_OF_LANES_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_num_of_lanes */

int32_t axi_adrv9102_ssi_num_of_lanes_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_NUM_OF_LANES_ADDR + addr_offset), AXI_ADRV9102_SSI_NUM_OF_LANES_POS, AXI_ADRV9102_SSI_NUM_OF_LANES_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_num_of_bits */

int32_t axi_adrv9102_ssi_num_of_bits_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_NUM_OF_BITS_ADDR + addr_offset), AXI_ADRV9102_SSI_NUM_OF_BITS_POS, AXI_ADRV9102_SSI_NUM_OF_BITS_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_num_of_bits */

int32_t axi_adrv9102_ssi_num_of_bits_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_NUM_OF_BITS_ADDR + addr_offset), AXI_ADRV9102_SSI_NUM_OF_BITS_POS, AXI_ADRV9102_SSI_NUM_OF_BITS_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_lstrb1_pstrb0 */

int32_t axi_adrv9102_ssi_lstrb1_pstrb0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_LSTRB1_PSTRB0_ADDR + addr_offset), AXI_ADRV9102_SSI_LSTRB1_PSTRB0_POS, AXI_ADRV9102_SSI_LSTRB1_PSTRB0_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_lstrb1_pstrb0 */

int32_t axi_adrv9102_ssi_lstrb1_pstrb0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_LSTRB1_PSTRB0_ADDR + addr_offset), AXI_ADRV9102_SSI_LSTRB1_PSTRB0_POS, AXI_ADRV9102_SSI_LSTRB1_PSTRB0_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_sdr1_ddr0 */

int32_t axi_adrv9102_ssi_sdr1_ddr0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_SDR1_DDR0_ADDR + addr_offset), AXI_ADRV9102_SSI_SDR1_DDR0_POS, AXI_ADRV9102_SSI_SDR1_DDR0_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_sdr1_ddr0 */

int32_t axi_adrv9102_ssi_sdr1_ddr0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_SDR1_DDR0_ADDR + addr_offset), AXI_ADRV9102_SSI_SDR1_DDR0_POS, AXI_ADRV9102_SSI_SDR1_DDR0_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_lsb1_msb0 */

int32_t axi_adrv9102_ssi_lsb1_msb0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_LSB1_MSB0_ADDR + addr_offset), AXI_ADRV9102_SSI_LSB1_MSB0_POS, AXI_ADRV9102_SSI_LSB1_MSB0_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_lsb1_msb0 */

int32_t axi_adrv9102_ssi_lsb1_msb0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_LSB1_MSB0_ADDR + addr_offset), AXI_ADRV9102_SSI_LSB1_MSB0_POS, AXI_ADRV9102_SSI_LSB1_MSB0_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_q1_i0 */

int32_t axi_adrv9102_ssi_q1_i0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_Q1_I0_ADDR + addr_offset), AXI_ADRV9102_SSI_Q1_I0_POS, AXI_ADRV9102_SSI_Q1_I0_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_q1_i0 */

int32_t axi_adrv9102_ssi_q1_i0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_Q1_I0_ADDR + addr_offset), AXI_ADRV9102_SSI_Q1_I0_POS, AXI_ADRV9102_SSI_Q1_I0_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_fall1_rise0 */

int32_t axi_adrv9102_ssi_fall1_rise0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_FALL1_RISE0_ADDR + addr_offset), AXI_ADRV9102_SSI_FALL1_RISE0_POS, AXI_ADRV9102_SSI_FALL1_RISE0_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_fall1_rise0 */

int32_t axi_adrv9102_ssi_fall1_rise0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_FALL1_RISE0_ADDR + addr_offset), AXI_ADRV9102_SSI_FALL1_RISE0_POS, AXI_ADRV9102_SSI_FALL1_RISE0_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_sign_extn */

int32_t axi_adrv9102_ssi_sign_extn_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_SIGN_EXTN_ADDR + addr_offset), AXI_ADRV9102_SSI_SIGN_EXTN_POS, AXI_ADRV9102_SSI_SIGN_EXTN_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_sign_extn */

int32_t axi_adrv9102_ssi_sign_extn_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_SIGN_EXTN_ADDR + addr_offset), AXI_ADRV9102_SSI_SIGN_EXTN_POS, AXI_ADRV9102_SSI_SIGN_EXTN_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_cmos1_lvds0 */

int32_t axi_adrv9102_ssi_cmos1_lvds0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_CMOS1_LVDS0_ADDR + addr_offset), AXI_ADRV9102_SSI_CMOS1_LVDS0_POS, AXI_ADRV9102_SSI_CMOS1_LVDS0_MASK, data));
}


/*  register read, axi_adrv9102_ssi_reg_io_control */

int32_t axi_adrv9102_ssi_reg_io_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_io_control */

int32_t axi_adrv9102_ssi_reg_io_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_IO_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_clk_sel */

int32_t axi_adrv9102_ssi_clk_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_SEL_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_SEL_POS, AXI_ADRV9102_SSI_CLK_SEL_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_clk_sel */

int32_t axi_adrv9102_ssi_clk_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_SEL_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_SEL_POS, AXI_ADRV9102_SSI_CLK_SEL_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_clk_msb */

int32_t axi_adrv9102_ssi_clk_msb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_MSB_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_MSB_POS, AXI_ADRV9102_SSI_CLK_MSB_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_clk_msb */

int32_t axi_adrv9102_ssi_clk_msb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_MSB_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_MSB_POS, AXI_ADRV9102_SSI_CLK_MSB_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_clk_enb */

int32_t axi_adrv9102_ssi_clk_enb_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_ENB_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_ENB_POS, AXI_ADRV9102_SSI_CLK_ENB_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_clk_enb */

int32_t axi_adrv9102_ssi_clk_enb_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_ENB_ADDR + addr_offset), AXI_ADRV9102_SSI_CLK_ENB_POS, AXI_ADRV9102_SSI_CLK_ENB_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_buf_enable */

int32_t axi_adrv9102_ssi_buf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_BUF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_BUF_ENABLE_POS, AXI_ADRV9102_SSI_BUF_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_buf_enable */

int32_t axi_adrv9102_ssi_buf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_BUF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_BUF_ENABLE_POS, AXI_ADRV9102_SSI_BUF_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_init */

int32_t axi_adrv9102_ssi_init_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_INIT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_init */

int32_t axi_adrv9102_ssi_init_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_INIT_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_ss_control */

int32_t axi_adrv9102_ssi_reg_ss_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_SS_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_ss_control */

int32_t axi_adrv9102_ssi_reg_ss_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_SS_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_ss_ratio */

int32_t axi_adrv9102_ssi_ss_ratio_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_SS_RATIO_ADDR + addr_offset), AXI_ADRV9102_SSI_SS_RATIO_POS, AXI_ADRV9102_SSI_SS_RATIO_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_ss_ratio */

int32_t axi_adrv9102_ssi_ss_ratio_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_SS_RATIO_ADDR + addr_offset), AXI_ADRV9102_SSI_SS_RATIO_POS, AXI_ADRV9102_SSI_SS_RATIO_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_obs_ss_ratio */

int32_t axi_adrv9102_ssi_obs_ss_ratio_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_OBS_SS_RATIO_ADDR + addr_offset), AXI_ADRV9102_SSI_OBS_SS_RATIO_POS, AXI_ADRV9102_SSI_OBS_SS_RATIO_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_obs_ss_ratio */

int32_t axi_adrv9102_ssi_obs_ss_ratio_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_OBS_SS_RATIO_ADDR + addr_offset), AXI_ADRV9102_SSI_OBS_SS_RATIO_POS, AXI_ADRV9102_SSI_OBS_SS_RATIO_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_ss_mode */

int32_t axi_adrv9102_ssi_ss_mode_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_SS_MODE_ADDR + addr_offset), AXI_ADRV9102_SSI_SS_MODE_POS, AXI_ADRV9102_SSI_SS_MODE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_ss_mode */

int32_t axi_adrv9102_ssi_ss_mode_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_SS_MODE_ADDR + addr_offset), AXI_ADRV9102_SSI_SS_MODE_POS, AXI_ADRV9102_SSI_SS_MODE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_data_sel */

int32_t axi_adrv9102_ssi_data_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_SEL_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_data_sel */

int32_t axi_adrv9102_ssi_data_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_SEL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_pat_0 */

int32_t axi_adrv9102_ssi_data_pat_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_0_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_data_pat_0 */

int32_t axi_adrv9102_ssi_data_pat_0_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_0_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_pat_1 */

int32_t axi_adrv9102_ssi_data_pat_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_1_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_data_pat_1 */

int32_t axi_adrv9102_ssi_data_pat_1_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_1_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_0 */

int32_t axi_adrv9102_ssi_data_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_0_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_1 */

int32_t axi_adrv9102_ssi_data_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_1_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_status */

int32_t axi_adrv9102_ssi_reg_status_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_STATUS_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_status */

int32_t axi_adrv9102_ssi_reg_status_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_STATUS_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_frm_oos */

int32_t axi_adrv9102_ssi_frm_oos_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_FRM_OOS_ADDR + addr_offset), AXI_ADRV9102_SSI_FRM_OOS_POS, AXI_ADRV9102_SSI_FRM_OOS_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_frm_oos */

int32_t axi_adrv9102_ssi_frm_oos_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_FRM_OOS_ADDR + addr_offset), AXI_ADRV9102_SSI_FRM_OOS_POS, AXI_ADRV9102_SSI_FRM_OOS_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_frm_err */

int32_t axi_adrv9102_ssi_frm_err_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_FRM_ERR_ADDR + addr_offset), AXI_ADRV9102_SSI_FRM_ERR_POS, AXI_ADRV9102_SSI_FRM_ERR_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_frm_err */

int32_t axi_adrv9102_ssi_frm_err_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_FRM_ERR_ADDR + addr_offset), AXI_ADRV9102_SSI_FRM_ERR_POS, AXI_ADRV9102_SSI_FRM_ERR_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_mon_oos */

int32_t axi_adrv9102_ssi_mon_oos_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_MON_OOS_ADDR + addr_offset), AXI_ADRV9102_SSI_MON_OOS_POS, AXI_ADRV9102_SSI_MON_OOS_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_mon_oos */

int32_t axi_adrv9102_ssi_mon_oos_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_MON_OOS_ADDR + addr_offset), AXI_ADRV9102_SSI_MON_OOS_POS, AXI_ADRV9102_SSI_MON_OOS_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_mon_err */

int32_t axi_adrv9102_ssi_mon_err_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_MON_ERR_ADDR + addr_offset), AXI_ADRV9102_SSI_MON_ERR_POS, AXI_ADRV9102_SSI_MON_ERR_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_mon_err */

int32_t axi_adrv9102_ssi_mon_err_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_MON_ERR_ADDR + addr_offset), AXI_ADRV9102_SSI_MON_ERR_POS, AXI_ADRV9102_SSI_MON_ERR_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_lb_ovf */

int32_t axi_adrv9102_ssi_lb_ovf_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_LB_OVF_ADDR + addr_offset), AXI_ADRV9102_SSI_LB_OVF_POS, AXI_ADRV9102_SSI_LB_OVF_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_lb_ovf */

int32_t axi_adrv9102_ssi_lb_ovf_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_LB_OVF_ADDR + addr_offset), AXI_ADRV9102_SSI_LB_OVF_POS, AXI_ADRV9102_SSI_LB_OVF_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_lb_unf */

int32_t axi_adrv9102_ssi_lb_unf_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_LB_UNF_ADDR + addr_offset), AXI_ADRV9102_SSI_LB_UNF_POS, AXI_ADRV9102_SSI_LB_UNF_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_lb_unf */

int32_t axi_adrv9102_ssi_lb_unf_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_LB_UNF_ADDR + addr_offset), AXI_ADRV9102_SSI_LB_UNF_POS, AXI_ADRV9102_SSI_LB_UNF_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_align_sel */

int32_t axi_adrv9102_ssi_align_sel_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_ALIGN_SEL_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_align_sel */

int32_t axi_adrv9102_ssi_align_sel_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_ALIGN_SEL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_delay_unlocked */

int32_t axi_adrv9102_ssi_delay_unlocked_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DELAY_UNLOCKED_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_delay_control */

int32_t axi_adrv9102_ssi_reg_delay_control_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_SSI_REG_DELAY_CONTROL_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_DELAY_CONTROL_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_delay_control */

int32_t axi_adrv9102_ssi_reg_delay_control_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_SSI_REG_DELAY_CONTROL_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_DELAY_CONTROL_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_wrdelay */

int32_t axi_adrv9102_ssi_wrdelay_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_SSI_WRDELAY_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_WRDELAY_ADDR + addr_offset), AXI_ADRV9102_SSI_WRDELAY_POS, AXI_ADRV9102_SSI_WRDELAY_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_wrdelay */

int32_t axi_adrv9102_ssi_wrdelay_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_SSI_WRDELAY_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_WRDELAY_ADDR + addr_offset), AXI_ADRV9102_SSI_WRDELAY_POS, AXI_ADRV9102_SSI_WRDELAY_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_rddelay */

int32_t axi_adrv9102_ssi_rddelay_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t index_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = (index_id * AXI_ADRV9102_SSI_RDDELAY_ADDR_OFFSET);
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_RDDELAY_ADDR + addr_offset), AXI_ADRV9102_SSI_RDDELAY_POS, AXI_ADRV9102_SSI_RDDELAY_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_delay_sweep */

int32_t axi_adrv9102_ssi_delay_sweep_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DELAY_SWEEP_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_delay_sweep */

int32_t axi_adrv9102_ssi_delay_sweep_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DELAY_SWEEP_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_timer */

int32_t axi_adrv9102_ssi_timer_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_TIMER_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_timer */

int32_t axi_adrv9102_ssi_timer_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_TIMER_ADDR + addr_offset), data));
}


/* timer delay (us) axi_adrv9102_ssi_timer */

int32_t axi_adrv9102_ssi_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_timer_delay_us(a_device, peripheral_id, (AXI_ADRV9102_SSI_TIMER_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_clk_mon_count */

int32_t axi_adrv9102_ssi_clk_mon_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/* count frequency (Hz) axi_adrv9102_ssi_clk_mon_count */

int32_t axi_adrv9102_ssi_clk_mon_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_count_freq_hz(a_device, peripheral_id, (AXI_ADRV9102_SSI_CLK_MON_COUNT_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_mcs_latency */

int32_t axi_adrv9102_ssi_reg_mcs_latency_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_MCS_LATENCY_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_mcs_latency_count */

int32_t axi_adrv9102_ssi_mcs_latency_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_LATENCY_COUNT_ADDR + addr_offset), AXI_ADRV9102_SSI_MCS_LATENCY_COUNT_POS, AXI_ADRV9102_SSI_MCS_LATENCY_COUNT_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_mcs_latency_busy */

int32_t axi_adrv9102_ssi_mcs_latency_busy_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_LATENCY_BUSY_ADDR + addr_offset), AXI_ADRV9102_SSI_MCS_LATENCY_BUSY_POS, AXI_ADRV9102_SSI_MCS_LATENCY_BUSY_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_mcs_delay_count */

int32_t axi_adrv9102_ssi_mcs_delay_count_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_DELAY_COUNT_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_mcs_delay_count */

int32_t axi_adrv9102_ssi_mcs_delay_count_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_DELAY_COUNT_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_mcs_strobe */

int32_t axi_adrv9102_ssi_mcs_strobe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_STROBE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_mcs_data */

int32_t axi_adrv9102_ssi_mcs_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_DATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_mcs_bstrobe */

int32_t axi_adrv9102_ssi_mcs_bstrobe_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_BSTROBE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_mcs_bdata */

int32_t axi_adrv9102_ssi_mcs_bdata_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_MCS_BDATA_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_pat_pulse */

int32_t axi_adrv9102_ssi_data_pat_pulse_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_PULSE_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_data_pat_pulse */

int32_t axi_adrv9102_ssi_data_pat_pulse_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_PULSE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_pat_start */

int32_t axi_adrv9102_ssi_data_pat_start_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_START_ADDR + addr_offset), data));
}


/*  register field write, axi_adrv9102_ssi_data_pat_start */

int32_t axi_adrv9102_ssi_data_pat_start_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_START_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_data_pat_latency */

int32_t axi_adrv9102_ssi_data_pat_latency_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_DATA_PAT_LATENCY_ADDR + addr_offset), data));
}


/*  register read, axi_adrv9102_ssi_reg_master_enable */

int32_t axi_adrv9102_ssi_reg_master_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_MASTER_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_master_enable */

int32_t axi_adrv9102_ssi_reg_master_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_MASTER_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_master_enable */

int32_t axi_adrv9102_ssi_master_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_MASTER_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_MASTER_ENABLE_POS, AXI_ADRV9102_SSI_MASTER_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_master_enable */

int32_t axi_adrv9102_ssi_master_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_MASTER_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_MASTER_ENABLE_POS, AXI_ADRV9102_SSI_MASTER_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_obs_master_enable */

int32_t axi_adrv9102_ssi_obs_master_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_POS, AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_obs_master_enable */

int32_t axi_adrv9102_ssi_obs_master_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_POS, AXI_ADRV9102_SSI_OBS_MASTER_ENABLE_MASK, data));
}


/*  register read, axi_adrv9102_ssi_reg_intr_enable */

int32_t axi_adrv9102_ssi_reg_intr_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_READ(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register write, axi_adrv9102_ssi_reg_intr_enable */

int32_t axi_adrv9102_ssi_reg_intr_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(AXI_REG_WRITE(a_device, peripheral_id, (AXI_ADRV9102_SSI_REG_INTR_ENABLE_ADDR + addr_offset), data));
}


/*  register field read, axi_adrv9102_ssi_intr_frm_oos_enable */

int32_t axi_adrv9102_ssi_intr_frm_oos_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_POS, AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_frm_oos_enable */

int32_t axi_adrv9102_ssi_intr_frm_oos_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_POS, AXI_ADRV9102_SSI_INTR_FRM_OOS_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_intr_frm_err_enable */

int32_t axi_adrv9102_ssi_intr_frm_err_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_POS, AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_frm_err_enable */

int32_t axi_adrv9102_ssi_intr_frm_err_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_POS, AXI_ADRV9102_SSI_INTR_FRM_ERR_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_intr_mon_oos_enable */

int32_t axi_adrv9102_ssi_intr_mon_oos_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_POS, AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_mon_oos_enable */

int32_t axi_adrv9102_ssi_intr_mon_oos_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_POS, AXI_ADRV9102_SSI_INTR_MON_OOS_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_intr_mon_err_enable */

int32_t axi_adrv9102_ssi_intr_mon_err_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_POS, AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_mon_err_enable */

int32_t axi_adrv9102_ssi_intr_mon_err_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_POS, AXI_ADRV9102_SSI_INTR_MON_ERR_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_intr_lb_ovf_enable */

int32_t axi_adrv9102_ssi_intr_lb_ovf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_POS, AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_lb_ovf_enable */

int32_t axi_adrv9102_ssi_intr_lb_ovf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_POS, AXI_ADRV9102_SSI_INTR_LB_OVF_ENABLE_MASK, data));
}


/*  register field read, axi_adrv9102_ssi_intr_lb_unf_enable */

int32_t axi_adrv9102_ssi_intr_lb_unf_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t *data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_read(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_POS, AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_MASK, data));
}


/*  register field write, axi_adrv9102_ssi_intr_lb_unf_enable */

int32_t axi_adrv9102_ssi_intr_lb_unf_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint32_t module_id, uint32_t data) {

    uint32_t addr_offset;

    addr_offset = 0x0;
    addr_offset = addr_offset + module_id;

    return(axi_utils_field_write(a_device, peripheral_id, (AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_ADDR + addr_offset), AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_POS, AXI_ADRV9102_SSI_INTR_LB_UNF_ENABLE_MASK, data));
}

// **********************************************************************************
// **********************************************************************************

