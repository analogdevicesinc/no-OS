/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_rx_types.h
*
* \brief Contains ADRV904X data types for on board cpus feature
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADRV904X_RX_TYPES_H_
#define _ADRV904X_RX_TYPES_H_

#include "adi_adrv904x_rx_types.h"

/**
*  \brief Data structure for Rx Carrier decimated power block configuration
*/
typedef struct adrv904x_RxCarrierDecPowerCfg
{
    adi_adrv904x_RxCarrierMask_t rxCarrierMask;               /*!< Rx Channel/Carrier Decimated Power Meter Mask selection */
    uint8_t                      measEnable;                  /*!< Enable Rx Carrier Dec Power measurements */
    uint8_t                      inputSelect;                 /*!< Measurement point selection for an Rx Carrier. 0: Channel Filter Input 1: Channel Filter Output 2: Band Data */
    uint8_t                      powerMeasurementDuration;    /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
    uint16_t                     dpdInOutPwrMeasContDlyCntr;  /*!< When in TDD mode (tddModeEnable = 1), wait before continuing with power measurement after RxOn */
    uint8_t                      peakToPowerMode;             /*!< Enable peak-to-power mode. When enabled, the largest peak (I^2+Q^2) within a measurement duration is available for readback */
} adrv904x_RxCarrierDecPowerCfg_t;


#endif /* _ADRV904X_RX_TYPES_H_ */

