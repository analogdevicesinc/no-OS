/*!
 * \brief     Tx Misc functional block API prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMISC
 * @{
 */
#ifndef __ADI_APOLLO_TXMISC_H__
#define __ADI_APOLLO_TXMISC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_txmisc_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sets low sampling tx mode
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel             Target side \ref adi_apollo_side_select_e
 * \param[in]  low_samp_en      1: low sampling enabled 0: low sampling disabled
 *
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmisc_low_samp_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t low_samp_en);

/**
 * \brief Tx datapath (DP) reset.
 *
 * \param[in]  device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel             Target side \ref adi_apollo_side_select_e
 * \param[in]  reset_en             0: clears the dayapath reset. 1: set DP reset
 *
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmisc_dp_reset(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, uint8_t reset_en);

/**
 * \brief  Inspect TX crossbars and low sample enable
 *
 * \param[in]   device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]   side                Target side \ref adi_apollo_side_select_e
 * \param[out]  txmisc_inspect      Pointer to tx path inspect struct \ref adi_apollo_txpath_inspect_t
 *
 * \return API_CMS_ERROR_OK                     Completed successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmisc_inspect(adi_apollo_device_t *device, uint16_t side, adi_apollo_txmisc_inspect_t *txmisc_inspect);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TXMISC_H__ */
/*! @} */
