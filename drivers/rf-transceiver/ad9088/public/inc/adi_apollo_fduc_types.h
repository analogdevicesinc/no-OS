/*!
 * \brief     Fine DUC functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_FDUC
 * @{
 */
#ifndef __ADI_APOLLO_FDUC_TYPES_H__
#define __ADI_APOLLO_FDUC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_FDUC_NUM              16
#define ADI_APOLLO_FDUC_PER_SIDE_NUM     8
#define ADI_APOLLO_FDUC_INT_DLY_NUM      6

/*!
 * \brief Enumerates Tx fine DUC select
 */
typedef enum {
    ADI_APOLLO_FDUC_NONE = 0x0000,                       /*!< No FINE DUC */
    ADI_APOLLO_FDUC_A0 = 0x0001,                         /*!< FINE DUC 0 of SIDE A */
    ADI_APOLLO_FDUC_A1 = 0x0002,                         /*!< FINE DUC 1 of SIDE A */
    ADI_APOLLO_FDUC_A2 = 0x0004,                         /*!< FINE DUC 2 of SIDE A */
    ADI_APOLLO_FDUC_A3 = 0x0008,                         /*!< FINE DUC 3 of SIDE A */
    ADI_APOLLO_FDUC_A4 = 0x0010,                         /*!< FINE DUC 4 of SIDE A */
    ADI_APOLLO_FDUC_A5 = 0x0020,                         /*!< FINE DUC 5 of SIDE A */
    ADI_APOLLO_FDUC_A6 = 0x0040,                         /*!< FINE DUC 6 of SIDE A */
    ADI_APOLLO_FDUC_A7 = 0x0080,                         /*!< FINE DUC 7 of SIDE A */
    ADI_APOLLO_FDUC_B0 = 0x0100,                         /*!< FINE DUC 0 of SIDE B */
    ADI_APOLLO_FDUC_B1 = 0x0200,                         /*!< FINE DUC 1 of SIDE B */
    ADI_APOLLO_FDUC_B2 = 0x0400,                         /*!< FINE DUC 2 of SIDE B */
    ADI_APOLLO_FDUC_B3 = 0x0800,                         /*!< FINE DUC 3 of SIDE B */
    ADI_APOLLO_FDUC_B4 = 0x1000,                         /*!< FINE DUC 4 of SIDE B */
    ADI_APOLLO_FDUC_B5 = 0x2000,                         /*!< FINE DUC 5 of SIDE B */
    ADI_APOLLO_FDUC_B6 = 0x4000,                         /*!< FINE DUC 6 of SIDE B */
    ADI_APOLLO_FDUC_B7 = 0x8000,                         /*!< FINE DUC 7 of SIDE B */
    ADI_APOLLO_FDUC_ALL = 0xFFFF,                        /*!< All FINE DUCs */
    ADI_APOLLO_FDUC_A_ALL = 0x00FF,                      /*!< All A side FINE DUCs */
    ADI_APOLLO_FDUC_B_ALL = 0xFF00,                      /*!< All B side FINE DUCs */
    ADI_APOLLO_FDUC_ALL_4T4R = 0x0F0F,                   /*!< All FINE DUCs (4T4R) */
    ADI_APOLLO_FDUC_A_ALL_4T4R = 0x000F,                 /*!< All A side FINE DUCs (4T4R) */
    ADI_APOLLO_FDUC_B_ALL_4T4R = 0x0F00,                 /*!< All B side FINE DUCs (4T4R) */
} adi_apollo_fduc_select_e;

/*!
 * \brief Enumerates fine DUC integer delay select
 */
typedef enum {
    ADI_APOLLO_FDUC_INT_DLY_NONE = 0x0000,               /*!< No int delay */
    ADI_APOLLO_FDUC_INT_DLY_HB1 = 0x0001,                /*!< HB1 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_HB2 = 0x0002,                /*!< HB2 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_HB3 = 0x0004,                /*!< HB3 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_HB4 = 0x0008,                /*!< HB4 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_HB5 = 0x0010,                /*!< HB5 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_HB6 = 0x0020,                /*!< HB6 int delay select */
    ADI_APOLLO_FDUC_INT_DLY_ALL = 0x003F,                /*!< All int delays selected*/
} adi_apollo_fduc_int_tdly_select_e;

/*!
 * \brief Tx fine DUC configuration
 */
typedef struct {
    uint8_t     interp;                                 /*!< Interpolation value */
    uint8_t     sub_dp_gain_en;                         /*!< Enable sub data path gain. Set value subdp_gain param */
    uint16_t    subdp_gain;                             /*!< Sub data path 12-bit gain */
    uint8_t     int_tdly_hb;                            /*!< Integer time delay control mask for HB1-HB6. b0=HB1, B1=HB2, etc. */

    uint8_t     fduc_spien_en;                          /*!< Enables(1) or disables(0) SPI control of FDUC. Bypasses auto decode. (Typically disabled) */
    uint8_t     fduc_spi_en;                            /*!< Enable(1) or disable(0) the duc if fduc_spien_en is enabled */
} adi_apollo_fduc_pgm_t;

/*!
 * \brief Tx fine DUC inspect parameters
 */
typedef struct {
    adi_apollo_fduc_cfg_t dp_cfg;            	        /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
    
    uint8_t     int_tdly_hb;                            /*!< Integer time delay control mask for HB1-HB6. b0=HB1, B1=HB2, etc. */
    uint8_t     fduc_spien_en;                          /*!< Enables(1) or disables(0) SPI control of FDUC. Bypasses auto decode. (Typically disabled) */
    uint8_t     fduc_spi_en;                            /*!< Enable(1) or disable(0) the duc if fduc_spien_en is enabled */
} adi_apollo_fduc_inspect_t;

#endif /* __ADI_APOLLO_FDUC_TYPES_H__ */
/*! @} */
