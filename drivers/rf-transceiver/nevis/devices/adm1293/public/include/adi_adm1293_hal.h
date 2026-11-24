/**
 * \file
 * \brief Hardware abstraction layer (HAL) for interacting with the ADM1293 digital power monitor. The ADM1293 monitors
 *        current and two voltages and is configurable via a PMBus interface (i2c). See the datasheet
 *        (https://www.analog.com/media/en/technical-documentation/data-sheets/ADM1293_1294.pdf) for details.
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADM1293_HAL_H_
#define _ADI_ADM1293_HAL_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup adm1293_hal "ADM1293 HAL"
 * The ADM1293 HAL consists of pointers to functions which must be implemented by the user, such that the ADM1293 API
 * can call said functions to interact with the chip without having to worry about hardware details.
 * For an example implementation, see adi_adm1293_hal_linux_uio.c.
 * @{
 */

/**
 * \brief Write data to the ADM1293 via i2c
 *
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] slaveAddress  The i2c slave address of the ADM1293 chip
 * \param[in] wrData        The data to be written
 * \param[in] numWrBytes    The number of bytes to write
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by ADM1293 API functions.
 */
extern int32_t(*adi_adm1293_hal_i2c_write)(void *devHalCfg,
                                           uint8_t slaveAddress,
                                           const uint8_t wrData[],
                                           uint32_t numWrBytes);

/**
 * \brief Read data from the ADM1293 via i2c
 *
 * \param[in]  devHalCfg     User-defined context variable
 * \param[in]  slaveAddress  The i2c slave address of the ADM1293 chip
 * \param[in]  wrData        The data to be written (command codes or register addresses to read from)
 * \param[in]  numWrBytes    The number of bytes to write
 * \param[out] rdData        The data which was read
 * \param[in]  numRdBytes    The number of bytes to read
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user implementation and are simply returned by ADM1293 API functions.
 */
extern int32_t(*adi_adm1293_hal_i2c_read)(void *devHalCfg,
                                          uint8_t slaveAddress,
                                          const uint8_t wrData[],
                                          uint32_t numWrBytes,
                                          uint8_t rdData[],
                                          uint32_t numRdBytes);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADM1293_HAL_H_ */
