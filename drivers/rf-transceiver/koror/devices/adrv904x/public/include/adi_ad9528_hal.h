/**
* Copyright 2020 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_ad9528_hal.h
* \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
*        functions implemented in adi_ad9528_hal.c
*/

#ifndef _ADI_AD9528_HAL_H_
#define _ADI_AD9528_HAL_H_

#include "adi_ad9528_types.h"

/**
* \brief Performs a AD9528 SPI Byte Write
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
* \param[in]     addr   Register Address
* \param[in]     data   Data to be Written to the Register
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SpiByteWrite( adi_ad9528_Device_t* const  device,
                                                        const uint16_t              addr,
                                                        const uint8_t               data);

/**
* \brief Performs a AD9528 SPI Byte Read
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to AD9528 device data structure
* \param[in]     addr   Register Address
* \param[out]    data   Pointer for Data Read
*
* \retval adi_common_ErrAction_e, ADI_COMMON_ERR_ACT_NONE if successful
*/
ADI_API adi_common_ErrAction_e adi_ad9528_SpiByteRead(  adi_ad9528_Device_t* const  device,
                                                        const uint16_t              addr,
                                                        uint8_t* const              data);

#endif /* ADI_AD9528_HAL_H_ */
