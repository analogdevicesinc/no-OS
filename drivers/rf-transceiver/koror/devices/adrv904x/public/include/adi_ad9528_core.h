/**
* Copyright 2020 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adi_ad9528.h
 * \brief Contains function declarations for AD9528 API
 */
#ifndef _ADI_AD9528_H_
#define _ADI_AD9528_H_

#include "adi_ad9528_types.h"

/**
* \brief Performs a Hardware Initialization for AD9528 Device.
*
* \pre This function may be called after device->common.devHalInfo has been initialized with
* user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_HwOpen(adi_ad9528_Device_t* const device);

/**
* \brief Performs a hardware shutdown for AD9528 Device
*
* \pre This function may be called after device->common.devHalInfo has been initialized with
* user values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_HwClose(adi_ad9528_Device_t* const device);

/**
*   \brief Service to perform a HwReset for a device
*
*   Call the HAL to send the HwReset Signal for a given device
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - is structure pointer to the AD9528 data structure containing 
*  devHalInfo
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_HwReset(adi_ad9528_Device_t* const device);

/**
* \brief Sets the AD9528 device SPI settings (3wire/4wire, msbFirst, etc).
*
* This function will use the settings in the adi_ad9528_SpiSettings_t structure
* to set SPI stream mode, address auto increment direction, msbFirst/lsbfirst,
* and 3wire/4wire mode.  The AD9528 device always uses SPI MODE 0 (CPHA=0, 
* CPOL=0), uses a 16bit instruction word, and defaults to SPI streaming (auto increment)
*
* <B>Dependencies</B>
* - device->common.devHalInfo
* - spi->msbFirst
* - spi->autoIncAddrUp
* - spi->fourWireMode
*
* \param[in,out] device Context variable - Pointer to the AD9528 data structure
* \param[in]     spi    Pointer to AD9528 SPI controller settings - not platform hardware SPI settings
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SpiCfgSet(adi_ad9528_Device_t* const              device,
                                                    const adi_ad9528_SpiSettings_t* const   spi);

/**
* \brief Gets the AD9528 device SPI settings (3wire/4wire, MSBFirst, etc).
*
* This function will read the current SPI settings and load them into the spi 
* input
* parameter. This function only works after the adi_ad9528_SpiCfgSet has been 
* called.
*
* <B>Dependencies</B>
* - device->common.devHalInfo
* - spi->msbFirst
* - spi->enSpiStreaming
* - spi->autoIncAddrUp
* - spi->fourWireMode
*
* \param[in,out] device Context variable - Pointer to the AD9528 data structure
* \param[out]    spi    Pointer to AD9528 SPI controller settings - not platform hardware SPI settings
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SpiCfgGet(adi_ad9528_Device_t* const      device,
                                                    adi_ad9528_SpiSettings_t* const spi);

/**
* \brief Initializes the AD9528 by writing all SPI registers
*
* Initializes the AD9528 using the values passed in through the 
* adi_ad9528_Init_t structure.
*
* <B>Dependencies</B>
* - All parameters in init structure
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - Pointer to AD9528 clock device structure
* \param[in]     init   Pointer to the AD9528 initialization settings
*  structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_Initialize(   adi_ad9528_Device_t* const      device,
                                                        const adi_ad9528_Init_t* const  init);

/**
* \brief Service to configure SPI SYSREF Request Bit
*
* \pre This function should be called after adi_ad9528_Initialize() or adi_ad9528_SysrefCfgSet()
*
* Note:
*   N-Shot Mode, SPI SYSREF request Bit is a self clearing bit
*   Continuous Mode, User is required to clear the SPI SYSREF request bit
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device         Context variable - Pointer to AD9528 clock device structure
* \param[in]     enableSysref   Flag to Set SPI SYSREF request Bit; 1: Set 0: Clear
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SysrefRequest(adi_ad9528_Device_t* const device,
                                                        const uint8_t              enableSysref);

/**
* \brief Update the Sysref settings for an AD9528 device by writing to the
*  corresponding configuration registers
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - Pointer to AD9528 clock device structure
* \param[in]     sysrefSettings Pointer to a Sysref settings device structure containing updated settings
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SysrefCfgSet( adi_ad9528_Device_t* const               device,
                                                        const adi_ad9528_SysrefSettings_t* const sysrefSettings);

/**
* \brief Retrieves the Sysref settings for an AD9528 device by reading to the
*  corresponding configuration registers
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - Pointer to AD9528 clock device structure
* \param[out]    sysrefSettings Pointer to a Sysref settings device structure which
*                               will be updated with read back values
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SysrefCfgGet( adi_ad9528_Device_t* const          device,
                                                        adi_ad9528_SysrefSettings_t* const  sysrefSettings);

/**
* \brief Update the AD9528 clock outputs that are enabled. The function probes
*  the AD9528 chip to determine the sysref settings.
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - is structure pointer to AD9528 clock device structure
* \param[in]     clkEnable Sets which clock outputs are enabled. Bit per clock output. bit0 = out0, etc.
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_ClockOutputsEnable(   adi_ad9528_Device_t* const  device,
                                                                const uint16_t              clkEnable);
/**
* \brief Service to Debounce the PLL Status Registers
*
* Waits until Reference A clock and VCXO clock input to be detected (present),
* and PLL1 and PLL2 to lock.
*
* <B>Dependencies</B>
* - device->common.devHalInfo
*
* \param[in,out] device Context variable - Pointer to AD9528 clock device structure
* \param[in]     timeout_ms the amount of time to wait for a successful lock
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_PllLockDebounce(adi_ad9528_Device_t* const device, const uint32_t timeout_ms);

/**
* \brief Performs a verification of AD9528 SPI functionality
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SpiVerify(adi_ad9528_Device_t* const device);

/**
* \brief Gets the Status Readback 0 of AD9528
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
* \param[out]    status status of Status Readback 0
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_StatusGet(adi_ad9528_Device_t* const device, adi_ad9528_Status_t* const status);

#endif /* _ADI_AD9528_H_ */
