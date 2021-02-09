/**
 * \file
 * \brief Contains ADRV9001 init related private function prototypes for
 *        adrv9001_init.c that helps adi_adrv9001_init.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV9001_INIT_H_
#define _ADRV9001_INIT_H_

#include "adi_adrv9001.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define ADRV9001_INIT_DEBUG 1
#ifdef ADRV9001_INIT_DEBUG
#include <stdio.h>
#define ADRV9001_DEBUG_INFO(x) printf("MESSAGE: %s ******************************* \n", (x));
#define ADRV9001_DEBUG_INFO_NUM(x,n) printf("MESSAGE: %s: %d 0x%08x \n", (x),(n),(n));
#else
#define ADRV9001_DEBUG_INFO(x)
#define ADRV9001_DEBUG_INFO_NUM(x,n)
#endif

//#define ADRV9001_INIT_DMAINFO_DEBUG 1
#ifdef ADRV9001_INIT_DMAINFO_DEBUG
#include <stdio.h>
#define ADRV9001_DMAINFO(text, addr, count) printf("MESSAGE: DMA: %30s: addr=0x%08x, count=%d \n", (text), (addr), (count));
#else
#define ADRV9001_DMAINFO(text, addr, count)
#endif

//#define ADRV9001_INIT_DMA_DEBUG 1
#ifdef ADRV9001_INIT_DMA_DEBUG
#include <stdio.h>
#define ADRV9001_SPIDMAINFO(s,a,b,c) printf((s),(a),(b),(c));
#else
#define ADRV9001_SPIDMAINFO(s,a,b,c)
#endif

#define ADRV9001_SPIWRITEBYTESDMA(devicePtr, text, addr, addrArray, dataArray, count) \
{\
    int32_t recoveryAction = 0; \
    recoveryAction = adi_adrv9001_spi_Bytes_Write(devicePtr, (addrArray), (dataArray), (count)); \
    ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing bytes to DMA over SPI"); \
    ADRV9001_DMAINFO((text), (addr), (count)); \
}


#define ADRV9001_SPIWRITEBYTEDMA(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv9001_spi_Byte_Write(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV9001_SPIDMAINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
    }

#define ADRV9001_SPIWRITEBYTESTREAMDMA(devicePtr, text, addr, dataArray, count) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv9001_spi_Bytes_Stream_Write(devicePtr, (addr), (dataArray), count); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV9001_DMAINFO((text), (addr), (count)); \
    }

#define ADRV9001_SPIREADBYTEDMA(devicePtr, text, addr, data) \
    {\
        recoveryAction = adi_adrv9001_spi_Byte_Read(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while Reading byte from DMA over SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV9001_SPIDMAINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
    }


//#define ADRV9001_INIT_SPI_DEBUG 1
#ifdef ADRV9001_INIT_SPI_DEBUG
#include <stdio.h>
#define ADRV9001_SPIINFO(s,a,b,c) printf((s),(a),(b),(c));
#define ADRV9001_SPI_FIELD_INFO(s,a,b,c,d) printf((s),(a),(b),(c), (d));
#else
#define ADRV9001_SPIINFO(s,a,b,c)
#define ADRV9001_SPI_FIELD_INFO(s,a,b,c,d)
#endif

#define ADRV9001_SPIINFO_NO(s,a,b,c)

#define ADRV9001_SPIWRITEBYTE(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0; \
        recoveryAction = adi_adrv9001_spi_Byte_Write(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV9001_DEBUG_INFO(__FUNCTION__); \
        ADRV9001_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
    }

#define ADRV9001_SPIREADBYTE(devicePtr, text, addr, data) \
    {\
        int32_t recoveryAction = 0;\
        recoveryAction = adi_adrv9001_spi_Byte_Read(devicePtr, (addr), (data)); \
        ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading byte from SPI"); \
        ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
        ADRV9001_DEBUG_INFO(__FUNCTION__); \
        ADRV9001_SPIINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
    }

#define ADRV9001_SPIFIELDWRITE(devicePtr, addr, fieldVal, mask, startBit, text) \
{ \
int32_t recoveryAction = 0;\
recoveryAction = adi_adrv9001_spi_Field_Write(devicePtr, addr, fieldVal, mask, startBit); \
ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error on SPI field write"); \
ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
ADRV9001_SPI_FIELD_INFO("MESSAGE: WRITE FIELD: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}


#define ADRV9001_SPIFIELDREAD(devicePtr, addr, fieldVal, mask, startBit, text) \
{ \
int32_t recoveryAction = 0;\
recoveryAction = adi_adrv9001_spi_Field_Read(devicePtr, addr, (fieldVal), mask, startBit); \
    ADI_ERROR_REPORT(&devicePtr->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error on SPI field read"); \
ADI_ERROR_RETURN(devicePtr->common.error.newAction); \
ADRV9001_SPI_FIELD_INFO("MESSAGE: READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}

/**
* \brief Set various Analog registers in the device
*
* \pre This function is private and is not called directly by the user.
*
* \param[in] device                        Structure pointer to ADRV9001 device data structure
* \param[in] init                          Structure pointer to ADRV9001 init data structure
* \param[in] adrv9001DeviceClockOutDivisor ADRV9001 device clock output divisor. An enum type ranging from 0 to 6,
*                                          the divisor value will be 2^N (1, 2, 4, 8, 16, 32, 64)
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_InitAnalog(adi_adrv9001_Device_t *device,
                            adi_adrv9001_Init_t *init,
                            adi_adrv9001_DeviceClockDivisor_e adrv9001DeviceClockOutDivisor);
    
/**
* \brief Verifies whether the init structure profiles are valid.
*
* This function checks that the Rx/Tx/ORx profiles have valid clock rates in
* order to operate together.  Rx/Tx and ORx share a common high speed digital
* clock. If an invalid combination of profiles is detected, an error will be
* returned. If a profile in the init structure is unused, the user should zero
* out all members of that particular profile structure.  If a Rx/Tx/ORx profile
* has an IQ rate = 0, it is assumed that the profile is disabled.
*
* \pre This function is private and is not called directly by the user.
*
* This function uses Adrv9001_TxProfileVerify(), Adrv9001_RxProfileVerify(), and
* Adrv9001_OrxProfileVerify() as helper functions.
*
* \param device Structure pointer to ADRV9001 device data structure
* \param init Pointer to ADRV9001 initialization settings structures
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_ProfilesVerify(adi_adrv9001_Device_t *device, adi_adrv9001_Init_t *init);

/**
* \brief Set various Analog clock registers in the device
*
* \pre This function is private and is not called directly by the user.
*
* \param device Structure pointer to ADRV9001 device data structure
* \param init   Structure pointer to ADRV9001 init data structure
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_AnalogClockSet(adi_adrv9001_Device_t *device, adi_adrv9001_Init_t *init);

#ifdef __cplusplus
}
#endif

#endif