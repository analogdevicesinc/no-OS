/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_carrier_reconfigure_types.h
*
* \brief Contains ADRV904X data types for private carrier reconfigure features
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_
#define _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_

#include "adi_adrv904x_carrier_reconfigure_types.h"
#include "adi_adrv904x_types.h"

#define ADRV904X_NO_OF_RSD_CARRIER_SLOTS                (128U)
#define ADRV904X_MAX_CARRIER_DELAY_VALUE                (1020)
#define ADRV904X_CARRIER_DELAY_ELEMENT_SIZE             (255)
#define ADRV904X_NO_OF_HALF_BAND_DELAYS                 (5U)
#define ADRV904X_JESD_IQ_RATE_KHZ                       (30720U)
#define ADRV904X_MAX_CARRIER_VIRTUAL_CONVERTER_RATE     (31250U)
#define ADRV904X_MIN_CARRIER_VIRTUAL_CONVERTER_RATE     (ADRV904X_MAX_CARRIER_VIRTUAL_CONVERTER_RATE >> 1)
#define ADRV904X_HALF_MAX_DELAY_CC                      (4)
#define ADRV904X_MAX_DELAY_CC                           (ADRV904X_HALF_MAX_DELAY_CC << 1)
#define ADRV904X_SLOT_TABLE_UNUSED                      (128U)
#define ADI_LFSR_START_SEED                             (0x4321u)
#define ADI_SLOT_SHUFFLE_RAND_ITERATIONS                (50u)
#define ADI_SLOT_SHUFFLE_USE_ONLY_RAND                  (0u)
#define ADI_ADI_ENABLE_DELAY_MATCHING_LOG_PRINTS        (0u)


typedef struct adrv904x_CarrierJesdParameters
{
    uint32_t frequency_kHz;
    uint16_t divide;
    uint16_t numSlots;
    uint16_t maxSlot;
    uint16_t ifaceMaxSlot;
    uint16_t initSlot;
    uint16_t slotTable[ADI_ADRV904X_NO_OF_JESD_CARRIER_SLOTS];
    uint16_t ifaceSlotTable[ADI_ADRV904X_MAX_NO_OF_JESD_IFACE_SLOTS];
    uint64_t slotValid;
} adrv904x_CarrierJesdParameters_t;

typedef struct adrv904x_SlotTableShuffleParams
{
    uint16_t jesdSlotTable[ADI_ADRV904X_NO_OF_JESD_CARRIER_SLOTS]; //64
    uint16_t jesdMaxSlot;
    uint16_t jesdSlotValid[ADI_ADRV904X_NO_OF_JESD_CARRIER_SLOTS];
    uint16_t carrierSlots[8];
    uint16_t carriersEnabled;
    int16_t carrierDelayMismatch[ADI_ADRV904X_MAX_CARRIERS];

    uint16_t jesdClkPeriod;
    uint16_t carrierClkPeriod[ADI_ADRV904X_MAX_CARRIERS];

    uint16_t ifaceMaxSlots;
} adrv904x_SlotTableShuffleParams_t;

typedef struct adrv904x_ShuffleDiag
{
    uint8_t  enabled;
    uint8_t  converged;
    uint32_t procTime_us;
    uint32_t ziter;
    uint32_t miter;
    uint32_t citer;
    uint32_t move_iter;
} adrv904x_ShuffleDiag_t;

/**
*  \brief Holds internal values used only for calculation
*/
typedef struct adrv904x_CarrierResourceSharePrm
{
    uint32_t dlyPrev[ADI_ADRV904X_MAX_CARRIERS];            /*!< Resource share delay value */
    uint32_t clkPeriod[ADI_ADRV904X_MAX_CARRIERS];          /*!< Resrouce share delay period */
    uint8_t numCarriers;                                    /*!< Number of Carriers  */
} adrv904x_CarrierResourceSharePrm_t;

/**
*  \brief Internal struct used to store settings from initialization
*/
typedef struct adrv904x_CarrierInitialCfg
{
    adi_adrv904x_BandCfgExtract_t                   bandSettings[ADI_ADRV904X_NO_OF_BANDS]; /*!< Band settings */
    uint8_t                                         maxSlot;                                /*!< Initial maxSlot value */
} adrv904x_CarrierInitialCfg_t;

#endif /* _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_ */

