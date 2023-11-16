/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/*!
* \file adi_adrv904x_types.h
* \brief Contains ADRV904X API configuration and run-time type definitions
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_TYPES_H_
#define _ADI_ADRV904X_TYPES_H_

#include "adi_common.h"
#include "adi_adrv904x_user.h"
#include "adi_adrv904x_cpu_types.h"
#include "adi_adrv904x_radioctrl_types.h"
#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_rx_types.h"
#include "adi_adrv904x_gpio_types.h"
#include "adi_adrv904x_datainterface_types.h"

#include "adi_adrv904x_radio_seq_image_format_types.h"
#include "adi_adrv904x_carrier_reconfigure_types.h"
#include "adi_adrv904x_dfe_cal_dpd_types.h"
#include "adi_adrv904x_dfe_cfr_types.h"

#define ADI_ADRV904X_TX_PROFILE_VALID    0x01
#define ADI_ADRV904X_RX_PROFILE_VALID    0x02
#define ADI_ADRV904X_ORX_PROFILE_VALID   0x04
#define ADI_ADRV904X_NUM_SHARED_RESOURCES 40U 

#define ADRV904X_ADDR_SPI_INTERFACE_CONFIG_A     0x0000
#define ADRV904X_CONFIG_A_SPI_LSB_FIRST          0x42
#define ADRV904X_CONFIG_A_SPI_ADDR_ASCENSION     0x24
#define ADRV904X_CONFIG_A_SPI_SDO_ACTIVE         0x18
#define ADRV904X_ADDR_DIGITAL_IO_CONTROL         0x0010
#define ADRV904X_IO_CONTROL_SPI2_OUTS_DRV_SEL    0x02
#define ADRV904X_IO_CONTROL_SPI_OUTS_DRV_SEL     0x01
#define ADRV904X_BF_CORE_ADDR                    0U

#define ADRV904X_ADDR_SPI_INTERFACE_CONFIG_B     0x0001
#define ADRV904X_CONFIG_B_SINGLE_INSTRUCTION     0x80

#define ADI_ADRV904X_MAX_UART    4U

#define ADI_ADRV904X_MAX_RXCHANNELS     10U
#define ADI_ADRV904X_MAX_RX_ONLY        8U
#define ADI_ADRV904X_MAX_ORX            2U

#define ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET 12

#define ADI_ADRV904X_MAX_SERDES_LANES 8U  /* Number of lanes in serializer and deserializer */

#define ADI_ADRV904X_MAX_CHANNELS 8

#define ADI_ADRV904X_MAX_FEATUREMASK 16

/**
 *  \brief Enum of possible channel enables
 */
typedef enum adi_adrv904x_Channels
{
    ADI_ADRV904X_CHOFF = 0x00, /*!< No channels are enabled */
    ADI_ADRV904X_CH0 = 0x01, /*!< Channel 0 enabled */
    ADI_ADRV904X_CH1 = 0x02, /*!< Channel 1 enabled */
    ADI_ADRV904X_CH2 = 0x04, /*!< Channel 2 enabled */
    ADI_ADRV904X_CH3 = 0x08, /*!< Channel 3 enabled */
    ADI_ADRV904X_CH4 = 0x10, /*!< Channel 4 enabled */
    ADI_ADRV904X_CH5 = 0x20, /*!< Channel 5 enabled */
    ADI_ADRV904X_CH6 = 0x40, /*!< Channel 6 enabled */
    ADI_ADRV904X_CH7 = 0x80 /*!< Channel 7 enabled */
} adi_adrv904x_Channels_e;

/**
 *  \brief Data structure to hold ADRV904X API State
 */
typedef enum adi_adrv904x_ApiStates
{
    ADI_ADRV904X_STATE_POWERONRESET         = 0x001U,     /*!< POR Complete */
    ADI_ADRV904X_STATE_INITIALIZED          = 0x002U,     /*!< Initialized */
    ADI_ADRV904X_STATE_STREAMLOADED         = 0x004U,     /*!< Stream Processor Loaded */
    ADI_ADRV904X_STATE_CPUDEBUGLOADED       = 0x008U,     /*!< CPU Debug SW Loaded */
    ADI_ADRV904X_STATE_CPU0LOADED           = 0x010U,     /*!< CPU 0 Loaded */
    ADI_ADRV904X_STATE_CPU1LOADED           = 0x020U,     /*!< CPU 1 Loaded */
    ADI_ADRV904X_STATE_ALLCPUSLOADED        = 0x040U,     /*!< All CPU's Loaded */
    ADI_ADRV904X_STATE_INITCALS_RUN         = 0x080U,     /*!< Init Cals Running */
    ADI_ADRV904X_STATE_RADIOON              = 0x100U,     /*!< Device Running */
    ADI_ADRV904X_STATE_DFECPULOADED         = 0x200U,     /*!< Dfe Cpu Loaded */
    ADI_ADRV904X_STATE_RSLOADED             = 0x400U,     /*!< Radio Sequencer Loaded */
    ADI_ADRV904X_STATE_STANDBY              = 0x800U,     /*!< Part is Powered down in Standby mode */
    ADI_ADRV904X_STATE_JRXREPAIRED          = 0x1000U,    /*!< Jrx repair has been applied */
    ADI_ADRV904X_STATE_MCSCARRIERRECONFIG   = 0x2000U,    /*!< Mcs carrier reconfig has been applied */
} adi_adrv904x_ApiStates_e;

/**
 * \brief Enumerated list of CMOS pads drive strength options for SPI_DO signal.
 * 
 * Note: These are the values used in the device's SpiCfgSettings, resource files and as arguments to
 * SpiDoutPadDriveStrengthSet, and are chosen to maintain binary compatability with API versions before
 * _STRENGTH_1 and _2 were added. The corresponding bitfield values written to the device differ.
 */
typedef enum adi_adrv904x_CmosPadDrvStr
{
    ADI_ADRV904X_CMOSPAD_DRV_WEAK       = 0U,       /*!<  Minimum pad drive strength */
    ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_1 = 2U,       /*!<  Is stronger than WEAK */
    ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_2 = 3U,       /*!<  Is stronger than STRENGTH_1 */
    ADI_ADRV904X_CMOSPAD_DRV_STRONG     = 1U,       /*!<  Maximum pad drive strength */
} adi_adrv904x_CmosPadDrvStr_e;

/**
* \brief Enumerated list of options to use to set the LO source for the Rx/Tx and ORx mixers.
*/
typedef enum adi_adrv904x_LoSel
{
    ADI_ADRV904X_LOSEL_LO0 = 0,    /*!< LO0 selection */
    ADI_ADRV904X_LOSEL_LO1 = 1,    /*!< LO1 selection */
    } adi_adrv904x_LoSel_e;

/**
*  \brief Enum of possible Capability Feature enables
*/
typedef enum adi_adrv904x_Feature
{
    ADI_ADRV904X_FEATUREOFF         = 0x0000,           /*!< No Capability Feature enabled */
    ADI_ADRV904X_FEATURE_RESERVED1  = 0x0001,           /*!< Reserve1 feature enabled */
    ADI_ADRV904X_FEATURE_SPI2       = 0x0002,           /*!< SPI2 feature enabled */
    ADI_ADRV904X_FEATURE_TXSRD      = 0x0004,           /*!< Tx Slew Rate Detector feature enabled */
    ADI_ADRV904X_FEATURE_RESERVED2  = 0x0008            /*!< Reserve2 feature enabled jesd compression */
} adi_adrv904x_Feature_e;

/**
 *  \brief Enum of Device Capability Features case index
 */
typedef enum adi_adrv904x_FeatureCaseIndex
{
    ADI_ADRV904X_FEATURE_SERIALIZER    = 0x00,         /*!< serializer feature case */
    ADI_ADRV904X_FEATURE_DESERIALIZER  = 0x01,         /*!< deserializer feature case */
    ADI_ADRV904X_FEATURE_TX            = 0x02,         /*!< txchannel feature case */
    ADI_ADRV904X_FEATURE_RX            = 0x03,         /*!< rxchannel feature case */
    ADI_ADRV904X_FEATURE_ORX_LOMASK    = 0x04          /*!< orxchannel and lomask features case */
} adi_adrv904x_FeatureCaseIndex_e;

/**
 * \brief MCS Sync Mode
 */
typedef enum adi_adrv904x_McsSyncMode
{
    ADI_ADRV904X_MCS_OFF               = 0U,            /*!< Notification that MCS is off */
    ADI_ADRV904X_MCS_START             = 1U << 0,       /*!< Notification that MCS is started */

    ADI_ADRV904X_MCS_CARRIER_RECONFIG  = 1U << 1        /*!< Notification that MCS carrier reconfig is on going */
} adi_adrv904x_McsSyncMode_e;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv904x_TxChannelCfg
{
    adi_adrv904x_TxAttenCfg_t           txAttenCfg;          /*!< Tx attenuation config */
    uint16_t                            txAttenInit_mdB;     /*!< Tx attenuation init time value */
    adi_adrv904x_PowerMonitorCfg_t      txpowerMonitorCfg;   /*!< Tx power monitor configuration */
    adi_adrv904x_SlewRateDetectorCfg_t  srlCfg;              /*!< Slew rate detector config */
    adi_adrv904x_ProtectionRampCfg_t    protectionRampCfg;   /*!< Tx Pa protection ramp config */
} adi_adrv904x_TxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV904X Tx Channel configuration settings
 */
typedef struct adi_adrv904x_TxSettings
{
    adi_adrv904x_TxChannelCfg_t     txChannelCfg[ADI_ADRV904X_MAX_TXCHANNELS];  /*!< Tx settings per Tx channel */
} adi_adrv904x_TxSettings_t;


/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv904x_RxChannelCfg
{
    adi_adrv904x_RxDataFormat_t         rxDataFormat;     /*!< Rx Data Format settings structure */
    uint8_t                             rxGainIndexInit;  /*!< Init Gain Index value for the channels selected */
} adi_adrv904x_RxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV904X Rx Channel configuration settings
 */
typedef struct adi_adrv904x_RxSettings
{
    adi_adrv904x_RxChannelCfg_t rxChannelCfg[ADI_ADRV904X_MAX_RXCHANNELS]; /*!< Rx settings per Rx channel */
} adi_adrv904x_RxSettings_t;

/*
* \brief Data structure to hold ADRV904X device capability settings
*/
typedef struct adi_adrv904x_CapabilityModel
{
    uint8_t  productId;                  /*!< device Product Id */
    uint8_t  siRevision;                 /*!< device silicon version */
    uint8_t  txNumber;                   /*!< device number of Tx */
    uint8_t  rxNumber;                   /*!< device number of Rx */
    uint8_t  orxNumber;                  /*!< device number of Orx */
    uint32_t txChannelMask;              /*!< device ChannelMask of Tx */
    uint32_t rxChannelMask;              /*!< device ChannelMask of Rx */
    uint32_t orxChannelMask;             /*!< device ChannelMask of Orx */
    uint32_t featureMask;                /*!< device featureMask*/
    uint32_t txMaxBw_kHz;                /*!< device max BW for Tx */
    uint32_t rxMaxBw_kHz;                /*!< device max BW for Rx */
    uint32_t orxMaxBw_kHz;               /*!< device max BW for Orx */
    uint32_t rfFreqRangeMin_kHz;         /*!< device RF Freq Range Min */
    uint32_t rfFreqRangeMax_kHz;         /*!< device RF Freq Range Max */
    uint32_t loMask;                     /*!< Mask of Hardware Enabled LOs and the bit0 to bit2 correspond to LO1, LO2 and ext LO*/
    uint32_t rfMaxIbw_Mhz;               /*!< device Instantaneous Bandwidth Max */
    uint32_t serializerLaneEnableMask;   /*!< Mask of Hardware Enabled Serializer Lanes */
    uint32_t deserializerLaneEnableMask; /*!< Mask of Hardware Enabled Deserializer Lanes */
} adi_adrv904x_CapabilityModel_t;


#ifndef CLIENT_IGNORE
                  
/**
* \brief Data Structure to hold ADRV904X device Rx Max and Min gain indices
*/
typedef struct adi_adrv904x_GainIndex
{
    uint8_t rx0MinGainIndex;           /*!< Current device minimum Rx0 gain index */
    uint8_t rx0MaxGainIndex;           /*!< Current device maximum Rx0 gain index */
    uint8_t rx1MinGainIndex;           /*!< Current device minimum Rx1 gain index */
    uint8_t rx1MaxGainIndex;           /*!< Current device maximum Rx1 gain index */
    uint8_t rx2MinGainIndex;           /*!< Current device minimum Rx2 gain index */
    uint8_t rx2MaxGainIndex;           /*!< Current device maximum Rx2 gain index */
    uint8_t rx3MinGainIndex;           /*!< Current device minimum Rx3 gain index */
    uint8_t rx3MaxGainIndex;           /*!< Current device maximum Rx3 gain index */
    uint8_t rx4MinGainIndex;           /*!< Current device minimum Rx4 gain index */
    uint8_t rx4MaxGainIndex;           /*!< Current device maximum Rx4 gain index */
    uint8_t rx5MinGainIndex;           /*!< Current device minimum Rx5 gain index */
    uint8_t rx5MaxGainIndex;           /*!< Current device maximum Rx5 gain index */
    uint8_t rx6MinGainIndex;           /*!< Current device minimum Rx6 gain index */
    uint8_t rx6MaxGainIndex;           /*!< Current device maximum Rx6 gain index */
    uint8_t rx7MinGainIndex;           /*!< Current device minimum Rx7 gain index */
    uint8_t rx7MaxGainIndex;           /*!< Current device maximum Rx7 gain index */
} adi_adrv904x_GainIndex_t;

#endif // !CLIENT_IGNORE

/**
 * \brief Struct for ADRV904X SPI Configuration Settings
 */
typedef struct adi_adrv904x_SpiConfigSettings
{
    uint8_t                         msbFirst;           /*!< 1 = MSB First, 0 = LSB First Bit order for SPI transaction */
    uint8_t                         fourWireMode;       /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
    adi_adrv904x_CmosPadDrvStr_e    cmosPadDrvStrength; /*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
} adi_adrv904x_SpiConfigSettings_t;

/**
 *  \brief Struct for ADRV904X Aux SPI Interface Configuration.
 */
typedef struct adi_adrv904x_AuxSpiConfig
{
    uint8_t                     enable;     /*!< 1 = ENABLE, 0 = DISABLE */
    adi_adrv904x_GpioPinSel_e   gpioCsb;    /*!< GPIO[3] or GPIO[16] */
    adi_adrv904x_GpioPinSel_e   gpioClk;    /*!< GPIO[2] or GPIO[15] */
    adi_adrv904x_GpioPinSel_e   gpioSdio;   /*!< GPIO[0] or GPIO[13] */
    adi_adrv904x_GpioPinSel_e   gpioSdo;    /*!< GPIO[1] or GPIO[14] or INVALID */
} adi_adrv904x_AuxSpiConfig_t;

/**
 * \brief Struct for ADRV904X SPI Configuration Settings
 */
typedef struct adi_adrv904x_SpiOptions
{
    uint8_t allowSpiStreaming;      /*!< 0 = SPI Streaming not allowed. 1 = Allowed */
    uint8_t allowAhbAutoIncrement; /*!< 0 = AHB Auto Incr not allowed. 1 = Allowed */
    uint8_t allowAhbSpiFifoMode;    /*!< 0 = AHB SPI FIFO use not allowed. 1 = Allowed 
                                         Note: AHB SPI FIFO value ignored unless SPI Streaming = 1
                                         and AHB Auto Incr */
} adi_adrv904x_SpiOptions_t;

/**
 *  \brief Data structure to hold uart settings
 */
typedef struct adi_adrv904x_UartSettings
{
    uint8_t                     enable;    /*!< Enable(1)/Disable(0) Uart GPIO */
    adi_adrv904x_GpioPinSel_e   pinSelect; /*!< Holds Uart Gpio Pin Sel. Valid Range (ADI_ADRV904X_GPIO_09|10|11|12) */
} adi_adrv904x_UartSettings_t;

/**
 *  \brief Data structure to hold DFE UART settings
 */
typedef struct adi_adrv904x_DfeUartSettings
{
    uint8_t     enableGpioTx;       /*!< Enable(1)/Disable(0) GPIO pin for DFE UART TX. DFE UART0 Pin ADI_ADRV904X_GPIO_08. */
    uint8_t     enableGpioRx;       /*!< Enable(1)/Disable(0) GPIO pin for DFE UART RX. DFE UART0 Pin ADI_ADRV904X_GPIO_17. */
    uint8_t     enableGpioCts;      /*!< Enable(1)/Disable(0) GPIO pin for DFE UART CTS. DFE UART0 Pin ADI_ADRV904X_GPIO_21. */
    uint8_t     enableGpioRts;      /*!< Enable(1)/Disable(0) GPIO pin for DFE UART RTS. DFE UART0 Pin ADI_ADRV904X_GPIO_22. */
} adi_adrv904x_DfeUartSettings_t;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv904x_ClockSettings
{
    uint8_t                     DevClkOnChipTermResEn; /*!< Enables the 100ohm on chip termination resistor 0: Disabled, 1: Enable */
} adi_adrv904x_ClockSettings_t;

/**
 * \brief Data structure to hold ADRV904X device instance initialization settings
 */
typedef struct adi_adrv904x_Init
{
    adi_adrv904x_SpiOptions_t           spiOptionsInit;                 /*!< Holds the initial SPI runtime settings */
    adi_adrv904x_ClockSettings_t        clocks;                         /*!< Holds settings for CLKPLL and reference clock */            
    adi_adrv904x_CpuMemDumpBinaryInfo_t cpuMemDump;                     /*!< CPU Memory Dump File Settings */
    adi_adrv904x_RxSettings_t           rx;                             /*!< Rx settings data structure */
    adi_adrv904x_TxSettings_t           tx;                             /*!< Tx settings data structure */
    adi_adrv904x_UartSettings_t         uart[ADI_ADRV904X_MAX_UART];    /*!< Uart settings data structure */
    adi_adrv904x_GpIntPinMaskCfg_t      gpIntPreInit;                   /*!< GP Interrupt Pin Mask Config data structure to be applied before Device Initialization */
    adi_adrv904x_RadioCtrlPreInit_t     radioCtrlPreInit;               /*!< Radio Control Config data structure to be applied before Device Initialzation */
    adi_adrv904x_DfeUartSettings_t      dfeUart0;                       /*!< DFE UART0 settings data structure */
} adi_adrv904x_Init_t;


#ifndef CLIENT_IGNORE
/**
* \brief Data structure to hold clock divide ratios
*/
typedef struct adi_adrv904x_ClkDivideRatios
{
    uint8_t armClkDivideRatio;  /*!< ARM clock divide ratio w.r.t hsDigClk */
    uint8_t agcClkDivideRatio;  /*!< AGC module clock divide ratio w.r.t hsDigClk */
    uint8_t regClkDivideRatio;  /*!< Register bus clock divide ratio w.r.t hsDigClk */
    uint8_t txAttenDeviceClockDivideRatio; /*!< Tx Atten module clock divide ratio w.r.t hsDigClk */
} adi_adrv904x_ClkDivideRatios_t;

typedef struct adi_adrv904x_SharedResourcePool
{
    uint32_t featureID;     /*!< Holds the feature ID of a shared resource pool member */
    uint8_t semaphoreCount; /*!< Holds the semaphore count of a shared resource pool member */
    uint32_t channelMask;   /*!< Holds the channel Mask of a shared resource pool member */
} adi_adrv904x_SharedResourcePool_t;

/**
 * \brief Data structure to hold a ADRV904X device instance status information
 *
 * Application layer allocates this memory, but only API writes to it to keep up with the API run time state.
 */
typedef struct adi_adrv904x_Info
{
    uint8_t                             deviceSiRev;                                    /*!< ADRV904X silicon rev read during HWOpen */
    uint8_t                             deviceProductId;                                /*!< ADRV904X Product ID read during HWOpen */
    adi_adrv904x_ApiStates_e            devState;                                       /*!< Current device state of the part, i.e., radio on, radio off, arm loaded, etc., defined by deviceState enum */
    uint32_t                            initializedChannels;                            /*!< Indicates initialized & calibrated Rx/ORx/Tx chans. RxN is at bit N. TxN is at bit N + ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET */
    uint32_t                            profilesValid;                                  /*!< Current device profilesValid bit field for use notification, i.e., Tx = 0x01, Rx = 0x02, Orx = 0x04 */
    adi_adrv904x_TxAttenStepSize_e      txAttenStepSize[ADI_ADRV904X_MAX_TXCHANNELS];   /*!< Current tx Atten step size for each Tx channel */
    uint64_t                            swTest;                                         /*!< Software Test Signal */
    uint32_t                            hsDigClk_kHz;                                   /*!< Calculated in initialize() digital clock used throughout API functions */
    adi_adrv904x_ClkDivideRatios_t      clkDivideRatios;                                /*!< Clock divide ratios w.r.t hsDigClk for various modules in the device */
    uint32_t                            profileAddr;                                    /*!< Address to load Profile */
    uint32_t                            adcProfileAddr;                                 /*!< Address to load ADC Profile */
    
    adi_adrv904x_GainIndex_t            gainIndexes;                                    /*!< Current device Rx min max gain index values */
    uint32_t                            chunkStreamImageSize[ADI_ADRV904X_STREAM_MAX];  /*!< Stream Image Size */
    uint32_t                            chunkStreamImageOffset[ADI_ADRV904X_STREAM_MAX];/*!< Stream Image Offset */
    uint32_t                            currentStreamBinBaseAddr;                       /*!< Address to load current stream */
    uint32_t                            currentStreamBaseAddr;                          /*!< Address to load current stream base */
    uint8_t                             currentStreamNumberStreams;                     /*!< Number of Streams for current stream  */
    uint8_t                             currentStreamImageIndex;                        /*!< Index of current stream  */
    uint32_t                            currentStreamImageSize;                         /*!< Image size of current stream */   
    uint8_t                             rxFramerNp;                                     /*!< Rx Framer Np - converter sample resolution (12, 16, 24) */
    uint8_t                             rxChannel3bitSlicerMode;                        /*!< If a bit is set - corresponding Rx Channel Data format slicer ctrl is 3 bits */
    adi_adrv904x_SharedResourcePool_t   sharedResourcePool[ADI_ADRV904X_NUM_SHARED_RESOURCES]; /*!< Shared resource pool for the given device instance*/
    uint8_t                             linkSharingEnabled;                             /*!< Link sharing enabled if 1, disabled otherwise */
    adi_adrv904x_Cpu_t                  cpu;                                            /*!< Processor related info */
    adi_adrv904x_SpiOptions_t           spiOptions;                                     /*!< Holds the SPI runtime options used during API function calls */
    uint8_t                             spiFifoModeOn;                                  /*!< Internal use only: Keeps track of current SPI FIFO mode status. */
    uint8_t                             autoIncrModeOn;                                 /*!< Internal use only: Keeps track of current auto-increment mode status. */
    uint8_t                             spiStreamingOn;                                 /*!< Internal use only: Keeps track of current SPI Streaming status. */
    uint32_t                            currentPageStartingAddress;                     /*!< Internal use only: Starting address of the current 4K page */
    uint32_t                            previousSpiStreamingAddress;                    /*!< Internal use only: Only valid when spiStreamingOn is asserted. Stores the previous address written to determine if a CS toggle is required */
    adi_adrv904x_TxToOrxMappingConfig_t txToOrxMappingConfig;                           /*!< Internal use only: Tx to Orx Mapping Configuration */
    adi_adrv904x_StreamGpioPinCfg_t     streamGpioMapping;                              /*!< Internal use only: Stream GPIO mapping */

    adi_adrv904x_ProtectionRampCfgRt_t  txProtectionRampCfg[ADI_ADRV904X_MAX_TXCHANNELS];        /*!< Internal use only: PA protection ramp down */
    adi_adrv904x_RadioSeqImageHeader_t  rsImageHeader;                                           /*!< Radio Sequence Image Header */
    uint8_t                             xbarSettings[ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE];          /*!< cross bar settings */
    uint32_t                            lookupTable[ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE]; /*!< Sequence lookup table */
    uint32_t                            lookupCrcRun;                                            /*!< Lookup table CRC run */
    uint32_t                            sequenceCrcRun;                                          /*!< Sequence Image CRC run */
    uint32_t                            fileImageCrcRun;                                         /*!< CRC of entire file image run */
    adi_adrv904x_CfrThrConfig_t         cfrThresholdsCfg0[ADI_ADRV904X_MAX_TXCHANNELS];          /*!< CFR threshold values for Cfg0 set */
    adi_adrv904x_CfrThrConfig_t         cfrThresholdsCfg1[ADI_ADRV904X_MAX_TXCHANNELS];          /*!< CFR threshold values for Cfg1 set */
    uint8_t                             dfeSwTest;                                               /*!< DFE Software test signal */
    uint8_t                             rsStarted;                                               /*!< Radio Sequencer started */
    uint8_t                             rsRadioRunning;                                          /*!< Radio Sequencer Radio Running */
    adi_adrv904x_RsSpecialSequencer_t   specialSequencerPatternInfo;                             /*!< Customer special frame timing pattern config for each radio sequencer */
    adi_adrv904x_RadioCtrlAlarmGpioConfig_t alarmGpioConfig;                                     /*!< GPIO pin configuration used by alarms */
    adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t dfeCtrlTxToOrxMappingConfig;                      /*!< Internal use only: DFE Control Mode Tx to Orx Mapping Configuration */
    adi_adrv904x_DtxInputPinCfg_t       dtxInputMapping;                                         /*!< Internal use only: DTX input mapping */
    adi_adrv904x_ModelSwitchInputPinCfg_t modelSwitchInputMapping;                               /*!< Internal use only: Model Switch input mapping */
    adi_adrv904x_GpioPinSel_e           digGpioTddSw;                                            /*!< Digital GPIO TDD SW */
    adi_adrv904x_GpioAnaPinSel_e        anaGpioTddSw;                                            /*!< Analog GPIO TDD SW */
    adi_adrv904x_GpioPinSel_e           digGpioPredrive;                                         /*!< Digital GPIO Predrive */
    adi_adrv904x_GpioAnaPinSel_e        anaGpioPredrive;                                         /*!< Analog GPIO Predrive */
    adi_adrv904x_StreamGpioAntCalOutCfg_t       streamGpioAntCalOutCfg;                             /*!< Stream GPIO Antenna Cal Output Pin Assignments */
    adi_adrv904x_CarrierReconfigLatencyCfg_t    txCarrierLatencyCfg[ADI_ADRV904X_MAX_TXCHANNELS];   /*!< Internal use only: Tx carrier reconfig latency settings */
    adi_adrv904x_CarrierReconfigLatencyCfg_t    rxCarrierLatencyCfg[ADI_ADRV904X_MAX_RX_ONLY];      /*!< Internal use only: Rx carrier reconfig latency settings */

    uint8_t vswrWaveformLoaded[ADI_ADRV904X_MAX_TXCHANNELS];                                        /*!< Internal use only: Shows if VSWR waveform is loaded by user */
} adi_adrv904x_Info_t;

/**
 *  \brief Data structure to hold band data
 */
typedef struct adi_adrv904x_BandCfgExtract
{
    uint8_t enabled;                /*!< Band enabled flag */
    uint32_t rfCenterFreq_kHz;      /*!< RF center frequency */
    uint32_t instBw_kHz;            /*!< Instantaneous signal bandwidth */
    uint32_t sampleRate_kHz;        /*!< Band sample rate */
    uint32_t bandOffset_kHz;        /*!< LO and NCO offset */
} adi_adrv904x_BandCfgExtract_t;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv904x_TxChannelCfgExtract
{
    uint32_t                        rfBandwidth_kHz;                            /*!< Tx RF passband bandwidth for the profile */
    uint32_t                        totalDecimation;                            /*!< Total DDC decimation  */
    uint32_t                        digChanMask;                                /*!< Tx digital channel mask indicates which DUCs to initialize */
    uint32_t                        txLbAdcSampleRate_kHz;                      /*!< TX LB ADC Sample rate */
    adi_adrv904x_BandCfgExtract_t   bandSettings[ADI_ADRV904X_DUC_NUM_BAND];    /*!< Tx DUC settings */
    
    uint32_t                            pfirRate_kHz;                           /*!< Tx PFIR rate */
    adi_adrv904x_CarrierRuntime_t       carrierRuntimeSettings;                 /*!< Tx carrier runtime settings */
    uint8_t maxSlot;                                                            /*!< Holds the max slot value for this channel's config */
    
} adi_adrv904x_TxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ADRV904X Tx Channel configuration settings
 */
typedef struct adi_adrv904x_TxSettingsExtract
{
    uint32_t                               txInitChannelMask;                           /*!< Tx channel mask of which channels to initialize */
    adi_adrv904x_TxChannelCfgExtract_t     txChannelCfg[ADI_ADRV904X_MAX_TXCHANNELS];   /*!< Tx settings per Tx channel */
} adi_adrv904x_TxSettingsExtract_t;

/**
 *  \brief Data structure to hold Rx data path settings
 */
typedef struct adi_adrv904x_RxChannelCfgExtract
{
    uint32_t                            rfBandwidth_kHz;                                    /*!< Rx RF passband bandwidth for the profile */
    uint32_t                            rxDdc0OutputRate_kHz;                               /*!< Rx output sample rate per Rx channel Band 0 */
    uint32_t                            rxDdc1OutputRate_kHz;                               /*!< Rx output sample rate per Rx channel Band 1 */
    uint32_t                            digChanMask;                                        /*!< Rx digital channel mask indicates which DDCs to initialize */
    uint32_t                            rxAdcSampleRate_kHz;                                /*!< Rx ADC sample rate */
    adi_adrv904x_BandCfgExtract_t       bandSettings[ADI_ADRV904X_DDC_NUM_BAND];            /*!< Rx DDC settings for this channel's profile */
    
    adi_adrv904x_CarrierRuntime_t       carrierRuntimeSettings;                           /*!< Rx carrier runtime settings */
    uint8_t                             bandRatio[ADI_ADRV904X_DDC_NUM_BAND];               /*!< Cddc to band ratio */
    uint8_t maxSlot;                                                                        /*!< Holds the max slot value for this channel's config */
} adi_adrv904x_RxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ORx data path settings
 */
typedef struct adi_adrv904x_OrxChannelCfgExtract
{
    uint32_t                            rfBandwidth_kHz;                     /*!< Rx RF passband bandwidth for the profile */
    uint32_t                            orxOutputRate_kHz;                   /*!< ORx output sample rate per ORx channel */
    uint32_t                            orxAdcSampleRate_kHz;                /*!< ORx ADC sample rate*/
} adi_adrv904x_OrxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ADRV904X Rx Channel configuration settings
 */
typedef struct adi_adrv904x_RxSettingsExtract
{
    uint32_t                            rxInitChannelMask;                              /*!< Rx channel mask of which channels to initialize */
    adi_adrv904x_RxChannelCfgExtract_t  rxChannelCfg[ADI_ADRV904X_MAX_RX_ONLY];         /*!< Rx settings per Rx channel */
} adi_adrv904x_RxSettingsExtract_t;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv904x_ClockSettingsExtract
{
    uint32_t                    deviceClockScaled_kHz; /*!< Scaled device clock frequency in kHz */
    uint8_t                     padDivideRatio;        /*!< Pad divider ratio calculated by configurator tool */
    uint8_t                     armClkDivideRatio;     /*!< FW Clock Divide ratio calculated by Configurator tool */
    uint8_t                     armClkDevClkDivRatio;  /*!< FW Clock Divide ratio calculated by Configurator tool */
    uint32_t                    hsDigClk_kHz;          /*!< Digital clock */
} adi_adrv904x_ClockSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer settings
 */
typedef struct adi_adrv904x_FramerSettingsExtract
{
    uint8_t   jesdM;                  /*!< M value */
    uint8_t   jesdNp;                 /*!< Np value */
    uint8_t   serialLaneEnabled;      /*!< lane enabled*/
    uint32_t  iqRate_kHz;             /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz;           /*!< Framer Lane rate */
    uint8_t   linkLsSampleXBar [ADI_ADRV904X_NUM_LKSH_SAMPLE_XBAR];
} adi_adrv904x_FramerSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer settings
 */
typedef struct adi_adrv904x_DeframerSettingsExtract
{
    uint8_t   jesdM;                  /*!< M value */
    uint8_t   jesdNp;                 /*!< Np value */
    uint8_t   deserialLaneEnabled;    /*!< lane enabled */
    uint32_t  iqRate_kHz;             /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz;           /*!< Framer Lane rate */
} adi_adrv904x_DeframerSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer and Deframer settings
 */
typedef struct adi_adrv904x_JesdSettingsExtract
{
    adi_adrv904x_FramerSettingsExtract_t   framerSetting[ADI_ADRV904X_MAX_FRAMERS];     /*!< Framer settings */
    adi_adrv904x_DeframerSettingsExtract_t deframerSetting[ADI_ADRV904X_MAX_DEFRAMERS]; /*!< Deframer setting */
    adi_adrv904x_FramerSettingsExtract_t   framerLsSetting[ADI_ADRV904X_MAX_FRAMERS_LS]; /*!< Framer Link Sharing settings */
    uint8_t                                rxdesQhfrate;   /*!< SERDES Rate mode */
} adi_adrv904x_JesdSettingsExtract_t;

/*
 *  \brief Data structure to hold ORx channel configuration settings
 */
typedef struct adi_adrv904x_OrxSettingsExtract
{
    adi_adrv904x_OrxChannelCfgExtract_t orxChannelCfg[ADI_ADRV904X_MAX_ORX]; /*!< Orx output sample rate */
} adi_adrv904x_OrxSettingsExtract_t;

typedef struct adi_adrv904x_DfeRadioSeqConfigExtract
{
    uint8_t  radioSeqEn;    /*!< Radio sequencer top level enable */
    uint8_t  sampleClkEn;   /*!< Radio sequencer sampling clock enable */
    uint8_t  sampleClkDiv;  /*!< Radio sequencer sampling clock. Power of 2 encoding */
    uint8_t  ssbSyncMode;   /*!< Radio sequencer SSB frame sync mode */
    uint32_t ssbSyncPeriod; /*!< Radio sequencer SSB sync duration */
    uint16_t symWidth;      /*!< Radio sequencer nominal symbol duration */
    uint16_t extSymWidth;   /*!< Radio sequencer extended symbol duration */
    uint16_t extSymSpacing; /*!< Radio sequencer spacing between consecutive extended symbols */
} adi_adrv904x_DfeRadioSeqConfigExtract_t;

typedef struct adi_adrv904x_DfeCducConfigExtract
{
    uint32_t cducOutputRate_kHz[ADI_ADRV904X_MAX_CHANNELS];  /*!< CDUC output rate - one for each Tx channel */
    uint32_t dpdOutputRate_kHz[ADI_ADRV904X_MAX_CHANNELS];   /*!< DPD  output rate */
} adi_adrv904x_DfeCducConfigExtract_t;

/**
 * \brief Data structure to hold ADRV904X device instance initialization settings
 */
typedef struct adi_adrv904x_InitExtract
{
   
    adi_adrv904x_ClockSettingsExtract_t     clocks;                                                             /*!< Holds settings for CLKPLL and reference clock */
    adi_adrv904x_RxSettingsExtract_t        rx;                                                                 /*!< Rx settings data structure */
    adi_adrv904x_TxSettingsExtract_t        tx;                                                                 /*!< Tx settings data structure */
    adi_adrv904x_OrxSettingsExtract_t       orx;                                                                /*!< ORx settings data structure */
    adi_adrv904x_JesdSettingsExtract_t      jesdSetting;                                                        /*!< Jesd settings *//*!< ORx settings data structure */
    uint8_t                                 rxTxCpuConfig[ADI_ADRV904X_MAX_CHANNELS];                           /*!< Defines which CPU each channel is assigned to (Rx/Tx) */
    uint8_t                                 orxCpuConfig[ADI_ADRV904X_MAX_ORX];                                 /*!< Defines which CPU each channel is assigned to (ORx)  */
    uint8_t                                 jesd204DesLaneCpuConfig[ADI_ADRV904X_MAX_SERDES_LANES];             /*!< Defines which CPU each deserializer lane calibration is assigned to */
    uint8_t                                 chanAssign[ADI_ADRV904X_MAX_CHANNELS];                              /*!< This is used to reference a channel to a rx/tx profile def'n in rxConfig/txConfig */
    uint8_t                                 featureMask[ADI_ADRV904X_MAX_FEATUREMASK];                          /*!< Device Extract information 128 bits of Feature Mask */
    adi_adrv904x_DfeRadioSeqConfigExtract_t     rs;                                                             /*!< Radio Sequencer configuration */
    adi_adrv904x_RadioCtrlPreInit_t             radioCtrlPreInit;                                               /*!< Radio Ctrl Pre Init configuration */
    adi_adrv904x_DfeCducConfigExtract_t         dfeCduc;                                                        /*!< Misc DFE parameters needed by the API */
    adi_adrv904x_CarrierRadioCfg_t              rxCarrierConfigs[ADI_ADRV904X_MAX_CHANNELS];                    /*!< Rx carrier config settings for each channel */
    adi_adrv904x_CarrierRadioCfg_t              txCarrierConfigs[ADI_ADRV904X_MAX_CHANNELS];                    /*!< Tx carrier config settings for each channel */
    adi_adrv904x_CarrierJesdCfg_t               txCarrierJesdCfg;                                               /*!< Holds Deframer Carrier XBar values */
    adi_adrv904x_CarrierJesdCfg_t               rxCarrierJesdCfg;                                               /*!< Holds Framer Carrier XBar values */
    uint8_t                                     framerInterleaveMode[ADI_ADRV904X_MAX_FRAMERS];                 /*!< Framer settings */
    uint8_t                                     deframerInterleaveMode[ADI_ADRV904X_MAX_DEFRAMERS];             /*!< Deframer setting */
} adi_adrv904x_InitExtract_t;


/**
 * \brief Data structure to hold ADRV904X device instance settings
 */
typedef struct adi_adrv904x_Device
{
    adi_common_Device_t                 common;         /*!< Common layer structure */
    adi_adrv904x_Info_t                 devStateInfo;   /*!< ADRV904X run time state information container */
    adi_adrv904x_SpiConfigSettings_t    spiSettings;    /*!< Pointer to ADRV904X SPI Settings */
    adi_adrv904x_InitExtract_t          initExtract;    /*!< ADRV904X init info extract from CPU Profile container */
} adi_adrv904x_Device_t;
#endif /* CLIENT_IGNORE */

#endif
