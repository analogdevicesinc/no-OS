/*!
 * \brief     APIs for GPIO select
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_GPIO_HOP
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_gpio_hop.h"
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_bf_apollo_profile_config.h"
#include "adi_apollo_bf_custom.h"

static uint32_t calc_profile_func_mode(int32_t index);
static uint32_t calc_block_func_mode(int32_t index);
static uint32_t calc_slice_func_mode(int32_t index);
static uint32_t calc_term_func_mode(int32_t index);

const adi_apollo_gpio_hop_profile_t * calc_profile_qc(int32_t index);
const adi_apollo_gpio_hop_block_t * calc_block_qc(int32_t index);
const adi_apollo_gpio_hop_side_t * calc_side_qc(int32_t index);
const adi_apollo_gpio_hop_slice_t * calc_slice_qc(int32_t index);
const adi_apollo_gpio_hop_terminal_t * calc_term_qc(int32_t index);

// Quick config mapping - several quick configs have overlaps indicated by the variable names
const adi_apollo_gpio_hop_profile_t ADI_APOLLO_GPIO_HOP_PROFILE_1238 = {{26, 27, 28, 29, 30}};
const adi_apollo_gpio_hop_profile_t ADI_APOLLO_GPIO_HOP_PROFILE_45 = {{24, 25, 26, 27, 28}};
const adi_apollo_gpio_hop_profile_t ADI_APOLLO_GPIO_HOP_PROFILE_6 = {{16, 17, 18, 19, 20}};

const adi_apollo_gpio_hop_block_t ADI_APOLLO_GPIO_HOP_BLOCK_123 = {{23, 24, 25, 31}};
const adi_apollo_gpio_hop_block_t ADI_APOLLO_GPIO_HOP_BLOCK_45 = {{21, 22, 23, 31}};
const adi_apollo_gpio_hop_block_t ADI_APOLLO_GPIO_HOP_BLOCK_8 = {{23, 24, 25, -1}};

const adi_apollo_gpio_hop_side_t ADI_APOLLO_GPIO_HOP_SIDE_128 = {{22}};
const adi_apollo_gpio_hop_side_t ADI_APOLLO_GPIO_HOP_SIDE_45 = {{20}};

const adi_apollo_gpio_hop_slice_t ADI_APOLLO_GPIO_HOP_SLICE_128 = {{19, 20, 21}};
const adi_apollo_gpio_hop_slice_t ADI_APOLLO_GPIO_HOP_SLICE_45 = {{17, 18, 19}};

const adi_apollo_gpio_hop_terminal_t ADI_APOLLO_GPIO_HOP_TERMINAL_128 = {{17, 18}};
const adi_apollo_gpio_hop_terminal_t ADI_APOLLO_GPIO_HOP_TERMINAL_3 = {{21, 22}};
const adi_apollo_gpio_hop_terminal_t ADI_APOLLO_GPIO_HOP_TERMINAL_45 = {{15, 16}};
const adi_apollo_gpio_hop_terminal_t ADI_APOLLO_GPIO_HOP_TERMINAL_6 = {{16, 17}};

/*==================== P U B L I C   A P I   C O D E ====================*/
int32_t adi_apollo_gpio_hop_profile_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_profile_t *config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_PROFILE_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            err = adi_apollo_gpio_cmos_func_mode_set(device, config->index[i], calc_profile_func_mode(i));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return err;
}

int32_t adi_apollo_gpio_hop_profile_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_profile_t *config, uint8_t profile, uint64_t *mask, uint64_t *value)
{
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(value);
    ADI_APOLLO_NULL_POINTER_RETURN(mask);

    *mask = (uint64_t)0;
    *value = (uint64_t)0;

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_PROFILE_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            *mask |= ((uint64_t)1 << config->index[i]);
            if ((profile & (1 << i)) > 0) {
                *value |= ((uint64_t)1 << config->index[i]);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_profile_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, uint8_t number, uint64_t *mask, uint64_t *value)
{
    const adi_apollo_gpio_hop_profile_t *idx;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    idx = calc_profile_qc(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(idx == NULL);

    return adi_apollo_gpio_hop_profile_calc(device, idx, number, mask, value);
}


int32_t adi_apollo_gpio_hop_block_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_block_t *config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_BLOCK_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            err = adi_apollo_gpio_cmos_func_mode_set(device, config->index[i], calc_block_func_mode(i));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return err;
}

int32_t adi_apollo_gpio_hop_block_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_block_t *config, adi_apollo_gpio_hop_block_e block, uint64_t *mask, uint64_t *value)
{
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(value);
    ADI_APOLLO_NULL_POINTER_RETURN(mask);

    *mask = (uint64_t)0;
    *value = (uint64_t)0;

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_BLOCK_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            *mask |= ((uint64_t)1 << config->index[i]);
            if ((block & (1 << i)) > 0) {
                *value |= ((uint64_t)1 << config->index[i]);
            }
        }
    }

    return API_CMS_ERROR_OK;    
}

int32_t adi_apollo_gpio_hop_block_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_gpio_hop_block_e block, uint64_t *mask, uint64_t *value)
{
    const adi_apollo_gpio_hop_block_t *idx;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    idx = calc_block_qc(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(idx == NULL);

    return adi_apollo_gpio_hop_block_calc(device, idx, block, mask, value);
}

int32_t adi_apollo_gpio_hop_side_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_side_t *config) 
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    err = adi_apollo_gpio_cmos_func_mode_set(device, config->index[0], ADI_APOLLO_FUNC_PROFILE_TXRX_BA);

    return err;
}

int32_t adi_apollo_gpio_hop_side_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_side_t *config, adi_apollo_side_select_e side, uint64_t *mask, uint64_t *value)
{
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(value);
    ADI_APOLLO_NULL_POINTER_RETURN(mask);
    ADI_APOLLO_INVALID_PARAM_RETURN(side != ADI_APOLLO_SIDE_A && side != ADI_APOLLO_SIDE_B)

    *mask = (uint64_t)0;
    *value = (uint64_t)0;

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_SIDE_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            *mask |= ((uint64_t)1 << config->index[i]);
            if (side == ADI_APOLLO_SIDE_B) {
                *value |= ((uint64_t)1 << config->index[i]);
            }
        }
    }

    return API_CMS_ERROR_OK;   
}

int32_t adi_apollo_gpio_hop_side_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_side_select_e side, uint64_t *mask, uint64_t *value)
{
    const adi_apollo_gpio_hop_side_t *idx;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    idx = calc_side_qc(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(idx == NULL);

    return adi_apollo_gpio_hop_side_calc(device, idx, side, mask, value);
}


int32_t adi_apollo_gpio_hop_slice_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_slice_t *config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_SLICE_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            err = adi_apollo_gpio_cmos_func_mode_set(device, config->index[i], calc_slice_func_mode(i));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return err;
}

int32_t adi_apollo_gpio_hop_slice_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_slice_t *config, uint8_t slice, uint64_t *mask, uint64_t *value)
{
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(value);
    ADI_APOLLO_NULL_POINTER_RETURN(mask);

    *mask = (uint64_t)0;
    *value = (uint64_t)0;

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_SLICE_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            *mask |= ((uint64_t)1 << config->index[i]);
            if ((slice & (1 << i)) > 0) {
                *value |= ((uint64_t)1 << config->index[i]);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_slice_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, uint8_t slice, uint64_t *mask, uint64_t *value)
{
    const adi_apollo_gpio_hop_slice_t *idx;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    idx = calc_slice_qc(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(idx == NULL);

    return adi_apollo_gpio_hop_slice_calc(device, idx, slice, mask, value);
}

int32_t adi_apollo_gpio_hop_terminal_configure(adi_apollo_device_t *device, const adi_apollo_gpio_hop_terminal_t *config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_TERMINAL_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            err = adi_apollo_gpio_cmos_func_mode_set(device, config->index[i], calc_term_func_mode(i));
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return err;
}

int32_t adi_apollo_gpio_hop_terminal_calc(adi_apollo_device_t *device, const adi_apollo_gpio_hop_terminal_t *config, adi_apollo_gpio_hop_terminal_e terminal, uint64_t *mask, uint64_t *value)
{
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_NULL_POINTER_RETURN(value);
    ADI_APOLLO_NULL_POINTER_RETURN(mask);

    *mask = (uint64_t)0;
    *value = (uint64_t)0;

    for (i = 0; i < ADI_APOLLO_GPIO_HOP_TERMINAL_BIT_NUMBER; i++) {
        if (config->index[i] != ADI_APOLLO_GPIO_HOP_IDX_NONE) {
            *mask |= ((uint64_t)1 << config->index[i]);
            if ((terminal & (1 << i)) > 0) {
                *value |= ((uint64_t)1 << config->index[i]);
            }
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_terminal_qc_calc(adi_apollo_device_t *device, adi_apollo_gpio_quick_cfg_profile_e config, adi_apollo_gpio_hop_terminal_e terminal, uint64_t *mask, uint64_t *value)
{
    const adi_apollo_gpio_hop_terminal_t *idx;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    idx = calc_term_qc(config);
    ADI_APOLLO_INVALID_PARAM_RETURN(idx == NULL);

    return adi_apollo_gpio_hop_terminal_calc(device, idx, terminal, mask, value);
}

int32_t adi_apollo_gpio_hop_block_select_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_select_e select)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_FCN_SEL_SPI_GPIO_INFO, select);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_gpio_hop_block_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_block_e block)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_PROFILE_FCN_SEL_INFO, block);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_gpio_hop_slice_select_set(adi_apollo_device_t *device, adi_apollo_gpio_hop_block_e block, adi_apollo_gpio_hop_select_e select)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    switch (block) {
        case ADI_APOLLO_GPIO_BLOCK_FNCO:
            return adi_apollo_hal_bf_set(device, BF_FNCO_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_CNCO:
            return adi_apollo_hal_bf_set(device, BF_CNCO_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_FNCO_CNCO:
            return adi_apollo_hal_bf_set(device, BF_FNCO_CNCO_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_PFILT:
            return adi_apollo_hal_bf_set(device, BF_PFILT_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_CNCO_PFILT:
            return adi_apollo_hal_bf_set(device, BF_CNCO_PFILT_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_FNCO_CFIR:
            return adi_apollo_hal_bf_set(device, BF_FNCO_CFIR_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_FDDC:
        case ADI_APOLLO_GPIO_BLOCK_CDDC:
            return adi_apollo_hal_bf_set(device, BF_DYN_CONFIG_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_BMEM_DELAY:
            return adi_apollo_hal_bf_set(device, BF_BMEM_HOP_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_CFIR:
            return adi_apollo_hal_bf_set(device, BF_CFIR_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_FNCO_PFILT:
            return adi_apollo_hal_bf_set(device, BF_FNCO_PFILT_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_PFILT_CFIR:
            return adi_apollo_hal_bf_set(device, BF_PFILT_CFIR_SLICE_SELECT_SPI_GPIO_INFO, select);

        case ADI_APOLLO_GPIO_BLOCK_LINX:
            return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_INVALID_PARAM;
}

int32_t adi_apollo_gpio_hop_fnco_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t fncos, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_FNCO_SLICE_SELECT_REG_ADDR;
    uint32_t fnco, index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (index = 0;  index < ADI_APOLLO_FNCO_NUM; index++) {
        fnco = fncos & (ADI_APOLLO_FNCO_A0 << index);
        if (fnco > 0) {
            err = adi_apollo_hal_bf_set(device, regmap_base, terminal == ADI_APOLLO_RX ? BF_PC_RX_FNCO_SLICE_SELECT_INFO(index) : BF_PC_TX_FNCO_SLICE_SELECT_INFO(index), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_cnco_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t cncos, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_CNCO_SLICE_SELECT_REG_ADDR;
    uint32_t cnco, index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (index = 0;  index < ADI_APOLLO_CNCO_NUM; index++) {
        cnco = cncos & (ADI_APOLLO_CNCO_A0 << index);
        if (cnco > 0) {
            err = adi_apollo_hal_bf_set(device, regmap_base, terminal == ADI_APOLLO_RX ? BF_PC_RX_CNCO_SLICE_SELECT_INFO(index) : BF_PC_TX_CNCO_SLICE_SELECT_INFO(index), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_cfir_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t cfirs, uint32_t paths, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_CFIR_SLICE_SELECT_REG_ADDR;
    uint32_t cfir, path, i, j, index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0;  i < ADI_APOLLO_CFIR_NUM; i++) {
        cfir = cfirs & (ADI_APOLLO_CFIR_A0 << i);
        if (cfir > 0) {
            for (j = 0;  j < ADI_APOLLO_CFIR_DP_PER_INST_NUM; j++) {
                path = paths & (ADI_APOLLO_CFIR_DP_0 << i);
                if (path > 0) {
                    index = i * ADI_APOLLO_CFIR_DP_PER_INST_NUM + j;
                    err = adi_apollo_hal_bf_set(device, regmap_base, terminal == ADI_APOLLO_RX ? BF_PC_RX_CFIR_SLICE_SELECT_INFO(index) : BF_PC_TX_CFIR_SLICE_SELECT_INFO(index), enable);
                    ADI_APOLLO_ERROR_RETURN(err);
                }
            }
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_pfilt_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t sides, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_PFILT_SLICE_SELECT_REG_ADDR;
    uint32_t side, side_index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (side_index = 0;  side_index < ADI_APOLLO_NUM_SIDES; side_index++) {
        side = sides & (ADI_APOLLO_SIDE_A << side_index);
        if (side > 0) {
            err = adi_apollo_hal_bf_set(device, regmap_base, terminal == ADI_APOLLO_RX ? BF_PC_RX_PFILT_SLICE_SELECT_INFO(side_index) : BF_PC_TX_PFILT_SLICE_SELECT_INFO(side_index), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_dynamic_config_enable_set(adi_apollo_device_t *device, uint32_t terminal, uint32_t fdrcs, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_DYN_CONFIG_SLICE_SELECT_REG_ADDR;
    uint32_t fdrc, index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (index = 0;  index < ADI_APOLLO_CDUC_NUM; index++) {
        fdrc = fdrcs & (ADI_APOLLO_CDUC_A0 << index);
        if (fdrc > 0) {
            err = adi_apollo_hal_bf_set(device, regmap_base, terminal == ADI_APOLLO_RX ? BF_PC_RX_DR_SLICE_SELECT_INFO(index) : BF_PC_TX_DR_SLICE_SELECT_INFO(index), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_gpio_hop_bmem_delay_enable_set(adi_apollo_device_t *device, uint32_t bmems, uint8_t enable)
{
    int32_t err;
    int32_t regmap_base = REG_BMEM_HOP_SLICE_SELECT_REG_ADDR;
    uint32_t bmem, index;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (index = 0;  index < ADI_APOLLO_BMEM_NUM; index++) {
        bmem = bmems & (ADI_APOLLO_BMEM_A0 << index);
        if (bmem > 0) {
            err = adi_apollo_hal_bf_set(device, regmap_base, BF_PC_BMEM_HOP_SLICE_SELECT_INFO(index), enable);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }
    return API_CMS_ERROR_OK;
}


static uint32_t calc_profile_func_mode(int32_t index)
{
    static uint32_t func_mode[] = {
        ADI_APOLLO_FUNC_PROFILE_0, ADI_APOLLO_FUNC_PROFILE_1, ADI_APOLLO_FUNC_PROFILE_2,
        ADI_APOLLO_FUNC_PROFILE_3, ADI_APOLLO_FUNC_PROFILE_4
    };
    return (func_mode[index]);
}

static uint32_t calc_block_func_mode(int32_t index)
{
    static uint32_t func_mode[] = {
        ADI_APOLLO_FUNC_PROFILE_FCN_SEL_0, ADI_APOLLO_FUNC_PROFILE_FCN_SEL_1, ADI_APOLLO_FUNC_PROFILE_FCN_SEL_2, ADI_APOLLO_FUNC_PROFILE_FCN_SEL_3
    };
    return (func_mode[index]);
}

static uint32_t calc_slice_func_mode(int32_t index)
{
    static uint32_t func_mode[] = {
        ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_0, ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_1, ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_2
    };
    return (func_mode[index]);
}

static uint32_t calc_term_func_mode(int32_t index)
{
    static uint32_t func_mode[] = {
        ADI_APOLLO_FUNC_PROFILE_TX_RXN_0, ADI_APOLLO_FUNC_PROFILE_TX_RXN_1
    };
    return (func_mode[index]);
}

const adi_apollo_gpio_hop_profile_t * calc_profile_qc(int32_t index)
{
    static const adi_apollo_gpio_hop_profile_t *map[] = {
        NULL, &ADI_APOLLO_GPIO_HOP_PROFILE_1238, &ADI_APOLLO_GPIO_HOP_PROFILE_1238, &ADI_APOLLO_GPIO_HOP_PROFILE_1238, &ADI_APOLLO_GPIO_HOP_PROFILE_45,
        &ADI_APOLLO_GPIO_HOP_PROFILE_45, &ADI_APOLLO_GPIO_HOP_PROFILE_6, NULL, &ADI_APOLLO_GPIO_HOP_PROFILE_1238
    };
    return (map[index]);
}

const adi_apollo_gpio_hop_block_t * calc_block_qc(int32_t index)
{
    static const adi_apollo_gpio_hop_block_t *map[] = {
        NULL, &ADI_APOLLO_GPIO_HOP_BLOCK_123, &ADI_APOLLO_GPIO_HOP_BLOCK_123, &ADI_APOLLO_GPIO_HOP_BLOCK_123, &ADI_APOLLO_GPIO_HOP_BLOCK_45,
        &ADI_APOLLO_GPIO_HOP_BLOCK_45, NULL, NULL, &ADI_APOLLO_GPIO_HOP_BLOCK_8
    };
    return (map[index]);
}

const adi_apollo_gpio_hop_side_t * calc_side_qc(int32_t index)
{
    static const adi_apollo_gpio_hop_side_t *map[] = {
        NULL, &ADI_APOLLO_GPIO_HOP_SIDE_128, &ADI_APOLLO_GPIO_HOP_SIDE_128, NULL, &ADI_APOLLO_GPIO_HOP_SIDE_45,
        &ADI_APOLLO_GPIO_HOP_SIDE_45, NULL, NULL, &ADI_APOLLO_GPIO_HOP_SIDE_128
    };
    return (map[index]);
}

const adi_apollo_gpio_hop_slice_t * calc_slice_qc(int32_t index)
{
    static const adi_apollo_gpio_hop_slice_t *map[] = {
        NULL, &ADI_APOLLO_GPIO_HOP_SLICE_128, &ADI_APOLLO_GPIO_HOP_SLICE_128, NULL, &ADI_APOLLO_GPIO_HOP_SLICE_45,
        &ADI_APOLLO_GPIO_HOP_SLICE_45, NULL, NULL, &ADI_APOLLO_GPIO_HOP_SLICE_128
    };
    return (map[index]);
}

const adi_apollo_gpio_hop_terminal_t * calc_term_qc(int32_t index)
{
    static const adi_apollo_gpio_hop_terminal_t *map[] = {
        NULL, &ADI_APOLLO_GPIO_HOP_TERMINAL_128, &ADI_APOLLO_GPIO_HOP_TERMINAL_128, &ADI_APOLLO_GPIO_HOP_TERMINAL_3, &ADI_APOLLO_GPIO_HOP_TERMINAL_45,
        &ADI_APOLLO_GPIO_HOP_TERMINAL_45, &ADI_APOLLO_GPIO_HOP_TERMINAL_6, NULL, &ADI_APOLLO_GPIO_HOP_TERMINAL_128
    };
    return (map[index]);
}

/*! @} */
