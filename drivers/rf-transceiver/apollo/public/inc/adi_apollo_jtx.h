/*!
 * \brief     JESD TX Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_JTX
 * @{
 */
#ifndef __ADI_APOLLO_JTX_H__
#define __ADI_APOLLO_JTX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_jtx_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Enable or Disable jtx link
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links      Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  link_en    1:Enable, 0:Disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_link_enable_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t link_en);

/**
 * \brief  Get jtx link status
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  link         Target link select \ref adi_apollo_jesd_link_select_e
 * \param[out]  status      Pointer to jtx status
 *                              bit[3:0] - QBF status
 *                              bit4     - frame sync status
 *                              bit5     - jtx pll locked
 *                              bit6     - phase established
 *                              bit7     - jtx invalid mode
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_link_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status);

/**
 * \brief  Inspect Jtx link param data from device
 *
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links       Target link. \ref adi_apollo_jesd_link_select_e
 * \param[out]  jtx_inspect Pointer to jtx inspect structure. \ref adi_apollo_jesd_tx_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_link_inspect(adi_apollo_device_t *device,
    const uint16_t links,
    adi_apollo_jesd_tx_inspect_t *jtx_inspect);

/**
 * \brief  Output LMFC phase adjustment in conv_clk cycles.
 *
 * There is a phase counter in each link. It is reset by each new SYSREF signal.
 * Thereafter, it increments on each conv_clk cycle.
 *
 * There are K*S/NS conv_clk cycles in an LEMC/LMFC. Therefore, the counter counts
 * from 0 to K*S/NS-1 and wraps around.
 *
 * \remarks This API typically for debug purposes. Set to 0 for normal operation.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links           Target link. \ref adi_apollo_jesd_link_select_e
 * \param[in]   phase_adjust    Maximum value is k*s/ns-1. Set to 0 for normal operation.
 *s
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_phase_adjust_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint16_t phase_adjust);

/**
 * \brief  Configure the JESD Tx lane cross bar between physical lane and logical lane
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  physical_lane   Physical lane index (0~11)
 * \param[in]  logical_lane    Logical lane index (0~11)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_xbar_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t physical_lane, uint8_t logical_lane);

/**
 * \brief  Configure the JESD Tx lanes cross bar between physical lane and logical lane
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link select \ref adi_apollo_jesd_link_select_e
 * \param[in]  logical_lanes   Logical lanes index (0~11 for each value)
 * \param[in]  length          Logical lanes index array size
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lanes_xbar_set(adi_apollo_device_t *device, const uint16_t links,
    uint8_t logical_lanes[], uint32_t length);

/**
 * \brief  Configure the JESD Tx (JTx) subclass mode
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  subclass        Link subclass operation mode. 0 = subclass 0, 1 = subclass 1
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_subclass_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint16_t subclass);

/**
 * \brief  Enable or disable 'force_power_down' on a particular lane for Tx
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]  physical_lane   Physical lane index (0~11)
 * \param[in]  power_down      1:Force power down, 0:Not force power down
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_force_pd_set(adi_apollo_device_t *device,
    const uint16_t link_sides,
    uint8_t physical_lane,
    uint8_t power_down);

/**
 * \brief  Set Output Drive Swing level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[in]   drive_swing     Output drive swing level of serdes lane \ref adi_apollo_ser_swing_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_drive_swing_set(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t drive_swing);

/**
 * \brief  Set Pre-Emphasis level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[in]   pre_emp         Pre-Emphasis level of serdes lane \ref adi_apollo_ser_pre_emp_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_pre_emphasis_set(adi_apollo_device_t *device,
                                             uint8_t link_sides,
                                             uint8_t physical_lane,
                                             uint8_t pre_emp);

/**
 * \brief  Set Post-Emphasis level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[in]   post_emp        Post-Emphasis level of serdes lane \ref adi_apollo_ser_post_emp_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_post_emphasis_set(adi_apollo_device_t *device,
                                              uint8_t link_sides,
                                              uint8_t physical_lane,
                                              uint8_t post_emp);

/**
 * \brief  Set swing/pre emphasis/post emphasis levels for multiple serdes lane.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Array of physical lane indexes (0~11)
 * \param[in]   drive_swing     Array of output drive swing levels of serdes lanes \ref adi_apollo_ser_swing_e
 * \param[in]   pre_emp         Array of pre-Emphasis levels of serdes lanes \ref adi_apollo_ser_pre_emp_e
 * \param[in]   post_emp        Array of Post-Emphasis levels of serdes lanes \ref adi_apollo_ser_post_emp_e
 * \param[in]   num_lanes       Number of serdes lane to set levels of
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_multi_lane_swing_emphasis_set(adi_apollo_device_t *device,
                                                uint8_t link_sides,
                                                uint8_t physical_lane[],
                                                uint8_t drive_swing[],
                                                uint8_t pre_emp[],
                                                uint8_t post_emp[],
                                                uint32_t num_lanes);
/**
 * \brief  Get Output Drive Swing level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[out]  drive_swing     Pointer to an 8-bit variable to which the value of
 *                              output drive swing level of serdes lane be returned \ref adi_apollo_ser_swing_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_drive_swing_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *drive_swing);

/**
 * \brief  Get jtx lane rate adapt values.
 *
 * \param[in]   device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides       Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane    Physical lane index (0~11)
 * \param[out]  lr_adapt         Jtx lane rate adapt \ref adi_apollo_serdes_lr_adapt_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lr_adapt_get(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t *lr_adapt);
/**
 * \brief  Get Pre-Emphasis level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[out]  pre_emp         Pointer to an 8-bit variable to which the value of
 *                              pre-emphasis level of serdes lane be returned \ref adi_apollo_ser_pre_emp_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_pre_emphasis_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *pre_emp);

/**
 * \brief  Get Post-Emphasis level for jtx serdes lanes.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[out]  post_emp        Pointer to an 8-bit variable to which the value of
 *                              post-emphasis level of serdes lane be returned \ref adi_apollo_ser_post_emp_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lane_post_emphasis_get(adi_apollo_device_t *device,
                                            uint8_t link_sides,
                                            uint8_t physical_lane,
                                            uint8_t *post_emp);

/**
 * \brief  Get swing/pre emphasis/post emphasis levels for multiple serdes lane.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Array of physical lane index (0~11)
 * \param[out]  drive_swing     Array of 8-bit variable to which the value of drive swing
 *                              output drive swing level of serdes lane be returned \ref adi_apollo_ser_swing_e
 * \param[out]  pre_emp         Array of 8-bit variable to which the value of
 *                              pre-emphasis level of serdes lane be returned \ref adi_apollo_ser_pre_emp_e
 * \param[out]  post_emp        Array of 8-bit variable to which the value of post emp
 *                              post-emphasis level of serdes lane be returned \ref adi_apollo_ser_post_emp_e
 * \param[in]   num_lanes       Number of serdes lane to get levels of
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_multi_lane_swing_emphasis_get(adi_apollo_device_t *device,
                                                uint8_t link_sides,
                                                uint8_t physical_lane[],
                                                uint8_t drive_swing[],
                                                uint8_t pre_emp[],
                                                uint8_t post_emp[],
                                                uint32_t num_lanes);

/**
 * \brief  Force sending invalid samples as part of FSRC reconfig flow.
 * 
 * Invalid forcing is auto cleared by device after reconfig completes.
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  enable          0 = don't force invalid samples, 1 = force invalids
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_force_invalids_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint8_t enable);

/**
 * \brief  Set jtx lane rate adapt values.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[in]   lr_adapt        Jtx lane rate adapt \ref adi_apollo_serdes_lr_adapt_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jtx_lr_adapt_set(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t lr_adapt);

#ifndef CLIENT_IGNORE
uint32_t calc_jtx_dual_link_base(int32_t link);
uint32_t calc_jtx_qbf_txfe_base(int32_t link);
uint32_t calc_jtx_jesd_param_base(int32_t link);
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_JTX_H__ */
/*! @} */
