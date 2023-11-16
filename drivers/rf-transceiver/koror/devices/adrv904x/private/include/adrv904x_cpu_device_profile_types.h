/**
 * \file adrv904x_cpu_device_profile_types.h
 *
 * \brief   Contains ADRV904X Device Profile type definitions
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_DEVICE_PROFILE_TYPES_H__
#define __ADRV904X_CPU_DEVICE_PROFILE_TYPES_H__

#include "adrv904x_cpu_device_profile_jesd_types.h"
#include "adrv904x_cpu_device_profile_pfir_types.h"
#include "adi_adrv904x_version_types.h"
#include "adi_adrv904x_platform_pack.h"

#define ADRV904X_PROFILE_TX_CHAN_MASK        (0x000000FFU)
#define ADRV904X_PROFILE_TX_CHAN_POS         (0U)

#define ADRV904X_PROFILE_RX_CHAN_MASK        (0x0000FF00U)
#define ADRV904X_PROFILE_RX_CHAN_POS         (8U)

#define ADRV904X_PROFILE_ORX_CHAN_MASK       (0x00030000U)
#define ADRV904X_PROFILE_ORX_CHAN_POS        (16U)

#define ADRV904X_PROFILE_CHAN_CONFIG_MASK    (ADRV904X_PROFILE_TX_CHAN_MASK   \
                                              | ADRV904X_PROFILE_RX_CHAN_MASK \
                                              | ADRV904X_PROFILE_ORX_CHAN_MASK)

/* Channel types used throughout the system */
typedef enum adrv904x_ChannelType
{
    ADRV904X_CH_TYPE_RX,
    ADRV904X_CH_TYPE_TX,
    ADRV904X_CH_TYPE_ORX,
    ADRV904X_CH_TYPE_LOOPBACK,
    ADRV904X_CH_TYPE_MAX
} adrv904x_ChannelType_e;

/* Enumeration for DDC modes, Rx and Orx */
typedef enum adrv904x_RxDdc
{
    ADRV904X_RXDDC_BYPASS = 0u,           /*!< No Half Band Enabled */
    ADRV904X_RXDDC_FILTERONLY,            /*!< Half Band Filters only */
    ADRV904X_RXDDC_INT2,                  /*!< Half Band Interpolation by 2 */
    ADRV904X_RXDDC_DEC2,                  /*!< Half Band Decimate by 2 */
    ADRV904X_RXDDC_BYPASS_REALIF,         /*!< No Half Band Enabled  (Real I/F MODE)*/
    ADRV904X_RXDDC_FILTERONLY_REALIF,     /*!< Half Band Filters only (Real I/F MODE) */
    ADRV904X_RXDDC_INT2_REALIF,           /*!< Half Band Interpolation by 2 (Real I/F MODE) */
    ADRV904X_RXDDC_DEC2_REALIF            /*!< Half Band Decimate by 2 (Real I/F MODE) */
} adrv904x_RxDdc_e;

/* Enumeration for RX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_RxChannelNum
{
    ADRV904X_RX_CH_1 = 0u,        /*!< Rx channel # 1 index   */
    ADRV904X_RX_CH_2,             /*!< Rx channel # 2 index   */
    ADRV904X_RX_CH_3,             /*!< Rx channel # 3 index   */
    ADRV904X_RX_CH_4,             /*!< Rx channel # 4 index   */
    ADRV904X_RX_CH_5,             /*!< Rx channel # 5 index   */
    ADRV904X_RX_CH_6,             /*!< Rx channel # 6 index   */
    ADRV904X_RX_CH_7,             /*!< Rx channel # 7 index   */
    ADRV904X_RX_CH_8,             /*!< Rx channel # 8 index   */
    ADRV904X_RX_CHAN_LEN          /*!< Total number of Rx Channels supported  */
} adrv904x_RxChannelNum_e;

/* Enumeration for TX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_TxChannelNum
{
    ADRV904X_TX_CH_1 = 0u,        /*!< Tx channel # 1 index   */
    ADRV904X_TX_CH_2,             /*!< Tx channel # 2 index   */
    ADRV904X_TX_CH_3,             /*!< Tx channel # 3 index   */
    ADRV904X_TX_CH_4,             /*!< Tx channel # 4 index   */
    ADRV904X_TX_CH_5,             /*!< Tx channel # 5 index   */
    ADRV904X_TX_CH_6,             /*!< Tx channel # 6 index   */
    ADRV904X_TX_CH_7,             /*!< Tx channel # 7 index   */
    ADRV904X_TX_CH_8,             /*!< Tx channel # 8 index   */
    ADRV904X_TX_CHAN_LEN          /*!< Total number of Tx Channels supported  */
} adrv904x_TxChannelNum_e;

/* Enumeration for ORX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_OrxChannelNum
{
    ADRV904X_ORX_CH_1 = 0u,        /*!< Orx channel # 1 index */
    ADRV904X_ORX_CH_2,             /*!< Orx channel # 2 index */
    ADRV904X_ORX_CHAN_LEN          /*!< Total number of Orx Channels supported  */
} adrv904x_OrxChannelNum_e;

/* Enumeration for RX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_RxChannelIdx
{
    ADRV904X_RX_CH_IDX_0 = 0u,     /*!< Rx channel index # 0 */
    ADRV904X_RX_CH_IDX_1,          /*!< Rx channel index # 1 */
    ADRV904X_RX_CH_IDX_2,          /*!< Rx channel index # 2 */
    ADRV904X_RX_CH_IDX_3,          /*!< Rx channel index # 3 */
    ADRV904X_RX_CH_IDX_4,          /*!< Rx channel index # 4 */
    ADRV904X_RX_CH_IDX_5,          /*!< Rx channel index # 5 */
    ADRV904X_RX_CH_IDX_6,          /*!< Rx channel index # 6 */
    ADRV904X_RX_CH_IDX_7,          /*!< Rx channel index # 7 */
    ADRV904X_RX_CHAN_IDX_LEN       /*!< Total number of Rx Channels supported  */
} adrv904x_RxChannelIdx_e;

/* Enumeration for TX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_TxChannelIdx
{
    ADRV904X_TX_CH_IDX_0 = 0u,        /*!< Tx channel index # 0 */
    ADRV904X_TX_CH_IDX_1,             /*!< Tx channel index # 1 */
    ADRV904X_TX_CH_IDX_2,             /*!< Tx channel index # 2 */
    ADRV904X_TX_CH_IDX_3,             /*!< Tx channel index # 3 */
    ADRV904X_TX_CH_IDX_4,             /*!< Tx channel index # 4 */
    ADRV904X_TX_CH_IDX_5,             /*!< Tx channel index # 5 */
    ADRV904X_TX_CH_IDX_6,             /*!< Tx channel index # 6 */
    ADRV904X_TX_CH_IDX_7,             /*!< Tx channel index # 7 */
    ADRV904X_TX_CHAN_IDX_LEN          /*!< Total number of Tx Channels supported  */
} adrv904x_TxChannelIdx_e;

/* Enumeration for ORX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_OrxChannelIdx
{
    ADRV904X_ORX_CH_IDX_0 = 0u,        /*!< Orx channel index # 0 */
    ADRV904X_ORX_CH_IDX_1,             /*!< Orx channel index # 1 */
    ADRV904X_ORX_CHAN_IDX_LEN          /*!< Total number of Orx Channels supported  */
} adrv904x_OrxChannelIdx_e;

/* Enumeration for serdes lane numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv904x_SerDesLaneNum
{
    ADRV904X_SERDES_LANE_1 = 0u,        /*!< SERDES LANE # 1 index   */
    ADRV904X_SERDES_LANE_2,             /*!< SERDES LANE # 2 index   */
    ADRV904X_SERDES_LANE_3,             /*!< SERDES LANE # 3 index   */
    ADRV904X_SERDES_LANE_4,             /*!< SERDES LANE # 4 index   */
    ADRV904X_SERDES_LANE_5,             /*!< SERDES LANE # 5 index   */
    ADRV904X_SERDES_LANE_6,             /*!< SERDES LANE # 6 index   */
    ADRV904X_SERDES_LANE_7,             /*!< SERDES LANE # 7 index   */
    ADRV904X_SERDES_LANE_8,             /*!< SERDES LANE # 8 index   */
    ADRV904X_SERDES_LANE_LEN            /*!< Total number of SerDes Lanes supported  */
} adrv904x_SerdesLaneNum_e;

#define ADRV904X_RX_CHAN_LEN_CPU        (ADRV904X_RX_CHAN_LEN / 2u)
#define ADRV904X_TX_CHAN_LEN_CPU        (ADRV904X_TX_CHAN_LEN / 2u)
#define ADRV904X_ORX_CHAN_LEN_CPU       (ADRV904X_ORX_CHAN_LEN / 2u)
#define ADRV904X_SERDES_LANE_LEN_CPU    (ADRV904X_SERDES_LANE_LEN / 2u)

/* Enumeration for EXT LO Select Device Profile def'n */
typedef enum
{
    ADRV904X_LOGEN_SEL_TX = 0x01u,               /*!< Set the PLL to Tx channels */
    ADRV904X_LOGEN_SEL_RX = 0x02u                /*!< Set the PLL to Rx channels */
} adrv904x_LogenRxTxSel_e;


typedef enum
{
    ADRV904X_LOGEN_INTERNAL_IN  = 0x01u,           /*!< Set Logen to internal mode */
    ADRV904X_LOGEN_EXTERNAL_IN  = 0x02u,           /*!< Set Logen to external IN mode */
    ADRV904X_LOGEN_EXTERNAL_OUT  = 0x04u           /*!< Set Logen to external OUT mode */
} adrv904x_LogenInOutSel_e;



typedef enum
{
    ADRV904X_RADIO_MODE_TDD,    /*!< Radio Mode is set to TDD */
    ADRV904X_RADIO_MODE_FDD,    /*!< Radio Mode is set to FDD */
    ADRV904X_RADIO_MODE_MAX     /*!< Internal use */
} adrv904x_ConfigDivDuplexMode_e;


typedef enum adrv904x_DfeMappingControl
{
    ADRV904X_DFE_MAPPING_CTRL_DEFAULT,      /*!< In RCI mode, BBIC controls both Tx/Rx/Orx enable and TxToOrx mapping pins externally. In RS mode, radio sequencer controls Tx/ORx enable signals
                                                 DFE CPU controls Tx to Orx mapping pins(through RS with GPIO outputs from Koror)	*/
    ADRV904X_DFE_MAPPING_CTRL_DFE           /*!< In RCI mode, BBIC controls Tx/Rx/Orx enable pins externally. DFE CPU controls Tx to Orx mapping pins(with GPIO outputs from Koror)
                                                 In RS mode,  RS controls Tx/Rx/Orx enable pins. DFE CPU controls Tx to Orx mapping pins(with GPIO outputs from Koror) */
} adrv904x_DfeMappingControl_e;

/* Enumeration for DUC channel number */
typedef enum adrv904x_DucNumber
{
    ADRV904X_DUC_BAND_0 = 0u,              /*!< DUC channel 0 */
    ADRV904X_DUC_BAND_1,                   /*!< DUC channel 1 */
    ADRV904X_DUC_NUM_BAND
} adrv904x_DucNumber_e;

/* Enumeration for DDC channel number */
typedef enum adrv904x_DdcNumber
{
    ADRV904X_DDC_BAND_0 = 0u,              /*!< DDC channel 0 */
    ADRV904X_DDC_BAND_1,                   /*!< DDC channel 1 */
    ADRV904X_DDC_NUM_BAND
} adrv904x_DdcNumber_e;

/* profiles */
#define ADRV904X_NUM_RX_PROFILES     (4u)
#define ADRV904X_NUM_TX_PROFILES     (4u)
#define ADRV904X_NUM_ORX_PROFILES    (2u)
/* slices */
#define ADRV904X_NUM_TXRX_CHAN       (8u)
#define ADRV904X_NUM_ORX_CHAN        (2u)

/****************************************************************************
* PLL structure.
****************************************************************************/
ADI_ADRV904X_PACK_START
typedef struct adrv904x_PllConfig
{
    uint64_t freqHz;                                        /*!< Internal PLL Freq Setting in Hz  */
    uint64_t vcoFreqHz;                                     /*!< Internal PLL VCO Setting in Hz  */
    uint8_t  loDiv;                                         /*!< Internal LO Divider 1 to 64  */
    uint8_t  divRange;                                      /*!< Serdes only Div Range  */
    uint8_t  div2;                                          /*!< Serdes only Div 2  */
    uint64_t extLoInFreqHz;                                 /*!< External LO Frequency IN in Hz */
    uint8_t  extLoOutDiv;                                   /*!< External LO Out Divider   */
    uint8_t  selInternalExt;                                /*!< Internal or External LO   */
    uint8_t  power;                                         /*!< PLL Power setting  */
    uint8_t  phaseMargin;                                   /*!< PLL Phase Margin  */
    uint32_t loopBandwidth;                                 /*!< PLL Loop bandwidth  */
    uint32_t refClock_kHz;                                  /*!< PLL ref clock in kHz  */
    uint8_t  refClkDiv;                                     /*!< PLL ref clock divider  */
    uint8_t  iBleedEnb;                                     /*!< PLL bleed ramp enable  */
    uint8_t  reserved[2];                                   /*!< Reserve  */
} adrv904x_PllConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* PLL structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_ClkGenConfig
{
    uint8_t div1enb;                /*!< div 2 enable setting  */
    uint8_t div3enb;                /*!< div 3 enable setting */
    uint8_t intDiv;                 /*!< int div bincode setting  */
    uint8_t clkDiv;                 /*!< clkDiv thermo bincode setting  */
    uint8_t reserved[4];            /*!< Reserve  */
} adrv904x_ClkGenConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* NCO Shifter structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_NcoShifter
{
    uint8_t  ncoEnabled;        /*!< NCO enabled  */
    uint32_t totalDecimation;   /*!< Total DDC decimation  */
    int32_t  ncoFreqin_kHz;     /*!< NCO Frequency  */
    int64_t  ncoFtw;            /*!< NCO tuning word  */
    uint32_t tinClockFreq_kHz;  /*!< NCO TIN freq in kHz  */
    uint8_t  hb3Enable;         /*!< HB3 Enable */
    uint8_t  hb2Enable;         /*!< HB2 Enable  */
    uint8_t  hb1Enable;         /*!< HB1 Enable  */
    uint8_t  hb3OutputClkDiv;   /*!< HB3 output clock rate divider */
    uint8_t  hb2OutputClkDiv;   /*!< HB2 output clock rate divider*/
    uint8_t  hb1OutputClkDiv;   /*!< HB1 output clock rate divider */
    uint8_t  hb3DpClkDiv;       /*!< HB3 DP clock rate divider */
    uint8_t  hb2DpClkDiv;       /*!< HB2 DP clock rate divider */
    uint8_t  hb1DpClkDiv;       /*!< HB1 DP clock rate divider*/
    uint8_t  tinClkDiv;         /*!< HB1 Tin clock rate  */
    uint8_t  rxBandEnb;         /*!< rx band enable */
    uint8_t  resampleMode;      /*!< Resampling filter mode */
    uint32_t rfCenterFreq_kHz;  /*!< RF center frequency */
    uint32_t instBw_kHz;        /*!< instantaneous signal bandwidth */
    uint32_t inputSigBw_kHz;    /*!< input signal bandwidth */
    uint8_t  rxGainDelay;       /*!< Rx gain delay */
    uint8_t  reserved[4];       /*!< Reserve  */
} adrv904x_NcoShifter_t;
ADI_ADRV904X_PACK_FINISH

/* carriers */
#define ADRV904X_NUM_TX_CARRIERS               (8u)
#define ADRV904X_NUM_RX_CARRIERS               (8u)
#define ADRV904X_NUM_CF_COEFFICIENTS           (648u)
#define ADRV904X_NUM_CF_DELAY_FIFOS            (8u)
/* defined in adrv904x_cpu_device_profile_jesd_types.h
 *   ADRV904X_JESD_MAX_CARRIER_SLOTS
 */
/* crest factor reduction */
#define ADRV904X_NUM_CFR_ENGINES               (3u)
#define ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN  (1024u)
/* charge trapping digital pre-distortion */
#define ADRV904X_NUM_CT_DPD_DELAY_LINES        (4u)

/****************************************************************************
* Carrier Channel structure.
****************************************************************************/
ADI_ADRV904X_PACK_START
/*!< adrv904x_CarrierChannelConfig:  structure of carrier channel configuration parameters */
typedef struct adrv904x_CarrierChannelConfig
{
    uint32_t inputRate_kHz;                                                     /*!< input sample rate in kHz */
    uint32_t ibw_kHz;                                                           /*!< instantaneous signal bandwidth */
    uint16_t filterTaps;                                                        /*!< number of filter taps */
    uint32_t outputRate_kHz;                                                    /*!< output sample rate in kHz */

    uint8_t  bandSelect;                                                        /*!< DUC/DDC-0 or 1 */
    int32_t  ncoFreq_kHz;                                                       /*!< mixer NCO frequency in kHz */
    uint64_t ncoFtw;                                                            /*!< alternate definition for NCO frequency; unused? */
    uint32_t ncoPhaseOffset;                                                    /*!< mixer NCO phase */
    uint8_t  mixerEnable;                                                       /*!< mixer enable */
    uint8_t  carrierRateRatio;                                                  /*!< ratio of carrier to CDUC clock rates */
    uint8_t  interpolationRatio;                                                /*!< number of HB stages (x2 per stage) */
    uint8_t  decimationRatio;                                                   /*!< number of downsample stages (/2 per stage) */
    uint8_t  startBank;                                                         /*!< filter start bank */
    uint8_t  endBank;                                                           /*!< filter end bank */
    uint8_t  filterBypass;                                                      /*!< bypass the filter */
    uint8_t  asymmetricTaps;                                                    /*!< asymmetric tap values */
    uint8_t  oddTaps;                                                           /*!< odd number of taps */
    uint8_t  loopbackPoint;                                                     /*!< filter loopback to support higher bandwidths */
    uint32_t carrierGain;                                                       /*!< Gain value for normalization of carrier power in/out of aggregation */
	uint8_t  carrierGainEnable;                                                 /*!< Enable field for carrier gain adjustments */

    uint8_t  reserved[2];                                                       /*!< Reserve */
} adrv904x_CarrierChannelConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Carrier DDC (digital down-converter) structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_CarrierDdcConfig:  structure of Ddc configuration parameters */
typedef struct adrv904x_CarrierDdcConfig
{
    adrv904x_CarrierChannelConfig_t carriers[ADRV904X_NUM_RX_CARRIERS];                 /*!< carrier definitions */
    int16_t                         coefficients[ADRV904X_NUM_CF_COEFFICIENTS];         /*!< filter coefficients for entire cDDC */
    uint8_t                         slotTable[ADRV904X_JESD_MAX_CARRIER_SLOTS];         /*!< JESD slot mapping */
    uint8_t                         maxSlot;                                            /*!< number of slots used */
    uint32_t                        slotValid_lower;                                    /*!< bitmap indicating slot validity; bits 0 to 31 */
    uint32_t                        slotValid_upper;                                    /*!< bitmap indicating slot validity; bits 32 to 63 */
    uint8_t                         cddcBypass;                                         /*!< bypass entire cDDC */
    uint8_t                         carriersEnabled;                                    /*!< bitmap of enabled carriers */
    uint8_t                         cddcClkDiv;                                         /*!< clock divide for cDDC processing */
    uint8_t                         interfaceClkDiv;                                    /*!< clock divide for internal interface to Jtx */
    uint8_t                         bandRatio[ADRV904X_DDC_NUM_BAND];                   /*!< rate relationship of cDDC to DDC-0/1 */
    uint8_t                         delayComp;                                          /*!< bitmap enable for delay compensation */
    uint8_t                         delayMem;                                           /*!< bitmap enable for delay memory */
    uint8_t                         delayChain;                                         /*!< bitmap enable for delay memory daisy chain */
    uint8_t                         delayCarrierSelect[ADRV904X_NUM_CF_DELAY_FIFOS];    /*!< delay memory carrier selection */
    uint16_t                        delayValue[ADRV904X_NUM_CF_DELAY_FIFOS];            /*!< delay compensation value */
    uint8_t                         pwrMeasClkDiv;                                      /*!< clock divide for cDDC power measurement block */

    uint8_t                         reserved[1];                                        /*!< Reserve */
} adrv904x_CarrierDdcConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Carrier DUC (digital up-converter) structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_CarrierDucConfig:  structure of Duc configuration parameters */
typedef struct adrv904x_CarrierDucConfig
{
    adrv904x_CarrierChannelConfig_t carriers[ADRV904X_NUM_TX_CARRIERS];                 /*!< carrier definitions */
    int16_t                         coefficients[ADRV904X_NUM_CF_COEFFICIENTS];         /*!< filter coefficients for entire cDUC */
    uint8_t                         slotTable[ADRV904X_JESD_MAX_CARRIER_SLOTS];         /*!< JESD slot mapping */
    uint8_t                         maxSlot;                                            /*!< number of slots used */
    uint32_t                        slotValid_lower;                                    /*!< bitmap indicating slot validity; bits 0 to 31 */
    uint32_t                        slotValid_upper;                                    /*!< bitmap indicating slot validity; bits 32 to 63 */
    uint8_t                         cducBypass;                                         /*!< bypass entire cDUC */
    uint8_t                         carriersEnabled;                                    /*!< bitmap of enabled carriers */
    uint8_t                         cducClkDiv;                                         /*!< clock divide for cDUC processing */
    uint8_t                         interfaceClkDiv;                                    /*!< clock divide for internal interface to Jrx  */
    uint8_t                         bandRatio[ADRV904X_DUC_NUM_BAND];                   /*!< rate relationship of cDUC to DUC-0/1 */
    uint8_t                         delayComp;                                          /*!< bitmap enable for delay compensation */
    uint8_t                         delayMem;                                           /*!< bitmap enable for delay memory */
    uint8_t                         delayChain;                                         /*!< bitmap enable for delay memory daisy chain */
    uint8_t                         delayCarrierSelect[ADRV904X_NUM_CF_DELAY_FIFOS];    /*!< delay memory carrier selection */
    uint16_t                        delayValue[ADRV904X_NUM_CF_DELAY_FIFOS];            /*!< delay compensation value */
    uint8_t                         bandAttenuation[ADRV904X_DUC_NUM_BAND];             /*!< band attenuation to ensure digital full scale is not exceeded */
    uint8_t                         pwrMeasClkDiv;                                      /*!< clock divide for cDUC power measurement block */
    uint16_t                        txFlushCount;                                       /*!< number of cycles required to zero output of cDUC */

    uint8_t                         reserved[1];                                        /*!< Reserve */
} adrv904x_CarrierDucConfig_t;
ADI_ADRV904X_PACK_FINISH


/****************************************************************************
* Tx DFE (digital front-end) CFR (crest factor reduction) Pulse Data structure.
****************************************************************************/
ADI_ADRV904X_PACK_START
/*!< adrv904x_TxDfeCfrCorrectionPulse:  structure of Tx Dfe Cfr Correction pulse configuration parameters */
typedef struct adrv904x_TxDfeCfrCorrectionPulse
{
	int16_t coeffRealHalfPulse[ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN];         /*!< An array consisting of the first half of the Real part of the complex CFR correction pulse coefficients */
	int16_t coeffImagHalfPulse[ADRV904X_MAX_CFR_CORRECTION_PULSE_LEN];         /*!< An array consisting of the first half of the Imag part of the complex CFR correction pulse coefficients */
	uint16_t numCoeffs;                                                        /*!< No. of coefficients contained in coeffReal and coeffImaginary arrays */
	uint8_t pulseSelectMask;                                                   /*!< This field selects which pulses to write. adi_adrv904x_CfrPulseSel_e should be used to create this mask.
                                                                                    User can select multiple pulses */
	uint8_t ramSelectMask;                                                     /*!< Selects the RAM's to write the pulse data. adi_adrv904x_CfrPulseRamSel_e should be used to create this mask.
																			        Please check adi_adrv904x_CfrPulseRamSel_e for detailed info */
}adrv904x_TxDfeCfrCorrectionPulse_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Tx DFE (digital front-end) CFR (crest factor reduction) structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_TxDfeCfrConfig:  structure of Tx Dfe Cfr configuration parameters */
typedef struct adrv904x_TxDfeCfrConfig
{
    uint32_t                           inputRate_kHz;                                    /*!< input sample rate in kHz */
    uint8_t                            cfr0Enable[ADRV904X_NUM_CFR_ENGINES];             /*!< enable engine (doesn't change delay) */
    uint8_t                            cfr0Bypass[ADRV904X_NUM_CFR_ENGINES];             /*!< bypass filter (including delay) */
    uint8_t                            cfr0Interpolation;                                /*!< log2 interpolation (shared by all engines) */
    uint16_t                           cfr0HalfPulseLength;                              /*!< number of samples stored in pulse RAM */
    uint16_t                           cfr0DelayValue[ADRV904X_NUM_CFR_ENGINES];         /*!< number of samples in delay FIFO */
    double                             cfr0EvmWeight[ADRV904X_NUM_TX_CARRIERS];          /*!< EVM weighting (no bitfield) */
	uint8_t                            cfr0NoCareSampleCount[ADRV904X_NUM_CFR_ENGINES];  /*!< maximum number of forgiven samples for cfr engine X */
	uint8_t                            cfr0PeakDuration[ADRV904X_NUM_CFR_ENGINES];       /*!< maximum number of samples that can be a part of a peak group */
    uint8_t                            cfr1Enable[ADRV904X_NUM_CFR_ENGINES];             /*!< enable engine (doesn't change delay) */
    uint8_t                            cfr1Bypass[ADRV904X_NUM_CFR_ENGINES];             /*!< bypass filter (including delay) */
    uint8_t                            cfr1Interpolation;                                /*!< log2 interpolation (shared by all engines) */
    uint16_t                           cfr1HalfPulseLength;                              /*!< number of samples stored in pulse RAM */
    uint16_t                           cfr1DelayValue[ADRV904X_NUM_CFR_ENGINES];         /*!< number of samples in delay FIFO */
    double                             cfr1EvmWeight[ADRV904X_NUM_TX_CARRIERS];          /*!< EVM weighting (no bitfield) */
	uint8_t                            cfr1NoCareSampleCount[ADRV904X_NUM_CFR_ENGINES];  /*!< maximum number of forgiven samples for cfr engine X */
	uint8_t                            cfr1PeakDuration[ADRV904X_NUM_CFR_ENGINES];       /*!< maximum number of samples that can be a part of a peak group */
    uint8_t                            inputRateOver500MHz;                              /*!< input rate > 500 MHz */
    uint8_t                            clk1Div;                                          /*!< clock divide for Pulse RAMs and Processing */
    uint8_t                            clk2Div;                                          /*!< clock divide for Input / Output / Delay RAM */
    uint8_t                            peakDetectClkDiv;                                 /*!< clock divide for Peak Detector */
	adrv904x_TxDfeCfrCorrectionPulse_t cfr0PulseData;                                    /*!< cfr0 pulse data */
	adrv904x_TxDfeCfrCorrectionPulse_t cfr1PulseData;                                    /*!< cfr1 pulse data */

    uint8_t                            reserved[2];                                      /*!< Reserve */
} adrv904x_TxDfeCfrConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Tx DFE (digital front-end) DPD (digital pre-distortion) structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_TxDfeDpdConfig:  structure of Tx Dfe Dpd configuration parameters */
typedef struct adrv904x_TxDfeDpdConfig
{
    uint32_t actRate_kHz;                                                       /*!< actuator sample rate in kHz */
    uint8_t  actEnable;                                                         /*!< enable actuator */
    uint8_t  preHb1Enable;                                                      /*!< enable HB1 interpolator */
    uint8_t  preHb2Enable;                                                      /*!< enable HB2 interpolator */
    uint8_t  postHb1Enable;                                                     /*!< enable HB1 decimator */
    uint8_t  clk0Div;                                                           /*!< clock divide for input of pre HB1 */
    uint8_t  clk1Div;                                                           /*!< clock divide for output of pre HB1 */
    uint8_t  actClkDiv;                                                         /*!< clock divide for Actuator */
    uint8_t  doutClkDiv;                                                        /*!< clock divide for output of post HB1 */
    uint8_t  gmpTxCaptureSelect;                                                /*!< GMP Tx data path capture point mux select */
    uint8_t  ctEnable;                                                          /*!< Charge Trapping DPD enable */
    uint8_t  ctFineDelay[ADRV904X_NUM_CT_DPD_DELAY_LINES];                      /*!< Charge Trapping DPD delay line fine delay values */
    uint8_t  ctCoarseDelay[ADRV904X_NUM_CT_DPD_DELAY_LINES];                    /*!< Charge Trapping DPD delay line coarse delay values */
    uint8_t  ctDpDelay;                                                         /*!< Charge Trapping DPD datapath delay */
    uint8_t  ctDpFineDelay;                                                     /*!< Charge Trapping DPD datapath fine delay */
    uint8_t  vswrTxCaptureSelect;                                               /*!< VSWR Tx data path capture point mux select */
    uint8_t  vswrRxCaptureSelect;                                               /*!< VSWR Rx data path capture point mux select */
    uint8_t  vswrDataGenClkDiv;                                                 /*!< clock divide for VSWR data generator */

    uint8_t  reserved[1];                                                       /*!< Reserve */
} adrv904x_TxDfeDpdConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* DFE (digital front-end) Tx configuration structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeTxConfig:  structure of Tx Dfe configuration parameters */
typedef struct adrv904x_DfeTxConfig
{
    uint8_t                      txduc1Enable;                                   /*!<   include DUC1 output into datapath */
    adrv904x_CarrierDucConfig_t  cduc;                                           /*!<   carrier digital up-convert */
    adrv904x_TxDfeCfrConfig_t    cfr;                                            /*!<   crest factor reduction */
    adrv904x_TxDfeDpdConfig_t    dpd;                                            /*!<   digital pre-distortion actuator */
    uint16_t                     cfrOutToPreDpdDelayNormFs4x;                    /*!<   delay from cfr out to capture point (HB1 or HB2) adjusted to hsdig */
    uint16_t                     capPointToPostDpdOutDelayNormFs4x;              /*!<   delay from capture point (HB1 or HB2) to post dpd hb decimator adjusted to hsdig */
    uint16_t                     cfrOutToPostDpdOutDelayNormFs4x;                /*!<   delay from cfr out to post dpd hb decimator adjusted to hsdig */
    uint8_t                      cicSecondInterpRatio;                           /*!<   second stage CIC interpolator ratio */
    uint8_t                      ctcPreDpdInputSelect;                           /*!<   Ctc Pre DPD mux select. 1:Hb1 ouput , 0:Hb2 output */
} adrv904x_DfeTxConfig_t;
ADI_ADRV904X_PACK_FINISH
/****************************************************************************
* DFE (digital front-end) Rx configuration structure.
****************************************************************************/
    ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeRxConfig:  structure of Rx Dfe configuration parameters */
typedef struct adrv904x_DfeRxConfig
{
    adrv904x_CarrierDdcConfig_t cddc;                                           /*!<  carrier digital down-convert */

    uint8_t                     reserved[4];                                    /*!< Reserve */
} adrv904x_DfeRxConfig_t;
ADI_ADRV904X_PACK_FINISH
/****************************************************************************
* Radio Sequencuer structure.
****************************************************************************/
/* _ADI_SVC_PROFILE_SPECIFIC_START */

typedef enum
{
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_0 = 0,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_1 = 1,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_2 = 2,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_2_ECP = 3,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_3 = 4,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_4 = 5,
    _ADI_SVC_PROFILE_RADIO_SEQ_NUMEROLOGY_INVALID = 6
} _adi_svc_ProfileRadioSeqNumerology_e;

/* _ADI_SVC_PROFILE_SPECIFIC_END */ 

    ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeRadioSequencerConfig:  structure of Dfe Radio Sequencer configuration parameters */
typedef struct adrv904x_DfeRadioSequencerConfig
{
    uint8_t  radioSeqEn;                                                            /*!< Radio sequencer top level enable */
    uint8_t  sampleClkEn;                                                           /*!< Radio sequencer sampling clock enable */
    uint32_t sampleClkFreqInKhz;                                                    /*!< Radio sequencer sampling clock frequency in kHz. */
    uint8_t  sampleClkDiv;                                                          /*!< Radio sequencer sampling clock. Power of 2 encoding */
    uint8_t  ssbSyncMode;                                                           /*!< Radio sequencer SSB frame sync mode */
    uint32_t ssbSyncPeriod;                                                         /*!< Radio sequencer SSB sync duration */
    uint32_t numerology;                                                            /*!< Radio sequencer numerology */	
    uint16_t symWidth;                                                              /*!< Radio sequencer nominal symbol duration */
    uint16_t extSymWidth;                                                           /*!< Radio sequencer extended symbol duration */
    uint16_t extSymSpacing;                                                         /*!< Radio sequencer spacing between consecutive extended symbols */
    uint8_t  reserved[2];                                                           /*!< Reserve */
}adrv904x_DfeRadioSequencerConfig_t;
ADI_ADRV904X_PACK_FINISH


/****************************************************************************
* FSC structure.
****************************************************************************/
ADI_ADRV904X_PACK_START
/*!< adrv904x_FscConfig:  structure of Fsc configuration parameters */
typedef struct adrv904x_FscConfig
{
    uint8_t  lbn;          /*!< Tx loopback samples per FSC clock - numerator            */
    uint8_t  lbd;          /*!< Tx loopback samples per FSC clock - denominator exponent */
    uint8_t  lbw;          /*!< Tx loopback samples per FSC clock - denominator weight   */
    uint8_t  txn;          /*!< Tx samples per FSC clock - numerator                     */
    uint8_t  txd;          /*!< Tx samples per FSC clock - denominator exponent          */
    uint8_t  txw;          /*!< Tx samples per FSC clock - denominator weight            */
    uint8_t  rxn;          /*!< Rx samples per FSC clock - numerator                     */
    uint8_t  rxd;          /*!< Rx samples per FSC clock - denominator exponent          */
    uint8_t  rxw;          /*!< Rx samples per FSC clock - denominator weight            */
    uint8_t  fastClkDiv;   /*!< Fast clock divider                                       */
    uint8_t  slowClkDiv;   /*!< Slow clock divider                                       */
    uint32_t fastClk_kHz;  /*!< Fast clock in kHz                                        */
    uint32_t slowClk_kHz;  /*!< Slow clock in kHz                                        */
} adrv904x_FscConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Profile information that will define the Receiver channel.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_RxConfig
{
    uint32_t                ibw_kHz;                       /*!<  Rx instantaneous bandwidth in kHz.  */
    uint32_t                ibwCenterFreq_kHz;             /*!<  Rx instantaneous center freq in kHz.  */
    uint32_t                tia1dB_Bw_kHz;                 /*!<  Rx TIA bandwith (1db) in kHz.  */
    uint32_t                tia3dB_Bw_kHz;                 /*!<  Rx TIA bandwith (3db) in kHz.  */
    uint32_t                adcClockRate_kHz;              /*!<  ADC clock rate in kHz */
    uint32_t                iQRate_kHz;                    /*!<  I/Q data rate at RxQEC in kHz */
    uint32_t                rxOutputRate_kHz;              /*!<  Output data rate in kHz  */
    uint32_t                pfirOutputRate_kHz;            /*!<  Output data at the Pfir in kHz  */
    uint32_t                routRate_kHz;                  /*!<  ROUT in kHz  */
    uint8_t                 totalDecimation;               /*!<  Total RX decimation  */
    adrv904x_NcoShifter_t   rxddc[ADRV904X_DDC_NUM_BAND];  /*!<  NCO Shifter  */
    uint8_t                 dpFilterSel;                   /*!<  Selectes Dec Mode */
    uint8_t                 reserved1;                     /*!<  Reserved  */
    uint8_t                 hb2Enable;                     /*!<  HB 2 enable setting  */
    uint8_t                 rxRinClkDiv;                   /*!<  Rx RIN clock divider setting  */
    uint8_t                 rxAgcClkDiv;                   /*!<  Rx AGC clock setting*/
    uint8_t                 rxDcOffsetDiv;                 /*!<  Rx DC Offset setting */
    uint8_t                 rxFir1InClkDiv;                /*!<  Rx FIR1 clock divider setting  */
    uint8_t                 rxFir2InClkDiv;                /*!<  Rx FIR2 clock divider setting  */
    uint8_t                 rxHb2InClkDiv;                 /*!<  Rx HB2 in clock divider setting  */
    uint8_t                 rxHb2OutClkDiv;                /*!<  Rx HB2 out clock divider setting  */
    uint32_t                rxHb2OutClk_kHz;               /*!<  Rx HB2 out clock setting  */
    uint8_t                 routClkDiv;                    /*!<  Rx ROUT out clock divider setting  */
    adrv904x_RxPfirData_t   rxPfirBank;                    /*!<  Rx PFIR Bank   */
    adrv904x_ClkGenConfig_t clkGenConfig;                  /*!<  Rx clock gen settings */
    uint8_t                 loLeafDiv;                     /*!<  LO Leaf Divider /1 /2 /4 */
    uint8_t                 clkdiv1p5enable;               /*!<  Resampling clock enable 1p5 */
    uint8_t                 clkdiv1p3enable;               /*!<  Resampling clock enable 1p3 */

    uint8_t                 reserved[10];                  /*!<  Reserved  */
} adrv904x_RxConfig_t;
ADI_ADRV904X_PACK_FINISH


/****************************************************************************
* Profile information that will define the Transmitter channel.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_TxConfig
{
    uint32_t                ibw_kHz;                      /*!<   Tx instantaneous bandwidth in kHz.  */
    uint32_t                ibwCenterFreq_kHz;            /*!<   Tx instantaneous center freq in kHz.  */
    uint32_t                butterFilter_kHz;             /*!<   Tx butter filter BW for TxBBF in kHz */
    uint32_t                txDacRate_kHz;                /*!<   Tx dac rate in kHz*/
    uint32_t                txOutputRate_kHz;             /*!<   Tx output rate in kHz*/
    adrv904x_NcoShifter_t   txduc[ADRV904X_DUC_NUM_BAND]; /*!<   ncoShifterA NCO Shifter  */
    uint8_t                 txFir1Enable;                 /*!<   Transmitter FIR 1 setting.  */
    uint8_t                 txFir2Enable;                 /*!<   Transmitter FIR 2 setting.  */
    uint8_t                 txFir3Enable;                 /*!<   Transmitter FIR 3 setting.  */
    uint8_t                 txInt3Enable;                 /*!<   Transmitter interpolate by 3 setting.  */
    uint8_t                 txPfirClkDiv;                 /*!<   PFIR Clock Divider setting */
    uint32_t                txPfirClk_kHz;                /*!<   PFIR Clock setting in kHz*/
    uint8_t                 txDacClkDiv;                  /*!<   Tx DAC Clock Divider setting */
    uint8_t                 txToutClockDiv;               /*!<   Clock divider for output clock rate of Tx datapath (to DAC input)  */
    uint8_t                 txFir2OutClkDiv;              /*!<   FIR2 output rate clock divider value  */
    uint8_t                 txFir1InClkDiv;               /*!<   FIR1 input rate clock divider value  */
    uint8_t                 txFir2InClkDiv;               /*!<   FIR1 input rate clock divider value  */
    uint8_t                 txFir1OutClkDiv;              /*!<   FIR1 output rate clock divider value  */
    uint8_t                 txAttenClkDiv;                /*!<   Tx Atten clock divider */
    uint8_t                 lpbkHb1OutClkDiv;             /*!<   Loopback HB1 divider setting */
    uint32_t                lpbkHb1OutRate_kHz;           /*!<   Loopback HB1 Output rate in kHz*/
    uint8_t                 lpbkHb1enb;                   /*!<   Loopback 1 enable */
    uint8_t                 lpbkReSampleEnb;              /*!<   Resample filter enable */
    int16_t                 lbbkResampFilter[10];         /*!<   Resample filter Coef */
    int32_t                 lpbkCoarseNcoFtw;             /*!<   Loopback course NCO setting */
    uint32_t                lpbkAdcClkRate_kHz;           /*!<   Loopback ADC clock setting in kHz*/
    adrv904x_ClkGenConfig_t txClkGenConfig;               /*!<   Tx clock gen settings */
    adrv904x_ClkGenConfig_t lpbkClkGenConfig;             /*!    Loopback clock gen settings */
    uint8_t                 overloadHb1ThresPre;          /*!    The pre-threshold value to be compared with magnitude. */
    uint8_t                 overloadHb1Thres;             /*!    The threshold value to be compared with I^2+Q^2 when in power mode, after pre_threshold met. */
    uint8_t                 overloadMode;                 /*!    When this bit is set, the I^2+Q^2 power is compared with the threshold(power mode) */
    uint8_t                 overloadEn;                   /*!    When this bit is set, the overload detector at output of lpbk hb1 is enabled. */
    uint8_t                 peakCountThresHb1;            /*!    The threshold value used to be compared with the peak counter. */
    uint8_t                 peakCountSpacing;             /*!    Defines the spacing between two peaks counted towards the peak count threshold. */
    uint8_t                 peakWinSize;                  /*!    Defines the window of measurement size(number of cycles) */
    uint8_t                 peakCount;                    /*!    With in the peak_window_size_hb1, this bitfield defines the minimum number of peaks need */
    uint8_t                 peakExperation;               /*!    Peak counter is cleared every peak count expiration period */
    adrv904x_FscConfig_t    fscConfig;                    /*!<   FSC config settings */
    uint8_t                 loLeafDiv;                    /*!<   LO Leaf Divider /1 /2 /4 */
    uint8_t                 quadDiv;                      /*!<   /2 or /4 HRM mixer divider */
    uint8_t                 dacPowerDownDiv_i;            /*!<   DAC PowerDown divider I */
    uint8_t                 dacPowerDownDiv_q;            /*!<   DAC PowerDown divider Q */
    uint8_t                 dacPowerUpDiv_i;              /*!<   DAC PowerUp divider I */
    uint8_t                 dacPowerUpDiv_q;              /*!<   DAC PowerUp divider Q */
    uint32_t                maxTxSynBw_kHz;               /*!<   Maximum Tx synthesis bandwidth */
    uint32_t                maxTxInstBw_kHz;              /*!<   Maximum Tx instantaneous bandwidth */
    uint32_t                maxTxTestToneBw_kHz;          /*!<   Maximum BW over which a test tonne can be transmitted */
    uint8_t                 txAttenDelay;                 /*!<   Tx atten delay */
    uint8_t                 lowNoiseModeEnb;              /*!<   Low noise mode operation state */

    uint8_t                 reserved[2];                  /*!<   Reserved  */
} adrv904x_TxConfig_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Profile information that will define the Observation Receiver channel.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_OrxConfig
{
    uint32_t                sbw_kHz;                                             /*!<    Orx instantaneous bandwidth in kHz.  */
    uint32_t                sbwCenterFreq_kHz;                                   /*!<    Orx instantaneous center freq in kHz.  */
    uint32_t                adcClockRate_kHz;                                    /*!<    ADC Clock rate in kHz */
    uint32_t                iQRate_kHz;                                          /*!<    I/Q Sample rate in kHz */
    uint32_t                orxOutputRate_kHz;                                   /*!<    Orx Output Rate in kHz.  */
    uint32_t                orxRinClockRate_kHz;                                 /*!<    Orx RIN Clock Rate in kHz */
    uint32_t                orxHb2OutRate_kHz;                                   /*!<    Orx HB2 effective output rate (sample rate, not clock rate) */
    uint32_t                orxHb1OutRate_kHz;                                   /*!<    Orx HB1 effective output rate */
    uint8_t                 totalDecimation;                                     /*!<    Total Orx decimation  */
    uint8_t                 dec3Enable;                                          /*!<    Decimation 3 enable setting (NOT SURE IF THESE ARE NEEDED) */
    uint8_t                 stageMode;                                           /*!<    Stage Mode  */
    uint8_t                 fir2Enable;                                          /*!<    Fir2 enable (NOT SURE IF THESE ARE NEEDED) */
    uint8_t                 fir1Enable;                                          /*!<    Fir 1 enable  (NOT SURE IF THESE ARE NEEDED)*/
    uint8_t                 hb2Enable;                                           /*!<    HB2 enable setting.  */
    uint8_t                 hb1Enable;                                           /*!<    HB1 enable setting.  */
    uint8_t                 rinEnable;                                           /*!<    RIN enable setting.  */
    uint8_t                 orxRinClkDiv;                                        /*!<    ORX Rin clock divider  */
    uint8_t                 orxRoutClkDiv;                                       /*!<    ORX Rout clock divider  */
    uint8_t                 orxFir1ClkDiv;                                       /*!<    ORX Fir in clock divider  */
    uint8_t                 orxHb2OutClkDiv;                                     /*!<    ORX HB2 out clock divider  */
    uint8_t                 orxHb2InClkDiv;                                      /*!<    ORX HB2 in clock divider  */
    uint8_t                 stageModeClkDiv;                                     /*!<    ORX Stage Mode clock divider  */
    uint8_t                 orxHb1OutClkDiv;                                     /*!<    ORX HB1 out clock divider  */
    int32_t                 orxAdcOutNcoFreq_kHz;                                /*!<    Default Orx ADC output NCO frequency in KHz */
    int32_t                 orxDpNcoFreq_khz;                                    /*!<    Default Orx Datapath NCO frequency in KHz */
    int32_t                 orxAdcOutNcoFreqArray_kHz[ADRV904X_NUM_TX_PROFILES]; /*!<    Orx ADC output NCO frequency in KHz, per Tx Profile */
    int32_t                 orxDpNcoFreqArray_kHz[ADRV904X_NUM_TX_PROFILES];     /*!<    Orx Datapath NCO frequency in KHz, per Tx Profile */
    uint8_t                 orxLolPathDelayHb2Out[ADRV904X_NUM_TX_PROFILES];     /*!<    Tx-to-ORx path delay used by LOL cal (in samples at the Tx PFIR/LOL rate) */
    uint8_t                 orxLolPathDelayHb1Out[ADRV904X_NUM_TX_PROFILES];     /*!<    Tx-to-ORx path delay used by LOL cal (in samples at the Tx PFIR/LOL rate) */
    adrv904x_ClkGenConfig_t clkGenConfig;                                        /*!<    Orx clock gen settings */
    uint8_t                 reserved[9];                                         /*!<    Reserved  */
} adrv904x_OrxConfig_t;
ADI_ADRV904X_PACK_FINISH


/****************************************************************************
* Profile information that will define the Radio Configuration
* Profile struct holds clock rates, divider ratios, sampling rates, etc.
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_RadioProfile
{
    adrv904x_Version_t      deviceProfileVersion;                      /*!<   Device Profile version. Use for easy checking between Configurator and API */
    uint32_t                hsDigFreq_kHz;                             /*!<   System HS Dig clock setting calculated by the API in kHz.  */
    uint32_t                deviceClkFreq_kHz;                         /*!<   System device clock setting, in multiples of 61.44 in kHz.  */
    uint32_t                deviceClkScaledFreq_kHz;                   /*!<   System device clock scaled setting */
    uint32_t                refClkFreq_kHz;                            /*!<   Reference clock frequency in kHz  */
    uint32_t                armClkFreq_kHz;                            /*!<   ARM clock frequency in kHz  */
    uint8_t                 padDiv;                                    /*!<   Pad divider */
    uint8_t                 armClkDiv;                                 /*!<   ARM clock Divider  */
    uint8_t                 armClkDivDevClk;                           /*!<   ARM clock Divider when on DevClk */
    uint8_t                 hsDigDiv;                                  /*!<   HS Dig h/w divider */
    uint8_t                 ncoGoldenClkDiv;                           /*!<   NCO Golden counter divider value */
    uint8_t                 clkGenSel;                                 /*!<   PLL select (Serdes or ClkPLL). Typically ClkPll will be selected but for debug it will be possible to use Serdes.  */
    adrv904x_ClkGenConfig_t clkGenConfig;                              /*!<   System clock gen settings */
    uint8_t                 loPhaseSync;                               /*!<   0 = Dont Sync; 1 = Run Init and sync; 2 = Run init and Continuous tracking  */
    adrv904x_PllConfig_t    clkPll;                                    /*!<   Clock PLL structure  */
    adrv904x_PllConfig_t    rf0Pll;                                    /*!<   RF0 PLL structure (EAST) */
    adrv904x_PllConfig_t    rf1Pll;                                    /*!<   RF1 PLL structure (WEST) */
    uint8_t                 rf0MuxEastTx;                              /*!<   RF0 (East) is connect to Tx when set. */
    uint8_t                 rf0MuxWestTx;                              /*!<   RF0 (West) is connect to Tx when set. */
    uint8_t                 rf0MuxEastRx;                              /*!<   RF0 (East) is connect to Rx when set. */
    uint8_t                 rf0MuxWestRx;                              /*!<   RF0 (West) is connect to Rx when set. */
    uint32_t                chanConfig;                                /*!<   Channel config. B0=Tx0, B1=Tx1,--- B7=Tx7, B8=Rx0, B9=Rx1 -- B15=Rx7, B16=Orx0, B17=Orx1  */
    uint8_t                 chanAssign[ADRV904X_NUM_TXRX_CHAN];        /*!<   This is used to reference a channel to a rx/tx profile def'n in rxConfig/txConfig */
    adrv904x_RxConfig_t     rxConfig[ADRV904X_NUM_RX_PROFILES];        /*!<   Receiver profile for 0-3  */
    adrv904x_TxConfig_t     txConfig[ADRV904X_NUM_TX_PROFILES];        /*!<   Transmitter profile for 0-3 */
    adrv904x_TxPfirData_t   txPfirBank[ADRV904X_NUM_TXRX_CHAN];        /*!<   Tx PFIR Bank */
    uint8_t                 rxTxCpuConfig[ADRV904X_NUM_TXRX_CHAN];     /*!<   Defines which CPU each channel is assigned to  */
    adrv904x_OrxConfig_t    orxConfig[ADRV904X_NUM_ORX_PROFILES];      /*!<   Observation Receiever profile for channels 0-1  */
    uint8_t                 orxCpuConfig[ADRV904X_NUM_ORX_PROFILES];   /*!<   This defines which cpu an Orx channel is allocated to.   */
    adrv904x_PllConfig_t    serdesPll;                                 /*!<   Serdes PLL structure  */
    adrv904x_JesdSettings_t jesdProfile;                               /*!<   Jesd configuration  */
    uint8_t                 pid;                                       /*!<   PID */
    uint8_t                 productId[20];                             /*!<   Product ID */
    uint8_t                 featureMask[16];                           /*!<   128 bits of Feature Mask */
    uint8_t                 reserved[8];                               /*!<   Reserved (adjust so Device profile size is multiple of 4 bytes) */
    uint32_t                profileChecksum;                           /*!<   Checksum of the entire profile excluding ADC profiles, Using the CRC32 algorithm  */
} adrv904x_RadioProfile_t;
ADI_ADRV904X_PACK_FINISH

ADI_ADRV904X_PACK_START
/*!< adrv904x_DfeProfile structure containing the DFE device profile */
typedef struct adrv904x_DfeProfile
{
    adrv904x_DfeRxConfig_t                  rxConfig[ADRV904X_NUM_RX_PROFILES];          /*!<   Receiver profile for 0-3  */
    adrv904x_DfeTxConfig_t                  txConfig[ADRV904X_NUM_TX_PROFILES];          /*!<   Transmitter profile for 0-3 */
    adrv904x_DfeJesdSettings_t              jesdProfile;                                 /*!<   Jesd configuration  */
    adrv904x_DfeRadioSequencerConfig_t      radioSeqProfile;                             /*!<   Radio Sequencer configuration */
    uint8_t                                 radioDivDuplexMode[ADRV904X_NUM_TXRX_CHAN];  /*!<   This is used to reference a channel's operating mode (TDD or FDD) */	
	uint32_t                                dfeMappingControl;                           /*!<  This variable selects how Tx/Rx/Orx En signals and TxToOrx mapping is controlled. For detailed info please check the adrv904x_DfeMappingControl_e enum documentation */
	
} adrv904x_DfeProfile_t;
ADI_ADRV904X_PACK_FINISH

/****************************************************************************
* Profile information provided by BBIC at init via CONFIG command
****************************************************************************/
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DeviceProfile
{
    adrv904x_RadioProfile_t radioProfile;                              /*!< Radio configuration parameters */
    adrv904x_DfeProfile_t dfeProfile;                                  /*!< DFE configuration parameters */
	uint32_t              profileChecksum;                             /*!< Checksum of the entire profile using the CRC32 algorithm  */
} adrv904x_DeviceProfile_t;
ADI_ADRV904X_PACK_FINISH

#define ADRV904X_DEVICE_PROFILE_SIZE_BYTES    sizeof(adrv904x_DeviceProfile_t) /* about 2440 bytes */
#define ADRV904X_RADIO_PROFILE_SIZE_BYTES     sizeof(adrv904x_RadioProfile_t)
#define ADRV904X_DFE_PROFILE_SIZE_BYTES    sizeof(adrv904x_DfeProfile_t)

/* Constant for adrv904x_RuntimeProfile_t signature1 field */
#define ADRV904X_RUNTIME_PROFILE_SIGNATURE1      (0x55AADD11UL)

ADI_ADRV904X_PACK_START
typedef struct adrv904x_RuntimeProfile
{
    uint32_t signature1;                                         /*!< Magic number to indicate that profile data is valid */
    uint32_t hsDigFreq_kHz;                                      /*!< System HS Dig clock setting in kHz */
    uint32_t txPfirSampleRate_kHz[ADRV904X_NUM_TXRX_CHAN];       /*!< DUC sampling rate for Tx in kHz */
    uint32_t txNcoMixerSampleRate_kHz[ADRV904X_NUM_TXRX_CHAN];   /*!< DUC NCO sampling rate for Tx in kHz */
    uint32_t orxOutputRate_kHz[ADRV904X_NUM_ORX_CHAN];           /*!< Orx output sample rate */
    uint32_t orxHb1OutRate_kHz[ADRV904X_NUM_ORX_CHAN];           /*!< Orx HB1 effective output sample rate */
    uint32_t orxHb2OutRate_kHz[ADRV904X_NUM_ORX_CHAN];           /*!< Orx HB2 effective output sample rate */
    int32_t  orxDpNcoFreqArray_kHz[ADRV904X_NUM_ORX_CHAN];       /*!< Orx NCO freqency in khz */
    uint32_t txLo0Freq_kHz;                                      /*!< Tx Lo0 frequency in kHz */
    uint32_t txLo1Freq_kHz;                                      /*!< Tx Lo1 frequency in kHz */
    uint32_t txChanConfig;                                       /*!< Tx Channel enable/disable bitmask */
    uint32_t rxChanConfig;                                       /*!< Rx Channel enable/disable bitmask */
    uint32_t orxChanConfig;                                      /*!< ORx Channel enable/disable bitmask */

    uint32_t dpdSampleRate_kHz[ADRV904X_NUM_TXRX_CHAN];          /*!< DPD sampling rate in kHz */
    uint8_t  dpdActEnable[ADRV904X_NUM_TXRX_CHAN];               /*!< DPD activate enable */
    uint8_t  dpdPreHb1Enable[ADRV904X_NUM_TXRX_CHAN];            /*!< DPD Pre-HB1 enable */
    uint8_t  dpdPreHb2Enable[ADRV904X_NUM_TXRX_CHAN];            /*!< DPD Pre-HB2 enable */
    uint8_t  dpdPostHb1Enable[ADRV904X_NUM_TXRX_CHAN];           /*!< DPD Post-HB1 enable */
    uint8_t  orxHb2Enable[ADRV904X_NUM_ORX_CHAN];                /*!< ORx HB2 enable */
    uint8_t  pad0;                                               /*!< padding for U32 word alignment */
    uint8_t  pad1;                                               /*!< padding for U32 word alignment */
    uint8_t  cfr0Enable[ADRV904X_NUM_TXRX_CHAN][ADRV904X_NUM_CFR_ENGINES]; /*!< CFR 0 enable */
    uint8_t  cfr1Enable[ADRV904X_NUM_TXRX_CHAN][ADRV904X_NUM_CFR_ENGINES]; /*!< CFR 1 enable */
    uint32_t cfrRate[ADRV904X_NUM_TXRX_CHAN];                    /*!< CFR rate */
    uint32_t actRateKhz[ADRV904X_NUM_TXRX_CHAN];                 /*!< DPD actuator Rate in kHz */
    uint32_t armClkFreq_kHz;                                     /*!< ARM clock frequency in kHz  */
    uint32_t ssbSyncPeriod;                                      /*!< Radio sequencer SSB sync duration */
    uint32_t radioSeqEn;                                         /*!< Radio sequencer top level enable */
    uint32_t numerology;                                         /*!< Radio sequencer numerology */
    uint8_t  radioDivDuplexMode[ADRV904X_NUM_TXRX_CHAN];         /*!< This is used to reference a channel's operating mode (TDD or FDD) */
    uint8_t  rxDdcHb1OutputClkDiv[ADRV904X_NUM_TXRX_CHAN][ADRV904X_DDC_NUM_BAND];           /*!< This is used to calculate sampling rate for main path RSSI blocks */
    uint32_t txCarrierTssiSampleRate_kHz[ADRV904X_NUM_TXRX_CHAN][ADRV904X_NUM_TX_CARRIERS]; /*!< Tx carrier TSSI sample rate in kHz */
    uint32_t rxCarrierRssiSampleRate_kHz[ADRV904X_NUM_TXRX_CHAN][ADRV904X_NUM_RX_CARRIERS]; /*!< Tx carrier TSSI sample rate in kHz */
    uint32_t dfeMappingControl;                                  /*!<  This variable selects how Tx/Rx/Orx En signals and TxToOrx mapping is controlled. For detailed info please check adrv904x_DfeMappingControl_e enum  documentation */
    uint8_t  clk0Div[ADRV904X_NUM_TXRX_CHAN];                    /*!< Clock Divider Ratio for input data of dpd_top cfr clock divide */
    uint8_t  clk1Div[ADRV904X_NUM_TXRX_CHAN];                    /*!< Clock Divider Ratio for output of HB1 Interpolator */
    uint8_t  actClkDiv[ADRV904X_NUM_TXRX_CHAN];                  /*!< Clock Divider Ratio for DPD Actuator Clock */
    uint8_t  doutClkDiv[ADRV904X_NUM_TXRX_CHAN];                 /*!< Clock Divider Ratio for Output Data of DPD Actuator Block */
    uint16_t cfrOutToPreDpdDelayNormFs4x[ADRV904X_NUM_TXRX_CHAN];           /*!<   delay from CFR output to pre DPD capture point (HB1 or HB2) delay adjusted to hsdig */
    uint16_t capPointToPostDpdOutDelayNormFs4x[ADRV904X_NUM_TXRX_CHAN];     /*!<   delay from pre DPD capture point (HB1 or HB2) to post dpd hb decimator adjusted to hsdig */
    uint16_t cfrOutToPostDpdOutDelayNormFs4x[ADRV904X_NUM_TXRX_CHAN];       /*!<   delay from cfr out to post dpd hb decimator adjusted to hsdig */
    uint8_t  cicSecondInterpRatio[ADRV904X_NUM_TXRX_CHAN];                  /*!<   second stage CIC interpolator ratio */
    uint8_t  ctcPreDpdInputSelect[ADRV904X_NUM_TXRX_CHAN];                  /*!< Ctc Pre DPD mux select. 1:Hb1 ouput , 0:Hb2 output */
    /* NOTE: If parameters are added, they must align to U32 or U64 word boundaries.
     * Be sure to add padding bytes as necessary.
     * Example: add "uint16_t someU16Param;" would require 2 uint8_t or 1 uint16_t padding parameter.
     */
} adrv904x_RuntimeProfile_t;
ADI_ADRV904X_PACK_FINISH


/****************************************************************************
* Mailbox buffer shared between DFE and radio CPUs
****************************************************************************/
    ADI_ADRV904X_PACK_START
typedef struct adrv904x_MailboxDfeToRadio
{
    uint32_t buffer[4];                             /*!< Mailbox Buffer for A55 to M4 CPU */
} adrv904x_MailboxDfeToRadio_t;
ADI_ADRV904X_PACK_FINISH

#endif /* ADRV904X_CPU_DEVICE_PROFILE_TYPES_H__ */


