/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_core.h
* \brief Contains top level FPGAGEN6 related function prototypes for
*        adi_fpgagen6_core.c
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_CORE_H_
#define _ADI_FPGAGEN6_CORE_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Prepares the FPGAGEN6 device for use.
*
* This function must be called on the device before any other function is called. The
* function must not be called more than once on the same device.
*
* Uses adi_common_hal_HwOpen internally. See that function for important relevant information.
*
* This API shall call the ADI HAL function adi_hal_HwOpen for
* FPGAGEN6 Hardware initialization.  This HAL function initializes all the external
* hardware blocks required in the operation of the FPGAGEN6 device.
* This API will also set the HAL timeout limit for the HAL driver as per API
* requirements.
*
* \pre device->common.devHalInfo must be initialized with user values before this function
* is called.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
* \param[in] hwOpenCfg Pointer to a structure holding FPGA Device setup information
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwOpen(adi_fpgagen6_Device_t* const device, adi_fpgagen6_HwOpenCfg_t* const hwOpenCfg);

/**
* \brief Performs a hardware shutdown for FPGAGEN6 Device.
*
* This API shall call the ADI HAL function adi_hal_HwClose for
* FPGAGEN6 Hardware shutdown.  This HAL function shuts down all the external
* hardware blocks required in the operation of the FPGAGEN6 device.
*
* Uses adi_common_hal_HwClose internally. See that function for important relevant information.
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwClose(adi_fpgagen6_Device_t* const device);

/**
 * \brief Acquires the device so that it can only be used only in the context of the calling
 * thread until such time as adi_fpgagen6_Unlock is called.
 *
 * Generally there is no need to explicitly lock or unlock a device before or after calling any
 * API function with that device (each API function locks the device internally itself before
 * carrying out it's actions and unlocks the device before returning). Any exceptions to this rule
 * will be clearly documented.
 *
 * Uses adi_common_hal_Lock internally. See that function for important relevant information.
 *
 * \param[in,out] device Context variable -Pointer to FPGAGEN6 device data structure containing settings
 *
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
 **/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Lock(adi_fpgagen6_Device_t* const device);

/**
 * \brief Releases a device previously acquired by the calling thread.
 *
 * Only the thread that acquired the device can release the device. Attempting to release a device
 * that has not been acquired by the calling thread results in undefined behaviour.
 *
 * Uses adi_common_hal_Unlock internally. See that function for important relevant information.
 *
 * \param[in,out] device Context variable -Pointer to FPGAGEN6 device data structure containing settings
 *
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
 **/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Unlock(adi_fpgagen6_Device_t* const device);

/**
* \brief Performs an FPGA reset by writing to the global reset bit and setting the DRP registers
*        back to their reset state, i.e. a 204C profile with links at 32.44032 Gpbs and a 491.52MHz
*        reference clock.
*
* \pre This function may be called any time after device->common.devHalInfo has been
* initialized with user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to FPGAGEN6 device data structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwReset(adi_fpgagen6_Device_t* const device);

/**
* \brief    The Dynamic Reconfiguration Port (DRP) allows the dynamic change of parameters of the
*           GTY primitives. The DRP interface is used primarily to configure PHY modules, including
*           QPLL and channel configuration. Please refer to Xilinx User Guide 578 for a complete
*           register map.
* 
*           To read a particular register through the DRP interface you must supply three items:
*               
*               1.  The module of interest, provided through the drp parameter. Examples include
*                   the PHY channels and the QPLLs. Please refer to adi_fpgagen6_DrpInterface_e
*                   for valid interfaces; all others will return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM
*               2.  The address, provided through the addr parameter. 
*               3.  The bits of interest, provided through the rdMask parameter. 
*               
*                   Note: As a convenience, the API will shift the register value readback so 
*                   that the bits of interest are in LSB position.
*                   Example, if rdMask = 0x70, the API will shift the register value >> 4 and
*                   return the shifted value in rdMask.
*                   This is different than adi_fpgagen6_DrpWrite in that the provided wrWord must
*                   already be shifted.
*                   
*           The adi_fpgagen6_DrpAddress_e and adi_fpgagen6_DrpMask_e can be used together, for
*           the addr and wrMask parameters respectively, to access configuration parameters that 
*           the API programs during initialization, however the API will not perform error checking 
*           on these two parameter so any combination can be provided.
*           
*           The addr and wrMask parameters are defined in the Xilix User Guide 578 Appendix C as
*           DRP Address and DRP Bits, respectively.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to FPGAGEN6 device data structure containing settings
* \param[in] addr is a uint16_t DRP address word
* \param[in] drp is a adi_fpga_DrpInterface_e enum type for DRP selection
* \param[out] rdWord is uint16_t pointer to place the readback and shifted bitfield value
* \param[in] rdMask is applied to the word read from the DRP register
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_DrpFieldRead(adi_fpgagen6_Device_t* const device,
                                                            const uint16_t addr,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint16_t* const rdWord,
                                                            const uint16_t rdMask);

/**
* \brief    The Dynamic Reconfiguration Port (DRP) allows the dynamic change of parameters of the
*           GTY primitives. The DRP interface is used primarily to configure PHY modules, including
*           QPLL and channel configuration. Please refer to Xilinx User Guide 578 for a complete
*           register map.
* 
*           To write a particular register through the DRP interface you must supply four items:
*               
*               1.  The module of interest, provided through the drp parameter. Examples include
*                   the PHY channels and the QPLLs. Please refer to adi_fpgagen6_DrpInterface_e
*                   for valid interfaces; all others will return ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM
*               2.  The address, provided through the addr parameter. 
*               3.  The bits of interest, provided through the wrMask parameter.
*               4.  The register value to write. The API will shift this value to the correct location
*                   based on the wrMask value.
*                   
*           The adi_fpgagen6_DrpAddress_e and adi_fpgagen6_DrpMask_e can be used together, for
*           the addr and wrMask parameters respectively, to access configuration parameters that 
*           the API programs during initialization, however the API will not perform error checking 
*           on these two parameter so any combination can be provided.
*           
*           The addr and wrMask parameters are defined in the Xilix User Guide 578 Appendix C as
*           DRP Address and DRP Bits, respectively. The allowable values for wrWord are also
*           provided in the DRP Encoding column.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to FPGAGEN6 device data structure containing settings
* \param[in] addr is a uint16_t DRP address word
* \param[in] drp is a adi_fpga_DrpInterface_e enum type for DRP selection
* \param[in] wrWord the value to write to DRP register
* \param[in] wrMask is applied to the word read from the DRP register
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DrpFieldWrite(adi_fpgagen6_Device_t* const device,
                                                            const uint16_t addr,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint16_t wrWord,
                                                            const uint16_t wrMask);

#endif
