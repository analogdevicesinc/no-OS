/**
 * \file
 * \brief Type definitions for ADRV9001 Rx settings
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_RXSETTINGS_TYPES_H_
#define _ADI_ADRV9001_RXSETTINGS_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv9001_pfirBuffer_types.h"

/**
 * \brief Enumeration of the Rx signal type
 */
typedef enum adi_adrv9001_RxSignalType
{
    ADI_ADRV9001_RX_IQ    = 0,
    ADI_ADRV9001_RX_FREQUENCY_DEVIATION = 1,
    ADI_ADRV9001_RX_FM_SYMBOLS = 2
} adi_adrv9001_RxSignalType_e;

typedef enum adi_adrv9001_AdcType
{
    ADI_ADRV9001_ADC_LP = 0, /*!< Low Power ADC */
    ADI_ADRV9001_ADC_HP = 1  /*!< High Power ADC */
} adi_adrv9001_AdcType_e;

/**
 * \brief Available Low Power ADC calibration modes
 */
typedef enum  adi_adrv9001_Adc_LowPower_CalMode
{
    ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
    ADI_ADRV9001_ADC_LOWPOWER_CONTINUOUS
} adi_adrv9001_Adc_LowPower_CalMode_e;

/**
 * \brief Data structure to hold ADRV9001 RX Narrowband decimation top block
 */
typedef struct adi_adrv9001_RxNbDecTop
{
    uint8_t scicBlk23En;			/*!< Setting for block #23. RSCIC */
    uint8_t scicBlk23DivFactor;		/*!< STATIC or DYNAMIC, BBIC to determine based on user case */
    uint8_t scicBlk23LowRippleEn;	/*!< Enable low ripple mode */
    uint8_t decBy2Blk35En;			/*!< Setting for block #35. DEC2_1 */
    uint8_t decBy2Blk37En;			/*!< Setting for block #37. DEC2_2 */
    uint8_t decBy2Blk39En;			/*!< Setting for block #39. DEC2_3 */
    uint8_t decBy2Blk41En;			/*!< Setting for block #41. DEC2_4 */
    uint8_t decBy2Blk43En;			/*!< Setting for block #43. DEC2_5 */
    uint8_t decBy3Blk45En;			/*!< Setting for block #45. DEC3   */
    uint8_t decBy2Blk47En;			/*!< Setting for block #47. DEC2_6 */
} adi_adrv9001_RxNbDecTop_t;

/**
 * \brief Data structure to hold ADRV9001 RX Wideband decimation top block
 */
typedef struct adi_adrv9001_RxWbDecTop
{
    uint8_t decBy2Blk25En;	/*!< Setting for block #25. DEC2_1 */
    uint8_t decBy2Blk27En;	/*!< Setting for block #27. DEC2_2 */
    uint8_t decBy2Blk29En;	/*!< Setting for block #29. DEC2_3 */
    uint8_t decBy2Blk31En;	/*!< Setting for block #31. DEC2_4 */
    uint8_t decBy2Blk33En;	/*!< Setting for block #33. DEC2_5 */
    uint8_t wbLpfBlk33p1En; /*!< Setting for block #33.1. WBLPF */
} adi_adrv9001_RxWbDecTop_t;

/**
* \brief Data structure to hold ADRV9001 RX decimation top block
*/
typedef struct adi_adrv9001_RxDecTop
{
    uint8_t decBy3Blk15En;		/*!< Setting for block #15.   DEC2 */
    uint8_t decBy2Hb3Blk17p1En; /*!< Setting for block #17.1. HB3 */
    uint8_t decBy2Hb4Blk17p2En; /*!< Setting for block #17.2. HB4 */
    uint8_t decBy2Hb5Blk19p1En; /*!< Setting for block #19.1. HB5 */
    uint8_t decBy2Hb6Blk19p2En; /*!< Setting for block #19.2. HB6 */
} adi_adrv9001_RxDecTop_t;

/**
 * \brief Enumerations of the SINC_MUX5 options
 */
typedef enum adi_adrv9001_RxSincMux5Output
{
    /* DO NOT modify the value of the enum as it matches the actual register value */
    ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO = 0u,  /*!< Output zero   */
    ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC3 = 1u, /*!< Select SINC3 for the output     */
    ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC4 = 2u, /*!< Select SINC4 for the output     */
    ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6 = 4u, /*!< Select SINC6 for the toutput    */
} adi_adrv9001_RxSincMux5Output_e;

/**
 * \brief Enumerations of the SINC gain settings
 */
typedef enum adi_adrv9001_RxSincGainMuxOutput
{
    /* DO NOT modify the value of the enum as it matches the actual register value */
    ADI_ADRV9001_RX_SINC_GAIN_MUX_0_DB      = 0u,   /*!< 0 dB */
    ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB      = 1u,   /*!< 6 dB */
    ADI_ADRV9001_RX_SINC_GAIN_MUX_12_DB     = 2u,   /*!< 12 dB */
    ADI_ADRV9001_RX_SINC_GAIN_MUX_NEG_6_DB  = 3u,   /*!< -6 dB */
    ADI_ADRV9001_RX_SINC_GAIN_MUX_NEG_12_DB = 4u    /*!< -12 dB */
} adi_adrv9001_RxSincGainMuxOutput_e;

/**
 * \brief Enumerations of the HB_MUX options
 */
typedef enum adi_adrv9001_RxHBMuxOutput
{
    /* DO NOT modify the value of the enum as it matches the actual register value */
    ADI_ADRV9001_RX_HB_MUX_OUTPUT_ZERO = 0u, /*!< Output zero   */
    ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB0 = 1u,  /*!< Select HB0 for the output     */
    ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2 = 2u,  /*!< Select HB2 for the output     */
    ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1 = 4u   /*!< Select HB1 for the toutput    */
} adi_adrv9001_RxHBMuxOutput_e;

/**
 *  \brief Enumerations of PFIR input selection
 */
typedef enum adi_adrv9001_RxWbNbPfirInSel
{
    ADI_ADRV9001_RX_WB_NB_PFIR_SEL_INT_IN = 0u, /*!< PFIR input from RXNB NCO */
    ADI_ADRV9001_RX_WB_NB_PFIR_SEL_EXT_IN = 1u, /*!< PFIR input from RP point */
} adi_adrv9001_RxWbNbPfirInSel_e;

/**
 * \brief Data structure to hold ADRV9001 RX Sinc Halfband top block
 */
typedef struct adi_adrv9001_RxSincHbTop
{
    adi_adrv9001_RxSincGainMuxOutput_e	sincGainMux;			/*!< Sinc gain setting */
    adi_adrv9001_RxSincMux5Output_e		sincMux;				/*!< Sinc MUX selection */
    adi_adrv9001_RxHBMuxOutput_e		hbMux;					/*!< HB MUX selection */

    uint8_t								isGainCompEnabled;      /*!< Gain compensation */
    int16_t								gainComp9GainI[ADI_ADRV9001_NUM_GAIN_COMP9];
    int16_t								gainComp9GainQ[ADI_ADRV9001_NUM_GAIN_COMP9];
} adi_adrv9001_RxSincHbTop_t;

/*! Enum for RX dpinfifo mode settings */
typedef enum adi_adrv9001_RxDpInFifoMode
{
    ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING = 0u, /*!< In detecting mode */
    ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTED  = 1u, /*!< In detected mode  */
} adi_adrv9001_RxDpInFifoMode_e;

/*! Enum for RX dpinfifo input data select */
typedef enum adi_adrv9001_RxDpInFifoInputSel
{
    ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL  = 0u, /*!< select data from datapath */
    ADI_ADRV9001_DP_IN_FIFO_INPUT_SPI_SEL = 1u, /*!< select test pattern from SPI  */
} adi_adrv9001_RxDpInFifoInputSel_e;

/*! dpinfifo configuration structure */
typedef struct adi_adrv9001_RxDpInFifoConfig
{
    uint8_t								dpInFifoEn;                  /*!< dpinfifo enable */
    adi_adrv9001_RxDpInFifoMode_e		dpInFifoMode;
    adi_adrv9001_RxDpInFifoInputSel_e	dpInFifoTestDataSel;
} adi_adrv9001_RxDpInFifoConfig_t;

/**
 * \brief Parameters for IQDM NCO
 */
typedef struct adi_adrv9001_NcoDpConfig
{
    int32_t  freq;        /*!< NCO output frequency */
    uint32_t sampleFreq;  /*!< Sampling frequency at NCO connects to */
    uint16_t phase;       /*!< Phase offset */
    uint8_t  realOut;     /*!< Real out enable */
} adi_adrv9001_NcoDpConfig_t;

/**
 * \brief Data structure to hold ADRV9001 Rx NB NCO structure
 */
typedef struct adi_adrv9001_RxNbNcoConfig
{
    uint8_t                      rxNbNcoEn;		/*!< rxnb nco enable */
    adi_adrv9001_NcoDpConfig_t rxNbNcoConfig;
} adi_adrv9001_RxNbNcoConfig_t;

/*! Enum for RX WbNb Compensation PFIR input select */
typedef enum adi_adrv9001_RxPfirInMuxSel
{
    ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN = 0u,
    ADI_ADRV9001_RP_FIR_IN_MUX_EXT_IN = 1u,
} adi_adrv9001_RxPfirInMuxSel_e;

typedef enum adi_adrv9001_RxGsOutMuxSel
{
    ADI_ADRV9001_GS_OUT_MUX_GS_OUT = 0u,
    ADI_ADRV9001_GS_OUT_MUX_BYPASS = 1u,
} adi_adrv9001_RxGsOutMuxSel_e;

/*! Enum for Rx out select settings */
typedef enum adi_adrv9001_RxOutSel
{
    ADI_ADRV9001_RX_OUT_IQ_SEL    = 0u, /*!< IQ out */
    ADI_ADRV9001_RX_OUT_FMDM_SEL  = 1u, /*!< FM demodulation out */
    ADI_ADRV9001_RX_OUT_SEL_PFIR  = 2u, /*!< PFIR out */
} adi_adrv9001_RxOutSel_e;

/*! Enum for RX round output mode */
typedef enum adi_adrv9001_RxRoundMode
{
    ADI_ADRV9001_RX_ROUNDMODE_IQ = 0u, /*!< IQ output */
    ADI_ADRV9001_RX_ROUNDMODE_I  = 1u, /*!< I only output */
} adi_adrv9001_RxRoundMode_e;

/*! Enum for RX output select */
typedef enum adi_adrv9001_RxDpArmSel
{
    ADI_ADRV9001_DP_SEL  = 0u, /*!< select datapath output */
    ADI_ADRV9001_ARM_SEL = 1u, /*!< select arm output */
} adi_adrv9001_RxDpArmSel_e;

/**
 * \brief Data structure to hold ADRV9001 Rx WbNb compensation PFir structure
 */
typedef struct adi_adrv9001_RxWbNbCompPFir
{
    adi_adrv9001_PfirBank_e      bankSel;		/*!< bank: PFIR_BANK_A = 0u (bank A), PFIR_BANK_B = 1u (bank B),
                                                           PFIR_BANK_C = 2u (bank C), PFIR_BANK_D = 3u (bank D) */
    adi_adrv9001_RxPfirInMuxSel_e   rxWbNbCompPFirInMuxSel;
    uint8_t							rxWbNbCompPFirEn;            /*!< rx WbNb compensation PFir enable */
} adi_adrv9001_RxWbNbCompPFir_t;

/**
 * \brief Data structure to hold ADRV9001 Rx resampler structure
 */
typedef struct adi_adrv9001_RxResampConfig
{
    uint8_t            rxResampEn;                 /*!< resampler enable */
    uint32_t           resampPhaseI;               /*!< I channel resampler phase */
    uint32_t           resampPhaseQ;               /*!< Q channel resampler phase */
} adi_adrv9001_RxResampConfig_t;

/**
 * \brief Data structure to hold ADRV9001 RX Dem block
 */
typedef struct adi_adrv9001_RxNbDemConfig
{
    adi_adrv9001_RxDpInFifoConfig_t	dpInFifo;
    adi_adrv9001_RxNbNcoConfig_t    rxNbNco;
    adi_adrv9001_RxWbNbCompPFir_t   rxWbNbCompPFir;
    adi_adrv9001_RxResampConfig_t   resamp;
    adi_adrv9001_RxGsOutMuxSel_e    gsOutMuxSel; /*!< fic algorithm enable */
    adi_adrv9001_RxOutSel_e         rxOutSel;
    adi_adrv9001_RxRoundMode_e      rxRoundMode;
    adi_adrv9001_RxDpArmSel_e       dpArmSel;
} adi_adrv9001_RxNbDemConfig_t;

/**
 *  \brief Data structure to hold ADRV9001 Rx Data Path profile settings
 */
typedef struct adi_adrv9001_RxDpProfile
{
    adi_adrv9001_RxNbDecTop_t		rxNbDecTop;		/*!< RX narrowband decimation top */
    adi_adrv9001_RxWbDecTop_t		rxWbDecTop;		/*!< RX wideband decimation top */
    adi_adrv9001_RxDecTop_t			rxDecTop;		/*!< RX Common decimation top */
    adi_adrv9001_RxSincHbTop_t		rxSincHBTop;	/*!< RX Sinc HB top */
    adi_adrv9001_RxNbDemConfig_t	rxNbDem;		/*!< RX NB Dem block */
} adi_adrv9001_RxDpProfile_t;

/**
 * \brief Enumeration of SSI type
 */
typedef enum adi_adrv9001_SsiType
{
    ADI_ADRV9001_SSI_TYPE_DISABLE = 0,  /*!< Disable SSI Type */
    ADI_ADRV9001_SSI_TYPE_CMOS = 1,     /*!< CMOS SSI Type */
    ADI_ADRV9001_SSI_TYPE_LVDS = 2      /*!< LVDS SSI Type */
} adi_adrv9001_SsiType_e;

/**
 * \brief Enumeration of the SSI data format
 */
typedef enum adi_adrv9001_SsiDataFormat
{
    ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA = 0,  /*!< 2 bit symbol data (CMOS) */
    ADI_ADRV9001_SSI_FORMAT_8_BIT_SYMBOL_DATA = 1,  /*!< 8 bit symbol data (CMOS) */
    ADI_ADRV9001_SSI_FORMAT_16_BIT_SYMBOL_DATA = 2, /*!< 16 bit symbol data (CMOS) */
    ADI_ADRV9001_SSI_FORMAT_12_BIT_I_Q_DATA = 3,    /*!< 12 bit I/Q data (LVDS) */
    ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA = 4     /*!< 16 bit I/Q data (CMOS/LVDS) */
} adi_adrv9001_SsiDataFormat_e;

/**
 * \brief Enumeration of the SSI number of lane
 */
typedef enum adi_adrv9001_SsiNumLane
{
    ADI_ADRV9001_SSI_1_LANE = 0, /*!< 1 lane (CMOS/LVDS) */
    ADI_ADRV9001_SSI_2_LANE = 1, /*!< 2 lane (LVDS) */
    ADI_ADRV9001_SSI_4_LANE = 2  /*!< 4 lane (CMOS) */
} adi_adrv9001_SsiNumLane_e;

/**
 * \brief Enumeration of the SSI Strobe
 */
typedef enum adi_adrv9001_SsiStrobeType
{
    ADI_ADRV9001_SSI_SHORT_STROBE = 0, /*!< Short SSI Strobe */
    ADI_ADRV9001_SSI_LONG_STROBE  = 1  /*!< Long SSI Strobe */
} adi_adrv9001_SsiStrobeType_e;


/**
 * \brief Tx SSI reference clock pin option enumeration
 */
typedef enum adi_adrv9001_SsiTxRefClockPin
{
    ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED                   = 0u,   /*!< Tx reference clock out pin disabled */
    ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED           = 1u,   /*!< Tx reference clock out pin TX_DCLK_OUT+/- enabled in LVDS mode or
                                                                                                         TX_DCLK_OUT+   enabled in CMOS mode */
    ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_CMOS_DCLK_OUT_N_ENABLED    = 2u,   /*!< Tx reference clock out pin TX_DCLK_OUT-   enabled in CMOS mode */
    ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_CMOS_STROBE_IN_N_ENABLED   = 3u    /*!< Tx reference clock out pin TX_STROBE_IN-  enabled in CMOS mode */
} adi_adrv9001_SsiTxRefClockPin_e;

/**
 * \brief Data structure to hold ADRV9001 SSI configuration.
 */
typedef struct adi_adrv9001_SsiConfig
{
    adi_adrv9001_SsiType_e       ssiType;					/*!< SSI type */
    adi_adrv9001_SsiDataFormat_e ssiDataFormatSel;			/*!< SSI data format */
    adi_adrv9001_SsiNumLane_e    numLaneSel;				/*!< SSI number of lanes */
    adi_adrv9001_SsiStrobeType_e strobeType;				/*!< SSI strobe type */
    uint8_t						 lsbFirst;					/*!< SSI LSB first */
    uint8_t						 qFirst;					/*!< SSI Q data first */
    adi_adrv9001_SsiTxRefClockPin_e	txRefClockPin;			/*!< SSI Tx reference clock GPIO select */
    uint8_t						 lvdsBitInversion;			/*!< LVDS SSI bit inversion */
    uint8_t						 lvdsUseLsbIn12bitMode;		/*!< LVDS use LSB in 12 bit mode */
    bool						 lvdsTxFullRefClkEn;        /*!< LVDS Tx full refclk enable */
    bool						 lvdsRxClkInversionEn;      /*!< LVDS Rx clock inversion enable */
    bool                         cmosTxDdrNegStrobeEn;      /*!< CMOS Tx DDR negative strobe enable */
    bool                         cmosDdrPosClkEn;           /*!< CMOS DDR positive clock enable */
    bool                         cmosDdrClkInversionEn;     /*!< CMOS DDR clock inversion enable */
    bool                         cmosDdrEn;                 /*!< CMOS DDR enable */
    bool                         rxMaskStrobeEn;            /*!< SSI Rx mask strobe enable */
} adi_adrv9001_SsiConfig_t;

/**
 *  \brief Data structure to hold settings for the current Rx specific use case profile
 */
typedef struct adi_adrv9001_RxProfile
{
    uint32_t		primarySigBandwidth_Hz;			  /*!< Rx primary signal BW in Hz */
    uint32_t		rxOutputRate_Hz;				  /*!< Rx output data rate in Hz */
    uint32_t		rxInterfaceSampleRate_Hz;		  /*!< Rx sample rate at serial interface */
    int32_t			rxOffsetLo_kHz;                   /*!< Offset in kHz. 0: On LO */
    uint8_t			rxSignalOnLo;                     /*!< !0: Signal on LO  0: no signal on LO */
    adi_adrv9001_RxSignalType_e outputSignaling;      /*!< Output to BBIC signal type */
    uint8_t			filterOrder;					  /*!< 1st or 2nd order ABBF filter */
    uint8_t			filterOrderLp;					  /*!< 1st or 2nd order ABBF filter Low Power ADC*/
    uint32_t		hpAdcCorner;					  /*!< High Power ADC corner freq*/
    uint32_t        lpAdcCorner;                      /*!< Low Power ADC 3dB corner frequency in Hz */
    uint32_t		adcClk_kHz;						  /*!< ADC clock 2.2G/1.47G/1.1G */
    uint32_t		rxCorner3dB_kHz;				  /*!< TIA bandwidth in kHz */
    uint32_t		rxCorner3dBLp_kHz;				  /*!< TIA bandwidth for Low Power ADC */
    adi_adrv9001_ComponentPowerLevel_e tiaPower;      /*!< Rx TIA power level before the high-performance ADC */
    adi_adrv9001_ComponentPowerLevel_e tiaPowerLp;    /*!< Rx TIA power level before the low power ADC*/
    uint32_t	    channelType;		              /*!< Channel type described by this profile (Rx/ORx/Loopback) */
    adi_adrv9001_AdcType_e		adcType;			  /*!< ADC type: low/high power ADC */
    adi_adrv9001_Adc_LowPower_CalMode_e  lpAdcCalMode; /*!< Select periodic or continuous Low Power ADC calibration */
    adi_adrv9001_RxDpProfile_t	rxDpProfile;		  /*!< RX digital data path config */
    adi_adrv9001_SsiConfig_t	rxSsiConfig;		  /*!< RX Serial data interface config */
} adi_adrv9001_RxProfile_t;

/**
 *  \brief Data structure to hold Rx data path settings
 */
typedef struct adi_adrv9001_RxChannelCfg
{
    adi_adrv9001_RxProfile_t profile; /*!< Rx datapath profile, 3dB corner frequencies, and digital filter enables */
} adi_adrv9001_RxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV9001 Rx Channel configuration settings
 */
typedef struct adi_adrv9001_RxSettings
{
    uint32_t rxInitChannelMask;                                            /*!< Rx channel mask of which channels to initialize */
    adi_adrv9001_RxChannelCfg_t rxChannelCfg[ADI_ADRV9001_MAX_RXCHANNELS]; /*!< Rx settings per Rx channel */
} adi_adrv9001_RxSettings_t;

#endif /* _ADI_ADRV9001_RXSETTINGS_TYPES_H_ */
