/*!
* \file
* \brief Contains ADRV9001 API configuration and run-time type definitions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_TYPES_H_
#define _ADI_ADRV9001_TYPES_H_

#include "adi_adrv9001_defines.h"
#include "adi_adrv9001_profile_types.h"

#include "adi_common.h"

#define ADI_ADRV9001_NUM_TXRX_CHANNELS         0x4
#define ADI_ADRV9001_NUM_RX_CHANNELS           0x2
#define ADI_ADRV9001_NUM_TX_CHANNELS           0x2

#define ADI_ADRV9001_TX_PROFILE_VALID		  0x01
#define ADI_ADRV9001_RX_PROFILE_VALID		  0x02
#define ADI_ADRV9001_ORX_PROFILE_VALID		  0x04
#define ADI_ADRV9001_ELB_PROFILE_VALID		  0x08

#define ADI_ADRV9001_NUM_CHANNELS 2
#define ADI_ADRV9001_NUM_PORTS 2

#define ADI_ADRV9001_MAX_AUXDACS              4U

/* TODO: Determine a reasonable value */
#define ADI_ADRV9001_READY_FOR_MCS_DELAY_US 100U


/**
* \brief ADRV9001 part number
*/
typedef enum adi_adrv9001_PartNumber
{
	ADI_ADRV9001_PART_NUMBER_UNKNOWN    = -1,
	ADI_ADRV9001_PART_NUMBER_ADRV9002   = 0x0,
	ADI_ADRV9001_PART_NUMBER_ADRV9003   = 0xC,
	ADI_ADRV9001_PART_NUMBER_ADRV9004   = 0x8,
} adi_adrv9001_PartNumber_e;

/**
 * \brief Enum of all ADRV9001 channels
 * \note Maskable
 */
typedef enum adi_adrv9001_MailboxChannel
{
    ADI_ADRV9001_RX1  = 0x0001,
    ADI_ADRV9001_RX2  = 0x0002,
    ADI_ADRV9001_TX1  = 0x0004,
    ADI_ADRV9001_TX2  = 0x0008,
    ADI_ADRV9001_ORX1 = 0x0010,
    ADI_ADRV9001_ORX2 = 0x0020,
    ADI_ADRV9001_ILB1 = 0x0040,
    ADI_ADRV9001_ILB2 = 0x0080,
    ADI_ADRV9001_ELB1 = 0x0100,
    ADI_ADRV9001_ELB2 = 0x0200
} adi_adrv9001_MailboxChannel_e;

/**
* \brief Possible device clock divisor values
*/
typedef enum adi_adrv9001_DeviceClockDivisor
{
    ADI_ADRV9001_DEVICECLOCKDIVISOR_BYPASS   = 0,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_2        = 1,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_4        = 2,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_8        = 3,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_16       = 4,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_32       = 5,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_64       = 6,
    ADI_ADRV9001_DEVICECLOCKDIVISOR_DISABLED = 7 /* Arbitrary value, just to select in case to disable device clock output */
} adi_adrv9001_DeviceClockDivisor_e;

/**
 *  \brief Data structure to hold ADRV9001 API State
 */
typedef enum adi_adrv9001_ApiStates
{
    ADI_ADRV9001_STATE_POWERON_RESET    = 0x00,
    ADI_ADRV9001_STATE_ANA_INITIALIZED  = 0x01,
    ADI_ADRV9001_STATE_DIG_INITIALIZED  = 0x02,
    ADI_ADRV9001_STATE_STREAM_LOADED    = 0x04,
    ADI_ADRV9001_STATE_ARM_DEBUG_LOADED = 0x08,
    ADI_ADRV9001_STATE_ARM_LOADED       = 0x10,
    ADI_ADRV9001_STATE_INITCALS_RUN     = 0x20,
    ADI_ADRV9001_STATE_PRIMED           = 0x40,
    ADI_ADRV9001_STATE_IDLE             = 0x80,
    ADI_ADRV9001_STATE_STANDBY          = 0x100,
} adi_adrv9001_ApiStates_e;

/**
 * \brief Enumerated list of CMOS pads drive strength options for SPI_DO signal
 */
typedef enum adi_adrv9001_CmosPadDrvStr
{
    ADI_ADRV9001_CMOSPAD_DRV_WEAK   = 0,	/*!<    5pF load @ 75MHz */
    ADI_ADRV9001_CMOSPAD_DRV_STRONG = 1		/*!<  100pF load @ 20MHz */
} adi_adrv9001_CmosPadDrvStr_e;

/**
 * \brief Enumerated list of options to use to set the LO source for the Tx, Rx, LB mixers.
 */
typedef enum adi_adrv9001_TxRxLoSel
{
    ADI_ADRV9001_TXRXLOSEL_RX_LO1 = 1,
    ADI_ADRV9001_TXRXLOSEL_RX_LO2 = 1,
    ADI_ADRV9001_TXRXLOSEL_TX_LO1 = 1,
    ADI_ADRV9001_TXRXLOSEL_TX_LO2 = 1,
    ADI_ADRV9001_TXRXLOSEL_AUXLO  = 2
} adi_adrv9001_TxRxLoSel_e;

/**
 *  \brief Enum to select the proper gain for the FIR
 */
typedef enum adi_adrv9001_FirGain
{
    ADRV9001_FIR_GAIN_NEG_12_DB   = -12, /*!< FIR gain -12 */
    ADRV9001_FIR_GAIN_NEG_6_DB    = -6,  /*!< FIR gain -6 */
    ADRV9001_FIR_GAIN_ZERO_DB     = 0,   /*!< FIR gain 0 */
    ADRV9001_FIR_GAIN_POS_6_DB    = 6,   /*!< FIR gain 6 */
    ADRV9001_FIR_GAIN_POS_9P54_DB = 9,   /*!< FIR gain 9.54 */
    ADRV9001_FIR_GAIN_POS_12_DB   = 12,  /*!< FIR gain 12 */
    ADRV9001_FIR_GAIN_POS_14_DB   = 14,  /*!< FIR gain 14 */
    ADRV9001_FIR_GAIN_POS_20_DB   = 20,  /*!< FIR gain 20 */
    ADRV9001_FIR_GAIN_POS_24_DB   = 24,  /*!< FIR gain 24 */
    ADRV9001_FIR_GAIN_POS_26_DB   = 26   /*!< FIR gain 26 */
} adi_adrv9001_FirGain_e;

/*
*********************************************************************************************************
*                                             Structure definition
*********************************************************************************************************
*/

/**
* \brief Data structure to hold SPI settings for all system device types
 */
typedef struct adi_adrv9001_SpiSettings
{
    uint8_t msbFirst;                           		/*!< 1 = MSB First, 0 = LSB First Bit order for SPI transaction */
    uint8_t enSpiStreaming;                     		/*!< 1 = ADRV9001 SPI streaming mode; 0 = Standard mode */
    uint8_t autoIncAddrUp;                      		/*!< For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
    uint8_t fourWireMode;                       		/*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
    adi_adrv9001_CmosPadDrvStr_e cmosPadDrvStrength;   	/*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
} adi_adrv9001_SpiSettings_t;

/**
* \brief Data structure to hold clock divide ratios
*/
typedef struct adi_adrv9001_ClkDivideRatios
{
    uint8_t devClkDivideRatio; /*!< Dev clock divide ratio */
    uint8_t refClkDivideRatio; /*!< Ref clock divide ratio */
    uint8_t armClkDivideRatio; /*!< ARM clock divide ratio w.r.t hsDigClk*/
    uint8_t agcClkDivideRatio; /*!< AGC module clock divide ratio w.r.t hsDigClk*/
    uint8_t regClkDivideRatio; /*!< Register bus clock divide ratio w.r.t hsDigClk*/
    uint8_t txAttenDeviceClockDivideRatio; /*!< Tx Atten module clock divide ratio w.r.t hsDigClk*/
    uint8_t anaRefClockRatio;  /*!< Analog clock divide ratio */
} adi_adrv9001_ClkDivideRatios_t;

/**
* \brief ADRV9001 silicon version information
*/	
typedef struct adi_adrv9001_SiliconVersion
{
    uint8_t major; /*!< Major silicon version (0xA, 0xB, etc) */
    uint8_t minor; /*!< Minor silicon version */
} adi_adrv9001_SiliconVersion_t;

#ifndef CLIENT_IGNORE
/**
* \brief Data structure to hold a ADRV9001 device instance status information
*
* Application layer allocates this memory, but only API writes to it to keep up with the API run time state.
*/
typedef struct adi_adrv9001_Info
{
    uint8_t deviceSiRev;												/*!< ADRV9001 silicon rev read during ADRV9001_initialize */
    uint8_t deviceProductId;                                            /*!< ADRV9001 Product ID read during HWOpen */
    uint16_t devState;									                /*!< Current device state of the part, i.e., radio on, radio off, arm loaded,
                                                                             etc., defined by deviceState enum */
    uint32_t initializedChannels;										/*!< Holds Rx/ORx/Tx channels that were initialized and calibrated for the
                                                                             current device */
    uint32_t trackingCalEnableMask;										/*!< Keeps track of tracking calibration mask of Rx and ORx as set by the
                                                                             customer through the api */
    uint32_t profilesValid;												/*!< Current device profilesValid bit field for use notification, i.e.,
                                                                             Tx = 0x01, Rx = 0x02, Orx = 0x04 */
    adi_adrv9001_TxSignalType_e outputSignaling[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Output to Analog signal type */
    uint32_t swTest;													/*!< Software testmode signal */
    uint32_t hsDigClk_Hz;												/*!< Calculated in initialize() digital clock used throughout API functions */
    uint32_t deviceClock_kHz;                                           /*!< Device clock frequency in kHz (copy from adi_adrv9001_ClockSettings_t struct) */
    adi_adrv9001_ClkPllMode_e  clkPllMode;                              /*!< CLKPLL Mode */
    adi_adrv9001_ClkDivideRatios_t clkDivideRatios;						/*!< Clock divide ratios w.r.t hsDigClk for various modules in the device */
    uint32_t  profileAddr;												/*!< Address to load Profile */
    uint32_t  adcProfileAddr;											/*!< Address to load ADC Profile */
    uint32_t  pfirProfileAddr;                                          /*!< Address to load PFIR coefficients */
    uint32_t  fhHopTable1Addr;                                          /*!< Address to load hop table 1 in frequency hopping mode */
    uint32_t  fhHopTable2Addr;                                          /*!< Address to load hop table 2 in frequency hopping mode */
    uint32_t txInputRate_kHz[ADI_ADRV9001_MAX_TXCHANNELS];				/*!< Tx Input sample rate from currently loaded profile */
    uint32_t rxOutputRate_kHz[ADI_ADRV9001_MAX_RXCHANNELS];				/*!< Rx Output sample rate from currently loaded profile */
    uint32_t rx1InterfaceSampleRate_kHz;                                /*!< Rx1 Interface sample rate from currently loaded profile */
    uint16_t chunkStreamImageSize[12];									/*!< Stream Image Size */
    uint16_t chunkStreamImageOffset[12];								/*!< Stream Image Offset */
    uint32_t currentStreamBinBaseAddr;									/*!< Address to load current stream */
    uint32_t currentStreamBaseAddr;										/*!< Address to load current stream base */
    uint8_t currentStreamNumberStreams;									/*!< Number of Streams for current stream  */
    uint8_t currentStreamImageIndex;									/*!< Index of current stream  */
    uint32_t currentStreamImageSize;									/*!< Image size of current stream */
    uint8_t frequencyHoppingEnabled;                                    /*!< Frequency hopping enabled flag from currently loaded profile */
} adi_adrv9001_Info_t;

/**
* \brief Data structure to hold ADRV9001 device instance settings
*/
typedef struct adi_adrv9001_Device
{
    adi_common_Device_t			common;        /*!< Common layer structure */
    adi_adrv9001_Info_t			devStateInfo;  /*!< ADRV9001 run time state information container */
    adi_adrv9001_SpiSettings_t	spiSettings;   /*!< Pointer to ADRV9001 SPI Settings */
} adi_adrv9001_Device_t;
#endif /* CLIENT_IGNORE */

#endif /* _ADI_ADRV9001_TYPES_H_ */
