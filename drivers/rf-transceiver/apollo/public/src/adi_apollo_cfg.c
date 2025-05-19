/*!
 * \brief     APIs for SYS TOP
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CFG
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_ddc_local.h"
#include "adi_apollo_cfg.h"
#include "adi_apollo_rx.h"
#include "adi_apollo_tx.h"
#include "adi_apollo_gpio.h"
#include "adi_apollo_clk_mcs.h"
#include "adi_apollo_dac.h"
#include "adi_apollo_sysclk_cond.h"
#include "adi_utils.h"

#define ADI_APOLLO_CONV_IDX_CONV(is_8t8r, idx, per_side) (is_8t8r) ? (idx) : (((idx) / (per_side/2)) + ((idx) % (per_side)))

static uint32_t calc_adc_cal_cfg_reg(int32_t idx);
static uint32_t calc_dac_cal_cfg_reg(int32_t idx);
static int32_t cal_data_size_addr_get(adi_apollo_device_t *device, uint8_t idx,
        uint32_t cal_obj_siz, uint32_t cal_obj_ofst, uint32_t obj_ptr, uint32_t siz_ptr,
        uint32_t *size, uint32_t *address);
static int32_t cal_data_len_get(adi_apollo_device_t* device,
        uint32_t cal_obj_ofst, uint32_t siz_ptr,
        uint32_t *size);
static uint8_t is_init_cal_cfg_valid(adi_apollo_init_cal_cfg_e init_cal_cfg);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_cfg_data_path(adi_apollo_device_t *device, adi_apollo_top_t *config)
{
    int32_t err;
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(config);
    ADI_APOLLO_LOG_FUNC();

    /* RX path config */
    for(i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        err = adi_apollo_rx_configure(device, i, &(config->rx_path[i]), &(config->jtx[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Enable Rx data FIFOs */
    err = adi_apollo_clk_mcs_rx_data_fifo_enable(device, config->adc_enable, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    /* TX path config*/
    for(i = 0; i < ADI_APOLLO_NUM_SIDES; i++) {
        err = adi_apollo_tx_configure(device, i, &(config->tx_path[i]), &(config->jrx[i]));
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Enable DAC Scramblers */
    err = adi_apollo_dac_scrambler_enable_set(device, config->dac_enable, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Enable DACs */
    err = adi_apollo_dac_data_enable(device, config->dac_enable, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Enable Tx data FIFOs */
    err = adi_apollo_clk_mcs_tx_data_fifo_enable(device, config->dac_enable, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    if (config->jtx[0].common_link_cfg.ver == ADI_APOLLO_JESD_204B) {
        err = adi_apollo_gpio_jesd_204b_configure(device);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_adc_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t adcs, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;
    uint8_t i;
    uint16_t adc;
    uint32_t init_cal_cfg_reg;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(!is_init_cal_cfg_valid(init_cal_cfg));

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i ++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);
        if (adc > 0) {
            init_cal_cfg_reg = calc_adc_cal_cfg_reg(i);

            err = adi_apollo_hal_bf_set(device, ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR + init_cal_cfg_reg, (i % 2 == 0) ? 0x00000400 : 0x00000404, init_cal_cfg);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_dac_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t dacs, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;
    uint8_t i;
    uint16_t dac;
    uint32_t init_cal_cfg_reg;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for(i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_DAC_A0 << i);
        if (dac > 0) {
            init_cal_cfg_reg = calc_dac_cal_cfg_reg(i);

            err = adi_apollo_hal_bf_set(device, ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR + init_cal_cfg_reg, (i % 2 == 0) ? 0x00000400 : 0x00000404, init_cal_cfg);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_rx_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;
    uint8_t i;
    uint16_t s;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for(i = 0; i < ADI_APOLLO_NUM_JRX_SERDES_12PACKS; i ++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            err = adi_apollo_hal_bf_set(device, ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR + ADI_APOLLO_WARMBOOT_CFG_SERDES_RX_1_0, (i % 2 == 0) ? 0x00000400 : 0x00000404, init_cal_cfg);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_tx_init_cal_cfg_set(adi_apollo_device_t *device, const uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg)
{
    int32_t err;
    uint8_t i;
    uint16_t s;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for(i = 0; i < ADI_APOLLO_NUM_JTX_SERDES_12PACKS; i ++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            err = adi_apollo_hal_bf_set(device, ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR + ADI_APOLLO_WARMBOOT_CFG_SERDES_TX_1_0, (i % 2 == 0) ? 0x00000400 : 0x00000404, init_cal_cfg);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_adc_cal_data_set(adi_apollo_device_t *device, const uint16_t adcs, uint8_t mode, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t adc;
    uint32_t size_bytes;
    uint32_t address;
    uint8_t cal_data_offset;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);
    ADI_APOLLO_INVALID_PARAM_RETURN(mode > 1);

    cal_data_offset = (mode == ADI_APOLLO_ADC_CAL_SEQUENTIAL_MODE) ? ADI_APOLLO_ADC_CAL_STRUCT_SEQ_DATA_OFFSET : ADI_APOLLO_ADC_CAL_STRUCT_RAND_DATA_OFFSET;

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);
        if (adc > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device,
                    ADI_APOLLO_CONV_IDX_CONV(device->dev_info.is_8t8r, i, ADI_APOLLO_ADC_PER_SIDE_NUM),
                    ADI_APOLLO_ADC_CAL_STRUCT_SIZE, cal_data_offset,
                    ADI_APOLLO_CPU_1_FW_ADC_RX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_ADC_RX_SIZE_PTR,
                    &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* Assumes incoming data has 4-byte crc */
                return API_CMS_ERROR_ERROR;     /* cal_data size fw mismatch */
            }

            /* Stream cal data to core */
            err = adi_apollo_hal_stream_reg_set(device, address, cal_data, len, 0);
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_dac_cal_data_set(adi_apollo_device_t *device, const uint16_t dacs, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t dac;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);

    for(i = 0; i < ADI_APOLLO_DAC_NUM; i ++) {
        dac = dacs & (ADI_APOLLO_DAC_A0 << i);
        if (dac > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device, i,
                    ADI_APOLLO_DAC_CAL_OBJ_SIZE, ADI_APOLLO_DAC_CAL_OBJ_OFFSET,
                    ADI_APOLLO_CPU_1_FW_DAC_TX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_DAC_TX_SIZE_PTR,
                    &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* Assumes incoming data has 4-byte crc */
                return API_CMS_ERROR_ERROR;     /* cal_data size fw mismatch */
            }

            /* Stream cal data to core */
             err = adi_apollo_hal_stream_reg_set(device, address, cal_data, len, 0);
             ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_rx_cal_data_set(adi_apollo_device_t *device, const uint16_t serdes, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t s;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);

    for(i = 0; i < ADI_APOLLO_NUM_JTX_SERDES_12PACKS; i ++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device, i,
                ADI_APOLLO_SERDES_RX_CAL_OBJ_SIZE, ADI_APOLLO_SERDES_RX_CAL_OBJ_OFFSET,
                ADI_APOLLO_CPU_1_FW_SERDES_RX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_SERDES_RX_SIZE_PTR,
                &size_bytes, &address);

            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* Assumes incoming data has 4-byte crc */
                return API_CMS_ERROR_ERROR;     /* cal_data size fw mismatch */
            }

            /* Stream cal data to core */
             err = adi_apollo_hal_stream_reg_set(device, address, cal_data, len, 0);
             ADI_APOLLO_ERROR_RETURN(err);

        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_tx_cal_data_set(adi_apollo_device_t *device, const uint16_t serdes, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t s;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);

    for(i = 0; i < ADI_APOLLO_NUM_JTX_SERDES_12PACKS; i ++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device, i,
                    ADI_APOLLO_SERDES_TX_CAL_OBJ_SIZE, ADI_APOLLO_SERDES_TX_CAL_OBJ_OFFSET,
                    ADI_APOLLO_CPU_1_FW_SERDES_TX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_SERDES_TX_SIZE_PTR,
                    &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* Assumes incoming data has 4-byte crc */
                return API_CMS_ERROR_ERROR;     /* cal_data size fw mismatch */
            }

            /* Stream cal data to core */
             err = adi_apollo_hal_stream_reg_set(device, address, cal_data, len, 0);
             ADI_APOLLO_ERROR_RETURN(err);

        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_adc_cal_data_get(adi_apollo_device_t *device, const uint16_t adcs, uint8_t mode, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t adc;
    uint32_t size_bytes;
    uint32_t address;
    uint8_t cal_data_offset;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(adcs) != 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(mode > 1);

    cal_data_offset = (mode == ADI_APOLLO_ADC_CAL_SEQUENTIAL_MODE) ? ADI_APOLLO_ADC_CAL_STRUCT_SEQ_DATA_OFFSET : ADI_APOLLO_ADC_CAL_STRUCT_RAND_DATA_OFFSET;

    for(i = 0; i < ADI_APOLLO_ADC_NUM; i++) {
        adc = adcs & (ADI_APOLLO_ADC_A0 << i);
        if (adc > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device,
                    ADI_APOLLO_CONV_IDX_CONV(device->dev_info.is_8t8r, i, ADI_APOLLO_ADC_PER_SIDE_NUM),
                    ADI_APOLLO_ADC_CAL_STRUCT_SIZE, cal_data_offset,
                    ADI_APOLLO_CPU_1_FW_ADC_RX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_ADC_RX_SIZE_PTR,
                    &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* size_bytes is size of cal data, not including 32-bit CRC */
                return API_CMS_ERROR_ERROR;     /* cal_data size FW mismatch */
            }

            /* Stream cal data from core */
            err = adi_apollo_hal_stream_reg_get(device, address, cal_data, len, 0);
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_dac_cal_data_get(adi_apollo_device_t *device, const uint16_t dacs, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t dac;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(dacs) != 1);

    for(i = 0; i < ADI_APOLLO_DAC_NUM; i++) {
        dac = dacs & (ADI_APOLLO_DAC_A0 << i);
        if (dac > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device,
                    ADI_APOLLO_CONV_IDX_CONV(device->dev_info.is_8t8r, i, ADI_APOLLO_DAC_PER_SIDE_NUM),
                    ADI_APOLLO_DAC_CAL_OBJ_SIZE, ADI_APOLLO_DAC_CAL_OBJ_OFFSET,
                    ADI_APOLLO_CPU_1_FW_DAC_TX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_DAC_TX_SIZE_PTR,
                    &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* size_bytes is size of cal data, not including 32-bit CRC */
                return API_CMS_ERROR_ERROR;     /* cal_data size FW mismatch */
            }

            /* Stream cal data from core */
            err = adi_apollo_hal_stream_reg_get(device, address, cal_data, len, 0);
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_rx_cal_data_get(adi_apollo_device_t* device, const uint16_t serdes, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t s;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(serdes) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_SERDES_12PACKS; i++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device, i,
                ADI_APOLLO_SERDES_RX_CAL_OBJ_SIZE, ADI_APOLLO_SERDES_RX_CAL_OBJ_OFFSET,
                ADI_APOLLO_CPU_1_FW_SERDES_RX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_SERDES_RX_SIZE_PTR,
                &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* size_bytes is size of cal data, not including 32-bit CRC */
                return API_CMS_ERROR_ERROR;     /* cal_data size FW mismatch */
            }

            /* Stream cal data from core */
            err = adi_apollo_hal_stream_reg_get(device, address, cal_data, len, 0);
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_tx_cal_data_get(adi_apollo_device_t* device, const uint16_t serdes, uint8_t cal_data[], uint32_t len)
{
    int32_t err;
    uint8_t i;
    uint16_t s;
    uint32_t size_bytes;
    uint32_t address;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cal_data);
    ADI_APOLLO_INVALID_PARAM_RETURN(adi_api_utils_num_selected(serdes) != 1);

    for (i = 0; i < ADI_APOLLO_NUM_JTX_SERDES_12PACKS; i++) {
        s = serdes & (ADI_APOLLO_TXRX_SERDES_12PACK_A << i);
        if (s > 0) {
            /* Get the cal data size and address location */
            err = cal_data_size_addr_get(device, i,
                ADI_APOLLO_SERDES_TX_CAL_OBJ_SIZE, ADI_APOLLO_SERDES_TX_CAL_OBJ_OFFSET,
                ADI_APOLLO_CPU_1_FW_SERDES_TX_OBJ_PTR, ADI_APOLLO_CPU_1_FW_SERDES_TX_SIZE_PTR,
                &size_bytes, &address);
            ADI_APOLLO_ERROR_RETURN(err);

            if (size_bytes != len - 4) {        /* size_bytes is size of cal data, not including 32-bit CRC */
                return API_CMS_ERROR_ERROR;     /* cal_data size FW mismatch */
            }

            /* Stream cal data from core */
            err = adi_apollo_hal_stream_reg_get(device, address, cal_data, len, 0);
            ADI_APOLLO_ERROR_RETURN(err);

            break;
        }
    }
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_adc_cal_data_len_get(adi_apollo_device_t* device, uint8_t mode, uint32_t* len)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t cal_data_offset;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(len);
    ADI_APOLLO_INVALID_PARAM_RETURN(mode > 1);

    cal_data_offset = (mode == ADI_APOLLO_ADC_CAL_SEQUENTIAL_MODE) ? ADI_APOLLO_ADC_CAL_STRUCT_SEQ_DATA_OFFSET : ADI_APOLLO_ADC_CAL_STRUCT_RAND_DATA_OFFSET;

    /* Get the cal data size in byes. Same length for all ADC instances */
    err = cal_data_len_get(device,
        cal_data_offset,
        ADI_APOLLO_CPU_1_FW_ADC_RX_SIZE_PTR,
        len);

    /* Add 32-bit checksum */
    *len += 4;

    return err;
}

int32_t adi_apollo_cfg_dac_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(len);

    /* Get the cal data size in byes. Same length for all ADC instances */
    err = cal_data_len_get(device,
        ADI_APOLLO_DAC_CAL_OBJ_OFFSET,
        ADI_APOLLO_CPU_1_FW_DAC_TX_SIZE_PTR,
        len);

    /* Add 32-bit checksum */
    *len += 4;

    return err;
}

int32_t adi_apollo_cfg_serdes_rx_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(len);

    /* Get the cal data size in byes. Same length for all ADC instances */
    err = cal_data_len_get(device,
        ADI_APOLLO_SERDES_RX_CAL_OBJ_OFFSET,
        ADI_APOLLO_CPU_1_FW_SERDES_RX_SIZE_PTR,
        len);

    /* Add 32-bit checksum */
    *len += 4;

    return err;
}

int32_t adi_apollo_cfg_serdes_tx_cal_data_len_get(adi_apollo_device_t* device, uint32_t* len)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(len);

    /* Get the cal data size in byes. Same length for all ADC instances */
    err = cal_data_len_get(device,
        ADI_APOLLO_SERDES_TX_CAL_OBJ_OFFSET,
        ADI_APOLLO_CPU_1_FW_SERDES_TX_SIZE_PTR,
        len);

    /* Add 32-bit checksum */
    *len += 4;

    return err;
}

int32_t adi_apollo_cfg_profile_type_version_get(adi_apollo_device_t *device, adi_apollo_profile_version_t *ver)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(ver);

    ver->major = ADI_APOLLO_PROFILE_VERSION_MAJOR;
    ver->minor = ADI_APOLLO_PROFILE_VERSION_MINOR;
    ver->patch = ADI_APOLLO_PROFILE_VERSION_PATCH;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_cfg_serdes_rx_bridging_cal_cfg_set(adi_apollo_device_t *device, uint8_t bridging_cal_runs)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_CMS_RANGE_CHECK(bridging_cal_runs, 0, 3);

    err = adi_apollo_hal_reg_set(device, (ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR + ADI_APOLLO_SERDES_RX_NUM_BRIDGING_CALS), (0x80 | bridging_cal_runs));
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}


static int32_t cal_data_len_get(adi_apollo_device_t* device,
    uint32_t cal_obj_ofst, uint32_t siz_ptr,
    uint32_t* size)
{
    int32_t err;
    uint32_t cal_size_p;

    err = adi_apollo_hal_reg32_get(device, siz_ptr, &cal_size_p);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_get(device, cal_size_p + cal_obj_ofst, size);    /* Size in bytes, doesn't include 4-byte crc */
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t cal_data_size_addr_get(adi_apollo_device_t *device, uint8_t idx,
        uint32_t cal_obj_siz, uint32_t cal_obj_ofst, uint32_t obj_ptr, uint32_t siz_ptr,
        uint32_t *size, uint32_t *address)
{
    int32_t err;
    uint32_t cal_size_p;
    uint32_t cal_data_addr_p;
    uint32_t cal_offset = cal_obj_siz * idx;   /* FW dependent byte offset into struct */

    err = adi_apollo_hal_reg32_get(device, obj_ptr, &cal_data_addr_p);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_get(device, siz_ptr, &cal_size_p);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_get(device, cal_size_p + cal_obj_ofst, size);    /* Size doesn't include 4-byte crc */
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_get(device, cal_data_addr_p + cal_offset + cal_obj_ofst, address);   /* Mem address of core's cal data */
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static uint8_t is_init_cal_cfg_valid(adi_apollo_init_cal_cfg_e init_cal_cfg) {
    switch (init_cal_cfg) {
        case ADI_APOLLO_INIT_CAL_DISABLED:
        case ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_NVM:
        case ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_USER:
        case ADI_APOLLO_INIT_CAL_ENABLED:
        case ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_NVM:
        case ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_USER:
        return 1;
    }

    return 0;
}

static uint32_t calc_adc_cal_cfg_reg(int32_t idx)
{
    static uint32_t cal_cfg_reg_addr[] = {
        ADI_APOLLO_WARMBOOT_CFG_ADC_A1_A0,        /* A1:A0 */
        ADI_APOLLO_WARMBOOT_CFG_ADC_A3_A2,        /* A3:A2 */
        ADI_APOLLO_WARMBOOT_CFG_ADC_B1_B0,        /* B1:B0 */
        ADI_APOLLO_WARMBOOT_CFG_ADC_B3_B2,        /* B3:B2 */
    };
    return cal_cfg_reg_addr[idx/2];
}

static uint32_t calc_dac_cal_cfg_reg(int32_t idx)
{
    static uint32_t cal_cfg_reg_addr[] = {
        ADI_APOLLO_WARMBOOT_CFG_DAC_A1_A0,        /* A1:A0 */
        ADI_APOLLO_WARMBOOT_CFG_DAC_A3_A2,        /* A3:A2 */
        ADI_APOLLO_WARMBOOT_CFG_DAC_B1_B0,        /* B1:B0 */
        ADI_APOLLO_WARMBOOT_CFG_DAC_B3_B2,        /* B3:B2 */
    };
    return cal_cfg_reg_addr[idx/2];
}
/*! @} */