/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_hal.c
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*        
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_error.h"

#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_platform_byte_order.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_HAL

/*
 * Private Helpers
 *
 **/

/* Used to re-write single byte AHB register read/writes to equivalent
 * but faster SPI register read/writes. */
static const uint32_t SPI_ONLY_REGS_ADDR = 0x47000000U;
static const uint32_t DIRECT_SPI_REGION_LEN = 0x4000U;

/**
* \brief Helper function to check the address passed into the Registers32bOnlyRead and Registers32bOnlyWrite functions
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device  Context variable - Pointer to the ADRV904X device data structure.
* \param[in] addr        The 32bit address that is checked to see if it lies in a region of the register map that requires
*                        a 32-bit spi transaction to be performed.
* \param[in] count       The number of bytes to read or write. Count should be greater than 0 and a multiple of 4. Furthermore,
*                        for semaphore registers the count needs to be exactly 4.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
static adi_adrv904x_ErrAction_e adrv904x_ThirtyTwoBitAhbAccessRangeCheck(adi_adrv904x_Device_t* const device, 
                                                                         const uint32_t               addr,
                                                                         const uint32_t               count);

/**
* \brief Encodes a write to a 'Direct SPI' device register. Appends encoded data to a caller
* supplied buffer and may flush that buffer using SpiFlush().
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[out] wrData caller supplied buffer to hold the encoded data byte array. Must be large enough to 
*      hold the required data.
* \param[out] numWrBytes is set to the number of valid bytes in wrData after the call. The caller
*      must monitor this value and manually call SpiFlush if a call to this function would overrun the
*      buffer. If the buffer is flushed during the call numWrBytes will be set to 0 (unless the call appends
*      further bytes after the flush).
* \param[in] addr the device register to write to. Must be 'Direct SPI' register i.e. less that DIRECT_SPI_REGION_LEN.
* \param[in] mask Only the register bits where the corresponding bit in mask are set to 1 will be written to.
*     The other register bits at addr will be left unchanged.
* \param[in] data the data to write.
* \param[in] writeFlag
*
* If the device is in streaming or FIFO mode it's possible that only the data byte needs to be appended to wrData.
* numWrBytes will only increment by one in this case.
*
* If mask is anything except 0xFF, hardware masking will be utilized automatically. This requires additional SPI
* writes to the masking register. These additional register writes are added through recursive calls to DirectSpiDataPack.
* It will also result in existing wrData contents being flushed.
*/
static adi_adrv904x_ErrAction_e adrv904x_DirectSpiDataPack(adi_adrv904x_Device_t* const    device,
                                                           uint8_t* const                  wrData,
                                                           uint32_t* const                 numWrBytes,
                                                           const uint16_t                  addr,
                                                           const uint8_t                   mask,
                                                           const uint8_t                   data,
                                                           const uint8_t                   writeFlag)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t initialDataByte = (*numWrBytes == 0) ? 1U : 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, wrData);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, numWrBytes);

    if (mask == 0xFFU)
    {
        /* mask = 0xFF => No write to device masking register required */

        /* Three conditions need to be met to add only the data & not the addr to the byte array: 
                 * - device is in streaming mode, AND
                 * - This is not the first data byte going across SPI, AND
                 * - The current address is the next sequentially from the previous address
                 * ---or---
                 * - device is in SPI FIFO mode, AND
                 * - This is not the first data byte going across SPI, AND
                 * - the address is equal to the previous address
                 * */
        if (((device->devStateInfo.spiStreamingOn   == 1U) &&
             (initialDataByte                       != 1U) &&
             (addr                                  == device->devStateInfo.previousSpiStreamingAddress + 1U)) ||
            ((device->devStateInfo.spiFifoModeOn    == 1U) &&
             (initialDataByte                       != 1U) &&
             (addr                                  == device->devStateInfo.previousSpiStreamingAddress)))
        {
            /* No need to write address. Just data required. */
            wrData[(*numWrBytes)++] = (uint8_t)data;
            recoveryAction          = ADI_ADRV904X_ERR_ACT_NONE;
        }
        else
        {
            /* Need to write an address to wrData */
            if ((device->devStateInfo.spiStreamingOn == 1U) && (initialDataByte != 1U))
            {
                /* Streaming mode and buffer has an existing write it it. Flush out and start new write */
                if (writeFlag == ADRV904X_SPI_WRITE_POLARITY)
                {
                    recoveryAction = adi_adrv904x_SpiFlush( device, wrData, numWrBytes);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
                        return recoveryAction;
                    }
                }
                else
                {
                    /* Because we have to flush and don't have a way to send the read data back to the user */
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                            recoveryAction,
                                            writeFlag,
                                            "Non consecutive reads cannot be packed together in streaming mode");
                    return recoveryAction;
                }
            }

            /*  Address Format:
             *
             *  |                        Byte 0                           |   Byte 1   |
             *  | Bit       7                       6           5 - 0     |   7 - 0    |
             *  +--------------------------+-----------------+------------+------------+
             *  |       read/write         |   Addr[14]      | Addr[13:8] | Addr[7:0]  |
             *  | SPI_WRITE_POLARITY read  | 0 direct region |            |            |
             *  |!SPI_WRITE_POLARITY write | 1 paged  region |            |            |
             *  +--------------------------+-----------------+------------+------------+
             */
            
            wrData[(*numWrBytes)++] = (uint8_t)(((writeFlag & 0x01U) << 7U) | ((addr >> 8U) & 0x7FU));
            wrData[(*numWrBytes)++] = (uint8_t)(addr);
            /* coverity will trigger an error which is OK in this case. */
            wrData[(*numWrBytes)++] = (uint8_t)data;
            recoveryAction          = ADI_ADRV904X_ERR_ACT_NONE;
        }
    }
    else
    {
        /* mask != 0xFF requires a mask value to be written to device masking register */
        
        if (device->devStateInfo.spiStreamingOn == 0U)
        {
            recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                            wrData,
                                                            numWrBytes,
                                                            ADRV904X_ADDR_SPI0_MASK_7TO0,
                                                            0xFFU,
                                                            mask,
                                                            writeFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                return recoveryAction;
            }
            recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                            wrData,
                                                            numWrBytes,
                                                            addr,
                                                            0xFFU,
                                                            data,
                                                            writeFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                return recoveryAction;
            }
        }
        else
        {
            /* Masked write in streaming mode. */ 

            /* Flush current wrData. */
            recoveryAction = adi_adrv904x_SpiFlush( device, wrData, numWrBytes);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
                return recoveryAction;
            }

            /* Flush out a write to the h/w mask register */
            recoveryAction = adrv904x_DirectSpiDataPack( device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_MASK_7TO0, 0xFF, mask, writeFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                return recoveryAction;
            }

            recoveryAction = adi_adrv904x_SpiFlush( device, wrData, numWrBytes);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
                return recoveryAction;
            }

            /* Flush out a write to the destination register */
            recoveryAction = adrv904x_DirectSpiDataPack( device, wrData, numWrBytes, addr, 0xFFU, data, writeFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                return recoveryAction;
            }
            
            recoveryAction = adi_adrv904x_SpiFlush( device, wrData, numWrBytes);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
                return recoveryAction;
            }
        }
    }

    device->devStateInfo.previousSpiStreamingAddress = addr;

    return recoveryAction;
}

/**
* \brief Encodes a write to a AHB device register. Appends encoded data to a caller
* supplied buffer and may flush that buffer using SpiFlush().
*
* Writes to AHB registers are effected by writes to special paging registers to select the correct page (only
* if not alredy selected) and then Direct SPI write to the currently selected page.  Therefore this function does
* not write directly to wrData itself but delegates adrv904x_DirectSpiDataPack to do so.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[out] wrData caller supplied buffer to hold the encoded data byte array. Must be large enough to 
*      hold the required data.
* \param[out] numWrBytes is set to the number of valid of bytes in wrData after the call. The caller
*      must monitor this value and manually call SpiFlush if a call to this function would overrun the
*      buffer. If buffer flushed during the call numWrBytes will be set to 0 (unless the call appends
*      further bytes after the flush).
* \param[in] addr the device register to write to. Must be 'Direct SPI' register i.e. less that DIRECT_SPI_REGION_LEN.
* \param[in] mask Only the register bits where the corresponding bit in mask are set to 1 will be written to.
*     The other register bits at addr will be left unchanged.
* \param[in] data the data to write.
* \param[in] writeFlag The value to be bitwise OR'd into the MSB of the 16-bit address.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/
static adi_adrv904x_ErrAction_e adrv904x_PagingSpiDataPack(adi_adrv904x_Device_t* const    device,
                                                           uint8_t* const                  wrData,
                                                           uint32_t* const                 numWrBytes,
                                                           uint32_t                        addr,
                                                           uint32_t                        mask,
                                                           uint32_t                        data,
                                                           const uint8_t                   writeFlag)
{
        static const uint16_t SPI_MASK   = 0x3FFFU;
    uint16_t SPI_PAGING = (uint16_t) DIRECT_SPI_REGION_LEN;
    uint32_t dataOffset = 0U;  /* Offset from pageAddr to the highest byte address within pageMask that contains set bits */

    /* SPI Paging variables: */
    uint32_t byteShiftVal = 0; /* The offset from pageAddr to dest addres */
    uint32_t pageAddr = 0;     /* The 32bit-aligned address before dest address */
    uint64_t pageMask = 0;     /* Equivalent to mask param but to be applied to pageAddr */
    uint64_t pageData = 0;     /* Equivalent to data param but to be applied to pageAddr */
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, wrData);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, numWrBytes);

    /* Calculate paging variables.
     *
     * For example when addr is one up from a 32b aligned address:
     *
     *         <----------------------------- pageMask (64b) ------------------------------------------>
     * Vars    <----------------------------- pageData (64b) ------------------------------------------>
     *                    <-------------------- mask (32b) ------------------>
     *                    <-------------------- data (32b) ------------------>
     * Device Reg.
     * Addr       b...00       b...01        b...10      b...11     b...00       b...01        b...10      b...11
     *         |  32b                                            |    32b                                            |
     *         | aligned                                         |  aligned                                          |
     *         +----------+------------+------------+------------+------------+------------+------------+------------+
     * Vars    | pageAddr |   addr     |            |            |            |            |            |            |
     *         |----------+------------+------------+------------|------------+------------+------------+------------+
     *
     **/

    byteShiftVal = addr - (addr & ~0x3U);
    pageAddr = (addr & ~0x3U);
    pageMask = ((uint64_t)mask) << (8U * byteShiftVal) ;
    pageData = ((uint64_t)(data)) << (8U * byteShiftVal);
    
    if ((pageMask & 0x00FF000000000000ULL) > 0U)
    {
        dataOffset = 6U;
    }
    else if ((pageMask & 0x0000FF0000000000ULL) > 0U)
    {
        dataOffset = 5U;
    }
    else if ((pageMask & 0x000000FF00000000ULL) > 0U)
    {
        dataOffset = 4U;
    }
    else if ((pageMask & 0xFF000000ULL) > 0U)
    {
        dataOffset = 3U;
    }
    else if ((pageMask & 0x00FF0000ULL) > 0U)
    {
        dataOffset = 2U;
    }
    else if ((pageMask & 0x0000FF00ULL) > 0U)
    {
        dataOffset = 1U;
    }
    else if ((pageMask & 0x000000FFULL) > 0U)
    {
        dataOffset = 0U;
    }
    else
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing invalid mask");
        return recoveryAction;
    }
    
    /* SPI InDirect address. Write paging registers */
    /* Update page addr is less than current page starting address OR greater than page size (4k) from starting address */
    if ((pageAddr < device->devStateInfo.currentPageStartingAddress) || ((pageAddr + dataOffset) > (device->devStateInfo.currentPageStartingAddress + SPI_MASK)))
    {
        /* The write affects bits outside of the page currently selected by the device page registers. Update them. */
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_PAGE_31TO24, 0xFFU, (uint8_t)(pageAddr >> 24U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_PAGE_23TO16, 0xFFU, (uint8_t)(pageAddr >> 16U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_PAGE_15TO8, 0xFFU, (uint8_t)(pageAddr >> 8U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_PAGE_7TO0, 0xFFU, (uint8_t)(pageAddr >> 0U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, ADRV904X_ADDR_SPI0_PAGING_CONTROL, 0xFFU, (uint8_t)(0U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }

        device->devStateInfo.currentPageStartingAddress = pageAddr;
    }

    /* Update addr from AHB address space to offset into currently selected page */
    addr = pageAddr - device->devStateInfo.currentPageStartingAddress;

    /* Depending on the mask bits set and the offset of addr from next lower 32bit aligned
     * addr at most most four of a possible seven writes releative to the 32bit alinged addr
     * will be generated. */
    if ((pageMask & 0x000000FFULL) > 0U)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 0U)), (uint8_t)(pageMask >> 0U), (uint8_t)(pageData >> 0U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0x0000FF00ULL) > 0U)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 1U)), (uint8_t)(pageMask >> 8U), (uint8_t)(pageData >> 8U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0x00FF0000ULL) > 0U)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 2U)), (uint8_t)(pageMask >> 16U), (uint8_t)(pageData >> 16U), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0xFF000000ULL) > 0U)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 3)), (uint8_t)(pageMask >> 24), (uint8_t)(pageData >> 24), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0x000000FF00000000ULL) > 0U)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 4)), (uint8_t)(pageMask >> 32), (uint8_t)(pageData >> 32), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0x0000FF0000000000ULL) > 0ULL)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 5)), (uint8_t)(pageMask >> 40), (uint8_t)(pageData >> 40), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }
    if ((pageMask & 0x00FF000000000000ULL) > 0ULL)
    {
        recoveryAction = adrv904x_DirectSpiDataPack(device, wrData, numWrBytes, (uint16_t)(SPI_PAGING | ((addr & SPI_MASK) + 6)), (uint8_t)(pageMask >> 48), (uint8_t)(pageData >> 48), writeFlag);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }
    }

    return recoveryAction;
}


/**
* \brief Writes out the data array of size count and returns parsed return data. Must be packed according ADIHAL layer specs.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device     Pointer to the ADRV904X device data structure.
* \param txData     byte array containing the data to write to SPI. Must be packed correctly.
* \param byteCount  pointer to the number of bytes in data parameter.
* \param rxData     empty array that will hold the raw SPI data. Must be at least as large as txData array
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
static adi_adrv904x_ErrAction_e adrv904x_SpiFlushWithRead(adi_adrv904x_Device_t* const    device,
                                                          const uint8_t                   txData[],
                                                          const uint16_t                  byteCount,
                                                          uint8_t                         rxData[])
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, txData);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, rxData);
    
    halError = adi_hal_SpiRead(device->common.devHalInfo, &txData[0U], rxData, byteCount);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_DEVICEHAL,
                            ADI_ADRV904X_ERRCODE_HAL_SPI_READ,
                            recoveryAction,
                            halError,
                            "SPI Read Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
* \brief Extracts the register data from the raw data received on a SPI transaction to a byte array. 
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device             Pointer to the ADRV904X device data structure.
* \param rxData             array of raw data bytes from a recent SPI read, e.g from calling adrv904x_SpiFlushWithRead
* \param rxDataCount        the number of bytes included in rxData
* \param parsedData         blank array that will hold the parsed data from rxData
* \param parsedDataCount    The number of bytes to extract from rxData
* \param parsedDataStride   Nth byte extracted is written to parsedData[parsedDataStride * n]. 
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
static adi_adrv904x_ErrAction_e adrv904x_SpiDataParse(adi_adrv904x_Device_t* const    device,
                                                      const uint8_t                   rxData[],
                                                      const uint16_t                  rxDataCount,
                                                      uint8_t                         parsedData[],
                                                      const uint32_t                  parsedDataCount,
                                                      const uint8_t                   parsedDataStride)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t                    i               = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if (device->devStateInfo.spiStreamingOn == 0U)
    {
        /* Two address bytes + 1 data byte for each parsedDataCount --> must also be a multiple of three */
        if (((rxDataCount % 3U) != 0U) || (parsedDataCount != (rxDataCount / 3U)))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    parsedDataCount,
                                    "Number of bytes read does not match requested number of registers");
            return recoveryAction;
        }

        for (i = 0U; i < parsedDataCount; ++i)
        {
            /* 
             * stride = 3 bytes (if not streaming stride = word size * 3)
             * offset = 2 for initial 2-byte address
             */
            parsedData[i * parsedDataStride] = rxData[(i * 3U) + 2U];
        }
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        /* byteCount should include the two extra bytes for starting address */
        if (parsedDataCount != ((uint32_t)rxDataCount - 2U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    parsedDataCount,
                                    "Number of bytes read does not match requested number of registers");
            return recoveryAction;
        }

        for (i = 0U; i < parsedDataCount; ++i)
        {
            /* 
             * stride = 1 byte (if streaming stride = word size)
             * offset = 2 for initial 2-byte address
             */
            parsedData[i * parsedDataStride] = rxData[i + 2U];
        }
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }

    return recoveryAction;
}

/*
 * \brief Writes the AHB address to the AHB interface SPI registers and performs SpiFlush
 **/
static adi_adrv904x_ErrAction_e adrv904x_DmaAddrSet(adi_adrv904x_Device_t* const    device,
                                                    const uint32_t                  address)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t localSpiCache[12U] = { 0U }; /* 12 bytes is max needed to set address when non-streaming mode */
    uint32_t localCacheCount = 0U;
    uint8_t spiReg = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = adrv904x_DirectSpiDataPack(device, &localSpiCache[0U], &localCacheCount, ADRV904X_ADDR_SPIDMA0_ADDR3, 0xFFU, (uint8_t)(address >> 24U), ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "DMA Address Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIDMA0_ADDR3,
                        (uint8_t)(address >> 24U),
                        0xFFU);

    recoveryAction = adrv904x_DirectSpiDataPack(device, &localSpiCache[0U], &localCacheCount, ADRV904X_ADDR_SPIDMA0_ADDR2, 0xFFU, (uint8_t)(address >> 16U), ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "DMA Address Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIDMA0_ADDR2,
                        (uint8_t) (address >> 16U),
                        0xFFU);

    recoveryAction = adrv904x_DirectSpiDataPack(device, &localSpiCache[0U], &localCacheCount, ADRV904X_ADDR_SPIDMA0_ADDR1, 0xFFU, (uint8_t)(address >> 8U), ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "DMA Address Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIDMA0_ADDR1,
                        (uint8_t) (address >> 8U),
                        0xFFU);

    recoveryAction = adrv904x_DirectSpiDataPack(device, &localSpiCache[0U], &localCacheCount, ADRV904X_ADDR_SPIDMA0_ADDR0, 0xFFU, (uint8_t)address, ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "DMA Address Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIDMA0_ADDR0,
                        (uint8_t) address,
                        0xFFU);

    recoveryAction = adi_adrv904x_SpiFlush(device, localSpiCache, &localCacheCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
        return recoveryAction;
    }

    if ((device->devStateInfo.autoIncrModeOn == ADI_TRUE) &&
        (device->devStateInfo.spiStreamingOn == ADI_TRUE) &&
        (device->devStateInfo.spiOptions.allowAhbSpiFifoMode == ADI_TRUE))
    {
        /* 0x1U = enable FIFO for byte mode (hard coded to byte mode) */
        spiReg = ADI_TRUE;
        device->devStateInfo.spiFifoModeOn = ADI_TRUE;

        recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                    &localSpiCache[0U],
                                                    &localCacheCount,
                                                    ADRV904X_ADDR_SPIFIFO_MODE,
                                                    0xFFU,
                                                    spiReg,
                                                    ADRV904X_SPI_WRITE_POLARITY);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
            return recoveryAction;
        }

        ADI_VARIABLE_LOG(   &device->common,
                            ADI_HAL_LOG_SPI,
                            "DMA Address Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                            ADRV904X_ADDR_SPIFIFO_MODE,
                            spiReg,
                            0xFFU);

        recoveryAction = adi_adrv904x_SpiFlush(device, localSpiCache, &localCacheCount);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
            return recoveryAction;
        }
    }

    return recoveryAction;
}

/*
 * \brief Writes the AHB configuration (from device structure)
 *        to the AHB interface SPI registers and performs SpiFlush.
 *        Sets the file-scoped global variables for SPI FIFO and Auto-Increment.
 **/
static adi_adrv904x_ErrAction_e adrv904x_IndirectReadSetup(adi_adrv904x_Device_t* const device,
                                                           uint8_t                      enAhbAutoIncrement)
{
    uint8_t localSpiCache[6U] = { 0U }; /* 6 bytes is max needed to configure AHB interface */
    uint32_t localCacheCount = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    /* Bit shifts in Ctrl register */
    static const uint8_t SPIDMA0_AUTO_INCR = 1U;
    static const uint8_t SPIDMA0_RD_WRB = 7U;
    uint8_t spiReg = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Setup ctrl word for AHB read */
    spiReg |= (1U << SPIDMA0_RD_WRB);
    /* Hard coded use of byte mode for Direct & Indirect access */
    /* spiReg |= (device->devStateInfo.ahbBusSize & 0x3U) << SPIDMA0_BUS_SIZE; */
    if (enAhbAutoIncrement == 1U)
    {
        spiReg |= (1U << SPIDMA0_AUTO_INCR);
        device->devStateInfo.autoIncrModeOn = ADI_ENABLE;
    }
    else
    {
        device->devStateInfo.autoIncrModeOn = ADI_DISABLE;
    }

    recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                &localSpiCache[0U],
                                                &localCacheCount,
                                                ADRV904X_ADDR_SPIDMA0_CTL,
                                                0xFFU,
                                                spiReg,
                                                ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    recoveryAction = adi_adrv904x_SpiFlush(device, localSpiCache, &localCacheCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/*
 * \brief Clears the AHB configuration and performs SpiFlush.
 *        Clears the file-scoped global variables for SPI FIFO and Auto-Increment.
 *
 *  NOTE: Public API's Clear Error Structure On Entry
 *          - Clean Up Functions Cannot Call Public API's
 *          - API could have already logged error information to Error Structure
 */
static adi_adrv904x_ErrAction_e adrv904x_IndirectReadTeardown(adi_adrv904x_Device_t* const device)
{
    uint8_t localSpiCache[6U] = { 0U }; /* 6 bytes is max needed to configure AHB interface */
    uint32_t localCacheCount = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t spiReg = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                &localSpiCache[0U],
                                                &localCacheCount,
                                                ADRV904X_ADDR_SPIDMA0_CTL,
                                                0xFFU,
                                                spiReg,
                                                ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "Direct Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIDMA0_CTL,
                        spiReg,
                        0xFFU);

    device->devStateInfo.spiFifoModeOn = ADI_FALSE;
    device->devStateInfo.autoIncrModeOn = ADI_FALSE;

    recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                &localSpiCache[0U],
                                                &localCacheCount,
                                                ADRV904X_ADDR_SPIFIFO_MODE,
                                                0xFFU,
                                                spiReg,
                                                ADRV904X_SPI_WRITE_POLARITY);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
        return recoveryAction;
    }

    ADI_VARIABLE_LOG(   &device->common,
                        ADI_HAL_LOG_SPI,
                        "Direct Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                        ADRV904X_ADDR_SPIFIFO_MODE,
                        spiReg,
                        0xFFU);

    recoveryAction = adi_adrv904x_SpiFlush(device, localSpiCache, &localCacheCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tear down SPI Flushing Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

/**
* \brief Transmits txData on SPI, parses received data and extracts payload.
* 
* Calls adrv904x_SpiFlushWithRead followed by adrv904x_SpiDataParse.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device             Pointer to the ADRV904X device data structure.
* \param txData             bytes to transmit on SPI
* \param byteCount          number of bytes in txData
* \param finalDataOut       Register data parsed from SPI rx data is written here
* \param finalDataOutCount  Number of bytes expected to be in rxData
* \param finalDataOutStride Nth byte extracted from rxData is written to parsedData[parsedDataStride * n]. 
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*/    
static adi_adrv904x_ErrAction_e adrv904x_FlushParse(adi_adrv904x_Device_t* const    device,
                                                    const uint8_t                   txData[],
                                                    const uint16_t                  byteCount,
                                                    uint8_t                         finalDataOut[],
                                                    const uint32_t                  finalDataOutCount,
                                                    const uint8_t                   finalDataOutStride)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t rxData[ADI_HAL_SPI_FIFO_SIZE] = { 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    
    /* Final flush before exiting the function. */
    recoveryAction = adrv904x_SpiFlushWithRead(device, txData, byteCount, rxData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush with Read Issue");
        return recoveryAction;
    }
    
    recoveryAction = adrv904x_SpiDataParse(device, rxData, byteCount, finalDataOut, finalDataOutCount, finalDataOutStride);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Data Parsing Issue");
        return recoveryAction;
    }
    
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adrv904x_Registers32bOnlyRead_vEndian(adi_adrv904x_Device_t* const   device,
                                                                       adi_adrv904x_SpiCache_t* const  spiCache,
                                                                       const uint32_t                  addr,
                                                                       uint8_t                         readData[],
                                                                       const uint32_t                  numBytesToRead,
                                                                       uint8_t                         isByteData)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U; /* loop index */
    uint32_t readDataIndex = 0U;
    uint8_t spiDmaCtl[1] = { 0x88U };
    uint8_t lclByte[1] = { 0U };
    const uint8_t AUTOINCMASK = 0x2U;
    uint8_t devSpiOptionsAllowAutoIncr = 0U;
    uint8_t initialAutoIncrHwState = 0U;
    uint32_t i32 = 0U;
    uint8_t* i32Bytes = (uint8_t*) &i32; /* Hold the big endian version of AHB DMA Read back data [32:0] */
    uint32_t localAddr = addr; /* In case auto-increment is disabled we'll use this to keep track of the current address to read from */

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, readData);

    if (spiCache != NULL)
    {
        recoveryAction = adi_adrv904x_SpiFlush(device, spiCache->data, &spiCache->count);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue After CPU Mailbox Buffer Write");
            goto cleanup;
        }
    }

    /* Check that the range of addresses meets the criteria for 32bOnly read.*/
    recoveryAction = adrv904x_ThirtyTwoBitAhbAccessRangeCheck(device, addr, numBytesToRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Range check failed. Address does not require a 32-bit SPI transaction.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_SPIDMA0_CTL, lclByte, NULL, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the spi dma control register.");
        goto cleanup;
    }

    /* Is the auto-incr bit set? */
    if ((lclByte[0] & AUTOINCMASK) == AUTOINCMASK)
    {
        initialAutoIncrHwState = 1U;
    }

    /* read/wrb | sys_codeb | bus_waiting | bus_response | bus_size[1] | bus_size[0] | auto_incr | bus_sel */
    if (device->devStateInfo.spiOptions.allowAhbAutoIncrement)
    {
        spiDmaCtl[0] = lclByte[0] | (uint8_t) 0x8AU; /* Read, word-mode, auto-increment enabled */
        devSpiOptionsAllowAutoIncr = 1U;
    }
    else
    {
        spiDmaCtl[0] = lclByte[0] | (uint8_t) 0x88U; /* Read, word-mode, auto-increment disabled */
    }

    /* Write the DMA control register */
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_SPIDMA0_CTL, spiDmaCtl, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma control register.");
        goto cleanup;
    }
    
    /* Write the bus address */
    recoveryAction = adrv904x_DmaAddrSet(device, localAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma address registers.");
        goto cleanup;
    }

    /* Foreach 4-bytes to read except the last one (in order to avoid side-effects of DMA's read-ahead). */
    for (i = 0U; i < numBytesToRead / 4U - 1U; ++i)
    {
        /* Read the data */
        recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_SPIDMA0_DATA3, &i32Bytes[0], NULL, 4U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the spi dma data registers.");
            goto cleanup;
        }

        /* The 32b-only regions are read via the DMA registers which are BE. So i32Bytes is BE at this point. */
        if (isByteData != 0)
        {
            /* Data in i32 byte-oriented. Convert to device endianess so that byte order here in the host is the same as
             * that on the device. */
            i32 = ADRV904X_NTOCL(i32);
        }
        else
        {
            /* Data in i32 BE 32bit integer. Convert to host endianess. */
            i32 = ADRV904X_NTOHL(i32);                        
        }
        
        readData[readDataIndex]     = i32Bytes[0];
        readData[readDataIndex + 1] = i32Bytes[1];
        readData[readDataIndex + 2] = i32Bytes[2];
        readData[readDataIndex + 3] = i32Bytes[3];

        readDataIndex += 4U;
        localAddr += 4U;
        
        if (devSpiOptionsAllowAutoIncr == ADI_FALSE)
        {
            /* Device's auto-increment feature is off; We must explicity write to the DMA dest addr register. */
            recoveryAction = adrv904x_DmaAddrSet(device, localAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma address registers.");
                goto cleanup;
            }
        }
    }
    
    /* Now do the last 4 bytes but without auto-increment enabled. 
     * Auto-incr will always read 4 bytes ahead of the last requested address which can have unintended effects
     * such as access violations or triggering clear-on-read. */

    if (devSpiOptionsAllowAutoIncr != 0U)
    {
        /* Auto-incr is set on in the device. Turn it off */
        spiDmaCtl[0U] = spiDmaCtl[0U] & (uint8_t)(~AUTOINCMASK);
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_SPIDMA0_CTL, spiDmaCtl, 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma control register.");
            goto cleanup;
        }
    }

    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_SPIDMA0_DATA3, &i32Bytes[0], NULL, 4U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the spi dma data registers.");
        goto cleanup;
    }

    /* The 32b-only regions are read via the DMA registers which are BE. So i32Bytes is BE at this point. */
    if (isByteData != 0)
    {
        /* Data in i32 byte-oriented. Convert to device endianess so that byte order here in the host is the same as
         * that on the device. */
        i32 = ADRV904X_NTOCL(i32);
    }
    else
    {
        /* Data in i32 BE 32bit integer. Convert to host endianess. */
        i32 = ADRV904X_NTOHL(i32);                        
    }
    
    readData[readDataIndex]     = i32Bytes[0];
    readData[readDataIndex + 1] = i32Bytes[1];
    readData[readDataIndex + 2] = i32Bytes[2];
    readData[readDataIndex + 3] = i32Bytes[3];
    
    if (initialAutoIncrHwState)
    {
        /* Auto-increment was on when we entered, turn it back on before we exit */
        spiDmaCtl[0] = spiDmaCtl[0U] | AUTOINCMASK;        
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_SPIDMA0_CTL, spiDmaCtl, 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma control register.");
            goto cleanup;
        }
    }

cleanup:
    return recoveryAction;
}
    

/*
 * Public Functions
 *
 **/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiFlush(adi_adrv904x_Device_t* const    device,
                                                       const uint8_t                   data[],
                                                       uint32_t* const                 count)
{
        adi_adrv904x_ErrAction_e    recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e               halError        = ADI_HAL_ERR_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, count, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, data, cleanup);

    if (*count == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

        halError = adi_hal_SpiWrite(device->common.devHalInfo, &data[0U], *count);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_DEVICEHAL,
                            ADI_ADRV904X_ERRCODE_HAL_SPI_WRITE,
                            recoveryAction,
                            halError,
                            "SPI Write Issue");
        goto cleanup;
    }

        *count = 0U;

cleanup :
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}

/**
 * Defines certain memory regions on the device that are:
 * - big endian
 * - must be accessed at a 32bit aligned address
 * - must be accessed with 32bit wide reads, writes.
 * 
 * Each array element defines a single memory region. Each region consists of N memory areas as per adi_adrv904x_RegisterMap_t.
 */
static const adi_adrv904x_RegisterMap_t adrv904x_ThirtyTwoBitRegisterMapArray[] = 
{         
    /* First mem area startAddr,                    first area endAddr,                                                                 num mem areas,                              offset between area start addrs */    
    { (uint32_t)ADRV904X_ORX_ANALOG_ADC_ADDR_BASE,  (uint32_t)(ADRV904X_ORX_ANALOG_ADC_ADDR_BASE + ADRV904X_ORX_ANALOG_ADC_REG_LEN),    (uint32_t)ADI_ADRV904X_MAX_ORX,             (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_ORX_RAM_ADDR_BASE,         (uint32_t)(ADRV904X_ORX_RAM_ADDR_BASE + ADRV904X_ORX_RAM_REG_LEN),                  (uint32_t)ADI_ADRV904X_MAX_ORX,             (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_RX_GAIN_TABLE_ADDR_BASE,   (uint32_t)(ADRV904X_RX_GAIN_TABLE_ADDR_BASE + ADRV904X_RX_GAIN_TABLE_REG_LEN),      (uint32_t)ADI_ADRV904X_MAX_RX_ONLY,         (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_DPD_RAM_ADDR_BASE,      (uint32_t)(ADRV904X_TX_DPD_RAM_ADDR_BASE + ADRV904X_TX_DPD_RAM_REG_LEN),            (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,      (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_ATTEN_TABLE_ADDR_BASE,  (uint32_t)(ADRV904X_TX_ATTEN_TABLE_ADDR_BASE + ADRV904X_TX_ATTEN_TABLE_REG_LEN),    (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,      (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_UART_ADDR_BASE,            (uint32_t)(ADRV904X_UART_ADDR_BASE + ADRV904X_UART_REG_LEN),                        1U,                                         0U },
    { (uint32_t)ADRV904X_SPI_MASTER_ADDR_BASE,      (uint32_t)(ADRV904X_SPI_MASTER_ADDR_BASE + ADRV904X_SPI_MASTER_REG_LEN),            1U,                                         0U },
    { (uint32_t)ADRV904X_INTR_TSMTR_ADDR_BASE,      (uint32_t)(ADRV904X_INTR_TSMTR_ADDR_BASE + ADRV904X_INTR_TSMTR_REG_LEN),            1U,                                         0U },
    { (uint32_t)ADRV904X_TELEMETRY_ADDR_BASE,       (uint32_t)(ADRV904X_TELEMETRY_ADDR_BASE + ADRV904X_TELEMETRY_REG_LEN),              1U,                                         0U },
    { (uint32_t)ADRV904X_SEMAPHORE_ADDR_BASE,       (uint32_t)(ADRV904X_SEMAPHORE_ADDR_BASE + ADRV904X_SEMAPHORE_ADDR_STRIDE),          32U,                                        (uint32_t)ADRV904X_SEMAPHORE_ADDR_STRIDE }
        ,
    { (uint32_t)ADRV904X_CORE_0_INTR_AGG_ADDR_BASE,      (uint32_t)(ADRV904X_CORE_0_INTR_AGG_ADDR_BASE + ADRV904X_CORE_0_INTR_AGG_REG_LEN),  1U,                                          0U },
    { (uint32_t)ADRV904X_CORE_1_INTR_AGG_ADDR_BASE,      (uint32_t)(ADRV904X_CORE_1_INTR_AGG_ADDR_BASE + ADRV904X_CORE_1_INTR_AGG_REG_LEN),  1U,                                          0U },
    { (uint32_t)ADRV904X_TX_DPD_ACT_LUT_ADDR_BASE,       (uint32_t)(ADRV904X_TX_DPD_ACT_LUT_ADDR_BASE + ADRV904X_TX_DPD_ACT_LUT_REG_LEN),    (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,       (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_ANA_LB_ADC_32_ADDR_BASE,     (uint32_t)(ADRV904X_TX_ANA_LB_ADC_32_ADDR_BASE + ADRV904X_TX_ANA_LB_ADC_32_REG_LEN),(uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,       (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_SEMAPHORE_ADDR_BASE,         (uint32_t)(ADRV904X_TX_SEMAPHORE_ADDR_BASE + ADRV904X_TX_SEMAPHORE_REG_LEN),        (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,       (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_CFR_PULSE_RAM_ADDR_BASE,     (uint32_t)(ADRV904X_TX_CFR_PULSE_RAM_ADDR_BASE + ADRV904X_TX_CFR_PULSE_RAM_REG_LEN),(uint32_t)ADI_ADRV904X_MAX_TXCHANNELS,       (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_RX_SEMAPHORE_ADDR_BASE,         (uint32_t)(ADRV904X_RX_SEMAPHORE_ADDR_BASE + ADRV904X_RX_SEMAPHORE_REG_LEN),        (uint32_t)ADI_ADRV904X_MAX_RX_ONLY,          (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_ORX_SEMAPHORE_ADDR_BASE,        (uint32_t)(ADRV904X_ORX_SEMAPHORE_ADDR_BASE + ADRV904X_ORX_SEMAPHORE_REG_LEN),      (uint32_t)ADI_ADRV904X_MAX_ORX,              (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_RS_GLOBAL_CFG_1_ADDR_BASE,      (uint32_t)(ADRV904X_RS_GLOBAL_CFG_1_ADDR_BASE + ADRV904X_RS_GLOBAL_CFG_1_REG_LEN),  1U,                                          0U },
    { (uint32_t)ADRV904X_RS_CONFIG_0_ADDR_BASE,          (uint32_t)(ADRV904X_RS_CONFIG_0_ADDR_BASE + ADRV904X_RS_CONFIG_0_REG_LEN),          1U,                                          0U },
    { (uint32_t)ADRV904X_A55_INTR_AGG_ADDR_BASE,         (uint32_t)(ADRV904X_A55_INTR_AGG_ADDR_BASE + ADRV904X_A55_INTR_AGG_REG_LEN),        1U,                                          0U },
    { (uint32_t)ADRV904X_A55_INTR_TSMTR_ADDR_BASE,       (uint32_t)(ADRV904X_A55_INTR_TSMTR_ADDR_BASE + ADRV904X_A55_INTR_TSMTR_REG_LEN),    1U,                                          0U },             
    { (uint32_t)ADRV904X_A55_UART0_ADDR_BASE,            (uint32_t)(ADRV904X_A55_UART0_ADDR_BASE + ADRV904X_A55_UART0_REG_LEN),              1U,                                          0U },             
    { (uint32_t)ADRV904X_A55_UART1_ADDR_BASE,            (uint32_t)(ADRV904X_A55_UART1_ADDR_BASE + ADRV904X_A55_UART1_REG_LEN),              1U,                                          0U },   
    { (uint32_t)ADRV904X_A55_TRU_ADDR_BASE,              (uint32_t)(ADRV904X_A55_TRU_ADDR_BASE + ADRV904X_A55_TRU_REG_LEN),                  1U,                                          0U },  
    { (uint32_t)ADRV904X_A55_GIC_ADDR_BASE,              (uint32_t)(ADRV904X_A55_GIC_ADDR_BASE + ADRV904X_A55_GIC_REG_LEN),                  1U,                                          0U },            
    { (uint32_t)ADRV904X_A55_MDMA_ADDR_BASE,             (uint32_t)(ADRV904X_A55_MDMA_ADDR_BASE + ADRV904X_A55_MDMA_REG_LEN),                1U,                                          0U },
    { (uint32_t)ADRV904X_A55_L4CFG_ADDR_BASE,            (uint32_t)(ADRV904X_A55_L4CFG_ADDR_BASE + ADRV904X_A55_L4CFG_REG_LEN),              1U,                                          0U },             
    { (uint32_t)ADRV904X_A55_CAPBUFDDE_ADDR_BASE,        (uint32_t)(ADRV904X_A55_CAPBUFDDE_ADDR_BASE + ADRV904X_A55_CAPBUFDDE_REG_LEN),      1U,                                          0U },
    { (uint32_t)ADRV904X_A55_TSGEN_CTRL_ADDR_BASE,       (uint32_t)(ADRV904X_A55_TSGEN_CTRL_ADDR_BASE + ADRV904X_A55_TSGEN_CTRL_REG_LEN),    1U,                                          0U },    
    { (uint32_t)ADRV904X_A55_MMR_FABRIC_ADDR_BASE,       (uint32_t)(ADRV904X_A55_MMR_FABRIC_ADDR_BASE + ADRV904X_A55_MMR_FABRIC_REG_LEN),    1U,                                          0U },
    { (uint32_t)ADRV904X_A55_SYSTEM_CTI_ADDR_BASE,       (uint32_t)(ADRV904X_A55_SYSTEM_CTI_ADDR_BASE + ADRV904X_A55_SYSTEM_CTI_REG_LEN),    1U,                                          0U },             
    { (uint32_t)ADRV904X_A55_ATB_FUNNEL_ADDR_BASE,       (uint32_t)(ADRV904X_A55_ATB_FUNNEL_ADDR_BASE + ADRV904X_A55_ATB_FUNNEL_REG_LEN),    1U,                                          0U },
    { (uint32_t)ADRV904X_A55_TRACE_FIFO_ADDR_BASE,       (uint32_t)(ADRV904X_A55_TRACE_FIFO_ADDR_BASE + ADRV904X_A55_TRACE_FIFO_REG_LEN),    1U,                                          0U },    
    { (uint32_t)ADRV904X_A55_TPIU_ADDR_BASE,             (uint32_t)(ADRV904X_A55_TPIU_ADDR_BASE + ADRV904X_A55_TPIU_REG_LEN),                1U,                                          0U },
    { (uint32_t)ADRV904X_A55_PE_DBGVIEW_ADDR_BASE,       (uint32_t)(ADRV904X_A55_PE_DBGVIEW_ADDR_BASE + ADRV904X_A55_PE_DBGVIEW_REG_LEN),    1U,                                          0U },
    { (uint32_t)ADRV904X_A55_PE_SYSVIEW_ADDR_BASE,       (uint32_t)(ADRV904X_A55_PE_SYSVIEW_ADDR_BASE + ADRV904X_A55_PE_SYSVIEW_REG_LEN),    1U,                                          0U },    
    { (uint32_t)ADRV904X_A55_FABRIC_ADDR_BASE,           (uint32_t)(ADRV904X_A55_FABRIC_ADDR_BASE + ADRV904X_A55_FABRIC_REG_LEN),            1U,                                          0U },
    { (uint32_t)ADRV904X_TX_VSWR_PLAYBACK_RAM_ADDR_BASE, (uint32_t)(ADRV904X_TX_VSWR_PLAYBACK_RAM_ADDR_BASE + ADRV904X_TX_VSWR_PLAYBACK_RAM_REG_LEN), (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS, (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
    { (uint32_t)ADRV904X_TX_CAPTURE_RAM_ADDR_BASE,       (uint32_t)(ADRV904X_TX_CAPTURE_RAM_ADDR_BASE + ADRV904X_TX_CAPTURE_RAM_REG_LEN), (uint32_t)ADI_ADRV904X_MAX_TXCHANNELS, (uint32_t)ADRV904X_AHB_ADDR_STRIDE },
};

/*
 * Return ADI_TRUE if the entire adrv904x memory area starting at addr and of length numBytes lies within a 32bOnly region.
 * 
 * Does not check alignment of the memory area.
 */
static uint8_t adrv904x_Is32BitAccess(const uint32_t               addr,
                                      const uint32_t               numBytes)
{
    uint8_t sizeOfRegMapArray = (uint8_t)(sizeof(adrv904x_ThirtyTwoBitRegisterMapArray) / sizeof(adrv904x_ThirtyTwoBitRegisterMapArray[0]));
    uint8_t is32BitAccess = 0U;
    uint8_t  i = 0U;
    uint32_t j = 0U;

    for (i = 0U; i < sizeOfRegMapArray; i++)
    {
        for (j = 0U; j < adrv904x_ThirtyTwoBitRegisterMapArray[i].numOfInstances; j++)
        {
            if ((addr                   >= (adrv904x_ThirtyTwoBitRegisterMapArray[i].startAddr   + (adrv904x_ThirtyTwoBitRegisterMapArray[i].strideValue * j))) &&
                (addr                   <  (adrv904x_ThirtyTwoBitRegisterMapArray[i].endAddr     + (adrv904x_ThirtyTwoBitRegisterMapArray[i].strideValue * j))) &&
                ((addr + numBytes - 1U)    >= (adrv904x_ThirtyTwoBitRegisterMapArray[i].startAddr   + (adrv904x_ThirtyTwoBitRegisterMapArray[i].strideValue * j))) &&
                ((addr + numBytes - 1U)    <  (adrv904x_ThirtyTwoBitRegisterMapArray[i].endAddr     + (adrv904x_ThirtyTwoBitRegisterMapArray[i].strideValue * j))))
            {
                is32BitAccess = 1U;
                return is32BitAccess;
            }
        }
    }

    return is32BitAccess;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Register32Write(adi_adrv904x_Device_t* const    device,
                                                              adi_adrv904x_SpiCache_t* const  spiCache,
                                                              uint32_t                        addr,
                                                              const uint32_t                  writeData,
                                                              const uint32_t                  mask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    /* Re-write addresses so that writes to 0x4700'0000 - 0x4700'3FFF are done
     * via the faster direct spi protocol. Possible as that memory region is aliased to 0x0 - 0x3FFF. */
    if ((addr >= SPI_ONLY_REGS_ADDR) &&
        (addr < (SPI_ONLY_REGS_ADDR + DIRECT_SPI_REGION_LEN) && 
        mask <= 0xFFU))
    {
        addr -= SPI_ONLY_REGS_ADDR;
    }

    recoveryAction = adi_adrv904x_Registers32Write(device, spiCache, &addr, &writeData, &mask, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write Issue");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32Write(adi_adrv904x_Device_t* const    device,
                                                               adi_adrv904x_SpiCache_t* const  spiCache,
                                                               const uint32_t                  addr[],
                                                               const uint32_t                  writeData[],
                                                               const uint32_t                  mask[],
                                                               const uint32_t                  count)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t SPI_PAGING = (uint16_t) DIRECT_SPI_REGION_LEN;
    uint32_t i = 0U;
    uint32_t numWrBytes = 0U;
    uint8_t wrData[ADI_HAL_SPI_FIFO_SIZE] = { 0U };
    uint32_t *numWrBytesPtr = NULL;
    uint8_t *wrDataPtr = NULL;
    uint32_t writeData32 = 0U;
    uint32_t writeMask32 = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, addr, cleanup);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, writeData, cleanup);

    if (spiCache == NULL)
    {
        wrDataPtr = &wrData[0U];
        numWrBytesPtr = &numWrBytes;
    }
    else
    {
        wrDataPtr = spiCache->data;
        numWrBytesPtr = &spiCache->count;
    }

    if (mask != NULL)
    {
        /* This is a scattered (non-contiguous) write:
         *     - each element of addr[] param indicates dest addr for each corresponding element in writeData[] param
         *     - each element of mask[] param indicates write mask for each corresponding element in writeData[] param
         */
        for (i = 0U; i < count; ++i)
        {
            writeData32 = writeData[i];
            writeMask32 = mask[i];
            
            if (adrv904x_Is32BitAccess(addr[i], 1U) == ADI_TRUE)
            {
                /* Address is a 32bOnly area use adi_adrv904x_Registers32bOnlyWrite */                
                /* Manual flush so that we can call Registers32bOnlyWrite with a null spiCache */
                recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                    goto cleanup;
                }

                if (mask[i] != 0xFFFFFFFFU)
                {
                    /* Mask must be all Fs for 32bOnly regions */
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                            recoveryAction,
                                            mask[i],
                                            "Address is in 32bit register map, mask should be 0xFFFFFFFFU");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Registers32bOnlyWrite(device,
                                                                    NULL,
                                                                    addr[i],
                                                                    (uint8_t*) &writeData32,
                                                                    4U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32bOnlyWrite Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "32-Bit Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")",
                                    addr[i],
                                    writeData32);
            }
            else if (addr[i] < SPI_PAGING)
            {
                /* This is a scattered (non-contiguous) write to the Direct Area */
                if ((writeData32 & writeMask32) > 0xFFU)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                            recoveryAction,
                                            writeData,
                                            "Overflow data on a Direct SPI write");
                    goto cleanup;
                }
                if ((*numWrBytesPtr + ADRV904X_DIRECT_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
                {
                    recoveryAction = adi_adrv904x_SpiFlush( device, wrDataPtr, numWrBytesPtr);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                        goto cleanup;
                    }
                }

                recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                            wrDataPtr,
                                                            numWrBytesPtr,
                                                            addr[i],
                                                            writeMask32,
                                                            writeData32,
                                                            ADRV904X_SPI_WRITE_POLARITY);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "Direct Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                    addr[i],
                                    writeData32,
                                    writeMask32);
            }
            else
            {
                /* This is a scattered (non-contiguous) write to the AHB area. Uses paging. */
                if ((*numWrBytesPtr + ADRV904X_PAGING_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
                {
                    recoveryAction = adi_adrv904x_SpiFlush( device, wrDataPtr, numWrBytesPtr);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                        goto cleanup;
                    }
                }
                
                recoveryAction = adrv904x_PagingSpiDataPack(device,
                                                            wrDataPtr,
                                                            numWrBytesPtr,
                                                            addr[i],
                                                            writeMask32,
                                                            writeData32,
                                                            ADRV904X_SPI_WRITE_POLARITY);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Paging SPI Data Packing Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "Page Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                    addr[i],
                                    writeData32,
                                    writeMask32);
            }
        }
    }
    else
    {
        /* mask == NULL => this is a contiguous write:
         *     - addr[0] is taken as the dest address. mask[] param is ignored.
         *     - writeData[0] is written to addr[0], writeData[1] is written to next address.
         *       Next address is addr + 1 for addrs in SPI_PAGING area. addr + 4 for others.
         */
        
        uint32_t baseAddr = addr[0U];

        if (adrv904x_Is32BitAccess(baseAddr, 1U) == ADI_TRUE)
        {
            /* Address is a 32bOnly area use adi_adrv904x_Registers32bOnlyWrite */                            
            for (i = 0U; i < count; ++i)
            {
                writeData32 = writeData[i];
                /* Manual flush so that we can call Registers32bOnlyWrite with a null spiCache */
                recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                    goto cleanup;
                }

                recoveryAction = adi_adrv904x_Registers32bOnlyWrite(device,
                    NULL,
                    baseAddr,
                    (uint8_t*) &writeData32,
                    4U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32bOnlyWrite Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                    ADI_HAL_LOG_SPI,
                    "32-Bit Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")",
                    baseAddr,
                    writeData32);

                baseAddr = baseAddr + 4U;
            }
        }
        else if (baseAddr < SPI_PAGING)
        {
            /* Contiguous/non-masked direct-area write. */
            for (i = 0U; i < count; ++i)
            {
                writeData32 = writeData[i];
                
                if ((writeData32) > 0xFFU)
                {
                    recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                    ADI_PARAM_ERROR_REPORT( &device->common,
                                            recoveryAction,
                                            writeData,
                                            "Overflow data on a Direct SPI write");
                    goto cleanup;
                }

                if ((*numWrBytesPtr + ADRV904X_DIRECT_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
                {
                    recoveryAction = adi_adrv904x_SpiFlush( device, wrDataPtr, numWrBytesPtr);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                        goto cleanup;
                    }
                }

                recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                            wrDataPtr,
                                                            numWrBytesPtr,
                                                            baseAddr,
                                                            0xFFU,
                                                            writeData32,
                                                            ADRV904X_SPI_WRITE_POLARITY);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "Bulk Direct Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                    baseAddr,
                                    writeData32,
                                    0xFFU);

                baseAddr = baseAddr + 1U;
            }
        }
        else
        {
            /* Contiguous/non-masked AHB write. Uses paging area. */
            for (i = 0U; i < count; ++i)
            {
                writeData32 = writeData[i];
                                
                if ((*numWrBytesPtr + ADRV904X_PAGING_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
                {
                    recoveryAction = adi_adrv904x_SpiFlush( device, wrDataPtr, numWrBytesPtr);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                        goto cleanup;
                    }
                }
                
                recoveryAction = adrv904x_PagingSpiDataPack(device,
                                                            wrDataPtr,
                                                            numWrBytesPtr,
                                                            baseAddr,
                                                            0xFFFFFFFFU,
                                                            writeData32,
                                                            ADRV904X_SPI_WRITE_POLARITY);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Paging SPI Data Packing Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "Page Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                    baseAddr,
                                    writeData[i],
                                    0xFFFFFFFFU);

                baseAddr = baseAddr + 4U;
            }
        }
    }

    if (spiCache == NULL)
    {
        recoveryAction = adi_adrv904x_SpiFlush( device, wrDataPtr, numWrBytesPtr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Register32Read(adi_adrv904x_Device_t* const    device,
                                                             adi_adrv904x_SpiCache_t* const  spiCache,
                                                             uint32_t                        addr,
                                                             uint32_t* const                 readData,
                                                             const uint32_t                  mask)
{
        uint8_t readBytes[4] = { 0U, 0U, 0U, 0U };
    uint32_t count = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    /* Re-write addresses so that writes to 0x4700'0000 - 0x4700'3FFF are done
     * via the faster direct spi protocol. That memory region is aliased to 0x0 to 0x4000. */
    if((addr >= SPI_ONLY_REGS_ADDR) &&
        (addr < (SPI_ONLY_REGS_ADDR + DIRECT_SPI_REGION_LEN) && 
        mask <= 0xFFU))
    {
        addr -= SPI_ONLY_REGS_ADDR;
    }

    /* Lazy read with RegistersByteRead only the amount of bytes that the mask needs.
     * It avoids 0xFF54 errors on the firmware side when adi_adrv904x_Register32Read
     * is used in BfGet functions and simplifies SPI transactions */
    count = (mask <= 0xFFU) ? 1U : ((mask <= 0xFFFFU) ? 2U : ((mask <= 0xFFFFFFU) ? 3U : 4U));

    recoveryAction = adi_adrv904x_RegistersByteRead(device, spiCache, addr, readBytes, NULL, count);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RegistersByteRead issue");
        goto cleanup;
    }

    /* Device are LE; Convert host-endianness */
    *readData = ( ((uint32_t)readBytes[0]  << 0U)    |
                  ((uint32_t)readBytes[1]  << 8U)    |
                  ((uint32_t)readBytes[2]  << 16U)   |
                  ((uint32_t)readBytes[3]  << 24U));
    *readData &= mask;

cleanup :
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32Read(adi_adrv904x_Device_t* const    device,
                                                              adi_adrv904x_SpiCache_t* const  spiCache,
                                                              const uint32_t                  addr,
                                                              uint32_t                        readData[],
                                                              uint32_t                        mask[],
                                                              const uint32_t                  count)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e exitRecoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t SPI_PAGING = (uint16_t) DIRECT_SPI_REGION_LEN;

    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t localCacheCount = 0U; /* Number of total bytes packed into the byte array for SPI transactions */
    uint32_t currentRdCount = 0U; /* Number of registers that are to be read in the current byte array. Different than localCacheCount b/c it doesn't include address packing */
    uint32_t readDataIndex = 0U; /* The current index in readData where data should be placed after it is read. Since it's possible multiple SPI transactions can occur based on the number of bytes requested */
    uint8_t localCache[ADI_HAL_SPI_FIFO_SIZE] = { 0U };
    uint32_t currentAddress = addr; /* In the case when we need to flush in the middle of byte packing we need to keep track of where the next buffer will start from */
    uint8_t *readDataBytes; /* Used to cast readData array to a byte array */
    uint8_t parsedDataStride = 0U; /* Used when parsing data to determine where the next register value should be placed in readData */
    uint32_t lastByteBool = 0U;
    uint32_t disableAutoIncrement = 0U;
    const uint32_t numRegistersToRead = count;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    if (device->common.wrOnly == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_DEVICEHAL,
                            ADI_ADRV904X_ERRCODE_HAL_INVALID_DEVICE_STATE,
                            recoveryAction,
                            device->common.wrOnly,
                            "Device is currently in write-only mode");
        goto cleanup;
    }

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, readData, cleanup);

    readDataBytes = (uint8_t *)readData;

    if (spiCache != NULL)
    {
        /* spiCache is a parameter to this function as a convenience. It is only used to flush any previous writes that have been cached. */
        recoveryAction = adi_adrv904x_SpiFlush( device, spiCache->data, &spiCache->count);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
            goto cleanup;
        }
    }

    if (adrv904x_Is32BitAccess(addr, numRegistersToRead * 4) == ADI_TRUE)
    {
        /* Address is a 32bOnly area use adi_adrv904x_Registers32bOnlyRead */
        for (i = 0U; i < numRegistersToRead; ++i)
        {
            if ((mask != NULL) && (mask[i] != 0xFFFFFFFFU))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                        recoveryAction,
                                        mask,
                                        "Address is in 32bit register map, mask should be 0xFFFFFFFFU");
                goto cleanup;
            }

            recoveryAction = adi_adrv904x_Registers32bOnlyRead( device,
                                                                NULL,
                                                                addr + (4U * i),
                                                                (uint8_t*) &readData[i],
                                                                4U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32bOnlyRead Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "32-Bit Read( Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")",
                                (addr + (4U * i)),
                                readData[i]);
        }
        /* There is no need to fix up endianess like in the 'addr < SPI_PAGING' and 'non-32bOnly' scenarios
         * as adi_adrv904x_Registers32bOnlyRead has already done it. */
    }
    else if (addr < SPI_PAGING)
    {
        /* For Direct SPI we're processing bytes that need to be indexed into a uint32_t array */
        parsedDataStride = 4U; /* Each byte read from device is copied an element of uint32_t readData[] */

        for (i = 0U; i < numRegistersToRead; ++i)
        {
            /* For direct SPI reads we can handle any number of bytes and only initiate a SPI transaction when the
             * buffer is full or after we've packed all the reads together */
            if ((localCacheCount + ADRV904X_DIRECT_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adrv904x_FlushParse(   device,
                                                        localCache,
                                                        localCacheCount,
                                                        &readDataBytes[readDataIndex * parsedDataStride],
                                                        currentRdCount,
                                                        parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }
                localCacheCount = 0U;
                readDataIndex += currentRdCount;
                currentRdCount = 0U;
            }

            recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                        localCache,
                                                        &localCacheCount,
                                                        currentAddress,
                                                        0xFFU,
                                                        0x00U,
                                                        ~ADRV904X_SPI_WRITE_POLARITY);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Pack Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "Direct Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                currentAddress,
                                readDataBytes[readDataIndex * parsedDataStride],
                                0xFFU);

            /* Keeping track of how many registers in this localCache instance.
             * This will get cleared on a all to adrv_FlushParse.
             * */
            currentRdCount++;
                
            /* Keeping track of which AHB address we're looking at.
             * This will NOT get cleared on a all to adrv_FlushParse.
             * */
            currentAddress++;
        }
        
        recoveryAction = adrv904x_FlushParse(   device,
                                                localCache,
                                                localCacheCount,
                                                &readDataBytes[readDataIndex * parsedDataStride],
                                                currentRdCount,
                                                parsedDataStride);
        
        for (i = 0; i < numRegistersToRead; ++i)
        {
            readData[i] = ADRV904X_CTOHL(readData[i]);
        }
        
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
            goto cleanup;
        }
    }
    else
    {
        /* Src addr is AHB region. We are processing bytes and treating the readData array as a byte array */
        parsedDataStride = 1U;
        
        /* Pack the ctrl and first addr then flush to write immediately */
        /* Using the configCache to keep things separate */
        recoveryAction = adrv904x_IndirectReadSetup(device, device->devStateInfo.spiOptions.allowAhbAutoIncrement);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Ctrl Issue");
            goto cleanup;
        }
        
        /* No matter what we always need to set the address for the first read. */
        recoveryAction = adrv904x_DmaAddrSet(device, currentAddress);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Address Pack Issue");
            goto cleanup;
        }
        
        for (i = 0U; i < numRegistersToRead; ++i)
        {
            
            if ((localCacheCount + (ADRV904X_DIRECT_SPI_BYTES * 4U)) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adrv904x_FlushParse(   device,
                                                        localCache,
                                                        localCacheCount,
                                                        &readDataBytes[readDataIndex],
                                                        currentRdCount,
                                                        parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }
                localCacheCount = 0U;
                readDataIndex += currentRdCount;
                currentRdCount = 0U;
            }

            /* This loops reads each byte of the current 32-bit AHB register */
            for (j = 0U; j < 4U; j++)
            {
                lastByteBool = (i == (numRegistersToRead - 1)) && (j == 3);
                disableAutoIncrement = (i == (numRegistersToRead - 1)) && (j == 2);
                
                recoveryAction = adrv904x_DirectSpiDataPack(device, localCache, &localCacheCount, ADRV904X_ADDR_SPIDMA0_DATA0, 0xFF, 0x00, ~ADRV904X_SPI_WRITE_POLARITY);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                    goto cleanup;
                }

                ADI_VARIABLE_LOG(   &device->common,
                                    ADI_HAL_LOG_SPI,
                                    "Page Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                    currentAddress,
                                    readDataBytes[readDataIndex],
                                    0xFFU);

                /* Keeping track of how many registers in this localCache instance.
                 * This will get cleared on a all to adrv_FlushParse.
                 * */
                ++currentRdCount;

                /* Keeping track of which AHB address we're looking at.
                 * This will NOT get cleared on a all to adrv_FlushParse.
                 * */
                ++currentAddress;

                /* Cases where we need to flush prior to reading next word:
                 *  - SPI streaming WITHOUT Spi FIFO mode enabled
                 *    - because reading the same register over & over again will throw an error
                 *      in SPI streaming
                 *  - auto-increment is turned off
                 *    - because we need to set the address for the next byte
                 * */
                if (((device->devStateInfo.spiStreamingOn == 1U) && (device->devStateInfo.spiFifoModeOn == 0U)) ||
                    (device->devStateInfo.autoIncrModeOn == 0U))
                {
                    recoveryAction = adrv904x_FlushParse(   device,
                                                            localCache,
                                                            localCacheCount,
                                                            &readDataBytes[readDataIndex],
                                                            currentRdCount,
                                                            parsedDataStride);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                        goto cleanup;
                    }
                    localCacheCount = 0U;
                    readDataIndex += currentRdCount;
                    currentRdCount = 0U;
                }

                /* Set the address on the first byte or each time if auto-increment is off.
                 * This also ensures we will NOT set the address when SPI FIFO mode because we cannot
                 * be in SPI FIFO mode with auto increment off.
                 * */
                if ((lastByteBool == 0U) && (device->devStateInfo.autoIncrModeOn == 0U))
                {
                    recoveryAction = adrv904x_DmaAddrSet(device, currentAddress);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Address Pack Issue");
                        goto cleanup;
                    }
                }
                else if (disableAutoIncrement == 1U)
                {
                    /* auto-increment can cause invalid memory regions to be accessed.
                     * Because of that, on the final word to read turn off auto increment.
                     */
                    recoveryAction = adrv904x_FlushParse(   device,
                        localCache,
                        localCacheCount,
                        &readDataBytes[readDataIndex],
                        currentRdCount,
                        parsedDataStride);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                        goto cleanup;
                    }
                    localCacheCount = 0U;
                    readDataIndex += currentRdCount;
                    currentRdCount = 0U;
                    
                    /* Pack the ctrl and first addr then flush to write immediately */
                    /* Using the configCache to keep things separate */
                    recoveryAction = adrv904x_IndirectReadSetup(device, ADI_DISABLE);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Ctrl Issue");
                        goto cleanup;
                    }
                }
            } /* foreach byte of 32bit register */
        } /* foreach 32bit register to read */

        /* Final flush before exit in case there's still cached up reads. */
        if (localCacheCount != 0U)
        {
            recoveryAction = adrv904x_FlushParse(   device,
                                                    localCache,
                                                    localCacheCount,
                                                    &readDataBytes[readDataIndex],
                                                    currentRdCount,
                                                    parsedDataStride);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                goto cleanup;
            }
        }
        
        /* Convert endianess all AHB register reads from device to host byte-order */
        for (i = 0; i < numRegistersToRead; i++)
        {
            readData[i] = ADRV904X_CTOHL(readData[i]);            
        }
    }

    /* Clean up the data for the following cases:
     *  - If masking is required
     */
    if (mask != NULL)
    {
        for (i = 0U; i < numRegistersToRead; ++i)
        {
            readData[i] &= mask[i];
        }
    }

cleanup:
    if (addr >= SPI_PAGING)
    {
        /* Reset any indirect configurations if used. */
        exitRecoveryAction = adrv904x_IndirectReadTeardown(device);
        if (ADI_ADRV904X_ERR_ACT_NONE != exitRecoveryAction)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
        }
    }

    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RegistersByteWrite(adi_adrv904x_Device_t* const    device,
                                                                 adi_adrv904x_SpiCache_t* const  spiCache,
                                                                 const uint32_t                  addr,
                                                                 const uint8_t                   writeData[],
                                                                 const uint32_t                  count)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t SPI_PAGING = (uint16_t) DIRECT_SPI_REGION_LEN;
    uint32_t i = 0U;
    uint32_t numWrBytes = 0U;
    uint8_t wrData[ADI_HAL_SPI_FIFO_SIZE] = {0U};
    uint32_t *numWrBytesPtr = NULL;
    uint8_t *wrDataPtr = NULL;
    uint32_t baseAddr = addr;
    uint32_t wordWriteCount = 0U;   /* Word Write Count */
    uint32_t wordWriteIndex = 0U;   /* Word Write Index */

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, writeData, cleanup);

    if (spiCache == NULL)
    {
        wrDataPtr = &wrData[0U];
        numWrBytesPtr = &numWrBytes;
    }
    else
    {
        wrDataPtr = spiCache->data;
        numWrBytesPtr = &spiCache->count;
    }

    if (adrv904x_Is32BitAccess(addr, count) == ADI_TRUE)
    {
        /* Write to 32bOnly area */
        /* Calculate Number of 32 Bit Register Write Transactions Required */
        if ((count & 3U) != 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, count, "32-Bit Access Requires Count to be Multiples of 4");
            goto cleanup;
        }

        wordWriteCount = count >> 2U;
        wordWriteIndex = wordWriteCount - 1U;

        /* Manual flush so that we can call Registers32bOnlyWrite with a null spiCache */
        recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
            goto cleanup;
        }

        for (i = 0U; i < wordWriteCount; ++i)
        {
            recoveryAction = adi_adrv904x_Registers32bOnlyWrite(device,
                                                                NULL,
                                                                addr + (4U * wordWriteIndex),
                                                                &writeData[4U * wordWriteIndex],
                                                                4U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32bOnlyRead Issue");
                goto cleanup;
            }
            ++wordWriteIndex;
        }

        ADI_VARIABLE_LOG(   &device->common,
                            ADI_HAL_LOG_SPI,
                            "32-Bit Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")",
                            addr,
                            writeData);
    }
    else if (baseAddr < SPI_PAGING)
    {
        /* Write to direct area */
        for (i = 0U; i < count; ++i)
        {
            if ((*numWrBytesPtr + ADRV904X_DIRECT_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                    goto cleanup;
                }
            }

            recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                        wrDataPtr,
                                                        numWrBytesPtr,
                                                        baseAddr,
                                                        0xFFU,
                                                        writeData[i],
                                                        ADRV904X_SPI_WRITE_POLARITY);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "Direct Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                baseAddr,
                                writeData[i],
                                0xFFU);

            baseAddr = baseAddr + 1U;
        }
    }
    else
    {
        /* Write to AHB address. Uses paging. */        
        for (i = 0U; i < count; ++i)
        {
            if ((*numWrBytesPtr + ADRV904X_PAGING_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue");
                    goto cleanup;
                }
            }

            recoveryAction = adrv904x_PagingSpiDataPack(device,
                                                        wrDataPtr,
                                                        numWrBytesPtr,
                                                        baseAddr,
                                                        0xFFU,
                                                        (uint32_t) writeData[i],
                                                        ADRV904X_SPI_WRITE_POLARITY);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Paging SPI Data Packing Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "Page Write (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                baseAddr,
                                writeData[i],
                                0xFFU);

            baseAddr = baseAddr + 1U;
        }
    }

    if (adrv904x_Is32BitAccess(addr, count) != ADI_TRUE)
    {
        if (spiCache == NULL)
        {
            recoveryAction = adi_adrv904x_SpiFlush(device, wrDataPtr, numWrBytesPtr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RegistersByteRead(adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const uint32_t                  addr,
                                                                uint8_t                         readData[],
                                                                const uint8_t                   mask[],
                                                                const uint32_t                  count)
{
       adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e exitRecoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t SPI_PAGING = (uint16_t) DIRECT_SPI_REGION_LEN;

    uint32_t i = 0U;
    uint32_t localCacheCount = 0U; /* Number of total bytes packed into the byte array for SPI transactions */
    uint32_t currentRdCount = 0U; /* Number of registers that are to be read in the current byte array. Different than localCacheCount b/c it doesn't include address packing */
    uint32_t readDataIndex = 0U; /* The current index in readData where data should be placed after it is read. Since it's possible multiple SPI transactions can occur based on the number of bytes requested */
    uint8_t localCache[ADI_HAL_SPI_FIFO_SIZE] = { 0U };
    uint32_t currentAddress = 0U; /* In the case when we need to flush in the middle of byte packing we need to keep track of where the next buffer will start from */
    uint8_t *readDataBytes; /* Used to cast readData array to a byte array */
    uint8_t parsedDataStride = 0U; /* Used when parsing data to determine where the next register value should be placed in readData */
    uint32_t wordReadCount = 0U;    /* Word Read Count */
    uint32_t addrActual = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    /* Re-write addresses so that writes to 0x4700'0000 - 0x4700'3FFF are done
     * via the faster direct spi protocol. Possible as that memory region is aliased to 0x0 to 0x4000. */
    if ((addr >= SPI_ONLY_REGS_ADDR) &&
        (addr < (SPI_ONLY_REGS_ADDR + DIRECT_SPI_REGION_LEN)))
    {
        addrActual = addr - SPI_ONLY_REGS_ADDR;
    }
    else
    {
        addrActual = addr;
    }

    currentAddress = addrActual;

    if (device->common.wrOnly == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_DEVICEHAL,
                            ADI_ADRV904X_ERRCODE_HAL_INVALID_DEVICE_STATE,
                            recoveryAction,
                            device->common.wrOnly,
                            "Device is currently in write-only mode");
        goto cleanup;
    }

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, readData, cleanup);

    readDataBytes = (uint8_t *)readData;

    if (spiCache != NULL)
    {
        /* spiCache is a parameter to this function as a convenience. It is only used to flush any previous writes that have been cached. */
        recoveryAction = adi_adrv904x_SpiFlush( device, spiCache->data, &spiCache->count);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flushing Issue");
            goto cleanup;
        }
    }

    if (adrv904x_Is32BitAccess(addrActual, count) == ADI_TRUE)
    {
        /* Calculate Number of 32 Bit Register Read Transactions Required */
        if ((count & 3U) != 0U)
        {
            wordReadCount = (count >> 2U) + 1U;
        }
        else
        {
            wordReadCount = count >> 2U;
        }

        for (i = 0U; i < wordReadCount; ++i)
        {
            /* Address is in the 32b register maps, force adi_adrv904x_Registers32bOnlyRead */
            if ((mask != NULL) && (mask[i] != 0xFF))
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       mask,
                                       "Address is in 32bit register map, mask should be 0xFFFFFFFF");
                goto cleanup;
            }

            recoveryAction = adi_adrv904x_Registers32bOnlyRead( device,
                                                                NULL,
                                                                addrActual + (4U * i),
                                                                &readData[(4U * i)],
                                                                4U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32bOnlyRead Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "32-Bit Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32")",
                                (addrActual + (4U * i)),
                                readData[(4U * i)]);
        }
    }
    else if (addrActual < SPI_PAGING)
    {
        /* For Direct SPI we're processing bytes that need to be indexed into a uint8_t array */
        parsedDataStride = 1U;
        
        for (i = 0U; i < count; ++i)
        {
            /* For direct SPI reads we can handle any number of bytes and only initiate a SPI transaction when the buffer is full 
                * or after we've packed all the reads together */
            if ((localCacheCount + ADRV904X_DIRECT_SPI_BYTES) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adrv904x_FlushParse(device,
                                                     localCache,
                                                     localCacheCount,
                                                     &readDataBytes[readDataIndex * parsedDataStride],
                                                     currentRdCount,
                                                     parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }
                localCacheCount = 0U;
                readDataIndex += currentRdCount;
                currentRdCount = 0U;
            }

            recoveryAction = adrv904x_DirectSpiDataPack(device,
                                                        localCache,
                                                        &localCacheCount,
                                                        currentAddress,
                                                        0xFFU,
                                                        0x00U,
                                                        ~ADRV904X_SPI_WRITE_POLARITY);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Pack Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "Direct Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                currentAddress,
                                readDataBytes[readDataIndex * parsedDataStride],
                                0xFFU);

            /* Keeping track of how many registers in this localCache instance.
             * This will get cleared on a all to adrv_FlushParse.
             * */
            ++currentRdCount;

            /* Keeping track of which AHB address we're looking at.
             * This will NOT get cleared on a all to adrv_FlushParse.
             * */
            ++currentAddress;
        }
        
        recoveryAction = adrv904x_FlushParse(device,
                                             localCache,
                                             localCacheCount,
                                             &readDataBytes[readDataIndex * parsedDataStride],
                                             currentRdCount,
                                             parsedDataStride);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
            goto cleanup;
        }
    }
    else /* Paging/AHB region must use the AHB indirect interface */
    {
        /* For AHB we're processing bytes and treating the readData array as a byte array
         * */
        parsedDataStride = 1U;
        
        /* auto-increment can cause invalid memory regions to be accessed.
         * Because of that, on the final word to read turn off auto increment.
         * This logic also prevents auto-increment from being used when count == 1
         * */
        if (count == 1U)
        {
            /* Pack the ctrl and first addr then flush to write immediately */
            /* Using the configCache to keep things separate */
            recoveryAction = adrv904x_IndirectReadSetup(device, ADI_DISABLE);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Ctrl Issue");
                goto cleanup;
            }
        }
        else if (i == 0U)
        {
            /* Pack the ctrl and first addr then flush to write immediately */
            /* Using the configCache to keep things separate */
            recoveryAction = adrv904x_IndirectReadSetup(device, device->devStateInfo.spiOptions.allowAhbAutoIncrement);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Ctrl Issue");
                goto cleanup;
            }
        }
        
        /* No matter what we always need to set the address for the first read. */
        recoveryAction = adrv904x_DmaAddrSet(device, currentAddress);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Address Pack Issue");
            goto cleanup;
        }

        for (i = 0U; i < count; ++i)
        {
            
            if ((localCacheCount + (ADRV904X_DIRECT_SPI_BYTES)) > ADI_HAL_SPI_FIFO_SIZE)
            {
                recoveryAction = adrv904x_FlushParse(device,
                                                     localCache,
                                                     localCacheCount,
                                                     &readDataBytes[readDataIndex],
                                                     currentRdCount,
                                                     parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }
                localCacheCount = 0U;
                readDataIndex += currentRdCount;
                currentRdCount = 0U;
            }

            recoveryAction = adrv904x_DirectSpiDataPack(device, localCache, &localCacheCount, ADRV904X_ADDR_SPIDMA0_DATA0, 0xFF, 0x00, ~ADRV904X_SPI_WRITE_POLARITY);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Direct SPI Data Packing Issue");
                goto cleanup;
            }

            ADI_VARIABLE_LOG(   &device->common,
                                ADI_HAL_LOG_SPI,
                                "Page Read (Address: 0x%08" PRIX32" Data: 0x%08" PRIX32" Mask: 0x%08" PRIX32")",
                                currentAddress,
                                readDataBytes[readDataIndex],
                                0xFFU);

            /* Keeping track of how many registers in this localCache instance.
                * This will get cleared on a all to adrv_FlushParse.
                * */
            ++currentRdCount;

            /* Keeping track of which AHB address we're looking at.
                * This will NOT get cleared on a all to adrv_FlushParse.
                * */
            ++currentAddress;

            /* Cases where we need to flush prior to reading next word:
                *  - SPI streaming WITHOUT Spi FIFO mode enabled
                *    - because reading the same register over & over again will throw an error
                *      in SPI streaming
                *  - auto-increment is turned on and second to last byte has been cached
                *    - because the last byte needs to be read with autoincrement disabled to avoid
                *      writing illegal addresses into the AHB bus
                *  - auto-increment is turned off
                *    - because we need to set the address for the next byte
                * */
            if (((device->devStateInfo.spiStreamingOn == 1U) && (device->devStateInfo.spiFifoModeOn == 0U)) ||
                ((device->devStateInfo.autoIncrModeOn == 1U) && i == (count - 2U)) ||
                (device->devStateInfo.autoIncrModeOn == 0U))
            {
                recoveryAction = adrv904x_FlushParse(device,
                                                     localCache,
                                                     localCacheCount,
                                                     &readDataBytes[readDataIndex],
                                                     currentRdCount,
                                                     parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }

                localCacheCount = 0U;
                readDataIndex += currentRdCount;
                currentRdCount = 0U;
            }

            /* Set the address on the first byte or each time if auto-increment is off.
             * This also ensures we will NOT set the address when SPI FIFO mode because we cannot
             * be in SPI FIFO mode with auto increment off.
             * Does not need to be done on the last iteration.
             */
            if ((device->devStateInfo.autoIncrModeOn == 0U) && (i < (count - 1U)))
            {
                recoveryAction = adrv904x_DmaAddrSet(device, currentAddress);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Address Pack Issue");
                    goto cleanup;
                }
            }
            else if ((device->devStateInfo.autoIncrModeOn == 1U) && (i == (count - 2U)))
            {
                /* (count - 1) cached reads have been flushed and auto-increment should be disabled
                 * for the last byte read to avoid invalid memory regions to be accessed.
                 */
                recoveryAction = adrv904x_IndirectReadSetup(device, ADI_DISABLE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DMA Ctrl Issue");
                    goto cleanup;
                }
            }
        }

        if (adrv904x_Is32BitAccess(addrActual, count) != ADI_TRUE)
        {
            /* Final flush before exit in case there's still cached up reads. */
            if (localCacheCount != 0U)
            {
                recoveryAction = adrv904x_FlushParse(device,
                                                     localCache,
                                                     localCacheCount,
                                                     &readDataBytes[readDataIndex],
                                                     currentRdCount,
                                                     parsedDataStride);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "AHB Flushing issue.");
                    goto cleanup;
                }
            }
        }
    }

    /* Clean up the data for the following cases:
     *  - If masking is required
     */
    if (mask != NULL)
    {
        for (i = 0U; i < count; ++i)
        {
            readData[i] &= mask[i];
        }
    }

cleanup:
    if (addrActual >= SPI_PAGING)
    {
        /* Reset any indirect configurations if used. */
        exitRecoveryAction = adrv904x_IndirectReadTeardown(device);
        if (ADI_ADRV904X_ERR_ACT_NONE != exitRecoveryAction)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
        }
    }
    
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32bOnlyRead(adi_adrv904x_Device_t* const    device,
                                                                   adi_adrv904x_SpiCache_t* const  spiCache,
                                                                   const uint32_t                  addr,
                                                                   uint8_t                         readData[],
                                                                   const uint32_t                  count)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, readData);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    recoveryAction = adrv904x_Registers32bOnlyRead_vEndian(device,
                                                           spiCache,
                                                           addr,
                                                           readData,
                                                           count,
                                                           0U);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adrv904x_Registers32bOnlyRead_vEndian failed");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32bOnlyWrite(adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_SpiCache_t* const  spiCache,
                                                                    const uint32_t                  addr,
                                                                    const uint8_t                   writeData[],
                                                                    const uint32_t                  count)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U; /* loop index */
    uint32_t j = 0U; /* loop index */
    uint32_t dataAligned32 = 0U;
    uint8_t *dataAligned8 = (uint8_t*) &dataAligned32;  /* Alias to dataAligned32 usable where uint8_t* arg is required */
    const uint32_t numBytesToWrite = count;

    /* read/wrb | sys_codeb | bus_waiting | bus_response | bus_size[1] | bus_size[0] | auto_incr | bus_sel */
    /*    0           0           0             0               1             0            0          0    */
    uint8_t spiDmaCtl[1U] = { 0x08U };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_NULL_PTR_REPORT_RETURN(&device->common, writeData);

    ADI_ADRV904X_API_ENTRY_QUIET(&device->common);

    if (spiCache != NULL)
    {
        recoveryAction = adi_adrv904x_SpiFlush(device, spiCache->data, &spiCache->count);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Flush Issue After CPU Mailbox Buffer Write");
            goto cleanup;
        }
    }

    /* Range check: is the address one that requires a 32-bit transactions?
     *              does addr + count - 4 lie within valid range?
     */
    recoveryAction = adrv904x_ThirtyTwoBitAhbAccessRangeCheck(device, addr, numBytesToWrite);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Range check failed. Address does not require a 32-bit SPI transaction.");
        goto cleanup;
    }

    /* Write the DMA control register */
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_SPIDMA0_CTL, spiDmaCtl, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma control register.");
        goto cleanup;
    }

    for (i = 0U; i < (numBytesToWrite / 4U); ++i)
    {
        /* Write the bus address */
        recoveryAction = adrv904x_DmaAddrSet(device, addr + (4U * i));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma address registers.");
            goto cleanup;
        }

        /* Ensure data is word aligned before using endianess macro */
        for (j = 0U; j < 4U; ++j)
        {
            dataAligned8[j] = writeData[(i * 4U) + j];
        }
        
        /* Data to be written to the DMA register must be big-endian */
        dataAligned32 = ADRV904X_HTONL(dataAligned32);
        
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_SPIDMA0_DATA3, dataAligned8, 4U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the spi dma data registers.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT_QUIET(&device->common, recoveryAction);

}

/* 
 * Return ADI_ADRV904X_ERR_ACT_NONE if the entire adrv904x memory area starting at addr and of length numBytes lies
 * within a 32bOnly region and the start & end of the memory area is 4-byte aligned and the memory region is not 
 * zero length.
 */
static adi_adrv904x_ErrAction_e adrv904x_ThirtyTwoBitAhbAccessRangeCheck(adi_adrv904x_Device_t* const device, 
                                                                         const uint32_t               addr,
                                                                         const uint32_t               numBytes)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    /* check that the count value is a multiple of 4 and not equal to 0U*/
    if (numBytes % 4U != 0U ||
       ((addr + numBytes - 4U) % 4U != 0U) ||
        numBytes == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numBytes, "The count parameter must greater than 0 and be a multiple of four.");
        return recoveryAction;
    }

    if (adrv904x_Is32BitAccess(addr, numBytes) != ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numBytes, "The address range is not completely in a 32-bit AHB region.");
        return recoveryAction;
    }

    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, addr, "The specified memory location does not require the use of 32-bit SPI transactions.");
        return recoveryAction;
    }

    return recoveryAction;
}
