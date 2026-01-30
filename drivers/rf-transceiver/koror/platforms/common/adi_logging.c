/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/* 
* \file adi_logging.c
*/

#include "adi_logging.h"
#include "adi_library.h"

typedef struct tm tm_t;

ADI_API adi_hal_Err_e adi_LogFileOpen(void* const devHalCfg, const char* const filename)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;
    time_t          seconds     = { 0 };
    tm_t            timeStampRec;
    const tm_t*     timeStamp   = NULL;

    if (NULL != devHalCfg)
    {
        halError = ADI_HAL_ERR_PARAM;

        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            halError = ADI_HAL_ERR_LOG;
            /* Check if Log File is already Open */
            if (NULL != halCfg->logCfg.logfd)
            {
                (void) adi_LogFileClose(devHalCfg);
            }

            if (NULL == halCfg->logCfg.logfd)  /* No Log File Open */
            {
                if (NULL != filename)
                {
                    /* New File Name */
                    (void) ADI_LIBRARY_STRNCPY(halCfg->logCfg.logFileName, filename, (ADI_HAL_STRING_LENGTH-1));
                }

                halCfg->logCfg.logfd = ADI_LIBRARY_FOPEN(halCfg->logCfg.logFileName, "w+");
                if (NULL != halCfg->logCfg.logfd)
                {
                    /* Time Stamp Log File */
                    ADI_LIBRARY_MEMSET(&timeStampRec, 0, sizeof(timeStampRec));
                    (void) ADI_LIBRARY_TIME(&seconds);
                    timeStamp = ADI_LIBRARY_LOCALTIME_R(&seconds, &timeStampRec); /* Returned value points to a shared internal statically allocated object */
                    if (EOF != ADI_LIBRARY_FPRINTF( halCfg->logCfg.logfd,
                                                    "ADI Log File: %s \nCreated: %04d-%02d-%02d %02d:%02d:%02d \nLog Mask: 0x%" PRIX32 "\n\n\n",
                                                    halCfg->logCfg.logFileName,
                                                    timeStamp->tm_year + 1900,
                                                    timeStamp->tm_mon + 1,
                                                    timeStamp->tm_mday,
                                                    timeStamp->tm_hour,
                                                    timeStamp->tm_min,
                                                    timeStamp->tm_sec,
                                                    halCfg->logCfg.logMask))
                    {
                        if (EOF != ADI_LIBRARY_FFLUSH(halCfg->logCfg.logfd))
                        {
                            halCfg->logCfg.currentLineNumber = 0U;
                            halCfg->logCfg.errCount = 0U;
                            halCfg->logCfg.errFlags = 0U;
                            halError = ADI_HAL_ERR_OK;
                        }
                    }
                }
            }
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_OPENING_FILE;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogFileFlush(void* const devHalCfg)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;

    if (NULL != devHalCfg)
    {
        halError = ADI_HAL_ERR_PARAM;

        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            halError = ADI_HAL_ERR_LOG;

            if (NULL != halCfg->logCfg.logfd)
            {
                if (EOF != ADI_LIBRARY_FFLUSH(halCfg->logCfg.logfd))
                {
                    halError = ADI_HAL_ERR_OK;
                }
            }
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_FFLUSH;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogFileClose(void* const devHalCfg)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;

    if (NULL != devHalCfg)
    {
        halError = ADI_HAL_ERR_PARAM;

        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            halError = ADI_HAL_ERR_LOG;

            if (NULL != halCfg->logCfg.logfd)
            {
                if (0 == ADI_LIBRARY_FFLUSH(halCfg->logCfg.logfd))
                {
                    if (EOF != ADI_LIBRARY_FCLOSE(halCfg->logCfg.logfd))
                    {
                        halCfg->logCfg.logfd    = NULL;
                        halError                = ADI_HAL_ERR_OK;
                    }
                }
            }
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_CLOSING_FILE;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogLevelSet(void* const devHalCfg, const uint32_t logMask)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;
    time_t          seconds     = { 0 };
    tm_t            timeStampRec;
    const tm_t*     timeStamp   = NULL;

    if (NULL != devHalCfg)
    {
        halError    = ADI_HAL_ERR_PARAM;
        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;
        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            halError                = ADI_HAL_ERR_LOG;
            halCfg->logCfg.logMask  = logMask;

            if (NULL != halCfg->logCfg.logfd)
            {
                /* Time Stamp Log File with Log Mask */
                ADI_LIBRARY_MEMSET(&timeStampRec, 0, sizeof(timeStampRec));
                (void) ADI_LIBRARY_TIME(&seconds);
                timeStamp = ADI_LIBRARY_LOCALTIME_R(&seconds, &timeStampRec); /* Returned value points to a shared internal statically allocated object */
                if (EOF != ADI_LIBRARY_FPRINTF(halCfg->logCfg.logfd,
                                               "%04d-%02d-%02d %02d:%02d:%02d: Log Mask: 0x%" PRIX32 "\n",
                                               timeStamp->tm_year + 1900,
                                               timeStamp->tm_mon + 1,
                                               timeStamp->tm_mday,
                                               timeStamp->tm_hour,
                                               timeStamp->tm_min,
                                               timeStamp->tm_sec,
                                               halCfg->logCfg.logMask))
                {
                    if (EOF != ADI_LIBRARY_FFLUSH(halCfg->logCfg.logfd))
                    {
                        halError = ADI_HAL_ERR_OK;
                    }
                }
            }
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_LEVEL;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogLevelGet(void* const devHalCfg, uint32_t* const logMask)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;

    if ((NULL != devHalCfg) &&
        (NULL != logMask))
    {
        halError    = ADI_HAL_ERR_PARAM;
        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;
        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            *logMask    = halCfg->logCfg.logMask;
            halError    = ADI_HAL_ERR_OK;
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_LEVEL;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogStatusGet( void* const                     devHalCfg,
                                        adi_hal_LogStatusGet_t* const   logStatus)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;

    if ((NULL != devHalCfg) &&
        (NULL != logStatus))
    {
        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        (void) ADI_LIBRARY_MEMSET(logStatus, 0, sizeof(adi_hal_LogStatusGet_t));



        /* Copy Data */
        logStatus->interfaceEnabled     = halCfg->logCfg.interfaceEnabled;
        logStatus->logMask              = halCfg->logCfg.logMask;
        logStatus->logFileName          = halCfg->logCfg.logFileName;
        logStatus->errCount             = halCfg->logCfg.errCount;
        logStatus->errFlags             = halCfg->logCfg.errFlags;
        logStatus->currentLineNumber    = halCfg->logCfg.currentLineNumber;

        /* Log Error Information is relative to previous call */
        halCfg->logCfg.errFlags = (uint32_t) ADI_LOG_ERR_OK;
        halCfg->logCfg.errCount = 0U;
        halError                = ADI_HAL_ERR_OK;
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogConsoleSet(void* const devHalCfg, const adi_hal_LogConsole_e logConsoleFlag)
{
    adi_hal_Err_e   halError    = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg      = NULL;

    if (NULL != devHalCfg)
    {
        halError = ADI_HAL_ERR_PARAM;

        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        if (0U != halCfg->logCfg.interfaceEnabled)
        {
            halCfg->logCfg.logConsole   = logConsoleFlag;
            halError                    = ADI_HAL_ERR_OK;
        }

        if (ADI_HAL_ERR_OK != halError)
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_LEVEL;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogWrite( void* const                 devHalCfg,
                                    const adi_hal_LogLevel_e    logLevel,
                                    const uint8_t               indent,
                                    const char* const           comment,
                                    va_list                     argp)
{
    adi_hal_Err_e   halError                        = ADI_HAL_ERR_NULL_PTR;
    adi_hal_Cfg_t*  halCfg                          = NULL;
    char            logMsg[ADI_HAL_MAX_LOG_LINE]    = { 0 };
    const char*     logMsgLevel                     = NULL;
    char*           logMsgPtr                       = NULL;
    time_t          seconds                         = { 0 };
    const tm_t*     timeStamp                       = NULL;
    tm_t            timeStampRec;

    if (NULL != devHalCfg)
    {
        halError = ADI_HAL_ERR_PARAM;

        /* Assume ADI HAL Structure */
        halCfg = (adi_hal_Cfg_t*) devHalCfg;

        if ((0U                 != halCfg->logCfg.interfaceEnabled) &&  /* Interface Enabled */
            (ADI_HAL_LOG_NONE   != halCfg->logCfg.logMask)          &&  /* Logging Mask Set */
            (ADI_HAL_LOG_ALL    > logLevel)                         &&  /* Valid Logging Level */
            (NULL               != halCfg->logCfg.logfd))               /* File is Open */
        {
            halError = ADI_HAL_ERR_LOG;

            if (halCfg->logCfg.currentLineNumber >= ADI_HAL_LOG_MAX_NUM_LINES)
            {
                /* Keep Existing Log Data in File until there is something to overwrite with */
                rewind(halCfg->logCfg.logfd);
                if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
                {
                    ++halCfg->logCfg.errCount;
                }
                halCfg->logCfg.errFlags |= ADI_LOG_ERR_REWIND;
                halCfg->logCfg.currentLineNumber = 0U;
            }

            /* Check Message Logging Level against the Log Level Mask */
            if ((halCfg->logCfg.logMask & ADI_HAL_LOG_MSG) && (logLevel == ADI_HAL_LOG_MSG))
            {
                logMsgLevel = "MESSAGE:      ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_WARN) && (logLevel == ADI_HAL_LOG_WARN))
            {
                logMsgLevel = "WARNING:      ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_ERR) && (logLevel == ADI_HAL_LOG_ERR))
            {
                logMsgLevel = "ERROR:        ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_API) && (logLevel == ADI_HAL_LOG_API))
            {
                logMsgLevel = "API_PUBLIC:   ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_BF) && (logLevel == ADI_HAL_LOG_BF))
            {
                logMsgLevel = "API_BITFIELD: ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_HAL) && (logLevel == ADI_HAL_LOG_HAL))
            {
                logMsgLevel = "HAL:          ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_SPI) && (logLevel == ADI_HAL_LOG_SPI))
            {
                logMsgLevel = "SPI:          ";
            }
            else if ((halCfg->logCfg.logMask & ADI_HAL_LOG_API_PRIV) && (logLevel == ADI_HAL_LOG_API_PRIV))
            {
                logMsgLevel = "API_PRIVATE:  ";
            }
            else
            {
                /* Do Nothing; Logging Level not set in the Mask */
                halError = ADI_HAL_ERR_PARAM;
            }

            if (ADI_HAL_ERR_PARAM != halError)
            {
                /* Input Parameters OK at this Point; Logging Level Enabled */
                halError = ADI_HAL_ERR_LOG;
                ADI_LIBRARY_MEMSET(&timeStampRec, 0, sizeof(timeStampRec));
                (void) ADI_LIBRARY_TIME(&seconds);
                timeStamp = ADI_LIBRARY_LOCALTIME_R(&seconds, &timeStampRec); /* Returned value points to a shared internal statically allocated object */
                if (-1 < ADI_LIBRARY_SNPRINTF(  logMsg,
                                                ADI_HAL_MAX_LOG_LINE,
                                                "%04d-%02d-%02d %02d:%02d:%02d: %s",
                                                timeStamp->tm_year + 1900,
                                                timeStamp->tm_mon + 1,
                                                timeStamp->tm_mday,
                                                timeStamp->tm_hour,
                                                timeStamp->tm_min,
                                                timeStamp->tm_sec,
                                                logMsgLevel))
                {
                    /* Indentation */
                    logMsgPtr = logMsg + ADI_LIBRARY_STRLEN(logMsg);
                    if (indent > 0U)
                    {
                        (void) ADI_LIBRARY_MEMSET(logMsgPtr, ' ', indent);
                        logMsgPtr[indent] = '\0';
                    }

                    logMsgPtr = logMsg + ADI_LIBRARY_STRLEN(logMsg);

                    if (-1 < ADI_LIBRARY_VSNPRINTF( (logMsgPtr),                                            /* Pointer to End of Log Message Header */
                                                    (ADI_HAL_MAX_LOG_LINE - ADI_LIBRARY_STRLEN(logMsg)),    /* Adjusted Line Limit based on Log Message Header */
                                                    comment,
                                                    argp))                                                  /* Append Incoming Log Message */
                    {
                        /* Logging To File */
                        if (-1 < ADI_LIBRARY_FPRINTF(halCfg->logCfg.logfd, "%s\n", logMsg))
                        {
                            if (0 == ADI_LIBRARY_FFLUSH(halCfg->logCfg.logfd))
                            {
                                /* Log Information may be still buffered at this point */
                                ++halCfg->logCfg.currentLineNumber;
                                halError = ADI_HAL_ERR_OK;
                            }
                        }

                        if (0U != halCfg->logCfg.logConsole)
                        {
                            /* Writing to STDOUT; API Logic is geared for Writing to File */
                            (void) ADI_LIBRARY_FPRINTF(stdout, "%s\n", logMsg);
                            (void) ADI_LIBRARY_FFLUSH(stdout);
                        }
                    }
                }
            }
        }

        if ((ADI_HAL_ERR_OK     != halError)    &&  /* Log Write OK; Not an Error */
            (ADI_HAL_ERR_PARAM  != halError))       /* Log Writing Not Enabled OR Log Level is not Enabled in the Mask; Not an Error */
        {
            if (LOG_ERR_COUNT_LIMIT > halCfg->logCfg.errCount)
            {
                ++halCfg->logCfg.errCount;
            }
            halCfg->logCfg.errFlags |= ADI_LOG_ERR_WRITE;
        }
    }

    return halError;
}

ADI_API adi_hal_Err_e adi_LogWrite1(void* const                 devHalCfg,
                                    const adi_hal_LogLevel_e    logLevel,
                                    const char* const           comment,
                                    ...)
{
    adi_hal_Err_e   halError        = ADI_HAL_ERR_NULL_PTR;
    va_list         empty_va_list;

    if ((NULL != devHalCfg) &&
        (NULL != comment))
    {
        ADI_LIBRARY_VA_START(empty_va_list, comment);
        /* Logging Feature will report Errors to own Data Structure */
        (void) adi_LogWrite(devHalCfg, logLevel, 0, comment, empty_va_list);
        ADI_LIBRARY_VA_END(empty_va_list);

        halError = ADI_HAL_ERR_OK;
    }

    return halError;
}
