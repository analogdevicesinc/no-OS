/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_gpio.h
* \brief Contains ADRV9025 transmit related function prototypes for
*        adrv9025_gpio.c
*
* ADRV9025 API Version: 6.4.0.14
*
* Copyright 2015-2019 Analog Devices Inc.
* Released under the AD9378-AD9379 API license, for more information see the "LICENSE.pdf" file.
*/

#ifndef _ADRV9025_GPIO_H_
#define _ADRV9025_GPIO_H_

#include "adi_adrv9025_types.h"
#include "adi_adrv9025_gpio_types.h"
#include "adi_common_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief [TOK A0] Private function called by API's adi_adrv9025_GpInt1Handler and adi_adrv9025_GpInt0Handler.  This
* function performs all possible recover actions for any GP_INT source as well as returning the required
* BBIC recovery action. This handles GP Interrupts specific to A0 silicon
*
* \pre This function can only be called by the adi_adrv9025_GpInt1Handler or adi_adrv9025_GpInt0Handler.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpIntStatus Pointer containing all information for the gp interrupt.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_ERR_RERUN_INIT_CALS Recovery actions for INIT CALIBRATION errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_GpIntHandler(adi_adrv9025_Device_t*      device,
                              adi_adrv9025_gpIntStatus_t* gpIntStatus);

/**
* \brief [TOK B0] Private function called by API's adi_adrv9025_GpInt1Handler and adi_adrv9025_GpInt0Handler.  This
* function performs all possible recover actions for any GP_INT source as well as returning the required
* BBIC recovery action.  This handles GP Interrupts specific to B0 silicon
*
* \pre This function can only be called by the adi_adrv9025_GpInt1Handler or adi_adrv9025_GpInt0Handler.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param gpIntStatus Pointer containing all information for the gp interrupt.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_ERR_RERUN_INIT_CALS Recovery actions for INIT CALIBRATION errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_b0_GpIntHandler(adi_adrv9025_Device_t*      device,
                                 adi_adrv9025_gpIntStatus_t* gpIntStatus);

/**
* \brief This function performs range check on the parameters for adi_adrv9025_AuxDacCfg_t.
*
* This function verifies that the requested configuration for AuxDAC is valid
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacCfg Pointer to the configuration struct to be tested
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxDacCfgRangeCheck(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_AuxDacCfg_t* auxDacCfg);

/**
* \brief This function sets the configuration for selected AuxDAC.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacSel AuxDAC selection to set the configuration
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxDacCfgSet(adi_adrv9025_Device_t* device,
                              adi_adrv9025_AuxDacs_e auxDacSel);

/**
* \brief This function sets the 12 bit DAC word for selected AuxDAC.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param auxDacSel AuxDAC selection to set the configuration
* \param dacWord 12 bit DAC word 0-4095 maps to 0-1.8V
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_CHECK_TIMER Recovery action for timer time-out check required
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxDacValueSet(adi_adrv9025_Device_t* device,
                                adi_adrv9025_AuxDacs_e auxDacSel,
                                uint16_t               dacWord);

/**
* \brief Private function called by API's adi_adrv9025_AuxAdcCfgSet function. This
* function validates the supplied configuration in terms of acceptable ranges
* and values.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcConfig Pointer to the auxiliary ADC configuration structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxAdcCfgRangeCheck(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_AuxAdcCfg_t* auxAdcConfig);

/**
* \brief Private function called by API's adi_adrv9025_AuxAdcCfgSet function. This
* function writes the supplied configuration to the selected ADC's
* configuration registers
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcConfig Pointer to the auxiliary ADC configuration structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxAdcCfgWrite(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_AuxAdcCfg_t* auxAdcConfig);

/**
* \brief Private function called by API's adi_adrv9025_AuxAdcCfgGet function. This
* function writes the configuration to the selected ADC configuration registers
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcSelect The auxiliary ADC to obtain a sample from
* \param auxAdcConfig Pointer to the auxiliary ADC configuration structure
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxAdcCfgRead(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                               adi_adrv9025_AuxAdcCfg_t*   auxAdcConfig);

/**
* \brief Private function called by API's adi_adrv9025_AuxAdcMeasurementStart function.
* This function starts AuxADC measurement by toggling the reset bit for selected AuxADC
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcSelect The auxiliary ADC to start measurement
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxAdcMeasurementStart(adi_adrv9025_Device_t*      device,
                                        adi_adrv9025_AuxAdcSelect_e auxAdcSelect);

/**
* \brief Private function called by API's adi_adrv9025_AuxAdcValueGet function.
* This function reads the selected ADC sample value.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 data structure
* \param auxAdcSelect The auxiliary ADC to obtain a sample from
* \param auxAdcValue A pointer to the supplied value structure to populate with
*        the obtained ADC sample data
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_AuxAdcValueGet(adi_adrv9025_Device_t*      device,
                                adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                adi_adrv9025_AuxAdcValue_t* auxAdcValue);

#ifdef __cplusplus
}
#endif

#endif /* _ADRV9025_GPIO_H_ */
