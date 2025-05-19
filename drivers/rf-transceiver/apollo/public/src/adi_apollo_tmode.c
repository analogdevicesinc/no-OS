/*!
 * \brief     APIs for JESD Tx (JTx)
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TMODE
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_tmode.h"
#include "adi_apollo_jtx.h"
#include "adi_apollo_dformat.h"
#include "adi_apollo_dformat_local.h"
#include "adi_apollo_tmode_local.h"

#include "adi_apollo_bf_jtx_dformat.h"
#include "adi_apollo_bf_jtx_dual_link.h"
#include "adi_apollo_bf_jtx_qbf_txfe.h"
#include "adi_apollo_bf_serdes_txdig_phy_core1p2.h"

#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_tmode_config_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint8_t converter_mask,
    adi_apollo_rx_tmode_type_sel_e mode, 
    adi_apollo_rx_tmode_res_e res)

{
    int32_t err;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    
    /* set test mode converters */
    err = adi_apollo_dformat_conv_test_mode_enable_set(device, links, converter_mask);
    ADI_APOLLO_ERROR_RETURN(err);

    /* set JTx output as 16bit resolution */
    err = adi_apollo_dformat_res_sel_set(device, links, ADI_APOLLO_CHIP_OUT_RES_16BIT);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Set ADC TMODE block res */
    adi_apollo_tmode_resolution_set(device, links, res); 

    /* set test mode type */
    err = adi_apollo_tmode_type_sel_set(device, links, mode);
    ADI_APOLLO_ERROR_RETURN(err);
    
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tmode_usr_pattern_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint16_t usr_pattern[],
    uint32_t length)
{
    int32_t err, i, j, k; 
    int32_t pat_addr_offset_lsb[ADI_APOLLO_TMODE_USR_PAT_NUM], 
            pat_addr_offset_msb[ADI_APOLLO_TMODE_USR_PAT_NUM]; 
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(length > ADI_APOLLO_TMODE_USR_PAT_NUM);

    /* Generating address offsets for User Pattern indices */

    pat_addr_offset_lsb[0] = REG_TMODE_USR_LSB_P0_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P0_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[1] = REG_TMODE_USR_LSB_P1_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P0_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[2] = REG_TMODE_USR_LSB_P2_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P1_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[3] = REG_TMODE_USR_LSB_P3_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P2_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[4] = REG_TMODE_USR_LSB_P4_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P3_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[5] = REG_TMODE_USR_LSB_P5_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P4_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[6] = REG_TMODE_USR_LSB_P6_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P5_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_lsb[7] = REG_TMODE_USR_LSB_P7_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_LSB_P6_JTX_DFORMAT_ADDR(0, 0);

    pat_addr_offset_msb[0] = REG_TMODE_USR_MSB_P0_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P0_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[1] = REG_TMODE_USR_MSB_P1_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P0_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[2] = REG_TMODE_USR_MSB_P2_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P1_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[3] = REG_TMODE_USR_MSB_P3_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P2_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[4] = REG_TMODE_USR_MSB_P4_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P3_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[5] = REG_TMODE_USR_MSB_P5_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P4_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[6] = REG_TMODE_USR_MSB_P6_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P5_JTX_DFORMAT_ADDR(0, 0);
    pat_addr_offset_msb[7] = REG_TMODE_USR_MSB_P7_JTX_DFORMAT_ADDR(0, 0) - REG_TMODE_USR_MSB_P6_JTX_DFORMAT_ADDR(0, 0);

    for (k = 1; k < length; k ++) {
       pat_addr_offset_lsb[k] = pat_addr_offset_lsb[k] + pat_addr_offset_lsb[k-1];
       pat_addr_offset_msb[k] = pat_addr_offset_msb[k] + pat_addr_offset_msb[k-1];
   } 

    /* Writing user pattern data to registers */
    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i++) {
        if ((1 << i) & links) {

            regmap_base_addr = calc_jtx_dformat_base(i);

            for (j = 0; j < length; j ++) {

                err = adi_apollo_hal_reg_set(device, REG_TMODE_USR_LSB_P0_JTX_DFORMAT_ADDR(regmap_base_addr, (i % ADI_APOLLO_TMODE_N)) + pat_addr_offset_lsb[j],
                                            (uint8_t)usr_pattern[j]);
                ADI_APOLLO_ERROR_RETURN(err);

                err = adi_apollo_hal_reg_set(device, REG_TMODE_USR_MSB_P0_JTX_DFORMAT_ADDR(regmap_base_addr, (i % ADI_APOLLO_TMODE_N)) + pat_addr_offset_msb[j],
                                            (uint8_t)(usr_pattern[j] >> 8));
                ADI_APOLLO_ERROR_RETURN(err);
             }
        }
    }

    return API_CMS_ERROR_OK;
}

/*==================== L O C A L   A P I   C O D E ====================*/


int32_t adi_apollo_tmode_type_sel_set(adi_apollo_device_t *device, 
    const uint16_t links,
    adi_apollo_rx_tmode_type_sel_e mode) 
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i ++) {
        if ((1 << i) & links) {

            regmap_base_addr = calc_jtx_dformat_base(i);

            if ((i % ADI_APOLLO_TMODE_N) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_TMODE_TYPE_SEL_INFO(regmap_base_addr), mode);        /* Link 0 */
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_TMODE_TYPE_SEL_LINK1_INFO(regmap_base_addr), mode);  /* Link 1 */
                ADI_APOLLO_ERROR_RETURN(err);
            }
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_tmode_resolution_set(adi_apollo_device_t *device, 
    const uint16_t links,
    adi_apollo_rx_tmode_res_e res) 
{
    int32_t err, i;
    uint32_t regmap_base_addr = 0;
 
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    for (i = 0; i < ADI_APOLLO_NUM_JTX_LINKS; i ++) {
        if ((1 << i) & links) {
            regmap_base_addr = calc_jtx_dformat_base(i);

            if ((i % ADI_APOLLO_TMODE_N) == 0) {
                err = adi_apollo_hal_bf_set(device, BF_TMODE_RES_INFO(regmap_base_addr), res);        /* Link 0 */
                ADI_APOLLO_ERROR_RETURN(err);
            } else {
                err = adi_apollo_hal_bf_set(device, BF_TMODE_RES_LINK1_INFO(regmap_base_addr), res);  /* Link 1 */
                ADI_APOLLO_ERROR_RETURN(err);
            }
            ADI_APOLLO_ERROR_RETURN(err);
        }
    }

    return API_CMS_ERROR_OK;
}
/*! @} */