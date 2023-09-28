// SPDX-License-Identifier: GPL-2.0
/**
* \file adi_common_log.c
* \brief Contains ADI Transceiver Logging functions
*        Analog Devices maintains and provides updates to this code layer.
*        The end user should not modify this file or any code in this directory.
*
* ADI common lib Version: 0.0.1.1
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADI API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/* Intermediate platform HAL layer maintained by Analog Devices */
#include "adi_common_hal.h"
#include "adi_platform.h"
#include "adi_common_error_types.h"
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdarg.h>
#endif

void adrv9025_LogWrite(adi_common_Device_t*  commonDev,
                         adi_common_LogLevel_e logLevel,
                         const char*           comment,
                         ...)
{
    int32_t halError  = (int32_t)ADI_COMMON_HAL_OK;
    uint8_t logStatus = commonDev->error.logEnable;

    if (commonDev->devHalInfo == NULL)
    {
        halError = ADI_COMMON_HAL_GEN_SW;
    }
    else
    {
        va_list argp;
        va_start(argp,
                 comment);
        halError = adi_hal_LogWrite(commonDev->devHalInfo,
                                    (int32_t)logLevel,
                                    comment,
                                    argp);
        va_end(argp);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        /* reentrant function call, ADI_ERROR_REPORT calls adrv9025_LogWrite
         * At this point we should disable logging as it is not available, but keep reporting errors/warnings
         *
         * Todo:
         * LogLevel should be set to NONE
         * LogLevel should be reset to the particular level once the HAL layer is reset or fixed
         *
         * Figure out when to re-enable commonDev->error.logEnable
         *
         * Read state of log enable and restore it
         */
        commonDev->error.logEnable = ADI_DISABLE;
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_LOG,
                         halError,
                         ADI_COMMON_ACT_WARN_RESET_LOG,
                         NULL,
                         "Logging failure");
        commonDev->error.logEnable = logStatus;
    }
}

void adi_common_LogLevelSet(adi_common_Device_t* commonDev,
                            int32_t              halLogLevel)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev->devHalInfo == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
    }
    else
    {
        halError = adi_hal_LogLevelSet(commonDev->devHalInfo,
                                       halLogLevel);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_LOG,
                         halError,
                         ADI_COMMON_ACT_WARN_RESET_LOG,
                         NULL,
                         "Log level set error");
    }
}

void adi_common_LogLevelGet(adi_common_Device_t* commonDev,
                            int32_t*             halLogLevel)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if ((commonDev->devHalInfo == NULL) || (halLogLevel == NULL))
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
    }
    else
    {
        halError = adi_hal_LogLevelGet(commonDev->devHalInfo,
                                       halLogLevel);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_LOG,
                         halError,
                         ADI_COMMON_ACT_WARN_RESET_LOG,
                         NULL,
                         "Log level get error");
    }
}

int32_t adi_common_LogFileOpen(adi_common_Device_t* commonDev,
                               char*                fileName)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev->devHalInfo == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
    }
    else
    {
        halError = adi_hal_LogFileOpen(commonDev->devHalInfo,
                                       fileName);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_LOG,
                         halError,
                         ADI_COMMON_ACT_WARN_RESET_LOG,
                         NULL,
                         "Log file Open error");
    }

    return commonDev->error.newAction;
}

int32_t adi_common_LogFileClose(adi_common_Device_t* commonDev)
{
    int32_t halError = (int32_t)ADI_COMMON_HAL_OK;

    if (commonDev->devHalInfo == NULL)
    {
        halError = (int32_t)ADI_COMMON_HAL_GEN_SW;
    }
    else
    {
        halError = adi_hal_LogFileClose(commonDev->devHalInfo);
    }

    if (halError != (int32_t)ADI_COMMON_HAL_OK)
    {
        ADI_ERROR_REPORT(commonDev,
                         ADI_COMMON_SRC_LOG,
                         halError,
                         ADI_COMMON_ACT_WARN_RESET_LOG,
                         NULL,
                         "Log file Close error");
    }

    return commonDev->error.newAction;
}
