/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_radioctrl_types.h
* \brief Contains ADRV903X data types for radio ctrl features
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_RADIOCTRL_TYPES_H_
#define _ADI_ADRV903X_RADIOCTRL_TYPES_H_

#include "adi_adrv903x_lo_types.h"
#include "adi_adrv903x_dev_temp_types.h"
#include "adi_adrv903x_gpio_types.h"
#include "adi_adrv903x_rx_types.h"
#include "adi_adrv903x_tx_types.h"

#define ADI_ADRV903X_STREAM_MAX 20
#define ADI_ADRV903X_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (88*1024)
#define ADI_ADRV903X_TRX_CTRL_PIN_COUNT 8
#define ADI_ADRV903X_MAX_STREAMGPIO 24
#define ADI_ADRV903X_TX_TO_ORX_MAPPING_GPIO_MAX 8
#define ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE 16
#define ADI_ADRV903X_STREAM_VERSION_ADDR 0x46A00008

#define ADI_ADRV903X_GPIO_AS_TRXCONTROL_MAX_NUMBER_PINS 4U

/**
*  \brief Enum of Tx Enable Modes
*/
typedef enum adi_adrv903x_TxEnableMode
{
    ADI_ADRV903X_TX_EN_SPI_MODE = 0, /*!< Selects SPI registers to control Tx signal path on/off */
    ADI_ADRV903X_TX_EN_PIN_MODE,     /*!< Selects Pin mode to control Tx signal path on/off. Tx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV903X_TX_EN_INVALID_MODE
} adi_adrv903x_TxEnableMode_e;

/**
*  \brief Enum of Rx Enable Modes
*/
typedef enum adi_adrv903x_RxEnableMode
{
    ADI_ADRV903X_RX_EN_SPI_MODE = 0, /*!< Selects SPI registers to control Rx signal path on/off */
    ADI_ADRV903X_RX_EN_PIN_MODE,     /*!< Selects Pin mode to control Rx signal path on/off. Rx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV903X_RX_EN_INVALID_MODE
} adi_adrv903x_RxEnableMode_e;

/**
*  \brief Enum of Rx Enable Modes
*/
typedef enum adi_adrv903x_ORxEnableMode
{
    ADI_ADRV903X_ORX_EN_SPI_MODE = 0,    /*!< Selects SPI registers to control ORx signal path on/off */
    ADI_ADRV903X_ORX_EN_PIN_MODE,        /*!< Selects Pin mode to control ORx signal path on/off. Rx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV903X_ORX_EN_INVALID_MODE
} adi_adrv903x_ORxEnableMode_e;
/**
 *  \brief Enum of the eight TxRx Enable Pins
 */
typedef enum adi_adrv903x_TxRxEnPin
{
    ADI_ADRV903X_TXRXEN_PIN0    = 0x01, /*!< TxRxEn Pin0 */
    ADI_ADRV903X_TXRXEN_PIN1    = 0x02, /*!< TxRxEn Pin1 */
    ADI_ADRV903X_TXRXEN_PIN2    = 0x04, /*!< TxRxEn Pin2 */
    ADI_ADRV903X_TXRXEN_PIN3    = 0x08, /*!< TxRxEn Pin3 */
    ADI_ADRV903X_TXRXEN_PIN4    = 0x10, /*!< TxRxEn Pin4 */
    ADI_ADRV903X_TXRXEN_PIN5    = 0x20, /*!< TxRxEn Pin5 */
    ADI_ADRV903X_TXRXEN_PIN6    = 0x40, /*!< TxRxEn Pin6 */
    ADI_ADRV903X_TXRXEN_PIN7    = 0x80, /*!< TxRxEn Pin7 */
    ADI_ADRV903X_TXRXEN_PINALL  = 0xFF  /*!< All TxRxEn pins */
} adi_adrv903x_TxRxEnPin_e;

/**
*  \brief Enum of the TxRx Enable Pin Configuration settings
*/
typedef enum adi_adrv903x_TxRxEnCfg
{
    ADI_ADRV903X_TXRXEN_TX_ENABLE_MAP       = 0x01,     /*!< Selects the Tx Enable mapping configuration */
    ADI_ADRV903X_TXRXEN_TX_ALTENABLE_MAP    = 0x02,     /*!< Selects the Tx Alternate Enable mapping configuration */
    ADI_ADRV903X_TXRXEN_RX_ENABLE_MAP       = 0x04,     /*!< Selects the Rx Enable mapping configuration */
    ADI_ADRV903X_TXRXEN_RX_ALTENABLE_MAP    = 0x08,     /*!< Selects the Rx AlternateEnable mapping configuration */
    ADI_ADRV903X_TXRXEN_ALL                 = 0x0F      /*!< Selects all members in the structure */
} adi_adrv903x_TxRxEnCfg_e;

/**
 *  \brief Enum to select desired Tx to ORx Mapping Mode to be used for GPIO/API control
 */
typedef enum adi_adrv903x_TxToOrxMappingMode
{
    ADI_ADRV903X_TX_ORX_MAPPING_MODE_2BIT       = 2U,   /*!< 2Bit Tx to ORx Mapping Mode*/
    ADI_ADRV903X_TX_ORX_MAPPING_MODE_3BIT       = 3U,   /*!< 3Bit Tx to ORx Mapping Mode*/
    ADI_ADRV903X_TX_ORX_MAPPING_MODE_4BIT       = 4U,   /*!< 4Bit Tx to ORx Mapping Mode*/
    ADI_ADRV903X_TX_ORX_MAPPING_MODE_6BIT       = 6U,   /*!< 6Bit Tx to ORx Mapping Mode*/
    ADI_ADRV903X_TX_ORX_MAPPING_MODE_8BIT       = 8U,   /*!< 8Bit Tx to ORx Mapping Mode*/
} adi_adrv903x_TxToOrxMappingMode_e;

/**
 *  \brief Enum to select desired Tx Channel Mapping for a GPIO Pin state
 */
typedef enum adi_adrv903x_TxToOrxMappingPinTable
{
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX0       = 0x000U,   /*!< Pin state selects Tx0 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX1       = 0x001U,   /*!< Pin state selects Tx1 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX2       = 0x002U,   /*!< Pin state selects Tx2 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX3       = 0x003U,   /*!< Pin state selects Tx3 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX4       = 0x004U,   /*!< Pin state selects Tx4 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX5       = 0x005U,   /*!< Pin state selects Tx5 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX6       = 0x006U,   /*!< Pin state selects Tx6 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_TX7       = 0x007U,   /*!< Pin state selects Tx7 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NONE      = 0x008U,   /*!< Pin state selects No Mapping */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_NO_CHANGE = 0x00FU,   /*!< Pin state causes no change in current mapping */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_0   = 0x009U,   /*!< State 0 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_1   = 0x00AU,   /*!< State 1 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_2   = 0x00BU,   /*!< State 2 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_3   = 0x00CU,   /*!< State 3 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_4   = 0x00DU,   /*!< State 4 */
    ADI_ADRV903X_TX_ORX_MAPPING_PIN_TABLE_STATE_5   = 0x00EU    /*!< State 5 */
} adi_adrv903x_TxToOrxMappingPinTable_e;

/**
* \brief Data structure to hold Tx Signal Path Control configurations
*/
typedef struct adi_adrv903x_TxRadioCtrlModeCfg
{
    adi_adrv903x_TxEnableMode_e txEnableMode; /*!< Configuration for Tx Channel Enable mode */
    uint32_t txChannelMask;                   /*!< Tx channels for which configuration is valid */
} adi_adrv903x_TxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Rx Signal Path Control configurations
*/
typedef struct adi_adrv903x_RxRadioCtrlModeCfg
{
    adi_adrv903x_RxEnableMode_e rxEnableMode; /*!< Configuration for Rx Channel Enable mode */
    uint32_t rxChannelMask;                   /*!< Rx channels for which configuration is valid */
} adi_adrv903x_RxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold ORx Signal Path Control configurations
*/
typedef struct adi_adrv903x_ORxRadioCtrlModeCfg
{
    adi_adrv903x_ORxEnableMode_e orxEnableMode; /*!< Configuration for ORx Channel Enable mode */
    uint32_t orxChannelMask;                    /*!< ORx channels for which configuration is valid */
} adi_adrv903x_ORxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Signal Path Control configurations
*/
typedef struct adi_adrv903x_RadioCtrlModeCfg
{
    adi_adrv903x_TxRadioCtrlModeCfg_t  txRadioCtrlModeCfg;  /*!< Tx signal path enable mode configuration */
    adi_adrv903x_RxRadioCtrlModeCfg_t  rxRadioCtrlModeCfg;  /*!< Rx signal path enable mode configuration */
    adi_adrv903x_ORxRadioCtrlModeCfg_t orxRadioCtrlModeCfg; /*!< ORx signal path enable mode configuration */
} adi_adrv903x_RadioCtrlModeCfg_t;

/**
* \brief Data structure to hold the LO configuration settings
*/
typedef struct adi_adrv903x_LoConfig
{
    adi_adrv903x_LoName_e                loName ;                /*!< Select the target RF LO source */
    uint64_t                             loFrequency_Hz ;        /*!< Desired RF LO frequency in Hz */
    adi_adrv903x_LoOption_e              loConfigSel ;           /*!< Select for LO config */
    } adi_adrv903x_LoConfig_t ;

/**
* \brief Data structure to indicate to which PLL Tx and Rx Channels are connected. 
*/
typedef struct adi_adrv903x_ChanCtrlToPlls
{
	uint8_t                 rf0MuxTx0_3;    /*!<   If set to 1 East Tx channels (Tx0-3) are connected to PLL0 (East PLL). If set to 0 Tx0-3 are connected to PLL1 (West PLL) */
	uint8_t                 rf0MuxTx4_7;    /*!<   If set to 1 West Tx channels (Tx4-7) are connected to PLL0 (East PLL). If set to 0 Tx4-7 are connected to PLL1 (West PLL) */
	uint8_t                 rf0MuxRx0_3;    /*!<   If set to 1 East Rx channels (Rx0-3) are connected to PLL0 (East PLL). If set to 0 Rx0-3 are connected to PLL1 (West PLL) */
	uint8_t                 rf0MuxRx4_7;    /*!<   If set to 1 West Rx channels (Rx4-7) are connected to PLL0 (East PLL). If set to 0 Rx4-7 are connected to PLL1 (West PLL) */
}adi_adrv903x_ChanCtrlToPlls_t ;

/**
* \brief Data structure to hold the LO configuration read back values.
*/
typedef struct adi_adrv903x_LoConfigReadback
{
    adi_adrv903x_LoName_e                loName ;                /*!< Select the target RF LO source to read back */
    uint64_t                             loFrequency_Hz ;        /*!< RF LO frequency in Hz */
} adi_adrv903x_LoConfigReadback_t ;

/**
* \brief Data structure to hold the Radio Control GPIO pin information.
*/
typedef struct adi_adrv903x_RadioCtrlTxRxEnCfg
{
    uint8_t txEnMapping[ADI_ADRV903X_TRX_CTRL_PIN_COUNT]; /*!< Tx Enable Pin bitfield. Each bit in each byte corresponds to Each Tx Channel, eg txEnMapping[0] => ADI_ADRV903X_TX0. Use adi_adrv903x_TxChannels_e to build up bitmask. */
    uint8_t txAltMapping[ADI_ADRV903X_TRX_CTRL_PIN_COUNT]; /*!< Tx Alternate Enable Pin bitfield. Each bit in each byte corresponds to Each Tx Channel, eg txAltMapping[0] => ADI_ADRV903X_TX0. Use adi_adrv903x_TxChannels_e to build up bitmask. */
    uint8_t rxEnMapping[ADI_ADRV903X_TRX_CTRL_PIN_COUNT]; /*!< Rx Enable Pin bitfield. Each bit in each byte corresponds to Each Rx Channel, eg rxEnMapping[0] => ADI_ADRV903X_RX0. Use adi_adrv903x_RxChannels_e to build up bitmask. */
    uint8_t rxAltMapping[ADI_ADRV903X_TRX_CTRL_PIN_COUNT]; /*!< Rx Alternate Enable Pin bitfield. Each bit in each byte corresponds to Each Rx Channel, eg rxAltMapping[0] => ADI_ADRV903X_RX0. Use adi_adrv903x_RxChannels_e to build up bitmask. */
} adi_adrv903x_RadioCtrlTxRxEnCfg_t;

/**
* \brief Data structure to hold Synthesizer RF LO Loop filter settings
*/
typedef struct adi_adrv903x_LoLoopFilterCfg
{
    uint16_t loopBandwidth_kHz;            /*!< Synthesizer LO Loop filter bandwidth. Range 60-1000 */
    uint8_t  phaseMargin_degrees;          /*!< Synthesizer LO Loop filter phase margin in degrees. Range 45-75 */
} adi_adrv903x_LoLoopFilterCfg_t;

/**
* \brief Data structure to hold Stream GPIO pin assignments. 
*/
typedef struct adi_adrv903x_StreamGpioPinCfg
{ 
    adi_adrv903x_GpioPinSel_e streamGpInput[ADI_ADRV903X_MAX_STREAMGPIO]; /*!< Enable GPIO pin input to ADRV903X stream processor GP input. To enable set streamGpInput[0]=GPIO_00, streamGpInput[1]=GPIO_01,,etc. To disable select ADI_ADRV903X_GPIO_INVALID */
} adi_adrv903x_StreamGpioPinCfg_t;

/**
 * \brief Data structure to hold Tx to ORx configuration settings.
 */
typedef struct adi_adrv903x_TxToOrxMappingConfig
{
    uint16_t                                txObservability;                                                /*!< Tx observability word. This is a bitmask consisting of eight 2bit-pairs (nibbles), where each nibble makes a Tx channel observable by either ORx0 (set nibble to 2'b01 = 1), ORx1 (set nibble to 2'b10 = 2), or neither (set nibble to 2'b00 = 0). It is not allowed for a single Tx channel to be observable on both ORx0 and ORx1 (e.g. setting a nibble to 2'b11 = 3 is invalid). For example, setting B0 = Tx0 obs by ORx0, B1 = Tx0 obs by Orx1, ..., B8 = Tx4 obs by Orx0, B9 = Tx4 obs on Orx1, ..., B14 = Tx7 obs by Orx0, B15 = Tx7 obs on Orx1 */
    adi_adrv903x_TxToOrxMappingMode_e       mode;                                                           /*!< Tx to ORx Mapping Mode */
    adi_adrv903x_GpioPinSel_e               gpioSelect[ADI_ADRV903X_TX_TO_ORX_MAPPING_GPIO_MAX];            /*!< Select up to 8 GPIO to set mapping */
    adi_adrv903x_TxToOrxMappingPinTable_e   pinTableOrx0[ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];    /*!< ORX0 Look-Up Table for Tx Channel selection based on GPIO pin states */
    adi_adrv903x_TxToOrxMappingPinTable_e   pinTableOrx1[ADI_ADRV903X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];    /*!< ORX1 Look-Up Table for Tx Channel selection based on GPIO pin states */
    uint8_t                                 autoSwitchOrxAttenEnable;                                       /*!< Auto-switching of Orx Atten value enabled. 0 = Disabled. 1 = Enabled */
    uint8_t                                 autoSwitchOrxNcoEnable;                                         /*!< Auto-switching of Orx NCO frequency enabled. 0 = Disabled. 1 = Enabled */
} adi_adrv903x_TxToOrxMappingConfig_t;

/**
 *  \brief Structure for preset data used in Tx To Orx Mapping Auto-Update of ORx Atten and NCO Freq based on Mapping
 */
typedef struct adi_adrv903x_TxToOrxMappingPresetNco
{
    int32_t     ncoFreqAdc_Khz;
    int32_t     ncoFreqDatapath_Khz;
} adi_adrv903x_TxToOrxMappingPresetNco_t;

/**
 * \brief loopfilter RX/TX LO command response structure
 */
typedef struct adi_adrv903x_RxTxLoFreqReadback
{
    adi_adrv903x_LoName_e   rxLoName   [ADI_ADRV903X_MAX_RX_ONLY];      /*!< PLL number of Rx Channels */
    uint32_t                rxFreq_Khz [ADI_ADRV903X_MAX_RX_ONLY];      /*!< Freq of Rx Channels */
    adi_adrv903x_LoName_e   txLoName   [ADI_ADRV903X_MAX_TXCHANNELS];   /*!< PLL number of Tx Channels */
    uint32_t                txFreq_Khz [ADI_ADRV903X_MAX_TXCHANNELS];   /*!< Freq of Tx Channels */
} adi_adrv903x_RxTxLoFreqReadback_t ;

/**
* \brief Struct for ADRV903X Antenna Cal Mode Pre-set Configuration for Rx
*/
typedef struct adi_adrv903x_RxRadioCtrlAntennaCalConfig
{
    uint32_t rxChannelMask;     /*!< Rx Channel selection */
    uint8_t rxGainIndex;        /*!< Rx Gain table index to use during Antenna Cal */
    int32_t rxNcoFreqKhz;       /*!< Rx NCO Frequency to use during Antenna Cal */
} adi_adrv903x_RxRadioCtrlAntennaCalConfig_t;

/**
* \brief Struct for ADRV903X Antenna Cal Mode Pre-set Configuration for Tx
*/
typedef struct adi_adrv903x_TxRadioCtrlAntennaCalConfig
{
    uint32_t txChannelMask;     /*!< Tx Channel selection */
    uint16_t txAttenuation_mdB; /*!< Tx Attenuation value to use during Antenna Cal [mdB] */
    int32_t txNcoFreqKhz;       /*!< Tx NCO Frequency to use during Antenna Cal */
} adi_adrv903x_TxRadioCtrlAntennaCalConfig_t;

/**
 *  \brief Enum to classify the stream type being analyzed 
 */
typedef enum adi_adrv903x_StreamProcType
{
    ADI_ADRV903X_STREAM_MAIN    = 0U,
    ADI_ADRV903X_STREAM_KFA     = 1U,
    ADI_ADRV903X_STREAM_RX      = 2U,
    ADI_ADRV903X_STREAM_TX      = 3U,
    ADI_ADRV903X_STREAM_ORX     = 4U    
} adi_adrv903x_StreamProcType_e;

/**
*  \brief Enum to indicate error cause (is the output of sp_rdbk_error_val register of each stream processor
*/
typedef enum adi_adrv903x_StreamError
{
    ADI_ADRV903X_STREAM_ERR_NONE                   = 0U,
    ADI_ADRV903X_STREAM_ERR_SPI_ADDR               = 1U,
    ADI_ADRV903X_STREAM_ERR_CHECK_INSTR            = 2U,
    ADI_ADRV903X_STREAM_ERR_TIMEOUT                = 4U,
    ADI_ADRV903X_STREAM_ERR_STACK_OVERFLOW         = 8U,    
    ADI_ADRV903X_STREAM_ERR_INVALID_STREAM_NUM     = 16U,    
    ADI_ADRV903X_STREAM_ERR_AHB_ADDR               = 32U,
    ADI_ADRV903X_STREAM_ERR_INVALID_INSTR          = 64U,
    ADI_ADRV903X_STREAM_ERR_EXT_TIMER              = 128U,
    ADI_ADRV903X_STREAM_ERR_FIFO_FULL              = 256U,
    ADI_ADRV903X_STREAM_ERR_POSTED_HRESP_ON_SYSBUS = 512U
} adi_adrv903x_StreamError_e;

/**
* \brief Data structure to hold each stream processor error information (the stream processor, the stream number and the error code)
*/
typedef struct adi_adrv903x_StreamErr
{
    uint8_t                         streamProcId; /*!< stream processor id. Together with streamProcType indicates the Stream Proc */
    uint8_t                         erroredStreamNumber; /*!< output of reading the sp_errored_stream_number_register */
    adi_adrv903x_StreamProcType_e   streamProcType; /*!< would say what type of stream we are dealing with */
    adi_adrv903x_StreamError_e      streamErrEnum; /*!< output of reading the sp_rdbk_error_val in enum form */
} adi_adrv903x_StreamErr_t;

/**
* \brief Data structure to hold all StreamErr elements inside an array. 
*/
typedef struct adi_adrv903x_StreamErrArray
{
    adi_adrv903x_StreamErr_t    streamProcArray[ADI_ADRV903X_STREAM_MAX];
} adi_adrv903x_StreamErrArray_t;


/**
* \brief Enum to classify the abstract gpio's used for using gpio's to trigger Rx and/or Tx channels
*/
typedef enum adi_adrv903x_TxRxCtrlGpioLogicalPin
{ 
    ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_0 = 0U,
    ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_1 = 1U,
    ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_2 = 2U,
    ADRV903X_STREAM_TRIGGER_GPIO_LOGICAL_3 = 3U
} adi_adrv903x_TxRxCtrlGpioLogicalPin_e;

/**
* \brief Enum to classify the abstract gpio's used for using gpio's to trigger Rx and/or Tx channels
*/
typedef enum adi_adrv903x_TxRxCtrlGpioRxOrTx
{ 
    ADRV903X_STREAM_TRIGGER_GPIO_TXRXMASK_RX = 0x1U,
    ADRV903X_STREAM_TRIGGER_GPIO_TXRXMASK_TX = 0x2U
} adi_adrv903x_TxRxCtrlGpioRxOrTx_e;

/**
* \brief Data structure to hold all StreamErr elements inside an array. 
*/
typedef struct adi_adrv903x_TxRxCtrlGpioMap
{
    adi_adrv903x_TxRxCtrlGpioLogicalPin_e logicalPin;  /*!< Logical pin to be mapped internally with a physical gpio pin and a channel mask to control if channels are ON or OFF. */
    adi_adrv903x_GpioPinSel_e             gpioPin;     /*!< Physical gpio pin that when HIGH or LOW is to control the channels inside channelMask of Tx or Rx depending on rxTxMask value. */
    uint8_t                               channelMask; /*!< Mask containing the desired channels to be controlled. Bit 0 means channel 0 and so on. 1 means mapped to this gpio and 0 means ignored. */
    adi_adrv903x_TxRxCtrlGpioRxOrTx_e     rxTxMask;    /*!< Contains if the channels being controlled are for rx or tx. 1 means Rx and 2 means Tx. */
    
} adi_adrv903x_TxRxCtrlGpioMap_t;

#endif /* RADIOCTRL_TYPES */
