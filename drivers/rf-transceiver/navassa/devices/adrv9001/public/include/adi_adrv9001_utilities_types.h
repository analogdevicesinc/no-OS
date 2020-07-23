/*!
* \file
* \brief Contains ADRV9001 API utility initialization type definitions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_UTILITIES_TYPES_H_
#define _ADI_ADRV9001_UTILITIES_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_cals_types.h"
#include "adi_adrv9001_dpd_types.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_rx_types.h"
#include "adi_adrv9001_ssi_types.h"
#include "adi_adrv9001_powermanagement_types.h"
#include "adi_adrv9001_fh_types.h"
#include "adi_adrv9001_tx_types.h"
#include "adi_adrv9001_types.h"
#include "adi_common_error_types.h"

/**
* \brief Macro constants
*/
#define ADI_ADRV9001_ARM_BINARY_IMAGE_FILE_SIZE_BYTES (256*1024)
#define ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (32*1024)
#define ADI_ADRV9001_RX_GAIN_TABLE_SIZE_ROWS 256
#define ADI_ADRV9001_TX_ATTEN_TABLE_SIZE_ROWS 1024

#define ADI_ADRV9001_LINE_BUFFER_SIZE 128
#define ADI_ADRV9001_HEADER_BUFFER_SIZE 16

/**
* \brief Data structure to hold Radio Ctrl Utility Init structures
*/
typedef struct adi_adrv9001_RadioCtrlInit
{
    adi_adrv9001_DeviceClockDivisor_e adrv9001DeviceClockOutputDivisor;    /*!< ADRV9001 divisor for DEV_CLK_OUT to the user */
    adi_adrv9001_GpioCtrlInitCfg_t gpioCtrlInitCfg;
    adi_adrv9001_SlewRateLimiterCfg_t  slewRateLimiterCfg; /*!< Configuration to hold Slew Rate Limiter on init */

    adi_adrv9001_Carrier_t rxCarriers[ADI_ADRV9001_MAX_RX_ONLY];    /*!< Configuration to hold Rx1/Rx2 channel carrier configuration on init */
    adi_adrv9001_Carrier_t txCarriers[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Configuration to hold Tx1/Tx2 channel carrier configuration on init */

    bool txOutputPowerBoostEnable[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Enables Tx output power boost, increasing Tx output power by 3dB. Linearity will be degraded */
    adi_adrv9001_DpdInitCfg_t txDpdInit[ADI_ADRV9001_MAX_TXCHANNELS];

    /* Stream binary image to download.  Used if adi_adrv9001_PlatformFiles.streamImageFile[0] == 0 */
    uint8_t (*streamImageBinary)[ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES];

    adi_adrv9001_ChannelEnablementDelays_t rxEnableDelays[ADI_ADRV9001_NUM_CHANNELS];
    adi_adrv9001_ChannelEnablementDelays_t txEnableDelays[ADI_ADRV9001_NUM_CHANNELS];

    adi_adrv9001_SsiCalibrationCfg_t ssiConfig;                 /*!< CMOS/LVDS SSI calibration */
    uint32_t externalPathDelay_ps[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< External path delay (LSB is 100ps; valid 0-6553500 ps) */
    bool adcDynamicSwitchEnable[ADI_ADRV9001_MAX_RX_ONLY];      /*!< Enable ADC Dynamic Switching */

    /* TODO: Make these pointers in future */
    adi_adrv9001_PowerManagementSettings_t powerManagementSettings;                    /*!< LDO settings for adjusting power consumption */
    adi_adrv9001_arm_MonitorModeInitCfg_t monitorModeInitCfg;  /*!< Monitor Mode configuration at initialization time */
    adi_adrv9001_FhCfg_t fhConfig; /*!< Frequency Hopping configuration at initialisation time */
    adi_adrv9001_SsiPowerDown_e rxSsiPowerDown[ADI_ADRV9001_MAX_RX_ONLY]; /*!< LSSI power down for Rx1/2 */
    adi_adrv9001_SsiPowerDown_e txSsiPowerDown[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< LSSI power down for Tx1/2 */
} adi_adrv9001_RadioCtrlInit_t;

/**
* \brief Data structure to hold platform file addresses on the SD card
*/
typedef struct adi_adrv9001_PlatformFiles
{
    uint8_t armImageFile[128];
    uint8_t streamImageFile[256];
    uint8_t rxGainTableFile[128];
    uint8_t orxGainTableFile[128];
    uint8_t txAttenTableFile[128];
} adi_adrv9001_PlatformFiles_t;

/**
* \brief Data Structure to hold Resource Config settings
*/
typedef struct adi_adrv9001_ResourceCfg
{
    adi_adrv9001_Init_t *adrv9001Init;
    adi_adrv9001_RadioCtrlInit_t *radioCtrlInit;
    adi_adrv9001_PlatformFiles_t *adrv9001PlatformFiles;
} adi_adrv9001_ResourceCfg_t;

#endif