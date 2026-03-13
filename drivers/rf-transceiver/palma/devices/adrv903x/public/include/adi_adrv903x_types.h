/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/*!
* \file adi_adrv903x_types.h
* \brief Contains ADRV903X API configuration and run-time type definitions
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_TYPES_H_
#define _ADI_ADRV903X_TYPES_H_

#include "adi_common.h"
#include "adi_adrv903x_user.h"
#include "adi_adrv903x_cpu_types.h"
#include "adi_adrv903x_radioctrl_types.h"
#include "adi_adrv903x_tx_types.h"
#include "adi_adrv903x_rx_types.h"
#include "adi_adrv903x_gpio_types.h"
#include "adi_adrv903x_datainterface_types.h"

#define ADI_ADRV903X_TX_PROFILE_VALID    0x01
#define ADI_ADRV903X_RX_PROFILE_VALID    0x02
#define ADI_ADRV903X_ORX_PROFILE_VALID   0x04
#define ADI_ADRV903X_NUM_SHARED_RESOURCES 40U 

#define ADRV903X_ADDR_SPI_INTERFACE_CONFIG_A     0x0000
#define ADRV903X_CONFIG_A_SPI_LSB_FIRST          0x42
#define ADRV903X_CONFIG_A_SPI_ADDR_ASCENSION     0x24
#define ADRV903X_CONFIG_A_SPI_SDO_ACTIVE         0x18
#define ADRV903X_ADDR_DIGITAL_IO_CONTROL         0x0010
#define ADRV903X_IO_CONTROL_SPI2_OUTS_DRV_SEL    0x02
#define ADRV903X_IO_CONTROL_SPI_OUTS_DRV_SEL     0x01
#define ADRV903X_BF_CORE_ADDR                    0U

#define ADRV903X_ADDR_SPI_INTERFACE_CONFIG_B     0x0001
#define ADRV903X_CONFIG_B_SINGLE_INSTRUCTION     0x80

#define ADI_ADRV903X_MAX_UART    4U

#define ADI_ADRV903X_MAX_RXCHANNELS     10U
#define ADI_ADRV903X_MAX_RX_ONLY        8U
#define ADI_ADRV903X_MAX_ORX            2U

#define ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET 12

#define ADI_ADRV903X_MAX_SERDES_LANES 8U  /* Number of lanes in serializer and deserializer */

#define ADI_ADRV903X_MAX_CHANNELS 8

#define ADI_ADRV903X_MAX_FEATUREMASK 16

/**
 *  \brief Enum of possible channel enables
 */
typedef enum adi_adrv903x_Channels
{
    ADI_ADRV903X_CHOFF = 0x00, /*!< No channels are enabled */
    ADI_ADRV903X_CH0 = 0x01, /*!< Channel 0 enabled */
    ADI_ADRV903X_CH1 = 0x02, /*!< Channel 1 enabled */
    ADI_ADRV903X_CH2 = 0x04, /*!< Channel 2 enabled */
    ADI_ADRV903X_CH3 = 0x08, /*!< Channel 3 enabled */
    ADI_ADRV903X_CH4 = 0x10, /*!< Channel 4 enabled */
    ADI_ADRV903X_CH5 = 0x20, /*!< Channel 5 enabled */
    ADI_ADRV903X_CH6 = 0x40, /*!< Channel 6 enabled */
    ADI_ADRV903X_CH7 = 0x80 /*!< Channel 7 enabled */
} adi_adrv903x_Channels_e;

/**
 *  \brief Data structure to hold ADRV903X API State
 */
typedef enum adi_adrv903x_ApiStates
{
    ADI_ADRV903X_STATE_POWERONRESET         = 0x001U,     /*!< POR Complete */
    ADI_ADRV903X_STATE_INITIALIZED          = 0x002U,     /*!< Initialized */
    ADI_ADRV903X_STATE_STREAMLOADED         = 0x004U,     /*!< Stream Processor Loaded */
    ADI_ADRV903X_STATE_CPUDEBUGLOADED       = 0x008U,     /*!< CPU Debug SW Loaded */
    ADI_ADRV903X_STATE_CPU0LOADED           = 0x010U,     /*!< CPU 0 Loaded */
    ADI_ADRV903X_STATE_CPU1LOADED           = 0x020U,     /*!< CPU 1 Loaded */
    ADI_ADRV903X_STATE_ALLCPUSLOADED        = 0x040U,     /*!< All CPU's Loaded */
    ADI_ADRV903X_STATE_INITCALS_RUN         = 0x080U,     /*!< Init Cals Running */
    ADI_ADRV903X_STATE_RADIOON              = 0x100U,     /*!< Device Running */
        ADI_ADRV903X_STATE_STANDBY              = 0x800U,     /*!< Part is Powered down in Standby mode */
    ADI_ADRV903X_STATE_JRXREPAIRED          = 0x1000U,    /*!< Jrx repair has been applied */
    ADI_ADRV903X_STATE_MCSCARRIERRECONFIG   = 0x2000U,    /*!< Mcs carrier reconfig has been applied */
} adi_adrv903x_ApiStates_e;

/**
 * \brief Enumerated list of CMOS pads drive strength options for SPI_DO signal.
 * 
 * Note: These are the values used in the device's SpiCfgSettings, resource files and as arguments to
 * SpiDoutPadDriveStrengthSet, and are chosen to maintain binary compatability with API versions before
 * _STRENGTH_1 and _2 were added. The corresponding bitfield values written to the device differ.
 */
typedef enum adi_adrv903x_CmosPadDrvStr
{
    ADI_ADRV903X_CMOSPAD_DRV_WEAK       = 0U,       /*!<  Minimum pad drive strength */
    ADI_ADRV903X_CMOSPAD_DRV_STRENGTH_1 = 2U,       /*!<  Is stronger than WEAK */
    ADI_ADRV903X_CMOSPAD_DRV_STRENGTH_2 = 3U,       /*!<  Is stronger than STRENGTH_1 */
    ADI_ADRV903X_CMOSPAD_DRV_STRONG     = 1U,       /*!<  Maximum pad drive strength */
} adi_adrv903x_CmosPadDrvStr_e;

/**
* \brief Enumerated list of options to use to set the LO source for the Rx/Tx and ORx mixers.
*/
typedef enum adi_adrv903x_LoSel
{
    ADI_ADRV903X_LOSEL_LO0 = 0,    /*!< LO0 selection */
    ADI_ADRV903X_LOSEL_LO1 = 1,    /*!< LO1 selection */
    } adi_adrv903x_LoSel_e;

/**
*  \brief Enum of possible Capability Feature enables
*/
typedef enum adi_adrv903x_Feature
{
    ADI_ADRV903X_FEATUREOFF         = 0x0000,           /*!< No Capability Feature enabled */
    ADI_ADRV903X_FEATURE_RESERVED1  = 0x0001,           /*!< Reserve1 feature enabled */
    ADI_ADRV903X_FEATURE_SPI2       = 0x0002,           /*!< SPI2 feature enabled */
    ADI_ADRV903X_FEATURE_TXSRD      = 0x0004,           /*!< Tx Slew Rate Detector feature enabled */
    ADI_ADRV903X_FEATURE_RESERVED2  = 0x0008            /*!< Reserve2 feature enabled jesd compression */
} adi_adrv903x_Feature_e;

/**
 *  \brief Enum of Device Capability Features case index
 */
typedef enum adi_adrv903x_FeatureCaseIndex
{
    ADI_ADRV903X_FEATURE_SERIALIZER    = 0x00,         /*!< serializer feature case */
    ADI_ADRV903X_FEATURE_DESERIALIZER  = 0x01,         /*!< deserializer feature case */
    ADI_ADRV903X_FEATURE_TX            = 0x02,         /*!< txchannel feature case */
    ADI_ADRV903X_FEATURE_RX            = 0x03,         /*!< rxchannel feature case */
    ADI_ADRV903X_FEATURE_ORX_LOMASK    = 0x04          /*!< orxchannel and lomask features case */
} adi_adrv903x_FeatureCaseIndex_e;

/**
 * \brief MCS Sync Mode
 */
typedef enum adi_adrv903x_McsSyncMode
{
    ADI_ADRV903X_MCS_OFF               = 0U,            /*!< Notification that MCS is off */
    ADI_ADRV903X_MCS_START             = 1U << 0,       /*!< Notification that MCS is started */
} adi_adrv903x_McsSyncMode_e;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv903x_TxChannelCfg
{
    adi_adrv903x_TxAttenCfg_t           txAttenCfg;          /*!< Tx attenuation config */
    uint16_t                            txAttenInit_mdB;     /*!< Tx attenuation init time value */
    adi_adrv903x_PowerMonitorCfg_t      txpowerMonitorCfg;   /*!< Tx power monitor configuration */
    adi_adrv903x_SlewRateDetectorCfg_t  srlCfg;              /*!< Slew rate detector config */
    adi_adrv903x_ProtectionRampCfg_t    protectionRampCfg;   /*!< Tx Pa protection ramp config */
} adi_adrv903x_TxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV903X Tx Channel configuration settings
 */
typedef struct adi_adrv903x_TxSettings
{
    adi_adrv903x_TxChannelCfg_t     txChannelCfg[ADI_ADRV903X_MAX_TXCHANNELS];  /*!< Tx settings per Tx channel */
} adi_adrv903x_TxSettings_t;


/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv903x_RxChannelCfg
{
    adi_adrv903x_RxDataFormat_t         rxDataFormat;     /*!< Rx Data Format settings structure */
    uint8_t                             rxGainIndexInit;  /*!< Init Gain Index value for the channels selected */
} adi_adrv903x_RxChannelCfg_t;

/**
 *  \brief Data structure to hold ADRV903X Rx Channel configuration settings
 */
typedef struct adi_adrv903x_RxSettings
{
    adi_adrv903x_RxChannelCfg_t rxChannelCfg[ADI_ADRV903X_MAX_RXCHANNELS]; /*!< Rx settings per Rx channel */
} adi_adrv903x_RxSettings_t;

/*
* \brief Data structure to hold ADRV903X device capability settings
*/
typedef struct adi_adrv903x_CapabilityModel
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
} adi_adrv903x_CapabilityModel_t;


#ifndef CLIENT_IGNORE
                  
/**
* \brief Data Structure to hold ADRV903X device Rx Max and Min gain indices
*/
typedef struct adi_adrv903x_GainIndex
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
} adi_adrv903x_GainIndex_t;

#endif // !CLIENT_IGNORE

/**
 * \brief Struct for ADRV903X SPI Configuration Settings
 */
typedef struct adi_adrv903x_SpiConfigSettings
{
    uint8_t                         msbFirst;           /*!< 1 = MSB First, 0 = LSB First Bit order for SPI transaction */
    uint8_t                         fourWireMode;       /*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
    adi_adrv903x_CmosPadDrvStr_e    cmosPadDrvStrength; /*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
} adi_adrv903x_SpiConfigSettings_t;

/**
 *  \brief Struct for ADRV903X Aux SPI Interface Configuration.
 */
typedef struct adi_adrv903x_AuxSpiConfig
{
    uint8_t                     enable;     /*!< 1 = ENABLE, 0 = DISABLE */
    adi_adrv903x_GpioPinSel_e   gpioCsb;    /*!< GPIO[3] or GPIO[16] */
    adi_adrv903x_GpioPinSel_e   gpioClk;    /*!< GPIO[2] or GPIO[15] */
    adi_adrv903x_GpioPinSel_e   gpioSdio;   /*!< GPIO[0] or GPIO[13] */
    adi_adrv903x_GpioPinSel_e   gpioSdo;    /*!< GPIO[1] or GPIO[14] or INVALID */
} adi_adrv903x_AuxSpiConfig_t;

/**
 * \brief Struct for ADRV903X SPI Configuration Settings
 */
typedef struct adi_adrv903x_SpiOptions
{
    uint8_t allowSpiStreaming;      /*!< 0 = SPI Streaming not allowed. 1 = Allowed */
    uint8_t allowAhbAutoIncrement; /*!< 0 = AHB Auto Incr not allowed. 1 = Allowed */
    uint8_t allowAhbSpiFifoMode;    /*!< 0 = AHB SPI FIFO use not allowed. 1 = Allowed 
                                         Note: AHB SPI FIFO value ignored unless SPI Streaming = 1
                                         and AHB Auto Incr */
} adi_adrv903x_SpiOptions_t;

/**
 *  \brief Data structure to hold uart settings
 */
typedef struct adi_adrv903x_UartSettings
{
    uint8_t                     enable;    /*!< Enable(1)/Disable(0) Uart GPIO */
    adi_adrv903x_GpioPinSel_e   pinSelect; /*!< Holds Uart Gpio Pin Sel. Valid Range (ADI_ADRV903X_GPIO_09|10|11|12) */
} adi_adrv903x_UartSettings_t;

/**
* \brief Memdump enum to represent record type 
*/
typedef enum adi_adrv903x_MemdumpRecordType
{
    ADI_ADRV903X_MEMDUMP_FIRMWARE_VER_RECORD    = 1U,               /* Firmware Version Record */
    ADI_ADRV903X_MEMDUMP_TELEM_RECORD           = 2U,               /* Relemetry Record */
    ADI_ADRV903X_MEMDUMP_ETM_TRACE_RECORD       = 3U,               /* ETM/Trace Record */
    ADI_ADRV903X_MEMDUMP_DEV_DRVR_STATE_RECORD  = 4U,               /* Device Driver State Record */
    ADI_ADRV903X_MEMDUMP_CPU_RAM_RECORD         = 5U,               /* CPU RAM Record */
    ADI_ADRV903X_MEMDUMP_REG_RECORD             = 6U,               /* Register Record */
} adi_adrv903x_MemdumpRecordType_e;

/**
* \brief Memdump enum to represent CPU Type 
*/
typedef enum adi_adrv903x_MemdumpCpuType
{
    ADI_ADRV903X_MEMDUMP_STREAM_CPU             = 0x1U,             /* Custom CPU RAM */

    /* All arm CPU's added below */
    ADI_ADRV903X_MEMDUMP_ARM_V7                 = 0x2U,             /* 32 bit arm v7 */
    ADI_ADRV903X_MEMDUMP_ARM_V8                 = 0x3U,             /* aarch64, arm v8 */

    /* All risc-v CPU's added below */
    ADI_ADRV903X_MEMDUMP_RISC_V_1               = 0x8000U,          /* risc-v */
} adi_adrv903x_MemdumpCpuType_e;

/**
* \brief Memdump enum to represent FW Type
*/
typedef enum adi_adrv903x_MemdumpFirmwareType
{
    ADI_ADRV903X_MEMDUMP_STREAM_FW              = 0x1U,             /* Stream FW */
    ADI_ADRV903X_MEMDUMP_RADIO_FW               = 0x2U,             /* Radio FW  */
} adi_adrv903x_MemdumpFirmwareType_e;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv903x_ClockSettings
{
    uint8_t                     DevClkOnChipTermResEn; /*!< Enables the 100ohm on chip termination resistor 0: Disabled, 1: Enable */
} adi_adrv903x_ClockSettings_t;

/**
 * \brief Data structure to hold ADRV903X device instance initialization settings
 */
typedef struct adi_adrv903x_Init
{
    adi_adrv903x_SpiOptions_t           spiOptionsInit;                 /*!< Holds the initial SPI runtime settings */
    adi_adrv903x_ClockSettings_t        clocks;                         /*!< Holds settings for CLKPLL and reference clock */            
    adi_adrv903x_CpuMemDumpBinaryInfo_t cpuMemDump;                     /*!< CPU Memory Dump File Settings */
    adi_adrv903x_RxSettings_t           rx;                             /*!< Rx settings data structure */
    adi_adrv903x_TxSettings_t           tx;                             /*!< Tx settings data structure */
    adi_adrv903x_UartSettings_t         uart[ADI_ADRV903X_MAX_UART];    /*!< Uart settings data structure */
    adi_adrv903x_GpIntPinMaskCfg_t      gpIntPreInit;                   /*!< GP Interrupt Pin Mask Config data structure to be applied before Device Initialization */
} adi_adrv903x_Init_t;


#ifndef CLIENT_IGNORE
/**
* \brief Data structure to hold clock divide ratios
*/
typedef struct adi_adrv903x_ClkDivideRatios
{
    uint8_t armClkDivideRatio;  /*!< ARM clock divide ratio w.r.t hsDigClk */
    uint8_t agcClkDivideRatio;  /*!< AGC module clock divide ratio w.r.t hsDigClk */
    uint8_t regClkDivideRatio;  /*!< Register bus clock divide ratio w.r.t hsDigClk */
    uint8_t txAttenDeviceClockDivideRatio; /*!< Tx Atten module clock divide ratio w.r.t hsDigClk */
} adi_adrv903x_ClkDivideRatios_t;

typedef struct adi_adrv903x_SharedResourcePool
{
    uint32_t featureID;     /*!< Holds the feature ID of a shared resource pool member */
    uint8_t semaphoreCount; /*!< Holds the semaphore count of a shared resource pool member */
    uint32_t channelMask;   /*!< Holds the channel Mask of a shared resource pool member */
} adi_adrv903x_SharedResourcePool_t;

/**
 * \brief Data structure to hold a ADRV903X device instance status information
 *
 * Application layer allocates this memory, but only API writes to it to keep up with the API run time state.
 */
typedef struct adi_adrv903x_Info
{
    uint8_t                             deviceSiRev;                                    /*!< ADRV903X silicon rev read during HWOpen */
    uint8_t                             deviceProductId;                                /*!< ADRV903X Product ID read during HWOpen */
    adi_adrv903x_ApiStates_e            devState;                                       /*!< Current device state of the part, i.e., radio on, radio off, arm loaded, etc., defined by deviceState enum */
    uint32_t                            initializedChannels;                            /*!< Indicates initialized & calibrated Rx/ORx/Tx chans. RxN is at bit N. TxN is at bit N + ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET */
    uint32_t                            profilesValid;                                  /*!< Current device profilesValid bit field for use notification, i.e., Tx = 0x01, Rx = 0x02, Orx = 0x04 */
    adi_adrv903x_TxAttenStepSize_e      txAttenStepSize[ADI_ADRV903X_MAX_TXCHANNELS];   /*!< Current tx Atten step size for each Tx channel */
    uint64_t                            swTest;                                         /*!< Software Test Signal */
    uint32_t                            hsDigClk_kHz;                                   /*!< Calculated in initialize() digital clock used throughout API functions */
    adi_adrv903x_ClkDivideRatios_t      clkDivideRatios;                                /*!< Clock divide ratios w.r.t hsDigClk for various modules in the device */
    uint32_t                            profileAddr;                                    /*!< Address to load Profile */
    uint32_t                            adcProfileAddr;                                 /*!< Address to load ADC Profile */
    
    adi_adrv903x_GainIndex_t            gainIndexes;                                    /*!< Current device Rx min max gain index values */
    uint32_t                            chunkStreamImageSize[ADI_ADRV903X_STREAM_MAX];  /*!< Stream Image Size */
    uint32_t                            chunkStreamImageOffset[ADI_ADRV903X_STREAM_MAX];/*!< Stream Image Offset */
    uint32_t                            currentStreamBinBaseAddr;                       /*!< Address to load current stream */
    uint32_t                            currentStreamBaseAddr;                          /*!< Address to load current stream base */
    uint8_t                             currentStreamNumberStreams;                     /*!< Number of Streams for current stream  */
    uint8_t                             currentStreamImageIndex;                        /*!< Index of current stream  */
    uint32_t                            currentStreamImageSize;                         /*!< Image size of current stream */   
    uint8_t                             rxFramerNp;                                     /*!< Rx Framer Np - converter sample resolution (12, 16, 24) */
    uint8_t                             rxChannel3bitSlicerMode;                        /*!< If a bit is set - corresponding Rx Channel Data format slicer ctrl is 3 bits */
    adi_adrv903x_SharedResourcePool_t   sharedResourcePool[ADI_ADRV903X_NUM_SHARED_RESOURCES]; /*!< Shared resource pool for the given device instance*/
    uint8_t                             linkSharingEnabled;                             /*!< Link sharing enabled if 1, disabled otherwise */
    adi_adrv903x_Cpu_t                  cpu;                                            /*!< Processor related info */
    adi_adrv903x_SpiOptions_t           spiOptions;                                     /*!< Holds the SPI runtime options used during API function calls */
    uint8_t                             spiFifoModeOn;                                  /*!< Internal use only: Keeps track of current SPI FIFO mode status. */
    uint8_t                             autoIncrModeOn;                                 /*!< Internal use only: Keeps track of current auto-increment mode status. */
    uint8_t                             spiStreamingOn;                                 /*!< Internal use only: Keeps track of current SPI Streaming status. */
    uint32_t                            currentPageStartingAddress;                     /*!< Internal use only: Starting address of the current 4K page */
    uint32_t                            previousSpiStreamingAddress;                    /*!< Internal use only: Only valid when spiStreamingOn is asserted. Stores the previous address written to determine if a CS toggle is required */
    adi_adrv903x_TxToOrxMappingConfig_t txToOrxMappingConfig;                           /*!< Internal use only: Tx to Orx Mapping Configuration */
    adi_adrv903x_StreamGpioPinCfg_t     streamGpioMapping;                              /*!< Internal use only: Stream GPIO mapping */

} adi_adrv903x_Info_t;

/**
 *  \brief Data structure to hold band data
 */
typedef struct adi_adrv903x_BandCfgExtract
{
    uint8_t enabled;                /*!< Band enabled flag */
    uint32_t rfCenterFreq_kHz;      /*!< RF center frequency */
    uint32_t instBw_kHz;            /*!< Instantaneous signal bandwidth */
    uint32_t sampleRate_kHz;        /*!< Band sample rate */
    uint32_t bandOffset_kHz;        /*!< LO and NCO offset */
} adi_adrv903x_BandCfgExtract_t;

/**
 *  \brief Data structure to hold Tx data path settings
 */
typedef struct adi_adrv903x_TxChannelCfgExtract
{
    uint32_t                        rfBandwidth_kHz;                            /*!< Tx RF passband bandwidth for the profile */    
    uint32_t                        totalDecimation;                            /*!< Total DDC decimation  */
    uint32_t                        digChanMask;                                /*!< Tx digital channel mask indicates which DUCs to initialize */
    uint32_t                        txLbAdcSampleRate_kHz;                      /*!< TX LB ADC Sample rate */
    uint32_t                        txLbAdcClkDiv;                              /*!< The logical (not bitfield) value of the LB ClkGen's interface divider. i.e. 5 => Div5 etc. */
    adi_adrv903x_BandCfgExtract_t   bandSettings[ADI_ADRV903X_DUC_NUM_BAND];    /*!< Tx DUC settings */
    uint32_t                        pfirRate_kHz;                               /*!< Tx PFIR rate */
} adi_adrv903x_TxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ADRV903X Tx Channel configuration settings
 */
typedef struct adi_adrv903x_TxSettingsExtract
{
    uint32_t                               txInitChannelMask;                           /*!< Tx channel mask of which channels to initialize */
    adi_adrv903x_TxChannelCfgExtract_t     txChannelCfg[ADI_ADRV903X_MAX_TXCHANNELS];   /*!< Tx settings per Tx channel */
} adi_adrv903x_TxSettingsExtract_t;

/**
 *  \brief Data structure to hold Rx data path settings
 */
typedef struct adi_adrv903x_RxChannelCfgExtract
{
    uint32_t                        rfBandwidth_kHz;                            /*!< Rx RF passband bandwidth for the profile */
    uint32_t                        rxDdc0OutputRate_kHz;                       /*!< Rx output sample rate per Rx channel Band 0 */
    uint32_t                        rxDdc1OutputRate_kHz;                       /*!< Rx output sample rate per Rx channel Band 1 */
    uint32_t                        digChanMask;                                /*!< Rx digital channel mask indicates which DDCs to initialize */
    uint32_t                        rxAdcSampleRate_kHz;                        /*!< Rx ADC sample rate */
    adi_adrv903x_BandCfgExtract_t   bandSettings[ADI_ADRV903X_DDC_NUM_BAND];    /*!< Rx DDC settings for this channel's profile */
} adi_adrv903x_RxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ORx data path settings
 */
typedef struct adi_adrv903x_OrxChannelCfgExtract
{
    uint32_t                            rfBandwidth_kHz;                     /*!< Rx RF passband bandwidth for the profile */
    uint32_t                            orxOutputRate_kHz;                   /*!< ORx output sample rate per ORx channel */
    uint32_t                            orxAdcSampleRate_kHz;                /*!< ORx ADC sample rate*/
} adi_adrv903x_OrxChannelCfgExtract_t;

/**
 *  \brief Data structure to hold ADRV903X Rx Channel configuration settings
 */
typedef struct adi_adrv903x_RxSettingsExtract
{
    uint32_t                            rxInitChannelMask;                              /*!< Rx channel mask of which channels to initialize */
    adi_adrv903x_RxChannelCfgExtract_t  rxChannelCfg[ADI_ADRV903X_MAX_RX_ONLY];         /*!< Rx settings per Rx channel */
} adi_adrv903x_RxSettingsExtract_t;

/**
 *  \brief Data structure to hold digital clock settings
 */
typedef struct adi_adrv903x_ClockSettingsExtract
{
    uint32_t                    deviceClockScaled_kHz; /*!< Scaled device clock frequency in kHz */
    uint8_t                     padDivideRatio;        /*!< Pad divider ratio calculated by configurator tool */
    uint8_t                     armClkDivideRatio;     /*!< FW Clock Divide ratio calculated by Configurator tool */
    uint8_t                     armClkDevClkDivRatio;  /*!< FW Clock Divide ratio calculated by Configurator tool */
    uint32_t                    hsDigClk_kHz;          /*!< Digital clock */
} adi_adrv903x_ClockSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer settings
 */
typedef struct adi_adrv903x_FramerSettingsExtract
{
    uint8_t   jesdM;                  /*!< M value */
    uint8_t   jesdNp;                 /*!< Np value */
    uint8_t   serialLaneEnabled;      /*!< lane enabled*/
    uint32_t  iqRate_kHz;             /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz;           /*!< Framer Lane rate */
    uint8_t   linkLsSampleXBar [ADI_ADRV903X_NUM_LKSH_SAMPLE_XBAR];
} adi_adrv903x_FramerSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer settings
 */
typedef struct adi_adrv903x_DeframerSettingsExtract
{
    uint8_t   jesdM;                  /*!< M value */
    uint8_t   jesdNp;                 /*!< Np value */
    uint8_t   deserialLaneEnabled;    /*!< lane enabled */
    uint32_t  iqRate_kHz;             /*!< Framer I/Q rate */
    uint32_t  laneRate_kHz; /*!< Framer Lane rate */
    uint32_t  interleavingEnabled; /*!< Interleaver enabled */
} adi_adrv903x_DeframerSettingsExtract_t;

/**
 *  \brief Data structure to hold Jesd Framer and Deframer settings
 */
typedef struct adi_adrv903x_JesdSettingsExtract
{
    adi_adrv903x_FramerSettingsExtract_t   framerSetting[ADI_ADRV903X_MAX_FRAMERS];     /*!< Framer settings */
    adi_adrv903x_DeframerSettingsExtract_t deframerSetting[ADI_ADRV903X_MAX_DEFRAMERS]; /*!< Deframer setting */
    adi_adrv903x_FramerSettingsExtract_t   framerLsSetting[ADI_ADRV903X_MAX_FRAMERS_LS]; /*!< Framer Link Sharing settings */
    uint8_t                                rxdesQhfrate;   /*!< SERDES Rate mode */
} adi_adrv903x_JesdSettingsExtract_t;

/*
 *  \brief Data structure to hold ORx channel configuration settings
 */
typedef struct adi_adrv903x_OrxSettingsExtract
{
    adi_adrv903x_OrxChannelCfgExtract_t orxChannelCfg[ADI_ADRV903X_MAX_ORX]; /*!< Orx output sample rate */
} adi_adrv903x_OrxSettingsExtract_t;

/**
 * \brief Data structure to hold ADRV903X device instance initialization settings
 */
typedef struct adi_adrv903x_InitExtract
{
   
    adi_adrv903x_ClockSettingsExtract_t     clocks;                                                             /*!< Holds settings for CLKPLL and reference clock */
    adi_adrv903x_RxSettingsExtract_t        rx;                                                                 /*!< Rx settings data structure */
    adi_adrv903x_TxSettingsExtract_t        tx;                                                                 /*!< Tx settings data structure */
    adi_adrv903x_OrxSettingsExtract_t       orx;                                                                /*!< ORx settings data structure */
    adi_adrv903x_JesdSettingsExtract_t      jesdSetting;                                                        /*!< Jesd settings *//*!< ORx settings data structure */
    uint8_t                                 rxTxCpuConfig[ADI_ADRV903X_MAX_CHANNELS];                           /*!< Defines which CPU each channel is assigned to (Rx/Tx) */
    uint8_t                                 orxCpuConfig[ADI_ADRV903X_MAX_ORX];                                 /*!< Defines which CPU each channel is assigned to (ORx)  */
    uint8_t                                 jesd204DesLaneCpuConfig[ADI_ADRV903X_MAX_SERDES_LANES];             /*!< Defines which CPU each deserializer lane calibration is assigned to */
    uint8_t                                 chanAssign[ADI_ADRV903X_MAX_CHANNELS];                              /*!< This is used to reference a channel to a rx/tx profile def'n in rxConfig/txConfig */
    uint8_t                                 featureMask[ADI_ADRV903X_MAX_FEATUREMASK];                          /*!< Device Extract information 128 bits of Feature Mask */
} adi_adrv903x_InitExtract_t;


/**
 * \brief Data structure to hold ADRV903X device instance settings
 */
typedef struct adi_adrv903x_Device
{
    adi_common_Device_t                 common;         /*!< Common layer structure */
    adi_adrv903x_Info_t                 devStateInfo;   /*!< ADRV903X run time state information container */
    adi_adrv903x_SpiConfigSettings_t    spiSettings;    /*!< Pointer to ADRV903X SPI Settings */
    adi_adrv903x_InitExtract_t          initExtract;    /*!< ADRV903X init info extract from CPU Profile container */
} adi_adrv903x_Device_t;
#endif /* CLIENT_IGNORE */

#endif
