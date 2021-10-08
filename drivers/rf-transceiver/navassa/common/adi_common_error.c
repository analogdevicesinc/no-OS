/**
 * \file
 * \brief Contains common API error handling functions implementations
 *
 * These functions are public to the customer for getting more details on
 * errors and debugging.
 *
 * ADI common lib Version: $ADI_COMMON_LIB_VERSION$
 */

/**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdlib.h>
#endif

#include "adi_common_error.h"
#include "adi_common_log.h"

void adi_common_ErrorReport(adi_common_Device_t *commonDev,
                            adi_common_ErrSources_e errSrc,
                            int32_t detErr,
                            int32_t actionToRecover,
                            const char* fileName,
                            const char* funcName,
                            uint32_t lineNum,
                            const char* varName,
                            const char* customError)
{
    int32_t actionReport = actionToRecover;
    int32_t lastAction = ADI_COMMON_ACT_NO_ACTION;
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
                 * Warnings are allways of a lower importance than actions.
                 * if warning then the API is design not to termintate execution see
                 * IF_ERROR_RETURN macro
                 */
                if ((actionReport > commonDev->error.newAction) && (commonDev->error.newAction != ADI_COMMON_ACT_NO_ACTION))
                {
                    /* log the error that has called this function and keep previous error in the error structure */
                    long ii;
                    for (ii = 0; ADI_ERROR_MSG_MAX_LEN > ii; ii++)
                    {
                        localSave.errormessage[ii] = customError[ii];
                        if ('\0' == customError[ii])
                        {
                            break;
                        }
                    }
                    localSave.errFile = fileName;
                    localSave.errFunc = funcName;
                    localSave.errLine = lineNum;
                    localSave.errCode = detErr;
                    localSave.errSource = errSrc;
                    localSave.varName = varName;
                    localSave.newAction = actionReport;

                    if (commonDev->error.logEnable == true)
                    {
                        ADI_ERROR_LOG(commonDev, localSave);
                    }

                    actionReport = commonDev->error.newAction;
                }
                else
                {
                    /* Assign detected action and error to the errors structure */
                    long ii;
                    for (ii = 0; ADI_ERROR_MSG_MAX_LEN > ii; ii++)
                    {
                        commonDev->error.errormessage[ii] = customError[ii];
                        if ('\0' == customError[ii])
                        {
                            break;
                        }
                    }
                    commonDev->error.errSource = errSrc;
                    commonDev->error.errCode = detErr;
                    commonDev->error.errLine = lineNum;
                    commonDev->error.errFunc = funcName;
                    commonDev->error.errFile = fileName;
                    commonDev->error.varName = varName;
                    commonDev->error.lastAction = commonDev->error.newAction;
                    commonDev->error.newAction = actionToRecover;

                    if (commonDev->error.logEnable == true)
                    {
                        ADI_ERROR_LOG(commonDev, commonDev->error);
                    }
                }
            }
            else /* actionReport == ADI_COMMON_ACT_NO_ACTION */
            {
                /* Clear error and pass it to the previous action */
                if (commonDev->error.newAction != ADI_COMMON_ACT_NO_ACTION)
                {
                    lastAction = commonDev->error.newAction;
                    adi_common_ErrorClear(commonDev);
                    commonDev->error.lastAction = lastAction;
                }
            }
        }
    }
}


int32_t adi_common_ErrorClear(adi_common_Device_t *commonDev)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(commonDev);
    
    commonDev->error.errormessage[0] = '\0';
    commonDev->error.errSource = 0;
    commonDev->error.errCode = 0;
    commonDev->error.errLine = 0;
    commonDev->error.errFunc = "";
    commonDev->error.errFile = "";
    commonDev->error.varName = "";
    commonDev->error.lastAction = 0;
    commonDev->error.newAction = 0;

    return ADI_COMMON_ACT_NO_ACTION;
}
