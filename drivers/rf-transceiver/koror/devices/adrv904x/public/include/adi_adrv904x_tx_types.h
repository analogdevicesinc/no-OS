/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_tx_types.h
* \brief Contains ADRV904X API Tx datapath data types
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_TX_TYPES_H_
#define _ADI_ADRV904X_TX_TYPES_H_

#include "adi_library_types.h"
#include "adi_adrv904x_tx_nco.h"
#include "adi_adrv904x_gpio_types.h"

#define ADRV904X_TX_ATTEN_TABLE_MAX_IDX 959      /*!< Maximum valid tx attenuation table index */
#define ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX 41950    /*!< Maximum Value of TX Attenuation in 1/1000s of a dB */
#define ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05 50       /*!< The only supported attenuation step size is 0.05dB */
#define ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P1 100       /*!< Not supported */
#define ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P2 200       /*!< Not supported */
#define ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P4 400       /*!< Not supported */

#define ADRV904X_ADDR_TX0_ATTEN_TABLE 0x60812000
#define ADRV904X_ADDR_TX1_ATTEN_TABLE 0x60912000
#define ADRV904X_ADDR_TX2_ATTEN_TABLE 0x60A12000
#define ADRV904X_ADDR_TX3_ATTEN_TABLE 0x60B12000
#define ADRV904X_ADDR_TX4_ATTEN_TABLE 0x60C12000
#define ADRV904X_ADDR_TX5_ATTEN_TABLE 0x60D12000
#define ADRV904X_ADDR_TX6_ATTEN_TABLE 0x60E12000
#define ADRV904X_ADDR_TX7_ATTEN_TABLE 0x60F12000

#define ADRV904X_TX_ATTEN_TABLE_HP_MASK     0x0007F000
#define ADRV904X_TX_ATTEN_TABLE_HP_SHIFT    12
#define ADRV904X_TX_ATTEN_TABLE_MULT_MASK   0x00000FFF
#define ADRV904X_TX_ATTEN_TABLE_MULT_SHIFT  0

#define ADRV904X_TX_ATTEN_HP_MIN_VALUE      1
#define ADRV904X_TX_ATTEN_HP_MAX_VALUE      64
#define ADRV904X_TX_ATTEN_HP_SET_ALL_VALUE  0xFF

#define ADI_ADRV904X_MAX_TXCHANNELS 8U
#define ADI_ADRV904X_TX_CHAN_ID_MAX (ADI_ADRV904X_MAX_TXCHANNELS - 1U)
#define ADI_ADRV904X_TX_CHAN_MASK_MAX ((1U << (ADI_ADRV904X_TX_CHAN_ID_MAX + 1U)) - 1U)
#define ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES  (8192U) /*!< Maxinum number of TX playback samples */

/**
 *  \brief Enum of possible Tx channel enables
 */
typedef enum adi_adrv904x_TxChannels
{
    ADI_ADRV904X_TXOFF = 0x00U, /*!< No Tx channels are enabled */
    ADI_ADRV904X_TX0   = 0x01U, /*!< Tx0 channel enabled */
    ADI_ADRV904X_TX1   = 0x02U, /*!< Tx1 channel enabled */
    ADI_ADRV904X_TX2   = 0x04U, /*!< Tx2 channel enabled */
    ADI_ADRV904X_TX3   = 0x08U, /*!< Tx3 channel enabled */
    ADI_ADRV904X_TX4   = 0x10U, /*!< Tx4 channel enabled */
    ADI_ADRV904X_TX5   = 0x20U, /*!< Tx5 channel enabled */
    ADI_ADRV904X_TX6   = 0x40U, /*!< Tx6 channel enabled */
    ADI_ADRV904X_TX7   = 0x80U, /*!< Tx7 channel enabled */
    ADI_ADRV904X_TXALL = 0xFFU  /*!< All tx channel mask */
} adi_adrv904x_TxChannels_e;

/**
 * \brief Holds a single row of adrv904x tx attenuation table.
 */
typedef struct adi_adrv904x_TxAttenTableRow
{
    uint16_t txAttenMult;      /*!< Tx Attenuation Multiplier (0-4095) */
    uint8_t  txAttenHp;        /*!< Tx Attenuation HP (0-63)           */
    uint8_t  reserved;         /*!< Reserved                           */
} adi_adrv904x_TxAttenTableRow_t;

/**
*  \brief Enum select for pa protection input
*/
typedef enum adi_adrv904x_PaProtectionInputSel
{
    ADI_ADRV904X_POST_DPD_HC_OUTPUT = 0x0000,    /*!< Input data to Pa protection block is probed from Post DPD HC output */
    ADI_ADRV904X_TXQEC_ACTUATOR_OUTPUT = 0x0001  /*!< Input data to Pa protection block is probed from tx qec actuator output */

} adi_adrv904x_PaProtectionInputSel_e;

/**
*  \brief Enum select events for triggering ramp down
*/
typedef enum adi_adrv904x_RampDownTrigger
{
    ADI_ADRV904X_RDT_PEAK_PA            = 0x1,      /*!< Use the Peak PA block to trigger ramp down event */
    ADI_ADRV904X_RDT_AVG_PA             = 0x2,      /*!< Use the Average PA block to trigger ramp down event */
    ADI_ADRV904X_RDT_SRD                = 0x4,      /*!< Use the Slew Rate Detector block to trigger ramp down event */
    ADI_ADRV904X_RDT_CLK_PLL_UNLOCK     = 0x8,      /*!< Use the CLK PLL unlock to trigger ramp down event */
    ADI_ADRV904X_RDT_RF_PLL1_UNLOCK     = 0x10,     /*!< Use the RF PLL1 unlock to trigger ramp down event - This enum is deprecated. Please use ADI_ADRV904X_RDT_RF0_PLL_UNLOCK instead */
    ADI_ADRV904X_RDT_RF_PLL2_UNLOCK     = 0x20,     /*!< Use the RF PLL2 unlock to trigger ramp down event - This enum is deprecated. Please use ADI_ADRV904X_RDT_RF1_PLL_UNLOCK instead */
    ADI_ADRV904X_RDT_RF0_PLL_UNLOCK     = 0x10,     /*!< Use the RF0 PLL unlock to trigger ramp down event */
    ADI_ADRV904X_RDT_RF1_PLL_UNLOCK     = 0x20,     /*!< Use the RF1 PLL unlock to trigger ramp down event */
    ADI_ADRV904X_RDT_SERDES_PLL_UNLOCK  = 0x40,     /*!< Use the SERDES PLL unlock to trigger ramp down event */
    ADI_ADRV904X_RDT_DFMR0_EVENT        = 0x80,     /*!< Use Deframer0 IRQ to trigger ramp down event - This enum is deprecated */
    ADI_ADRV904X_RDT_DFRM0_EVENT        = 0x80,     /*!< Use Deframer0 IRQ to trigger ramp down event */
    ADI_ADRV904X_RDT_DFRM1_EVENT        = 0x100     /*!< Use Deframer1 IRQ to trigger ramp down event */
} adi_adrv904x_RampDownTrigger_e;

/**
*  \brief Data structure to hold ADRV904X Tx Peak & Average Power Protection block configuration
*/
typedef struct adi_adrv904x_PowerMonitorCfg
{
    adi_adrv904x_PaProtectionInputSel_e inputSel;   /*!< This selects the source of input signal for power monitor block */
    uint16_t peakThreshold;                         /*!< Power monitor peak power threshold. Peak Threshold[dBFS] = 10 * Log10(peakThreshold/65535) */
    uint8_t measDuration;                           /*!< Power monitor peak and avg power measurement duration: Duration = 2^(measDuration + 7) number of cycles. Valid range [0-15] */
    uint8_t peakCount;                              /*!< Peak count causing peak power error. */
    uint8_t peakErrorClearRequired;                 /*!< Set this to make the peak power error sticky. When set, user needs to call adi_adrv904x_TxProtectionErrorClear to clear peak power error */
    uint8_t peakPowerEnable;                        /*!< This enables peak power measurement block. 1 = Peak power error is flagged when peak count is above peak count threshold, 0 = disabled */
    uint8_t peakPowerIrqEnable;                     /*!< 1 = enables peak power error interrupt, 0 = no IRQ */
    uint16_t avgThreshold;                          /*!< Power monitor average power threshold. Average Threshold[dBFS] = 10 * Log10(avgThreshold/65535) */    
    uint8_t avgErrorClearRequired;                  /*!< Set this to make the average power error sticky. When set, user needs to call adi_adrv904x_TxProtectionErrorClear to clear average power error */
    uint8_t avgPowerEnable;                         /*!< This enables average power measurement block. 1 = PA error is flagged when average power measurement is above average power threshold, 0 = disabled */
    uint8_t avgPowerIrqEnable;                      /*!< 1 = enables average power error interrupt, 0 = no IRQ */
    uint8_t avgPeakRatioEnable;                     /*!< 1 = enables average to peak power ratio calculation block, both avgPower and peakPower calculations must be enabled before enabling ratio calculation, 0 = disabled */
}adi_adrv904x_PowerMonitorCfg_t;

/**
 * \brief Indicates the tx channels to which an instance of adi_adrv904x_PowerProtectionCfg_t applies
 */
typedef struct adi_adrv904x_PowerMonitorCfgRt
{
    uint32_t txChannelMask;                           /*!< The tx channels to which powerCfg applies. Suggested to use adi_adrv904x_TxChannels_e to build.  */
    adi_adrv904x_PowerMonitorCfg_t txPowerMonitorCfg; /*!< PowerProtection configuration used for channels specified in txChannelMask */
} adi_adrv904x_PowerMonitorCfgRt_t;

/**
 * \brief Holds the configuration for the PA protection ramp events
 */
typedef struct adi_adrv904x_ProtectionRampCfg
{
    uint32_t rampDownMask;                          /*!< Mask showing ramp down events. Bit descriptions can be found at adi_adrv904x_RampDownTrigger_e */
    uint8_t altEventClearReqd;                      /*!< 1 = user must manually clear pll_jesd_prot_clr after a PLL or JESD ramp down event, 0 = hardware clears */    
} adi_adrv904x_ProtectionRampCfg_t;

/**
 * \brief Indicates the tx channels to which an instance of adi_adrv904x_ProtectionRampCfg_t applies
 */
typedef struct adi_adrv904x_ProtectionRampCfgRt
{
    uint32_t txChannelMask;                             /*!< The tx channels to which protectionRampCfg applies. Suggested to use adi_adrv904x_TxChannels_e to build.  */
    adi_adrv904x_ProtectionRampCfg_t protectionRampCfg; /*!< ProtectionRamp configuration used for channels specified in txChannelMask */
} adi_adrv904x_ProtectionRampCfgRt_t;

/**
*  \brief Data structure to hold ADRV904X Tx Slew Rate Detector Configuration
*/
typedef struct adi_adrv904x_SlewRateDetectorCfg
{
    adi_adrv904x_PaProtectionInputSel_e inputSel;   /*!< This selects the source of input signal for SRD block */
    uint16_t srdOffset;                             /*!< These bits sets the offset (threshold) for the slew. Threshold[dBFS] = 10 * Log10(srdOffset/65535) */
    uint8_t srdEnable;                              /*!< 1:Enable // 0:Disable the slew rate detection block */
    uint8_t srdIrqEnable;                           /*!< 1:Enable // 0:Disable slew rate interrupt signal */
    uint8_t autoRecoveryWaitTime;                   /*!< HW will clear the SRD error after this wait time. Any new slew event during the countdown period reinitializes the timer.
                                                         This counter runs at the sample rate. Wait period in cycles = 2^(autoRecoveryWaitTime + 6). Valid Range[0-15] */
    uint8_t autoRecoveryEnable;                     /*!< 1:Enable 0:Disable auto-recovery. If enabled, HW will clear SRD error after autoRecoveryWaitTime. If disabled,
                                                         user needs to call adi_adrv904x_TxProtectionErrorClear to clear SRD error. */
    uint8_t autoRecoveryDisableTimerWhenTxOff;      /*!< 1:Disable // 0:Enable timer when Tx off. If disabled, auto-recovery counter is disabled when TxOn is low and during txon_wait_period. */
    uint8_t srdStatisticsEnable;                    /*!< 1:Enable SRD statistics // 0:Disable SRD statistics. When enabled, user can read statistics through adi_adrv904x_TxSlewRateStatisticsRead */
    uint8_t srdStatisticsMode;                      /*!< 0:Record the maximum slew observed. 1:Record the number of samples which have slew above threshold. */
} adi_adrv904x_SlewRateDetectorCfg_t;

/**
 * \brief Indicates the tx channels to which an instance of adi_adrv904x_SlewRateDetectorCfg_t applies
 */
typedef struct adi_adrv904x_SlewRateDetectorCfgRt
{
    uint32_t txChannelMask;                         /*!< The tx channels to which srlCfg applies. Suggested to use adi_adrv904x_TxChannels_e to build.  */
    adi_adrv904x_SlewRateDetectorCfg_t srlCfg;      /*!< SlewRateDetector configuration used for channels specified in txChannelMask */
} adi_adrv904x_SlewRateDetectorCfgRt_t;

/**
*  \brief Data structure to hold ADRV904X Tx Power Monitor Status fields
*/
typedef struct adi_adrv904x_TxPowerMonitorStatus_t
{
    uint16_t avgPower;         /*!< Power monitor average power readback. */
    uint16_t peakPower;        /*!< Power monitor peak power readback. */
    uint16_t avgErrorPower;    /*!< Power monitor average power error. This shows the average 
                                    power when average power error flag is asserted. */
    uint16_t peakErrorPower;   /*!< Power monitor peak power error. This shows the peak 
                                    power when peak power error flag is asserted. */
    uint16_t avgPeakRatio;     /*!< Average to Peak power ratio readback. */

} adi_adrv904x_TxPowerMonitorStatus_t;

/**
* \brief Data structure to hold a adrv904x device Tx attenuation calls
*/
typedef struct adi_adrv904x_TxAtten
{
    uint32_t txChannelMask;     /*!< Tx Channel mask selection           */
    uint16_t txAttenuation_mdB; /*!< Tx Attenuation value in millidB           */
} adi_adrv904x_TxAtten_t;

/**
* \brief Enum to set the Tx Attenuation step size
*/
typedef enum adi_adrv904x_TxAttenStepSize
{
    ADI_ADRV904X_TXATTEN_0P05_DB = 0, /*!< S0, S1 registers programmed in units of 0.05dB */
    ADI_ADRV904X_TXATTEN_0P1_DB  = 1, /*!< Not supported */
    ADI_ADRV904X_TXATTEN_0P2_DB  = 2, /*!< Not supported */
    ADI_ADRV904X_TXATTEN_0P4_DB  = 3, /*!< Not supported */
    ADI_ADRV904X_TXATTEN_INVALID = 4
} adi_adrv904x_TxAttenStepSize_e;

/**
 * \brief Each Tx channel can have it's Tx attenuation updated from several sources.
 */ 
typedef enum adi_adrv904x_TxAttenUpdateSrc
{
    ADI_ADRV904X_TXATTEN_UPD_SRC_S0,       /*!< The attenuation register S0 */
    ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1  /*!< Either S0 or S1 depending on a GPIO input level */
} adi_adrv904x_TxAttenUpdateSrc_e;

/**
 * \brief Tx attenuation update source plus any required extra configuration of source.
 */ 
typedef struct adi_adrv904x_TxAttenUpdateSrcCfg
{
    adi_adrv904x_TxAttenUpdateSrc_e updateSrc; /*!< The update source */
    
    /** If updateSrc is S0_OR_S1 indicates which GPIO input selects between the two. If that GPIO input is
     *  high S1 is used otherwise S0 used. */
    adi_adrv904x_GpioPinSel_e s0OrS1Gpio; /*!< GPIO selection */
}adi_adrv904x_TxAttenUpdateSrcCfg_t;

/**
 * \brief Each Tx channel has it's Tx attenuation updated on the occurrence of one of several triggers.
 */
typedef enum adi_adrv904x_TxAttenUpdateTrg_e
{
    ADI_ADRV904X_TXATTEN_UPD_TRG_NONE, 
    ADI_ADRV904X_TXATTEN_UPD_TRG_SPI,
    ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO
} adi_adrv904x_TxAttenUpdateTrg_e;

/**
 *  \brief Tx Atten update trigger plus any configuration of that trigger.
 */
typedef struct adi_adrv904x_TxAttenUpdateTrgCfg_t
{
    adi_adrv904x_TxAttenUpdateTrg_e updateTrg; /*!< Update trigger selection */
    adi_adrv904x_GpioPinSel_e triggerGpio; /*!< If updateTrg is GPIO indicate which GPIO rising-edge triggers the update. */
} adi_adrv904x_TxAttenUpdateTrgCfg_t;

/**
 * \brief Complete configuration for the Tx attenuation update trigger and source for a single channel.
 */
typedef struct adi_adrv904x_TxAttenUpdateCfg
{
    adi_adrv904x_TxAttenUpdateSrcCfg_t srcCfg; /*!< Tx attenuation update source config */
    adi_adrv904x_TxAttenUpdateTrgCfg_t trgCfg; /*!< Tx attenuation update trigger config */
} adi_adrv904x_TxAttenUpdateCfg_t;
 
/**
 * \brief Configuration structure for Tx power control
 */
typedef struct adi_adrv904x_TxAttenCfg
{
    adi_adrv904x_TxAttenUpdateCfg_t updateCfg;       /*!< Tx atten update configuration */
    adi_adrv904x_TxAttenStepSize_e  txAttenStepSize; /*!< The units in which the S0 and S1 registers are programmed */
} adi_adrv904x_TxAttenCfg_t;


/**
*  \brief Data structure for Tx decimated power block configuration
*/
typedef struct adi_adrv904x_TxDecimatedPowerCfg
{
    uint32_t    txChannelMask; /*!< Tx channel selection */    
    uint8_t     measurementEnable; /*!< 0:Disable , 1:Enable power measurements */
    uint8_t     powerInputSelect; /*!< Use output of Tx for power measurement. 0: Band DUC output, 1: DPD output 2: QEC output, 3:PFIR output. Valid Range from 0 to 3*/
    uint8_t     powerMeasurementDuration; /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
    uint8_t     peakToPowerMode; /*!< 0:Disable , 1:Enable peak-to-power mode.  */
}adi_adrv904x_TxDecimatedPowerCfg_t;


/**
* \brief Enum to set the Discontinuous Transmit (DTX) Mode
* This 2 bit bitfield is used to select the mode of operation of the DTx mode control block.
*  00 - DTx mode control disabled 
*  01 - Automatic mode. Triggers the power down stream on observing a specified number of zero samples (dtx_zero_counter) on Tx input data. Power up stream is triggered as soon as a non zero sample is seen.
*  10 - SPI controlled mode - In this mode, power down is triggered on setting the dtx_force bitfield. Clearing that bit triggers the power up stream.
*  11 - Pin controlled mode - In this mode, a GPIO pin specified by the dtx_force_gpio_select controls Power up/down. Rising edge on the pin triggers the power down stream and a falling edge triggers a power up stream 
*/
typedef enum adi_adrv904x_DtxMode
{
    ADI_ADRV904X_DTXMODE_DISABLE = 0,    /*!< DTX is in Disable Mode */
    ADI_ADRV904X_DTXMODE_AUTO = 1,       /*!< DTX is in Automatic Mode */
    ADI_ADRV904X_DTXMODE_SPI = 2,        /*!< DTX is in SPI Control Mode */
    ADI_ADRV904X_DTXMODE_PIN = 3         /*!< DTX is in PIN Control Mode */
} adi_adrv904x_DtxMode_e;

/**
 * \brief Data structure to hold Discontinuous Transmit (DTX) configuration.
 */
typedef struct adi_adrv904x_DtxCfg
{
    uint32_t txChannelMask;         /*!< Tx Channels for which the Dtx to be updated. Bits 0-7 correspond to Tx0-Tx7 */
    adi_adrv904x_DtxMode_e dtxMode; /*!< Select the Mode configuration for DTX */
    uint16_t dtxZeroCounter;        /*!< DTX Zero Counter for DTX Auto Mode. When dtx_mode_enable=2'b01 (Auto mode), this bitfield specifies the number of continues zeros to trigger DTX power down  */
} adi_adrv904x_DtxCfg_t;

/**
* \brief Data structure to hold Discontinuous Transmit (DTX) GPIO configuration.
*/
typedef struct adi_adrv904x_DtxGpioCfg
{
    adi_adrv904x_GpioPinSel_e dtxGpioTxSel[ADI_ADRV904X_MAX_TXCHANNELS];  /*!< GPIO selection for each Tx channel. User can select ADI_ADRV904X_GPIO_INVALID if they don't need to route DTX signal to a GPIO. */
} adi_adrv904x_DtxGpioCfg_t;



/**
 *  \brief Enum of Carriers within an Tx Channel
 */
typedef enum adi_adrv904x_TxCarrier
{
    ADI_ADRV904X_TX_CARRIER_NONE = 0x00U,
    ADI_ADRV904X_TX_CARRIER_0    = 0x01U,
    ADI_ADRV904X_TX_CARRIER_1    = 0x02U,
    ADI_ADRV904X_TX_CARRIER_2    = 0x04U,
    ADI_ADRV904X_TX_CARRIER_3    = 0x08U,
    ADI_ADRV904X_TX_CARRIER_4    = 0x10U,
    ADI_ADRV904X_TX_CARRIER_5    = 0x20U,
    ADI_ADRV904X_TX_CARRIER_6    = 0x40U,
    ADI_ADRV904X_TX_CARRIER_7    = 0x80U,
    ADI_ADRV904X_TX_CARRIER_ALL  = 0xFFU,
} adi_adrv904x_TxCarrier_e;

/**
 * \brief Hard clipper configuration structure
 */
typedef struct adi_adrv904x_HardClipperConfig
{
    uint8_t hardClipperSel; /*!< Hardclipper PreDpd/PostAtten selection. 1: PreDpd // 0 : PostAtten */
    uint8_t hardClipperEnable; /*!< Hardclipper enable/disable selection. 1: Enable // 0 : Disable */
    uint32_t hardClipperThreshold; /*!< Normalized threshold for the hard clipper in the range 0 to 2^16.
                                                  The threshold is relative to 0dBFS which means that a threshold value of 2^16 corresponds to 0dBFS.
                                                  The threshold is applied to an approximation of SQRT(I^2 + Q^2). The hard clipper threshold can
                                                  be calculated as HardClipperThreshold = 10^(threshold_dBFS/20) * 2^16 */
} adi_adrv904x_HardClipperConfig_t;

/**
 * \brief Hard Clipper Statistics. For details please check adi_adrv904x_HardClipperStatisticsGet function
 */
typedef struct adi_adrv904x_HardClipperStatistics
{
    uint16_t postAttenHardClipperStat;  /*!< Hardclipper PostAtten Statistics. */
    uint16_t preDpdHardClipperStat; /*!< Hardclipper PreDpd Statistics.*/
} adi_adrv904x_HardClipperStatistics_t;

#define ADI_ADRV904X_MAX_TX_CARRIERS 8U

/**
 * \brief Data structure to hold Tx Carrier Mask selections (Tx Channel mask + Carrier mask)
 */
typedef struct adi_adrv904x_TxCarrierMask
{
    uint32_t txChannelMask; /*!< Tx Channel Mask selection */
    uint32_t carrierMask;   /*!< Carrier Mask selection */
} adi_adrv904x_TxCarrierMask_t;

/**
 * \brief Data structure to hold Tx Carrier Selections (Tx Channel Select + Carrier Mask)
 */
typedef struct adi_adrv904x_TxCarrierSel
{
    adi_adrv904x_TxChannels_e   txChannel;      /*!< Tx Channel Selection */
    uint32_t                    carrierMask;    /*!< Carrier Mask selection */
} adi_adrv904x_TxCarrierSel_t;

/**
 *  \brief Enum of Carriers Band within an Tx Channel
 */
typedef enum adi_adrv904x_TxCarrierBand
{
    ADI_ADRV904X_TX_CARRIER_BAND_NONE = 0x00U,
    ADI_ADRV904X_TX_CARRIER_BAND_0    = 0x01U,
    ADI_ADRV904X_TX_CARRIER_BAND_1    = 0x02U,
    ADI_ADRV904X_TX_CARRIER_BAND_ALL  = 0x03U,
} adi_adrv904x_TxCarrierBand_e;

#define ADI_ADRV904X_MAX_TX_CARRIER_BANDS 2U
/** 
 * \brief Data structure to hold the Tx carrier band Mask selections (Tx Channel mask + band mask).
 */ 
typedef struct adi_adrv904x_TxBandMask 
{
    uint32_t  txChannelMask; /*!< Tx Channel Mask selection */ 
    uint32_t  bandMask;      /*!< Tx Band Mask selection */
} adi_adrv904x_TxBandMask_t;

/**
 *  \brief Enum of Carriers Band within an Tx Channel
 */
typedef enum adi_adrv904x_TxDpdChanId
{
    ADI_ADRV904X_TX_DPD_ID_NONE = 0x00U,
    ADI_ADRV904X_TX_DPD_ID_0    = 0x01U,
    ADI_ADRV904X_TX_DPD_ID_1    = 0x02U,
    ADI_ADRV904X_TX_DPD_ID_2    = 0x04U,
    ADI_ADRV904X_TX_DPD_ID_3    = 0x08,
    ADI_ADRV904X_TX_DPD_ID_ALL  = 0x0FU,
} adi_adrv904x_TxDpdChanId_e;

#define ADI_ADRV904X_MAX_TX_DPD_IDS 4U
/** 
 * \brief Data structure to hold the Tx carrier band Mask selections (Tx Channel mask + band mask).
 */ 
typedef struct adi_adrv904x_TxDpdMask 
{
    uint32_t  txChannelMask; /*!< Tx Channel Mask selection */ 
    uint32_t  dpdIdMask;     /*!< Tx Dpd Channel Id Mask selection */
} adi_adrv904x_TxDpdMask_t; 
 

/** 
 * \brief Data structure to hold the Post DPD atten parameters 
 */ 
typedef struct adi_adrv904x_PostDpdAtten 
{ 
    uint8_t magGain;     /*!< Actuator Magnitude Square Input Gain Range 0-4*FS. Saturated to 1x. Data Format: Unsigned Int.Frac=2.6. Unity gain=0x40.*/ 

    uint8_t gmpOutShift; /*!< Actuator Output Shift for gmp terms Range 0-15. Add one to double output gain. 
                          * Output is left shifted to accommodate output normalization. 
                          * Default shift is configured for unity coefficient sum of 0x2000 and 
                          * 2 bits of headroom for individual coefficient. When large number of terms 
                          * are used resulting in bit growth, the output gain can be decreased (right shift) 
                          * to the correct the overall gain. Max dynamic range at out_shift=0, supporting 
                          * all LUTs active and data at maximum. */ 

    uint8_t ddrOutShift; /*!< Actuator Output Shift for ddr terms Range 0-15. Add one to double output gain. 
                          * Output is left shifted to accommodate output normalization. 
                          * Default shift is configured for unity coefficient sum of 0x2000 and 
                          * 2 bits of headroom for individual coefficient. When large number of terms 
                          * are used resulting in bit growth, the output gain can be decreased (right shift) 
                          * to the correct the overall gain. Max dynamic range at out_shift=0, supporting 
                          * all LUTs active and data at maximum. */ 

} adi_adrv904x_PostDpdAtten_t; 
 
/**
 * \brief TX playback data. It contains 4096 samples maximum
 */
typedef struct adi_adrv904x_TxPlaybackData
{
    int16_t playbackDataReal[ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES]; /*!< An array of samples for TX playback - 16bit real samples */
    int16_t playbackDataImag[ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES]; /*!< An array of samples for TX playback - 16bit imag samples */
    uint16_t numSamples; /*!< No. of samples contained in playbackDataReal and playbackDataImag arrays */
}adi_adrv904x_TxPlaybackData_t;

#endif /* _ADI_ADRV904X_TX_TYPES_H_ */
