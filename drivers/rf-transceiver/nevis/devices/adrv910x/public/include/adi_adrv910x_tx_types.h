/**
* \file
* \brief Contains ADRV910X API Tx datapath data types
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the FPGA910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV910X_TX_TYPES_H_
#define _ADI_ADRV910X_TX_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#define ADRV910X_TX_ATTEN_TABLE_MAX 960
#define ADRV910X_ADDR_TX_ATTEN_TABLE 0x45300000
#define ADRV910X_TX_ATTEN_MDB_MAX 41950
#define ADRV910X_TX_ATTEN_MDB_RESOLUTION 50

/**
*  \brief Enum to set the Tx Attenuation step size
*/
typedef enum adi_adrv910x_TxAttenStepSize
{
    ADI_ADRV910X_TXATTEN_0P05_DB = 0,    /*!< Tx attenuation 0.05dB step size */
    ADI_ADRV910X_TXATTEN_0P1_DB  = 1,    /*!< Tx attenuation 0.1dB step size */
    ADI_ADRV910X_TXATTEN_0P2_DB  = 2,    /*!< Tx attenuation 0.2dB step size */
    ADI_ADRV910X_TXATTEN_0P4_DB  = 3     /*!< Tx attenuation 0.4dB step size */
} adi_adrv910x_TxAttenStepSize_e;

/**
*  \brief Enum for the different attenuation mode
*/
typedef enum adi_adrv910x_TxAttenuationControlMode
{
	ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS = 0,
	/*!< Tx attenuation mode Bypass: zero total attenuation */
	ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI = 1,
	/*!< Tx attenuation mode set by 10-bit attenuation index used to determine total attenuation */
	ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_PIN = 3,
	/*!< Tx attenuation is control with GPIO Incr/Decr: total attenuation is altered incrementally using pin control */
	ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC = 4
	/*!< Tx attenuation : Closed Loop Gain Control : 
	 *   If CLGC is enabled in OBJID_CFG_DPD_PRE_INIT_CAL, limited to TX_ATTENUATION_CONTROL_MODE_CLGC */
} adi_adrv910x_TxAttenuationControlMode_e;

/**
* \brief Data structure to hold an adrv910x device Tx attenuation Table Row
*        (Single row of a complete table)
*/
typedef struct adi_adrv910x_TxAttenTableRow
{
    uint16_t txAttenMult;       /*!< Digital attenuation multiplier; Valid range: 0-4095 */
    uint8_t  txAttenHp;         /*!< Analog coarse attenuation gain; Valid range: 0-63 */
    uint8_t  Reserve;
} adi_adrv910x_TxAttenTableRow_t;

typedef struct adi_adrv910x_TxAttenuationConfig_t
{
	bool disableTxOnPllUnlock;							 /*!< If true, the PA will ramp down to the max attenuation if
                                                         *   an RF1 or RF2 PLL unlock occurs
                                                         *   NOTE: Currently read-only */
	adi_adrv910x_TxAttenStepSize_e txAttenStepSize;		/*!< Tx Attenuation step size */
	adi_adrv910x_TxAttenuationControlMode_e attenMode;	/*!< The mode to control Tx attenuation */
} adi_adrv910x_TxAttenuationConfig_t;

#endif /* _ADI_ADRV910X_TX_TYPES_H_ */
