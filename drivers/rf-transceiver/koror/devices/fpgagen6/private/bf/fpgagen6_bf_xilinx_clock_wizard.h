/** 
 * \file fpgagen6_bf_xilinx_clock_wizard.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _FPGAGEN6_BF_XILINX_CLOCK_WIZARD_H_
#define _FPGAGEN6_BF_XILINX_CLOCK_WIZARD_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_xilinx_clock_wizard_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief Fractional part of multiplier value i.e. For 8.125, this value is 125 = 0x7D.
 *        
 *        Note: You need not set any bit for specifying that the multiplier value is fractional. Just mention the fractional value in the register space. 
 *        
 *        The value of CLKFBOUT fractional divide can be from 0 to 875 representing the factional multiplied by 1000.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutFrac_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint16_t bfValue);


/**
 * \brief Fractional part of multiplier value i.e. For 8.125, this value is 125 = 0x7D.
 *        
 *        Note: You need not set any bit for specifying that the multiplier value is fractional. Just mention the fractional value in the register space. 
 *        
 *        The value of CLKFBOUT fractional divide can be from 0 to 875 representing the factional multiplied by 1000.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutFrac_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint16_t* const bfValue);



/**
 * \brief Integer part of multiplier value i.e. For 8.125, this value is 8 = 0x8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutMult_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief Integer part of multiplier value i.e. For 8.125, this value is 8 = 0x8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutMult_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for +/- phase.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutPhase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for +/- phase.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ClkfboutPhase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint32_t* const bfValue);



/**
 * \brief Integer part of clkout0 divide value For example, for 2.250, this value is 2 = 0x2
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Integer part of clkout0 divide value For example, for 2.250, this value is 2 = 0x2
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Fractional part of clkout0 divide value For example, for 2.250, this value is 250 = 0xFA
 *        
 *        Note: You need not set any bit for specifying that the multiplier value is fractional. Just mention the fractional value in the register space.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Frac_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint16_t bfValue);


/**
 * \brief Fractional part of clkout0 divide value For example, for 2.250, this value is 250 = 0xFA
 *        
 *        Note: You need not set any bit for specifying that the multiplier value is fractional. Just mention the fractional value in the register space.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Frac_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint16_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout0Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout1 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout1 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout1Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout2 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout2 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout2Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout3 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout3 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout3Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout4 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout4 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout4Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout5 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout5 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout5Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit clkout6 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Divide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint8_t bfValue);


/**
 * \brief Eight bit clkout6 divide value
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Divide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                uint8_t* const bfValue);



/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Duty_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              const uint32_t bfValue);


/**
 * \brief Duty cycle value = (Duty Cycle in %) * 1000
 *        
 *        For example, for 50% duty cycle, value is 50000 = 0xC350
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Duty_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Phase_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint32_t bfValue);


/**
 * \brief Phase values entered are Signed Number for
 *        +/- phase
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Clkout6Phase_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint32_t* const bfValue);



/**
 * \brief Eight bit divide value applied to all output clocks.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_DivclkDivide_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               const uint8_t bfValue);


/**
 * \brief Eight bit divide value applied to all output clocks.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_DivclkDivide_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                               uint8_t* const bfValue);



/**
 * \brief Bit Number Description
 *        0 User Clock 0 frequency is greater than the specifications
 *        1 User Clock 1 frequency is greater than the specifications
 *        2 User Clock 2 frequency is greater than the specifications
 *        3 User Clock 3 frequency is greater than the specifications
 *        4 User Clock 0 frequency is lesser than the specifications
 *        5 User Clock 1 frequency is lesser than the specifications
 *        6 User Clock 2 frequency is lesser than the specifications
 *        7 User Clock 3 frequency is lesser than the specifications
 *        8 Glitch occurred in the user clock 0
 *        9 Glitch occurred in the user clock 1
 *        10 Glitch occurred in the user clock 2
 *        11 Glitch occurred in the user clock 3
 *        12 Clock stop on User clock 0
 *        13 Clock stop on User clock 1
 *        14 Clock stop on User clock 2
 *        15 Clock stop on User clock 3
 *        16-31 Undefined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_ErrorStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                              uint32_t* const bfValue);



/**
 * \brief Bit Number Description
 *        0 User Clock 0 frequency is greater than the specifications
 *        1 User Clock 1 frequency is greater than the specifications
 *        2 User Clock 2 frequency is greater than the specifications
 *        3 User Clock 3 frequency is greater than the specifications
 *        4 User Clock 0 frequency is lesser than the specifications
 *        5 User Clock 1 frequency is lesser than the specifications
 *        6 User Clock 2 frequency is lesser than the specifications
 *        7 User Clock 3 frequency is lesser than the specifications
 *        8 Glitch occurred in the user clock 0
 *        9 Glitch occurred in the user clock 1
 *        10 Glitch occurred in the user clock 2
 *        11 Glitch occurred in the user clock 3
 *        12 Clock stop on User clock 0
 *        13 Clock stop on User clock 1
 *        14 Clock stop on User clock 2
 *        15 Clock stop on User clock 3
 *        16-31 Undefined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_InterruptEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                  const uint32_t bfValue);


/**
 * \brief Bit Number Description
 *        0 User Clock 0 frequency is greater than the specifications
 *        1 User Clock 1 frequency is greater than the specifications
 *        2 User Clock 2 frequency is greater than the specifications
 *        3 User Clock 3 frequency is greater than the specifications
 *        4 User Clock 0 frequency is lesser than the specifications
 *        5 User Clock 1 frequency is lesser than the specifications
 *        6 User Clock 2 frequency is lesser than the specifications
 *        7 User Clock 3 frequency is lesser than the specifications
 *        8 Glitch occurred in the user clock 0
 *        9 Glitch occurred in the user clock 1
 *        10 Glitch occurred in the user clock 2
 *        11 Glitch occurred in the user clock 3
 *        12 Clock stop on User clock 0
 *        13 Clock stop on User clock 1
 *        14 Clock stop on User clock 2
 *        15 Clock stop on User clock 3
 *        16-31 Undefined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_InterruptEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                  uint32_t* const bfValue);



/**
 * \brief Bit Number Description
 *        0 User Clock 0 frequency is greater than the specifications
 *        1 User Clock 1 frequency is greater than the specifications
 *        2 User Clock 2 frequency is greater than the specifications
 *        3 User Clock 3 frequency is greater than the specifications
 *        4 User Clock 0 frequency is lesser than the specifications
 *        5 User Clock 1 frequency is lesser than the specifications
 *        6 User Clock 2 frequency is lesser than the specifications
 *        7 User Clock 3 frequency is lesser than the specifications
 *        8 Glitch occurred in the user clock 0
 *        9 Glitch occurred in the user clock 1
 *        10 Glitch occurred in the user clock 2
 *        11 Glitch occurred in the user clock 3
 *        12 Clock stop on User clock 0
 *        13 Clock stop on User clock 1
 *        14 Clock stop on User clock 2
 *        15 Clock stop on User clock 3
 *        16-31 Undefined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_InterruptStatus_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                  const uint32_t bfValue);


/**
 * \brief Bit Number Description
 *        0 User Clock 0 frequency is greater than the specifications
 *        1 User Clock 1 frequency is greater than the specifications
 *        2 User Clock 2 frequency is greater than the specifications
 *        3 User Clock 3 frequency is greater than the specifications
 *        4 User Clock 0 frequency is lesser than the specifications
 *        5 User Clock 1 frequency is lesser than the specifications
 *        6 User Clock 2 frequency is lesser than the specifications
 *        7 User Clock 3 frequency is lesser than the specifications
 *        8 Glitch occurred in the user clock 0
 *        9 Glitch occurred in the user clock 1
 *        10 Glitch occurred in the user clock 2
 *        11 Glitch occurred in the user clock 3
 *        12 Clock stop on User clock 0
 *        13 Clock stop on User clock 1
 *        14 Clock stop on User clock 2
 *        15 Clock stop on User clock 3
 *        16-31 Undefined
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_InterruptStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                  uint32_t* const bfValue);



/**
 * \brief Loads Clock Configuration Register values to the internal register used for dynamic reconfiguration and initiates reconfiguration state machine. This bit should be asserted when the required settings are already written into Clock Configuration Registers.
 *        
 *        This bit retains to 0, when the dynamic reconfiguration is done and the clock is locked.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Load_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                       const uint8_t bfValue);


/**
 * \brief Loads Clock Configuration Register values to the internal register used for dynamic reconfiguration and initiates reconfiguration state machine. This bit should be asserted when the required settings are already written into Clock Configuration Registers.
 *        
 *        This bit retains to 0, when the dynamic reconfiguration is done and the clock is locked.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Load_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief When ‘1’ MMCM/PLL is Locked and ready for the reconfiguration. Status of this bit is ‘0’ during the reconfiguration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Locked_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                         uint8_t* const bfValue);



/**
 * \brief When written 0, default configuration done in the Clocking Wizard GUI is loaded for dynamic reconfiguration.
 *        
 *        When written 1, setting provided in the Clock Configuration Registers are used for dynamic reconfiguration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Saddr_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);


/**
 * \brief When written 0, default configuration done in the Clocking Wizard GUI is loaded for dynamic reconfiguration.
 *        
 *        When written 1, setting provided in the Clock Configuration Registers are used for dynamic reconfiguration.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_Saddr_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief A soft reset is initiated when this field is written to 0x0000000A.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxClockWizardChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxClockWizard_SoftwareReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                const fpgagen6_BfXilinxClockWizardChanAddr_e baseAddr,
                                                                                const uint32_t bfValue);
#endif // _FPGAGEN6_BF_XILINX_CLOCK_WIZARD_H_

