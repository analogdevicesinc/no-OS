/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_init.h
* \brief Contains ADRV9025 init related private function prototypes for
*        adrv9025_init.c that helps adi_adrv9025_init.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

# ifndef _ADRV9025_INIT_H_
#define _ADRV9025_INIT_H_

#include "adi_adrv9025.h"



#ifdef __cplusplus
extern "C" {
#endif


#define ADRV9025_BUGINFO(x)
#define ADRV9025_BUGINFO_NUM(x,n)

#define ADRV9025_DMAINFO(text, addr, count)

#define ADRV9025_SPIDMAINFO(s,a,b,c)


#define ADRV9025_SPIWRITEBYTESDMA(text, addr, addrArray, dataArray, count) \
{\
    recoveryAction = adi_adrv9025_SpiBytesWrite(device, (addrArray), (dataArray), (count)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing bytes to DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_DMAINFO((text), (addr), (count)); \
}

#define ADRV9025_SPIREADBYTESDMA(text, addr, addrArray, dataArray, count) \
{\
    recoveryAction = adi_adrv9025_SpiBytesRead(device, (addrArray), (dataArray), (count)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading bytes through DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_DMAINFO((text), (addr), (count)); \
}

#define ADRV9025_SPIWRITEWORDDMA(text, addr, data) \
{\
    recoveryAction = adi_adrv9025_SpiDmaWordWrite(device, (data), (addr)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing word to DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:WORD WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
}

#define ADRV9025_SPIREADWORDDMA(text, addr, data) \
{\
    recoveryAction = adi_adrv9025_SpiDmaWordRead(device, (data), (addr)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading word from DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:WORD READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
}

#define ADRV9025_SPIWRITEWORDDMASTREAMWITHCUSTOMHALFUNCTION(text, addr, data, byteCount) \
{\
    recoveryAction = adi_adrv9025_SpiWriteStreamWithCustomHalFunction(device, data, byteCount, (uint16_t)addr); \
    ADRV9025_SPI_STREAM_DISABLE_ON_ERROR(device, recoveryAction); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing bytes through DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:WRITE STREAM: %30s: addr=0x%04x, count=0x%02x \n", (text), (addr), (byteCount)); \
}

#define ADRV9025_SPIREADWORDDMASTREAMWITHCUSTOMHALFUNCTION(text, addr, readData, byteCount) \
{\
    recoveryAction = adi_adrv9025_SpiReadStreamWithCustomHalFunction(device, readData, byteCount, (uint16_t)addr); \
    ADRV9025_SPI_STREAM_DISABLE_ON_ERROR(device, recoveryAction); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading bytes through DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:READ STREAM: %30s: addr=0x%04x, count=0x%02x \n", (text), (addr), (byteCount)); \
}

#define ADRV9025_SPIWRITEWORDDMASTREAM(text, addr, data, byteCount) \
{\
    recoveryAction = adi_adrv9025_SpiDmaWordWriteStream(device, data, byteCount, (uint16_t)addr); \
    ADRV9025_SPI_STREAM_DISABLE_ON_ERROR(device, recoveryAction); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing bytes through DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:WRITE STREAM: %30s: addr=0x%04x, count=0x%02x \n", (text), (addr), (byteCount)); \
}

#define ADRV9025_SPIREADWORDDMASTREAM(text, addr, returnData, byteCount) \
{\
    recoveryAction = adi_adrv9025_SpiDmaWordReadStream(device, returnData, byteCount, (uint16_t)addr); \
    ADRV9025_SPI_STREAM_DISABLE_ON_ERROR(device, recoveryAction); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while reading bytes through DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:READ STREAM: %30s: addr=0x%04x, count=0x%02x \n", (text), (addr), (byteCount)); \
}


#define ADRV9025_SPIWRITEBYTEDMA(text, addr, data) \
{\
    recoveryAction = adi_adrv9025_SpiByteWrite(device, (addr), (data)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while writing byte to DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
}

#define ADRV9025_SPIREADBYTEDMA(text, addr, data) \
{\
    recoveryAction = adi_adrv9025_SpiByteRead(device, (addr), (data)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recoveryAction, NULL, "Error while Reading byte from DMA over Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIDMAINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
}

#define ADRV9025_SPIINFO(s,a,b,c)
#define ADRV9025_SPI_FIELD_INFO(s,a,b,c,d)

#define ADRV9025_SPIWRITEBYTE(text, addr, data) \
{\
    int32_t recAct = 0; \
    recAct = adi_adrv9025_SpiByteWrite(device, (addr), (data)); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recAct, NULL, "Error while writing byte to Spi"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPIINFO("MESSAGE: WRITE: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (data)); \
}

#define ADRV9025_SPIREADBYTE(text, addr, data) \
{\
    int32_t recAct = 0; \
    recAct = adi_adrv9025_SpiByteRead(device, (addr), (data)); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recAct, NULL, "Error while reading byte from Spi"); \
    ADRV9025_SPIINFO("MESSAGE:  READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (*(uint8_t*)(data))); \
}

#define ADRV9025_SPIFIELDWRITE(device, addr, fieldVal, mask, startBit, text) \
{ \
    int32_t recAct = 0; \
    recAct = adi_adrv9025_SpiFieldWrite(device, addr, fieldVal, mask, startBit); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recAct, NULL, "Error while Spi field write"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPI_FIELD_INFO("MESSAGE: WRITE FIELD: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}

#define ADRV9025_SPIFIELDREAD(device, addr, fieldVal, mask, startBit, text) \
{ \
    int32_t recAct = 0; \
    recAct = adi_adrv9025_SpiFieldRead(device, addr, (fieldVal), mask, startBit); \
    ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL, recAct, NULL, "Error while Spi field read"); \
    ADI_ERROR_RETURN(device->common.error.newAction); \
    ADRV9025_SPI_FIELD_INFO("MESSAGE: READ: %30s: addr=0x%04x, data=0x%02x \n", (text), (addr), (fieldVal), (startBit)); \
}

/**
* \brief Set various clock in the B0 silicon version device
* Configure the pad DEV_CLK divider so that output of pad divider is less than 500MHz.
* Configure ARM_clk_divide_ratio and reg_clk_divide_ratio (register bus clock).
* Maximum rate for ARM clock and REGBUS clock is 500MHz.
* Use DEVCLK to clock the HSDIGCLK input bypassing the PLL (write clock_config_6[7] to 1)
* The clock_config_6 register needs to be written to configure the clock divide ratios indicated
* Write SPI bit digital_clock_power_up to enable digital clocks (core.clock_control_6[4])
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param init Pointer to ADRV9025 initialization settings structures
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ClocksSet(adi_adrv9025_Device_t* device,
                           adi_adrv9025_Init_t*   init);

/**
* \brief Verifies the init structure profiles are valid combinations
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
* This function uses Adrv9025_TxProfileVerify(), Adrv9025_RxProfileVerify(), and
* Adrv9025_OrxProfileVerify() as helper functions.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param init Pointer to ADRV9025 initialization settings structures
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_ProfilesVerify(adi_adrv9025_Device_t* device,
                                adi_adrv9025_Init_t*   init);

/**
* \brief Set various LDO in the device
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param ldoSelect Value indicating how to configure the LDO selection
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_LdoEnable(adi_adrv9025_Device_t* device,
                           uint8_t                ldoSelect);

/**
* \brief Sets up the thresholds on Rx channel which triggers overload bits
*        if the input level exceeds threshold.
*
* \pre This function is private and is not called directly by the user.
*      This function can only be called once the clocks are setup via
*      adrv9025_ClocksSet() function.
*
* \dep_begin
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param rxChannel Channel for which overload protection config is desired
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxOverloadProtectionSet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel);

#ifdef __cplusplus
}
#endif

#endif
