/**
* \file
* \brief Hardware abstraction layer (HAL) for interacting with the ADRV9001 transceiver
*
* Copyright 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_HAL_H_
#define _ADI_ADRV9001_HAL_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup adrv9001_hal "ADRV9001 HAL"
 * The ADRV9001 HAL consists of pointers to functions which must be implemented by the user, such that the ADRV9001 API
 * can call said functions to interact with the chip without having to worry about hardware details.
 * 
 * For an example implementation, see adi_adrv9001_hal_linux_uio.c.
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
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_adrv9001_hal_open)(void *devHalCfg);

/**
 * \brief Close and/or shutdown any resources associated with the HAL implementation
 *
 * \note This is only required because of quirks with the linux_uio ADI evaluation platform - specifically LVDS
 * and CMOS configurations use separate FPGA binaries and switching between the two requires reopening the SPI device
 * driver. This is expected to be removed in the future.
 *
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_adrv9001_hal_close)(void *devHalCfg);

/**
 * \brief Write data to the ADRV9001 via SPI
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] txData        The data to be written
 * \param[in] numTxBytes    The number of bytes to write
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_SpiWrite)(void *devHalCfg, const uint8_t txData[], uint32_t numTxBytes);

/**
 * \brief Read data from the ADRV9001 via SPI
 * 
 * \param[in]  devHalCfg    User-defined context variable
 * \param[in]  txData       The data to be written (register addresses to read from)
 * \param[out] rxData       The data which was read
 * \param[in]  numRxBytes   The number of bytes to read
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_SpiRead)(void *devHalCfg, const uint8_t txData[], uint8_t rxData[], uint32_t numRxBytes);

/**
 * \brief Set the level of the RESETB pin
 * 
 * The ADRV9001 can be reset by bringing the RESETB pin low, then high. This function is called by adi_adrv9001_HwReset
 * which controls the timing.
 * 
 * \param[in] devHalCfg     User-defined context variable
 * \param[in] pinLevel      The logic level (0 = low; 1 = high) to which to set the RESETB pin
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_adrv9001_hal_resetbPin_set)(void *devHalCfg, uint8_t pinLevel);

/**
 * \addtogroup adrv9001_hal_files "File I/O"
 * Historically, the ADRV9001 evaluation software stack transferred certain "resource" files over SFTP to the Linux
 * filesystem on the platform. However, production platforms may not have a proper filesystem. These functions
 * allow a user to load resources in chunks (from flash storage, for example) to conserve program/data memory. The path
 * arguments allow you to set all of these function pointers to the same function while still differentiating between
 * resources.
 * 
 * Sample C code generated from the Transceiver Evaluation Software saves resources as variables on the stack and does
 * not call these functions.
 * @{
 */

/**
 * \brief Retrieve a page of the ARM firmware binary
 * 
 * \param[in]  devHalCfg        User-defined context variable
 * \param[in]  armImagePath     The "file path" (or generally a string identifier) of the firmware binary
 * \param[in]  pageIndex        The page of the file to retrieve
 * \param[in]  pageSize         The size of the page to retrieve in bytes
 * \param[out] rdBuff           The retrieved binary page data
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_ArmImagePageGet)(void *devHalCfg, 
                                         const char *armImagePath, 
                                         uint32_t pageIndex, 
                                         uint32_t pageSize, 
                                         uint8_t *rdBuff);

/**
 * \brief Retrieve a page of the stream binary
 * 
 * \param[in]  devHalCfg        User-defined context variable
 * \param[in]  streamImagePath  The "file path" (or generally a string identifier) of the stream binary
 * \param[in]  pageIndex        The page of the file to retrieve
 * \param[in]  pageSize         The size of the page to retrieve in bytes
 * \param[out] rdBuff           The retrieved binary page data
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_StreamImagePageGet)(void *devHalCfg, 
                                            const char *streamImagePath, 
                                            uint32_t pageIndex, 
                                            uint32_t pageSize, 
                                            uint8_t *rdBuff);

/**
 * \brief Retrieve a row of the Rx or ORx gain table
 * 
 * \param[in]  devHalCfg        User-defined context variable
 * \param[in]  rxGainTablePath  The "file path" (or generally a string identifier) of the gain table
 * \param[in]  lineCount        The row of the table to retrieve
 * \param[out] gainIndex        The gain index of the row
 * \param[out] rxFeGain         The front end gain
 * \param[out] tiaControl       The TIA control word
 * \param[out] adcControl       The ADC control word
 * \param[out] extControl       The ext control word
 * \param[out] phaseOffset      The phase offset
 * \param[out] digGain          The digital gain
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_RxGainTableEntryGet)(void *devHalCfg, 
                                             const char *rxGainTablePath, 
                                             uint16_t lineCount, 
                                             uint8_t *gainIndex, 
                                             uint8_t *rxFeGain, 
                                             uint8_t *tiaControl, 
                                             uint8_t *adcControl, 
                                             uint8_t *extControl, 
                                             uint16_t *phaseOffset, 
                                             int16_t *digGain);

/**
 * \brief Retrieve a row of the Tx attenuation table
 * 
 * \param[in]  devHalCfg        User-defined context variable
 * \param[in]  txAttenTablePath The "file path" (or generally a string identifier) of the attenuation table
 * \param[in]  lineCount        The row of the table to retrieve
 * \param[out] attenIndex       The atten index of the row
 * \param[out] txAttenHp        The hp control word
 * \param[out] txAttenMult      The mult control word
 * 
 * \returns 0 to indicate success, negative values to indicate error. Specific error codes are defined by the user
 * implementation and are simply returned by ADRV9001 API functions.
 */
extern int32_t(*adi_hal_TxAttenTableEntryGet)(void *devHalCfg, 
                                              const char *txAttenTablePath, 
                                              uint16_t lineCount, 
                                              uint16_t *attenIndex, 
                                              uint8_t *txAttenHp, 
                                              uint16_t *txAttenMult);

/** @} */

/** @} */
 
#ifdef __cplusplus
}
#endif

#endif
