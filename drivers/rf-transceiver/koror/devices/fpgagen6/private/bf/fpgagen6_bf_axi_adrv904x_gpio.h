/** 
 * \file fpgagen6_bf_axi_adrv904x_gpio.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_ADRV904X_GPIO_H_
#define _FPGAGEN6_BF_AXI_ADRV904X_GPIO_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_gpio_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief Write a 1 to place the ADRV903x (connected to the FMC connector) in reset.  Write a 0 to allow normal operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_Adrv903xReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);


/**
 * \brief Write a 1 to place the ADRV903x (connected to the FMC connector) in reset.  Write a 0 to allow normal operation.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_Adrv903xReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief The number of TDD clock cycles to delay the GPIO signal destined to an external (SMA) pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayExtTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    const uint16_t bfValue);


/**
 * \brief The number of TDD clock cycles to delay the GPIO signal destined to an external (SMA) pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayExtTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                    uint8_t channelId, 
                                                                                    uint16_t* const bfValue);



/**
 * \brief The number of RXDP clock cycles to delay the GPIO signal destined to a Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayRxdpTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint16_t bfValue);


/**
 * \brief The number of RXDP clock cycles to delay the GPIO signal destined to a Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayRxdpTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint16_t* const bfValue);



/**
 * \brief The number of TXDP clock cycles to delay the GPIO signal destined to a Transit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayTxdpTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     const uint16_t bfValue);


/**
 * \brief The number of TXDP clock cycles to delay the GPIO signal destined to a Transit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioDelayTxdpTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                     const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                     uint8_t channelId, 
                                                                                     uint16_t* const bfValue);



/**
 * \brief When set to 1, the SPI2 interface is enabled on GPIO[3:0].
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnSpi2_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief When set to 1, the SPI2 interface is enabled on GPIO[3:0].
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnSpi2_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief Each bit in this bit field enables a GPIO for the use of TDD Enable mapping.  This bit field must correspond to the GPIO Selection bit fields to ensure the proper signal is on the desired GPIO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   const uint32_t bfValue);


/**
 * \brief Each bit in this bit field enables a GPIO for the use of TDD Enable mapping.  This bit field must correspond to the GPIO Selection bit fields to ensure the proper signal is on the desired GPIO.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnTddSelection_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   uint32_t* const bfValue);



/**
 * \brief One-hot enables of the UART interfaces on the GPIO pins
 *        [0]: '1' enables the M4 UART
 *        [1]: '1' enables the A55-0 UART
 *        [2]: '1' enables the A55-1 UART
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnUart_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief One-hot enables of the UART interfaces on the GPIO pins
 *        [0]: '1' enables the M4 UART
 *        [1]: '1' enables the A55-0 UART
 *        [2]: '1' enables the A55-1 UART
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioEnUart_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief The current value on the GPIO inputs.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioInput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                          uint32_t* const bfValue);



/**
 * \brief The value of this field is driven on the corresponding GPIO pin if the pin is enabled as an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioOutput_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           const uint32_t bfValue);


/**
 * \brief The value of this field is driven on the corresponding GPIO pin if the pin is enabled as an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioOutput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           uint32_t* const bfValue);



/**
 * \brief The current value on the GPIO RFFE inputs.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioRffeInput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                              uint16_t* const bfValue);



/**
 * \brief The value of this field is driven on the corresponding GPIO RFFE pin if the pin is enabled as an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioRffeOutput_BfSet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                               const uint16_t bfValue);


/**
 * \brief The value of this field is driven on the corresponding GPIO RFFE pin if the pin is enabled as an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioRffeOutput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                               const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                               uint16_t* const bfValue);



/**
 * \brief Each bit corresponds to an individual GPIO RFFE pin's tristate control.  A 1 sets the tristate high and the pin is an input.  A 0 sets the tristate low and the pin is an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioRffeTristate_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                 const uint16_t bfValue);


/**
 * \brief Each bit corresponds to an individual GPIO RFFE pin's tristate control.  A 1 sets the tristate high and the pin is an input.  A 0 sets the tristate low and the pin is an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioRffeTristate_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                 uint16_t* const bfValue);



/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the external signal pins.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelExtTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  const uint8_t bfValue);


/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the external signal pins.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelExtTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                  const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                  uint8_t channelId, 
                                                                                  uint8_t* const bfValue);



/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the data path tollgate for each Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelRxdpTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);


/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the data path tollgate for each Receive Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelRxdpTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief This bit field identifies the GPIO that should be used for mapping each TDD Enable signal.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelTddEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 const uint8_t bfValue);


/**
 * \brief This bit field identifies the GPIO that should be used for mapping each TDD Enable signal.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelTddEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                 uint8_t channelId, 
                                                                                 uint8_t* const bfValue);



/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the data path tollgate for each Transmit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelTxdpTrigger_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   const uint8_t bfValue);


/**
 * \brief This bitfield identifies the GPIO that will be connected to the GPIO trigger of the data path tollgate for each Transmit Data Path.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioSelTxdpTrigger_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                   uint8_t channelId, 
                                                                                   uint8_t* const bfValue);



/**
 * \brief Each bit corresponds to an individual GPIO pin's tristate control.  A 1 sets the tristate high and the pin is an input.  A 0 sets the tristate low and the pin is an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                             const uint32_t bfValue);


/**
 * \brief Each bit corresponds to an individual GPIO pin's tristate control.  A 1 sets the tristate high and the pin is an input.  A 0 sets the tristate low and the pin is an output.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpioTristate_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                             uint32_t* const bfValue);



/**
 * \brief This bit field shows the current state of GP Interrupts detection.  Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterrupt_BfSet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                            const uint8_t bfValue);


/**
 * \brief This bit field shows the current state of GP Interrupts detection.  Write a 1 to clear.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterrupt_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief This bit field shows the number of accumulated detection events on GP Interrupt.  This bit field is cleared on read.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetCounter_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                      uint8_t channelId, 
                                                                                      uint32_t* const bfValue);



/**
 * \brief Write to a 1 to detect GP Interrupts on an edge.  Write to a 0 to detect GP Interrupts on a level.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetEdgeLevel_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                        const uint8_t bfValue);


/**
 * \brief Write to a 1 to detect GP Interrupts on an edge.  Write to a 0 to detect GP Interrupts on a level.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetEdgeLevel_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                        uint8_t* const bfValue);



/**
 * \brief Write to a 1 to detect GP Interrupts on a rising edge or level high, depending on gp_interrupt_det_edge_level.  Write to a 0 to detect GP Interrupts on a falling edge or level low.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetPolarity_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                       const uint8_t bfValue);


/**
 * \brief Write to a 1 to detect GP Interrupts on a rising edge or level high, depending on gp_interrupt_det_edge_level.  Write to a 0 to detect GP Interrupts on a falling edge or level low.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_GpInterruptDetPolarity_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief General purpose Read/Write register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                        const uint32_t bfValue);


/**
 * \brief General purpose Read/Write register.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                        uint32_t* const bfValue);



/**
 * \brief When the TEST pin is configured as an input, the value of the pin is available here.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_TestInput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief When the TEST pin is configured as an output, the value of this bit field is driven on the pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_TestOutput_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief When the TEST pin is configured as an output, the value of this bit field is driven on the pin.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_TestOutput_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief When written to a 1, the TEST pin is configured as an input to allow it to be driven manually on the eval board.  When written to a 0, the TEST pin is driven by the FPGA.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_TestTristate_BfSet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                             const uint8_t bfValue);


/**
 * \brief When written to a 1, the TEST pin is configured as an input to allow it to be driven manually on the eval board.  When written to a 0, the TEST pin is driven by the FPGA.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_TestTristate_BfGet(adi_fpgagen6_Device_t* const device,
                                                                             const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                             uint8_t* const bfValue);



/**
 * \brief The Minor Version indicates that a change has been made to the design to modify or add a feature.  Minor Version changes will be backward compatible with previous software releases.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiAdrvgen6GpioChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiAdrvgen6Gpio_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiAdrvgen6GpioChanAddr_e baseAddr,
                                                                        uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_ADRV904X_GPIO_H_

