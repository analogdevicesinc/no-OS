/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file: adi_fpgagen6_clk.h
 *
 * \brief Contains top level fpgagen6 clock related API functions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_CLK_H_
#define _ADI_FPGAGEN6_CLK_H_

#include "adi_fpgagen6_clk_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief The MMCM clock module provides the JESD lane-rate parallel clock rate to all
*        the link layer modules in the FPGA. It also provides the state-machine clock
*        for the TDD state machine module.
*        
*        After a new configuration is written to the MMCM module dynamically using this
*        API function the MMCM will be ready for link-bringup. Checking the lock bit
*        prior to bringup is required to verify the settings and module are in the correct
*        state.
* 
*        This function writes out the MMCM link-layer clock module configuration
*        defined in the adi_fpgagen6_MmcmCfg_t structure pointed to by mmcm input
*        parameter.
*        
*        The adi_fpgagen6_MmcmCfg_t structure can be populated by using the utility
*        layer adi_fpgagent_MmcmConfigure function using clock frequencies derived
*        from the profile JESD settings.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] mmcmSel selection of the mmcm device to config
* \param[in] mmcmCfgSet is a adi_fpgagen6_MmcmCfg_t used to configure the mmcm
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmCfgSet(   adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_MmcmSel_e        mmcmSel,
                                                            const adi_fpgagen6_MmcmCfg_t* const mmcmCfgSet);

/**
* \brief This function reads the settings in the selected MMCM link-layer clock
*        module and returns them in the adi_fpgagen6_MmcmCfg_t structure pointed
*        to by the mmcm input parameter.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] mmcmSel selection of the mmcm device to config
* \param[in,out] mmcmCfgGet is a adi_fpga_MmcmCfg_t used to configure the mmcm
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmCfgGet(   adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_MmcmSel_e    mmcmSel,
                                                            adi_fpgagen6_MmcmCfg_t* const   mmcmCfgGet);

/**
* \brief Reads the lock state of the selected MMCM module and writes the value
*        to the byte pointed to by lockBit input parameter. 
*
* \dep_begin
* \dep{device}
* \dep{mmcm}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] mmcmSel selection of the mmcm device to config, rx or tx
* \param[out] lockBit Pointer to return lock bit value
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmLockGet(  adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_MmcmSel_e    mmcmSel,
                                                            uint8_t* const                  lockBit);

/**
* \brief    Peforms a soft reset on the selected MMCM. The reset is self-clearing. After
*           the MMCM module is reset using this function it is necessary to call
*           adi_fpgagen6_MmcmCfgSet with a valid configuration before attempting to
*           bringup the JESD links.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] mmcmSel selection of the mmcm device to config
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmResetSet( adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_MmcmSel_e    mmcmSel);

/**
* \brief    The QPLL provides the lane rate for the PHY module. It is configured to output the fastest
*           lane rate which allows the channels to use their final dividers to divide that rate down.
*           
*           There are two Qplls in each of the two PHY quads. They are selected based on the desired
*           output frequency. Qpll0 has a wider output range and is the default selected. Qpll1 will
*           only be used when the output rate is lower than Qpll0 can handle.
*           
*           The qPll input parameter can be calculated by calling the utility layer function
*           adi_fpgagen6_QpllSettingsConfigure.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] quadMask bitmask selecting quads to program. bit 0 -> quad 0, etc
* \param[in,out] qPll the adi_fpgagen6_QpllCfg_t used to configure the Qpll
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllCfgSet(   adi_fpgagen6_Device_t*  const device,
                                                            const uint8_t                 quadMask,
                                                            const adi_fpgagen6_QpllCfg_t* const qPll);

/**
* \brief    This API function reads the configuration for the selected Quad in quadSel. Only one
*           of the Qplls in a quad can be powered up at a time so the function reads the power down
*           bit for each qpll. It returns the configuration for the qpll that is powered up.
*           If both qplls are either powered up or powered down it returns a
*           ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM 
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] quadSel numerical value selecting which quad. Valid values 0:ADI_FPGAGEN6_PHY_QUAD_COUNT
* \param[in,out] qPll is a adi_fpgagen6_QpllCfg_t used to configure the Qpll
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllCfgGet(   adi_fpgagen6_Device_t* const device,
                                                            const uint8_t quadSel,
                                                            adi_fpgagen6_QpllCfg_t* const qPll);

/**
* \brief Returns the value of the QPLL lock bit pointed to by the quadMask and bandSel.
*
* \dep_begin
* \dep{device}
* \dep{qPll->band}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] quadSel numerical value selecting which quad. Valid values 0:ADI_FPGAGEN6_PHY_QUAD_COUNT
* \param[in] laneClkSel one of adi_fpgagen6_LaneClkSel_e enum values
* \param[out] lockBit will be set with the lock bit bitfield value, either 0 or 1
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllLockGet(  adi_fpgagen6_Device_t* const    device,
                                                            const uint8_t                   quadSel,
                                                            const adi_fpgagen6_LaneClkSel_e laneClkSel,
                                                            uint8_t* const                  lockBit);

/**
* \brief    Reads the power good bit for each lane. This function is typically used as part of the Qpll
*           bringup and is made available for debug purposes.
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] quadSel numerical value selecting which quad. Valid values 0:ADI_FPGAGEN6_PHY_QUAD_COUNT. Unused parameter.
* \param[out] powerGood status an 8-bit mask of each channels power good bit, bit 7 = channel 7's power good, etc.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LanePowerGoodGet( adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   quadSel,
                                                                uint8_t* const                  powerGood);

#endif /* _ADI_FPGAGEN6_CLK_H_ */
