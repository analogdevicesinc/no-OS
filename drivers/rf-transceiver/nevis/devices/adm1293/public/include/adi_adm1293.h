/**
 * \file
 * \brief Functions for interacting with the power monitoring chips on the CE board
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADM1293_H_
#define _ADI_ADM1293_H_

#include "adi_adm1293_types.h"


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
 * \brief Configure the power monitoring chips for operation
 *
 * \note This function must be called once before calling adi_adrv910x_PowerMonitor_Read()
 *
 * \param[in] adm1293  Context variable - Pointer to the ADM1293 device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adm1293_Configure(adi_adm1293_Device_t *adm1293);

/**
 * \brief Read the voltage and current from the specified power monitor
 *
 * \param[in]  adm1293             Context variable - Pointer to the ADM1293 device data structure
 * \param[in]  powerMonitor        The power monitoring chip from which to read the voltage and current
 * \param[out] measuredVoltage_uV  The measured voltage, denoted in microvolts (uV)
 * \param[out] measuredCurrent_uA  The measured current, denoted in microamperes (uA)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adm1293_Read(adi_adm1293_Device_t *adm1293,
                         adi_adrv910x_PowerMonitor_e powerMonitor,
                         uint32_t *measuredVoltage_uV,
                         uint32_t *measuredCurrent_uA);

/**
 * \brief Read the voltage from the specified power monitor
 *
 * \param[in]  adm1293             Context variable - Pointer to the ADM1293 device data structure
 * \param[in]  powerMonitor        The power monitoring chip from which to read the voltage
 * \param[out] measuredVoltage_uV  The measured voltage, denoted in microvolts (uV)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adm1293_VoltageRead(adi_adm1293_Device_t *adm1293,
                                adi_adrv910x_PowerMonitor_e powerMonitor,
                                uint32_t *measuredVoltage_uV);

/**
 * \brief Read the current from the specified power monitor
 *
 * \param[in]  adm1293             Context variable - Pointer to the ADM1293 device data structure
 * \param[in]  powerMonitor        The power monitoring chip from which to read the current
 * \param[out] measuredCurrent_uA  The measured current, denoted in microamperes (uA)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adm1293_CurrentRead(adi_adm1293_Device_t *adm1293,
                                adi_adrv910x_PowerMonitor_e powerMonitor,
                                uint32_t *measuredCurrent_uA);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADM1293_H_ */
