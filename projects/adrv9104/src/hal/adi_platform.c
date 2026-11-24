/**
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef CUSTOMER_PLATFORM

#include "adi_linux_uio_logging.h"
#include "adi_linux_uio_timer.h"
#include "adi_adrv910x_hal_linux_uio.h"
#include "adi_adm1293_hal_linux_uio.h"
#include "adi_fpga910x_hal_linux_uio.h"

/* Logging interface */
int32_t(*adi_common_hal_LogWrite)(void *devHalCfg, uint32_t logLevel, const char *comment, va_list args) = linux_uio_LogWrite;

/* Timer interface */
int32_t(*adi_common_hal_Wait_us)(void *devHalCfg, uint32_t time_us) = linux_uio_TimerWait_us;

/* Pointer assignments for ADM1293 HAL */
int32_t (*adi_adm1293_hal_i2c_write)(void *devHalCfg,
									 uint8_t slaveAddress,
									 const uint8_t wrData[],
									 uint32_t numWrBytes) = linux_uio_adi_adm1293_hal_i2c_write;
int32_t (*adi_adm1293_hal_i2c_read)(void *devHalCfg,
									uint8_t slaveAddress,
									const uint8_t wrData[],
									uint32_t numWrBytes,
									uint8_t rdData[],
									uint32_t numRdBytes) = linux_uio_adi_adm1293_hal_i2c_read;

/* Pointer assignments for ADRV910X HAL */
int32_t(*adi_adrv910x_hal_open)(void *devHalCfg) = linux_uio_adi_adrv910x_hal_open;
int32_t(*adi_adrv910x_hal_close)(void *devHalCfg) = linux_uio_adi_adrv910x_hal_close;
int32_t(*adi_adrv910x_hal_spi_write)(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes) = linux_uio_adi_adrv910x_hal_spi_write;
int32_t(*adi_adrv910x_hal_spi_read)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numRxBytes) = linux_uio_adi_adrv910x_hal_spi_read;

int32_t(*adi_adrv910x_hal_resetbPin_set)(void *devHalCfg, uint8_t pinLevel) = linux_uio_adi_adrv910x_hal_resetbPin_set;

/* Pointer assignments for FPGA910X HAL */
int32_t(*adi_fpga910x_hal_Uart_Open)(void *devHalCfg, const adi_hal_uartCfg_t* uartConfig) = linux_uio_adi_fpga910x_hal_Uart_Open;
int32_t(*adi_fpga910x_hal_Uart_Close)(void *devHalCfg) = linux_uio_adi_fpga910x_hal_Uart_Close;
int32_t(*adi_fpga910x_hal_Uart_Read)(void *devHalCfg, uint8_t rxData[], uint32_t *rxByteCount, uint32_t timeOut_ms) = linux_uio_adi_fpga910x_hal_Uart_Read;
int32_t(*adi_fpga910x_hal_Uart_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount) = linux_uio_adi_fpga910x_hal_Uart_Write;

int32_t(*adi_fpga910x_hal_Qspi_Open)(void *devHalCfg, const adi_hal_qspiCfg_t* qspiConfig) = linux_uio_adi_fpga910x_hal_Qspi_Open;
int32_t(*adi_fpga910x_hal_Qspi_Close)(void *devHalCfg) = linux_uio_adi_fpga910x_hal_Qspi_Close;
int32_t(*adi_fpga910x_hal_Qspi_Read)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t rxByteCount) = linux_uio_adi_fpga910x_hal_Qspi_Read;
int32_t(*adi_fpga910x_hal_Qspi_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount) = linux_uio_adi_fpga910x_hal_Qspi_Write;

int32_t(*adi_fpga910x_hal_Register_Read)(void *devHalCfg, const uint32_t addr, uint32_t *data) = linux_uio_adi_fpga910x_hal_register_read;

int32_t(*adi_fpga910x_hal_Register_Write)(void *devHalCfg, uint32_t addr, uint32_t data) = linux_uio_adi_fpga910x_hal_register_write;

int32_t(*adi_fpga910x_hal_Ram_Read)(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length) = linux_uio_adi_fpga910x_hal_ram_read;

int32_t(*adi_fpga910x_hal_Ram_Write)(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length) = linux_uio_adi_fpga910x_hal_ram_write;

int32_t(*adi_fpga910x_hal_SsiType_Set)(void *devHalCfg, adi_fpga910x_SsiType_e ssiType) = linux_uio_adi_fpga910x_hal_SsiType_Set;

int32_t(*adi_fpga910x_hal_DmaRelativeAddress_Get)(void *devHalCfg, adi_fpga910x_hal_Dma_e dma, uint32_t *addr) = linux_uio_adi_fpga910x_hal_DmaRelativeAddress_Get;

int32_t(*adi_fpga910x_hal_CurrentPlatform_Get)(void *devHalCfg, adi_fpga910x_Platform_e *platform) = linux_uio_adi_fpga910x_hal_CurrentPlatform_Get;

#else

#include "adi_common_hal_customer.h"
#include "adi_adm1293_hal_customer.h"
#include "adi_adrv910x_hal_customer.h"
#include "adi_fpga910x_hal_customer.h"

/* Logging interface */
int32_t(*adi_common_hal_LogWrite)(void *devHalCfg, uint32_t logLevel, const char *comment, va_list args) = customer_LogWrite;

/* Timer interface */
int32_t(*adi_common_hal_Wait_us)(void *devHalCfg, uint32_t time_us) = customer_TimerWait_us;

/* Pointer assignments for ADM1293 HAL */
int32_t (*adi_adm1293_hal_i2c_write)(void *devHalCfg,
									 uint8_t slaveAddress,
									 const uint8_t wrData[],
									 uint32_t numWrBytes) = customer_adi_adm1293_hal_i2c_write;
int32_t (*adi_adm1293_hal_i2c_read)(void *devHalCfg,
									uint8_t slaveAddress,
									const uint8_t wrData[],
									uint32_t numWrBytes,
									uint8_t rdData[],
									uint32_t numRdBytes) = customer_adi_adm1293_hal_i2c_read;

/* Pointer assignments for ADRV910X HAL */
int32_t(*adi_adrv910x_hal_open)(void *devHalCfg) = customer_adi_adrv910x_hal_open;
int32_t(*adi_adrv910x_hal_close)(void *devHalCfg) = customer_adi_adrv910x_hal_close;
int32_t(*adi_adrv910x_hal_spi_write)(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes) = customer_adi_adrv910x_hal_spi_write;
int32_t(*adi_adrv910x_hal_spi_read)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numRxBytes) = customer_adi_adrv910x_hal_spi_read;

int32_t(*adi_adrv910x_hal_resetbPin_set)(void *devHalCfg, uint8_t pinLevel) = customer_adi_adrv910x_hal_resetbPin_set;

/* Pointer assignments for FPGA910X HAL */
int32_t(*adi_fpga910x_hal_Uart_Open)(void *devHalCfg, const adi_hal_uartCfg_t* uartConfig) = customer_adi_fpga910x_hal_Uart_Open;

int32_t(*adi_fpga910x_hal_Uart_Close)(void *devHalCfg) = customer_adi_fpga910x_hal_Uart_Close;

int32_t(*adi_fpga910x_hal_Uart_Read)(void *devHalCfg, uint8_t rxData[], uint32_t *rxByteCount, uint32_t timeOut_ms) = customer_adi_fpga910x_hal_Uart_Read;

int32_t(*adi_fpga910x_hal_Uart_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount) = customer_adi_fpga910x_hal_Uart_Write;

int32_t(*adi_fpga910x_hal_Qspi_Open)(void *devHalCfg, const adi_hal_qspiCfg_t* qspiConfig) = customer_adi_fpga910x_hal_Qspi_Open;

int32_t(*adi_fpga910x_hal_Qspi_Close)(void *devHalCfg) = customer_adi_fpga910x_hal_Qspi_Close;

int32_t(*adi_fpga910x_hal_Qspi_Read)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t rxByteCount) = customer_adi_fpga910x_hal_Qspi_Read;

int32_t(*adi_fpga910x_hal_Qspi_Write)(void *devHalCfg, const uint8_t txData[], uint32_t txByteCount) = customer_adi_fpga910x_hal_Qspi_Write;

int32_t(*adi_fpga910x_hal_Register_Read)(void *devHalCfg, const uint32_t addr, uint32_t *data) = customer_adi_fpga910x_hal_register_read;

int32_t(*adi_fpga910x_hal_Register_Write)(void *devHalCfg, uint32_t addr, uint32_t data) = customer_adi_fpga910x_hal_register_write;

int32_t(*adi_fpga910x_hal_Ram_Read)(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length) = customer_adi_fpga910x_hal_ram_read;

int32_t(*adi_fpga910x_hal_Ram_Write)(void *devHalCfg, const uint32_t ramAddress, uint32_t data[], uint32_t length) = customer_adi_fpga910x_hal_ram_write;

int32_t(*adi_fpga910x_hal_SsiType_Set)(void *devHalCfg, adi_fpga910x_SsiType_e ssiType) = customer_adi_fpga910x_hal_SsiType_Set;

int32_t(*adi_fpga910x_hal_DmaRelativeAddress_Get)(void *devHalCfg, adi_fpga910x_hal_Dma_e dma, uint32_t *addr) = customer_adi_fpga910x_hal_DmaRelativeAddress_Get;

int32_t(*adi_fpga910x_hal_CurrentPlatform_Get)(void *devHalCfg, adi_fpga910x_Platform_e *platform) = customer_adi_fpga910x_hal_CurrentPlatform_Get;

#endif // !CUSTOMER_PLATFORM
