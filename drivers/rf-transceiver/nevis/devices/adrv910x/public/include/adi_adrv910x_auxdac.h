/**
 * \file
 * \brief ADRV910X AUX DAC header file
 *
 * Copyright (c) 2025 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_AUXDAC_H_
#define _ADI_ADRV910X_AUXDAC_H_

#include "adi_adrv910x_auxdac_types.h"
#include "adi_adrv910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This function sets the configuration for AuxDACs.
 *
 * There are 4 AuxDACs at Adrv910x. AuxDAC0-3 outputs to GPIO Analog pin0-3 respectively.
 *
 * AuxDAC[n] | GPIO_ANA_n
 * -------   |-----------------------
 * AuxDAC0   | GPIO_ANA_0
 * AuxDAC1   | GPIO_ANA_1
 * AuxDAC2   | GPIO_ANA_2
 * AuxDAC3   | GPIO_ANA_3
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] auxDac    Selected AuxDAC to be configured
 * \param[in] enable    True:Enable selected AuxDAC, False:Disable selected AuxDAC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxDac_Configure(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxDac_e auxDac, bool enable);

/**
 * \brief This function gets the configuration of selected AuxDAC
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  auxDac    Select AuxDAC to read back the configuration
 * \param[out] enabled   Read back the status; True:selected AuxDAC is enabled, False:selected AuxDAC is disabled
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxDac_Inspect(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxDac_e auxDac, bool *enabled);

/**
 * \brief This function sets 12 bit DAC code of selected AuxDAC.
 *
 * There are 4 AuxDACs at Adrv910x. AuxDAC0-3 outputs to GPIO Analog pin0-3 respectively.
 *
 * AuxDAC[n] | GPIO_ANA_n
 * -------   |-----------------------
 * AuxDAC0   | GPIO_ANA_0
 * AuxDAC1   | GPIO_ANA_1
 * AuxDAC2   | GPIO_ANA_2
 * AuxDAC3   | GPIO_ANA_3
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] auxDac    Selected AuxDAC for which the DAC code to be set
 * \param[in] code      The desired 12-bit DAC code to be written
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxDac_Code_Set(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxDac_e auxDac, uint16_t code);

/**
 * \brief This function reads the DAC word of selected AuxDAC.
 *
 * There are 4 AuxDACs at Adrv910x. AuxDAC0-3 uses a GPIO Analog pin0-3 respectively.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  auxDac    AuxDAC selection to read DAC AuxDAC
 * \param[out] code      DAC code read from ADRV910X device for the selected AuxDAC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_AuxDac_Code_Get(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_AuxDac_e auxDac, uint16_t *code);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_AUXDAC_H_ */
