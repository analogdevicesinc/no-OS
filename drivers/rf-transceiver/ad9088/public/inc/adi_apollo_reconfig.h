/*!
 * \brief     RECONFIG Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RECONFIG
 * @{
 */
#ifndef __ADI_APOLLO_RECONFIG_H__
#define __ADI_APOLLO_RECONFIG_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_reconfig_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Configure reconfig controller modes of trigger, coherence, reconfiguration interval etc.
 *
 * \param[in]  device              Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal            Target terminal \ref adi_apollo_terminal_e
 * \param[in]  sides               Target side  \ref adi_apollo_side_select_e
 * \param[in]  config              \ref adi_apollo_reconfig_ctrl_pgm_t
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_reconfig_ctrl_pgm(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint8_t sides, adi_apollo_reconfig_ctrl_pgm_t *config);

/**
 * \brief  Gets a reconfig controller's internal or trigger sync event counter. 
 *	
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  terminal         Target terminal (ADI_APOLLO_RX, _TX) \ref adi_apollo_terminal_e
 * \param[in]  sides            Target side select (ADI_APOLLO_SIDE_A, _B). One side per call. \ref adi_apollo_side_select_e
 * \param[in]  rec_type         Reconfig controller trigger type (ADI_APOLLO_RECONFIG_EXTERNAL, _INTERNAL) \ref adi_apollo_reconfig_type_e
 * \param[out] count			Pointer to reconfig event count return val. This is a 4-bit value that rolls over.
 *
 * \return API_CMS_ERROR_OK    API Completed Successfully
 * \return <0                  Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_reconfig_trig_evt_cnt_get(adi_apollo_device_t *device, adi_apollo_terminal_e terminal, uint8_t sides, adi_apollo_reconfig_type_e rec_type, uint16_t *count);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_RECONFIG_H__ */
/*! @} */
