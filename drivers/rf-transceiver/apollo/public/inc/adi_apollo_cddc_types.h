/*!
 * \brief   Coarse DDC functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDDC
 * @{
 */
#ifndef __ADI_APOLLO_CDDC_TYPES_H__
#define __ADI_APOLLO_CDDC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_CDDC_NUM 8
#define ADI_APOLLO_CDDC_PER_SIDE_NUM 4

/*!
 * \brief Enumerates RX coarse DDC select
 */
typedef enum {
    ADI_APOLLO_CDDC_NONE = 0x00,                      /*!< No COARSE DDC */
    ADI_APOLLO_CDDC_A0 = 0x01,                         /*!< COARSE DDC 0 of SIDE A */
    ADI_APOLLO_CDDC_A1 = 0x02,                         /*!< COARSE DDC 1 of SIDE A */
    ADI_APOLLO_CDDC_A2 = 0x04,                         /*!< COARSE DDC 2 of SIDE A */
    ADI_APOLLO_CDDC_A3 = 0x08,                         /*!< COARSE DDC 3 of SIDE A */
    ADI_APOLLO_CDDC_B0 = 0x10,                         /*!< COARSE DDC 0 of SIDE B */
    ADI_APOLLO_CDDC_B1 = 0x20,                         /*!< COARSE DDC 1 of SIDE B */
    ADI_APOLLO_CDDC_B2 = 0x40,                         /*!< COARSE DDC 2 of SIDE B */
    ADI_APOLLO_CDDC_B3 = 0x80,                         /*!< COARSE DDC 3 of SIDE B */
    ADI_APOLLO_CDDC_ALL = 0xFF,                        /*!< ALL COARSE DDCs */
    ADI_APOLLO_CDDC_ALL_4T4R = 0x33                    /*!< ALL COARSE DDCs (4T4R) */
} adi_apollo_cddc_select_e;


/*!
 * \brief Enumerates the CDDC filter +6db gain block selection
 */
typedef enum {
    ADI_APOLLO_CDDC_GAIN_NONE = 0x00,
    ADI_APOLLO_CDDC_GAIN_HB1 = 0x01,                    /*!< CDDC HB1 filter gain block select */
    ADI_APOLLO_CDDC_GAIN_TB1 = 0x02,                    /*!< CDDC TB1 filter gain block select */
    ADI_APOLLO_CDDC_GAIN_ALL = 0x03                     /*!< ALL CDDC gain blocks */

} adi_apollo_cddc_gain_select_e;


/*!
 * \brief Rx coarse DDC configuration
 */
typedef struct {
    adi_apollo_coarse_ddc_dcm_e dcm;                    /*!< \ref adi_apollo_coarse_ddc_dcm_e */
    uint8_t link_num;                                   /*!< 0: Link0, 1: Link1 */   
    uint8_t debug_clkoff_n;                             /*!< bit 0: filter input clock 
                                                             bit 1: filter output clock
                                                             bit 2: hb1 clock
                                                             bit 3: hb2 clock
                                                             bit 4: reserved
                                                             bit 5: reserved
                                                             bit 6: tb1 clock */
    uint8_t fine_bypass;                                /*!< 0: Disable, 1: Enable */
    uint8_t hb1_filt_dly_en;                            /*!< HB1 filter delay enable */
    uint8_t hb2_filt_dly_en;                            /*!< HB2 filter delay enable */
    adi_apollo_tb1_filt_dly_cycles_e tb1_filt_dly;      /*!< TB1 filter delay */
    uint8_t hb1_gain_6db_en;                            /*!< 6dB gain enabled for HB1 filter */
    uint8_t tb1_gain_6db_en;                            /*!< 6dB gain enabled for TB1 filter */
} adi_apollo_cddc_pgm_t;

/*!
 * \brief CDDC inspect params
 */
typedef struct {
    adi_apollo_cddc_cfg_t dp_cfg;            	/*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */

	adi_apollo_cddc_select_e cddc;				/*!< The CDDC single instance select enum (e.g. A0, B0, etc.) */
	
} adi_apollo_cddc_inspect_t;

#endif /* __ADI_APOLLO_CDDC_TYPES_H__ */
/*! @} */
