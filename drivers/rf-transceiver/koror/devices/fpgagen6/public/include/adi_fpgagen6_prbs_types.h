/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_prbs_types.h
* \brief Contains FPGAGEN6 API configuration and run-time type definitions
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_PRBS_TYPES_H_
#define _ADI_FPGAGEN6_PRBS_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"

/**
*  \brief Industry standard PRBS patterns supported by this API
*/
typedef enum adi_fpgagen6_PrbsTestModes
{
    ADI_FPGAGEN6_PRBS_OFF   = 0U,       /*!< Disable all PRBS modes. */
    ADI_FPGAGEN6_PRBS_7     = 1U,       /*!< Used to test channels with 8B/10B. */
    ADI_FPGAGEN6_PRBS_9     = 2U,       /*!< PRBS-9 is one of the recommended test patterns for SFP+. */
    ADI_FPGAGEN6_PRBS_15    = 3U,       /*!< PRBS-15 is often used for jitter measurement because it is the longest pattern the Agilent DCA-J sampling scope can handle. */
    ADI_FPGAGEN6_PRBS_23    = 4U,       /*!< PRBS-23 is often used for non-8B/10B encoding schemes. It is one of the recommended test patterns in the SONET specification. */
    ADI_FPGAGEN6_PRBS_31    = 5U       /*!< PRBS-31 is often used for non-8B/10B encoding schemes. It is a recommended PRBS test pattern for 10 Gigabit Ethernet. See IEEE 802.3ae-2002. */
} adi_fpgagen6_PrbsTestModes_e;

/**
*  \brief Data structure to hold the error count for the corresponding deserializer channel
*/
typedef struct adi_fpgagen6_PrbsErrorCount
{
    uint32_t errorCount[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];   /*!< Used to read the error count for all lanes. Index 0 corresponds to channel 0, etc. */
} adi_fpgagen6_PrbsErrorCount_t;

/**
*  \brief Data structure to hold the settings as they were when PRBS was enabled
*/
typedef struct adi_fpgagen6_PrbsStoredData
{
    uint8_t bufferEnable;       /*!< The value of the buffer enable prior to enabling PRBS (only valid for Serializer channels) */
    uint8_t gearboxEnable;      /*!< The value of the gearbox enable prior to enabling PRBS (only valid for Serializer channels) */
    uint8_t outClkSel;          /*!< The value of the out clk select bitfield prior to enabling PRBS (valid for both Serializer and Deserializer) */
    uint8_t prbsEnabled;        /*!< Flag to indicate if PRBS is currently enabled and the settings in this structure can be used to restore when PRBS is diabled */
} adi_fpgagen6_PrbsStoredData_t;

#endif
