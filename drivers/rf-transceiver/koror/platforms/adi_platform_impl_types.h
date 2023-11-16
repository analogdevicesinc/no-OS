/*
 * HAL implementors/porters may modify this file to suit the underlying OS
 * and hardware.  For instance to map abstract HAL types to concrete
 * platform/OS-dependent types.
 */

#ifndef __ADI_PLATFORM_IMPL_TYPES_H__
#define __ADI_PLATFORM_IMPL_TYPES_H__

#include <pthread.h>
typedef void* adi_hal_mutex_t;
typedef pthread_t adi_hal_thread_t;
/**
 *  \brief  Default filepath for logfiles if none provided
 *
 *   This should be used in platform implementation of adi_hal_LogFileOpen
 *   to define the default logfile path.
 */
#define ADI_PLATFORM_LOG_PATH_DEFAULT     "adi_default_log.txt"


/**
 *  \brief  Max number of characters for log file
 *
 *   This should be used in platform implementation of adi_hal_LogFileOpen
 *   to define the max filepath length.
 */
#define ADI_HAL_STRING_LENGTH       256U

/**
 *  \brief  Max number of characters per log message
 *
 *   This should be used in platform implementation of adi_hal_LogWrite
 *   to define the number of characters allowed per log message.
 */
#define ADI_HAL_MAX_LOG_LINE        1000U

/**
 *  \brief  Max number of lines in log file
 *
 *   This should be used in platform implementation of adi_hal_LogWrite.
 *   This will ensure log files do not become excessively large during
 *   operation.
 */
#define ADI_HAL_LOG_MAX_NUM_LINES   2000000U

/**
 *  \brief  Size, in bytes, of the FIFO used in the SPI driver.
 *
 *   This is critical to get correct based on hardware SPI driver
 *   if the DUT is placed in streaming mode, since the API SPI
 *   functions use this value to determine when a new address phase
 *   should be inserted into the SPI data stream.
 */
#define ADI_HAL_SPI_FIFO_SIZE   255U


/**
 *  \brief  Default mode for log console output
 *
 *   Can be used in platform implementation of adi_hal_DevHalCfgCreate to 
 *   give a default value for log console mode when configuring logging.
 */
#define ADI_LOG_CONSOLE_DEFAULT_MODE        ADI_LOG_CONSOLE_OFF

typedef struct no_os_hal_fileio
{
    char *data;
    char *ptr, *start, *end;
} CUSTOM_FILE;

#endif

