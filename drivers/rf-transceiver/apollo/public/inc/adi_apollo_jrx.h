/*!
 * \brief     JRX BLOCK definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_JRX
 * @{
 */
#ifndef __ADI_APOLLO_JRX_H__
#define __ADI_APOLLO_JRX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_jrx_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Enable or disable the JESD link for Rx
 *
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links      Target link from adi_apollo_jesd_link_select_e
 * \param[in]  link_en    1b'0 -  LINK_POWER_DOWN,
 *                        1b'1 -  LINK_ENABLE
 *
 * \return API_CMS_ERROR_OK       API Completed Successfully
 * \return <0                     Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_link_enable_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t link_en);

/**
 * \brief  Get current jrx 204C lane status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link from adi_apollo_jesd_link_select_e
 * \param[in]   lane       Target serdes lane. Valid values from 0 to ADI_APOLLO_JESD_DESER_COUNT-1 \ref adi_apollo_common_types.h
 * \param[out]  status     Pointer to jrx 204c lane status
 *                              bit[2:0]    -   Current Link State.
 *                                               3'b0    :   ENUM000 :   RESET.
 *                                               3'b1    :   ENUM001 :   UNLOCKED. Complete Sync Header (SH) detection and alignment.
 *                                               3'b10   :   ENUM002 :   BLOCK. Complete the lane de-skew and lane alignment.
 *                                               3'b11   :   ENUM003 :   DESKEW. Complete Multi-Block detection and alignment.
 *                                               3'b100  :   ENUM004 :   M_BLOCK. Complete Extended Multi-block detection.
 *                                               3'b101  :   ENUM005 :   E_M_BLOCK. Complete Extended Multi-block alignment.
 *                                               3'b110  :   ENUM006 :   Link Ready. Link is up and running.
 *                                               3'b111  :   ENUM007 :   FORCED. A lock state by asserting bit-field jrx_dl_204C_link_lock=1
 *                              bit3        -   Reserved
 *                              bit4        -   Link PCLK slower copmare to conv_clk error. 0=NORMAL, 1=PCLK_SLOWER
 *                              bit5        -   Link PCLK faster copmare to conv_clk error. 0=NORMAL, 1=PCLK_FASTER
 *                              bit6        -   reserved
 *                              bit7        -   reserved
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204c_lane_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status);

/**
 * \brief  Get current jrx link status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link side from adi_apollo_jesd_link_select_e
 * \param[out]  status     Pointer to jrx link status
 *                              bit0    -   link JESD mode. 0=CORRECT, 1=ERROR
 *                              bit1    -   link NS Down_Scale_Ratio. 0=NO_OVERFLOW, 1=OVERFLOW
 *                              bit2    -   reserved
 *                              bit3    -   reserved
 *                              bit4    -   reserved
 *                              bit5    -   user data ready. 1=ready, 0=not ready.
 *                              bit6    -   sysref phase established. 1=established, 0=not established
 *                              bit7    -   core config invalid. 1=invald, 0=valid
 * \return API_CMS_ERROR_OK                 API Completed Successfully
 * \return <0                               Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_link_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status);

/**
 * \brief  Get current jrx 204B link status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link adi_apollo_jesd_link_select_e
 * \param[in]   lane       Target serdes lane. Valid values from 0 to ADI_APOLLO_JESD_DESER_COUNT-1 \ref adi_apollo_common_types.h
 * \param[out]  status     Pointer to jrx link status
 *                              bit0    -   the End of Frame
 *                              bit1    -   the End of Multi-Frame
 *                              bit2    -   frame sync lost
 *                              bit3    -   sync~ output from link layer
 *                              bit4    -   data payload available. 0=data not ready, 1=data ready
 *                              bit5    -   checksum status. 0=bad checksum, 1=good checksum
 *                              bit6    -   reserved
 *                              bit7    -   reserved
 * \return API_CMS_ERROR_OK                 API Completed Successfully
 * \return <0                               Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204b_lane_status_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status);


/**
 * \brief  Get jrx lane rate adapt values.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[out]  lr_adapt        Jrx lane rate adapt \ref adi_apollo_serdes_lr_adapt_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_lr_adapt_get(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t *lr_adapt);

/**
 * \brief  Get current jrx 204B link error status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link adi_apollo_jesd_link_select_e
 * \param[in]   lane       Target serdes lane. Valid values from 0 to ADI_APOLLO_JESD_DESER_COUNT-1 \ref adi_apollo_common_types.h
 * \param[out]  status     Pointer to jrx link status
 *                              bit0    -   bad disparity error. 0=NORMAL, 1=ERROR
 *                              bit1    -   code group sync (CGS) error. 0=FAIL, 1=PASS
 *                              bit2    -   checksum error. 0=FAIL, 1=PASS
 *                              bit3    -   frame sync error. 0=FAIL, 1=PASS
 *                              bit4    -   interlane de-scew error. 0=NORMAL, 1=ERROR
 *                              bit5    -   interlane synchronization status. 0=NORMAL, 1=ERROR
 *                              bit6    -   not in table error status. 0=NORMAL, 1=ERROR
 *                              bit7    -   unexpected K-character error status. 0=NORMAL, 1=ERROR
 * \return API_CMS_ERROR_OK                 API Completed Successfully
 * \return <0                               Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204b_lane_error_get(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    uint16_t *status);

/**
 * \brief  Get current jrx rm fifo status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link from adi_apollo_jesd_link_select_e
 * \param[out]  status     Pointer to jrx rm_fifo status
 *                              bit[1:0]    -   RM_FIFO empty status
 *                                              2'b00   :   Not_Empty   :   Both Slice RM Not Empty.
 *                                              2'b01   :   Slice0_RM_Empty :   Only Slice0 RM Empty.
 *                                              2'b10   :   SLICE1_RM_EMPTY :   Only Slice1 RM Empty.
 *                                              2'b11   :   Both_RM_EMPTY   :   Both Slice RM Empty.
 *                              bit[3:2]    -   RM_FIFO full status
 *                                              2'b00   :   Not_Full    :   No RM Full.
 *                                              2'b01   :   Slice0_RM_Full  :   Only Slice0 RM Full.
 *                                              2'b10   :   Slice1_RM_Full  :   Only Slice1 RM Full.
 *                                              2'b11   :   Both_RM_Full    :   Both Slice RM Full.
 *                              bit4        -   Invalid sample error flag
 *                              bit[7:5]    -   reserved
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_rm_fifo_status(adi_apollo_device_t *device,
    const uint16_t link,
    uint16_t *status);

/**
 * \brief  Reset the JRx rate match fifo
 *
 * This can be called to clean up DAC output when FSRC is enabled.
 * 
 * \param[in]  device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]  link_sides Target link side \ref adi_apollo_jesd_link_side_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_rm_fifo_reset(adi_apollo_device_t *device,
    const uint16_t link_sides);

/**
 * \brief  Get current jrx lane fifo status
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link       Target link from adi_apollo_jesd_link_select_e
 * \param[out]  status     Pointer to jrx lane fifo status
 *                              bit[11:0]   -   lane fifo empty flag for data from SERDINx.
 *                              bit[27:16]  -   lane fifo full flag for data from SERDINx.
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_lane_fifo_status(adi_apollo_device_t *device,
    const uint16_t link,
    uint32_t *status);

/**
 * \brief  clear all pclk error counters
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links       Target link side from adi_apollo_jesd_link_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_clear_pclk_errors(adi_apollo_device_t *device,
    const uint16_t links);

/**
 * \brief  clear all fec error counters
 * 
 * \deprecated Use \ref adi_apollo_jrx_j204c_irq_clear() instead.
 *             This will be removed in the future releases.
 * 
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links      Target link adi_apollo_jesd_link_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_clear_fec_errors(adi_apollo_device_t *device,
    const uint16_t links);

/**
 * \brief  Get current jrx FEC error counters
 * 
 * \deprecated Use \ref adi_apollo_jrx_j204c_irq_get() instead.
 *             This will be removed in the future releases.
 *
 * \param[in]   device     Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link      Target link. Only 1 link selection per call. adi_apollo_jesd_link_select_e
 * \param[in]   lane       Target serdes lane. Valid values from 0 to ADI_APOLLO_JESD_DESER_COUNT-1 \ref adi_apollo_common_types.h
 * \param[out]  fec_errors Pointer to jrx fec errors structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_fec_errors(adi_apollo_device_t *device,
    const uint16_t link,
    const uint16_t lane,
    adi_apollo_fec_errors_t *fec_errors);

/**
 * \brief  Inspect Jrx link param data from device
 *
 * \param[in]   device      Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links       Target link. Only 1 link selection per call \ref adi_apollo_jesd_link_select_e
 * \param[out]  jrx_inspect Pointer to jrx inspect structure. \ref adi_apollo_jesd_rx_inspect_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_link_inspect(adi_apollo_device_t *device,
    const uint16_t links,
    adi_apollo_jesd_rx_inspect_t *jrx_inspect);

/**
 * \brief  Set LMFC phase adjustment in conv_clk cycles.
 *
 * There is a phase counter in each link. It is reset by each new SYSREF signal.
 * Thereafter, it increments on each conv_clk cycle.
 *
 * There are K*S/NS conv_clk cycles in an LEMC/LMFC. Therefore, the counter counts
 * from 0 to K*S/NS-1 and wraps around.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   links           Target link. \ref adi_apollo_jesd_link_select_e
 * \param[in]   phase_adjust    Maximum value is k*s/ns-1. Set to 0 for normal operation.
 *s
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_phase_adjust_set(adi_apollo_device_t* device,
    const uint16_t links,
    const uint16_t phase_adjust);

/**
 * \brief Get LMFC phase adjustment in conv_clk cycles.
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[out] phase_adjust    Pointer to phase_adjust result.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_phase_adjust_get(adi_apollo_device_t *device,
                                        const uint16_t links,
                                        uint16_t *phase_adjust);

/**
 * \brief  Calculates the jrx phase adjustment.
 *
 * The arrival time of local LMFC/LEMC can be different across links. This function
 * determines the phase differences across links and calculates an adjustment value that
 * can be applied with \ref adi_apollo_jrx_phase_adjust_set().
 * 
 * Given that delay or skew can vary some between power on and across temperature/environment,
 * a margin can be added. Default is ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT.
 * 
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target links.
 * \param[in]  margin          Margin for particular system. Default is ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT
 * \param[out] phase_adjust    Maximum value is k*s/ns-1.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_phase_adjust_calc(adi_apollo_device_t *device, const uint16_t links, uint16_t margin, uint16_t *phase_adjust);

/**
 * \brief  Configure the JESD Rx lane cross bar between physical lane and logic lane
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  physical_lane   Physical lane index (0~11)
 * \param[in]  logical_lane    Logical lane index
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_lane_xbar_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t physical_lane,
    uint8_t logical_lane);

/**
 * \brief  Configure the JESD Rx lanes cross bar between physical lane and logic lane
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  logical_lanes   Logical lane index
 * \param[in]  length          Logical lane index array size
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_lanes_xbar_set(adi_apollo_device_t *device,
    const uint16_t links,
    uint8_t logical_lanes[],
    uint32_t length);

/**
 * \brief  Set jrx lane rate adapt values.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   link_sides      Target link side \ref adi_apollo_jesd_link_side_select_e
 * \param[in]   physical_lane   Physical lane index (0~11)
 * \param[in]   lr_adapt        Jrx lane rate adapt \ref adi_apollo_serdes_lr_adapt_ratio_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_lr_adapt_set(adi_apollo_device_t *device,
                                        uint8_t link_sides,
                                        uint8_t physical_lane,
                                        uint8_t lr_adapt);

/**
 * \brief Gets the number of conv clock cycles since last SYSREF, phase_diff
 *
 * The largest phase difference of all links is for the latest arrival local LMFC/LEMC.
 * Reset by each SYSREF.
 * The counter counts from 0 to K*S/NS-1 and wraps around.
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[out] phase_diff      Pointer to phase_diff result
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_phase_diff_get(adi_apollo_device_t* device,
    const uint16_t links,
    uint16_t *phase_diff);

/**
 * \brief  Configure the JESD Rx (JRx) subclass mode
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  subclass        Link subclass operation mode. 0 = subclass 0, 1 = subclass 1
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_subclass_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint16_t subclass);

/**
 * \brief  Enable JRx 204C IRQs
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  irqs            IRQ bitmask \ref adi_apollo_jrx_j204c_irq_e
 * \param[in]  enable          1 = Enable. 0 = Disable
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204c_irq_enable_set(adi_apollo_device_t *device,
    const uint16_t links,
    const uint32_t irqs, uint8_t enable);

/**
 * \brief  Get enable status for JRx 204C IRQs
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[out] irqs_enabled    IRQ enable status bitmask. Bits defined by \ref adi_apollo_jrx_j204c_irq_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204c_irq_enable_get(adi_apollo_device_t *device,
                                            const uint16_t links,
                                            uint32_t *irqs_enabled);

/**
 * \brief  Get status of JRx 204C IRQs
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target links. if links > 1, 'status' is combination of all links or'd together
 * \param[in]  irqs            IRQ bitmask \ref adi_apollo_jrx_j204c_irq_e
 * \param[in]  clear           If true, then the interrupts will be cleared after reading
 * \param[out] status          IRQ status bitmask. Bits defined by \ref adi_apollo_jrx_j204c_irq_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204c_irq_get(adi_apollo_device_t *device,
    const uint16_t links, const uint32_t irqs, 
    uint8_t clear, uint32_t *status);

/**
 * \brief  Clear JRx 204C IRQ sticky bits
 *
 * \param[in]  device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]  links           Target link
 * \param[in]  irqs            IRQ bitmask \ref adi_apollo_jrx_j204c_irq_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_jrx_j204c_irq_clear(adi_apollo_device_t *device,
    const uint16_t links,
    const uint32_t irqs);

#ifndef CLIENT_IGNORE
	
int32_t adi_apollo_jesd_rx_sample_repeat_en(adi_apollo_device_t *device,
	const uint16_t link_sides,
	uint8_t enable);
	
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_JRX_H__ */
/*! @} */
