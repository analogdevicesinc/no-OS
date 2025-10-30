/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adi_adrv903x_user.h
 * \brief Contains ADRV903X API macro definitions for user to override
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADI_ADRV903X_USER_H_
#define _ADI_ADRV903X_USER_H_

#ifndef ADI_ADRV903X_LOGGING
#define ADI_ADRV903X_LOGGING     (uint32_t)     (   ADI_HAL_LOG_MSG     | \
                                                    ADI_HAL_LOG_WARN    | \
                                                    ADI_HAL_LOG_ERR     | \
                                                    ADI_HAL_LOG_API)         /* LogLevel Set to API, Messages, Warnings, Error */
#endif /* !ADI_ADRV903X_LOGGING */

/*
 *****************************************
 * Interface Macros
 ******************************************
 */
#ifndef ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
    #define ADI_ADRV903X_CPU_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES         1024U    /*!< Please ensure that the CPU Binary Image Size is perfectly divisible by the chunk size */
#endif  /* CPU Binary Image Chunk Loading Size */

#ifndef ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
#define ADI_ADRV903X_CPU_PROFILE_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES     1024U    /*!< Please ensure that the CPU Profile Binary Image Size is perfectly divisible by the chunk size */
#endif  /* CPU Profile Binary Image Chunk Loading Size */
 
#ifndef ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES
    #define ADI_ADRV903X_STREAM_BINARY_IMAGE_LOAD_CHUNK_SIZE_BYTES      1024U    /*!< Please ensure that the Stream Processor Binary Image Size is perfectly divisible by the chunk size */
#endif  /* Steam Processor Binary Image Chunk Loading Size */

#ifndef ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX
    #define ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX                       256U   /*!< Cache value: up to 1024, multiple of 4 */
#endif

#ifndef ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE
    #define ADI_ADRV903X_MEM_DUMP_CHUNK_SIZE                            256U   /*!< Cache value: up to 1024, multiple of 4 */
#endif

/**
 * Macro for compiler-specific structure packing directive.
 * Define this if your compiler is not supported in
 * devices/adrv903x/public/adi_adrv903x_platform_pack.h.
 * See adi_adrv903x_platform_pack.h for examples. To use 
 * this macro from the list in adi_adrv903x_plaform_pack.h
 * remove this definition (including ifndef)
 */
#ifndef ADI_ADRV903X_PACKED
    #define ADI_ADRV903X_PACKED(d) _Pragma("pack(1)") d _Pragma("pack()")
#endif

/*
 *****************************************
 * Rx gain tables
 ******************************************
 */
#ifndef ADI_ADRV903X_RX_GAIN_TABLE_ARR_MAX
    #define ADI_ADRV903X_RX_GAIN_TABLE_ARR_MAX          1U    /*!< Number of Rx Gain Tables to be loaded */
#endif  /* Rx Gain Table Array Size */

#ifndef ADI_ADRV903X_MAX_GAIN_TABLE_INDEX
    #define ADI_ADRV903X_MAX_GAIN_TABLE_INDEX           255U
#endif


#ifndef ADI_ADRV903X_RADIOCTRL_RANGE_CHECK
    #define ADI_ADRV903X_RADIOCTRL_RANGE_CHECK  1
#endif

/**
 * Macro for defining platform-specific byte ordering (endianness).
 * Define this if your compiler is not supported in
 * devices/adrv903x/private/adrv903x_platform_byte_order.h.
 * See adrv903x_platform_byte_order.h for examples.
 */
/*
#ifndef ADI_ADRV903X_LITTLE_ENDIAN
#define ADI_ADRV903X_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#endif
*/

#ifndef ADI_ADRV903X_SENDCPUCMD_TIMEOUT_US
#define ADI_ADRV903X_SENDCPUCMD_TIMEOUT_US         2000000
#endif

#ifndef ADI_ADRV903X_SENDCPUCMD_INTERVAL_US
#define ADI_ADRV903X_SENDCPUCMD_INTERVAL_US           1000
#endif

#ifndef ADI_ADRV903X_READCPURESP_TIMEOUT_US
#define ADI_ADRV903X_READCPURESP_TIMEOUT_US         1000000
#endif

#ifndef ADI_ADRV903X_READCPURESP_INTERVAL_US
#define ADI_ADRV903X_READCPURESP_INTERVAL_US           1000
#endif

#ifndef ADI_ADRV903X_INITCALSWAIT_INTERVAL_US
#define ADI_ADRV903X_INITCALSWAIT_INTERVAL_US          1000
#endif

#ifndef ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US
#define ADI_ADRV903X_TRACKCALDISABLE_TIMEOUT_US      4000000
#endif

#ifndef ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US
#define ADI_ADRV903X_TRACKCALDISABLE_INTERVAL_US      100000
#endif

#ifndef ADI_ADRV903X_RUNEYESWEEP_TIMEOUT_US
#define ADI_ADRV903X_RUNEYESWEEP_TIMEOUT_US        180000000
#endif

#ifndef ADI_ADRV903X_RUNEYESWEEP_TIMEOUT_ONE_HOUR_IN_US
#define ADI_ADRV903X_RUNEYESWEEP_TIMEOUT_ONE_HOUR_IN_US        (60UL * 60UL * 1000000UL)
#endif

#ifndef ADI_ADRV903X_RUNEYESWEEP_INTERVAL_US
#define ADI_ADRV903X_RUNEYESWEEP_INTERVAL_US         10000
#endif

#ifndef ADI_ADRV903X_GETCPUBOOTUP_TIMEOUT_US
#define ADI_ADRV903X_GETCPUBOOTUP_TIMEOUT_US       120000000
#endif

#ifndef ADI_ADRV903X_GETCPUBOOTUP_INTERVAL_US
#define ADI_ADRV903X_GETCPUBOOTUP_INTERVAL_US           1000
#endif

#ifndef ADI_ADRV903X_WRITECPUEXCEPTION_TIMEOUT_US
#define ADI_ADRV903X_WRITECPUEXCEPTION_TIMEOUT_US    5000000
#endif

#ifndef ADI_ADRV903X_WRITECPUEXCEPTION_INTERVAL_US
#define ADI_ADRV903X_WRITECPUEXCEPTION_INTERVAL_US      1000
#endif

#ifndef ADI_ADRV903X_MIN_GAIN_TABLE_INDEX
    #define ADI_ADRV903X_MIN_GAIN_TABLE_INDEX             0
#endif

#ifndef ADI_ADRV903X_START_RX_GAIN_INDEX
    #define ADI_ADRV903X_START_RX_GAIN_INDEX              255
#endif

#ifndef ADI_ADRV903X_MIN_RX_GAIN_TABLE_INDEX
    #define ADI_ADRV903X_MIN_RX_GAIN_TABLE_INDEX          0
#endif

#ifndef ADI_ADRV903X_FACTORY_TEMPERATURE_TEST
    #define ADI_ADRV903X_FACTORY_TEMPERATURE_TEST         70
#endif

#ifndef ADI_ADRV903X_FACTORY_TEMPERATURE_MARGIN
    #define ADI_ADRV903X_FACTORY_TEMPERATURE_MARGIN        3
#endif

#ifndef ADI_ADRV903X_JRXREPAIR_APPLY_WAIT_US
    #define ADI_ADRV903X_JRXREPAIR_APPLY_WAIT_US           50000U
#endif

#ifndef ADI_ADRV903X_JRXREPAIR_TEST_WAIT_US
    #define ADI_ADRV903X_JRXREPAIR_TEST_WAIT_US            200000U
#endif

#ifndef ADI_ADRV903X_RX_RANGE_CHECK
    /* TODO: Turn on when current UT brings Trx CPU to a mode
     * that is compatible with RxTxEnableSetRangeCheck */
    #define ADI_ADRV903X_RX_RANGE_CHECK  1
#endif


#ifndef ADI_ADRV903X_API_VERSION_RANGE_CHECK
     #define ADI_ADRV903X_API_VERSION_RANGE_CHECK  0
#endif

/**
 * \brief Enable Flag for Spi Verify Check in PreMcsInit_NonBroadcast
 */

#ifndef ADI_ADRV903X_SPI_VERIFY
    #define ADI_ADRV903X_SPI_VERIFY  0
#endif


/**
 * \brief   Enable Flag for SPI Pins Hysteresis
 */


#ifndef ADI_ADRV903X_SPI_HYSTERESIS 
    #define ADI_ADRV903X_SPI_HYSTERESIS  0
#endif

/**
 * \brief Enable Flag for SPI Pins Hysteresis Debug
 */
#ifndef ADI_ADRV903X_SPI_HYSTERESIS_DEBUG
    #define ADI_ADRV903X_SPI_HYSTERESIS_DEBUG  0
#endif


/**
 * \brief Drive Strength Configuration for ResetB Pin
 */

#ifndef ADI_ADRV903X_RESETB_PIN_DRIVE_STRENGTH
    #define ADI_ADRV903X_RESETB_PIN_DRIVE_STRENGTH  0
#endif


/**
 * \brief Enable Flag for ResetB Pin Hysteresis
 */

#ifndef ADI_ADRV903X_RESETB_PIN_HYSTERESIS
    #define ADI_ADRV903X_RESETB_PIN_HYSTERESIS  0
#endif

#endif

