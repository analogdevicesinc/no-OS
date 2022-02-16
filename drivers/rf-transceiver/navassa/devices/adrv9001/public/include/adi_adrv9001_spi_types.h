/**
 * \file
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv9001_hal.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef ADRV9001_SPI_TYPES_H_
#define ADRV9001_SPI_TYPES_H_

#ifndef __KERNEL__
#include <stdint.h>
#include <stddef.h>
#endif

#include "adi_adrv9001.h"
#include "adi_common_hal.h"

#include "adi_adrv9001_gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_ADDR_SIZE 16
#define SPI_DATA_SIZE 8
#define SPI_MASK_SIZE 8
#define HW_RMW_DATA_BYTES 12
#define SPI_MASTER_TOTAL_BYTES_MAX 30

/**
 *  \brief Enum of possible HAL layer error codes.
 */
typedef enum adrv9001_Hal_Err
{
    ADRV9001HAL_OK = 0,        /*!< HAL function successful. No error Detected */
    ADRV9001HAL_SPI_FAIL,      /*!< HAL SPI operation failure. SPI controller Down */
    ADRV9001HAL_GPIO_FAIL,     /*!< HAL GPIO function Failure */
    ADRV9001HAL_TIMER_FAIL,    /*!< HAL Timer function Failure */
    ADRV9001HAL_WAIT_TIMEOUT,  /*!< HAL function Timeout */
    ADRV9001HAL_LOG_FAIL,
    ADRV9001HAL_LOG_LEVEL_FAIL,
    ADRV9001HAL_HAL_MODE_FAIL,
    ADRV9001HAL_GEN_SW,        /*!< HAL function failed due to general invalid  HAL data*/
    ADRV9001HAL_WARNING,       /*!< HAL function warning that non critical error was detected*/
    ADRV9001HAL_BUFFER_OVERFLOW
} adrv9001_Hal_Err_e;

typedef enum
{
	ADI_ADRV9001_SPI_MASTER_SLAVE_SINGLE = 0u,		/* Single SPI slave device connected to ADRV9001 device */
	ADI_ADRV9001_SPI_MASTER_SLAVE_RESERVED = 1u     /* More than one SPI slave device connected; Reserved for future */
} adi_adrv9001_spiMasterSlaveDevices_e;

typedef enum
{
	ADI_ADRV9001_SPI_MASTER_CS_SOURCE_GPIO_ANALOG = 0u,		/* Select digital GPIO for SPI CS */
	ADI_ADRV9001_SPI_MASTER_CS_SOURCE_GPIO_DIGITAL = 1u		/* Select analog GPIO for SPI CS */
} adi_adrv9001_spiMasterSource_e;

typedef enum
{
	ADI_ADRV9001_SPI_MASTER_TRIGGER_SOURCE_ARM = 0u,		/* Reserved for future - Trigger SPI transaction directly from ARM */
	ADI_ADRV9001_SPI_MASTER_TRIGGER_SOURCE_MONITOR = 1u,	/* Trigger SPI transaction from monitor logic */
	ADI_ADRV9001_SPI_MASTER_TRIGGER_SOURCE_REF_TIMER = 2u,	/* Reserved for future - Trigger SPI transaction directly by reference timer */
	ADI_ADRV9001_SPI_MASTER_TRIGGER_SOURCE_GPIO = 3u		/* Reserved for future - Trigger SPI transaction from GPIO */
} adi_adrv9001_spiMasterTriggerSource_e;

typedef enum
{
	ADI_ADRV9001_SPI_MASTER_CS_TRANSFER_TRANSACTION_BYTES = 0u, /* CS assert for transactionBytes and only sends transactionBytes bytes */
	ADI_ADRV9001_SPI_MASTER_CS_TRANSFER_NUM_BYTES = 1u			/* CS assert for numBytes and send all numBytes */
} adi_adrv9001_spiMasterTransferMode_e;

typedef struct
{
	uint8_t                       numBytes;                               /* Number of bytes to write. Valid range is 1 - 30 */
	uint8_t                       baudRateDiv;                            /* SPI baudRate = DEV_CLK_IN / baudRateDiv; Valid range is 0 - 31 */
	uint8_t                       transactionBytes;                       /* Number of bytes per CS assertion.Valid range is 1 - 30 */
	adi_adrv9001_spiMasterTransferMode_e       transferMode;              /* CS transfer during frame duration */
	adi_adrv9001_spiMasterSlaveDevices_e       spiSlaveDevicesConnected;  /* Single or multiple(only in future) SPI slaves connected with ADRV9001 */
	adi_adrv9001_spiMasterSource_e			   csSource;                  /* GPIO type for all SPI pins: Analog or Digital */
	adi_adrv9001_GpioPin_e                     pin;                       /* CS pin - Other SPI signals have dedicated pins: Pin9 = SPI_CLK, 
																										Pin10 = SPI_MISO, Pin11= SPI_MOSI*/
	adi_adrv9001_spiMasterTriggerSource_e      triggerSource;             /* SPI master transaction trigger source */
	uint32_t                      wakeupTimer_us;                         /* Monitor mode early wakeup timer for SPI master transaction. Only available 
                                                                                     if triggerSource = ADI_ADRV9001_SPI_MASTER_TRIGGER_SOURCE_MONITOR */
	uint8_t                      spiData[SPI_MASTER_TOTAL_BYTES_MAX];	  /* Bytes to be written in SPI slave device */
} adi_adrv9001_spiMasterConfig_t;

#ifdef __cplusplus
}
#endif

#endif /* ADRV9001_HAL_TYPES_H_ */
