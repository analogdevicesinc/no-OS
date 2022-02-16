/**
* \file
* \brief Contains Adrv9001 features related function implementation defined in
* adi_adrv9001_hal.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_spi.h"
#include "adi_common_error.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"

#include "adi_adrv9001_hal.h"
#include "object_ids.h"

/*********************************************************************************************************/
int32_t adi_adrv9001_spi_DataPack(adi_adrv9001_Device_t *device,
                                 uint8_t *wrData,
                                 uint16_t *numWrBytes,
                                 uint16_t addr,
                                 uint8_t mask,
                                 uint8_t data,
                                 uint8_t writeFlag)
{
    ADI_ENTRY_EXPECT(device);

    ADI_NULL_PTR_RETURN(&device->common, wrData);
    
    ADI_NULL_PTR_RETURN(&device->common, numWrBytes);

    if (mask == 0xFF)
    {
        /* if the mask is 0xFF then the entire 8 bits is being written to the 
           register and so no masking is required and the operation can be
           achieved using 3 bytes*/
        if ((*numWrBytes + ADRV9001_SPI_BYTES) >= HAL_SPIWRITEARRAY_BUFFERSIZE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEHAL, 
                             ADRV9001HAL_BUFFER_OVERFLOW,
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
        if ((*numWrBytes + ADRV9001_HW_RMW_BYTES) >= HAL_SPIWRITEARRAY_BUFFERSIZE)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_DEVICEHAL,
                             ADRV9001HAL_BUFFER_OVERFLOW,
                             ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                             NULL,
                             "Buffer size Exceeded error");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

        /* pack the low address byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_LO_ADDR >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_LO_ADDR);
        wrData[(*numWrBytes)++] = (uint8_t)addr;

        /* pack the high address byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_HI_ADDR >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_HI_ADDR);
        wrData[(*numWrBytes)++] = (uint8_t)(addr >> 8);

        /* pack the mask byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_MASK >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_MASK);
        wrData[(*numWrBytes)++] = (uint8_t)mask;

        /* pack the mask byte*/
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_DATA >> 8);
        wrData[(*numWrBytes)++] = (uint8_t)(ADRV9001_HW_RMW_DATA);
        /*coverty will trigger an error which is ok in this case.*/
        wrData[(*numWrBytes)++] = (uint8_t)data;
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Stream_DataPack(adi_adrv9001_Device_t *device,
                                         uint8_t *wrData,
                                         uint32_t *numWrBytes,
                                         uint16_t addr,
                                         const uint8_t data[],
                                         uint32_t count,
                                         uint8_t writeFlag)
{
    int32_t i = 0;

    ADI_ENTRY_EXPECT(device);

    ADI_NULL_PTR_RETURN(&device->common, wrData);
    
    ADI_NULL_PTR_RETURN(&device->common, numWrBytes);

    /* if the mask is 0xFF then the entire 8 bits is being written to the 
   register and so no masking is required and the operation can be
   achieved using 3 bytes*/
    if ((*numWrBytes + ADRV9001_SPI_BYTES) > 0xC0000)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_DEVICEHAL, 
            ADRV9001HAL_BUFFER_OVERFLOW,
            ADI_COMMON_ACT_ERR_RESET_INTERFACE,
            NULL, 
            "Buffer size Exceeded error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    wrData[(*numWrBytes)++] = (uint8_t)(((writeFlag & 0x01) << 7) | ((addr >> 8) & 0x7F));
    wrData[(*numWrBytes)++] = (uint8_t)(addr);

    for (i = count - 1 ; i >= 0 ; i--)
    {
        wrData[(*numWrBytes)++] = (uint8_t)data[i];
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Byte_Write(adi_adrv9001_Device_t *device, uint16_t addr, uint8_t data)
{
    int32_t halError = 0;
    int32_t i = 0;
    uint16_t numTxBytes = 0;
    uint8_t txData[ADRV9001_SPI_BYTES] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr, data);

    ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &txData[0], &numTxBytes, addr, 0xFF, data, ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiWrite(device->common.devHalInfo, &txData[0], numTxBytes);
        if (halError == 0)
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
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Bytes_Stream_Write(adi_adrv9001_Device_t *device, uint16_t addr, const uint8_t data[], uint32_t count)
{
    int32_t halError = 0;
    int32_t i = 0;
    uint32_t numTxBytes = 0;
    uint8_t txData[6] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr + i, data[i]);
    }

    ADI_EXPECT(adi_adrv9001_spi_Stream_DataPack,device, &txData[0], &numTxBytes, addr, data, count, ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiWrite(device->common.devHalInfo, &txData[0], numTxBytes);
        if (halError == 0)
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
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Bytes_Write(adi_adrv9001_Device_t *device, const uint16_t addr[], const uint8_t data[], uint32_t count)
{
    int32_t halError = 0;
    uint32_t i = 0;
    uint16_t numWrBytes = 0;
    uint8_t wrData[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr[i], data[i]);
    }

    ADI_NULL_PTR_RETURN(&device->common, addr);

    ADI_NULL_PTR_RETURN(&device->common, data);

    for (i = 0; i < count; i++)
    {
        ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &wrData[0], &numWrBytes, addr[i], 0xFF, data[i], ADRV9001_SPI_WRITE_POLARITY);
    }
 
    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiWrite(device->common.devHalInfo, &wrData[0], numWrBytes);
        if (halError == 0)
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
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Byte_Read(adi_adrv9001_Device_t *device, uint16_t addr, uint8_t *readData)
{
    int32_t halError = 0;
    int32_t i = 0;
    uint16_t numWrBytes = 0;
    uint8_t wrData[ADRV9001_SPI_BYTES] = { 0 };
    uint8_t regVal = 0;
    uint8_t regData[ADRV9001_SPI_BYTES] = { 0 };
   
    ADI_ENTRY_PTR_EXPECT(device, readData);

    ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &wrData[0], &numWrBytes, addr, 0xFF, regVal, ~ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiRead(device->common.devHalInfo, &wrData[0], &regData[0], numWrBytes);
        if (halError == 0)
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

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr, *readData);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Bytes_Read(adi_adrv9001_Device_t *device, 
                                  const uint16_t addr[], 
                                  uint8_t readData[], 
                                  uint32_t count)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint32_t i = 0;
    uint32_t j = 0;
    int32_t halError = 0;
    uint8_t wrData[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };
    uint8_t rdData[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };
    uint16_t numWrBytes = 0;
    uint8_t regVal = 0;
    
    ADI_ENTRY_EXPECT(device);

    ADI_NULL_PTR_RETURN(&device->common, addr);

    ADI_NULL_PTR_RETURN(&device->common, readData);

    for (i = 0; i < count; i++)
    {
        recoveryAction = adi_adrv9001_spi_DataPack(device, &wrData[0], &numWrBytes, addr[i], 0xFF, regVal, ~ADRV9001_SPI_WRITE_POLARITY);
        ADI_ERROR_RETURN(device->common.error.newAction);  
    }

    for (j = 0; j < ADI_ADRV9001_NUMBER_SPI_RETRY; j++)
    {
        halError = adi_hal_SpiRead(device->common.devHalInfo, &wrData[0], &rdData[0], numWrBytes);
        if (halError == 0)
        {
            break;
        }
    }

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr[i], rdData[(i * 3) + 2]);
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
    return recoveryAction;
}

int32_t adi_adrv9001_spi_Field_Write(adi_adrv9001_Device_t *device, 
                                   uint16_t addr, 
                                   uint8_t  fieldVal, 
                                   uint8_t mask, 
                                   uint8_t startBit)
{
    int32_t halError = 0;
    uint16_t numWrBytes = 0;
    uint8_t regVal = ((fieldVal << startBit) & mask);
    uint8_t newRegVal = 0;

    uint8_t registerVal[ADRV9001_SPI_BYTES] = { 0 };
    uint8_t wrData[ADRV9001_HW_RMW_BYTES] = { 0 };
    uint8_t i = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr, regVal);
            
    ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &wrData[0], &numWrBytes, addr, 0xFF, regVal, ~ADRV9001_SPI_WRITE_POLARITY);
    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiRead(device->common.devHalInfo, &wrData[0], &registerVal[0], numWrBytes);
        if (halError == 0)
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

    newRegVal = (registerVal[2] & ~mask) | (regVal & mask);
    numWrBytes = 0;

    ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &wrData[0], &numWrBytes, addr, 0xFF, newRegVal, ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiWrite(device->common.devHalInfo, &wrData[0], numWrBytes);
        if (halError == 0)
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

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Field_Read(adi_adrv9001_Device_t *device, 
                                  uint16_t addr, 
                                  uint8_t *fieldVal, 
                                  uint8_t mask, 
                                  uint8_t startBit)
{
    /* only use if caching is off */
    int32_t halError = 0;
    uint16_t numWrBytes = 0;
    uint8_t wrData[8] = { 0 };
    uint8_t rdData[8] = { 0 };
    uint8_t regVal = 0;
    uint8_t i = 0;

    ADI_ENTRY_EXPECT(device);

    ADI_NULL_PTR_RETURN(&device->common, fieldVal);

    ADI_EXPECT(adi_adrv9001_spi_DataPack, device, &wrData[0], &numWrBytes, addr, 0xFF, regVal, ~ADRV9001_SPI_WRITE_POLARITY);

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiRead(device->common.devHalInfo, &wrData[0], &rdData[0], numWrBytes);
        if (halError == 0)
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

    regVal = rdData[2];
    startBit = startBit & 0x0F;

    *fieldVal = (uint8_t)((regVal & mask) >> startBit);

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", addr, regVal);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Mask_Write(adi_adrv9001_Device_t *device, uint16_t addr, uint8_t data, uint8_t mask)
{
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Cache_Write(adi_adrv9001_Device_t *device, 
                                   const uint32_t wrCache[], 
                                   uint32_t count)
{
    int32_t halError = 0;
    uint32_t i = 0;
    uint16_t numWrBytes = 0;
    uint8_t wrData[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };

    ADI_NULL_DEVICE_PTR_RETURN(device);

    for (i = 0; i < count; i++)
    {
        ADI_EXPECT(adi_adrv9001_spi_DataPack, 
                       device, 
                       &wrData[0], 
                       &numWrBytes, 
                       wrCache[i] >> SPI_ADDR_SIZE, 
                       wrCache[i] >> SPI_MASK_SIZE, 
                       wrCache[i], 
                       ADRV9001_SPI_WRITE_POLARITY);
       
    }

    for (i = 0; i < ADI_ADRV9001_NUMBER_SPI_RETRY; i++)
    {
        halError = adi_hal_SpiWrite(device->common.devHalInfo, &wrData[0], numWrBytes);
        if (halError == 0)
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

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Cache_Read(adi_adrv9001_Device_t *device, 
                                  const uint32_t rdCache[], 
                                  uint8_t readData[], 
                                  uint32_t count)
{
    uint32_t i = 0;
    uint32_t j = 0;
    int32_t halError = 0;
    uint8_t wrData[HAL_SPIWRITEARRAY_BUFFERSIZE] = { 0 };
    uint16_t numWrBytes = 0;

    ADI_ENTRY_EXPECT(device);

    ADI_NULL_PTR_RETURN(&device->common, rdCache);

    ADI_NULL_PTR_RETURN(&device->common, readData);

    for (i = 0; i < count; i++)
    {
        ADI_EXPECT(adi_adrv9001_spi_DataPack,
                       device,  
                       &wrData[0], 
                       &numWrBytes, 
                       rdCache[i]>> SPI_ADDR_SIZE, 
                       0xFF, 
                       0,
                       ~ADRV9001_SPI_WRITE_POLARITY);
    }

    for (j = 0; j < ADI_ADRV9001_NUMBER_SPI_RETRY; j++)
    {
        halError = adi_hal_SpiRead(device->common.devHalInfo, &wrData[0], &readData[0], numWrBytes);
        if (halError == 0)
        {
            break;
        }
    }

    for (i = 0; i < count; i++)
    {
        ADI_FUNCTION_ENTRY_VARIABLE_LOG(&device->common, "%s(0x%04X, 0x%02X)", wrData[i], readData[(i * 3) + 2]);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     (adi_common_ErrSources_e)halError,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "SPI write error");
    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_spi_Master_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
												adi_adrv9001_spiMasterConfig_t *spiMasterConfig)
{
	ADI_NULL_PTR_RETURN(&adrv9001->common, spiMasterConfig);
	ADI_RANGE_CHECK(adrv9001, spiMasterConfig->numBytes, 1, SPI_MASTER_TOTAL_BYTES_MAX);
	ADI_RANGE_CHECK(adrv9001, spiMasterConfig->baudRateDiv, 0, 31);
	ADI_RANGE_CHECK(adrv9001, spiMasterConfig->transactionBytes, 1, SPI_MASTER_TOTAL_BYTES_MAX);

	if(spiMasterConfig->csSource == ADI_ADRV9001_SPI_MASTER_CS_SOURCE_GPIO_ANALOG)
	{
		ADI_RANGE_CHECK(adrv9001, spiMasterConfig->pin, ADI_ADRV9001_GPIO_ANALOG_00, ADI_ADRV9001_GPIO_ANALOG_08);
	}
	else if(spiMasterConfig->csSource == ADI_ADRV9001_SPI_MASTER_CS_SOURCE_GPIO_DIGITAL)
	{
		ADI_RANGE_CHECK(adrv9001, spiMasterConfig->pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_08);
	}

	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_spi_Master_Configure(adi_adrv9001_Device_t *device,
										  adi_adrv9001_spiMasterConfig_t *spiMasterConfig)
{
	uint8_t armData[48] = { 0 };
	uint8_t extData[5] = { 0 };
	uint32_t offset = 0;
	uint8_t cnt = 0;

	ADI_PERFORM_VALIDATION(adi_adrv9001_spi_Master_Configure_Validate, device, spiMasterConfig);

	adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
	armData[offset++] = spiMasterConfig->numBytes;
	armData[offset++] = spiMasterConfig->baudRateDiv;
	armData[offset++] = spiMasterConfig->transactionBytes;
	armData[offset++] = spiMasterConfig->transferMode;
	armData[offset++] = spiMasterConfig->spiSlaveDevicesConnected;
	armData[offset++] = spiMasterConfig->csSource;
	armData[offset++] = spiMasterConfig->pin - 1;
	armData[offset++] = spiMasterConfig->triggerSource;
	adrv9001_LoadFourBytes(&offset, armData, spiMasterConfig->wakeupTimer_us);

	for (cnt = 0; cnt < SPI_MASTER_TOTAL_BYTES_MAX; cnt++)
	{
		armData[offset++] = spiMasterConfig->spiData[cnt];
	}

	extData[0] = 0;
	extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_SPI_MASTER_CONFIG;

	ADI_EXPECT(adi_adrv9001_arm_Config_Write, device, armData, sizeof(armData), extData, sizeof(extData))

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_spi_Master_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
	adi_adrv9001_spiMasterConfig_t *spiMasterConfig)
{
	ADI_NULL_PTR_RETURN(&adrv9001->common, spiMasterConfig);
	ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_spi_Master_Inspect(adi_adrv9001_Device_t *device,
										adi_adrv9001_spiMasterConfig_t *spiMasterConfig)
{
	uint8_t armReadBack[44] = { 0 };
	uint8_t channelMask = 0;
	uint32_t offset = 0;
	uint8_t cnt = 0;

	ADI_PERFORM_VALIDATION(adi_adrv9001_spi_Master_Inspect_Validate, device, spiMasterConfig);

	ADI_EXPECT(adi_adrv9001_arm_Config_Read, device, OBJID_CFG_SPI_MASTER_CONFIG, channelMask, offset, armReadBack, sizeof(armReadBack))

	spiMasterConfig->numBytes = armReadBack[offset++];
	spiMasterConfig->baudRateDiv = armReadBack[offset++];
	spiMasterConfig->transactionBytes = armReadBack[offset++];
	spiMasterConfig->transferMode = armReadBack[offset++];
	spiMasterConfig->spiSlaveDevicesConnected = armReadBack[offset++];
	spiMasterConfig->csSource = armReadBack[offset++];
	spiMasterConfig->pin = (armReadBack[offset++] + 1);
	spiMasterConfig->triggerSource = armReadBack[offset++];
	adrv9001_ParseFourBytes(&offset, armReadBack, &spiMasterConfig->wakeupTimer_us);

	for (cnt = 0; cnt < SPI_MASTER_TOTAL_BYTES_MAX; cnt++)
	{
		spiMasterConfig->spiData[cnt] = armReadBack[offset++];
	}

	ADI_API_RETURN(device);
}