/**
 * \file
 * \brief Contains ADRV9001 API Multi-Chip Synchronization (MCS) data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_MCS_TYPES_H_
#define _ADI_ADRV9001_MCS_TYPES_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The type of MCS action requested by the ADRV9001
 */
typedef enum adi_adrv9001_McsAction
{
    ADI_ADRV9001_MCS_READY = 0x06,              /*!< Indicates ADRV9001 is ready for MCS after BBIC initiates */
    ADI_ADRV9001_MCS_INT_DIGITAL_ALL = 0x07,    /*!< ADRV9001 requested internal sync of digital CLK and SSI */
    ADI_ADRV9001_MCS_INT_DIGITAL_CLK = 0x08,    /*!< ADRV9001 requested internal sync of digital CLK */
    ADI_ADRV9001_MCS_INT_DIGITAL_SSI = 0x09,    /*!< ADRV9001 requested internal sync of digital SSI */
    ADI_ADRV9001_MCS_EXT_DIGITAL_ALL = 0x0A,    /*!< ADRV9001 requested external sync of digital CLK and SSI */
    ADI_ADRV9001_MCS_EXT_DIGITAL_CLK = 0x0B,    /*!< ADRV9001 requested external sync of digital CLK */
    ADI_ADRV9001_MCS_EXT_DIGITAL_SSI = 0x0C,    /*!< ADRV9001 requested external sync of digital SSI */
    ADI_ADRV9001_MCS_EXT_ANALOG_ALL = 0x0D,     /*!< ADRV9001 requested external sync of all 4 analog stages */
    ADI_ADRV9001_MCS_EXT_ANALOG_1 = 0x0E,       /*!< ADRV9001 requested external sync of first 2 analog stages */
    ADI_ADRV9001_MCS_EXT_ANALOG_2 = 0x0F,       /*!< ADRV9001 requested external sync of analog stage 3 */
    ADI_ADRV9001_MCS_EXT_ANALOG_3 = 0x10,       /*!< ADRV9001 requested external sync of analog stage 4 */
} adi_adrv9001_McsAction_e;
    
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_MCS_TYPES_H_ */
