/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_radioctrl_types.h
* \brief Contains ADRV904X data types for radio ctrl features
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_RADIOCTRL_TYPES_H_
#define _ADI_ADRV904X_RADIOCTRL_TYPES_H_

#include "adi_adrv904x_lo_types.h"
#include "adi_adrv904x_dev_temp_types.h"
#include "adi_adrv904x_gpio_types.h"
#include "adi_adrv904x_rx_types.h"
#include "adi_adrv904x_tx_types.h"

#define ADI_ADRV904X_STREAM_MAX 20
#define ADI_ADRV904X_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (88*1024)
#define ADI_ADRV904X_TRX_CTRL_PIN_COUNT 8
#define ADI_ADRV904X_MAX_STREAMGPIO 24
#define ADI_ADRV904X_TX_TO_ORX_MAPPING_GPIO_MAX 8
#define ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE 16
#define ADI_ADRV904X_STREAM_VERSION_ADDR 0x46A00008

#define ADI_ADRV904X_MAX_RADIOSEQUENCER 16U
#define ADI_ADRV904X_MAX_CUSTOMERCONFIGSEQUENCER 10U
#define ADI_ADRV904X_CARRIER_TABLE_COUNT 8U
#define ADI_ADRV904X_DFE_CTRL_TX_TO_ORX_MAPPING_GPIO_MAX 8
#define ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG 24
#define ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA 16
#define ADI_ADRV904X_DTX_INPUT_MAX 8
#define ADI_ADRV904X_MAX_STREAM_GPIO_ANTENNA_CAL_OUT_PINS 8
#define ADI_ADRV904X_MODEL_SWITCH_INPUT_MAX 8

/**
*  \brief Enum of Tx Enable Modes
*/
typedef enum adi_adrv904x_TxEnableMode
{
    ADI_ADRV904X_TX_EN_SPI_MODE = 0, /*!< Selects SPI registers to control Tx signal path on/off */
    ADI_ADRV904X_TX_EN_PIN_MODE,     /*!< Selects Pin mode to control Tx signal path on/off. Tx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV904X_TX_EN_INVALID_MODE
} adi_adrv904x_TxEnableMode_e;

/**
*  \brief Enum of Rx Enable Modes
*/
typedef enum adi_adrv904x_RxEnableMode
{
    ADI_ADRV904X_RX_EN_SPI_MODE = 0, /*!< Selects SPI registers to control Rx signal path on/off */
    ADI_ADRV904X_RX_EN_PIN_MODE,     /*!< Selects Pin mode to control Rx signal path on/off. Rx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV904X_RX_EN_INVALID_MODE
} adi_adrv904x_RxEnableMode_e;

/**
*  \brief Enum of Rx Enable Modes
*/
typedef enum adi_adrv904x_ORxEnableMode
{
    ADI_ADRV904X_ORX_EN_SPI_MODE = 0,    /*!< Selects SPI registers to control ORx signal path on/off */
    ADI_ADRV904X_ORX_EN_PIN_MODE,        /*!< Selects Pin mode to control ORx signal path on/off. Rx signal path is controlled by dedicated input pins to CPU */
    ADI_ADRV904X_ORX_EN_INVALID_MODE
} adi_adrv904x_ORxEnableMode_e;
/**
 *  \brief Enum of the eight TxRx Enable Pins
 */
typedef enum adi_adrv904x_TxRxEnPin
{
    ADI_ADRV904X_TXRXEN_PIN0    = 0x01, /*!< TxRxEn Pin0 */
    ADI_ADRV904X_TXRXEN_PIN1    = 0x02, /*!< TxRxEn Pin1 */
    ADI_ADRV904X_TXRXEN_PIN2    = 0x04, /*!< TxRxEn Pin2 */
    ADI_ADRV904X_TXRXEN_PIN3    = 0x08, /*!< TxRxEn Pin3 */
    ADI_ADRV904X_TXRXEN_PIN4    = 0x10, /*!< TxRxEn Pin4 */
    ADI_ADRV904X_TXRXEN_PIN5    = 0x20, /*!< TxRxEn Pin5 */
    ADI_ADRV904X_TXRXEN_PIN6    = 0x40, /*!< TxRxEn Pin6 */
    ADI_ADRV904X_TXRXEN_PIN7    = 0x80, /*!< TxRxEn Pin7 */
    ADI_ADRV904X_TXRXEN_PINALL  = 0xFF  /*!< All TxRxEn pins */
} adi_adrv904x_TxRxEnPin_e;

/**
*  \brief Enum of the TxRx Enable Pin Configuration settings
*/
typedef enum adi_adrv904x_TxRxEnCfg
{
    ADI_ADRV904X_TXRXEN_TX_ENABLE_MAP       = 0x01,     /*!< Selects the Tx Enable mapping configuration */
    ADI_ADRV904X_TXRXEN_TX_ALTENABLE_MAP    = 0x02,     /*!< Selects the Tx Alternate Enable mapping configuration */
    ADI_ADRV904X_TXRXEN_RX_ENABLE_MAP       = 0x04,     /*!< Selects the Rx Enable mapping configuration */
    ADI_ADRV904X_TXRXEN_RX_ALTENABLE_MAP    = 0x08,     /*!< Selects the Rx AlternateEnable mapping configuration */
    ADI_ADRV904X_TXRXEN_ALL                 = 0x0F      /*!< Selects all members in the structure */
} adi_adrv904x_TxRxEnCfg_e;

/**
 *  \brief Enum to select desired Tx to ORx Mapping Mode to be used for GPIO/API control
 */
typedef enum adi_adrv904x_TxToOrxMappingMode
{
    ADI_ADRV904X_TX_ORX_MAPPING_MODE_2BIT       = 2U,   /*!< 2Bit Tx to ORx Mapping Mode*/
    ADI_ADRV904X_TX_ORX_MAPPING_MODE_3BIT       = 3U,   /*!< 3Bit Tx to ORx Mapping Mode*/
    ADI_ADRV904X_TX_ORX_MAPPING_MODE_4BIT       = 4U,   /*!< 4Bit Tx to ORx Mapping Mode*/
    ADI_ADRV904X_TX_ORX_MAPPING_MODE_6BIT       = 6U,   /*!< 6Bit Tx to ORx Mapping Mode*/
    ADI_ADRV904X_TX_ORX_MAPPING_MODE_8BIT       = 8U,   /*!< 8Bit Tx to ORx Mapping Mode*/

    ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL   = 32U,  /*!< DFE Controlled Tx to ORx Mapping Mode */
} adi_adrv904x_TxToOrxMappingMode_e;

/**
 *  \brief Enum to select desired Tx Channel Mapping for a GPIO Pin state
 */
typedef enum adi_adrv904x_TxToOrxMappingPinTable
{
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX0       = 0x000U,   /*!< Pin state selects Tx0 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX1       = 0x001U,   /*!< Pin state selects Tx1 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX2       = 0x002U,   /*!< Pin state selects Tx2 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX3       = 0x003U,   /*!< Pin state selects Tx3 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX4       = 0x004U,   /*!< Pin state selects Tx4 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX5       = 0x005U,   /*!< Pin state selects Tx5 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX6       = 0x006U,   /*!< Pin state selects Tx6 */
    ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX7       = 0x007U,   /*!< Pin state selects Tx7 */

    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX0         = 0x008U,   /*!< Tx0 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX1         = 0x009U,   /*!< Tx1 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX2         = 0x00AU,   /*!< Tx2 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX3         = 0x00BU,   /*!< Tx3 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX4         = 0x00CU,   /*!< Tx4 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX5         = 0x00DU,   /*!< Tx5 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX6         = 0x00EU,   /*!< Tx6 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX7         = 0x00FU,   /*!< Tx7 channel enabled for VSWR */
    ADI_ADRV904X_TX_TO_ORX_MAPPING_NO_CHG_FOR_ORX1  = 0x000U,   /*!< No Change to Current Mapping for ORX1 */
    ADI_ADRV904X_TX_TO_ORX_MAPPING_NO_CHG_FOR_ORX0  = 0x00FU    /*!< No Change to Current Mapping for ORX0 */
} adi_adrv904x_TxToOrxMappingPinTable_e;

/**
* \brief Data structure to hold Tx Signal Path Control configurations
*/
typedef struct adi_adrv904x_TxRadioCtrlModeCfg
{
    adi_adrv904x_TxEnableMode_e txEnableMode; /*!< Configuration for Tx Channel Enable mode */
    uint32_t txChannelMask;                   /*!< Tx channels for which configuration is valid */
} adi_adrv904x_TxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Rx Signal Path Control configurations
*/
typedef struct adi_adrv904x_RxRadioCtrlModeCfg
{
    adi_adrv904x_RxEnableMode_e rxEnableMode; /*!< Configuration for Rx Channel Enable mode */
    uint32_t rxChannelMask;                   /*!< Rx channels for which configuration is valid */
} adi_adrv904x_RxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold ORx Signal Path Control configurations
*/
typedef struct adi_adrv904x_ORxRadioCtrlModeCfg
{
    adi_adrv904x_ORxEnableMode_e orxEnableMode; /*!< Configuration for ORx Channel Enable mode */
    uint32_t orxChannelMask;                    /*!< ORx channels for which configuration is valid */
} adi_adrv904x_ORxRadioCtrlModeCfg_t;

/**
* \brief Data structure to hold Signal Path Control configurations
*/
typedef struct adi_adrv904x_RadioCtrlModeCfg
{
    adi_adrv904x_TxRadioCtrlModeCfg_t  txRadioCtrlModeCfg;  /*!< Tx signal path enable mode configuration */
    adi_adrv904x_RxRadioCtrlModeCfg_t  rxRadioCtrlModeCfg;  /*!< Rx signal path enable mode configuration */
    adi_adrv904x_ORxRadioCtrlModeCfg_t orxRadioCtrlModeCfg; /*!< ORx signal path enable mode configuration */
} adi_adrv904x_RadioCtrlModeCfg_t;

/**
* \brief Data structure to hold the LO configuration settings
*/
typedef struct adi_adrv904x_LoConfig
{
    adi_adrv904x_LoName_e                loName ;                /*!< Select the target RF LO source */
    uint64_t                             loFrequency_Hz ;        /*!< Desired RF LO frequency in Hz */
    adi_adrv904x_LoOption_e              loConfigSel ;           /*!< Select for LO config */
    } adi_adrv904x_LoConfig_t ;

/**
* \brief Data structure to hold the LO configuration read back values.
*/
typedef struct adi_adrv904x_LoConfigReadback
{
    adi_adrv904x_LoName_e                loName ;                /*!< Select the target RF LO source to read back */
    uint64_t                             loFrequency_Hz ;        /*!< RF LO frequency in Hz */
} adi_adrv904x_LoConfigReadback_t ;

/**
* \brief Data structure to hold the Radio Control GPIO pin information.
*/
typedef struct adi_adrv904x_RadioCtrlTxRxEnCfg
{
    uint8_t txEnMapping[ADI_ADRV904X_TRX_CTRL_PIN_COUNT]; /*!< Tx Enable Pin bitfield. Each bit in each byte corresponds to Each Tx Channel, eg txEnMapping[0] => ADI_ADRV904X_TX0. Use adi_adrv904x_TxChannels_e to build up bitmask. */
    uint8_t txAltMapping[ADI_ADRV904X_TRX_CTRL_PIN_COUNT]; /*!< Tx Alternate Enable Pin bitfield. Each bit in each byte corresponds to Each Tx Channel, eg txAltMapping[0] => ADI_ADRV904X_TX0. Use adi_adrv904x_TxChannels_e to build up bitmask. */
    uint8_t rxEnMapping[ADI_ADRV904X_TRX_CTRL_PIN_COUNT]; /*!< Rx Enable Pin bitfield. Each bit in each byte corresponds to Each Rx Channel, eg rxEnMapping[0] => ADI_ADRV904X_RX0. Use adi_adrv904x_RxChannels_e to build up bitmask. */
    uint8_t rxAltMapping[ADI_ADRV904X_TRX_CTRL_PIN_COUNT]; /*!< Rx Alternate Enable Pin bitfield. Each bit in each byte corresponds to Each Rx Channel, eg rxAltMapping[0] => ADI_ADRV904X_RX0. Use adi_adrv904x_RxChannels_e to build up bitmask. */
} adi_adrv904x_RadioCtrlTxRxEnCfg_t;

/**
* \brief Data structure to hold Synthesizer RF LO Loop filter settings
*/
typedef struct adi_adrv904x_LoLoopFilterCfg
{
    uint16_t loopBandwidth_kHz;            /*!< Synthesizer LO Loop filter bandwidth. Range 60-1000 */
    uint8_t  phaseMargin_degrees;          /*!< Synthesizer LO Loop filter phase margin in degrees. Range 45-75 */
} adi_adrv904x_LoLoopFilterCfg_t;

/**
* \brief Data structure to hold Stream GPIO pin assignments. 
*/
typedef struct adi_adrv904x_StreamGpioPinCfg
{ 
    adi_adrv904x_GpioPinSel_e streamGpInput[ADI_ADRV904X_MAX_STREAMGPIO]; /*!< Enable GPIO pin input to ADRV904X stream processor GP input. To enable set streamGpInput[0]=GPIO_00, streamGpInput[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_INVALID */

    adi_adrv904x_GpioPinSel_e streamGpDigOutput[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG];    /*!< Enable digital GPIO pin output to ADRV904X stream processor GP input. To enable set streamGpDigOutput[0]=GPIO_00, streamGpDigOutput[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_INVALID */
    adi_adrv904x_GpioAnaPinSel_e streamGpAnaOutput[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA]; /*!< Enable analog GPIO pin output to ADRV904X stream processor GP input. To enable set streamGpAnaOutput[0]=GPIO_00, streamGpAnaOutput[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_ANA_INVALID */
} adi_adrv904x_StreamGpioPinCfg_t;

/**
 * \brief Data structure to hold Tx to ORx configuration settings.
 */
typedef struct adi_adrv904x_TxToOrxMappingConfig
{
    uint16_t                                txObservability;                                                /*!< Tx observability word. This is a bitmask consisting of eight 2bit-pairs (nibbles), where each nibble makes a Tx channel observable by either ORx0 (set nibble to 2'b01 = 1), ORx1 (set nibble to 2'b10 = 2), or neither (set nibble to 2'b00 = 0). It is not allowed for a single Tx channel to be observable on both ORx0 and ORx1 (e.g. setting a nibble to 2'b11 = 3 is invalid). For example, setting B0 = Tx0 obs by ORx0, B1 = Tx0 obs by Orx1, ..., B8 = Tx4 obs by Orx0, B9 = Tx4 obs on Orx1, ..., B14 = Tx7 obs by Orx0, B15 = Tx7 obs on Orx1 */
    adi_adrv904x_TxToOrxMappingMode_e       mode;                                                           /*!< Tx to ORx Mapping Mode */
    adi_adrv904x_GpioPinSel_e               gpioSelect[ADI_ADRV904X_TX_TO_ORX_MAPPING_GPIO_MAX];            /*!< Select up to 8 GPIO to set mapping */
    adi_adrv904x_TxToOrxMappingPinTable_e   pinTableOrx0[ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];    /*!< ORX0 Look-Up Table for Tx Channel selection based on GPIO pin states */
    adi_adrv904x_TxToOrxMappingPinTable_e   pinTableOrx1[ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];    /*!< ORX1 Look-Up Table for Tx Channel selection based on GPIO pin states */
    uint8_t                                 autoSwitchOrxAttenEnable;                                       /*!< Auto-switching of Orx Atten value enabled. 0 = Disabled. 1 = Enabled */
    uint8_t                                 autoSwitchOrxNcoEnable;                                         /*!< Auto-switching of Orx NCO frequency enabled. 0 = Disabled. 1 = Enabled */

    adi_adrv904x_GpioPinSel_e               vswrDirOrx0Gpio;            /*!< Select GPIO to map VSWR to ORx0 */
    adi_adrv904x_GpioPinSel_e               vswrDirOrx1Gpio;            /*!< Select GPIO to map VSWR to ORx1 */
    adi_adrv904x_GpioPinSel_e               vswrDirCommonGpio;          /*!< Select GPIO to map VSWR to both ORx0 and ORx1 */
    uint8_t                                 vswrDirOrx0GpioPolarity;    /*!< Set GPIO polarity for ORx0 mapping. 0 = forward. 1 = reverse */
    uint8_t                                 vswrDirOrx1GpioPolarity;    /*!< Set GPIO polarity for ORx1 mapping. 0 = forward. 1 = reverse */
    uint8_t                                 vswrDirCommonGpioPolarity;  /*!< Set GPIO polarity for both ORx0 and ORx1 mapping. 0 = forward. 1 = reverse */
} adi_adrv904x_TxToOrxMappingConfig_t;

/**
 *  \brief Structure for preset data used in Tx To Orx Mapping Auto-Update of ORx Atten and NCO Freq based on Mapping
 */
typedef struct adi_adrv904x_TxToOrxMappingPresetNco
{
    int32_t     ncoFreqAdc_Khz;
    int32_t     ncoFreqDatapath_Khz;
} adi_adrv904x_TxToOrxMappingPresetNco_t;

/**
 * \brief loopfilter RX/TX LO command response structure
 */
typedef struct adi_adrv904x_RxTxLoFreqReadback
{
    adi_adrv904x_LoName_e   rxLoName   [ADI_ADRV904X_MAX_RX_ONLY];      /*!< PLL number of Rx Channels */
    uint32_t                rxFreq_Khz [ADI_ADRV904X_MAX_RX_ONLY];      /*!< Freq of Rx Channels */
    adi_adrv904x_LoName_e   txLoName   [ADI_ADRV904X_MAX_TXCHANNELS];   /*!< PLL number of Tx Channels */
    uint32_t                txFreq_Khz [ADI_ADRV904X_MAX_TXCHANNELS];   /*!< Freq of Tx Channels */
} adi_adrv904x_RxTxLoFreqReadback_t ;

/**
* \brief Struct for ADRV904X Antenna Cal Mode Pre-set Configuration for Rx
*/
typedef struct adi_adrv904x_RxRadioCtrlAntennaCalConfig
{
    uint32_t rxChannelMask;     /*!< Rx Channel selection */
    uint8_t rxGainIndex;        /*!< Rx Gain table index to use during Antenna Cal */
    int32_t rxNcoFreqKhz;       /*!< Rx NCO Frequency to use during Antenna Cal */
} adi_adrv904x_RxRadioCtrlAntennaCalConfig_t;

/**
* \brief Struct for ADRV904X Antenna Cal Mode Pre-set Configuration for Tx
*/
typedef struct adi_adrv904x_TxRadioCtrlAntennaCalConfig
{
    uint32_t txChannelMask;     /*!< Tx Channel selection */
    uint16_t txAttenuation_mdB; /*!< Tx Attenuation value to use during Antenna Cal [mdB] */
    int32_t txNcoFreqKhz;       /*!< Tx NCO Frequency to use during Antenna Cal */
} adi_adrv904x_TxRadioCtrlAntennaCalConfig_t;

/**
 *  \brief Enum to classify the stream type being analyzed 
 */
typedef enum adi_adrv904x_StreamProcType
{
    ADI_ADRV904X_STREAM_MAIN    = 0U,
    ADI_ADRV904X_STREAM_KFA     = 1U,
    ADI_ADRV904X_STREAM_RX      = 2U,
    ADI_ADRV904X_STREAM_TX      = 3U,
    ADI_ADRV904X_STREAM_ORX     = 4U    
} adi_adrv904x_StreamProcType_e;

/**
*  \brief Enum to indicate error cause (is the output of sp_rdbk_error_val register of each stream processor
*/
typedef enum adi_adrv904x_StreamError
{
    ADI_ADRV904X_STREAM_ERR_NONE                   = 0U,
    ADI_ADRV904X_STREAM_ERR_SPI_ADDR               = 1U,
    ADI_ADRV904X_STREAM_ERR_CHECK_INSTR            = 2U,
    ADI_ADRV904X_STREAM_ERR_TIMEOUT                = 4U,
    ADI_ADRV904X_STREAM_ERR_STACK_OVERFLOW         = 8U,    
    ADI_ADRV904X_STREAM_ERR_INVALID_STREAM_NUM     = 16U,    
    ADI_ADRV904X_STREAM_ERR_AHB_ADDR               = 32U,
    ADI_ADRV904X_STREAM_ERR_INVALID_INSTR          = 64U,
    ADI_ADRV904X_STREAM_ERR_EXT_TIMER              = 128U,
    ADI_ADRV904X_STREAM_ERR_FIFO_FULL              = 256U,
    ADI_ADRV904X_STREAM_ERR_POSTED_HRESP_ON_SYSBUS = 512U
} adi_adrv904x_StreamError_e;

/**
* \brief Data structure to hold each stream processor error information (the stream processor, the stream number and the error code)
*/
typedef struct adi_adrv904x_StreamErr
{
    uint8_t                         streamProcId; /*!< stream processor id. Together with streamProcType indicates the Stream Proc */
    uint8_t                         erroredStreamNumber; /*!< output of reading the sp_errored_stream_number_register */
    adi_adrv904x_StreamProcType_e   streamProcType; /*!< would say what type of stream we are dealing with */
    adi_adrv904x_StreamError_e      streamErrEnum; /*!< output of reading the sp_rdbk_error_val in enum form */
} adi_adrv904x_StreamErr_t;

/**
* \brief Data structure to hold all StreamErr elements inside an array. 
*/
typedef struct adi_adrv904x_StreamErrArray
{
    adi_adrv904x_StreamErr_t    streamProcArray[ADI_ADRV904X_STREAM_MAX];
} adi_adrv904x_StreamErrArray_t;

/**
* \brief Data structure holding the Radio Ctrl Pre Init
*/
typedef struct adi_adrv904x_RadioCtrlPreInit_t
{
    adi_adrv904x_GpioPinSel_e    radioSequencerSsbSyncGpioCtrl;                             /*!< Radio Sequencer Ssb Sync Gpio Ctrl Pin Select. Pin is allocated for SSB SYNC use only if Radio Sequencer is enabled. */
    adi_adrv904x_GpioPinSel_e    radioSequencerGpioDigOut[ADI_ADRV904X_GPIO_COUNT];         /*!< Enable Digital GPIO pin output from ADRV904X radio sequencer xbar. To enable set radioSequencerGpioDigOut[0]=GPIO_00, radioSequencerGpioDigOut[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_INVALID. No allocation of pins is made unles Radio Sequencer is enabled. */
    adi_adrv904x_GpioAnaPinSel_e radioSequencerGpioAnaOut[ADI_ADRV904X_GPIO_ANALOG_COUNT];  /*!< Enable Analog GPIO pin output from ADRV904X radio sequencer xbar. To enable set radioSequencerGpioAnaOut[0]=GPIO_00, radioSequencerGpioAnaOut[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_ANA_INVALID. No allocation of pins is made unles Radio Sequencer is enabled. */
} adi_adrv904x_RadioCtrlPreInit_t;

/**
*  \brief Enum for Vswr directions 
*/
typedef enum adi_adrv904x_DfeVswrDirection 
{
    ADI_ADRV904X_DFE_VSWR_REV = 0,   /*!< VSWR calibration reverse direction */
    ADI_ADRV904X_DFE_VSWR_FWD = 1    /*!< VSWR calibration forward direction */
} adi_adrv904x_DfeVswrDirection_e;

#define ADI_ADRV904X_ALARM_MAX    10
#define PA_EN_OUT_MAX             8
#define LNA_EN_OUT_MAX            8
#define OC_FUSE_EN_OUT_MAX        8
#define AUX_GRP0_OUT_MAX          8
#define AUX_GRP1_OUT_MAX          8

/**
 *  \brief Enum of PA_EN outputs
 */
typedef enum adi_adrv904x_PaEnOutputs
{
    ADI_ADRV904X_PA_EN_OUT_NONE = 0x0000,    /*!< PA_EN output none */
    ADI_ADRV904X_PA_EN_OUT_0    = 0x0001,    /*!< PA_EN output 0 */
    ADI_ADRV904X_PA_EN_OUT_1    = 0x0002,    /*!< PA_EN output 1 */
    ADI_ADRV904X_PA_EN_OUT_2    = 0x0004,    /*!< PA_EN output 2 */
    ADI_ADRV904X_PA_EN_OUT_3    = 0x0008,    /*!< PA_EN output 3 */
    ADI_ADRV904X_PA_EN_OUT_4    = 0x0010,    /*!< PA_EN output 4 */
    ADI_ADRV904X_PA_EN_OUT_5    = 0x0020,    /*!< PA_EN output 5 */
    ADI_ADRV904X_PA_EN_OUT_6    = 0x0040,    /*!< PA_EN output 6 */
    ADI_ADRV904X_PA_EN_OUT_7    = 0x0080,    /*!< PA_EN output 7 */
} adi_adrv904x_PaEnOutputs_e;

#define ADI_ADRV904X_PA_EN_OUT_ALL   (ADI_ADRV904X_PA_EN_OUT_0 | \
                                      ADI_ADRV904X_PA_EN_OUT_1 | \
                                      ADI_ADRV904X_PA_EN_OUT_2 | \
                                      ADI_ADRV904X_PA_EN_OUT_3 | \
                                      ADI_ADRV904X_PA_EN_OUT_4 | \
                                      ADI_ADRV904X_PA_EN_OUT_5 | \
                                      ADI_ADRV904X_PA_EN_OUT_6 | \
                                      ADI_ADRV904X_PA_EN_OUT_7)

/**
 *  \brief Enum of LNA_EN outputs
 */
typedef enum adi_adrv904x_LnaEnOutputs
{
    ADI_ADRV904X_LNA_EN_OUT_NONE = 0x0000, /*!< LNA_EN output none */
    ADI_ADRV904X_LNA_EN_OUT_0 = 0x0001,    /*!< LNA_EN output 0 */
    ADI_ADRV904X_LNA_EN_OUT_1 = 0x0002,    /*!< LNA_EN output 1 */
    ADI_ADRV904X_LNA_EN_OUT_2 = 0x0004,    /*!< LNA_EN output 2 */
    ADI_ADRV904X_LNA_EN_OUT_3 = 0x0008,    /*!< LNA_EN output 3 */
    ADI_ADRV904X_LNA_EN_OUT_4 = 0x0010,    /*!< LNA_EN output 4 */
    ADI_ADRV904X_LNA_EN_OUT_5 = 0x0020,    /*!< LNA_EN output 5 */
    ADI_ADRV904X_LNA_EN_OUT_6 = 0x0040,    /*!< LNA_EN output 6 */
    ADI_ADRV904X_LNA_EN_OUT_7 = 0x0080,    /*!< LNA_EN output 7 */
} adi_adrv904x_LnaEnOutputs_e;

#define ADI_ADRV904X_LNA_EN_OUT_ALL  (ADI_ADRV904X_LNA_EN_OUT_0 | \
                                      ADI_ADRV904X_LNA_EN_OUT_1 | \
                                      ADI_ADRV904X_LNA_EN_OUT_2 | \
                                      ADI_ADRV904X_LNA_EN_OUT_3 | \
                                      ADI_ADRV904X_LNA_EN_OUT_4 | \
                                      ADI_ADRV904X_LNA_EN_OUT_5 | \
                                      ADI_ADRV904X_LNA_EN_OUT_6 | \
                                      ADI_ADRV904X_LNA_EN_OUT_7)

/**
 *  \brief Enum of OC fuse enable outputs
 */
typedef enum adi_adrv904x_OcFuseEnOutputs
{
    ADI_ADRV904X_OC_FUSE_EN_OUT_NONE = 0x0000, /*!< OC fuse enable output none */
    ADI_ADRV904X_OC_FUSE_EN_OUT_0 = 0x0001,    /*!< OC fuse enable output 0 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_1 = 0x0002,    /*!< OC fuse enable output 1 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_2 = 0x0004,    /*!< OC fuse enable output 2 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_3 = 0x0008,    /*!< OC fuse enable output 3 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_4 = 0x0010,    /*!< OC fuse enable output 4 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_5 = 0x0020,    /*!< OC fuse enable output 5 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_6 = 0x0040,    /*!< OC fuse enable output 6 */
    ADI_ADRV904X_OC_FUSE_EN_OUT_7 = 0x0080,    /*!< OC fuse enable output 7 */
} adi_adrv904x_OcFuseEnOutputs_e;

#define ADI_ADRV904X_OC_FUSE_EN_OUT_ALL   (ADI_ADRV904X_OC_FUSE_EN_OUT_0 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_1 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_2 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_3 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_4 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_5 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_6 | \
                                           ADI_ADRV904X_OC_FUSE_EN_OUT_7)

/**
 *  \brief Enum of AUX Group0 outputs
 */
typedef enum adi_adrv904x_AuxGrp0Outputs
{
    ADI_ADRV904X_AUX_GRP0_OUT_NONE = 0x0000, /*!< AUX Group0 output none */
    ADI_ADRV904X_AUX_GRP0_OUT_0 = 0x0001,    /*!< AUX Group0 output 0 */
    ADI_ADRV904X_AUX_GRP0_OUT_1 = 0x0002,    /*!< AUX Group0 output 1 */
    ADI_ADRV904X_AUX_GRP0_OUT_2 = 0x0004,    /*!< AUX Group0 output 2 */
    ADI_ADRV904X_AUX_GRP0_OUT_3 = 0x0008,    /*!< AUX Group0 output 3 */
    ADI_ADRV904X_AUX_GRP0_OUT_4 = 0x0010,    /*!< AUX Group0 output 4 */
    ADI_ADRV904X_AUX_GRP0_OUT_5 = 0x0020,    /*!< AUX Group0 output 5 */
    ADI_ADRV904X_AUX_GRP0_OUT_6 = 0x0040,    /*!< AUX Group0 output 6 */
    ADI_ADRV904X_AUX_GRP0_OUT_7 = 0x0080,    /*!< AUX Group0 output 7 */
} adi_adrv904x_AuxGrp0Outputs_e;

#define ADI_ADRV904X_AUX_GRP0_OUT_ALL  (ADI_ADRV904X_AUX_GRP0_OUT_0 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_1 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_2 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_3 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_4 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_5 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_6 | \
                                        ADI_ADRV904X_AUX_GRP0_OUT_7)

/**
 *  \brief Enum of AUX Group1 outputs
 */
typedef enum adi_adrv904x_AuxGrp1Outputs
{
    ADI_ADRV904X_AUX_GRP1_OUT_NONE = 0x0000, /*!< AUX Group1 output none */
    ADI_ADRV904X_AUX_GRP1_OUT_0 = 0x0001,    /*!< AUX Group1 output 0 */
    ADI_ADRV904X_AUX_GRP1_OUT_1 = 0x0002,    /*!< AUX Group1 output 1 */
    ADI_ADRV904X_AUX_GRP1_OUT_2 = 0x0004,    /*!< AUX Group1 output 2 */
    ADI_ADRV904X_AUX_GRP1_OUT_3 = 0x0008,    /*!< AUX Group1 output 3 */
    ADI_ADRV904X_AUX_GRP1_OUT_4 = 0x0010,    /*!< AUX Group1 output 4 */
    ADI_ADRV904X_AUX_GRP1_OUT_5 = 0x0020,    /*!< AUX Group1 output 5 */
    ADI_ADRV904X_AUX_GRP1_OUT_6 = 0x0040,    /*!< AUX Group1 output 6 */
    ADI_ADRV904X_AUX_GRP1_OUT_7 = 0x0080,    /*!< AUX Group1 output 7 */
} adi_adrv904x_AuxGrp1Outputs_e;

#define ADI_ADRV904X_AUX_GRP1_OUT_ALL  (ADI_ADRV904X_AUX_GRP1_OUT_0 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_1 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_2 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_3 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_4 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_5 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_6 | \
                                        ADI_ADRV904X_AUX_GRP1_OUT_7)

/**
* \brief Enum of available output GPIO pin states
*/
typedef enum adi_adrv904x_OutGpioPinStates
{
    ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF = 0,       /*!< Select output GPIO pin ON state */
    ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON = 1,        /*!< Select output GPIO pin OFF state */
    ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z = 2,    /*!< Select output GPIO pin HIGH_Z state*/
    ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP = 3  /*!< Select output GPIO pin NORMAL OPERATION state */
} adi_adrv904x_OutGpioPinStates_e;

/**
* \brief Enum of available alarm input GPIO pin modes
*/
typedef enum adi_adrv904x_AlarmInputGpioPinModes
{
    ADI_ADRV904X_ALARM_INPUT_GPIO_PIN_MODE_ACTIVE_LOW = 0,   /*!< Select Active Low mode for alarm input GPIO pin */
    ADI_ADRV904X_ALARM_INPUT_GPIO_PIN_MODE_ACTIVE_HIGH = 1,  /*!< Select Active High mode for alarm input GPIO pin */
} adi_adrv904x_AlarmInputGpioPinModes_e;

/**
* \brief Enum of available output GPIO pin modes
*/
typedef enum adi_adrv904x_OutGpioPinModes
{
    ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW = 0,   /*!< Select Active Low mode for GPIO pin */
    ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH = 1,  /*!< Select Active High mode for GPIO pin */
    ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z = 2        /*!< Select High_Z mode for GPIO pin */
} adi_adrv904x_OutGpioPinModes_e;

/**
* \brief Enum of output GPIO signal types
*/
typedef enum adi_adrv904x_OutGpioSigTypes
{
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN = 0x0000,       /*!< LNA_EN GPIO signal type. Valid signal range: 0..7 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN = 0x0001,        /*!< PA_EN GPIO signal type. Valid signal range: 0..7 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_TDD_SW_EN = 0x0002,    /*!< TDD_SW_EN GPIO signal type. Valid signal range: 1 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_DRV_PA_EN = 0x0003,    /*!< DRV_PA_EN GPIO signal type. Valid signal range: 1 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EN = 0x0004,     /*!< FB_SW_EN GPIO signal type. Valid signal range: 0..7 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP0_OUT = 0x0005, /*!< AUX_GRP0_OUT GPIO signal type. Valid signal range: 0..7 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_FB_SW_EXT_EN = 0x0006, /*!< FB_SW_EXT_EN GPIO signal type. Valid signal range: 0..7 */
    ADI_ADRV904X_OUT_GPIO_SIG_TYPE_AUX_GRP1_OUT = 0x0007  /*!< AUX_GRP1_OUT GPIO signal type. Valid signal range: 0..7 */
} adi_adrv904x_OutGpioSigTypes_e;

/**
 *  \brief Enum of available alarms
 */
typedef enum adi_adrv904x_Alarms
{
    ADI_ADRV904X_ALARM_0 = 0x0001, /*!< Alarm 0 */
    ADI_ADRV904X_ALARM_1 = 0x0002, /*!< Alarm 1 */
    ADI_ADRV904X_ALARM_2 = 0x0004, /*!< Alarm 2 */
    ADI_ADRV904X_ALARM_3 = 0x0008, /*!< Alarm 3 */
    ADI_ADRV904X_ALARM_4 = 0x0010, /*!< Alarm 4 */
    ADI_ADRV904X_ALARM_5 = 0x0020, /*!< Alarm 5 */
    ADI_ADRV904X_ALARM_6 = 0x0040, /*!< Alarm 6 */
    ADI_ADRV904X_ALARM_7 = 0x0080, /*!< Alarm 7 */
    ADI_ADRV904X_ALARM_8 = 0x0100, /*!< Alarm 8 */
    ADI_ADRV904X_ALARM_9 = 0x0200, /*!< Alarm 9 */
} adi_adrv904x_Alarms_e;

#define ADI_ADRV904X_ALARM_MASK_ALL    (ADI_ADRV904X_ALARM_0 | \
                                        ADI_ADRV904X_ALARM_1 | \
                                        ADI_ADRV904X_ALARM_2 | \
                                        ADI_ADRV904X_ALARM_3 | \
                                        ADI_ADRV904X_ALARM_4 | \
                                        ADI_ADRV904X_ALARM_5 | \
                                        ADI_ADRV904X_ALARM_6 | \
                                        ADI_ADRV904X_ALARM_7 | \
                                        ADI_ADRV904X_ALARM_8 | \
                                        ADI_ADRV904X_ALARM_9)
 
/**
 *  \brief Enum of available alarm types
 */
typedef enum adi_adrv904x_AlarmType
{
    ADI_ADRV904X_ALARM_TYPE_NONE  = 0x0000,   /*!< Unused alarm input */
    ADI_ADRV904X_ALARM_TYPE_OCP   = 0x0001,   /*!< Selects OCP as an alarm type */
    ADI_ADRV904X_ALARM_TYPE_CFPGA = 0x0002,   /*!< Selects Control FPGA as an alarm type */
    ADI_ADRV904X_MAX_ALARM_TYPE   = 0x0002     /*!< Max value allowed for alarm type */
} adi_adrv904x_AlarmType_e;

#define ADI_ADRV904X_ALARM_TYPE_MASK_ALL    (ADI_ADRV904X_ALARM_TYPE_OCP | ADI_ADRV904X_ALARM_TYPE_CFPGA)

/**
 *  \brief Enum of available alarm states
 */
typedef enum adi_adrv904x_AlarmState
{
    ADI_ADRV904X_ALARM_STATE_INACTIVE = 0x0000,      /*!< Alarm is not active */
    ADI_ADRV904X_ALARM_STATE_ACTIVE   = 0x0001       /*!< Alarm is active */
} adi_adrv904x_AlarmState_e;

/**
 *  \brief Enum of available output states
 */
typedef enum adi_adrv904x_OutputState_e
{
    ADI_ADRV904X_OUTPUT_DISABLE = 0x0000,  /*!< Disable Output */
    ADI_ADRV904X_OUTPUT_ENABLE  = 0x0001,  /*!< Enable Output */
} adi_adrv904x_OutputState_e;
    
/**
 *  \brief Enum of available PA_EN inputs
 */
typedef enum adi_adrv904x_PaEnInputSel
{
    ADI_ADRV904X_PA_EN_INPUT_SEL_RS  = 0,       /*!< Select PA_EN signal from RS  */
    ADI_ADRV904X_PA_EN_INPUT_SEL_DTX_RS = 1,    /*!< Select DTX signal ANDed with PA_EN */
} adi_adrv904x_PaEnInputSel_e;

/**
  * \brief Data structure to hold alarm configuration
  */
typedef struct adi_adrv904x_RadioCtrlAlarmConfig
{
    adi_adrv904x_AlarmType_e  alarmType;   /*!< Source of the alarm */
    uint32_t                  lnaEnOut;    /*!< Used LNA_EN Output. Multi-bits mask of adi_adrv904x_LnaEnOutputs_e */
    uint32_t                  paEnOut;     /*!< Used PA_EN Output. Multi-bits mask of adi_adrv904x_PaEnOutputs_e */
    uint32_t                  ocFuseEnOut; /*!< Used OC Fuse Enable Output. Multi-bits mask of adi_adrv904x_OcFuseEnOutputs_e */
    uint32_t                  auxGrp0Out;  /*!< Used AUX_GRP0 Output. Multi-bits mask of adi_adrv904x_AuxGrp0Outputs_e */
    uint32_t                  auxGrp1Out;  /*!< Used AUX_GRP1 Output. Multi-bits mask of adi_adrv904x_AuxGrp1Outputs_e */
} adi_adrv904x_RadioCtrlAlarmConfig_t;

/**
  * \brief Data structure to config customer special frame timing pattern ID for each radio sequencer
  */
typedef struct adi_adrv904x_RsSpecialPattern
{
    uint8_t                   specialPatternUsed;  /*!< Customer special frame timing pattern config flag. true: use customer special pattern; false, use default */
    uint32_t                  specialPatternId;    /*!< Customer special frame timing pattern ID when specialPatternUsed is true */
} adi_adrv904x_RsSpecialPattern_t;

/**
* \brief Data structure to hold all special radio sequencer pattern elements inside an array 
*/
typedef struct adi_adrv904x_RsSpecialSequencer
{
    adi_adrv904x_RsSpecialPattern_t  specialSequencer[ADI_ADRV904X_MAX_CUSTOMERCONFIGSEQUENCER]; /*!< Customer special frame timing pattern ID for each radio sequencer */
} adi_adrv904x_RsSpecialSequencer_t;

typedef struct adi_adrv904x_RadioCtrlAlarmGpioConfig
{
    adi_adrv904x_GpioPinSel_e              alarmInputGpio[ADI_ADRV904X_ALARM_MAX];         /*!< Digital GPIOs selected as alarm input */
    adi_adrv904x_AlarmInputGpioPinModes_e  alarmInputGpioPinMode[ADI_ADRV904X_ALARM_MAX];  /*!< Selected as alarm input GPIO pin mode */

    adi_adrv904x_GpioPinSel_e              lnaEnOutDigGpio[LNA_EN_OUT_MAX];                /*!< Digital GPIOs selected as LNA_EN outputs */
    adi_adrv904x_GpioAnaPinSel_e           lnaEnOutAnaGpio[LNA_EN_OUT_MAX];                /*!< Analog GPIOs selected as LNA_EN outputs */
    adi_adrv904x_OutGpioPinModes_e         lnaEnOutGpioPinMode;                            /*!< Selected as LNA_EN outputs pin mode */

    adi_adrv904x_GpioPinSel_e              paEnOutDigGpio[PA_EN_OUT_MAX];                  /*!< Digital GPIOs selected as LNA_EN outputs */
    adi_adrv904x_GpioAnaPinSel_e           paEnOutAnaGpio[PA_EN_OUT_MAX];                  /*!< Analog GPIOs selected as LNA_EN outputs */
    adi_adrv904x_OutGpioPinModes_e         paEnOutGpioPinMode;                             /*!< Selected as PA_EN outputs pin mode */

    adi_adrv904x_GpioPinSel_e              ocFuseEnOutDigGpio[OC_FUSE_EN_OUT_MAX];         /*!< Digital GPIOs selected as OC_FUSE_EN outputs */
    adi_adrv904x_GpioAnaPinSel_e           ocFuseEnOutAnaGpio[OC_FUSE_EN_OUT_MAX];         /*!< Analog GPIOs selected as OC_FUSE_EN outputs */
    adi_adrv904x_OutGpioPinModes_e         ocFuseEnOutGpioPinMode;                         /*!< Selected as OC_FUSE outputs pin mode */

    adi_adrv904x_GpioPinSel_e              auxGrp0OutDigGpio[AUX_GRP0_OUT_MAX];            /*!< Digital GPIOs selected as AUX GRP0 outputs */
    adi_adrv904x_GpioAnaPinSel_e           auxGrp0OutAnaGpio[AUX_GRP0_OUT_MAX];            /*!< Analog GPIOs selected as AUX GRP0 outputs */
    adi_adrv904x_OutGpioPinModes_e         auxGrp0OutGpioPinMode[AUX_GRP0_OUT_MAX];        /*!< Selected as AUX GRP0 outputs pin mode */

    adi_adrv904x_GpioPinSel_e              auxGrp1OutDigGpio[AUX_GRP1_OUT_MAX];            /*!< Digital GPIOs selected as AUX GRP1 outputs */
    adi_adrv904x_GpioAnaPinSel_e           auxGrp1OutAnaGpio[AUX_GRP1_OUT_MAX];            /*!< Analog GPIOs selected as AUX GRP1 outputs */
    adi_adrv904x_OutGpioPinModes_e         auxGrp1OutGpioPinMode[AUX_GRP1_OUT_MAX];        /*!< Selected as AUX GRP1 outputs pin mode */
} adi_adrv904x_RadioCtrlAlarmGpioConfig_t;



/**
 * \brief Data structure to hold antenna calibration initial configuration
 */
typedef struct adi_adrv904x_RadioCtrlAntCalCarrierCfgInit
{
    uint32_t    commonRxMask;                                          /*!< Common Rx channels mask. It should be 0 if there is no common Rx channel.
                                                                            It can have multiple bits set, Koror can have multiple Rx channels as common Rx channel */
    uint32_t    commonTxMask;                                          /*!< Common Tx channels mask. It should be 0 if there is no common Tx channel. 
                                                                            It can have multiple bits set, Koror can have multiple Tx channels as common Tx channel */
} adi_adrv904x_RadioCtrlAntCalCarrierCfgInit_t;

/**
 * \brief Data structure to hold antenna calibration configuration
 */
typedef struct adi_adrv904x_RadioCtrlAntCalCarrierCfg
{
    uint32_t    patternId;                                        /*!< Pattern id to switch when antenna cal pin is asserted */
    uint8_t     numOfGpioToSwitchCarrier;                         /*!< Number of pattern groups (number of GPIO pin assertions) to switch to next carrier gain */
    uint8_t     carrierTable[ADI_ADRV904X_CARRIER_TABLE_COUNT];   /*!< Carrier gain table to be used during antenna cal. this table should be the order of carrier
                                                                       id's to be calibrated. If not all carriers will be calibrated, table should be padded with 255.
                                                                       E.g. if just calibrate C0/C1/C2/C3 table should have 0, 1, 2, 3, 255, 255, 255, 255 */
    uint8_t     firstPatternNumberOfLoops;                        /*!< Number of loops for the first pattern during antenna cal */
    uint8_t     secondPatternNumberOfLoops;                       /*!< Number of loops for the second pattern during antenna cal */
    int32_t     rxCarrierGainForAntCal[ADI_ADRV904X_MAX_RX_CARRIERS]; /*!< Gain value for each Rx carriers, range from -90000 to 36000, unit, mdB */
    int32_t     txCarrierGainForAntCal[ADI_ADRV904X_MAX_TX_CARRIERS]; /*!< Gain value for each Tx carriers, range from -90000 to 36000, unit, mdB */
} adi_adrv904x_RadioCtrlAntCalCarrierCfg_t;



/**
 * \brief Data structure to hold DFE Control Mode Specific Tx to ORx configuration settings. Not for use in RCI mode.
 */
typedef struct adi_adrv904x_DfeCtrlTxToOrxMappingConfig
{
    uint16_t                                txObservability;                                                    /*!< Tx observability word. This is a bitmask consisting of eight 2bit-pairs (nibbles), where each nibble makes a Tx channel observable by either ORx0 (set nibble to 2'b01 = 1), ORx1 (set nibble to 2'b10 = 2), or neither (set nibble to 2'b00 = 0). It is not allowed for a single Tx channel to be observable on both ORx0 and ORx1 (e.g. setting a nibble to 2'b11 = 3 is invalid). For example, setting B0 = Tx0 obs by ORx0, B1 = Tx0 obs by Orx1, ..., B8 = Tx4 obs by Orx0, B9 = Tx4 obs on Orx1, ..., B14 = Tx7 obs by Orx0, B15 = Tx7 obs on Orx1 */
    adi_adrv904x_TxToOrxMappingMode_e       mode;                                                               /*!< Tx to ORx Mapping Mode */
    adi_adrv904x_GpioGenericPinSel_e        gpioSelectOrx0[ADI_ADRV904X_DFE_CTRL_TX_TO_ORX_MAPPING_GPIO_MAX];   /*!< Select up to 4 Digital/Analog GPIO output pins to drive based on current Mapping for Orx0*/
    adi_adrv904x_GpioGenericPinSel_e        gpioSelectOrx1[ADI_ADRV904X_DFE_CTRL_TX_TO_ORX_MAPPING_GPIO_MAX];   /*!< Select up to 4 Digital/Analog GPIO output pins to drive based on current Mapping for Orx1*/
    uint32_t                                pinTableOrx0[ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];        /*!< Select GPIO output pin states to drive for any given Tx to ORx Mapping of Orx0 */
    uint32_t                                pinTableOrx1[ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE];        /*!< Select GPIO output pin states to drive for any given Tx to ORx Mapping of Orx1 */
    uint8_t                                 autoSwitchOrxAttenEnable;                                           /*!< Auto-switching of Orx Atten value enabled. 0 = Disabled. 1 = Enabled */
    uint8_t                                 autoSwitchOrxNcoEnable;                                             /*!< Auto-switching of Orx NCO frequency enabled. 0 = Disabled. 1 = Enabled */
} adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t;

/**
* \brief Data structure to hold DTX input pin assignments. 
*/
typedef struct adi_adrv904x_DtxInputPinCfg
{
    adi_adrv904x_GpioPinSel_e dtxInput[ADI_ADRV904X_DTX_INPUT_MAX]; /*!< Enable DTX pin input to ADRV904X stream processor input. To enable set dtxInput[0]=GPIO_00, dtxInput[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_INVALID */
    uint32_t                  dtxEventWaitTimeNs;                   /*!< DTX Event Wait Time in [ns] */
} adi_adrv904x_DtxInputPinCfg_t;

/**
* \brief Data structure to hold Stream GPIO AntCal Output pin assignments loaded in Stream Processor binary
*/
typedef struct adi_adrv904x_StreamGpioAntCalOutCfg
{
    adi_adrv904x_GpioGenericPinSel_e        gpioSelect[ADI_ADRV904X_MAX_STREAM_GPIO_ANTENNA_CAL_OUT_PINS];  /*!< Stream GPIO Antenna Cal Output assignments */
} adi_adrv904x_StreamGpioAntCalOutCfg_t;

/**
* \brief Data structure to hold Model Switch input pin assignments.
*/
typedef struct adi_adrv904x_ModelSwitchInputPinCfg
{
    adi_adrv904x_GpioPinSel_e modelSwitchInput[ADI_ADRV904X_MODEL_SWITCH_INPUT_MAX]; /*!< Enable Model Switch pin input to ADRV904X stream processor input. To enable set modelSwitchInput[0]=GPIO_00, modelSwitchInput[1]=GPIO_01,,etc. To disable select ADI_ADRV904X_GPIO_INVALID */
} adi_adrv904x_ModelSwitchInputPinCfg_t;

/**
* \brief Data structure to hold DTX fanout to PA_EN outputs for all TX channels
*/
typedef struct adi_adrv904x_DtxFanoutToPaEnCfg
{
    uint32_t  fanout[ADI_ADRV904X_MAX_TXCHANNELS]; /*!< Select PA_EN outputs to be driven by DTX for any given Tx. */
} adi_adrv904x_DtxFanoutToPaEnCfg_t;
#endif /* RADIOCTRL_TYPES */
