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
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_
#define _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_

#include "adi_adrv904x_carrier_reconfigure_types.h"
#include "adi_adrv904x_types.h"
#include "adrv904x_cpu_device_profile_jesd_types.h"

#define ADRV904X_MAX_NO_OF_JESD_SLOTS           (32U)
#define ADRV904X_NO_OF_CARRIER_DELAY_FIFOS      (8U)
#define ADRV904X_NO_OF_JESD_CARRIER_SLOTS       (64U)
#define ADRV904X_NO_OF_BANDS                    (2U)
#define ADRV904X_MAX_CARRIER_DELAY_VALUE        (1020)
#define ADRV904X_CARRIER_DELAY_ELEMENT_SIZE     (255)
#define ADRV904X_NO_OF_HALF_BAND_DELAYS         (5U)
#define ADRV904X_JESD_IQ_RATE_KHZ               (30720U)

/**
*  \brief Holds calculated delay parameters that get written to device after reconfig
*/
typedef struct adrv904x_CarrierReconfigDelayParams
{
    uint16_t delayCmpEnable;                                            /*!< Delay compensation enable */
    uint16_t delayMemEnable;                                            /*!< Delay mem enable */
    uint16_t daisyChainEnable;                                          /*!< Daisy chain enable */
    uint16_t delayCarrierSelect[ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];    /*!< Delay carrier select */
    uint16_t delayValue[ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];            /*!< Delay value */
} adrv904x_CarrierReconfigDelayParams_t;

/**
*  \brief Holds internal values used only for calculation
*/
typedef struct adrv904x_CarrierDelayParameters
{    
    uint32_t jesdIfaceFrequencyKhz;                                     /*!< Jesd interface frequency */
    uint16_t ratioLog2;                                                 /*!< Ratio Log 2 */
    uint16_t slotTable[ADRV904X_MAX_NO_OF_JESD_SLOTS];                  /*!< Slot table */
    uint16_t delayCmpEnable;                                            /*!< Delay compensation enable */
    uint16_t memoryEnable;                                              /*!< Memory enable */
    uint16_t daisyChainEnable;                                          /*!< Daisy chain enable */
    uint16_t carrierSelect[ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];         /*!< Carrier select array */
    uint16_t delayValue[ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];            /*!< Delay values */
    uint16_t carrierDelay[ADI_ADRV904X_MAX_CARRIERS];                   /*!< Carrier delay values */
} adrv904x_CarrierDelayParameters_t;

/**
*  \brief Holds internal values used only for calculation
*/
typedef struct adrv904x_FilterDelayParams
{
    uint32_t tap;           /*!< Filter delay tap */
    uint32_t pipe;          /*!< Filter delay pip  */

} adrv904x_FilterDelayParams_t;

/**
*  \brief Holds internal values used only for calculation
*/
typedef struct adrv904x_InternalDelayParams
{
    uint32_t slotTable;                                 /*!< Internal slot table */
    uint32_t delayMatchEnable;                          /*!< Internaal delay match enable value */
    adrv904x_FilterDelayParams_t channelFilter;         /*!< Internal channel filter config */
    adrv904x_FilterDelayParams_t halfBandFilter;        /*!< Internal half band filter config */
    uint32_t resource;                                  /*!< Internal resource value */
} adrv904x_InternalDelayParams_t;

/**
*  \brief Holds internal values used only for calculation
*/
typedef struct adrv904x_ResourceShareDelayParams
{
    int32_t delay;                                      /*!< Resource share delay value */
    int32_t period;                                     /*!< Resrouce share delay period */ 
} adrv904x_ResourceShareDelayParams_t;

/**
 * \brief Holds internal jesd settings written to device after reconfig
 */
typedef struct adrv904x_CarrierJesdCfg
{
    uint16_t maxSlot;                                               /*!< JESD slot table max slot */
    uint8_t  jesdInterfaceClkDiv;                                   /*!<  JESD Interface Clk Div */
    uint64_t slotValidLower;                                        /*!< JESD slot valid lower word */
    uint64_t slotValidUpper;                                        /*!< JESD slot valid upper word */
    uint16_t slotTable[ADRV904X_NO_OF_JESD_CARRIER_SLOTS];          /*!< JESD slot table */
} adrv904x_CarrierJesdCfg_t;

/**
 * \brief Holds internal jesd values used only for calculation
 */
typedef struct adrv904x_CarrierJesdInternalCfg
{
    uint32_t frequencyKhz;                                          /*!< JESD interface frequency */
    uint16_t divide;                                                /*!< JESD interface divide value */
    uint16_t numSlots;                                              /*!< JESD slot table number of slots */
    uint16_t ifaceMaxSlot;                                          /*!< JESD interface max slot */
    uint16_t initSlot;                                              /*!< JESD interface initial slot */
    uint16_t ifaceSlotTable[ADRV904X_MAX_NO_OF_JESD_SLOTS];         /*!< JESD interface slot table */
    uint64_t slotValid;                                             /*!< JESD slot valid */
    uint32_t jesdSampleRate_kHz[ADI_ADRV904X_MAX_CARRIERS];         /*!<   JESD interface sample rate */
} adrv904x_CarrierJesdInternalCfg_t;

typedef struct JesdComponentCarrierXbarOutput
{
    adrv904x_JesdCarrierXBarSelect_t        carrierXbarSelect;  /*!< Carrier XBar settings (Written to device) */
    adi_adrv904x_JesdComponentCarrierXbar_e carrierSelect;      /*!< Carrier select (Internal use only) */
} JesdComponentCarrierXbarOutput_t;

typedef struct adrv904x_LinkCfg
{
    JesdComponentCarrierXbarOutput_t    jesdCfg[ADI_ADRV904X_MAX_CARRIER_SLOTS];    /*!< Carrier XBar settings (Written to device) */
} adrv904x_LinkCfg_t;
/**
 * \brief Internal struct to hold carrier jesd settings
 */
typedef struct adrv904x_CarrierDynamicReconfigJesdCfg
{    
    adrv904x_LinkCfg_t                  linkCfg[ADI_ADRV904X_MAX_CARRIER_LINKS];        /*!< Holds JESD settings if required based on reconfig settings */
} adrv904x_CarrierDynamicReconfigJesdCfg_t;

/**
*  \brief Internal struct used to store settings from initialization
*/
typedef struct adrv904x_CarrierInitialCfg
{
    adi_adrv904x_BandCfgExtract_t                   bandSettings[ADRV904X_NO_OF_BANDS]; /*!< Band settings */
    uint8_t                                         maxSlot;                            /*!< Initial maxSlot value */
} adrv904x_CarrierInitialCfg_t;

/**
*  \brief Internal struct used to hold profile specific data for both Tx & Rx
*/
typedef struct adrv904x_CarrierDynamicReconfigProfileCfg
{
    adrv904x_CarrierJesdInternalCfg_t           internalJesdCfg;        /*!< Internal JESD settings for carrier reconfig */
    adrv904x_CarrierJesdCfg_t                   jesdCfg;                /*!< Internal JESD settings for carrier reconfig written to device */
    adrv904x_CarrierInitialCfg_t                initialCfg;             /*!< Stores initial settings for each profile */
    adi_adrv904x_CarrierRuntime_t               carrierCfgs;            /*!< Array of settings for all possible carriers */
    adrv904x_CarrierReconfigDelayParams_t       delayCfg;               /*!< Holds internal delay settings written to part */
    uint16_t                                    band0Atten;             /*!< Band0 attenuation value. Only used for Tx */
    uint16_t                                    band1Atten;             /*!< Band1 attenuation value. Only used for Tx */
    uint32_t                                    carriersEnabled;        /*!< Bitmask for all enabled carriers */
} adrv904x_CarrierDynamicReconfigProfileCfg_t;

/**
*  \brief Internal struct used during dynamic carrier reconfiguration common for both Tx & Rx
*/
typedef struct adrv904x_CarrierDynamicReconfigInternalCfg
{
    adrv904x_CarrierDynamicReconfigProfileCfg_t   profileCfgs[ADI_ADRV904X_MAX_NUM_PROFILES];       /*!< Array of settings for all profiles */
    adrv904x_CarrierDynamicReconfigJesdCfg_t    carrierJesdCfg;                                     /*!< Holds internal jesd settings for carrier reconfig */
    adi_adrv904x_CarrierReconfigLatencyCfg_t    latencyCfg;                                         /*!< Internal use only: carrier reconfig latency */
} adrv904x_CarrierDynamicReconfigInternalCfg_t;

#endif /* _ADRV904X_CARRIER_RECONFIGURE_TYPES_H_ */

