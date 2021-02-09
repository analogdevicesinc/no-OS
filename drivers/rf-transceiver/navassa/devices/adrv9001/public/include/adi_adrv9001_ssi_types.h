/*!
* \file
* \brief Contains ADRV9001 API SSI configuration and data type definitions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_SSI_TYPES_H_
#define _ADI_ADRV9001_SSI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adi_common.h"
#include "adi_adrv9001_rx_types.h"
#include "adi_adrv9001_tx_types.h"

/**
 * \brief Type of data to transmit over SSI
 */
typedef enum adi_adrv9001_SsiTestModeData
{
    ADI_ADRV9001_SSI_TESTMODE_DATA_NORMAL = 0,      /*!< No check available for this data type */
    ADI_ADRV9001_SSI_TESTMODE_DATA_FIXED_PATTERN,   /*!< Length of pattern is dependent on SSI configuration.
                                                         Refer to 'fixedDataPatternToTransmit' in adi_adrv9001_RxSsiTestModeCfg_t for details */
    ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_NIBBLE,     /*!< CSSI-Only */
    ADI_ADRV9001_SSI_TESTMODE_DATA_RAMP_16_BIT,     /*!< CSSI and LSSI */
    ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS15,          /*!< LSSI-Only */
    ADI_ADRV9001_SSI_TESTMODE_DATA_PRBS7,           /*!< LSSI-Only */
} adi_adrv9001_SsiTestModeData_e;

/**
 * \brief SSI power down modes in LVDS mode
 */
typedef enum adi_adrv9001_SsiPowerDown
{
    ADI_ADRV9001_SSI_POWER_DOWN_DISABLED = 0, /*!< All SSI PADS powered up in PRIMED */
    ADI_ADRV9001_SSI_POWER_DOWN_MEDIUM = 1,   /*!< RX_CLK and TX_REF_CLK SSI pads powered up, 
                                                   TX_CLK and all STROBE and DATA SSI pads powered down in PRIMED */
    ADI_ADRV9001_SSI_POWER_DOWN_HIGH = 2,     /*!< All SSI pads powered down in PRIMED */
}adi_adrv9001_SsiPowerDown_e;

/**
 * \brief Data structure to transmit ADRV9001 SSI test mode configuration for Rx channel.
 */
typedef struct adi_adrv9001_RxSsiTestModeCfg
{
    adi_adrv9001_SsiTestModeData_e testData;   /*!< Type of data to transmit over SSI */
    uint32_t fixedDataPatternToTransmit;       /*!< Value of Fixed pattern to transmit over interface. For various SSI data format:
                                                CMOS: Pattern is truncated to bit3  - bit0 value is transmitted on RxSSI I and Q (where applicable)
                                                LVDS: Pattern is truncated to bit15 - bit0 value transmitted on RxSSI I and Q (where applicable) */
} adi_adrv9001_RxSsiTestModeCfg_t;

/**
 * \brief Data structure to receive ADRV9001 SSI test mode configuration for Tx channel.
 */
typedef struct adi_adrv9001_TxSsiTestModeCfg
{
    adi_adrv9001_SsiTestModeData_e testData;   /*!< Type of data to receive over SSI and check */
    uint32_t fixedDataPatternToCheck;          /*!< Value of Fixed pattern to check against pattern recevied over interface */
} adi_adrv9001_TxSsiTestModeCfg_t;

/**
 * \brief Data structure to hold ADRV9001 SSI test mode configuration status.
 */
typedef struct adi_adrv9001_TxSsiTestModeStatus
{
    uint8_t dataError;                       /*!< non zero if error from Fixed Pattern, Ramp or PRBS checker */
    uint8_t fifoFull;                        /*!< non-zero if FIFO is full */
    uint8_t fifoEmpty;                       /*!< non-zero if FIFO is empty */
    uint8_t strobeAlignError;                /*!< Mismatch in external and internal clock */
} adi_adrv9001_TxSsiTestModeStatus_t;

/**
 * \brief Data structure to hold ADRV9001 CMOS delay calibration configuration for each channel.
 */
typedef struct adi_adrv9001_SsiCalibrationCfg
{
    uint8_t rxClkDelay[ADI_ADRV9001_MAX_RX_ONLY];       /*!< CLK delay for Rx1/Rx2 channels */
    uint8_t rxStrobeDelay[ADI_ADRV9001_MAX_RX_ONLY];    /*!< Strobe delay for Rx1/Rx2 channels */
    uint8_t rxIDataDelay[ADI_ADRV9001_MAX_RX_ONLY];     /*!< I data delay for Rx1/Rx2 channels */
    uint8_t rxQDataDelay[ADI_ADRV9001_MAX_RX_ONLY];     /*!< Q data delay for Rx1/Rx2 channels */

    uint8_t txClkDelay[ADI_ADRV9001_MAX_TXCHANNELS];    /*!< CLK delay for Tx1/Tx2 channels */
    uint8_t txRefClkDelay[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Ref CLK delay for Tx1/Tx2 channels */
    uint8_t txStrobeDelay[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Strobe delay for Tx1/Tx2 channels */
    uint8_t txIDataDelay[ADI_ADRV9001_MAX_TXCHANNELS];  /*!< I data delay for Tx1/Tx2 channels */
    uint8_t txQDataDelay[ADI_ADRV9001_MAX_TXCHANNELS];  /*!< Q data delay for Tx1/Tx2 channels */

} adi_adrv9001_SsiCalibrationCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_SSI_TYPES_H_ */
