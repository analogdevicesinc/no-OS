/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_carrier_reconfigure_types.h
 *
 * \brief   Contains ADRV904X Carrier Reconfigure data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_H__
#define __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_H__

#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_carrier_reconfigure_common_types.h"

/**
 *  \brief Max number of carrier configurations allowed
 */
#define ADI_ADRV904X_MAX_NUM_PROFILES                           (4U)
#define ADI_ADRV904X_MAX_NO_OF_JESD_IFACE_SLOTS                 (16U)
#define ADI_ADRV904X_MIN_NO_OF_JESD_IFACE_SLOTS                 (8U)
#define ADI_ADRV904X_NO_OF_JESD_CARRIER_SLOTS                   (64U)
#define ADI_ADRV904X_NO_OF_CARRIER_DELAY_FIFOS                  (8U)
#define ADI_ADRV904X_NO_OF_CHAN_FILTER_DATA_PIPE_TIERS          (3U)
#define ADI_ADRV904X_NO_OF_BANDS                                (2U)
    
/**
 * \brief Crossbar mux to route sample through component carrier chain. Only first M values are valid and used
 */
typedef struct adi_adrv904x_JesdComponentCarrierXbarOutput
{
    uint8_t                                 channelSelect;          /*!< Rx/Tx slice selection */
    adi_adrv904x_JesdComponentCarrierXbar_e carrierSelect;          /*!< Carrier select*/
    adi_adrv904x_JesdComponentCarrierXbar_e slotSelect;             /*!< Carrier slot selection */
} adi_adrv904x_JesdComponentCarrierXbarOutput_t;


/**
 * \brief Struct to hold calculated output carrier jesd link settings
 */
typedef struct adi_adrv904x_LinkCfgOut
{
    adi_adrv904x_JesdComponentCarrierXbarOutput_t   jesdCfg[ADI_ADRV904X_MAX_CARRIER_SLOTS];
} adi_adrv904x_LinkCfgOut_t;
    
/**
 * \brief Struct to hold calculated output carrier jesd settings
 */
typedef struct adi_adrv904x_CarrierReconfigJesdOut
{    
    adi_adrv904x_LinkCfgOut_t           linkCfg[ADI_ADRV904X_MAX_CARRIER_LINKS];
} adi_adrv904x_CarrierReconfigJesdOut_t;


/**
 * \brief Holds jesd values used only for calculation
 */
typedef struct adi_adrv904x_CarrierJesdInternalCfg
{
    uint32_t frequencyKhz;                                              /*!< JESD interface frequency */
    uint16_t divide;                                                    /*!< JESD interface divide value */
    uint16_t numSlots;                                                  /*!< JESD carrier slot table number of carrier slots. Support up to 64 */
    uint16_t initSlot;                                                  /*!< JESD interface initial slot to use in carrier slot table. With Koror HW, initslot is always 0 */
    uint16_t maxSlot;                                                   /*!< JESD interface max slot to use in carrier slot table: maxSlot = initSlot + numSlots. With Koror HW, initslot is always 0, so maxSlot = numSlots */
    uint16_t ifaceMaxSlot;                                              /*!< JESD interface max slot. Supprts only 8 or 16. */
    uint16_t slotTable[ADI_ADRV904X_NO_OF_JESD_CARRIER_SLOTS];          /*!< JESD carrier slot table. Size 64 */
    uint16_t ifaceSlotTable[ADI_ADRV904X_MAX_NO_OF_JESD_IFACE_SLOTS];   /*!< JESD interface slot table. Size 16 */
    uint64_t slotValid;                                                 /*!< JESD slot valid. 64bit value. Each bit associated with a carrier slot 0-63 */
    uint32_t jesdSampleRate_kHz[ADI_ADRV904X_MAX_CARRIERS];             /*!< JESD interface sample rate */
    uint32_t dummyIfaceSlotsRemoved[ADI_ADRV904X_MAX_CARRIERS];         /*!< Counter used during shuffling to temporarily remove dummy slots during the process */
} adi_adrv904x_CarrierJesdInternalCfg_t;

typedef struct adi_adrv904x_CarrierFilterDelay
{
    uint32_t tap;                                                       /*!< Filter Tap Delay */
    uint32_t pipe;                                                      /*!< Filter Pipe Delay */
} adi_adrv904x_CarrierFilterDelay_t;

typedef struct adi_adrv904x_CarrierGroupDelayComp
{
    adi_adrv904x_CarrierFilterDelay_t cfilt;                            /*!< Channel Filter Delay values */
    adi_adrv904x_CarrierFilterDelay_t halfBand;                         /*!< Half-band Filter Delay values */
    uint32_t deinterleaver;                                             /*!< Inherent delay in Slot Table Deinterleaver position */
    uint32_t gainResrc;                                                 /*!< Shared Resource Delay */
    uint32_t matchEnabled;                                              /*!< Delay due to enabling on-chip Delay matching HW (Delay Buffer FIFOs) */
    uint32_t bandAlignDelay;                                            /*!< Delay due to band clk alignment */
} adi_adrv904x_CarrierGroupDelayComp_t;

typedef struct adi_adrv904x_CarrierHwDelayBufferConfig
{
    uint16_t carrierSelect[ADI_ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];     /*!< HW Carrier select for each Delay FIFO in HW */
    uint16_t delayValue[ADI_ADRV904X_NO_OF_CARRIER_DELAY_FIFOS];        /*!< HW Delay buffers counts per carrier (offset_carr_samples) translated to Delay FIFO values in HW*/
    uint16_t cmpEn;                                                     /*!< HW cmpEn HW setting */
    uint16_t memEn;                                                     /*!< HW memEn HW setting */
    uint16_t daisyEn;                                                   /*!< HW daisyEn HW setting */
} adi_adrv904x_CarrierHwDelayBufferConfig_t;

/* These are the parameters that are used to calculate delays within the CDUC or CDDC, add to Band delay, and to determine the Delay Buffer Values to apply */
typedef struct adi_adrv904x_CarrierDelayParameters
{
    uint16_t noOfCarriers;
    uint32_t jesdFrequency_kHz;
    uint16_t clkToJesdRatioLog2;
    uint16_t carrSamplePeriod_cc[ADI_ADRV904X_MAX_CARRIERS];                            /*!< Carrier sample period in units of hsdig cc's */
    adi_adrv904x_CarrierGroupDelayComp_t carrierComponents[ADI_ADRV904X_MAX_CARRIERS];  /*!< Used to track CDUC/CDDC internal component delays. Not all fields applicable to both */
    uint16_t groupDelayBand_cc[ADI_ADRV904X_MAX_CARRIERS];                              /*!< Inherent group delay per carrier in its assigned Band DUC/DDC alone */
    uint16_t groupDelayCarr_cc[ADI_ADRV904X_MAX_CARRIERS];                              /*!< Inherent group delay per carrier in the CDUC or CDDC alone */
    uint16_t groupDelayComb_cc[ADI_ADRV904X_MAX_CARRIERS];                              /*!< Combined group delay per carrier: groupDelayBand_cc + groupDelayCarr_cc*/
    uint16_t targetDelay_cc[ADI_ADRV904X_MAX_CARRIERS];                                 /*!< Target delay per carrier */
    uint16_t uncompOffset_cc[ADI_ADRV904X_MAX_CARRIERS];                                /*!< Initial/uncompensated offset (e.g. w/0 integer sample delay buffers) to add to combined group delay to match target: offset_ideal_cc[i] = targetDelay_cc[i] - groupDelayComb_cc[i]  */
    uint16_t compIntSamples[ADI_ADRV904X_MAX_CARRIERS];                                 /*!< Integer number of Carrier Sample Periods to use in Delay Compensation per carrier */
    uint16_t comp_cc[ADI_ADRV904X_MAX_CARRIERS];                                        /*!< Delay compensation provided by compIntSamples buffers in units of hsdig cc's: compIntSamples[i] * carrSamplePeriod_cc[i] */
    uint16_t finalDelayTotal_cc[ADI_ADRV904X_MAX_CARRIERS];                             /*!< Final Overall Delays that sum Band DUC/DDC + CDUC/CDDC grp delay + delay elements. groupDelayComb_cc + offset_actual_cc  */
    uint16_t finalDelayCarr_cc[ADI_ADRV904X_MAX_CARRIERS];                              /*!< Final delay through the CDUC/CDDC alone, including delay elements: finalDelayTotal_cc - groupDelayBand_cc */
    uint32_t finalDelayTotal_ns[ADI_ADRV904X_MAX_CARRIERS];                             /*!< Final Overall Delays that sum Band DUC/DDC + CDUC/CDDC grp delay + delay elements. finalDelayTotal_cc[i] * T_hsdigclk_ns */
    uint32_t finalDelayCarr_ns[ADI_ADRV904X_MAX_CARRIERS];                              /*!< Final delay through the CDUC/CDDC alone, including delay elements: finalDelayCarr_cc[i] * T_hsdigclk_ns */
    int16_t  delayDiffPerCarrier_cc[ADI_ADRV904X_MAX_CARRIERS];                         /*!< Delta between target delay and final delay per carrier: targetDelay_cc - finalDelayTotal_cc (Check order/sign) */
    int16_t  delayMismatch_cc;                                                          /*!< Largest error between any 2 carriers: max(delayDiffPerCarrier_cc[]) - min(delayDiffPerCarrier_cc[]) */
    adi_adrv904x_CarrierHwDelayBufferConfig_t bufferCfg;                                /*!< HW configuration to apply to Delay Buffers */
} adi_adrv904x_CarrierDelayParameters_t;

/**
*  \brief Struct used to hold profile specific output data for both Tx & Rx
*/
typedef struct adi_adrv904x_CarrierReconfigProfileCfgOut
{
    adi_adrv904x_CarrierJesdInternalCfg_t       internalJesdCfg;                /*!< Internal JESD settings for carrier reconfig */
    adi_adrv904x_CarrierRuntime_t               carrierCfgs;                    /*!< Array of settings for all possible carriers */
    adi_adrv904x_CarrierDelayParameters_t       delayCfg;                       /*!< Holds internal delay settings written to part */
    uint16_t                                    band0Atten;                     /*!< Band0 attenuation value. Only used for Tx */
    uint16_t                                    band1Atten;                     /*!< Band1 attenuation value. Only used for Tx */
    uint32_t                                    carriersEnabled;                /*!< Bitmask for all enabled carriers */
} adi_adrv904x_CarrierReconfigProfileCfgOut_t;
    
/**
 *  \brief Holds stored Input values from a ReconfigSolve() so that they can be used on next ReconfigApply()
 */
typedef struct adi_adrv904x_CarrierReconfigInput
{
    adi_adrv904x_CarrierJesdCfg_t               jesdCfg;                                    /*!< Copy of input to Solve() function */
    uint32_t                                    numProfiles;                                /*!< Copy of input to Solve() function. Number of profiles that are used. Only first N=numCarrierProfiles are populated */
    adi_adrv904x_CarrierRadioCfg_t              profileCfgs[ADI_ADRV904X_MAX_NUM_PROFILES]; /*!< Copy of input to Solve() function, but enough memory for all 4 profiles */
} adi_adrv904x_CarrierReconfigInput_t;


/**
 *  \brief Holds calculated latency values for a single Tx or Rx Channel
 */
typedef struct adi_adrv904x_CarrierReconfigLatencyCfg
{
    uint32_t groupDelay[ADI_ADRV904X_MAX_CARRIERS];                             /*!< Group delay */
    uint32_t clkCddcCducInkHz;                                                  /*!< CDDC/CDUC clock in kHz */
} adi_adrv904x_CarrierReconfigLatencyCfg_t;

/**
 *  \brief Holds calculated latency values per Channel
 */
typedef struct adi_adrv904x_CarrierReconfigLatencyCfgTop
{
    adi_adrv904x_CarrierReconfigLatencyCfg_t    channel[ADI_ADRV904X_MAX_TXCHANNELS]; /*!< Internal use only: Latency Cfg Array for all Channels */
} adi_adrv904x_CarrierReconfigLatencyCfgTop_t;
    
/**
 *  \brief Holds stored Output values from a ReconfigSolve() so that they can be used on next ReconfigApply()
 */
typedef struct adi_adrv904x_CarrierReconfigOutput
{
    adi_adrv904x_CarrierReconfigProfileCfgOut_t profileCfgs[ADI_ADRV904X_MAX_NUM_PROFILES];
    adi_adrv904x_CarrierReconfigJesdOut_t       jesdCfg;
} adi_adrv904x_CarrierReconfigOutput_t;

/**
 *  \brief Holds stored Solution (both Input and Output values) from a ReconfigSolve() so that they can be used on next ReconfigApply()
 */
typedef struct adi_adrv904x_CarrierReconfigSoln
{
    adi_adrv904x_CarrierReconfigInput_t     inputs;
    adi_adrv904x_CarrierReconfigOutput_t    outputs;
} adi_adrv904x_CarrierReconfigSoln_t;

#endif /* __ADI_ADRV904X_CARRIER_RECONFIGURE_TYPES_H__ */