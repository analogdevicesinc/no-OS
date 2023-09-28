// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_adrv9025_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* ADRV9025 API Version: 6.4.0.14
*/

/* Intermediate platform HAL layer maintained by Analog Devices */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
#include "adi_adrv9025_user.h"
#include "adi_common_hal.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_platform.h"

#ifdef _RELEASE_BUILD_
    #line __LINE__ "adi_adrv9025_hal.c"
#endif

int32_t adi_adrv9025_SpiByteWrite(adi_adrv9025_Device_t* device,
                                  uint16_t               addr,
                                  uint8_t                data)
{
    int32_t  halError                   = 0;
    int32_t  i                          = 0;
    uint16_t numTxBytes                 = 0;
    uint8_t  txData[ADRV9025_SPI_BYTES] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                    ADI_COMMON_LOG_SPI,
                                    "%s(0x%04X, 0x%02X)",
                                    addr,
                                    data);

    adi_adrv9025_SpiDataPack(device,
                             &txData[0],
                             &numTxBytes,
                             addr,
                             0xFF,
                             data,
                             ADRV9025_SPI_WRITE_POLARITY);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                    &txData[0],
                                    numTxBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiCacheWrite(adi_adrv9025_Device_t* device,
                                   const uint32_t         wrCache[],
                                   uint32_t               count)
{
    int32_t  halError                            = 0;
    uint32_t i                                   = 0;
    uint16_t numWrBytes                          = 0;
    uint8_t  wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

#if ADI_ADRV9025_VERBOSE > 0
    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        (uint16_t)(wrCache[i] >> SPI_ADDR_SIZE),
                                        (uint8_t)wrCache[i]);
    }

#endif

    for (i = 0; i < count; i++)
    {
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 wrCache[i] >> SPI_ADDR_SIZE,
                                 wrCache[i] >> SPI_MASK_SIZE,
                                 wrCache[i],
                                 ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                    &wrData[0],
                                    numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiBytesWrite(adi_adrv9025_Device_t* device,
                                   const uint16_t         addr[],
                                   const uint8_t          data[],
                                   uint32_t               count)
{
    int32_t  halError                            = 0;
    uint32_t i                                   = 0;
    uint16_t numWrBytes                          = 0;
    uint8_t  wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        addr[i],
                                        data[i]);
    }

    ADI_NULL_PTR_RETURN(&device->common,
                        addr);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    for (i = 0; i < count; i++)
    {
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 addr[i],
                                 0xFF,
                                 data[i],
                                 ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                    &wrData[0],
                                    numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiDmaWordWrite(adi_adrv9025_Device_t* device,
                                     const uint32_t         data,
                                     uint16_t               startAddress)
{
    static const uint8_t FULL_WORD_SIZE                      = 4;
    int32_t              halError                            = 0;
    uint8_t              i                                   = 0;
    uint8_t              byteToWrite                         = 0;
    uint16_t             numWrBytes                          = 0;
    uint8_t              wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    for (i = 0; i < FULL_WORD_SIZE; i++)
    {
        byteToWrite = (uint8_t)(data >> (uint32_t)((FULL_WORD_SIZE - 1 - i) * 8));
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        startAddress,
                                        byteToWrite);

        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 startAddress++,
                                 0xFF,
                                 byteToWrite,
                                 ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    //TODO make macro
    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                    &wrData[0],
                                    numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEBF,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiWriteStreamWithCustomHalFunction(adi_adrv9025_Device_t* device,
                                                         const uint8_t          data[],
                                                         uint32_t               count,
                                                         uint16_t               address)
{
    static const uint8_t NUM_OF_BYTES_OF_ADDRESS         = 2;
    static const uint8_t NUM_OF_BYTES_OF_DATA_PER_STREAM = 4;
    int32_t              halError                        = 0;
    uint8_t              i                               = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    if (adi_hal_CustomSpiStreamWrite == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adi_hal_CustomSpiStreamWrite,
                         "adi_hal_CustomSpiStreamWrite should point to a user defined hal layer function prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (device->spiSettings.enSpiStreaming != 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->spiSettings.enSpiStreaming,
                         "Please enable spi streaming prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_CustomSpiStreamWrite(device->common.devHalInfo,
                                                address,
                                                data,
                                                count,
                                                NUM_OF_BYTES_OF_ADDRESS,
                                                NUM_OF_BYTES_OF_DATA_PER_STREAM);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SpiReadStreamWithCustomHalFunction(adi_adrv9025_Device_t* device,
                                                        uint8_t                readData[],
                                                        uint32_t               count,
                                                        uint16_t               address)
{
    static const uint8_t NUM_OF_BYTES_OF_ADDRESS         = 2;
    static const uint8_t NUM_OF_BYTES_OF_DATA_PER_STREAM = 4;
    int32_t              halError                        = 0;
    uint8_t              i                               = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        readData);

    if (adi_hal_CustomSpiStreamRead == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adi_hal_CustomSpiStreamRead,
                         "adi_hal_CustomSpiStreamRead should point to a user defined hal layer function prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    if (device->spiSettings.enSpiStreaming != 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->spiSettings.enSpiStreaming,
                         "Please enable spi streaming prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_CustomSpiStreamRead(device->common.devHalInfo,
                                               address,
                                               readData,
                                               count,
                                               NUM_OF_BYTES_OF_ADDRESS,
                                               NUM_OF_BYTES_OF_DATA_PER_STREAM);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI read error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return (device->common.error.newAction);
}

int32_t adi_adrv9025_SpiDmaWordWriteStream(adi_adrv9025_Device_t* device,
                                           const uint8_t          data[],
                                           uint32_t               count,
                                           uint16_t               address)
{
#define NUM_OF_BYTES_TO_WRITE 6
    static const uint8_t WORD_LENGTH = 4;

    int32_t  halError                           = 0;
    uint8_t  i                                  = 0;
    uint32_t j                                  = 0;
    uint8_t  dataToWrite[NUM_OF_BYTES_TO_WRITE] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    if (device->spiSettings.enSpiStreaming != 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->spiSettings.enSpiStreaming,
                         "Please enable spi streaming prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((count % WORD_LENGTH) != 0) || (count == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         count,
                         "This function only supports full dma word transfers \n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dataToWrite[0] = (uint8_t)((address >> 8) & 0x007F) | (uint8_t)((ADRV9025_SPI_WRITE_POLARITY & 0x01) << 7);
    dataToWrite[1] = (uint8_t)(address & 0x00FF);
    for (j = 0; j < count; j += WORD_LENGTH)
    {
        dataToWrite[2] = data[j + 3];
        dataToWrite[3] = data[j + 2];
        dataToWrite[4] = data[j + 1];
        dataToWrite[5] = data[j + 0];
        for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
        {
            halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                        &dataToWrite[0],
                                        NUM_OF_BYTES_TO_WRITE);
            if (halError == ADI_COMMON_HAL_OK)
            {
                break;
            }
        }
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         halError,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         NULL,
                         "SPI write error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiDmaWordReadStream(adi_adrv9025_Device_t* device,
                                          uint8_t                readData[],
                                          uint32_t               count,
                                          uint16_t               address)
{
#define NUM_OF_BYTES_TO_READ 6
    static const uint8_t WORD_LENGTH = 4;

    int32_t  halError                          = 0;
    uint8_t  i                                 = 0;
    uint32_t j                                 = 0;
    uint8_t  dataToWrite[NUM_OF_BYTES_TO_READ] = {0};
    uint8_t  dataToRead[NUM_OF_BYTES_TO_READ]  = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        readData);

    if (device->spiSettings.enSpiStreaming != 1)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->spiSettings.enSpiStreaming,
                         "Please enable spi streaming prior calling this function\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((count % WORD_LENGTH) != 0) || (count == 0))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         count,
                         "This function only supports full dma word transfers \n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    dataToWrite[0] = (uint8_t)((address >> 8) & 0x007F) | (uint8_t)((~ADRV9025_SPI_WRITE_POLARITY & 0x01) << 7);
    dataToWrite[1] = (uint8_t)(address & 0x00FF);
    for (j = 0; j < count; j += WORD_LENGTH)
    {
        for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
        {
            halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                       &dataToWrite[0],
                                       &dataToRead[0],
                                       NUM_OF_BYTES_TO_READ);
            if (halError == ADI_COMMON_HAL_OK)
            {
                break;
            }
        }
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         halError,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         NULL,
                         "SPI write error");
        ADI_ERROR_RETURN(device->common.error.newAction);

        readData[j + 0] = dataToRead[5];
        readData[j + 1] = dataToRead[4];
        readData[j + 2] = dataToRead[3];
        readData[j + 3] = dataToRead[2];
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiByteRead(adi_adrv9025_Device_t* device,
                                 uint16_t               addr,
                                 uint8_t*               readData)
{
    int32_t  halError                    = 0;
    int32_t  i                           = 0;
    uint16_t numWrBytes                  = 0;
    uint8_t  wrData[ADRV9025_SPI_BYTES]  = {0};
    uint8_t  regVal                      = 0;
    uint8_t  regData[ADRV9025_SPI_BYTES] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        readData);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_adrv9025_SpiDataPack(device,
                             &wrData[0],
                             &numWrBytes,
                             addr,
                             0xFF,
                             regVal,
                             ~ADRV9025_SPI_WRITE_POLARITY);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = (adi_common_hal_Err_e)adrv9025_hal_SpiRead(device->common.devHalInfo,
                                                         &wrData[0],
                                                         &regData[0],
                                                         numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    *readData = regData[2];

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                    ADI_COMMON_LOG_SPI,
                                    "%s(0x%04X, 0x%02X)",
                                    addr,
                                    *readData);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiBytesRead(adi_adrv9025_Device_t* device,
                                  const uint16_t         addr[],
                                  uint8_t                readData[],
                                  uint32_t               count)
{
    uint32_t i                                   = 0;
    uint32_t j                                   = 0;
    int32_t  halError                            = 0;
    uint8_t  wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint8_t  rdData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint16_t numWrBytes                          = 0;
    uint8_t  regVal                              = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        addr);

    ADI_NULL_PTR_RETURN(&device->common,
                        readData);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < count; i++)
    {
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 addr[i],
                                 0xFF,
                                 regVal,
                                 ~ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (j = 0; j < ADI_ADRV9025_NUMBER_SPI_RETRY; j++)
    {
        halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                   &wrData[0],
                                   &rdData[0],
                                   numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        addr[i],
                                        rdData[(i * 3) + 2]);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < count; i++)
    {
        readData[i] = rdData[(i * 3) + 2];
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiDmaWordRead(adi_adrv9025_Device_t* device,
                                    uint32_t*              data,
                                    uint16_t               startAddress)
{
    static const uint8_t FULL_WORD_SIZE                      = 4;
    uint32_t             i                                   = 0;
    uint32_t             j                                   = 0;
    int32_t              halError                            = 0;
    uint8_t              wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint8_t              rdData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint16_t             numWrBytes                          = 0;
    uint8_t              regVal                              = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        data);

    for (i = 0; i < FULL_WORD_SIZE; i++)
    {
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 startAddress++,
                                 0xFF,
                                 regVal,
                                 ~ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (j = 0; j < ADI_ADRV9025_NUMBER_SPI_RETRY; j++)
    {
        halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                   &wrData[0],
                                   &rdData[0],
                                   numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }

    /* Revert the value of startAddress to its original value */
    startAddress -= FULL_WORD_SIZE;
    for (i = 0; i < FULL_WORD_SIZE; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        startAddress++,
                                        rdData[(i * 3) + 2]);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEBF,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI read error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    *data = 0;
    for (i = 0; i < FULL_WORD_SIZE; i++)
    {
        *data |= ((uint32_t)rdData[(i * 3) + 2] << ((FULL_WORD_SIZE - 1 - i) * 8));
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiCacheRead(adi_adrv9025_Device_t* device,
                                  const uint32_t         rdCache[],
                                  uint8_t                readData[],
                                  uint32_t               count)
{
    uint32_t i                                   = 0;
    uint32_t j                                   = 0;
    int32_t  halError                            = 0;
    uint8_t  wrData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint8_t  rdData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};
    uint16_t numWrBytes                          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        rdCache);

    ADI_NULL_PTR_RETURN(&device->common,
                        readData);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (i = 0; i < count; i++)
    {
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 rdCache[i] >> SPI_ADDR_SIZE,
                                 0xFF,
                                 0,
                                 ~ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    for (j = 0; j < ADI_ADRV9025_NUMBER_SPI_RETRY; j++)
    {
        halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                   &wrData[0],
                                   &readData[0],
                                   numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                        ADI_COMMON_LOG_SPI,
                                        "%s(0x%04X, 0x%02X)",
                                        wrData[i],
                                        rdData[(i * 3) + 2]);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiFieldWrite(adi_adrv9025_Device_t* device,
                                   uint16_t               addr,
                                   uint8_t                fieldVal,
                                   uint8_t                mask,
                                   uint8_t                startBit)
{
    int32_t  halError   = 0;
    uint16_t numWrBytes = 0;
    uint8_t  regVal     = ((fieldVal << startBit) & mask);
    uint8_t  newRegVal  = 0;

    uint8_t registerVal[ADRV9025_SPI_BYTES] = {0};
    uint8_t wrData[ADRV9025_HW_RMW_BYTES]   = {0};
    uint8_t i                               = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                    ADI_COMMON_LOG_SPI,
                                    "%s(0x%04X, 0x%02X)",
                                    addr,
                                    regVal);

    if (device->common.cacheInfo.HW_RMW_Enabled)
    {
        /* Write with mask(with 12 byte packages) when HW_RMW_Enabled is available */
        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 addr,
                                 mask,
                                 regVal,
                                 ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
        for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
        {
            halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                        &wrData[0],
                                        numWrBytes);
            if (halError == ADI_COMMON_HAL_OK)
            {
                break;
            }
        }
    }
    else
    {
        /* Generate an error if wrOnly is TRUE, since rest of the code will perform manual read/modify/write */
        if (device->common.cacheInfo.wrOnly == ADI_TRUE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->common.cacheInfo.wrOnly,
                             "Cannot write to spi field when wrOnly is true and HW_RMW is not enabled");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 addr,
                                 0xFF,
                                 regVal,
                                 ~ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);
        for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
        {
            halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                       &wrData[0],
                                       &registerVal[0],
                                       numWrBytes);
            if (halError == ADI_COMMON_HAL_OK)
            {
                break;
            }
        }
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEHAL,
                         (adi_common_ErrSources_e)halError,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         NULL,
                         "SPI read error");
        ADI_ERROR_RETURN(device->common.error.newAction);

        newRegVal  = (registerVal[2] & ~mask) | (regVal & mask);
        numWrBytes = 0;

        adi_adrv9025_SpiDataPack(device,
                                 &wrData[0],
                                 &numWrBytes,
                                 addr,
                                 0xFF,
                                 newRegVal,
                                 ADRV9025_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
        {
            halError = adrv9025_hal_SpiWrite(device->common.devHalInfo,
                                        &wrData[0],
                                        numWrBytes);
            if (halError == ADI_COMMON_HAL_OK)
            {
                break;
            }
        }
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEHAL,
                         (adi_common_ErrSources_e)halError,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         NULL,
                         "SPI write error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiFieldRead(adi_adrv9025_Device_t* device,
                                  uint16_t               addr,
                                  uint8_t*               fieldVal,
                                  uint8_t                mask,
                                  uint8_t                startBit)
{
    /* only use if caching is off */
    int32_t  halError   = 0;
    uint16_t numWrBytes = 0;
    uint8_t  wrData[8]  = {0};
    uint8_t  rdData[8]  = {0};
    uint8_t  regVal     = 0;
    uint8_t  i          = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_HAL);

    ADI_NULL_PTR_RETURN(&device->common,
                        fieldVal);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    adi_adrv9025_SpiDataPack(device,
                             &wrData[0],
                             &numWrBytes,
                             addr,
                             0xFF,
                             regVal,
                             ~ADRV9025_SPI_WRITE_POLARITY);
    ADI_ERROR_RETURN(device->common.error.newAction);

    for (i = 0; i < ADI_ADRV9025_NUMBER_SPI_RETRY; i++)
    {
        halError = adrv9025_hal_SpiRead(device->common.devHalInfo,
                                   &wrData[0],
                                   &rdData[0],
                                   numWrBytes);
        if (halError == ADI_COMMON_HAL_OK)
        {
            break;
        }
    }
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_ADRV9025_ACT_ERR_RESET_SPI,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    regVal   = rdData[2];
    startBit = startBit & 0x0F;

    *fieldVal = (uint8_t)((regVal & mask) >> startBit);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common,
                                    ADI_COMMON_LOG_SPI,
                                    "%s(0x%04X, 0x%02X)",
                                    addr,
                                    regVal);

    return device->common.error.newAction;
}

int32_t adi_adrv9025_SpiDataPack(adi_adrv9025_Device_t* device,
                                 uint8_t*               wrData,
                                 uint16_t*              numWrBytes,
                                 uint16_t               addr,
                                 uint8_t                mask,
                                 uint8_t                data,
                                 uint8_t                writeFlag)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        wrData);

    ADI_NULL_PTR_RETURN(&device->common,
                        numWrBytes);

    if (mask == 0xFF)
    {
        /* if the mask is 0xFF then the entire 8 bits is being written to the
           register and so no masking is required and the operation can be
           achieved using 3 bytes*/
        if ((*numWrBytes + ADRV9025_SPI_BYTES) > ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEHAL,
                             ADRV9025HAL_BUFFER_OVERFLOW,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             NULL,
                             "Buffer size Exceeded error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        wrData[(*numWrBytes)++] = (uint8_t)(((writeFlag & 0x01) << 7) | ((addr >> 8) & 0x7F));
        wrData[(*numWrBytes)++] = (uint8_t)(addr);
        /* coverty will trigger an error which is ok in this case. */
        wrData[(*numWrBytes)++] = (uint8_t)data;
    }
    else
    {
        if (device->common.cacheInfo.HW_RMW_Enabled == ADI_FALSE)
        {
            /* Generate an error if HW_RMW is not enabled yet */
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_ADI_HAL,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->common.cacheInfo.HW_RMW_Enabled,
                             "Cannot use HW_RMW functionality to write a field in Spi register because HW_RMW_Enabled is not available");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        if ((*numWrBytes + ADRV9025_HW_RMW_BYTES) > ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEHAL,
                             ADRV9025HAL_BUFFER_OVERFLOW,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             NULL,
                             "Buffer size Exceeded error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* pack the low address byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_LO_ADDR >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_LO_ADDR);
        wrData[(*numWrBytes)++] = (uint8_t)addr;

        /* pack the high address byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_HI_ADDR >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_HI_ADDR);
        wrData[(*numWrBytes)++] = (uint8_t)(addr >> 8);

        /* pack the mask byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_MASK >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_MASK);
        wrData[(*numWrBytes)++] = (uint8_t)mask;

        /* pack the mask byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_DATA >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9025_HW_RMW_DATA);
        /*coverty will trigger an error which is ok in this case.*/
        wrData[(*numWrBytes)++] = (uint8_t)data;
    }

    return device->common.error.newAction;
}

int32_t adi_adrv9025_HwVerify(adi_adrv9025_Device_t *device)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_COMMON_LOG_API_PRIV);

    return adi_common_hal_HwVerify(&device->common);
}
