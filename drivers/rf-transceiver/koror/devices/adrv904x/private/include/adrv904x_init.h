/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_init.h
 * \brief Contains ADRV904X init related private function prototypes for
 *        adrv904x_init.c that helps adi_adrv904x_init.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADRV904X_INIT_H_
#define _ADRV904X_INIT_H_

#include "adi_adrv904x_error.h"
#include "adi_library.h"

//#define ADRV904X_INIT_DEBUG 1
#ifdef ADRV904X_INIT_DEBUG
#define ADRV904X_BUGINFO(x) ADI_LIBRARY_PRINTF("MESSAGE: %s ******************************* \n", (x));
#define ADRV904X_BUGINFO_NUM(x,n) ADI_LIBRARY_PRINTF("MESSAGE: %s: %d 0x%08x \n", (x),(n),(n));
#else
#define ADRV904X_BUGINFO(x) 
#define ADRV904X_BUGINFO_NUM(x,n) 
#endif

//#define ADRV904X_INIT_DMAINFO_DEBUG 1
#ifdef ADRV904X_INIT_DMAINFO_DEBUG
#define ADRV904X_DMAINFO(text, addr, count) ADI_LIBRARY_PRINTF("MESSAGE: DMA: %30s: addr=0x%08x, count=%d \n", (text), (addr), (count));
#else
#define ADRV904X_DMAINFO(text, addr, count)
#endif

//#define ADRV904X_INIT_DMA_DEBUG 1
#ifdef ADRV904X_INIT_DMA_DEBUG
#define ADRV904X_SPIDMAINFO(s,a,b,c) ADI_LIBRARY_PRINTF((s),(a),(b),(c));
#else
#define ADRV904X_SPIDMAINFO(s,a,b,c)
#endif

//#define ADRV904X_INIT_SPI_DEBUG 1
#ifdef ADRV904X_INIT_SPI_DEBUG
#define ADRV904X_SPIINFO(s,a,b,c) ADI_LIBRARY_PRINTF((s),(a),(b),(c));
#define ADRV904X_SPI_FIELD_INFO(s,a,b,c,d) ADI_LIBRARY_PRINTF((s),(a),(b),(c), (d));
#else
#define ADRV904X_SPIINFO(s,a,b,c)
#define ADRV904X_SPI_FIELD_INFO(s,a,b,c,d)
#endif

#define ADRV904X_SPIWRITEBYTE_RETURN(text, addr, data, recoveryAction)                              \
{                                                                                                   \
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, (addr), (data), 0xFFU);             \
    if(ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)                                                 \
    {                                                                                               \
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Byte Write Issue");              \
        return recoveryAction;                                                                      \
    }                                                                                               \
    ADRV904X_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data));  \
}

#define ADRV904X_SPIWRITEBYTE_GOTO(text, addr, data, recoveryAction, label)                         \
{                                                                                                   \
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, (addr), (data), 0xFFU);             \
    if(ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)                                                 \
    {                                                                                               \
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Byte Write Issue");              \
        goto cleanup;                                                                               \
    }                                                                                               \
    ADRV904X_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data));  \
}

/**
* \brief Sets up the thresholds on Rx channel which triggers overload bits
*        if the input level exceeds threshold.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] rxChannel Channel for which overload protection config is desired
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxOverloadProtectionSet(adi_adrv904x_Device_t* const    device,
                                                                  const adi_adrv904x_RxChannels_e rxChannel);

/**
* \brief Private function used by the APIs that can benefit from SPI streaming. This function should be called
*        at the beginning of the function. If device->devStatInfo->spiOptions.allowSpiStreaming is set the part
*        is placed into streaming mode. Otherwise, this function does nothing.
*
* This function to range check the version number of the API
*
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SpiStreamingEntry(adi_adrv904x_Device_t* const device);
                                                                                                               
/**
* \brief Private function used by the APIs that call adrv904x_SpiStreamingEntry. It is the responsibility of each
*        API function that calls adrv904x_SpiStreamingEntry to call this function also to disable streaming if
*        necessary.
*
* This function to range check the version number of the API
*
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_SpiStreamingExit(adi_adrv904x_Device_t* const device);

/**
* \brief Configures and allocates selected GPIO pins that will trigger a Stream that handle 
*           Tx to Orx Mapping changes at runtime.
* 
* This function will handle GPIO setup (if appropriate) and store the selected feature mode 
* in the device data structure for use at runtime.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] mappingConfig Pointer to the mapping config stored in the init structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxToOrxMappingInit(adi_adrv904x_Device_t* const                       device,
                                                             const adi_adrv904x_TxToOrxMappingConfig_t* const   mappingConfig);
#ifndef CLIENT_IGNORE

/**
* \brief Configures the part to use DEVCLK as HSDIG to allow FW to boot and syncs slice clocks.
*           * Program FW divide registers using configurator settings
*           * Configure HSDIG as DEVCLK
*           * Calculate and program DEVCLK divider values
*           * Clear master bias igen powerdown
*           * Enable DEVCLK & digital clock
*           * Sync slice clocks
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in] device A pointer to the device settings structure
* \param[in] init A pointer to the init data structure for clock settings
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_ClocksSync(adi_adrv904x_Device_t* const        device,
                                                     const adi_adrv904x_Init_t* const    init);



/**
* \brief Initializes power monitoring related parameters during device bootup.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txChannelMask Tx channel/s to initialize power monitor configuration for
* \param[in] totalInputInterpoloationRate Total interpolation rate for input signal.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxPowerMonitorInitialize(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t               txChannelMask,
                                                                   const uint8_t                totalInputInterpoloationRate);

/**
* \brief Helper function to set the specified Uart Signal to Gpio Output Pin.
*
* \pre This function can be called before the CPU start
*
*        GPIO Pins            : UART Signals
*        ==============================================================
*        ADI_ADRV904X_GPIO_09 : ADI_ADRV904X_GPIO_SIGNAL_UART_PADRXSIN
*        ADI_ADRV904X_GPIO_10 : ADI_ADRV904X_GPIO_SIGNAL_UART_PADCTS
*        ADI_ADRV904X_GPIO_11 : ADI_ADRV904X_GPIO_SIGNAL_UART_PADRTSOUT
*        ADI_ADRV904X_GPIO_12 : ADI_ADRV904X_GPIO_SIGNAL_UART_PADTXSOUT
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure containing settings
* \param[in] pinSelect The GPIO Pin Select for UART signal
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_UartCfg(adi_adrv904x_Device_t* const       device,
                                                  const adi_adrv904x_GpioPinSel_e    pinSelect);

/**
* \brief Helper functon to set the specified Uart Signal to Gpio Output Pin.
*
* \pre This function can be called before the CPU start
* 
* DFE UART0 GPIO pin/signal usage for is as follows:
*
*        UART : GPIO Pin             : GPIO Signal
*        ==========================================================================
*        TX   : ADI_ADRV904X_GPIO_08 : ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADTXSOUT
*        RX   : ADI_ADRV904X_GPIO_17 : ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRXSIN
*        CTS  : ADI_ADRV904X_GPIO_21 : ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADCTS
*        RTS  : ADI_ADRV904X_GPIO_22 : ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRTSOUT
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure containing settings
* \param[in] index The DFE UART port index. Only DFE UART port index supported is 0.
* \param[in] settings The DFE UART port settings containing which GPIO pins to enable.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeUartCfg(adi_adrv904x_Device_t* const                   device,
                                                     const uint8_t                                  index,
                                                     const adi_adrv904x_DfeUartSettings_t* const    settings);

/**
* \brief Configures and allocates selected GPIO pins for DFE Ctrl Tx to Orx Mapping
* 
* This function will handle GPIO setup (if appropriate) and store the selected feature mode 
* in the device data structure for use at runtime.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Structure pointer to the ADRV904X device data structure
* \param[in] mappingConfig Pointer to the mapping config stored in the init structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_DfeCtrlTxToOrxMappingInit(adi_adrv904x_Device_t* const                            device,
                                                                    const adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t* const mappingConfig);

/**
* \brief Range Check API version number
*
* This function to range check the version number of the API
*
* \param[in] device     Pointer to the ADRV904X data structure
* \param[in] version    Pointer to structure where API version information
* \param[in] minVersion Pointer to structure where API version information
* \param[in] maxVersion Pointer to structure where API version information
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_ApiVersionRangeCheck(adi_adrv904x_Device_t* const         device,
                                                               const adi_adrv904x_Version_t* const  version,
                                                               const adi_adrv904x_Version_t* const  minVersion,
                                                               const adi_adrv904x_Version_t* const  maxVersion);

#endif //CLIENT_IGNORE

#endif
