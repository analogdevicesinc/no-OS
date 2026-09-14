/*!
 * \brief:   Apollo private system clock calibration functions
 *
 * \copyright copyright(c) 2023 Analog Devices, Inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated Analog Devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SYSCLK_COND
 * @{
 */
#ifndef __ADI_APOLLO_PRIVATE_SYSCLK_COND_H__
#define __ADI_APOLLO_PRIVATE_SYSCLK_COND_H__

#include "adi_apollo_sysclk_cond_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Sets the ADC clock conditioning firmware defaults.
 *
 * \note Should be called after firmware load and before device profile load.
 *
 * \param[in]  device   Context variable - Pointer to the APOLLO device data structure
 * \param[in]  clk_cfg                     Pointer to \ref adi_apollo_clk_cfg_t used to determine CC defaults.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_private_sysclk_cond_defaults_set(adi_apollo_device_t *device, adi_apollo_clk_cfg_t *clk_cfg);

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_PRIVATE_SYSCLK_COND_H__ */
/*! @} */
