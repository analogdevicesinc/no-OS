/**
 * \file
 * \brief Template customer implementation of the ADM1293 HAL
* 
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adm1293_hal_customer.h"

int32_t customer_adi_adm1293_hal_i2c_write(void *devHalCfg, uint8_t slaveAddress, const uint8_t wrData[], uint32_t numWrBytes)
{
    /* Customer code goes here */
    return 0;
}

int32_t customer_adi_adm1293_hal_i2c_read(void *devHalCfg, 
                                          uint8_t slaveAddress, 
                                          const uint8_t wrData[], 
                                          uint32_t numWrBytes, 
                                          uint8_t rdData[], 
                                          uint32_t numRdBytes)
{
    /* Customer code goes here */
    return 0;
}