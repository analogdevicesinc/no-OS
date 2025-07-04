/*!
 * \brief     Private BMEM API implementations
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_BMEM
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_private_bmem.h"
#include "adi_apollo_private_blk_sel_types.h"
#include "adi_apollo_bf_rx_bmem.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

#define TIMEOUT_US 100000
#define POLL_DELAY_US 10

/*==================== L O C A L   F U N C T I O N S ====================*/
typedef uint32_t(*calc_bmem_base_f)(int32_t bmem_index);

static uint32_t calc_bmem_hsdin_base(int32_t bmem_index);
static uint32_t calc_bmem_fddc_base(int32_t bmem_index);
static uint32_t calc_bmem_cddc_base(int32_t bmem_index);
static uint32_t calc_hsdin_bmem_sram_base(int32_t bmem_index);
static uint32_t calc_fine_bmem_sram_base(int32_t bmem_index);
static uint32_t calc_coarse_bmem_sram_base(int32_t bmem_index);
static uint32_t calc_hsdin_bmem_sram_base(int32_t bmem_index);

static calc_bmem_base_f calc_bmem_base_func(adi_apollo_bmem_loc_e bmem_loc);
static calc_bmem_base_f calc_sram_base_func(adi_apollo_bmem_loc_e bmem_loc);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_private_bmem_delay_sample_set(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, uint16_t sample_delay)
{
    int32_t err = API_CMS_ERROR_OK;
    int32_t i;
    uint32_t regmap_base_addr;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Set sample delay 
            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_DLY_INFO(regmap_base_addr), sample_delay);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_private_bmem_delay_hop_set(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, uint16_t sample_delay[], uint32_t sample_delay_length)
{
    int32_t err = API_CMS_ERROR_OK;
    int32_t i;
    uint32_t regmap_base_addr;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(sample_delay);
    ADI_CMS_INVALID_PARAM_CHECK(sample_delay_length != ADI_APOLLO_BMEM_HOP_PROFILES);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Set hop delays
            err = adi_apollo_hal_bf_set(device, BF_HOP_DELAY0_INFO(regmap_base_addr), sample_delay[0]);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_DELAY1_INFO(regmap_base_addr), sample_delay[1]);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_DELAY2_INFO(regmap_base_addr), sample_delay[2]);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_DELAY3_INFO(regmap_base_addr), sample_delay[3]);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return err;
}

int32_t adi_apollo_private_bmem_delay_sample_config(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_sample_t *config)
{
    int32_t err;
    int32_t i;
    uint32_t regmap_base_addr;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Enable BMEM
            err = adi_apollo_hal_bf_set(device, BF_BMEM_EN_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Set delay mode
            err = adi_apollo_hal_bf_set(device, BF_BMEM_MODE_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_SIZE_INFO(regmap_base_addr), config->sample_size);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_RAMCLK_PH_DIS_INFO(regmap_base_addr), config->ramclk_ph_dis);
            ADI_CMS_ERROR_RETURN(err);

            // Disable triggers
            err = adi_apollo_hal_bf_set(device, BF_TRIG_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_HOP_DLY_SEL_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            
            // Set parity check
            err = adi_apollo_hal_bf_set(device, BF_PARITY_CHECK_EN_INFO(regmap_base_addr), config->parity_check_en);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    err = adi_apollo_private_bmem_delay_sample_set(device, bmem_loc, bmems, config->sample_delay);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_delay_hop_config(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_delay_hop_t *config)
{
    int32_t err;
    int32_t i;
    uint32_t regmap_base_addr;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Enable BMEM
            err = adi_apollo_hal_bf_set(device, BF_BMEM_EN_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Set delay mode
            err = adi_apollo_hal_bf_set(device, BF_BMEM_MODE_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_SIZE_INFO(regmap_base_addr), config->sample_size);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_RAMCLK_PH_DIS_INFO(regmap_base_addr), config->ramclk_ph_dis);
            ADI_CMS_ERROR_RETURN(err);

            // Enable triggers
            err = adi_apollo_hal_bf_set(device, BF_TRIG_MODE_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Set hop delay select mode
            err = adi_apollo_hal_bf_set(device, BF_HOP_DLY_SEL_MODE_INFO(regmap_base_addr), config->hop_delay_sel_mode);
            ADI_CMS_ERROR_RETURN(err);

            // Set trigger mode self clear
            err = adi_apollo_hal_bf_set(device, BF_TRIG_MODE_SCLR_EN_INFO(regmap_base_addr), config->trig_mode_sclr_en);
            ADI_CMS_ERROR_RETURN(err);
            
            // Set parity check
            err = adi_apollo_hal_bf_set(device, BF_PARITY_CHECK_EN_INFO(regmap_base_addr), config->parity_check_en);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    err = adi_apollo_private_bmem_delay_hop_set(device, bmem_loc, bmems, config->hop_delay, ADI_APOLLO_BMEM_HOP_PROFILES);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_delay_start(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Issue start signal for pushing data in loop
            err = adi_apollo_hal_bf_set(device, BF_BMEM_START_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_wait_to_clear(device, BF_BMEM_START_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_capture_config(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_capture_t *config)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Enable BMEM
            err = adi_apollo_hal_bf_set(device, BF_BMEM_EN_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Set capture mode
            err = adi_apollo_hal_bf_set(device, BF_BMEM_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_SIZE_INFO(regmap_base_addr), config->sample_size);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_RAMCLK_PH_DIS_INFO(regmap_base_addr), config->ramclk_ph_dis);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_ST_ADDR_CPT_INFO(regmap_base_addr), config->st_addr_cpt);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_END_ADDR_CPT_INFO(regmap_base_addr), config->end_addr_cpt);
            ADI_CMS_ERROR_RETURN(err);

            // Set mask value
            err = adi_apollo_hal_bf_set(device, BF_BMEM_8T8R_CAP_MASK_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_PARITY_CHECK_EN_INFO(regmap_base_addr), config->parity_check_en);
            ADI_CMS_ERROR_RETURN(err);

            // Set trigger mode
            err = adi_apollo_hal_bf_set(device, BF_TRIG_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            // Disable sample threshold capture mode
            err = adi_apollo_hal_bf_set(device, BF_ST_CPTR_ON_SMPL_VAL_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_capture_run(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Issue BMEM Reset before every capture
            err = adi_apollo_hal_bf_set(device, BF_BMEM_RESET_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_wait_to_clear(device, BF_BMEM_RESET_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);

            // Set SRAM access to converter - Fast
            err = adi_apollo_hal_bf_set(device, BF_FAST_NSLOW_MODE_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Wait at least 4 i_HCLK and 4 (i_refclk/i_clk_div) when fast_nslow_mode switched
            err = adi_apollo_hal_delay_us(device, 10000);
            ADI_CMS_ERROR_RETURN(err);

            // Issue start signal for normal capture
            err = adi_apollo_hal_bf_set(device, BF_BMEM_START_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_wait_to_clear(device, BF_BMEM_START_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);

            // Wait for Capture to complete and RAM to fill
            err = adi_apollo_hal_bf_wait_to_set(device, BF_FULL_IRQ_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);

            // Set SRAM access to uP/SPI/HSCI - Slow
            err = adi_apollo_hal_bf_set(device, BF_FAST_NSLOW_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            // Delay after switching fast_nslow_mode. Before reading the memory
            err = adi_apollo_hal_delay_us(device, 10000);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_capture_get(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_sram_base;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data);
    ADI_CMS_SINGLE_SELECT_CHECK(bmems);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    calc_sram_base = calc_sram_base_func(bmem_loc);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        // verify we're only requesting one bmem
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_sram_base(i);

            // Read data from SRAM
            err = adi_apollo_hal_stream_reg32_get(device, regmap_base_addr, data, length, 0);
            ADI_CMS_ERROR_RETURN(err);

            return API_CMS_ERROR_OK;
        }
    }

    return API_CMS_ERROR_INVALID_PARAM;
}

int32_t adi_apollo_private_bmem_awg_config(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, adi_apollo_bmem_awg_t *config)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(config);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Enable BMEM
            err = adi_apollo_hal_bf_set(device, BF_BMEM_EN_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            // Set AWG mode
            err = adi_apollo_hal_bf_set(device, BF_BMEM_MODE_INFO(regmap_base_addr), 2);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_SAMPLE_SIZE_INFO(regmap_base_addr), config->sample_size);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_RAMCLK_PH_DIS_INFO(regmap_base_addr), config->ramclk_ph_dis);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_PARITY_CHECK_EN_INFO(regmap_base_addr), config->parity_check_en);
            ADI_CMS_ERROR_RETURN(err);

            // Disable trigger mode
            err = adi_apollo_hal_bf_set(device, BF_TRIG_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_awg_start(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Set SRAM access to converter - Fast
            err = adi_apollo_hal_bf_set(device, BF_FAST_NSLOW_MODE_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    // Wait at least 4 i_HCLK and 4 (i_refclk/i_clk_div) when fast_nslow_mode switched
    err = adi_apollo_hal_delay_us(device, 10000);
    ADI_CMS_ERROR_RETURN(err);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Issue start signal for pushing data in loop
            err = adi_apollo_hal_bf_set(device, BF_BMEM_START_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            err = adi_apollo_hal_bf_wait_to_clear(device, BF_BMEM_START_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_awg_stop(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    calc_bmem_base_f calc_bmem_base;
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);

            // Issue BMEM Reset for Multiple AWGs
            err = adi_apollo_hal_bf_set(device, BF_BMEM_RESET_INFO(regmap_base_addr), 1);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_wait_to_clear(device, BF_BMEM_RESET_INFO(regmap_base_addr) , TIMEOUT_US, POLL_DELAY_US);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_awg_sram_set(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, uint32_t data[], uint32_t length)
{
    int32_t err;
    uint8_t i;
    uint32_t regmap_base_addr = 0;
    adi_apollo_hal_protocol_e ap;
    calc_bmem_base_f calc_sram_base, calc_bmem_base;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    /* Only SPI transactions can be used for writing BMEM AWG */
    err = adi_apollo_hal_active_protocol_get(device, &ap);
    ADI_CMS_ERROR_RETURN(err);
    
    if (ap != ADI_APOLLO_HAL_PROTOCOL_SPI0) {
        ADI_CMS_ERROR_RETURN(API_CMS_ERROR_PROTOCOL_OP_NOT_SUPPORTED);
    }

    calc_sram_base = calc_sram_base_func(bmem_loc);
    calc_bmem_base = calc_bmem_base_func(bmem_loc);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_bmem_base(i);
            // Set SRAM access to uP - slow
            err = adi_apollo_hal_bf_set(device, BF_FAST_NSLOW_MODE_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_ST_ADDR_AWG_INFO(regmap_base_addr), 0);
            ADI_CMS_ERROR_RETURN(err);

            err = adi_apollo_hal_bf_set(device, BF_END_ADDR_AWG_INFO(regmap_base_addr), length - 1);
            ADI_CMS_ERROR_RETURN(err);

        }
    }

    // Wait at least 4 i_HCLK and 4 (i_refclk/i_clk_div) when fast_nslow_mode switched
    err = adi_apollo_hal_delay_us(device, 10000);
    ADI_CMS_ERROR_RETURN(err);

    for (i = 0; i < ADI_APOLLO_BMEM_NUM; i++) {
        if ((bmems & (ADI_APOLLO_BMEM_A0 << i)) > 0) {
            regmap_base_addr = calc_sram_base(i);

            // Write data to sram
            err = adi_apollo_hal_stream_reg32_set(device, regmap_base_addr, data, length, 0);
            ADI_CMS_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_awg_sample_write(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems, int16_t data16[], uint32_t data16_len)
{
    int32_t err;
    uint32_t vec32_len = data16_len/2;
    uint32_t vec32[vec32_len];
    uint32_t s0, s1;
    int32_t i;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data16);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    /* Pack 2 16-bit samples into 32-bit word for BMEM write */
    for (i = 0; i < data16_len; i += 2)
    {
        s0 = ((uint32_t)data16[i + 0] <<4 ) & 0xffff;      // Left justify 12-bit to data. Lower 4 bits must be 0 per design.
        s1 = ((uint32_t)data16[i + 1] <<4 ) & 0xffff;
        vec32[i / 2] = s0 | s1 << 16;
    }

    err = adi_apollo_private_bmem_awg_sram_set(device, bmem_loc, bmems, vec32, vec32_len);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_private_bmem_awg_sample_shared_write(adi_apollo_device_t *device, adi_apollo_bmem_loc_e bmem_loc, adi_apollo_blk_sel_t bmems,
                                             int16_t data16_0[], int16_t data16_1[], uint32_t data16_len,
                                             uint32_t scratch32[], uint32_t scratch32_len)
{
    int32_t err;
    uint32_t vec32_len = data16_len;
    uint32_t *vec32 = scratch32;
    uint32_t s0, s1;
    int32_t i;

    ADI_CMS_NULL_PTR_CHECK(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_NULL_PTR_CHECK(data16_0);
    ADI_CMS_NULL_PTR_CHECK(data16_1);
    ADI_CMS_NULL_PTR_CHECK(scratch32);
    ADI_CMS_INVALID_PARAM_CHECK(data16_len > (ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_BYTES / (2 * sizeof(uint16_t))));
    ADI_CMS_INVALID_PARAM_CHECK(scratch32_len < data16_len);
    ADI_APOLLO_BMEM_BLK_SEL_MASK(bmems);

    /* Pack 2 16-bit samples into 32-bit word for BMEM write */
    for (i = 0; i < data16_len; i += 2) {
        s0 = ((uint32_t)data16_0[i + 0] << 4) & 0xffff; // Left justify 12-bit to data. Lower 4 bits must be 0 per design.
        s1 = ((uint32_t)data16_0[i + 1] << 4) & 0xffff;
        vec32[i] = s0 | s1 << 16;

        s0 = ((uint32_t)data16_1[i + 0] << 4) & 0xffff; // Left justify 12-bit to data. Lower 4 bits must be 0 per design.
        s1 = ((uint32_t)data16_1[i + 1] << 4) & 0xffff;
        vec32[i + 1] = s0 | s1 << 16;
    }

    err = adi_apollo_private_bmem_awg_sram_set(device, bmem_loc, bmems, vec32, vec32_len);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static uint32_t calc_bmem_hsdin_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        RX_BMEM0_REG_RX_SLICE_0_RX_DIGITAL0, RX_BMEM0_REG_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM0_REG_RX_SLICE_0_RX_DIGITAL0, RX_BMEM0_REG_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM0_REG_RX_SLICE_0_RX_DIGITAL1, RX_BMEM0_REG_RX_SLICE_1_RX_DIGITAL1,
        RX_BMEM0_REG_RX_SLICE_0_RX_DIGITAL1, RX_BMEM0_REG_RX_SLICE_1_RX_DIGITAL1
    };

    return bmem_regmap[bmem_index];
}

static uint32_t calc_bmem_fddc_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        RX_BMEM2_REG0_RX_SLICE_0_RX_DIGITAL0, RX_BMEM2_REG0_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM2_REG0_RX_SLICE_0_RX_DIGITAL0, RX_BMEM2_REG0_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM2_REG0_RX_SLICE_0_RX_DIGITAL1, RX_BMEM2_REG0_RX_SLICE_1_RX_DIGITAL1,
        RX_BMEM2_REG0_RX_SLICE_0_RX_DIGITAL1, RX_BMEM2_REG0_RX_SLICE_1_RX_DIGITAL1
    };

    return bmem_regmap[bmem_index];
}

static uint32_t calc_bmem_cddc_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        RX_BMEM1_REG0_RX_SLICE_0_RX_DIGITAL0, RX_BMEM1_REG0_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM1_REG0_RX_SLICE_0_RX_DIGITAL0, RX_BMEM1_REG0_RX_SLICE_1_RX_DIGITAL0,
        RX_BMEM1_REG0_RX_SLICE_0_RX_DIGITAL1, RX_BMEM1_REG0_RX_SLICE_1_RX_DIGITAL1,
        RX_BMEM1_REG0_RX_SLICE_0_RX_DIGITAL1, RX_BMEM1_REG0_RX_SLICE_1_RX_DIGITAL1
    };

    return bmem_regmap[bmem_index];
}

static uint32_t calc_hsdin_bmem_sram_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        0x60240000, 0x60440000,
        0x60240000, 0x60440000,
        0x60A40000, 0x60C40000,
        0x60A40000, 0x60C40000
    };

    return bmem_regmap[bmem_index];
}

static uint32_t calc_fine_bmem_sram_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        0x603C0000, 0x605C0000,
        0x603C0000, 0x605C0000,
        0x60BC0000, 0x60DC0000,
        0x60BC0000, 0x60DC0000
    };

    return bmem_regmap[bmem_index];
}

static uint32_t calc_coarse_bmem_sram_base(int32_t bmem_index)
{
    static uint32_t bmem_regmap[ADI_APOLLO_BMEM_NUM] = {
        0x60380000, 0x60580000,
        0x60380000, 0x60580000,
        0x60B80000, 0x60D80000,
        0x60B80000, 0x60D80000
    };

    return bmem_regmap[bmem_index];
}

static calc_bmem_base_f calc_bmem_base_func(adi_apollo_bmem_loc_e bmem_loc)
{
    static calc_bmem_base_f bmem_base_func[3] = {
        &calc_bmem_hsdin_base, &calc_bmem_cddc_base, &calc_bmem_fddc_base
    };
    return bmem_base_func[bmem_loc];
}

static calc_bmem_base_f calc_sram_base_func(adi_apollo_bmem_loc_e bmem_loc)
{
    static calc_bmem_base_f bmem_base_func[3] = {
        &calc_hsdin_bmem_sram_base, &calc_coarse_bmem_sram_base, &calc_fine_bmem_sram_base
    };
    return bmem_base_func[bmem_loc];
}
