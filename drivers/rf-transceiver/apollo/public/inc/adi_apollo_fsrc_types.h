/*!
 * \brief     FSRC Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FSRC
 * @{
 */
#ifndef __ADI_APOLLO_FSRC_TYPES_H__
#define __ADI_APOLLO_FSRC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_FSRC_NUM 4
#define ADI_APOLLO_FSRC_PER_SIDE_NUM 2

/*!
* \brief Enumerates FSRC selection
*/
typedef enum {
    ADI_APOLLO_FSRC_NONE   = 0x0,                        /*!< No FSRC */
    ADI_APOLLO_FSRC_A0     = 0x1,                        /*!< FSRC 0 of SIDE A */
    ADI_APOLLO_FSRC_A1     = 0x2,                        /*!< FSRC 1 of SIDE A */
    ADI_APOLLO_FSRC_B0     = 0x4,                        /*!< FSRC 0 of SIDE B */
    ADI_APOLLO_FSRC_B1     = 0x8,                        /*!< FSRC 1 of SIDE B */
    ADI_APOLLO_FSRC_ALL    = 0xF,                        /*!< All FSRC */
    ADI_APOLLO_FSRC_ALL_4T4R = 0xF                       /*!< All FSRC 4T4R (same as 8T8R)*/
} adi_apollo_fsrc_sel_e;

/*!
* \brief FSRC programming data
*/
typedef struct {
    uint64_t fsrc_rate_int;                 /*!< A variable of FSRC rate = (FSRC rate_int +  rate_frac_a/rate_frac_b)/2^48 */
    uint64_t fsrc_rate_frac_a;              /*!< A variable of FSRC rate = (FSRC rate_int +  rate_frac_a/rate_frac_b)/2^48 */
    uint64_t fsrc_rate_frac_b;              /*!< A variable of FSRC rate = (FSRC rate_int +  rate_frac_a/rate_frac_b)/2^48 */
    uint16_t gain_reduction;                /*!< Gain Reduction for Decimation */
    uint16_t sample_frac_delay;             /*!< This is used to delay the output sample by this fractional delay */
    uint8_t ptr_syncrstval;                 /*!< This is used for the FIFO pointer value when the internal fifo-sync is generated */
    uint8_t ptr_overwrite;                  /*!< 1: FIFO reset pointer value is used from regmap; 0: FIFO reset pointer value computed in design */
    uint8_t fsrc_data_mult_dither_en;       /*!< If this bit is 1, dither is added into FSRC in the data and delta multiplication calculation */    
    uint8_t fsrc_dither_en;                 /*!< If this bit is 1, dither is added into FSRC in the delta delay calculation */
    uint8_t fsrc_4t4r_split;                /*!< If this bit is set FSRC0_1 is enabled. This bit is only to be used for 4T4R two stream use case */
    uint8_t fsrc_bypass;                    /*!< If this bit is 1, the FSRC is bypassed */
    uint8_t fsrc_en;                        /*!< If this bit is 1, FSRC is enabled */
    uint8_t fsrc_1x_mode;                   /*!< If this bit is 1, FSRC will be in 1x mode */
} adi_apollo_fsrc_pgm_t;

/*!
* \brief FSRC inspect param
*/
typedef struct {
    adi_apollo_fsrc_cfg_t dp_cfg;           /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
    uint8_t fsrc_bypass;                    /*!< If this bit is 1, the FSRC is bypassed */
} adi_apollo_fsrc_inspect_t;


/*!
* \brief RX Rate Match FIFO programming data
*/
typedef struct {
    uint16_t dcm_ratio;                               /*!< Chip Decimation Ratio corresponding to link. The least overall decimation among DDCs present in link */
    uint16_t total_dcm;                               /*!< Link Total Decimation */
    uint8_t invalid_en;                               /*!< When Link_TOTAL_DEC is not equal to Chip Decimation , Invalid_en should be 1 to indicate that invalid data can be inserted by the rate match fifo. */
    uint8_t dfor_ddc_dither_en;                       /*!< Dformat dither enable for DDC mode for link. 0: dformat dither disable, 1: dformat dither enable */
    uint8_t sample_repeat_en;                         /*!< Sample Repeat Enable. 1:Enable, 0:Disable */
    uint8_t startup_force_inv_en;                     /*!< If this bit is high and invalid_en bit is also high for that link,
                                                           then after the sync (clkgen_sync) till the reconfig_done signal (inside reconfig control), we will transmit invalid samples */
} adi_apollo_rx_rm_fifo_pgm_t;


#endif /* __ADI_APOLLO_FSRC_TYPES_H__ */
/*! @} */
