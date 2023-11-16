/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
/**
* \file fpgagen6_test.c
*
* \brief    Test Interface Functions for the FPGAGEN6 Device
*
* FPGA API Version: 2.9.0.4
*/
#include "../../private/include/fpgagen6_test.h"

#include "adi_fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PRIVATE_TEST

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_SwTestGet(adi_fpgagen6_Device_t* const    device,
                                                    fpgagen6_TestInfo_t* const      testInfo)
{
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, testInfo);

    /* Inject Test Data Into FPGA Device Structure */

    testInfo->swTest = device->devStateInfo.swTest;

    testInfo->testLoopbackFlag = device->devStateInfo.testLoopbackFlag;

    testInfo->interfaceType = device->devStateInfo.interfaceType;

    return ADI_FPGAGEN6_ERR_ACT_NONE;
}

ADI_API adi_fpgagen6_ErrAction_e fpgagen6_SwTestSet(adi_fpgagen6_Device_t* const        device,
                                                    const fpgagen6_TestInfo_t* const    testInfo)
{
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, testInfo);

    /* Inject Test Data Into FPGA Device Structure */

    device->devStateInfo.swTest = testInfo->swTest;

    device->devStateInfo.testLoopbackFlag = testInfo->testLoopbackFlag;

    device->devStateInfo.interfaceType = testInfo->interfaceType;

    return ADI_FPGAGEN6_ERR_ACT_NONE;
}