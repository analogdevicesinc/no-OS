/*!
 * \brief     Fine DDC functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDDC
 * @{
 */
#ifndef __ADI_APOLLO_FDDC_TYPES_H__
#define __ADI_APOLLO_FDDC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_FDDC_NUM 16
#define ADI_APOLLO_FDDC_PER_SIDE_NUM 8

/*!
 * \brief Enumerates RX fine DDC select
 */
typedef enum {
    ADI_APOLLO_FDDC_NONE = 0x0000,                      /*!< No FINE DDC */
    ADI_APOLLO_FDDC_A0 = 0x0001,                         /*!< FINE DDC 0 of SIDE A */
    ADI_APOLLO_FDDC_A1 = 0x0002,                         /*!< FINE DDC 1 of SIDE A */
    ADI_APOLLO_FDDC_A2 = 0x0004,                         /*!< FINE DDC 2 of SIDE A */
    ADI_APOLLO_FDDC_A3 = 0x0008,                         /*!< FINE DDC 3 of SIDE A */
    ADI_APOLLO_FDDC_A4 = 0x0010,                         /*!< FINE DDC 4 of SIDE A */
    ADI_APOLLO_FDDC_A5 = 0x0020,                         /*!< FINE DDC 5 of SIDE A */
    ADI_APOLLO_FDDC_A6 = 0x0040,                         /*!< FINE DDC 6 of SIDE A */
    ADI_APOLLO_FDDC_A7 = 0x0080,                         /*!< FINE DDC 7 of SIDE A */
    ADI_APOLLO_FDDC_B0 = 0x0100,                         /*!< FINE DDC 0 of SIDE B */
    ADI_APOLLO_FDDC_B1 = 0x0200,                         /*!< FINE DDC 1 of SIDE B */
    ADI_APOLLO_FDDC_B2 = 0x0400,                         /*!< FINE DDC 2 of SIDE B */
    ADI_APOLLO_FDDC_B3 = 0x0800,                         /*!< FINE DDC 3 of SIDE B */
    ADI_APOLLO_FDDC_B4 = 0x1000,                         /*!< FINE DDC 4 of SIDE B */
    ADI_APOLLO_FDDC_B5 = 0x2000,                         /*!< FINE DDC 5 of SIDE B */
    ADI_APOLLO_FDDC_B6 = 0x4000,                         /*!< FINE DDC 6 of SIDE B */
    ADI_APOLLO_FDDC_B7 = 0x8000,                         /*!< FINE DDC 7 of SIDE B */
    ADI_APOLLO_FDDC_ALL = 0xFFFF,                        /*!< ALL FINE DDCs */
    ADI_APOLLO_FDDC_ALL_4T4R = 0x0F0F                    /*!< ALL FINE DDCs (4T4R) */
} adi_apollo_fine_ddc_select_e;

/*!
* \brief FDDC programming data
*/
typedef struct {
    adi_apollo_fddc_ratio_e dcm;                    /*!< \ref adi_apollo_fddc_ratio_e */
    uint8_t link_num;                               /*!< 0: Link0, 1: Link1 */ 
    uint8_t debug_clkoff_n;                         /*!< bit 0: filter input clock 
                                                        bit 1: hb1 clock
                                                        bit 2: hb2 clock
                                                        bit 3: hb3 clock
                                                        bit 4: hb4 clock
                                                        bit 5: hb5 clock
                                                        bit 6: hb6 clock
                                                        bit 7: filter output clock  */
    uint8_t hb1_gain_6db_en;                        /*!< +6dB gain enable on HB1. 0: disable, 1: enable */

} adi_apollo_fddc_pgm_t;

/*!
* \brief FDDC inspect params
*/
typedef struct {
    adi_apollo_fddc_cfg_t dp_cfg;                       /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */ 

} adi_apollo_fddc_inspect_t;

#endif /* __ADI_APOLLO_FDDC_TYPES_H__ */
/*! @} */
