// SPDX-License-Identifier: GPL-2.0
/**
 * \file adi_common_error.c
 * \brief Contains common API error handling functions implementations
 *
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
 * ADI common lib Version: 0.0.1.1
 */

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#include "adi_common_error.h"
#include "adi_common_log.h"
#include "adi_platform.h"
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stdio.h>
#endif

void adrv9025_ErrorReport(adi_common_Device_t*    commonDev,
                            adi_common_ErrSources_e errSrc,
                            int32_t                 detErr,
                            int32_t                 actionToRecover,
                            const char*             fileName,
                            const char*             funcName,
                            uint32_t                lineNum,
                            const char*             varName,
                            const char*             customError)
{
    int32_t                actionReport = actionToRecover;
    int32_t                lastAction   = ADI_COMMON_ACT_NO_ACTION;
    adi_common_ErrStruct_t localSave;

    /* Check errorPtr pointer is not null */
    if (commonDev != NULL)
    {
        /* Check if error is an actual error */
        if (detErr > ADI_COMMON_ERR_OK)
        {
            /* Check if the action to recover is a warning or action */
            if (actionReport != ADI_COMMON_ACT_NO_ACTION)
            {
                /* Check if the action to recover is of more importance than the current action
                 * Warnings are always of a lower importance than actions.
                 * if warning then the API is design not to termintate execution see
                 * IF_ERROR_RETURN macro
                 */
                if ((actionReport > commonDev->error.newAction) && (commonDev->error.newAction != ADI_COMMON_ACT_NO_ACTION))
                {
                    /* log the error that has called this function and keep previous error in the error structure */
                    localSave.errormessage = customError;
                    localSave.errFile      = fileName;
                    localSave.errFunc      = funcName;
                    localSave.errLine      = lineNum;
                    localSave.errCode      = detErr;
                    localSave.errSource    = errSrc;
                    localSave.varName      = varName;
                    localSave.newAction    = actionReport;

                    if (commonDev->error.logEnable == ADI_ENABLE)
                    {
                        ADI_ERROR_LOG(commonDev,
                                      localSave);
                    }
                }
                else
                {
                    /* Assign detected action and error to the errors structure */
                    commonDev->error.errSource    = errSrc;
                    commonDev->error.errCode      = detErr;
                    commonDev->error.errLine      = lineNum;
                    commonDev->error.errFunc      = funcName;
                    commonDev->error.errFile      = fileName;
                    commonDev->error.varName      = varName;
                    commonDev->error.errormessage = customError;
                    commonDev->error.lastAction   = commonDev->error.newAction;
                    commonDev->error.newAction    = actionToRecover;

                    if (commonDev->error.logEnable == ADI_ENABLE)
                    {
                        ADI_ERROR_LOG(commonDev,
                                      commonDev->error);
                    }
                }
            }
            else /* actionReport == ADI_COMMON_ACT_NO_ACTION */
            {
                /* Clear error and pass it to the previous action */
                if (commonDev->error.newAction != ADI_COMMON_ACT_NO_ACTION)
                {
                    lastAction = commonDev->error.newAction;
                    adrv9025_ErrorClear(commonDev);
                    commonDev->error.lastAction = lastAction;
                }
            }
        }
    }
}

int32_t adrv9025_ErrorClear(adi_common_Device_t* commonDev)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(commonDev);

    ADI_FUNCTION_ENTRY_LOG(commonDev,
                           ADI_COMMON_LOG_API);

    commonDev->error.errSource    = 0;
    commonDev->error.errCode      = 0;
    commonDev->error.errLine      = 0;
    commonDev->error.errFunc      = "";
    commonDev->error.errFile      = "";
    commonDev->error.varName      = "";
    commonDev->error.errormessage = "";
    commonDev->error.lastAction   = 0;
    commonDev->error.newAction    = 0;

    return recoveryAction;
}
