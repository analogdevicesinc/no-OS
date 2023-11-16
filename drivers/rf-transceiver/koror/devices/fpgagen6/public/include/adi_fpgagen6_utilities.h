/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
 
/**
* \file adi_fpgagen6_utilities.h
* \brief Contains top level fpgagen6 related function prototypes for
*        adi_fpgagen6_utilities.c
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_UTILITIES_H_
#define _ADI_FPGAGEN6_UTILITIES_H_

#include "adi_fpgagen6_utilities_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Utility API function used to calculate the required MMCM settings for the
*        desired link-layer lane rate. The inputs for calculating the settings are
*        the refClk_kHz input parameter, which is the refclk input for the FPGA, and
*        the output rates assigned in mmcmConfig. These should map to the appropriate
*        links and their respective lane rates.
*        
*        The mapping is as follows:
*        
*           mmcmConfig->outputClkFreq_kHz index  |  Link
*           -------------------------------------|-------------------
*                        0                       |        Deframer 0
*                        1                       |        Deframer 1
*                        2                       |        Deframer 2
*                        3                       |        Framer 0
*                        4                       |        Framer 1
*                        5                       |        TDD State Machine
*
* \dep_begin
* \dep{device}
* \dep{mmcmConfig->outputClkFreq_kHz}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] refClk_kHz The input clock rate in kHz
* \param[in,out] mmcmConfig Used as input & output. Set the desired channel frequencies before calling.
*                The remainder of the structure is used to store the configuration values.
*
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmSettingsConfigure(adi_fpgagen6_Device_t* const device,
                                                                    const uint32_t          refClk_kHz,
                                                                    adi_fpgagen6_MmcmCfg_t* const mmcmConfig);

/**
* \brief    Utility API function used to calculate the required QPLL settings for the desired PHY channel
*           data rate. The QPLL will provide the fastest PHY data rate clock which allows for the channel
*           dividers to divide down the QPLL clock as needed.
*
* \dep_begin
* \dep{device}
* \dep{divisors}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] refClk_kHz The input clock rate in kHz
* \param[in] fastestLaneRate_kHz The fastest lane rate in kHz
* \param[in,out] qplls A pointer to a QPLL configuration structure
* 
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllSettingsConfigure(adi_fpgagen6_Device_t* const device,
                                                                    const uint32_t          refClk_kHz,
                                                                    const uint32_t          fastestLaneRate_kHz,
                                                                    adi_fpgagen6_QpllCfg_t* const qplls);


/**
* \brief    Using the QPLL VCO frequency this function will calculate the necessary channel divider based on the
*           available values.
*           
*           Use adi_fpgagen6_QpllSettingsConfigure to calculate the VCO frequency.
*
* \dep_begin
* \dep{device}
* \dep{divisors}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] qpllVcoFreq_kHz The qpll Vco rate
* \param[in] desiredLaneRate_kHz The desired lane rate
* \param[out] dVal The divider value necessary to achieve the desiredLaneRate_Khz
* 
* \retval adi_common_ErrAction_e - ADI_COMMON_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_fpgagen6_QpllDivisorCalculate(adi_fpgagen6_Device_t* const    device,
                                                                            const uint32_t                  qpllVcoFreq_kHz,
                                                                            const uint32_t                  desiredLaneRate_kHz,
                                                                            uint8_t* const                  dVal);

/**
* \brief    Service to translate a lane XBar from one side of the JESD PHY link to the opposite side.
*           This function will check for valid lane assignements in the inputXBar before setting the
*           corresponding index in the outputXBar.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] inputXBar      Lane XBar to be translated
* \param[out] outputXBar    Will hold the converted values corresponding to the opposite end of the JESD link
* \param[in] laneCount      Number of lanes, ie count of each XBar
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarConfigure(adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   inputXBar[],
                                                                uint8_t                         outputXBar[],
                                                                const uint32_t                  laneCount);
/**
* \brief Service to Generate Lane Reset Mask
*
*           Function will parse Init Data Structure to Configure Reset Mask based on Lanes in Use
*           for Deframer & LaneXBar Values for the Framers.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device         Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] init               Initialization Data Structure
* \param[in,out] laneResetMaskInfo Mask Configurations for Resetting Lanes based on Init Data
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetConfigure(   adi_fpgagen6_Device_t* const            device,
                                                                        const adi_fpgagen6_Init_t* const        init,
                                                                        adi_fpgagen6_LaneXBarResetMask_t* const laneResetMaskInfo);

/**
* \brief Service to Create a Deframer Configuration Structure using Deframer Link Data
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] deframerlink   Deframer Link Config
* \param[in,out] deframerCfg   Deframer Configuration Data
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerConfigure(adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_DeframerDataLinkCfg_t* const deframerlink,
                                                                adi_fpgagen6_DeframerCfg_t* const               deframerCfg);

/**
* \brief Service to Create a Framer Configuration Structure using Framer Link Data
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] framerlink framer Link Config
* \param[in,out] framerCfg Framer Configuration Data
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerConfigure(  adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_FramerDataLinkCfg_t* const   framerlink,
                                                                adi_fpgagen6_FramerCfg_t* const                 framerCfg);

/**
* \brief Service Configure a SyncXBar Mode
*
*       Depending on the Sync Mode Selected, a SyncXbar Configuration will be provided
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] syncMode SyncBar Mode
* \param[in,out] syncXBarCfg SyncBar Configuration Used based on Mode Selection
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarConfigure(adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_SyncModeSel_e    syncMode,
                                                                adi_fpgagen6_SyncXBar_t* const      syncXBarCfg);

/**
* \brief    Service to populate an FPGA init structure.
*
*           The init structure values are calculated with a combination of JESD formulas and hard-coded default values.
*           The input to the function is the fpgaCfg, which has the JESD information for each link in the FPGA.
*           In a typical usecase these values would come from the TRX device init settings.
*           After this function has been called the init structure is ready to write out to the appropriate
*           FPGA modules.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] fpgaCfg Input settings to the function
* \param[in,out] fpgaInit A pointer to an init structure that will hold valid init settings after function
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_InitConfig(  adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FpgaCfg_t* const     fpgaCfg,
                                                            adi_fpgagen6_Init_t* const              fpgaInit);


/**
* \brief    This API function will take a populated FPGA Init structure and configure the entire FPGA.
*           By using the Trx device information an FPGA Init structure can be populated using the
*           adi_fpgagen6_InitConfig API function.
*           
*           Summary of steps in InitSet:
*               * using the clock settings from initSettings it will verify that all PLLs and MMCMs
*                 are configured and locked
*               * it will write the SERDES configuration and verify that the selected lanes complete their
*                 reset sequence
*               * it will write the Deframer/Framer configurations and verify that the modules complete their
*                 reset sequence
*               * it will configure the LaneXBar reset bits to ensure any unused lanes are held in reset for
*                 each link
*               * it will configure the Datachain (Transport & DMAs) with their initial conditions
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -  Pointer to the FPGA9025 device data structure.
* \param[in,out] fpgaInit initialisation settings for fpga.
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InitSet(  adi_fpgagen6_Device_t* const device,
                                                        const adi_fpgagen6_Init_t* const  fpgaInit);

#endif  /* _ADI_FPGAGEN6_UTILITIES_H_ */