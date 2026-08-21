/*!
 * \brief     Coarse DUC functional block definitions
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CDUC
 * @{
 */
#ifndef __ADI_APOLLO_CDUC_TYPES_H__
#define __ADI_APOLLO_CDUC_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_CDUC_NUM              8
#define ADI_APOLLO_CDUC_PER_SIDE_NUM     4

/*!
 * \brief Enumerates TX coarse DUC select
 */
typedef enum {
    ADI_APOLLO_CDUC_NONE = 0x00,                         /*!< No COARSE DUC */
    ADI_APOLLO_CDUC_A0 = 0x01,                           /*!< COARSE DUC 0 of SIDE A */
    ADI_APOLLO_CDUC_A1 = 0x02,                           /*!< COARSE DUC 1 of SIDE A */
    ADI_APOLLO_CDUC_A2 = 0x04,                           /*!< COARSE DUC 2 of SIDE A */
    ADI_APOLLO_CDUC_A3 = 0x08,                           /*!< COARSE DUC 3 of SIDE A */
    ADI_APOLLO_CDUC_B0 = 0x10,                           /*!< COARSE DUC 0 of SIDE B */
    ADI_APOLLO_CDUC_B1 = 0x20,                           /*!< COARSE DUC 1 of SIDE B */
    ADI_APOLLO_CDUC_B2 = 0x40,                           /*!< COARSE DUC 2 of SIDE B */
    ADI_APOLLO_CDUC_B3 = 0x80,                           /*!< COARSE DUC 3 of SIDE B */
    ADI_APOLLO_CDUC_ALL = 0xFF,                          /*!< All COARSE DUCs */
    ADI_APOLLO_CDUC_ALL_4T4R = 0x33                      /*!< All COARSE DUCs (4T4R) */
} adi_apollo_cduc_select_e;

/*!
 * \brief Tx coarse DUC configuration
 */
typedef struct {
    uint8_t interp;                                     /*!< Interpolation value */
    uint8_t chb1_int_time_dly;                          /*!< Coarse HB1 int time delay control */
    uint8_t chb2_int_time_dly;                          /*!< Coarse HB2 int time delay control */
    uint8_t chb3_int_time_dly;                          /*!< Coarse HB3 int time delay control */
    uint8_t ctb1_int_time_dly;                          /*!< Coarse TB1 int time delay control */
    uint8_t test_mux;                                   /*!< Test signal mux */
    uint16_t cduc_irq_en;                               /*!< Coarse DUC irq enable */
    uint16_t cduc_irq_status;                           /*!< Coarse DUC irq status (write to clear status) */

    uint8_t cduc_spien_en;                              /*!< Enables(1) or disables(0) SPI control of CDUC. Bypasses auto decode. (Typically disabled) */
    uint8_t cduc_spi_en;                                /*!< Enable(1) or disable(0) the duc if cduc_spien_en is enabled */
} adi_apollo_cduc_pgm_t;                              

/*!
 * \brief CDUC inspect params
 */
typedef struct {
    adi_apollo_cduc_cfg_t dp_cfg;                       /*!< Parameters defined from device profile \ref apollo_cpu_device_profile_types.h */
    uint8_t chb1_int_time_dly;                          /*!< Coarse HB1 int time delay control */
    uint8_t chb2_int_time_dly;                          /*!< Coarse HB2 int time delay control */
    uint8_t chb3_int_time_dly;                          /*!< Coarse HB3 int time delay control */
    uint8_t ctb1_int_time_dly;                          /*!< Coarse TB1 int time delay control */
    uint8_t test_mux;                                   /*!< Test signal mux */
    uint16_t cduc_irq_en;                               /*!< Coarse DUC irq enable */
    uint16_t cduc_irq_status;                           /*!< Coarse DUC irq status (write to clear status) */

    uint8_t cduc_spien_en;                              /*!< Enables(1) or disables(0) SPI control of CDUC. Bypasses auto decode. (Typically disabled) */
    uint8_t cduc_spi_en;                                /*!< Enable(1) or disable(0) the duc if cduc_spien_en is enabled */
} adi_apollo_cduc_inspect_t;

#endif /* __ADI_APOLLO_CDUC_TYPES_H__ */
/*! @} */
