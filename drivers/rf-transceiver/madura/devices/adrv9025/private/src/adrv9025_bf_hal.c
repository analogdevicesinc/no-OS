// SPDX-License-Identifier: GPL-2.0
/**
* \file adrv9025_bf_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information.
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_common_hal.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "./../../private/include/adrv9025_bf_hal.h"

int32_t adrv9025_BfWriteCacheEn(adi_adrv9025_Device_t*      device,
                                adrv9025_Bf_WrCache_State_e wrCacheState)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);
    if (wrCacheState > ADRV9025_WR_HW_RMW_ON)
    {
        if (device->common.cacheInfo.HW_RMW_Enabled == ADI_FALSE && device->common.cacheInfo.wrOnly == ADI_FALSE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Caching not available");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    device->common.cacheInfo.wrCacheState = (uint8_t)wrCacheState;
    /* if merge caching is selected ensure merge distance is at least = 1*/
    if ((device->common.cacheInfo.mergeDistance == 0) && (wrCacheState == ADRV9025_WR_GBL_MERGE_ON))
    {
        device->common.cacheInfo.mergeDistance = 1;
    }

    return recoveryAction;
}

int32_t adrv9025_BfReadCacheEn(adi_adrv9025_Device_t*      device,
                               adrv9025_Bf_RdCache_State_e rdCacheState)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    device->common.cacheInfo.rdCacheState = rdCacheState;

    return recoveryAction;
}

int32_t adrv9025_BfReadCacheInit(adi_adrv9025_Device_t* device)
{
    uint64_t rdData         = 0;
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t numRdBytes     = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    if (device->common.cacheInfo.rdCacheState == ADRV9025_RD_BF_CACHE_ON)
    {
        recoveryAction = adrv9025_BfReadCacheFlush(device,
                                                   &rdData,
                                                   &numRdBytes);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         "Cache init failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
        /* if Global Caching is being used, no further flushing should be performed
         * until the user calls bf flush */
    }

    return recoveryAction;
}

int32_t adrv9025_BfWriteCacheInit(adi_adrv9025_Device_t* device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    if (device->common.cacheInfo.wrCacheState > ADRV9025_WR_HW_RMW_ON)
    {
        if ((device->common.cacheInfo.HW_RMW_Enabled == ADI_FALSE) && (device->common.cacheInfo.wrOnly == ADI_FALSE))
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Caching not available");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    /* skip cacheInit if Global level Cache is on. */
    if (device->common.cacheInfo.wrCacheState == ADRV9025_WR_BF_CACHE_ON)
    {
        /* Ensures the cache is empty if bitField Level Caching is on */
        recoveryAction = adrv9025_BfWriteCacheFlush(device);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_API_FAIL,
                         recoveryAction,
                         device->common.cacheInfo.wrFlushEn,
                         "Wr Cache flush failure");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* if using bitField level caching re-enable caching after each flush */
        device->common.cacheInfo.wrFlushEn = ADI_ENABLE;
    }
    else
    {
        /* disable flushing within the bitfield function when global
         * caching is being used */
        device->common.cacheInfo.wrFlushEn = ADI_DISABLE;
    }

    return recoveryAction;
}

int32_t adrv9025_BfByteWrite(adi_adrv9025_Device_t* device,
                             uint16_t               addr,
                             uint8_t                regVal)
{
    int32_t  recoveryAction   = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i                = 0;
    uint8_t  addrFoundInCache = ADI_FALSE;
    uint8_t  wrFlushEnSaved   = device->common.cacheInfo.wrFlushEn;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    if (device->common.cacheInfo.wrCacheState == ADRV9025_WR_GBL_MERGE_ON)
    {
        /* Write merging is on, so the cache is searched to see if address is in the cache */
        for (i = 0; i < device->common.cacheInfo.wrCacheIdx; i++)
        {
            /* The cache is packed in the form AABBMMDD
             * where AA is MSB Addr, BB is the LSB addr,
             * MM is the mask and DD is the Data.
             */
            /* if the address part of the cache word = wanted address */
            if ((uint16_t)(device->common.cacheInfo.wrCache[i] >> SPI_ADDR_SIZE) == addr)
            {
                addrFoundInCache = ADI_TRUE;
                ADI_FUNCTION_ENTRY_LOG(&device->common,
                                       ADI_COMMON_LOG_BF);
                /* preserve the address but clear the data. */

                device->common.cacheInfo.wrCache[i] &= 0xFFFFFF00;
                device->common.cacheInfo.wrCache[i] |= (((uint32_t)0xFF << SPI_MASK_SIZE) | regVal);
            }
        }
    }

    if ((device->common.cacheInfo.wrCacheState > ADRV9025_WR_HW_RMW_ON) &&
        (addrFoundInCache == ADI_FALSE))
    {
        /* Write caching is turned on and the bitField has not been merged */
        if ((device->common.cacheInfo.spiWrBufferCnt + ADRV9025_SPI_BYTES) >= ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            /* if cache is full then flush cache and add next instruction.
             * should not be a problem flushing a write cache. */
            device->common.cacheInfo.wrFlushEn = ADI_ENABLE;
            recoveryAction                     = adrv9025_BfWriteCacheFlush(device);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             device->common.cacheInfo.spiWrBufferCnt,
                             "Wr Cache flush failure");
            ADI_ERROR_RETURN(device->common.error.newAction);
            device->common.cacheInfo.wrFlushEn = wrFlushEnSaved;
        }

        device->common.cacheInfo.wrCache[device->common.cacheInfo.wrCacheIdx++] =
        ((uint32_t)addr << SPI_ADDR_SIZE) | (0xFF << SPI_MASK_SIZE) | regVal;
        /* track the size of the spibuffer needed to write the cache. */
        device->common.cacheInfo.spiWrBufferCnt += ADRV9025_SPI_BYTES;
    }

    if (device->common.cacheInfo.wrCacheState < ADRV9025_WR_BF_CACHE_ON)
    {
        /* Write caching is not being used, so do an immediate write */
        if (device->common.devHalInfo != NULL)
        {
            recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                       addr,
                                                       regVal);
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_API_FAIL,
                             recoveryAction,
                             NULL,
                             "SPI write failure");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    return recoveryAction;
}

int32_t adrv9025_BfByteRead(adi_adrv9025_Device_t* device,
                            uint16_t               addr,
                            uint8_t*               regVal,
                            uint8_t                endOfBf)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common,
                        regVal);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (device->common.cacheInfo.rdCacheState > ADRV9025_RD_CACHE_OFF)
    {
        /* Read Caching is turned on */
        if ((device->common.cacheInfo.spiRdBufferCnt + ADRV9025_SPI_BYTES) >= ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            /* if the cache is full then raise an error */
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Rx Cache size exceeded error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        /* set bits [31:16] of the cache to the 16 bit address
         * set bits [15:8] of cache to the 8 bit mask address
         * set bits [7:0] of cache to the 8 bit endOfBf.
         * endOfBf contains two 4 bit fields.
         * [7:4] is 0xF or 0x0 and masks a end of bitField data value.
         * [3:0] is the register startBit ie where the value resides in the register.
         */
        device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx++] =
        ((uint32_t)addr << SPI_ADDR_SIZE) | (0xFF << SPI_DATA_SIZE) | (uint32_t)endOfBf;
    }
    else
    {
        recoveryAction = adi_adrv9025_SpiByteRead(device,
                                                  addr,
                                                  regVal);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_SPI_FAIL,
                         recoveryAction,
                         NULL,
                         "SPI byte read failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_BfFieldWrite(adi_adrv9025_Device_t* device,
                              uint16_t               addr,
                              uint8_t                fieldVal,
                              uint8_t                mask,
                              uint8_t                startBit)
{
    static const uint8_t MAX_START_BIT = 7;

    int32_t  recoveryAction    = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i                 = 0;
    int16_t  mergeIdx          = 0;
    uint8_t  regVal            = 0;
    uint8_t  addrFoundInCache  = ADI_FALSE;
    uint8_t  cachemask         = 0;
    uint8_t  newMask           = 0;
    uint8_t  cacheVal          = 0;
    uint8_t  newVal            = 0;
    uint8_t  registerVal       = 0;
    uint8_t  newRegVal         = 0;
    uint8_t  wrFlushEnSaved    = device->common.cacheInfo.wrFlushEn;
    uint8_t  wrCacheStateLocal = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    if (startBit > MAX_START_BIT)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_ADRV9025_ACT_ERR_FLUSH_CACHE,
                         NULL,
                         "startbit exceeded its max value of 7 ");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((device->common.cacheInfo.wrCacheState > ADRV9025_WR_CACHE_OFF) && (device->common.cacheInfo.HW_RMW_Enabled == ADI_DISABLE))
    {
        /* If HW_RMW functionality is unavailable, we should force the manual read/modify/write operation since
         * it's not possible to call adi_adrv9025SpiDataPack with a mask different than 0xFF */
        wrCacheStateLocal = ADRV9025_WR_CACHE_OFF;
    }
    else
    {
        wrCacheStateLocal = device->common.cacheInfo.wrCacheState;
    }

    regVal = ((fieldVal << startBit) & mask);

    if (wrCacheStateLocal == ADRV9025_WR_GBL_MERGE_ON)
    {
        /* if the cache is larger then the merge distance start looking for
         * the address merge distance indexes back else search from the beginning
         * if found merge the new data and mask with the existing data
         */
        if (device->common.cacheInfo.wrCacheIdx > device->common.cacheInfo.mergeDistance)
        {
            mergeIdx = device->common.cacheInfo.wrCacheIdx - device->common.cacheInfo.mergeDistance;
            if (mergeIdx < 0)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_DEVICEBF,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_ADRV9025_ACT_ERR_RESET_CACHE,
                                 NULL,
                                 "mergIdx cannot be negative");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }

        for (i = mergeIdx; i < device->common.cacheInfo.wrCacheIdx; i++)
        {
            /* search the cache for addr in the cache
             * if found merge field val and the mask into the original values
             */
            if ((uint16_t)(device->common.cacheInfo.wrCache[i] >> SPI_ADDR_SIZE) == addr)
            {
                addrFoundInCache = ADI_TRUE;
                cachemask        = (uint8_t)((device->common.cacheInfo.wrCache[i] & 0x0000FF00) >> SPI_MASK_SIZE);
                newMask          = cachemask | mask;
                cacheVal         = (uint8_t)(device->common.cacheInfo.wrCache[i] & 0x000000FF);
                cacheVal &= ~mask;
                newVal = cacheVal | regVal;

                /* clear org mask and data. */
                device->common.cacheInfo.wrCache[i] &= 0xFFFF0000;
                device->common.cacheInfo.wrCache[i] |= (uint32_t)(newMask << SPI_MASK_SIZE) | newVal;
            }
        }
    }

    if ((wrCacheStateLocal > ADRV9025_WR_HW_RMW_ON) && !addrFoundInCache)
    {
        /* if caching is on or merging is on
         * but addr was not found in the cache
         * add the wr instruction to the cache.
        */
        /* if the cache requirements will exceed the spi buffer flush it now. */
        if ((device->common.cacheInfo.spiWrBufferCnt + ADRV9025_HW_RMW_BYTES) >= ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            device->common.cacheInfo.wrFlushEn = ADI_ENABLE;
            recoveryAction                     = adrv9025_BfWriteCacheFlush(device);
            ADI_ERROR_RETURN(device->common.error.newAction);
            device->common.cacheInfo.wrFlushEn = wrFlushEnSaved;
        }

        device->common.cacheInfo.wrCache[device->common.cacheInfo.wrCacheIdx++] =
        ((uint32_t)addr << SPI_ADDR_SIZE) | ((uint32_t)mask << 8) | regVal;

        /* track the size of the spibuffer needed to write the cache.*/
        if (mask == 0xFF)
        {
            device->common.cacheInfo.spiWrBufferCnt += ADRV9025_SPI_BYTES;
        }
        else
        {
            device->common.cacheInfo.spiWrBufferCnt += ADRV9025_HW_RMW_BYTES;
        }
    }

    /* Immediate Read modify write using HW_RMW function */
    if (wrCacheStateLocal == ADRV9025_WR_HW_RMW_ON)
    {
        /* if HW-RMW is turned on write the data immediately using HW-RMW */
        device->common.cacheInfo.wrCache[0] = ((uint32_t)addr << SPI_ADDR_SIZE) | ((uint32_t)mask << 8) | regVal;

        recoveryAction = adi_adrv9025_SpiCacheWrite(device,
                                                    &device->common.cacheInfo.wrCache[0],
                                                    1);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (wrCacheStateLocal == ADRV9025_WR_CACHE_OFF)
    {
        if (device->common.cacheInfo.wrOnly == ADI_TRUE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             device->common.cacheInfo.wrOnly,
                             "Cannot write this field with manual RMW since wrOnly is true");

            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* Immediate Standard read Modify Write using separate read
         * and writespi transactions.
         */
        recoveryAction = adi_adrv9025_SpiByteRead(device,
                                                  addr,
                                                  &registerVal);
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* modify the register value */
        newRegVal = (registerVal & ~mask) | regVal;

        /* write to the register with modified value */
        recoveryAction = adi_adrv9025_SpiByteWrite(device,
                                                   addr,
                                                   newRegVal);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_BfFieldRead(adi_adrv9025_Device_t* device,
                             uint16_t               addr,
                             uint8_t*               fieldVal,
                             uint8_t                mask,
                             uint8_t                startBit)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common,
                        fieldVal);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (device->common.cacheInfo.rdCacheState > ADRV9025_RD_CACHE_OFF)
    {
        /* adds spi call to cache */
        if ((device->common.cacheInfo.spiRdBufferCnt + ADRV9025_SPI_BYTES) >= ADI_COMMON_HAL_SPIARRAYSIZE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEBF,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             NULL,
                             "Rx Cache size exceeded error");

            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx++] =
        ((uint32_t)addr << SPI_ADDR_SIZE) | ((uint32_t)mask << SPI_MASK_SIZE) | startBit;
        device->common.cacheInfo.spiRdBufferCnt += ADRV9025_SPI_BYTES;
    }
    else
    {
        /* Implements immediate spi transaction call only used when caching is turned off. */
        recoveryAction = adi_adrv9025_SpiFieldRead(device,
                                                   addr,
                                                   fieldVal,
                                                   mask,
                                                   startBit);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_SPI_FAIL,
                         recoveryAction,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         "Bitfield read failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_BfReadAssembleData(adi_adrv9025_Device_t* device,
                                    uint8_t*               rxBfData,
                                    uint8_t                numRxBfData,
                                    uint64_t*              rxData)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i              = 0;
    uint16_t numRxBitFields = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxBfData);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxData);

    if (device->common.cacheInfo.rdCacheState == ADRV9025_RD_BF_CACHE_ON)
    {
        recoveryAction = adrv9025_BfReadCacheFlush(device,
                                                   rxData,
                                                   &numRxBitFields);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_SPI_FAIL,
                         recoveryAction,
                         ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                         "Read cache flush failure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (device->common.cacheInfo.rdCacheState == ADRV9025_RD_CACHE_OFF)
    {
        /* Caching is not used so then assembly the bitFiled register values
         * into a bitField Value. The max bitField Value is 64bits long.
         * so rdData could be shifted 8 times to concatenate a 64 bit value.
         */
        for (i = 0; i < numRxBfData; i++)
        {
            /* shift rxData to the left to make room for the next lower byte
             * in the bitField
             */
            *rxData = (*rxData << 8) | (uint64_t)rxBfData[i];
        }
    }

    return recoveryAction;
}

int32_t adrv9025_BfReadCacheFlush(adi_adrv9025_Device_t* device,
                                  uint64_t*              rdData,
                                  uint16_t*              numRdBitFields)
{
    static const uint8_t SHIFT_BYTE                           = 8;
    static const uint8_t MAX_START_BIT                        = 7;
    int32_t              recoveryAction                       = ADI_COMMON_ACT_NO_ACTION;
    uint32_t             bfValue                              = 0;
    uint32_t             i                                    = 0;
    uint8_t              regVal                               = 0;
    uint8_t              mask                                 = 0;
    uint8_t              startBit                             = 0;
    uint8_t              endOfBitfield                        = 0;
    uint8_t              regData[ADI_COMMON_HAL_SPIARRAYSIZE] = {0};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    ADI_NULL_PTR_RETURN(&device->common,
                        rdData);

    ADI_NULL_PTR_RETURN(&device->common,
                        numRdBitFields);

    if (device->common.cacheInfo.wrOnly == ADI_TRUE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device->common.cacheInfo.wrOnly,
                         "Read Operations cannot be called when wrOnly is enabled");

        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    *numRdBitFields = 0;

    if (device->common.cacheInfo.spiRdBufferCnt > ADI_COMMON_HAL_SPIARRAYSIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_ADRV9025_ACT_ERR_FLUSH_CACHE,
                         NULL,
                         "Rd Cache index exceeded the spi cache Size ");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (device->common.cacheInfo.rdCacheIdx > 0)
    {
        recoveryAction = adi_adrv9025_SpiCacheRead(device,
                                                   device->common.cacheInfo.rdCache,
                                                   &regData[0],
                                                   device->common.cacheInfo.rdCacheIdx);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         recoveryAction,
                         NULL,
                         "Rd Cache failure ");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* depack the array of register values into an array of bitField values*/
        for (i = 0; i < device->common.cacheInfo.rdCacheIdx; i++)
        {
            /* for each of the register values in the cache (if any)
               combine the data into a bitfield value*/

            mask     = (uint8_t)(device->common.cacheInfo.rdCache[i] >> SHIFT_BYTE);
            startBit = (uint8_t)(device->common.cacheInfo.rdCache[i]);

            /* The last byte in the bit field is marked by setting the upper
             * nible of the startBit to F, this allows the lower nible to
             * handle the byte with values from 0 to 7.
             * This marker is added within the get bitfield function.
             */
            endOfBitfield = startBit > 0xC;

            if (endOfBitfield)
            {
                /*remove the endofbitField marker*/
                startBit = startBit & 0x3F;
            }

            regVal = regData[((3 * i) + 2)];

            if (startBit > MAX_START_BIT)
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_DEVICEBF,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_ADRV9025_ACT_ERR_FLUSH_CACHE,
                                 NULL,
                                 "startbit exceeded its max value of 7 ");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }

            bfValue = ((uint32_t)bfValue << SHIFT_BYTE) | ((regVal & mask) >> startBit);

            if (endOfBitfield)
            {
                rdData[(*numRdBitFields)++] = bfValue;
                bfValue                     = 0;
            }

            device->common.cacheInfo.rdCache[device->common.cacheInfo.rdCacheIdx] = '\0';
        }

        device->common.cacheInfo.rdCacheIdx     = 0;
        device->common.cacheInfo.spiRdBufferCnt = 0;
    }

    return recoveryAction;
}

int32_t adrv9025_BfWriteCacheFlush(adi_adrv9025_Device_t* device)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i              = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_BF);

    if (device->common.cacheInfo.spiWrBufferCnt > ADI_COMMON_HAL_SPIARRAYSIZE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_ADRV9025_ACT_ERR_FLUSH_CACHE,
                         NULL,
                         "Wr Cache index exceeded the spi cache Size ");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    /* only call flush if there is something in the cache */
    if ((device->common.cacheInfo.wrFlushEn == ADI_ENABLE) && (device->common.cacheInfo.wrCacheIdx > 0))
    {
        recoveryAction = adi_adrv9025_SpiCacheWrite(device,
                                                    device->common.cacheInfo.wrCache,
                                                    device->common.cacheInfo.wrCacheIdx);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_DEVICEBF,
                         ADI_COMMON_ERR_SPI_FAIL,
                         recoveryAction,
                         NULL,
                         "Wr Cache failure ");
        ADI_ERROR_RETURN(device->common.error.newAction);

        for (i = 0; i < device->common.cacheInfo.wrCacheIdx; i++)
        {
            device->common.cacheInfo.wrCache[i] = '\0';
        }
        device->common.cacheInfo.wrCacheIdx     = 0;
        device->common.cacheInfo.spiWrBufferCnt = 0;
    }

    device->common.cacheInfo.wrFlushEn = ADI_ENABLE;

    return recoveryAction;
}
