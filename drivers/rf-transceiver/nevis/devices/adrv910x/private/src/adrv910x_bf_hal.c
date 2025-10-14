/**
* \file
* \brief BitField hardware abstraction layer
*
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adrv910x_bf_hal.h"
#include "adi_adrv910x_spi.h"

int32_t adi_bf_hal_Field_Write(void *device, uint16_t address, uint8_t value, uint8_t mask, uint8_t startBit)
{
	return adi_adrv910x_spi_Field_Write((adi_adrv910x_Device_t *)device, address, value, mask, startBit);
}

int32_t adi_bf_hal_Field_Read(void *device, uint16_t address, uint8_t *value, uint8_t mask, uint8_t startBit)
{
	return adi_adrv910x_spi_Field_Read((adi_adrv910x_Device_t *)device, address, value, mask, startBit);
}

int32_t adi_bf_hal_Register_Write(void *device, uint16_t address, uint8_t value)
{
	return adi_adrv910x_spi_Byte_Write((adi_adrv910x_Device_t *)device, address, value);
}

int32_t adi_bf_hal_Register_Read(void *device, uint16_t address, uint8_t *value)
{
	return adi_adrv910x_spi_Byte_Read((adi_adrv910x_Device_t *)device, address, value);
}