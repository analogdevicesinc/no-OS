/**
* Copyright 2020 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_eeprom_types.h
 * \brief Contains enum and structure data types for all EEPROM function calls
 */

#ifndef _ADI_EEPROM_TYPES_H_
#define _ADI_EEPROM_TYPES_H_

#include "adi_common.h"
#include "adi_platform_types.h"

/* NOTE: EPRC Limitation; String Length should be ADI_HAL_STRING_LENGTH */
#define ADI_EEPROM_MAX_STRING_LENGTH    256U

/**
 * \brief FRU Structure
 */
typedef struct adi_eeprom_BoardInfo
{
    uint8_t manufacturer[ADI_EEPROM_MAX_STRING_LENGTH];     /* Manufacturer */
    uint8_t productName[ADI_EEPROM_MAX_STRING_LENGTH];      /* Product Name */
    uint8_t serialNumber[ADI_EEPROM_MAX_STRING_LENGTH];     /* Serial Number */
    uint8_t partNumber[ADI_EEPROM_MAX_STRING_LENGTH];       /* Part Number */
    uint8_t pcbId[ADI_EEPROM_MAX_STRING_LENGTH];            /* PCB ID */
    uint8_t pcbName[ADI_EEPROM_MAX_STRING_LENGTH];          /* PCB Name */
    uint8_t bomRev[ADI_EEPROM_MAX_STRING_LENGTH];           /* BOM Revision */
} adi_eeprom_BoardInfo_t;


#ifndef CLIENT_IGNORE

/**
 * \brief Data structure to hold an EEPROM device instance status information
 */
typedef struct adi_eeprom_Info
{
    adi_eeprom_BoardInfo_t boardInfo;
} adi_eeprom_Info_t;

/**
 * \brief Structure to hold EEPROM device settings
 */
typedef struct adi_eeprom_Device
{
    adi_common_Device_t common;         /*!< Common layer structure */
    adi_eeprom_Info_t   devStateInfo;   /*!< EEPROM run time state information */
} adi_eeprom_Device_t;

#endif // !CLIENT_IGNORE

#endif /* _ADI_EEPROM_TYPES_H_ */
