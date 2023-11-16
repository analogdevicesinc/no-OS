/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file fpgagen6_test_types.h
 * \brief Contains FPGA Test enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_TEST_TYPES_H_
#define _ADI_FPGAGEN6_TEST_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"


typedef struct fpgagen6_TestInfo
{
    uint32_t                        swTest;             /*!< Software testmode signal */
    uint8_t                         testLoopbackFlag;   /*!< Loopback Mode */
    adi_fpgagen6_InterfaceType_e    interfaceType;      /*!< Data Interface Type */
} fpgagen6_TestInfo_t;

#endif  /* _FPGAGEN6_TEST_TYPES_H_ */
