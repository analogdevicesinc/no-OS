/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file fpgagen6_test.h
*
* \brief
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _FPGAGEN6_TEST_H_
#define _FPGAGEN6_TEST_H_

#include "fpgagen6_test_types.h"

#include "adi_fpgagen6_data_interface_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Service for Loading Test Information into FPGA Device
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[out] testInfo  Pointer to Test Info structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_SwTestGet(adi_fpgagen6_Device_t* const    device,
                                                    fpgagen6_TestInfo_t* const      testInfo);

/**
* \brief Service for Loading Test Information into FPGA Device
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] testInfo   Pointer to Test Info structure
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_SwTestSet(adi_fpgagen6_Device_t* const        device,
                                                    const fpgagen6_TestInfo_t* const    testInfo);

#endif  /* _FPGAGEN6_TEST_H_ */
