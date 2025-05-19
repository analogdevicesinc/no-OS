/*!
 * \brief     Top level configuration definition headers
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
#ifndef __ADI_APOLLO_CFG_TYPES_H__
#define __ADI_APOLLO_CFG_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "apollo_cpu_device_profile_types.h"

/*============= D E F I N E S ==============*/
/* Scratch registers for warmboot INT_CAL_CFG flags. Each module uses 4 bits. */
#define ADI_APOLLO_WARMBOOT_CFG_BASE_ADDR               0x47000200

#define ADI_APOLLO_WARMBOOT_CFG_SERDES_RX_1_0               163
#define ADI_APOLLO_WARMBOOT_CFG_SERDES_TX_1_0               164

#define ADI_APOLLO_WARMBOOT_CFG_ADC_A1_A0                   165
#define ADI_APOLLO_WARMBOOT_CFG_ADC_A3_A2                   166
#define ADI_APOLLO_WARMBOOT_CFG_ADC_B1_B0                   167
#define ADI_APOLLO_WARMBOOT_CFG_ADC_B3_B2                   168

#define ADI_APOLLO_WARMBOOT_CFG_DAC_A1_A0                   169
#define ADI_APOLLO_WARMBOOT_CFG_DAC_A3_A2                   170
#define ADI_APOLLO_WARMBOOT_CFG_DAC_B1_B0                   171
#define ADI_APOLLO_WARMBOOT_CFG_DAC_B3_B2                   172

#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_RX_A1_A0            173
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_RX_A3_A2            174
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_RX_B1_B0            175
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_RX_B3_B2            176

#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_TX_A1_A0            177
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_TX_A3_A2            178
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_TX_B1_B0            179
#define ADI_APOLLO_WARMBOOT_CFG_LINEARX_TX_B3_B2            180

#define ADI_APOLLO_SERDES_RX_NUM_BRIDGING_CALS              196

#define ADI_APOLLO_DAC_CAL_OBJ_SIZE                         24
#define ADI_APOLLO_DAC_CAL_OBJ_OFFSET                       20
#define ADI_APOLLO_SERDES_RX_CAL_OBJ_SIZE                   24
#define ADI_APOLLO_SERDES_RX_CAL_OBJ_OFFSET                 20
#define ADI_APOLLO_SERDES_TX_CAL_OBJ_SIZE                   24
#define ADI_APOLLO_SERDES_TX_CAL_OBJ_OFFSET                 20

#define ADI_APOLLO_ADC_CAL_STRUCT_SIZE                      28
#define ADI_APOLLO_ADC_CAL_STRUCT_RAND_DATA_OFFSET          20
#define ADI_APOLLO_ADC_CAL_STRUCT_SEQ_DATA_OFFSET           24

#define ADI_APOLLO_ADC_CAL_RANDOM_MODE                      00
#define ADI_APOLLO_ADC_CAL_SEQUENTIAL_MODE                  01

/* This register states if the TDC/firmware MCS is in progress and if the synch is achieved */
#define ADI_APOLLO_MCS_OPERATION_AND_SYNCH_STATUS       181

/* Warm boot fw calibration object locations */
#define ADI_APOLLO_CPU_1_FW_ADC_RX_OBJ_PTR              (0x21000534U)         /*!< ADC Rx object pointer */
#define ADI_APOLLO_CPU_1_FW_ADC_RX_SIZE_PTR             (0x21000538U)         /*!< ADC Rx size pointer */
#define ADI_APOLLO_CPU_1_FW_ADC_RX_VERSION_PTR          (0x2100053CU)         /*!< ADC Rx version pointer */
#define ADI_APOLLO_CPU_1_FW_DAC_TX_OBJ_PTR              (0x21000540U)         /*!< DAC Tx object pointer */
#define ADI_APOLLO_CPU_1_FW_DAC_TX_SIZE_PTR             (0x21000544U)         /*!< DAC Tx size pointer */
#define ADI_APOLLO_CPU_1_FW_DAC_TX_VERSION_PTR          (0x21000548U)         /*!< DAC Tx version pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_RX_OBJ_PTR           (0x2100054CU)         /*!< Serdes Rx object pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_RX_SIZE_PTR          (0x21000550U)         /*!< Serdes Rx size pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_RX_VERSION_PTR       (0x21000554U)         /*!< Serdes Rx version pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_TX_OBJ_PTR           (0x21000558U)         /*!< Serdes Tx object pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_TX_SIZE_PTR          (0x2100055CU)         /*!< Serdes Tx size pointer */
#define ADI_APOLLO_CPU_1_FW_SERDES_TX_VERSION_PTR       (0x21000560U)         /*!< Serdes Tx version pointer */

/* Clock conditioning structure pointer locations */
#define APOLLO_CPU_1_FW_CLK_COND_CONFIG_PTR             (0x21000660U)          /*!< ADC clock conditioning config ref pointer */
#define APOLLO_CPU_1_FW_CLK_COND_0_CALDATA_PTR          (0x21000664U)          /*!< ADC clock conditioning side A cal ref pointer */
#define APOLLO_CPU_1_FW_CLK_COND_1_CALDATA_PTR          (0x21000668U)          /*!< ADC clock conditioning side B cal ref pointer */
#define APOLLO_CPU_1_FW_CLK_COND_BGCAL_CLIP_PTR         (0x21000688U)          /*!< Clock conditioning Bg calibration clip status pointer */

/*!
 * \brief Enumeration of initial calibration configurations
 */
typedef enum
{
    ADI_APOLLO_INIT_CAL_DISABLED = 0,                  /*!< No  Init Calibration - start with default calibration data   */
    ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_NVM,    /*!< No  Init Calibration - start with calibration data from NVM  */
    ADI_APOLLO_INIT_CAL_DISABLED_WARMBOOT_FROM_USER,   /*!< No  Init Calibration - start with calibration data from USER */
    ADI_APOLLO_INIT_CAL_ENABLED,                       /*!< Run Init Calibration - start with default calibration data   */
    ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_NVM,     /*!< Run Init Calibration - start with calibration data from NVM  */
    ADI_APOLLO_INIT_CAL_ENABLED_WARMBOOT_FROM_USER,    /*!< Run Init Calibration - start with calibration data from USER */
} adi_apollo_init_cal_cfg_e;

#endif /* __ADI_APOLLO_CFG_TYPES_H__ */

/*! @} */