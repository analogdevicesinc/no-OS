/*!
 * \brief     APIs for GPIO
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_GPIO
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_bf_core.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_gpio.h"

static uint32_t calc_sync_pad_ctl_addr(int32_t index);
static uint32_t calc_gpio_stage_sel_byte_address(uint8_t gpio_index);
static uint32_t calc_bf_info_gpio_stage_sel(uint8_t gpio_index);

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_gpio_cmos_debug_mode_set(adi_apollo_device_t *device, uint8_t gpio_index,
                                            adi_apollo_gpio_cmos_debug_stage_e debug_stage)
{
    int32_t err;
    uint32_t gpio_stage_sel_addr = 0;
    uint8_t lvds_pad;
    uint32_t bf_info;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);

    if (gpio_index > ADI_APOLLO_NUM_CMOS_GPIO - 1) {
        // Config SYNC PAD to CMOS mode
        lvds_pad = (gpio_index % 2 == 1) ? gpio_index : gpio_index - 1;
        err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, lvds_pad, ADI_APOLLO_CMOS_MODE);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    gpio_stage_sel_addr = calc_gpio_stage_sel_byte_address(gpio_index);
    bf_info = calc_bf_info_gpio_stage_sel(gpio_index);

    err = adi_apollo_hal_bf_set(device, gpio_stage_sel_addr, bf_info, debug_stage);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_cmos_func_mode_set(adi_apollo_device_t *device, uint8_t gpio_index,
                                           adi_apollo_gpio_func_e func_num)
{
    int32_t err;
    uint32_t gpio_stage_sel_addr = 0;
    uint8_t lvds_pad;
    uint32_t bf_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);

    if (gpio_index > ADI_APOLLO_NUM_CMOS_GPIO - 1) {
        // Config SYNC PAD to CMOS mode
        lvds_pad = (gpio_index % 2 == 1) ? gpio_index : gpio_index - 1;
        err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, lvds_pad, ADI_APOLLO_CMOS_MODE);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    gpio_stage_sel_addr = calc_gpio_stage_sel_byte_address(gpio_index);
    bf_info = calc_bf_info_gpio_stage_sel(gpio_index);

    err = adi_apollo_hal_bf_set(device, gpio_stage_sel_addr, bf_info, 0); // GPIO_STAGE_SEL = 2’b00 : Functional Stage
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_GPIO_SOURCE_CONTROL_INFO(gpio_index), func_num);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_cmos_gpio_mode_set(adi_apollo_device_t *device, uint8_t gpio_index, adi_apollo_gpio_dir_e gpio_dir)
{
    int32_t err;
    uint32_t gpio_mode_ctrl_byte_addr = 0;
    uint32_t gpio_dir_addr = 0;
    uint8_t  bf_start, lvds_pad, gpio_ctrl_byte;
    uint32_t bf_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);

    // calc mode_ctrl_byte and mode_dir byte addr
    gpio_ctrl_byte = gpio_index / 8;
    bf_start = gpio_index % 8;
    gpio_mode_ctrl_byte_addr = REG_GPIO_MODE_CONTROL_BYTE0_ADDR + gpio_ctrl_byte;
    gpio_dir_addr = REG_GPIO_MODE_DIR_BYTE0_ADDR + gpio_ctrl_byte;
    bf_info = bf_start | (1<< 8); //1 bit wide BF

    if (gpio_index > ADI_APOLLO_NUM_CMOS_GPIO - 1) {
        // Config SYNC PAD to CMOS mode
        lvds_pad = (gpio_index % 2 == 1) ? gpio_index : gpio_index - 1;
        err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, lvds_pad, ADI_APOLLO_CMOS_MODE);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    err = adi_apollo_hal_bf_set(device, gpio_mode_ctrl_byte_addr, bf_info, 1); // GPIO MODE EN
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, gpio_dir_addr, bf_info, gpio_dir); // GPIO Dir
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_cmos_output_set(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t gpio_data)
{
    int32_t err;
    uint32_t gpio_from_master_byte_addr = 0;
    uint8_t bf_start, gpio_from_master_byte;
    uint32_t bf_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_data > 1);

    gpio_from_master_byte = gpio_index / 8;
    bf_start = gpio_index % 8;
    bf_info = bf_start | (1<< 8); //1 bit wide BF

    gpio_from_master_byte_addr = REG_GPIO_FROM_MASTER_BYTE0_ADDR + gpio_from_master_byte;

    err = adi_apollo_hal_bf_set(device, gpio_from_master_byte_addr, bf_info, gpio_data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_cmos_input_get(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t *gpio_data)
{
    int32_t err;
    uint32_t spi_read_byte_addr = 0;
    uint8_t bf_start, spi_read_byte;
    uint32_t bf_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(gpio_data);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);

    // Enable Schmitt trigger.
    err = adi_apollo_gpio_cmos_st_enable(device, gpio_index, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    spi_read_byte = gpio_index / 8;
    bf_start = gpio_index % 8;
    bf_info = bf_start | (1<< 8); //1 bit wide BF

    spi_read_byte_addr = REG_GPIO_SPI_READ_BYTE0_ADDR + spi_read_byte;

    err = adi_apollo_hal_bf_get(device, spi_read_byte_addr, bf_info, gpio_data, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_quick_config_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e profile)
{
    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    /* set stage_sel of GPIO's to functional mode */
    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE0_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE1_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE2_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE3_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE4_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE5_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE6_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, REG_GPIO_STAGE_SEL_BYTE7_ADDR, 0x00);
    ADI_APOLLO_ERROR_RETURN(err);

    /*Quick Config Profile*/
    err = adi_apollo_hal_bf_set(device, BF_GPIO_QUICK_CONFIG_INFO, profile);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_sync_pad_lvds_debug_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad,
                                                     adi_apollo_gpio_debug_func_e debug_func_num)
{
    int32_t err;
    uint32_t sync_pad_ctl_addr = 0;
    uint32_t gpio_stage_sel_info = BF_INFO_EXTRACT(BF_SYNCOUTB0_A_GPIO_STAGE_SEL_INFO);
    uint32_t debug_source_sel_info = BF_INFO_EXTRACT(BF_SYNCOUTB0_A_GPIO_DEBUG_SOURCE_SEL_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    // Debug mode-Only valid for SYNC_OUT Pads
    ADI_APOLLO_INVALID_PARAM_RETURN(sync_pad < ADI_APOLLO_SYNCOUTB0_B);

    // Configure pad in LVDS mode
    err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, sync_pad, ADI_APOLLO_LVDS_MODE);
    ADI_APOLLO_ERROR_RETURN(err);

    sync_pad_ctl_addr = calc_sync_pad_ctl_addr(sync_pad);

    // gpio_stage_sel
    err = adi_apollo_hal_bf_set(device, sync_pad_ctl_addr + 1, gpio_stage_sel_info, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Assign debug function to SYNC PAD
    err = adi_apollo_hal_bf_set(device, sync_pad_ctl_addr + 1, debug_source_sel_info,  debug_func_num);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_sync_pad_lvds_func_mode_set(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad,
                                                    adi_apollo_gpio_func_e func_num)
{
    int32_t err;
    uint32_t sync_pad_ctl_addr = 0;
    uint32_t gpio_stage_sel_info = BF_INFO_EXTRACT(BF_SYNCOUTB0_A_GPIO_STAGE_SEL_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    // Configure pad in LVDS mode
    err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, sync_pad, ADI_APOLLO_LVDS_MODE);
    ADI_APOLLO_ERROR_RETURN(err);

    sync_pad_ctl_addr = calc_sync_pad_ctl_addr(sync_pad);

    if (sync_pad >= ADI_APOLLO_SYNCOUTB0_B) {
        // gpio_stage_sel to func mode
        err = adi_apollo_hal_bf_set(device, sync_pad_ctl_addr + 1, gpio_stage_sel_info, 0);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    // Assign function number to SYNC PAD
    err = adi_apollo_hal_bf_set(device, BF_GPIO_SOURCE_CONTROL_INFO(sync_pad), func_num);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(adi_apollo_device_t *device, uint8_t pad_index,
                                                    adi_apollo_gpio_sync_pad_mode_e mode)
{
    int32_t err;
    uint32_t sync_pad_ctrl_addr = 0;
    uint32_t lvds_sel_info = BF_INFO_EXTRACT(BF_SYNCINB0_A_LVDS_SEL_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(pad_index < ADI_APOLLO_SYNCINB0_B);

    sync_pad_ctrl_addr = calc_sync_pad_ctl_addr(pad_index);
    err = adi_apollo_hal_bf_set(device, sync_pad_ctrl_addr, lvds_sel_info, mode);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_sync_pad_lvds_input_get(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad,
                                                uint8_t *pad_data)
{
    int32_t err;
    uint32_t sync_pad_ctrl_addr = 0;
    uint32_t spi_read_info = BF_INFO_EXTRACT(BF_SYNCINB0_A_SPI_READ_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(pad_data);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(sync_pad > ADI_APOLLO_SYNCINB1_A); //Valid only for Sync in Pads

    sync_pad_ctrl_addr = calc_sync_pad_ctl_addr(sync_pad);
    err = adi_apollo_hal_bf_get(device, sync_pad_ctrl_addr, spi_read_info, pad_data, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_cmos_st_enable(adi_apollo_device_t *device, uint8_t gpio_index, uint8_t st_enable)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint32_t cmos_pad_st_addr = ( REG_CMOS_PAD_ST_BYTE0_ADDR + (gpio_index / 8) );
    uint32_t st_byte_info = ( (1 << 8) | (gpio_index % 8) );

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(gpio_index > ADI_APOLLO_NUM_GPIO - 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(st_enable > 1);

    err = adi_apollo_hal_bf_set(device, cmos_pad_st_addr, st_byte_info, st_enable);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_syncin_pad_termination_enable(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e syncin_pad, uint8_t termination_enable)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint32_t sync_pad_ctrl_addr = 0;
    uint32_t term_en_info = BF_INFO_EXTRACT(BF_SYNCINB0_A_TERM_EN_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(termination_enable > 1);

    if ( (syncin_pad < ADI_APOLLO_SYNCINB0_B) || (syncin_pad > ADI_APOLLO_SYNCINB1_A) )
        return API_CMS_ERROR_INVALID_PARAM;

    sync_pad_ctrl_addr = calc_sync_pad_ctl_addr(syncin_pad);

    err = adi_apollo_hal_bf_set(device, sync_pad_ctrl_addr, term_en_info, termination_enable);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_sync_pad_lvds_enable(adi_apollo_device_t *device, adi_apollo_gpio_sync_pad_e sync_pad, uint8_t enable)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint32_t sync_pad_ctrl_addr = 0;
    uint32_t en_info = BF_INFO_EXTRACT(BF_SYNCINB0_A_PAD_EN_INFO);

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(sync_pad > ADI_APOLLO_SYNCOUTB1_A);
    ADI_APOLLO_INVALID_PARAM_RETURN(enable > 1);

    sync_pad_ctrl_addr = calc_sync_pad_ctl_addr(sync_pad);

    err = adi_apollo_hal_bf_set(device, sync_pad_ctrl_addr, en_info, enable);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_jesd_204b_configure(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_ERROR;
    uint32_t i;
    uint8_t pads[8] = {ADI_APOLLO_SYNCINB0_B,ADI_APOLLO_SYNCINB0_A,ADI_APOLLO_SYNCOUTB0_B,ADI_APOLLO_SYNCOUTB0_A,ADI_APOLLO_SYNCINB1_B,ADI_APOLLO_SYNCINB1_A,ADI_APOLLO_SYNCOUTB1_B,ADI_APOLLO_SYNCOUTB1_A};

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < 8; i++) {
        err = adi_apollo_gpio_sync_pad_cmos_lvds_mode_set(device, pads[i], ADI_APOLLO_LVDS_MODE);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_gpio_sync_pad_lvds_enable(device, pads[i], 1);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

static uint32_t calc_sync_pad_ctl_addr(int32_t index)
{
    static uint32_t lvds_pad_ctrl_addr[] = {
        REG_SYNCINB0_B_CNTRL_ADDR, REG_SYNCINB1_B_CNTRL_ADDR, REG_SYNCINB0_A_CNTRL_ADDR,
        REG_SYNCINB1_A_CNTRL_ADDR, REG_SYNCOUTB0_B_CNTRL_BYTE0_ADDR, REG_SYNCOUTB1_B_CNTRL_BYTE0_ADDR,
        REG_SYNCOUTB0_A_CNTRL_BYTE0_ADDR, REG_SYNCOUTB1_A_CNTRL_BYTE0_ADDR
    };

    return (lvds_pad_ctrl_addr[(index % ADI_APOLLO_SYNCINB0_B) / 2]);
}

static uint32_t calc_gpio_stage_sel_byte_address(uint8_t gpio_index)
{
    uint8_t bf_start_bit, stage_sel_byte;
    // 102 bit wide bitfield split into 13 reg.
    bf_start_bit   = 2 *gpio_index;      // 2*i : 2*i +1
    stage_sel_byte = bf_start_bit / 8;

    return REG_GPIO_STAGE_SEL_BYTE0_ADDR + stage_sel_byte;
}

static uint32_t calc_bf_info_gpio_stage_sel(uint8_t gpio_index)
{
    uint8_t bf_start_bit, bf_start_byte;
    uint32_t bf_info;
    // 102 bit wide bitfield split into 13 reg.
    bf_start_bit   = 2 *gpio_index;      // 2*i : 2*i +1
    bf_start_byte  = bf_start_bit % 8;
    bf_info = bf_start_byte | (2 << 8); //2 bit wide BF

    return bf_info;
}
/*! @} */
