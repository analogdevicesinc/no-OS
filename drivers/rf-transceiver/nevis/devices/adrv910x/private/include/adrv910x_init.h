/**
 * \file
 * \brief Contains ADRV910X init related private function prototypes for
 *        adrv910x_init.c that helps adi_adrv910x_init.c
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV910X_INIT_H_
#define _ADRV910X_INIT_H_

#include "adi_adrv910x_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define ADRV910X_INIT_DEBUG 1
#ifdef ADRV910X_INIT_DEBUG
#include <stdio.h>
#define ADRV910X_DEBUG_INFO(x) printf("MESSAGE: %s ******************************* \n", (x));
#define ADRV910X_DEBUG_INFO_NUM(x,n) printf("MESSAGE: %s: %d 0x%08x \n", (x),(n),(n));
#else
#define ADRV910X_DEBUG_INFO(x)
#define ADRV910X_DEBUG_INFO_NUM(x,n)
#endif

//#define ADRV910X_INIT_DMAINFO_DEBUG 1
#ifdef ADRV910X_INIT_DMAINFO_DEBUG
#include <stdio.h>
#define ADRV910X_DMAINFO(text, addr, count) printf("MESSAGE: DMA: %30s: addr=0x%08x, count=%d \n", (text), (addr), (count));
#else
#define ADRV910X_DMAINFO(text, addr, count)
#endif

//#define ADRV910X_INIT_DMA_DEBUG 1
#ifdef ADRV910X_INIT_DMA_DEBUG
#include <stdio.h>
#define ADRV910X_SPIDMAINFO(s,a,b,c) printf((s),(a),(b),(c));
#else
#define ADRV910X_SPIDMAINFO(s,a,b,c)
#endif

#define ADRV910X_SPIWRITEBYTESDMA(devicePtr, text, addr, addrArray, dataArray, count) \
{\
    int32_t recoveryAction = 0; \
    recoveryAction = adi_adrv910x_spi_Bytes_Write(devicePtr, (addrArray), (dataArray), (count)); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing bytes to DMA over SPI"); \
    ADRV910X_DMAINFO((text), (addr), (count)); \
}


#define ADRV910X_SPIWRITEBYTEDMA(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv910x_spi_Byte_Write(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV910X_SPIDMAINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
    }

#define ADRV910X_SPIWRITEBYTESTREAMDMA(devicePtr, text, addr, dataArray, count) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv910x_spi_Bytes_Stream_Write(devicePtr, (addr), (dataArray), count); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV910X_DMAINFO((text), (addr), (count)); \
    }

#define ADRV910X_SPIREADBYTEDMA(devicePtr, text, addr, data) \
    {\
        recoveryAction = adi_adrv910x_spi_Byte_Read(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while Reading byte from DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV910X_SPIDMAINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
    }


//#define ADRV910X_INIT_SPI_DEBUG 1
#ifdef ADRV910X_INIT_SPI_DEBUG
#include <stdio.h>
#define ADRV910X_SPIINFO(s,a,b,c) printf((s),(a),(b),(c));
#define ADRV910X_SPI_FIELD_INFO(s,a,b,c,d) printf((s),(a),(b),(c), (d));
#else
#define ADRV910X_SPIINFO(s,a,b,c) printf(s, a, b, c)
#define ADRV910X_SPI_FIELD_INFO(s,a,b,c,d) printf(s,a,b,c,d)
#endif

#define ADRV910X_SPIINFO_NO(s,a,b,c)

#define ADRV910X_SPIWRITEBYTE(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv910x_spi_Byte_Write(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV910X_DEBUG_INFO(__FUNCTION__); \
        ADRV910X_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
    }

#define ADRV910X_SPIREADBYTE(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0;\
        recoveryAction = adi_adrv910x_spi_Byte_Read(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading byte from SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV910X_DEBUG_INFO(__FUNCTION__); \
        ADRV910X_SPIINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
    }

#define ADRV910X_SPIFIELDWRITE(devicePtr, addr, fieldVal, mask, startBit, text) \
{ \
int32_t recoveryAction = 0;\
recoveryAction = adi_adrv910x_spi_Field_Write(devicePtr, addr, fieldVal, mask, startBit); \
ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error on SPI field write"); \
ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
ADRV910X_SPI_FIELD_INFO("MESSAGE: WRITE FIELD: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}


#define ADRV910X_SPIFIELDREAD(devicePtr, addr, fieldVal, mask, startBit, text) \
{ \
int32_t recoveryAction = 0;\
recoveryAction = adi_adrv910x_spi_Field_Read(devicePtr, addr, (fieldVal), mask, startBit); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error on SPI field read"); \
ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
ADRV910X_SPI_FIELD_INFO("MESSAGE: READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}
	
/**
* \brief Set various Analog registers in the device
*
* \pre This function is private and is not called directly by the user.
*
* \param[in] device                        Structure pointer to ADRV910X device data structure
* \param[in] profile                       Structure pointer to ADRV910X profile data structure
* \param[in] adrv910xDeviceClockOutDivisor ADRV910X device clock output divisor. An enum type ranging from 0 to 6,
*                                          the divisor value will be 2^N (1, 2, 4, 8, 16, 32, 64)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_InitAnalog(adi_adrv910x_Device_t *device,
                            deviceProfile_t *profile,
                            adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor);

/**
* \brief Configure device clock output divider
*
* This function initializes the ADRV910X device output clock divider
*
*
* \param[in] adrv910x						Context variable - Pointer to the ADRV910X device data structure
* \param[in] adrv910xDeviceClockOutDivisor ADRV910X device clock output divisor; An enum type ranging from 0 to 6,
*                                          the divisor value will be 2^N (1, 2, 4, 8, 16, 32, 64)"
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adrv910x_SetDeviceClockOutDivider(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor);
	
#ifdef __cplusplus
}
#endif

#endif
