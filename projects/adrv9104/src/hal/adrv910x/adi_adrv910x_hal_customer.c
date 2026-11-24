/**
 * \file
 * \brief Template customer implementation of the ADRV910X HAL
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#include "adi_adrv910x_hal_customer.h"

int32_t customer_adi_adrv910x_hal_open(void *devHalCfg)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_adrv910x_hal_close(void *devHalCfg)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_adrv910x_hal_spi_write(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_adrv910x_hal_spi_read(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numTxRxBytes)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_adrv910x_hal_resetbPin_set(void *devHalCfg, uint8_t pinLevel)
{
    /* Customer code goes here */
    return 0;
}
