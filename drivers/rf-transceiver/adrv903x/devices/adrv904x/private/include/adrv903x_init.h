/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_init.h
 * \brief Contains ADRV903X init related private function prototypes for
 *        adrv903x_init.c that helps adi_adrv903x_init.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_INIT_H_
#define _ADRV903X_INIT_H_

#include "adi_adrv903x_error.h"
#include "adi_library.h"

//#define ADRV903X_INIT_DEBUG 1
#ifdef ADRV903X_INIT_DEBUG
#define ADRV903X_BUGINFO(x) ADI_LIBRARY_PRINTF("MESSAGE: %s ******************************* \n", (x));
#define ADRV903X_BUGINFO_NUM(x,n) ADI_LIBRARY_PRINTF("MESSAGE: %s: %d 0x%08x \n", (x),(n),(n));
#else
#define ADRV903X_BUGINFO(x) 
#define ADRV903X_BUGINFO_NUM(x,n) 
#endif

//#define ADRV903X_INIT_DMAINFO_DEBUG 1
#ifdef ADRV903X_INIT_DMAINFO_DEBUG
#define ADRV903X_DMAINFO(text, addr, count) ADI_LIBRARY_PRINTF("MESSAGE: DMA: %30s: addr=0x%08x, count=%d \n", (text), (addr), (count));
#else
#define ADRV903X_DMAINFO(text, addr, count)
#endif

//#define ADRV903X_INIT_DMA_DEBUG 1
#ifdef ADRV903X_INIT_DMA_DEBUG
#define ADRV903X_SPIDMAINFO(s,a,b,c) ADI_LIBRARY_PRINTF((s),(a),(b),(c));
#else
#define ADRV903X_SPIDMAINFO(s,a,b,c)
#endif

//#define ADRV903X_INIT_SPI_DEBUG 1
#ifdef ADRV903X_INIT_SPI_DEBUG
#define ADRV903X_SPIINFO(s,a,b,c) ADI_LIBRARY_PRINTF((s),(a),(b),(c));
#define ADRV903X_SPI_FIELD_INFO(s,a,b,c,d) ADI_LIBRARY_PRINTF((s),(a),(b),(c), (d));
#else
#define ADRV903X_SPIINFO(s,a,b,c)
#define ADRV903X_SPI_FIELD_INFO(s,a,b,c,d)
#endif

#define ADRV903X_SPIWRITEBYTE_RETURN(text, addr, data, recoveryAction)                              \
{                                                                                                   \
    recoveryAction = adi_adrv903x_Register32Write(device, NULL, (addr), (data), 0xFFU);             \
    if(ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)                                                 \
    {                                                                                               \
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Byte Write Issue");              \
        return recoveryAction;                                                                      \
    }                                                                                               \
    ADRV903X_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data));  \
}

#define ADRV903X_SPIWRITEBYTE_GOTO(text, addr, data, recoveryAction, label)                         \
{                                                                                                   \
    recoveryAction = adi_adrv903x_Register32Write(device, NULL, (addr), (data), 0xFFU);             \
    if(ADI_ADRV903X_ERR_ACT_NONE != recoveryAction)                                                 \
    {                                                                                               \
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Byte Write Issue");              \
        goto cleanup;                                                                               \
    }                                                                                               \
    ADRV903X_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data));  \
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
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure
* \param[in] rxChannel Channel for which overload protection config is desired
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_RxOverloadProtectionSet(adi_adrv903x_Device_t* const    device,
                                                                  const adi_adrv903x_RxChannels_e rxChannel);

/**
* \brief Private function used by the APIs that can benefit from SPI streaming. This function should be called
*        at the beginning of the function. If device->devStatInfo->spiOptions.allowSpiStreaming is set the part
*        is placed into streaming mode. Otherwise, this function does nothing.
*
* This function to range check the version number of the API
*
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_SpiStreamingEntry(adi_adrv903x_Device_t* const device);
                                                                                                               
/**
* \brief Private function used by the APIs that call adrv903x_SpiStreamingEntry. It is the responsibility of each
*        API function that calls adrv903x_SpiStreamingEntry to call this function also to disable streaming if
*        necessary.
*
* This function to range check the version number of the API
*
* \param[in,out] device Context variable - Pointer to the ADRV903X data structure
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_SpiStreamingExit(adi_adrv903x_Device_t* const device);

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
* \param[in,out] device Context variable - Structure pointer to the ADRV903X device data structure
* \param[in] mappingConfig Pointer to the mapping config stored in the init structure
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxToOrxMappingInit(adi_adrv903x_Device_t* const                       device,
                                                             const adi_adrv903x_TxToOrxMappingConfig_t* const   mappingConfig);
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
* \retval adi_adrv903x_ErrAction_e, ADI_ADRV903X_ERR_ACT_NONE if successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_ClocksSync(adi_adrv903x_Device_t* const        device,
                                                     const adi_adrv903x_Init_t* const    init);



/**
* \brief Initializes power monitoring related parameters during device bootup.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure
* \param[in] txChannelMask Tx channel/s to initialize power monitor configuration for
* \param[in] totalInputInterpoloationRate Total interpolation rate for input signal.
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_TxPowerMonitorInitialize(adi_adrv903x_Device_t* const device,
                                                                   const uint32_t               txChannelMask,
                                                                   const uint8_t                totalInputInterpoloationRate);

/**
* \brief Helper function to set the specified Uart Signal to Gpio Output Pin.
*
* \pre This function can be called before the CPU start
*
*        GPIO Pins            : UART Signals
*        ==============================================================
*        ADI_ADRV903X_GPIO_09 : ADI_ADRV903X_GPIO_SIGNAL_UART_PADRXSIN
*        ADI_ADRV903X_GPIO_10 : ADI_ADRV903X_GPIO_SIGNAL_UART_PADCTS
*        ADI_ADRV903X_GPIO_11 : ADI_ADRV903X_GPIO_SIGNAL_UART_PADRTSOUT
*        ADI_ADRV903X_GPIO_12 : ADI_ADRV903X_GPIO_SIGNAL_UART_PADTXSOUT
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV903X device data structure containing settings
* \param[in] pinSelect The GPIO Pin Select for UART signal
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_UartCfg(adi_adrv903x_Device_t* const       device,
                                                  const adi_adrv903x_GpioPinSel_e    pinSelect);

/**
* \brief Range Check API version number
*
* This function to range check the version number of the API
*
* \param[in] device     Pointer to the ADRV903X data structure
* \param[in] version    Pointer to structure where API version information
* \param[in] minVersion Pointer to structure where API version information
* \param[in] maxVersion Pointer to structure where API version information
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_ApiVersionRangeCheck(adi_adrv903x_Device_t* const         device,
                                                               const adi_adrv903x_Version_t* const  version,
                                                               const adi_adrv903x_Version_t* const  minVersion,
                                                               const adi_adrv903x_Version_t* const  maxVersion);

#endif //CLIENT_IGNORE

#endif
