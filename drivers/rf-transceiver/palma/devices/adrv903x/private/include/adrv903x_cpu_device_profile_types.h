/**
 * \file adrv903x_cpu_device_profile_types.h
 *
 * \brief   Contains ADRV903X Device Profile type definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_DEVICE_PROFILE_TYPES_H__
#define __ADRV903X_CPU_DEVICE_PROFILE_TYPES_H__

#include "adrv903x_cpu_device_profile_jesd_types.h"
#include "adrv903x_cpu_device_profile_pfir_types.h"
#include "adi_adrv903x_version_types.h"
#include "adi_adrv903x_platform_pack.h"

#define ADRV903X_PROFILE_TX_CHAN_MASK        (0x000000FFU)
#define ADRV903X_PROFILE_TX_CHAN_POS         (0U)

#define ADRV903X_PROFILE_RX_CHAN_MASK        (0x0000FF00U)
#define ADRV903X_PROFILE_RX_CHAN_POS         (8U)

#define ADRV903X_PROFILE_ORX_CHAN_MASK       (0x00030000U)
#define ADRV903X_PROFILE_ORX_CHAN_POS        (16U)

#define ADRV903X_PROFILE_CHAN_CONFIG_MASK    (ADRV903X_PROFILE_TX_CHAN_MASK   \
                                              | ADRV903X_PROFILE_RX_CHAN_MASK \
                                              | ADRV903X_PROFILE_ORX_CHAN_MASK)

/* Channel types used throughout the system */
typedef enum adrv903x_ChannelType
{
    ADRV903X_CH_TYPE_RX,
    ADRV903X_CH_TYPE_TX,
    ADRV903X_CH_TYPE_ORX,
    ADRV903X_CH_TYPE_LOOPBACK,
    ADRV903X_CH_TYPE_MAX
} adrv903x_ChannelType_e;

/* Enumeration for DDC modes, Rx and Orx */
typedef enum adrv903x_RxDdc
{
    ADRV903X_RXDDC_BYPASS = 0u,           /*!< No Half Band Enabled */
    ADRV903X_RXDDC_FILTERONLY,            /*!< Half Band Filters only */
    ADRV903X_RXDDC_INT2,                  /*!< Half Band Interpolation by 2 */
    ADRV903X_RXDDC_DEC2,                  /*!< Half Band Decimate by 2 */
    ADRV903X_RXDDC_BYPASS_REALIF,         /*!< No Half Band Enabled  (Real I/F MODE)*/
    ADRV903X_RXDDC_FILTERONLY_REALIF,     /*!< Half Band Filters only (Real I/F MODE) */
    ADRV903X_RXDDC_INT2_REALIF,           /*!< Half Band Interpolation by 2 (Real I/F MODE) */
    ADRV903X_RXDDC_DEC2_REALIF            /*!< Half Band Decimate by 2 (Real I/F MODE) */
} adrv903x_RxDdc_e;

/* Enumeration for RX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_RxChannelNum
{
    ADRV903X_RX_CH_1 = 0u,        /*!< Rx channel # 1 index   */
    ADRV903X_RX_CH_2,             /*!< Rx channel # 2 index   */
    ADRV903X_RX_CH_3,             /*!< Rx channel # 3 index   */
    ADRV903X_RX_CH_4,             /*!< Rx channel # 4 index   */
    ADRV903X_RX_CH_5,             /*!< Rx channel # 5 index   */
    ADRV903X_RX_CH_6,             /*!< Rx channel # 6 index   */
    ADRV903X_RX_CH_7,             /*!< Rx channel # 7 index   */
    ADRV903X_RX_CH_8,             /*!< Rx channel # 8 index   */
    ADRV903X_RX_CHAN_LEN          /*!< Total number of Rx Channels supported  */
} adrv903x_RxChannelNum_e;

/* Enumeration for TX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_TxChannelNum
{
    ADRV903X_TX_CH_1 = 0u,        /*!< Tx channel # 1 index   */
    ADRV903X_TX_CH_2,             /*!< Tx channel # 2 index   */
    ADRV903X_TX_CH_3,             /*!< Tx channel # 3 index   */
    ADRV903X_TX_CH_4,             /*!< Tx channel # 4 index   */
    ADRV903X_TX_CH_5,             /*!< Tx channel # 5 index   */
    ADRV903X_TX_CH_6,             /*!< Tx channel # 6 index   */
    ADRV903X_TX_CH_7,             /*!< Tx channel # 7 index   */
    ADRV903X_TX_CH_8,             /*!< Tx channel # 8 index   */
    ADRV903X_TX_CHAN_LEN          /*!< Total number of Tx Channels supported  */
} adrv903x_TxChannelNum_e;

/* Enumeration for ORX channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_OrxChannelNum
{
    ADRV903X_ORX_CH_1 = 0u,        /*!< Orx channel # 1 index */
    ADRV903X_ORX_CH_2,             /*!< Orx channel # 2 index */
    ADRV903X_ORX_CHAN_LEN          /*!< Total number of Orx Channels supported  */
} adrv903x_OrxChannelNum_e;

/* Enumeration for RX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_RxChannelIdx
{
    ADRV903X_RX_CH_IDX_0 = 0u,     /*!< Rx channel index # 0 */
    ADRV903X_RX_CH_IDX_1,          /*!< Rx channel index # 1 */
    ADRV903X_RX_CH_IDX_2,          /*!< Rx channel index # 2 */
    ADRV903X_RX_CH_IDX_3,          /*!< Rx channel index # 3 */
    ADRV903X_RX_CH_IDX_4,          /*!< Rx channel index # 4 */
    ADRV903X_RX_CH_IDX_5,          /*!< Rx channel index # 5 */
    ADRV903X_RX_CH_IDX_6,          /*!< Rx channel index # 6 */
    ADRV903X_RX_CH_IDX_7,          /*!< Rx channel index # 7 */
    ADRV903X_RX_CHAN_IDX_LEN       /*!< Total number of Rx Channels supported  */
} adrv903x_RxChannelIdx_e;

/* Enumeration for TX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_TxChannelIdx
{
    ADRV903X_TX_CH_IDX_0 = 0u,        /*!< Tx channel index # 0 */
    ADRV903X_TX_CH_IDX_1,             /*!< Tx channel index # 1 */
    ADRV903X_TX_CH_IDX_2,             /*!< Tx channel index # 2 */
    ADRV903X_TX_CH_IDX_3,             /*!< Tx channel index # 3 */
    ADRV903X_TX_CH_IDX_4,             /*!< Tx channel index # 4 */
    ADRV903X_TX_CH_IDX_5,             /*!< Tx channel index # 5 */
    ADRV903X_TX_CH_IDX_6,             /*!< Tx channel index # 6 */
    ADRV903X_TX_CH_IDX_7,             /*!< Tx channel index # 7 */
    ADRV903X_TX_CHAN_IDX_LEN          /*!< Total number of Tx Channels supported  */
} adrv903x_TxChannelIdx_e;

/* Enumeration for ORX channel indices - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_OrxChannelIdx
{
    ADRV903X_ORX_CH_IDX_0 = 0u,        /*!< Orx channel index # 0 */
    ADRV903X_ORX_CH_IDX_1,             /*!< Orx channel index # 1 */
    ADRV903X_ORX_CHAN_IDX_LEN          /*!< Total number of Orx Channels supported  */
} adrv903x_OrxChannelIdx_e;

/* Enumeration for serdes lane numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum adrv903x_SerDesLaneNum
{
    ADRV903X_SERDES_LANE_1 = 0u,        /*!< SERDES LANE # 1 index   */
    ADRV903X_SERDES_LANE_2,             /*!< SERDES LANE # 2 index   */
    ADRV903X_SERDES_LANE_3,             /*!< SERDES LANE # 3 index   */
    ADRV903X_SERDES_LANE_4,             /*!< SERDES LANE # 4 index   */
    ADRV903X_SERDES_LANE_5,             /*!< SERDES LANE # 5 index   */
    ADRV903X_SERDES_LANE_6,             /*!< SERDES LANE # 6 index   */
    ADRV903X_SERDES_LANE_7,             /*!< SERDES LANE # 7 index   */
    ADRV903X_SERDES_LANE_8,             /*!< SERDES LANE # 8 index   */
    ADRV903X_SERDES_LANE_LEN            /*!< Total number of SerDes Lanes supported  */
} adrv903x_SerdesLaneNum_e;

#define ADRV903X_RX_CHAN_LEN_CPU        (ADRV903X_RX_CHAN_LEN / 2u)
#define ADRV903X_TX_CHAN_LEN_CPU        (ADRV903X_TX_CHAN_LEN / 2u)
#define ADRV903X_ORX_CHAN_LEN_CPU       (ADRV903X_ORX_CHAN_LEN / 2u)
#define ADRV903X_SERDES_LANE_LEN_CPU    (ADRV903X_SERDES_LANE_LEN / 2u)

/* Map human-readable symbols to the the register values use in the hardware and
 * required in adrv903x_DeviceProfile_t struct.
 * Note not all values are supported by all ClkGen instances. */
#define ADRV903X_CLKGEN_DIV1_OFF    0u
#define ADRV903X_CLKGEN_DIV1_DIVBY2 1u
#define ADRV903X_CLKGEN_DIV1_DIVBY3 2u
#define ADRV903X_CLKGEN_DIV1_DIVBY4 3u
#define ADRV903X_CLKGEN_DIV1_DIVBY5 4u

/* Enumeration for EXT LO Select Device Profile def'n */
typedef enum
{
    ADRV903X_LOGEN_SEL_TX = 0x01u,               /*!< Set the PLL to Tx channels */
    ADRV903X_LOGEN_SEL_RX = 0x02u                /*!< Set the PLL to Rx channels */
} adrv903x_LogenRxTxSel_e;


typedef enum
{
    ADRV903X_LOGEN_INTERNAL_IN  = 0x01u,           /*!< Set Logen to internal mode */
    ADRV903X_LOGEN_EXTERNAL_IN  = 0x02u,           /*!< Set Logen to external IN mode */
    ADRV903X_LOGEN_EXTERNAL_OUT  = 0x04u           /*!< Set Logen to external OUT mode */
} adrv903x_LogenInOutSel_e;


/* Enumeration for DUC channel number */
typedef enum adrv903x_DucNumber
{
    ADRV903X_DUC_BAND_0 = 0u,              /*!< DUC channel 0 */
    ADRV903X_DUC_NUM_BAND
} adrv903x_DucNumber_e;

/* Enumeration for DDC channel number */
typedef enum adrv903x_DdcNumber
{
    ADRV903X_DDC_BAND_0 = 0u,              /*!< DDC channel 0 */
    ADRV903X_DDC_BAND_1,                   /*!< DDC channel 1 */
    ADRV903X_DDC_NUM_BAND
} adrv903x_DdcNumber_e;

/* profiles */
#define ADRV903X_NUM_RX_PROFILES     (4u)
#define ADRV903X_NUM_TX_PROFILES     (4u)
#define ADRV903X_NUM_ORX_PROFILES    (2u)
/* slices */
#define ADRV903X_NUM_TXRX_CHAN       (8u)
#define ADRV903X_NUM_ORX_CHAN        (2u)

/****************************************************************************
* PLL structure.
****************************************************************************/
ADI_ADRV903X_PACK_START
typedef struct adrv903x_PllConfig
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
} adrv903x_PllConfig_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* PLL structure.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_ClkGenConfig
{
    uint8_t div1enb;                /*!< div 1 (sample clk) enable setting  */
    uint8_t div3enb;                /*!< div 3 enable setting */
    uint8_t intDiv;                 /*!< interface clk divider register setting */
    uint8_t clkDiv;                 /*!< sample clk divider register setting  */
    uint8_t reserved[4];            /*!< Reserved  */
} adrv903x_ClkGenConfig_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* NCO Shifter structure.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_NcoShifter
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
} adrv903x_NcoShifter_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* FSC structure.
****************************************************************************/
ADI_ADRV903X_PACK_START
/*!< adrv903x_FscConfig:  structure of Fsc configuration parameters */
typedef struct adrv903x_FscConfig
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
} adrv903x_FscConfig_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* Profile information that will define the Receiver channel.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_RxConfig
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
    adrv903x_NcoShifter_t   rxddc[ADRV903X_DDC_NUM_BAND];  /*!<  NCO Shifter  */
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
    adrv903x_RxPfirData_t   rxPfirBank;                    /*!<  Rx PFIR Bank   */
    adrv903x_ClkGenConfig_t clkGenConfig;                  /*!<  Rx clock gen settings */
    uint8_t                 loLeafDiv;                     /*!<  LO Leaf Divider /1 /2 /4 */
    uint8_t                 clkdiv1p5enable;               /*!<  Resampling clock enable 1p5 */
    uint8_t                 clkdiv1p3enable;               /*!<  Resampling clock enable 1p3 */

    uint8_t                 reserved[10];                  /*!<  Reserved  */
} adrv903x_RxConfig_t;
ADI_ADRV903X_PACK_FINISH


/****************************************************************************
* Profile information that will define the Transmitter channel.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_TxConfig
{
    uint32_t                ibw_kHz;                      /*!<   Tx instantaneous bandwidth in kHz.  */
    uint32_t                ibwCenterFreq_kHz;            /*!<   Tx instantaneous center freq in kHz.  */
    uint32_t                butterFilter_kHz;             /*!<   Tx butter filter BW for TxBBF in kHz */
    uint32_t                txDacRate_kHz;                /*!<   Tx dac rate in kHz*/
    uint32_t                txOutputRate_kHz;             /*!<   Tx output rate in kHz*/
    adrv903x_NcoShifter_t   txduc[ADRV903X_DUC_NUM_BAND]; /*!<   ncoShifterA NCO Shifter  */
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
    adrv903x_ClkGenConfig_t txClkGenConfig;               /*!<   Tx clock gen settings */
    adrv903x_ClkGenConfig_t lpbkClkGenConfig;             /*!    Loopback clock gen settings */
    uint8_t                 overloadHb1ThresPre;          /*!    The pre-threshold value to be compared with magnitude. */
    uint8_t                 overloadHb1Thres;             /*!    The threshold value to be compared with I^2+Q^2 when in power mode, after pre_threshold met. */
    uint8_t                 overloadMode;                 /*!    When this bit is set, the I^2+Q^2 power is compared with the threshold(power mode) */
    uint8_t                 overloadEn;                   /*!    When this bit is set, the overload detector at output of lpbk hb1 is enabled. */
    uint8_t                 peakCountThresHb1;            /*!    The threshold value used to be compared with the peak counter. */
    uint8_t                 peakCountSpacing;             /*!    Defines the spacing between two peaks counted towards the peak count threshold. */
    uint8_t                 peakWinSize;                  /*!    Defines the window of measurement size(number of cycles) */
    uint8_t                 peakCount;                    /*!    With in the peak_window_size_hb1, this bitfield defines the minimum number of peaks need */
    uint8_t                 peakExperation;               /*!    Peak counter is cleared every peak count expiration period */
    adrv903x_FscConfig_t    fscConfig;                    /*!<   FSC config settings */
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
} adrv903x_TxConfig_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* Profile information that will define the Observation Receiver channel.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_OrxConfig
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
    int32_t                 orxAdcOutNcoFreqArray_kHz[ADRV903X_NUM_TX_PROFILES]; /*!<    Orx ADC output NCO frequency in KHz, per Tx Profile */
    int32_t                 orxDpNcoFreqArray_kHz[ADRV903X_NUM_TX_PROFILES];     /*!<    Orx Datapath NCO frequency in KHz, per Tx Profile */
    uint8_t                 orxLolPathDelayHb2Out[ADRV903X_NUM_TX_PROFILES];     /*!<    Tx-to-ORx path delay used by LOL cal (in samples at the Tx PFIR/LOL rate) */
    uint8_t                 orxLolPathDelayHb1Out[ADRV903X_NUM_TX_PROFILES];     /*!<    Tx-to-ORx path delay used by LOL cal (in samples at the Tx PFIR/LOL rate) */
    adrv903x_ClkGenConfig_t clkGenConfig;                                        /*!<    Orx clock gen settings */
    uint8_t                 reserved[9];                                         /*!<    Reserved  */
} adrv903x_OrxConfig_t;
ADI_ADRV903X_PACK_FINISH


/****************************************************************************
* Profile information that will define the Radio Configuration
* Profile struct holds clock rates, divider ratios, sampling rates, etc.
****************************************************************************/
    ADI_ADRV903X_PACK_START
typedef struct adrv903x_RadioProfile
{
    adrv903x_Version_t      deviceProfileVersion;                      /*!<   Device Profile version. Use for easy checking between Configurator and API */
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
    adrv903x_ClkGenConfig_t clkGenConfig;                              /*!<   System clock gen settings */
    uint8_t                 loPhaseSync;                               /*!<   0 = Dont Sync; 1 = Run Init and sync; 2 = Run init and Continuous tracking  */
    adrv903x_PllConfig_t    clkPll;                                    /*!<   Clock PLL structure  */
    adrv903x_PllConfig_t    rf0Pll;                                    /*!<   RF0 PLL structure (EAST) */
    adrv903x_PllConfig_t    rf1Pll;                                    /*!<   RF1 PLL structure (WEST) */
    uint8_t                 rf0MuxEastTx;                              /*!<   RF0 (East) is connect to Tx when set. */
    uint8_t                 rf0MuxWestTx;                              /*!<   RF0 (West) is connect to Tx when set. */
    uint8_t                 rf0MuxEastRx;                              /*!<   RF0 (East) is connect to Rx when set. */
    uint8_t                 rf0MuxWestRx;                              /*!<   RF0 (West) is connect to Rx when set. */
    uint32_t                chanConfig;                                /*!<   Channel config. B0=Tx0, B1=Tx1,--- B7=Tx7, B8=Rx0, B9=Rx1 -- B15=Rx7, B16=Orx0, B17=Orx1  */
    uint8_t                 chanAssign[ADRV903X_NUM_TXRX_CHAN];        /*!<   This is used to reference a channel to a rx/tx profile def'n in rxConfig/txConfig */
    adrv903x_RxConfig_t     rxConfig[ADRV903X_NUM_RX_PROFILES];        /*!<   Receiver profile for 0-3  */
    adrv903x_TxConfig_t     txConfig[ADRV903X_NUM_TX_PROFILES];        /*!<   Transmitter profile for 0-3 */
    adrv903x_TxPfirData_t   txPfirBank[ADRV903X_NUM_TXRX_CHAN];        /*!<   Tx PFIR Bank */
    uint8_t                 rxTxCpuConfig[ADRV903X_NUM_TXRX_CHAN];     /*!<   Defines which CPU each channel is assigned to  */
    adrv903x_OrxConfig_t    orxConfig[ADRV903X_NUM_ORX_PROFILES];      /*!<   Observation Receiever profile for channels 0-1  */
    uint8_t                 orxCpuConfig[ADRV903X_NUM_ORX_PROFILES];   /*!<   This defines which cpu an Orx channel is allocated to.   */
    adrv903x_PllConfig_t    serdesPll;                                 /*!<   Serdes PLL structure  */
    adrv903x_JesdSettings_t jesdProfile;                               /*!<   Jesd configuration  */
    uint8_t                 pid;                                       /*!<   PID */
    uint8_t                 productId[20];                             /*!<   Product ID */
    uint8_t                 featureMask[16];                           /*!<   128 bits of Feature Mask */
    uint8_t                 reserved[8];                               /*!<   Reserved (adjust so Device profile size is multiple of 4 bytes) */
    uint32_t                profileChecksum;                           /*!<   Checksum of the entire profile excluding ADC profiles, Using the CRC32 algorithm  */
} adrv903x_RadioProfile_t;
ADI_ADRV903X_PACK_FINISH

/****************************************************************************
* Profile information provided by BBIC at init via CONFIG command
****************************************************************************/
ADI_ADRV903X_PACK_START
typedef struct adrv903x_DeviceProfile
{
    adrv903x_RadioProfile_t radioProfile;                              /*!< Radio configuration parameters */
} adrv903x_DeviceProfile_t;
ADI_ADRV903X_PACK_FINISH

#define ADRV903X_DEVICE_PROFILE_SIZE_BYTES    sizeof(adrv903x_DeviceProfile_t) /* about 2440 bytes */
#define ADRV903X_RADIO_PROFILE_SIZE_BYTES     sizeof(adrv903x_RadioProfile_t)
/* Constant for adrv903x_RuntimeProfile_t signature1 field */
#define ADRV903X_RUNTIME_PROFILE_SIGNATURE1      (0x55AADD11UL)

ADI_ADRV903X_PACK_START
typedef struct adrv903x_RuntimeProfile
{
    uint32_t signature1;                                         /*!< Magic number to indicate that profile data is valid */
    uint32_t hsDigFreq_kHz;                                      /*!< System HS Dig clock setting in kHz */
    uint32_t txPfirSampleRate_kHz[ADRV903X_NUM_TXRX_CHAN];       /*!< DUC sampling rate for Tx in kHz */
    uint32_t txNcoMixerSampleRate_kHz[ADRV903X_NUM_TXRX_CHAN];   /*!< DUC NCO sampling rate for Tx in kHz */
    uint32_t orxOutputRate_kHz[ADRV903X_NUM_ORX_CHAN];           /*!< Orx output sample rate */
    uint32_t orxHb1OutRate_kHz[ADRV903X_NUM_ORX_CHAN];           /*!< Orx HB1 effective output sample rate */
    uint32_t orxHb2OutRate_kHz[ADRV903X_NUM_ORX_CHAN];           /*!< Orx HB2 effective output sample rate */
    int32_t  orxDpNcoFreqArray_kHz[ADRV903X_NUM_ORX_CHAN];       /*!< Orx NCO freqency in khz */
    uint32_t txLo0Freq_kHz;                                      /*!< Tx Lo0 frequency in kHz */
    uint32_t txLo1Freq_kHz;                                      /*!< Tx Lo1 frequency in kHz */
    uint32_t txChanConfig;                                       /*!< Tx Channel enable/disable bitmask */
    uint32_t rxChanConfig;                                       /*!< Rx Channel enable/disable bitmask */
    uint32_t orxChanConfig;                                      /*!< ORx Channel enable/disable bitmask */
} adrv903x_RuntimeProfile_t;
ADI_ADRV903X_PACK_FINISH

#endif /* ADRV903X_CPU_DEVICE_PROFILE_TYPES_H__ */


