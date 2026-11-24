/**
* \file
* \brief Contains ADI Transceiver Hardware Abstraction functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_fpga910x_hal.h"
#include "adi_fpga910x_hal_wrapper.h"

int32_t adi_fpga910x_hal_wrapper_Register_Read(adi_fpga910x_Device_t *fpga910x, const uint32_t addr, uint32_t *data)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(fpga910x, data);
    
    halError = adi_fpga910x_hal_Register_Read(fpga910x->common.devHalInfo, addr, data);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "FPGA register read failed");

    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&fpga910x->common, "%s(0x%04X, 0x%02X)", addr, *data);
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Register_Write(adi_fpga910x_Device_t *fpga910x, uint32_t addr, uint32_t data)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);

    halError = adi_fpga910x_hal_Register_Write(fpga910x->common.devHalInfo, addr, data);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "FPGA register write failed");
    
    ADI_FUNCTION_ENTRY_VARIABLE_LOG(&fpga910x->common, "%s(0x%05X, 0x%02X)", addr, data);

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Ram_Read(adi_fpga910x_Device_t *fpga910x, 
                                          const uint32_t ramAddress,
                                          uint32_t data[],
                                          uint32_t length)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(fpga910x, data);
    
    halError = adi_fpga910x_hal_Ram_Read(fpga910x->common.devHalInfo, ramAddress, data, length);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "FPGA RAM read failed");
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Ram_Write(adi_fpga910x_Device_t *fpga910x, 
                                           const uint32_t ramAddress,
                                           uint32_t data[],
                                           uint32_t length)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_NULL_PTR_RETURN(fpga910x, data);
    
    halError = adi_fpga910x_hal_Ram_Write(fpga910x->common.devHalInfo, ramAddress, data, length);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "FPGA RAM write failed");
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_SsiType_Set(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_SsiType_e ssiType)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, ssiType, ADI_FPGA910X_SSI_TYPE_CMOS, ADI_FPGA910X_SSI_TYPE_LVDS);
    
    halError = adi_fpga910x_hal_SsiType_Set(fpga910x->common.devHalInfo, ssiType);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to set FPGA SSI type");
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_DmaRelativeAddress_Get(adi_fpga910x_Device_t *fpga910x,
                                                        adi_fpga910x_hal_Dma_e dma,
                                                        uint32_t *addr)
{
    int32_t halError = 0;
    
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    ADI_RANGE_CHECK(fpga910x, dma, ADI_FPGA910X_HAL_DMA_RX1, ADI_FPGA910X_HAL_DMA_SPI);
    ADI_NULL_PTR_RETURN(fpga910x, addr);
    
    halError = adi_fpga910x_hal_DmaRelativeAddress_Get(fpga910x->common.devHalInfo, dma, addr);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to get DMA relative address");
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_CurrentPlatform_Get(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_Platform_e *platform)
{
    int32_t halError = 0;
    ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
    halError = adi_fpga910x_hal_CurrentPlatform_Get(fpga910x->common.devHalInfo, platform);
    ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to get FPGA platform type");
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Uart_Open(adi_fpga910x_Device_t *fpga910x, const adi_hal_uartCfg_t *uartConfig)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	halError = adi_fpga910x_hal_Uart_Open(fpga910x, uartConfig);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to open UART");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Uart_Close(adi_fpga910x_Device_t *fpga910x)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	halError = adi_fpga910x_hal_Uart_Close(fpga910x);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to close UART");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Uart_Read(adi_fpga910x_Device_t *fpga910x, uint8_t rxData[], uint32_t *rxByteCount, uint32_t timeOut_ms)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	halError = adi_fpga910x_hal_Uart_Read(fpga910x, rxData, rxByteCount, timeOut_ms);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to read from UART");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Uart_Write(adi_fpga910x_Device_t *fpga910x, const uint8_t txData[], uint32_t txByteCount)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	halError = adi_fpga910x_hal_Uart_Write(fpga910x, txData, txByteCount);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to write to UART");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Qspi_Open(adi_fpga910x_Device_t *fpga910x, const adi_hal_qspiCfg_t *qspiConfig)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	ADI_NULL_PTR_RETURN(fpga910x, qspiConfig);
	halError = adi_fpga910x_hal_Qspi_Open(fpga910x, qspiConfig);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to open QSPI");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Qspi_Close(adi_fpga910x_Device_t *fpga910x)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	halError = adi_fpga910x_hal_Qspi_Close(fpga910x);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to close QSPI");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Qspi_Read(adi_fpga910x_Device_t *fpga910x,
                                           const uint8_t txData[],
                                           uint8_t rxData[],
                                           uint32_t rxByteCount)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	/* txData is intentionally not checked, NULL means clock out 0xAA(configurable) */
	ADI_NULL_PTR_RETURN(fpga910x, rxData);
	halError = adi_fpga910x_hal_Qspi_Read(fpga910x, txData, rxData, rxByteCount);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to read from QSPI");
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_hal_wrapper_Qspi_Write(adi_fpga910x_Device_t *fpga910x, const uint8_t txData[], uint32_t txByteCount)
{
	int32_t halError = 0;
	ADI_NULL_DEVICE_PTR_RETURN(fpga910x);
	ADI_NULL_PTR_RETURN(fpga910x, txData);
	halError = adi_fpga910x_hal_Qspi_Write(fpga910x, txData, txByteCount);
	ADI_ERROR_REPORT(fpga910x, 2, halError, ADI_COMMON_ACT_ERR_RESET_FEATURE, NULL, "Failed to write to QSPI");
	ADI_API_RETURN(fpga910x);
}
