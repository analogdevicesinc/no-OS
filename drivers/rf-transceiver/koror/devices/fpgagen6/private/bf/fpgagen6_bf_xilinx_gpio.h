/** 
 * \file fpgagen6_bf_xilinx_gpio.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_XILINX_GPIO_H_
#define _FPGAGEN6_BF_XILINX_GPIO_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_xilinx_gpio_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief Enable Channel 1 Interrupt.
 *        0 = Disabled (masked)
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel1InterruptEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);


/**
 * \brief Enable Channel 1 Interrupt.
 *        0 = Disabled (masked)
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel1InterruptEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief Channel 1 Interrupt Status
 *        0 = No Channel 1 input interrupt
 *        1 = Channel 1 input interrupt
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel1InterruptStatus_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);


/**
 * \brief Channel 1 Interrupt Status
 *        0 = No Channel 1 input interrupt
 *        1 = Channel 1 input interrupt
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel1InterruptStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief Enable Channel 2 Interrupt.
 *        0 = Disabled (masked)
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel2InterruptEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);


/**
 * \brief Enable Channel 2 Interrupt.
 *        0 = Disabled (masked)
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel2InterruptEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief Channel 2 Interrupt Status
 *        0 = No Channel 2 input interrupt
 *        1 = Channel 2 input interrupt
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel2InterruptStatus_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   const uint8_t bfValue);


/**
 * \brief Channel 2 Interrupt Status
 *        0 = No Channel 2 input interrupt
 *        1 = Channel 2 input interrupt
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Channel2InterruptStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                   uint8_t* const bfValue);



/**
 * \brief Master enable for the device interrupt output to
 *        the system interrupt controller:
 *        0 = Disabled
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GlobalInterruptEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                 const uint8_t bfValue);


/**
 * \brief Master enable for the device interrupt output to
 *        the system interrupt controller:
 *        0 = Disabled
 *        1 = Enabled
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GlobalInterruptEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                                 uint8_t* const bfValue);



/**
 * \brief AXI GPIO Data Register.
 *        For each I/O bit programmed
 *        as input:
 *        • R: Reads value on the input
 *        pin.
 *        • W: No effect.
 *        For each I/O bit programmed
 *        as output:
 *        • R: Reads to these bits
 *        always return zeros
 *        • W: Writes value to the
 *        corresponding AXI GPIO
 *        data register bit and output
 *        pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Gpio2xData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                      const uint32_t bfValue);


/**
 * \brief AXI GPIO Data Register.
 *        For each I/O bit programmed
 *        as input:
 *        • R: Reads value on the input
 *        pin.
 *        • W: No effect.
 *        For each I/O bit programmed
 *        as output:
 *        • R: Reads to these bits
 *        always return zeros
 *        • W: Writes value to the
 *        corresponding AXI GPIO
 *        data register bit and output
 *        pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Gpio2xData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



/**
 * \brief AXI GPIO 3-State Control Register.
 *        Each I/O pin of the AXI GPIO is individually
 *        programmable as an input or output.
 *        For each of the bits:
 *        0 = I/O pin configured as output.
 *        1 = I/O pin configured as input.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Gpio2xTri_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                     const uint32_t bfValue);


/**
 * \brief AXI GPIO 3-State Control Register.
 *        Each I/O pin of the AXI GPIO is individually
 *        programmable as an input or output.
 *        For each of the bits:
 *        0 = I/O pin configured as output.
 *        1 = I/O pin configured as input.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_Gpio2xTri_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                     uint32_t* const bfValue);



/**
 * \brief AXI GPIO Data Register.
 *        For each I/O bit programmed
 *        as input:
 *        • R: Reads value on the input
 *        pin.
 *        • W: No effect.
 *        For each I/O bit programmed
 *        as output:
 *        • R: Reads to these bits
 *        always return zeros
 *        • W: Writes value to the
 *        corresponding AXI GPIO
 *        data register bit and output
 *        pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GpioxData_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                     const uint32_t bfValue);


/**
 * \brief AXI GPIO Data Register.
 *        For each I/O bit programmed
 *        as input:
 *        • R: Reads value on the input
 *        pin.
 *        • W: No effect.
 *        For each I/O bit programmed
 *        as output:
 *        • R: Reads to these bits
 *        always return zeros
 *        • W: Writes value to the
 *        corresponding AXI GPIO
 *        data register bit and output
 *        pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GpioxData_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                     uint32_t* const bfValue);



/**
 * \brief AXI GPIO 3-State Control Register.
 *        Each I/O pin of the AXI GPIO is individually
 *        programmable as an input or output.
 *        For each of the bits:
 *        0 = I/O pin configured as output.
 *        1 = I/O pin configured as input.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GpioxTri_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                    const uint32_t bfValue);


/**
 * \brief AXI GPIO 3-State Control Register.
 *        Each I/O pin of the AXI GPIO is individually
 *        programmable as an input or output.
 *        For each of the bits:
 *        0 = I/O pin configured as output.
 *        1 = I/O pin configured as input.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfXilinxGpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_XilinxGpio_GpioxTri_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfXilinxGpioChanAddr_e baseAddr,
                                                                    uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_XILINX_GPIO_H_

