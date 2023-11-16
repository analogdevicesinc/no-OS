/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_axi_ads10v1.h
* \brief Contains top level fpga related function prototypes for
*        adi_fpga_axi_ads10v1.c
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_AXI_ADS10V1_H_
#define _ADI_FPGAGEN6_AXI_ADS10V1_H_

#include "adi_fpgagen6_axi_ads10v1_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Service to Reset FPGAGEN6 Device
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1GlobalReset(adi_fpgagen6_Device_t* const device);

/**
* \brief Service to Get SMA Pin Configuration
*
*           adi_fpgagen6_Ads10v1SmaPinCfg_t will contain the current Selection for each Pin
*
*           Pin Direction is abstracted in adi_fpgagen6_AxiAds10v1SmaSel_e
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[out] smaPinInfo SMA Pin Configuration; 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SmaPinCfgGet( adi_fpgagen6_Device_t* const            device,
                                                            adi_fpgagen6_Ads10v1SmaPinCfg_t* const  smaPinInfo);

/**
* \brief Service to Configure SMA Pins
*
*           The SMA pins can be controlled to be inputs or outputs and the signal type is explained via
*           adi_fpgagen6_AxiAds10v1SmaSel_e. All possible input/output selections are described through 
*           this enum and the Pin Direction is handled automatically by the abstraction.
*
*           adi_fpgagen6_Ads10v1SmaPinCfg_t contains Pin Selections for all SMA Pins
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] smaPinInfo SMA Pin Configuration; 
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SmaPinCfgSet( adi_fpgagen6_Device_t* const                    device,
                                                            const adi_fpgagen6_Ads10v1SmaPinCfg_t* const    smaPinInfo);

/**
* \brief Service to Get LED Configuration
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[out] ledCfg FPGA Board LED Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1LedCfgGet( adi_fpgagen6_Device_t* const        device,
                                                                adi_fpgagen6_Ads10v1LedCfg_t* const ledCfg);


/**
* \brief Service to Configure LED's
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] ledCfg FPGA Board LED Configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Ads10v1LedCfgSet( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_Ads10v1LedCfg_t* const   ledCfg);

#endif  /* _ADI_FPGAGEN6_AXI_ADS10V1_H_ */
