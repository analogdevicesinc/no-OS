/*!
 * \brief     Rx Misc functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RXMISC
 * @{
 */
#ifndef __ADI_APOLLO_RXMISC_H__
#define __ADI_APOLLO_RXMISC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_rxmisc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sets low sampling rx mode
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  side_sel             Target side \ref adi_apollo_side_select_e
 * \param[in]  low_samp_en      1: low sampling enabled 0: low sampling disabled
 * 
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmisc_low_samp_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t low_samp_en);

/**
 * \brief  Configure RX MISC parameters
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel   Target side \ref adi_apollo_side_select_e
 * \param[in]  config     \ref adi_apollo_rx_misc_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmisc_pgm(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, adi_apollo_rx_misc_pgm_t *config);

/**
 * \brief Rx datapath (DP) reset. 
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure 
 * \param[in]  side_sel             Target side \ref adi_apollo_side_select_e
 * \param[in]  reset_en             0: clears the datapath reset. 1: set DP reset
 * 
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_rxmisc_dp_reset(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t reset_en);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RXMISC_H__ */
/*! @} */
