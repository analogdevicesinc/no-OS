/**
 * \file
 * \brief ADRV9001 AUX ADC header file
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_AUXADC_H_
#define _ADI_ADRV9001_AUXADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_common_error.h"
#include "adi_adrv9001_auxadc_types.h"
#include "adi_adrv9001_types.h"

/**
 * \brief This function sets the configuration for AuxADCs.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001	     Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] auxAdc         Selected AuxADC to be configured
 * \param[in] enable         True: Power up selected AuxADC, False: Power down selected AuxADC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_AuxAdc_Configure(adi_adrv9001_Device_t *adrv9001,
                                      adi_adrv9001_AuxAdc_e auxAdc,
                                      bool enable);

/**
 * \brief This function gets the configuration of selected AuxADC
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	     Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  auxAdc        Select AuxADC to read back the configuration
 * \param[out] enabled       Read back the status; True:selected AuxADC is powered up, False:selected AuxADC is powered down
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_AuxAdc_Inspect(adi_adrv9001_Device_t *adrv9001,
                                    adi_adrv9001_AuxAdc_e auxAdc,
                                    bool *enabled);

/**
 * \brief This function reads the ADC code of selected AuxADC and converts to mV.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	     Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  auxAdc        AuxADC selection to read ADC AuxADC
 * \param[out] auxAdc_mV     ADC value read in mV from ADRV9001 device for the selected AuxADC
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_AuxAdc_Voltage_Get(adi_adrv9001_Device_t *adrv9001,
                                        adi_adrv9001_AuxAdc_e auxAdc,
                                        uint16_t *auxAdc_mV);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_AUXADC_H_ */
