/**
 * \file
 * \brief ADRV910X core types
 * 
 * Copyright (c) 2022 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_ADRV910X_TYPES_H_
#define _ADI_ADRV910X_TYPES_H_

#include "adi_adrv910x_common.h"
#include "adi_common.h"
#include "device_profile_top_t.h"
#include "device_profile_ps2_t.h"

#define ADI_ADRV910X_NUM_CHANNELS 2
#define ADI_ADRV910X_NUM_PORTS 2
#define ADI_ADRV910X_MAX_RXCHANNELS 8

#define ADI_ADRV910X_TX_PROFILE_VALID		  0x01
#define ADI_ADRV910X_RX_PROFILE_VALID		  0x02
#define ADI_ADRV910X_ORX_PROFILE_VALID		  0x04
#define ADI_ADRV910X_ELB_PROFILE_VALID		  0x08

/**
 * \brief Enum of all ADRV910X channels
 * \note Maskable
 */
typedef enum adi_adrv910x_MailboxChannel
{
	ADI_ADRV910X_RX1 = 0x0001,
	ADI_ADRV910X_RXNB = 0x0002,
	ADI_ADRV910X_TX1 = 0x0004,
	ADI_ADRV910X_TXNB  = 0x0008,
	ADI_ADRV910X_ORX1 = 0x0010,
	ADI_ADRV910X_ORXNB = 0x0020,
	ADI_ADRV910X_ILB1 = 0x0040,
	ADI_ADRV910X_ILBNB = 0x0080,
	ADI_ADRV910X_ELB1 = 0x0100,
	ADI_ADRV910X_ELBNB = 0x0200
} adi_adrv910x_MailboxChannel_e;

/**
 *  \brief Data structure to hold ADRV910X API State
 */
typedef enum adi_adrv910x_ApiStates
{
	ADI_ADRV910X_STATE_POWERON_RESET    = 0x00,
	ADI_ADRV910X_STATE_ANA_INITIALIZED  = 0x01,
	ADI_ADRV910X_STATE_DIG_INITIALIZED  = 0x02,
	ADI_ADRV910X_STATE_STREAM_LOADED    = 0x04,
	ADI_ADRV910X_STATE_ARM_DEBUG_LOADED = 0x08,
	ADI_ADRV910X_STATE_ARM_LOADED       = 0x10,
	ADI_ADRV910X_STATE_INITCALS_RUN     = 0x20,
	ADI_ADRV910X_STATE_PRIMED           = 0x40,
	ADI_ADRV910X_STATE_IDLE             = 0x80,
	ADI_ADRV910X_STATE_STANDBY          = 0x100,
} adi_adrv910x_ApiStates_e;

/**
* \brief Possible device clock divisor values
*/
typedef enum adi_adrv910x_DeviceClockDivisor
{
	ADI_ADRV910X_DEVICECLOCKDIVISOR_BYPASS   = 0,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_2        = 1,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_4        = 2,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_8        = 3,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_16       = 4,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_32       = 5,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_64       = 6,
	ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED = 7 /* Arbitrary value, just to select in case to disable device clock output */
} adi_adrv910x_DeviceClockDivisor_e;

/**
 * \brief Enumerated list of CMOS pads drive strength options for SPI_DO signal
 */
typedef enum adi_adrv910x_CmosPadDrvStr
{
    ADI_ADRV910X_CMOSPAD_DRV_WEAK   = 0,	/*!<    5pF load @ 75MHz */
    ADI_ADRV910X_CMOSPAD_DRV_STRONG = 1		/*!<  100pF load @ 20MHz */
} adi_adrv910x_CmosPadDrvStr_e;

/**
* \brief ADRV910X part number
*/
typedef enum adi_adrv910x_PartNumber
{
    ADI_ADRV910X_PART_NUMBER_UNKNOWN    = -1,
    ADI_ADRV910X_PART_NUMBER_ADRV9101   = 0x19,
    ADI_ADRV910X_PART_NUMBER_ADRV9102   = 0x1,
    ADI_ADRV910X_PART_NUMBER_ADRV9103   = 0x18,
    ADI_ADRV910X_PART_NUMBER_ADRV9104   = 0x0,
} adi_adrv910x_PartNumber_e;

/**
* \brief ADRV910X Efuse error codes
*/
typedef enum adi_adrv910x_EfuseError
{
	ADI_ADRV910X_EFUSE_PLL2_DISABLE_ERROR = 0xD200,		/*!< Efuse RFPLL2 disabled  error */
	ADI_ADRV910X_EFUSE_RX2_DISABLE_ERROR = 0xD201,		/*!< Efuse RX2 disabled error */
	ADI_ADRV910X_EFUSE_TX2_DISABLE_ERROR = 0xD202,		/*!< Efuse TX2 disabled error */
	ADI_ADRV910X_EFUSE_DPD_DISABLE_ERROR = 0xD203,		/*!< Efuse DPD disabled error */
	ADI_ADRV910X_EFUSE_ASR_DISABLE_ERROR = 0xD204,		/*!< Efuse Arbitrary Sample Rates disabled error */
	ADI_ADRV910X_EFUSE_FH_DISABLE_ERROR = 0xD205,		/*!< Efuse Frequency Hopping disabled error */
	ADI_ADRV910X_EFUSE_FFH_DISABLE_ERROR = 0xD206,		/*!< Efuse Fast Frequency Hopping disabled error */
	ADI_ADRV910X_EFUSE_FPS_DISABLE_ERROR = 0xD207,		/*!< Efuse Fast Profile Switching disabled error */
	ADI_ADRV910X_EFUSE_HPADC_DISABLE_ERROR = 0xD208,	/*!< Efuse High Performance ADC disabled error */
	ADI_ADRV910X_EFUSE_PA_MON_DISABLE_ERROR = 0xD209,	/*!< Efuse PA monitoring disabled error */
	ADI_ADRV910X_EFUSE_WB_DISABLE_ERROR = 0xD20A,		/*!< Efuse WB disabled error */
	ADI_ADRV910X_EFUSE_MON_MODE_DISABLE_ERROR = 0xD20B, /*!< Efuse Monitor Mode disabled error */
	ADI_ADRV910X_EFUSE_HP_PLL_DISABLE_ERROR = 0xD20C,	/*!< Efuse High Power RFPLL disabled error */
	ADI_ADRV910X_EFUSE_FDD_DISABLE_ERROR = 0xD20D,		/*!< Efuse Frequency Division Duplex disabled error */
	ADI_ADRV910X_EFUSE_TX1_DISABLE_ERROR = 0xD20E,		/*!< Efuse TX1 disabled error */
	ADI_ADRV910X_EFUSE_SPARE_DISABLE_ERROR = 0xD20F,	/*!< Efuse Spare disabled error */
	ADI_ADRV910X_EFUSE_DATA_ERROR = 0xD210,				/*!< Efuse data error */
	ADI_ADRV910X_EFUSE_READ_DATA_TIMEOUT_ERROR = 0xD211,/*!< Efuse read data timeout error */
	ADI_ADRV910X_EFUSE_SEGMENT_NUM_ERROR = 0xD212,		/*!< Efuse segment number error */
	ADI_ADRV910X_EFUSE_PS2_DISABLE_ERROR = 0xD213		/*!< Efuse PS2 disabled error */

} adi_adrv910x_EfuseError_e;

/**
* \brief Data structure to hold SPI settings for all system device types
 */
typedef struct adi_adrv910x_SpiSettings
{
    uint8_t msbFirst;                           		/*!< 1 = MSB First, 0 = LSB First Bit order for SPI transaction */
    uint8_t enSpiStreaming;                     		/*!< 1 = ADRV910X SPI streaming mode; 0 = Standard mode */
    uint8_t autoIncAddrUp;                      		/*!< For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1 */
    uint8_t fourWireMode;                       		/*!< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode */
    adi_adrv910x_CmosPadDrvStr_e cmosPadDrvStrength;   	/*!< Drive strength of CMOS pads when used as outputs (SDIO, SDO, GP_INTERRUPT, GPIO 1, GPIO 0) */
} adi_adrv910x_SpiSettings_t;

#ifndef CLIENT_IGNORE

/**
* \brief Data structure to hold a ADRV910X device instance status information
*
* Application layer allocates this memory, but only API writes to it to keep up with the API run time state.
*/
typedef struct adi_adrv910x_Info
{
	uint8_t deviceSiRev;												/*!< ADRV910X silicon rev read during ADRV910X_initialize */
	uint8_t deviceProductId;                                            /*!< ADRV910X Product ID read during HWOpen */
	uint16_t devState;									                /*!< Current device state of the part, i.e., radio on, radio off, arm loaded,
	                                                                         etc., defined by deviceState enum */
	uint32_t initializedChannels;										/*!< Holds Rx/ORx/Tx channels that were initialized and calibrated for the
																		 current device */
	uint32_t profilesValid;												/*!< Current device profilesValid bit field for use notification, i.e.,
																		 Tx = 0x01, Rx = 0x02, Orx = 0x04 */
	uint32_t  profileAddr;												/*!< Address to load Profile */
	uint32_t  pfirProfileAddr;                                          /*!< Address to load PFIR coefficients */
	rxSignalType_e rxOutputSignaling[NUM_RX_CHANNELS];					/*!< Rx Output to Analog signal type */
	signalType_e txOutputSignaling[NUM_TX_CHANNELS];					/*!< Tx Output to Analog signal type */
	rxGainTableType_e gainTableType[NUM_RX_CHANNELS];					/*!< type of gain table loaded during ADRV910X initialization */
	uint32_t rxOutputRate_kHz[ADI_ADRV910X_MAX_RXCHANNELS];				/*!< Rx Output sample rate from currently loaded profile */
	uint32_t txInputRate_kHz[NUM_TX_CHANNELS];							/*!< Tx Input sample rate from currently loaded profile */
	uint8_t txAttenMode;								 				/* TX Attenuation Mode*/
	bool rxNbAgcHighThresholdModeEnable; 								/*!< Enable PFIR on AGC Path*/
	rxNbAgcHighThresholdSettings_t agcHighThresholdSettings; 			/*!< rxNbAgc settings for the profile */
} adi_adrv910x_Info_t;

/**
 * \brief ADRV910X device instance settings
 */
typedef struct 
{
    adi_common_Device_t common;
	adi_adrv910x_Info_t			devStateInfo;  /*!< ADRV910X run time state information container */
#ifndef NEVIS_PS2
	adi_adrv910x_SpiSettings_t	spiSettings;   /*!< Pointer to ADRV910X SPI Settings */
#endif
} adi_adrv910x_Device_t;
#endif // !CLIENT_IGNORE

#ifdef NEVIS_PS2
typedef int32_t(*halTimer)(void *devHalCfg, uint32_t time_us);
#endif /* NEVIS_PS2 */

#endif // !_ADI_ADRV910X_TYPES_H_
