/*!
 * \brief     DAC definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DAC
 * @{
 */
#ifndef __ADI_APOLLO_DAC_H__
#define __ADI_APOLLO_DAC_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_dac_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set internal dac core trim attributes (deliberately vague)
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs            Target DAC selector \ref adi_apollo_dac_select_e
 * \param[in]   trim_attr       Clock trim attribute. \ref adi_apollo_dac_clk_trim_e
 * \param[in]   trim_val        Trim value
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details
 */
int32_t adi_apollo_dac_clk_trim_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_dac_clk_trim_e trim_attr, uint8_t trim_val);

/**
 * \brief Get an internal dac core trim attribute (deliberately vague)
 * \note  Only 1 DAC selection per call
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs            Target DAC selector. Only one dac selected at a time. \ref adi_apollo_dac_select_e
 * \param[in]   trim_attr       Clock trim attribute. \ref adi_apollo_dac_clk_trim_e
 * \param[out]  trim_val        Trim value return
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details
 */
int32_t adi_apollo_dac_clk_trim_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, adi_apollo_dac_clk_trim_e trim_attr, uint8_t *trim_val);

/**
 * \brief  Enable or disable tx datapath scrambler to avoid substrate noise coupling
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  scrambler_sel    DAC Scrambler selection. \ref adi_apollo_dac_select_e
 * \param[in]  enable           1: to enable scrambler, 0: to disable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dac_scrambler_enable_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t scrambler_sel, uint8_t enable);

/**
 * \brief  Get state of tx datapath scrambler
 * \note   Only one scrambler channel per call. Select one within ADI_APOLLO_DAC_A0 - ADI_APOLLO_DAC_B3.
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  scrambler_sel    DAC Scrambler selection. \ref adi_apollo_dac_select_e
 * \param[out] enable           Enable flag. 0: disable, 1: enable
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_dac_scrambler_enable_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t scrambler_sel, uint8_t *enable);

/**
 * \brief Enable or disable internal dac data (deliberately vague)
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs            Target DAC selector \ref adi_apollo_dac_select_e
 * \param[in]   enable          1. Enable DAC data. 0. Disable DAC data.
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details
 */
int32_t adi_apollo_dac_data_enable(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t enable);

/**
 * \brief Locks out or unlock DAC's Standby control
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs            Target DAC selector \ref adi_apollo_dac_select_e
 * \param[in]   lock_state      1: Locks out DAC's Standby control ensuring DACs are always ON, (Default)
 *                              0: Unlocks DAC's Standby control so DACs can be toggled in or out of STANDBY using either SPI or GPIO control
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details
 */
int32_t adi_apollo_dac_standby_lock_set(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t lock_state);

/**
 * \brief Get DAC's Standby Control lock state
 * \note  Only 1 DAC selection per call
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   dacs            Target DAC selector \ref adi_apollo_dac_select_e
 * \param[out]  lock_state      1: DAC's Standby control is locked and DAC can't be configured into STANDBY
 *                              0: DAC's Standby control is unlocked and DACs can be toggled in or out of STANDBY using either SPI or GPIO control
 *
 * \return API_CMS_ERROR_OK     API Completed Successfully
 * \return <0                   Failed. \ref adi_cms_error_e for details
 */
int32_t adi_apollo_dac_standby_lock_get(adi_apollo_device_t *device, adi_apollo_blk_sel_t dacs, uint8_t *lock_state);

#ifdef __cplusplus
}
#endif

#endif /*__ADI_APOLLO_DAC_H__ */
/*! @} */
