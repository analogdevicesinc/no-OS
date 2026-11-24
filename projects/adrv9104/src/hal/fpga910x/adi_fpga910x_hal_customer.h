/**
 * \file
 * \brief Template customer implementation of the FPGA910X HAL
* 
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_FPGA910X_HAL_CUSTOMER_H_
#define _ADI_FPGA910X_HAL_CUSTOMER_H_

#include <stdint.h>
#include "adi_fpga910x_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \copydoc adi_fpga910x_hal_Register_Read
 */
int32_t customer_adi_fpga910x_hal_register_read(void *devHalCfg, const uint32_t addr, uint32_t *data);

/**
 * \copydoc adi_fpga910x_hal_Register_Write
 */
int32_t customer_adi_fpga910x_hal_register_write(void *devHalCfg, uint32_t addr, uint32_t data);

/**
 * \copydoc adi_fpga910x_hal_Ram_Read
 */
int32_t customer_adi_fpga910x_hal_ram_read(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length);

/**
 * \copydoc adi_fpga910x_hal_Ram_Write
 */
int32_t customer_adi_fpga910x_hal_ram_write(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length);

/**
 * \copydoc adi_fpga910x_hal_SsiType_Set
 */
int32_t customer_adi_fpga910x_hal_SsiType_Set(void *devHalCfg, adi_fpga910x_SsiType_e ssiType);

/**
 * \copydoc adi_fpga910x_hal_DmaRelativeAddress_Get
 */
int32_t customer_adi_fpga910x_hal_DmaRelativeAddress_Get(void *devHalCfg, adi_fpga910x_hal_Dma_e dma, uint32_t *addr);

/**
 * \copydoc adi_fpga910x_hal_CurrentPlatform_Get
 */
int32_t customer_adi_fpga910x_hal_CurrentPlatform_Get(void *devHalCfg, adi_fpga910x_Platform_e *platform);

/**
 * \copydoc adi_fpga910x_hal_Uart_Open
 */
int32_t customer_adi_fpga910x_hal_Uart_Open(void *devHalCfg, const adi_hal_uartCfg_t *uartConfig);

/**
 * \copydoc adi_fpga910x_hal_Uart_Close
 */
int32_t customer_adi_fpga910x_hal_Uart_Close(void *devHalCfg);

/**
 * \copydoc adi_fpga910x_hal_Uart_Read
 */
int32_t customer_adi_fpga910x_hal_Uart_Read(void *devHalCfg, uint8_t rxData[], uint32_t *rxByteCount, uint32_t timeOut_ms);

/**
 * \copydoc adi_fpga910x_hal_Uart_Write
 */
int32_t customer_adi_fpga910x_hal_Uart_Write(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount);

/**
 * \copydoc adi_fpga910x_hal_Qspi_Open
 */
int32_t customer_adi_fpga910x_hal_Qspi_Open(void *devHalCfg, const adi_hal_qspiCfg_t *qspiConfig);

/**
 * \copydoc adi_fpga910x_hal_Qspi_Close
 */
int32_t customer_adi_fpga910x_hal_Qspi_Close(void *devHalCfg);

/**
 * \copydoc adi_fpga910x_hal_Qspi_Read
 */
int32_t customer_adi_fpga910x_hal_Qspi_Read(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t rxByteCount);

/**
 * \copydoc adi_fpga910x_hal_Qspi_Write
 */
int32_t customer_adi_fpga910x_hal_Qspi_Write(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount);

#ifdef __cplusplus
}
#endif

#endif
