/*!
 * \brief     CFIR Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CFIR
 * @{
 */
#ifndef __ADI_APOLLO_CFIR_TYPES_H__
#define __ADI_APOLLO_CFIR_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_CFIR_NUM                 4
#define ADI_APOLLO_CFIR_PER_SIDE_NUM        2
#define ADI_APOLLO_CFIR_DP_PER_INST_NUM     4
#define ADI_APOLLO_CFIR_PROFILE_NUM         2
#define ADI_APOLLO_CFIR_COEFF_NUM           16
#define ADI_APOLLO_CFIR_MEM_SEL_NUM         3
#define ADI_APOLLO_CFIR_NUM_PROFILES 2
#define ADI_APOLLO_CFIR_COEFF_SETS 4
#define ADI_APOLLO_CFIR_NUM_TAPS                16

/*!
 * \brief Enumerates CFIR select
 */
typedef enum {
    ADI_APOLLO_CFIR_NONE = 0x0000,                       /*!< No CFIR */
    ADI_APOLLO_CFIR_A0 = 0x0001,                         /*!< CFIR 0 of SIDE A */
    ADI_APOLLO_CFIR_A1 = 0x0002,                         /*!< CFIR 1 of SIDE A */
    ADI_APOLLO_CFIR_B0 = 0x0004,                         /*!< CFIR 0 of SIDE B */
    ADI_APOLLO_CFIR_B1 = 0x0008,                         /*!< CFIR 1 of SIDE B */
    ADI_APOLLO_CFIR_ALL = 0x000F,                        /*!< ALL CFIRs */
    ADI_APOLLO_CFIR_ALL_4T4R = 0x000F                    /*!< ALL CFIRs 4T4R (same as 8T8R)*/
} adi_apollo_cfir_sel_e;

/*!
 * \brief Enumerates CFIR data path select for a CFIR instance
 */
typedef enum {
    ADI_APOLLO_CFIR_DP_NONE = 0x0000,                   /*!< No CFIR data path select */
    ADI_APOLLO_CFIR_DP_0 = 0x0001,                      /*!< CFIR data path 0  */
    ADI_APOLLO_CFIR_DP_1 = 0x0002,                      /*!< CFIR data path 1 */
    ADI_APOLLO_CFIR_DP_2 = 0x0004,                      /*!< CFIR data path 2 (8t8r only) */
    ADI_APOLLO_CFIR_DP_3 = 0x0008,                      /*!< CFIR data path 3 (8t8r only) */
    ADI_APOLLO_CFIR_DP_ALL = 0x000F,                    /*!< ALL CFIR data paths */
    ADI_APOLLO_CFIR_DP_ALL_4T4R = 0x0003                /*!< ALL CFIR data paths */
} adi_apollo_cfir_dp_sel;

/*!
* \brief Enumerates CFIR PROFILE selection
*/
typedef enum {
    ADI_APOLLO_CFIR_PROFILE_NONE = 0x00,                /*!< No filter profile */
    ADI_APOLLO_CFIR_PROFILE_0    = 0x01,                /*!< 1st filter profile */
    ADI_APOLLO_CFIR_PROFILE_1    = 0x02,                /*!< 2nd filter profile */
    ADI_APOLLO_CFIR_PROFILE_ALL  = 0x03,                /*!< All filter profile */
} adi_apollo_cfir_profile_sel_e;

/*!
* \brief Enumerates CFIR enable/bypass option
*/
typedef enum {
    ADI_APOLLO_CFIR_BYPASS      = 0x00,                 /*!< Bypass the CFIR */
    ADI_APOLLO_CFIR_ENABLE      = 0x01                  /*!< Enable the CFIR */
} adi_apollo_cfir_enable_sel_e;

/*!
* \brief Enumerates CFIR profile selection mode
*/
typedef enum
{
    ADI_APOLLO_CFIR_CHAN_SEL_DIRECT_REGMAP = 0, /*!< Direct spi/hsci CFIR hop select */
    ADI_APOLLO_CFIR_CHAN_SEL_DIRECT_GPIO = 1,   /*!< Direct GPIO hop select switch between 2 coeff sets  */
    ADI_APOLLO_CFIR_CHAN_SEL_TRIG_REGMAP = 2,   /*!< Trigger based hopping. Scheduled regmap */
    ADI_APOLLO_CFIR_CHAN_SEL_TRIG_GPIO = 3,     /*!< Trigger based hopping. Scheduled GPIO */
    ADI_APOLLO_CFIR_CHAN_SEL_NUM
} adi_apollo_cfir_profile_sel_mode_e;

/*!
* \brief CFIR programming data
*/
typedef struct {
    uint8_t cfir_bypass;                                        /*!< Bypass CFIR. 0: Disable, 1: Enable */
    uint8_t cfir_sparse_filt_en;                                /*!< Enable Sparse Filter. 0: Disable, 1: Enable */
    uint8_t cfir_32taps_en;                                     /*!< Enable 32-taps filter 0: Disable, 1: Enable */
    uint8_t cfir_coeff_transfer;                                /*!< Coefficient Transfer Signal. Transfers all coefficient data from master registers to slave registers. 0: Disable, 1: Enable */
} adi_apollo_cfir_pgm_t;

/*!
* \brief CFIR inspect param
*/
typedef struct {
    adi_apollo_cfir_cfg_t   dp_cfg;                    /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */

    uint8_t                 cfir_coeff_transfer;       /*!< Coefficient Transfer Signal. Transfers all coefficient data from master registers to slave registers. 0: Disable, 1: Enable */
    uint16_t                sparse_coeff[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_NUM_TAPS];
    uint8_t                 sparse_mem[ADI_APOLLO_CFIR_NUM_PROFILES][ADI_APOLLO_CFIR_COEFF_SETS][ADI_APOLLO_CFIR_MEM_SEL_NUM];
} adi_apollo_cfir_inspect_t;
#endif /* __ADI_APOLLO_CFIR_TYPES_H__ */
/*! @} */
