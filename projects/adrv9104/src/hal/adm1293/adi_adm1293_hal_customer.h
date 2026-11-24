/**
 * \file
 * \brief Template customer implementation of the ADM1293 HAL
* 
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADM1293_HAL_CUSTOMER_H_
#define _ADI_ADM1293_HAL_CUSTOMER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* \copydoc adi_adm1293_hal_i2c_write
*/
int32_t customer_adi_adm1293_hal_i2c_write(void *devHalCfg, uint8_t slaveAddress, const uint8_t wrData[], uint32_t numWrBytes);

/**
* \copydoc adi_adm1293_hal_i2c_read
*/
int32_t customer_adi_adm1293_hal_i2c_read(void *devHalCfg, 
                                          uint8_t slaveAddress, 
                                          const uint8_t wrData[], 
                                          uint32_t numWrBytes, 
                                          uint8_t rdData[], 
                                          uint32_t numRdBytes);
#ifdef __cplusplus
}
#endif

#endif
