
/**
* \file
* \brief Contains ADI Transceiver Logging functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include <stdarg.h>
#include "adi_platform.h"
#include "adi_common_error.h"
#include "adi_common_types.h"


void adi_common_LogWrite(adi_common_Device_t *commonDev, uint32_t logLevel, const char* comment, ...)
{
    int32_t halError = 0;
    if (commonDev->devHalInfo == NULL)
    {
        halError = ADI_COMMON_ERR_NULL_PARAM;
    }
    else
    {
        va_list argp;
        va_start(argp, comment);
        halError = adi_hal_LogWrite(commonDev->devHalInfo, (int32_t)logLevel, comment, argp);
        va_end(argp);
    }

    if (halError != ADI_COMMON_ERR_OK)
    {
        /* reentrant function call, ADI_ERROR_REPORT calls adi_common_LogWrite 
         * At this point we should disable logging as it is not available, but keep reporting errors/warnings */
        commonDev->error.logEnable = false;
        ADI_ERROR_REPORT(commonDev, ADI_COMMON_SRC_LOG, halError, ADI_COMMON_ACT_WARN_RESET_LOG, NULL, "Logging failed");
    }
}
