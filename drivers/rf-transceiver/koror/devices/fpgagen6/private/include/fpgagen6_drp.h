/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file fpgagen6_drp.h
*
* \brief
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _FPGAGEN6_DRP_H_
#define _FPGAGEN6_DRP_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Configures the Common Qpll DRP registers based on the clock configuration.
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] refClk The input clock rate in kHz
* \param[in] laneRateThe Lane Rate in Gbps
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DrpQpllCommonCfgSet(  adi_fpgagen6_Device_t* const    device,
                                                                    const uint32_t                  refClk_kHz,
                                                                    const uint32_t                  laneRate_kHz);

/**
* \brief Configures, per Channel, Qpll DRP registers based on the clock configuration.
*
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] refClk The input clock rate in kHz
* \param[in] laneRateThe Lane Rate in Gbps
* \param[in] laneRateThe Lane Rate in Gbps
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DrpQpllChannelCfgSet( adi_fpgagen6_Device_t* const        device,
                                                                    const uint32_t                      refClk_kHz,
                                                                    const uint32_t                      txLaneRate_kHz,
                                                                    const uint32_t                      rxLaneRate_kHz,
                                                                    const adi_fpgagen6_DrpInterface_e   drpInterface);

#endif  /* _FPGAGEN6_TEST_H_ */
