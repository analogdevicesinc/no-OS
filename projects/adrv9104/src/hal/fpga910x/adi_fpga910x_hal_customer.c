/**
 * \file
 * \brief Template customer implementation of the FPGA910X HAL
* 
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
 */

#include "adi_fpga910x_hal_customer.h"

int32_t customer_adi_fpga910x_hal_register_read(void *devHalCfg, const uint32_t addr, uint32_t *data)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_register_write(void *devHalCfg, uint32_t addr, uint32_t data)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_ram_read(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_ram_write(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_SsiType_Set(void *devHalCfg, adi_fpga910x_SsiType_e ssiType)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_DmaRelativeAddress_Get(void *devHalCfg, adi_fpga910x_hal_Dma_e dma, uint32_t *addr)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_fpga910x_hal_CurrentPlatform_Get(void *devHalCfg, adi_fpga910x_Platform_e *platform)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Uart_Open(void *devHalCfg, const adi_hal_uartCfg_t *uartConfig)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Uart_Close(void *devHalCfg)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Uart_Read(void *devHalCfg, uint8_t rxData[], uint32_t *rxByteCount, uint32_t timeOut_ms)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Uart_Write(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Qspi_Open(void *devHalCfg, const adi_hal_qspiCfg_t *qspiConfig)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Qspi_Close(void *devHalCfg)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Qspi_Read(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t rxByteCount)
{
	/* Customer code goes here */
	return 0;
}

int32_t customer_adi_fpga910x_hal_Qspi_Write(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount)
{
	/* Customer code goes here */
	return 0;
}

