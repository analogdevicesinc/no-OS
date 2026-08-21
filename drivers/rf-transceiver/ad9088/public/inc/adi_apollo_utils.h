/*!
 * \brief     Apollo API utilities
 *
 * \copyright copyright(c) 2023 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_UTILS
 * @{
 */
#ifndef __ADI_APOLLO_UTILS_H__
#define __ADI_APOLLO_UTILS_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the side select for selected adcs
 *
 * \param[in]   device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]   adc_select           Target adcs \ref adi_apollo_adc_select_e
 * \param[out]  side_select          Target sides \ref adi_apollo_side_select_e
 *
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_utils_side_from_adc_select_get(adi_apollo_device_t *device, uint16_t adc_select, uint16_t *side_select);

/**
 * \brief Get the side select for selected dacs
 *
 * \param[in]   device               Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dac_select           Target dacs \ref adi_apollo_dac_select_e
 * \param[out]  side_select          Target sides \ref adi_apollo_side_select_e
 *
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_utils_side_from_dac_select_get(adi_apollo_device_t *device, uint16_t dac_select, uint16_t *side_select);


#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE */

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_UTILS_H__ */
/*! @} */
