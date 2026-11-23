/**
 * \file
 * \brief Hardware abstraction layer (HAL) for interacting with the ADRV910X
 *
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_ADRV910X_HAL_H_
#define _ADI_ADRV910X_HAL_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup adrv910x_hal "ADRV910X HAL"
 * The ADRV910X HAL consists of pointers to functions which must be implemented by the user, such that the ADRV910X API
 * can call said functions to interact with the chip without having to worry about hardware details.
 * 
 * For an example implementation, see adi_adrv910x_hal_linux_uio.c.
 * @{
 */

/**
 * \brief Open and/or prepare any resources associated with the HAL implementation
 *
 * \note This is only required because of quirks with the linux_uio ADI evaluation platform - specifically LVDS
 * and CMOS configurations use separate FPGA binaries and switching between the two requires reopening the SPI device
 * driver. This is expected to be removed in the future.
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV910X API functions.
 */
extern int32_t(*adi_adrv910x_hal_open)(void *devHalCfg);

/**
 * \brief Close and/or shutdown any resources associated with the HAL implementation
 *
 * \note This is only required because of quirks with the linux_uio ADI evaluation platform - specifically LVDS
 * and CMOS configurations use separate FPGA binaries and switching between the two requires reopening the SPI device
 * driver. This is expected to be removed in the future.
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV910X API functions.
 */
extern int32_t(*adi_adrv910x_hal_close)(void *devHalCfg);
	
/**
 * \brief Write data to the ADRV910X via SPI
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] txData        The data to be written
 * \param[in] numTxBytes    The number of bytes to write
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV910X API functions.
 */
extern int32_t(*adi_adrv910x_hal_spi_write)(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes);

/**
 * \brief Read data from the ADRV910X via SPI
 * 
 * \param[in]  devHalCfg    User-defined context variable
 * \param[in]  txData       The data to be written (register addresses to read from)
 * \param[out] rxData       The data which was read
 * \param[in]  numRxBytes   The number of bytes to read
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV910X API functions.
 */
extern int32_t(*adi_adrv910x_hal_spi_read)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numRxBytes);

/**
 * \brief Set the level of the RESETB pin
 * 
 * The ADRV910X can be reset by bringing the RESETB pin low, then high. This function is called by adi_adrv910x_HwReset
 * which controls the timing.
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] pinLevel      The logic level (0 = low; 1 = high) to which to set the RESETB pin
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV910X API functions.
 */
extern int32_t(*adi_adrv910x_hal_resetbPin_set)(void *devHalCfg, uint8_t pinLevel);

/** @} */
 
#ifdef __cplusplus
}
#endif

#endif
