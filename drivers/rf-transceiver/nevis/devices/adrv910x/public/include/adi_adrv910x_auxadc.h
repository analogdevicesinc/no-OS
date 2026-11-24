/**
 * \file
 * \brief ADRV910X AUX ADC header file
 *
 * Copyright (c) 2025 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_AUXADC_H_
#define _ADI_ADRV910X_AUXADC_H_

#include "adi_adrv910x_auxadc_types.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_types.h"
#include "adi_common_error.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This function sets the configuration for AuxADCs.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] auxAdc    Selected AuxADC to be configured
 * \param[in] enable    True: Power up selected AuxADC, False: Power down selected AuxADC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxAdc_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxAdc_e auxAdc, bool enable);

/**
 * \brief This function gets the configuration of selected AuxADC
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  auxAdc    Select AuxADC to read back the configuration
 * \param[out] enabled   Read back the status; True:selected AuxADC is powered up, False:selected AuxADC is powered down
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxAdc_Inspect(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxAdc_e auxAdc, bool *enabled);

/**
 * \brief This function reads the ADC code of selected AuxADC and converts to mV.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x   Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  auxAdc     AuxADC selection to read ADC AuxADC
 * \param[out] auxAdc_mV  ADC value read in mV from ADRV910X device for the selected AuxADC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxAdc_Voltage_Get(adi_adrv910x_Device_t *adrv910x,
                                        adi_adrv910x_AuxAdc_e auxAdc,
                                        uint16_t *auxAdc_mV);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_AUXADC_H_ */
