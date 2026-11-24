/**
 * \file
 * \brief Template customer implementation of the ADRV910X HAL
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_ADRV910X_HAL_CUSTOMER_H_
#define _ADI_ADRV910X_HAL_CUSTOMER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \copydoc adi_adrv910x_hal_open
 */
int32_t customer_adi_adrv910x_hal_open(void *devHalCfg);

/**
 * \copydoc adi_adrv910x_hal_close
 */
int32_t customer_adi_adrv910x_hal_close(void *devHalCfg);

/**
 * \copydoc adi_hal_SpiWrite
 */
int32_t customer_adi_adrv910x_hal_spi_write(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes);

/**
 * \copydoc adi_hal_SpiRead
 */
int32_t customer_adi_adrv910x_hal_spi_read(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numTxRxBytes);

/**
 * \copydoc adi_adrv910x_hal_close
 */
int32_t customer_adi_adrv910x_hal_resetbPin_set(void *devHalCfg, uint8_t pinLevel);
    
#ifdef __cplusplus
}
#endif

#endif
