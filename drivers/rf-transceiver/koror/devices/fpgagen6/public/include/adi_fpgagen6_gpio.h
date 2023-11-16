/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_gpio.h
* \brief Contains top level fpgagen6 related function prototypes for
*        adi_fpgagen6_gpio.c
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_GPIO_H_
#define _ADI_FPGAGEN6_GPIO_H_

#include "adi_fpgagen6_gpio_types.h"
#include "adi_fpgagen6_tdd_types.h"
#include "adi_fpgagen6_error.h"
#include "adi_fpgagen6_axi_ads10v1.h"

/**
* \brief    This function is used to set the GPIO module into specific modes. Generally, each
*           mode has pre-set GPIOs that are used for its corresponding functionality. This function
*           is needed because these GPIO modes require additional, internal signals to be connected
*           to the GPIO module for valid operation.
*           
*           The current modes supported can be found in the adi_fpgagen6_GpioModes_e enum, however,
*           this function is not used to set TDD GPIO Enable mode.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] gpioMode The user provided variable containing the mode to set.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioModeSet(  adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_GpioModes_e gpioMode);

/**
* \brief    This function returns the current mode for the selected GPIO pin.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] gpioSel Which GPIO the tddModuleSel enable signal will be routed to.
* \param[out] gpioMode The user provided variable into which the mode value will be written.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioModeGet(  adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_GpioPinSel_e gpioSel,
                                                            adi_fpgagen6_GpioModes_e* const gpioMode);

/**
* \brief    This function is used to set the selected GPIO pins as input. Use the pinSelMask
*           as a way to select only those GPIOs that should be set as input.
*
*           For example, pinSelMask = 0x1001 will set GPIO[12] & GPIO[0] as input while leaving
*           all other GPIOs as they were.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] pinSelMask The user provided variable containing the mapping of bits
*                   to be set as input pins.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioInputDirSet(  adi_fpgagen6_Device_t* const    device,
                                                                const uint32_t                  pinSelMask);

/**
* \brief    This function is used to set the selected GPIO pins as output. Use the pinSelMask
*           as a way to select only those GPIOs that should be set as output.
*           
*           For example, pinSelMask = 0x1001 will set GPIO[12] & GPIO[0] as output while leaving
*           all other GPIOs as they were.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] pinSelMask The user provided variable containing the mapping of bits
*                   to be set as output pins.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioOutputDirSet( adi_fpgagen6_Device_t* const device,
                                                                const uint32_t pinSelMask);

/**
* \brief    This function is used to read the current direction set of all the GPIO pins.
*           The function will write the direction encoded into the pinSelMask input variable
*           to each bit corresponding to the appropriate GPIO.
*           
*           For example, pinSelMask = 0x1001 means GPIO[12] & GPIO[0] are set as input pins while
*           the remaining pins are set as output. Bits [31:24] of pinSelMask are unused.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[out] pinSelMask The user provided variable which will contain the direction of each GPIO. 1 = input, 0 = output
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDirectionGet( adi_fpgagen6_Device_t* const device,
                                                                uint32_t* const pinSelMask);

/**
* \brief    This is a wrapper to set the GPIO output register.
* 
*           The GPIO output register holds the output values for each
*           GPIO pin in the corresponding bit location. For example,
*           bit 15 of the output register holds the output value of GPIO
*           pin 15 if pin 15 is set as output.
*           
*           This function does not do any checking on the input/output status
*           of the GPIO pins, it simply writes the entire wrData value directly
*           to the output register.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] wrData The value to write to output register. Valid bits are wrData[23:0].
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioWrite(adi_fpgagen6_Device_t* const device,
                                                        const uint32_t wrData);

/**
* \brief    This is a wrapper to read the GPIO input register.
*
*           The GPIO input register holds the current value seen at each
*           GPIO pin in the corresponding bit location. For example,
*           bit 15 of the input register holds the current value of GPIO
*           pin 15.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[out] rdData Pointer to a uint32_t that will hold the input values. Valid bits are rdData[23:0].
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioRead(adi_fpgagen6_Device_t* const device,
                                                       uint32_t* const rdData);

/**
* \brief    This function is used in conjunction with the TDD State Machine GPIO modules. If the
*           TDD GPIO modules are enabled their enable signals will not be present on the physical
*           GPIO pins unless this function is called to assign a particular GPIO pin to output
*           the signal on.
*           
*           Any of the TDD Enable modules can be output on any of the available GPIO pins, but each
*           TDD Enable module must be configured separately.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] tddGpioEnSel The index of the GPIO Enable module to output
* \param[in] gpioSel Which GPIO the tddGpioEnSel enable signal will be routed to.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxSet(adi_fpgagen6_Device_t* const device,
                                                            const uint32_t tddGpioEnSel,
                                                            const adi_fpgagen6_GpioPinSel_e gpioSel);

/**
* \brief    This function returns the assigned GPIO for the selected TDD Enable module. An invalid
*           value means no GPIO has been assigned correctly.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] tddGpioEnSel The index of the GPIO Enable module to output
* \param[out] gpioSel A pointer to adi_fpgagen6_GpioPinSel_e that will hold the GPIO for the selected TDD module.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxGet(adi_fpgagen6_Device_t* const device,
                                                            const uint32_t tddGpioEnSel,
                                                            adi_fpgagen6_GpioPinSel_e* const gpioSel);

/**
* \brief    This function clears all current TDD Enable module settings and GPIO assignments.
*           Any GPIOs that were assigned to TDD Enable modules will be defaulted back to inputs.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioTddMuxClear(adi_fpgagen6_Device_t* const device);

/**
* \brief    This function configures the GP Interrupt detection options in the ADRV904X FPGA.
* 
* GP Interrupt configuration options include interrupt detection types (level vs edge) and 
* detection polarities (low vs high), per GP Interrupt pin. Each pin can be configured uniquely.
* 
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] config GP Interrupt detection configuration for the FPGA
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCfgSet(  adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_GpIntCfg_t* const    config);

/**
* \brief    This function reads back the configuration of the GP Interrupt detection options in the ADRV904X FPGA. 
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[out] config GP Interrupt detection configuration for the FPGA
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCfgGet(  adi_fpgagen6_Device_t* const        device,
                                                            adi_fpgagen6_GpIntCfg_t* const      config);

/**
* \brief    This function reads back the current status bits of the GP Interrupt pins from the ADRV904X FPGA.
* 
* There is a unqiue status readback field per GP Interrupt pin. If a pin status is 0, then no detection events 
* have been detected on the associated pin since last cleared. If pin status is 1, then at least 1 event has
* been detected on the pin since last cleared.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[out] statusRead GP Interrupt status readback from the FPGA
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntStatusGet(   adi_fpgagen6_Device_t* const        device,
                                                                adi_fpgagen6_GpIntStatus_t* const   statusRead);

/**
* \brief    This function allows clearing of the GP Interrupt pins from the ADRV904X FPGA.
*           It can be used to clear either one of the GP Interrupt status bits, or both simultaneously.
*           
* To clear a status readback for a given GP Interrupt pin, set the clear field for the pin to 1.
* To leave a status readback unchanged, set the clear field to 0.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] statusClear GP Interrupt clear bits. Set bit to 1 to clear a given gp interrupt status bit. Set bit to 0 to leave it unchanged.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntStatusClear( adi_fpgagen6_Device_t* const                device,
                                                                const adi_fpgagen6_GpIntStatus_t* const     statusClear);

/**
* \brief    This function reads back a selected GP Interrupt counter from the ADRV904X FPGA.
*           Note that reading back a counter will automatically reset that counter to 0.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] gpIntPinSelect Select GP Interrupt counter to retrieve. Only valid pin selections are 0 and 1.
* \param[out] counter Number of GP Interrupt detection events seen for the selected pin since last read.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpIntCounterGet(  adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   gpIntPinSelect,
                                                                uint32_t* const                 counter);
           
/**
* \brief    This function sets GPIO Select external Trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] smaPin GPIO SMA pin selection in adi_fpgagen6_GpioSmaPin_e.
* \param[in] gpioPin GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                   const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                   const adi_fpgagen6_GpioPinSel_e  gpioPin);

/**
* \brief    This function gets GPIO Select external Trigger.
*
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in]  smaPin GPIO SMA pin selection in adi_fpgagen6_GpioSmaPin_e.
* \param[out] gpioPin Pointer to hold GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                   const adi_fpgagen6_GpioSmaPin_e    smaPin,
                                                                   adi_fpgagen6_GpioPinSel_e * const  gpioPin);

/**
* \brief    This function clear GPIO Select external Trigger SMA Pins.
* This will clear any SMA Pins that selected in the smaPinMask that are being used by External Trigger.
* It will also relieve and GPIO that associated with these SMA Pins.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] smaPinMask GPIO SMA pin mask of selections in adi_fpgagen6_GpioSmaPin_e.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelExtTriggerClear(adi_fpgagen6_Device_t* const   device,
                                                                     const uint8_t                  smaPinMask);

/**
* \brief    This function sets GPIO Select RXDP Trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] rxDataPath  Rx datapath defined in adi_fpgagen6_RxDatapath_e.
* \param[in] gpioPin GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelRxdpTriggerSet(adi_fpgagen6_Device_t* const       device,
                                                                    const adi_fpgagen6_RxDatapath_e    rxDataPath,
                                                                    const adi_fpgagen6_GpioPinSel_e    gpioPin);

/**
* \brief    This function gets GPIO Select RXDP Trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] rxDataPath  Rx datapath defined in adi_fpgagen6_RxDatapath_e.
* \param[out] gpioPin Pointer to hold GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelRxdpTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                    const adi_fpgagen6_RxDatapath_e    rxDataPath,
                                                                    adi_fpgagen6_GpioPinSel_e * const  gpioPin);

/**
* \brief    This function sets GPIO Select TXDP Trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] txDataPath  Tx datapath defined in adi_fpgagen6_TxDatapath_e.
* \param[in] gpioPin GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelTxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                    const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                    const adi_fpgagen6_GpioPinSel_e  gpioPin);

/**
* \brief    This function gets GPIO Select TXDP Trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] txDataPath  Tx datapath defined in adi_fpgagen6_TxDatapath_e.
* \param[out] gpioPin Pointer to hold GPIO Pin selection in adi_fpgagen6_GpioPinSel_e
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioSelTxdpTriggerGet(adi_fpgagen6_Device_t* const       device,
                                                                    const adi_fpgagen6_TxDatapath_e    txDataPath,
                                                                    adi_fpgagen6_GpioPinSel_e * const  gpioPin);

/**
* \brief    This function sets GPIO delay external trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] smaPin  GPIO SMA pin selection in adi_fpgagen6_GpioSmaPin_e.
* \param[in] delayExtTrigger clock cycles for delay external trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayExtTriggerSet(adi_fpgagen6_Device_t* const   device,
                                                                     const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                     const uint16_t   delayExtTrigger);

/**
* \brief    This function gets GPIO delay external trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in]  smaPin  GPIO SMA pin selection in adi_fpgagen6_GpioSmaPin_e.
* \param[out] delayExtTrigger pointer to hold delayExtTrigger clock cycles for delay external trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayExtTriggerGet(adi_fpgagen6_Device_t* const   device,
                                                                     const adi_fpgagen6_GpioSmaPin_e  smaPin,
                                                                     uint16_t * const  delayExtTrigger);

/**
* \brief    This function sets GPIO delay RXDP trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] rxDataPath  Rx datapath defined in adi_fpgagen6_RxDatapath_e.
* \param[in] delayRxdpTrigger clock cycles for delay rxdp trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayRxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_RxDatapath_e  rxDataPath,
                                                                      const uint16_t                   delayRxdpTrigger);

/**
* \brief    This function gets GPIO delay external trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in]  rxDataPath  Rx datapath defined in adi_fpgagen6_RxDatapath_e.
* \param[out] delayRxdpTrigger pointer to hold clock cycles for delay rxdp trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayRxdpTriggerGet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_RxDatapath_e  rxDataPath,
                                                                      uint16_t * const                 delayRxdpTrigger);

/**
* \brief    This function sets GPIO delay TXDP trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in] txDataPath  Tx datapath defined in adi_fpgagen6_TxDatapath_e.
* \param[in] delayTxdpTrigger clock cycles for delay txdp trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayTxdpTriggerSet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                      const uint16_t                   delayTxdpTrigger);

/**
* \brief    This function gets GPIO delay TXDP trigger.
*           
* \param[in,out] device Context variable - Pointer to the FPGA device data structure.
* \param[in]  txDataPath  Tx datapath defined in adi_fpgagen6_TxDatapath_e.
* \param[out] delayTxdpTrigger pointer to hold clock cycles for delay txdp trigger.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_GpioDelayTxdpTriggerGet(adi_fpgagen6_Device_t* const     device,
                                                                      const adi_fpgagen6_TxDatapath_e  txDataPath,
                                                                      uint16_t * const                 delayTxdpTrigger);

#endif /* _ADI_FPGAGEN6_GPIO_H_ */
