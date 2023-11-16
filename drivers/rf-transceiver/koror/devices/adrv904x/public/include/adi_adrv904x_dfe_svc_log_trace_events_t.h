/**
 * Copyright 2020 - 2023 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_log_trace_events_t.h
 * \brief Contains data types for SDK Log Service
 *
 * SDK API Version: 2.9.0.4
 */

#ifndef ADRV904X_DFE_SVC_LOG_TRACE_EVENTS_T_H_
#define ADRV904X_DFE_SVC_LOG_TRACE_EVENTS_T_H_

#include "adi_adrv904x_dfe_svc_err_codes_t.h"

#define ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_SDK_MIN    (0x00u)      /* Sdk event min range */
#define ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_SDK_MAX    (0x7Fu)      /* Sdk event max range */

#define ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MIN    (0x80u)      /* App event min range */
#define ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MAX    (0xFFu)      /* App event max range */

/**
 * \brief Enumerated list of different log messages types that the application and
 *  different SDK components can log.
 *
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_DEBUG    = 0x1UL,    /*!< Debug Message logging, no action required */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_INFO     = 0x2UL,    /*!< Informative, no action required. Error code parameter will be ignore.  Info messages will not be logged to the telemetry*/
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_WARNING  = 0x4UL,    /*!< Some attention required. Warning messages will not be logged to the telemetry */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_CRITICAL = 0x8UL,    /*!< Can be continued after handling */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_FATAL    = 0x10UL    /*!< Cannot continue, needs reboot */
} adi_adrv904x_DfeSvcLogTraceLevel_e;

typedef uint32_t adi_adrv904x_DfeSvcLogTraceLevel_t;

/* All Logging Enabled in Log Mask*/
#define ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_ALL    (ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_DEBUG | ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_INFO | ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_WARNING | \
                                        ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_CRITICAL | ADI_ADRV904X_DFE_SVC_LOG_TRACE_LEVEL_FATAL)

/**
 * \brief Error Log pairing information memory dump
 */
typedef struct
{
    adi_adrv904x_DfeSvcErrCode_t       errLogCode;      /*!< error Code */
    adi_adrv904x_DfeSvcLogTraceLevel_t errLogLevel;     /*!< error LogLevel */
    uint64_t                errLogTimeStamp; /*!< error TimeStamp  */
} adi_adrv904x_DfeSvcLogTraceErrLogData_t;


/**
 * \brief Error Log information memory dump
 */
typedef struct
{
    adi_adrv904x_DfeSvcLogTraceErrLogData_t comObjErr;           /*!< Common Object Error */
    adi_adrv904x_DfeSvcLogTraceErrLogData_t svcLayerErr;         /*!< Service Layer */
    adi_adrv904x_DfeSvcLogTraceErrLogData_t drvObjErr;           /*!< Driver objects */
    adi_adrv904x_DfeSvcLogTraceErrLogData_t configObjErr;        /*!< Configuration objects */
    adi_adrv904x_DfeSvcLogTraceErrLogData_t calObjErr;           /*!< Calibration objects */
    adi_adrv904x_DfeSvcLogTraceErrLogData_t sdkAppObjErr;        /*!< SDK Application objects */
} adi_adrv904x_DfeSvcLogTraceErrLog_t;


/**
 * \brief Typedef representing the event ID for log tracing / telemetry
 * - All event IDs must be equal or less than 255
 * - Events used by the SDK must be in range 0..ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MIN-1  (0..127)
 * - Events used by the APP must be in range ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MIN..ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MAX (128..255)
 */

typedef uint8_t adi_adrv904x_DfeSvcLogTraceEvent_t;

/*
 * The memdump tool parses this header file as a configuration file.
 * The parser assumes that each telemetry "channel" is defined
 * on a line of its own with the following format:
 *
 * [ws_4]ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_<XXXX>, <ws>|*<ws>ZZ<ws>*|
 *
 * where:
 * - [ws_x] indicates x number whitespaces, ws_4 means 4 spaces is needed
 * - <ws> indicates optional whitespace
 * - <XXXX> is the telemetry channel name, which must be prefixed with "ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_"
 * - <ZZ> is the telemetry format (default EVENT), other valid telemetry format are "UINT32" and "WIRE"
 * - |* and *| are the start/end of a block comment (really forward slashes)
 *
 * Example:
 * #define ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_NAME      |* WIRE *|
 * Pay attention that one Event Id can only be used in one kind of data and use corresponding data write interface:
 *     For "WIRE" type, use the interface "adi_adrv904x_DfeSvcLogTraceAppWire"
 *     For "EVENT" type, use the interface "adi_adrv904x_DfeSvcLogTraceAppEvent"
 *     For "UINT32" type, use the interface "adi_adrv904x_DfeSvcLogTraceAppEvent32"
 */

typedef enum
{
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_START = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_SDK_MIN,

    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_ERROR = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_SDK_MIN,      /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DRV_INT,                                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH0,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH1,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH2,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH3,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH4,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH5,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH6,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CAP_PERIOD_CH7,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH0,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH1,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH2,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH3,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH4,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH5,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH6,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_ABORT_CH7,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH0,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH1,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH2,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH3,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH4,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH5,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH6,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_ABORT_CH7,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH0,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH1,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH2,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH3,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH4,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH5,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH6,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_START_CH7,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH0,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH1,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH2,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH3,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH4,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH5,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH6,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_START_CH7,                                /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH0,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH1,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH2,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH3,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH4,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH5,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH6,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_DATA_CAP_DONE_CH7,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH0,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH1,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH2,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH3,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH4,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH5,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH6,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_POWER_CAP_DONE_CH7,                                 /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CMD_ACK,                                            /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_CMD_INC,                                            /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_APP_CMD_ACK,                                        /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_APP_CMD_INC,                                        /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_MAILBOX_INTERRUPT,                                  /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_WATCHDOG_1,                                         /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_WATCHDOG_2,                                         /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_NMI_EXCEPTION,                                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_SYNC_EXCEPTION,                                     /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_ENTER,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_ENTER_STACK_START,                           /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_ENTER_STACK_PTR,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_EXIT,                                        /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_EXIT_STACK_START,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_THREAD_EXIT_STACK_PTR,                              /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_ISR_ENTER,                                          /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_ISR_EXIT,                                           /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_ISR_INT_ID,                                         /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_ASSERTION,                                          /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_TRACE_CYCLE_SYNC,                                   /* UINT32 */

    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_SDK_END   = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_SDK_MAX,

    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_START = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MIN,

    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_ERROR = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MIN, /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH0,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH1,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH2,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH3,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH4,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH5,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH6,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CLGC_CAP_REQ_CH7,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH0,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH1,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH2,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH3,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH4,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH5,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH6,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAP_REQ_CH7,                               /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH0,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH1,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH2,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH3,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH4,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH5,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH6,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_PROC_STATE_CH7,                            /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH0,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH1,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH2,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH3,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH4,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH5,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH6,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_RUN_CH7,                                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH0,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH1,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH2,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH3,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH4,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH5,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH6,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_RUN_CLEAR_CH7,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH0,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH1,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH2,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH3,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH4,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH5,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH6,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_EXIT_FLAG_CTRL_CLEAR_CH7,                      /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH0,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH1,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH2,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH3,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH4,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH5,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH6,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_WAIT_CH7,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH0,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH1,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH2,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH3,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH4,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH5,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH6,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_GRAB_CH7,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH0,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH1,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH2,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH3,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH4,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH5,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH6,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_SEM_POST_CH7,                              /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH0,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH1,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH2,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH3,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH4,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH5,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH6,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_DPD_CAL_TIMER_CH7,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAL_STATUS_CMD_RECEIVE,                        /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAL_STATUS_CMD_DONE,                           /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAL_CTRL_CMD_RECEIVE,                          /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAL_CTRL_CMD_DONE,                             /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_FEAT_COMP_COMPUTE_START,                       /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_FEAT_COMP_PROCESSED,                           /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_FEAT_COMP_XACC_DONE,                           /* EVENT */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH0,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH1,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH2,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH3,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH4,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH5,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH6,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_CAPTURE_EVENT_CH7,                             /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH0,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH1,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH2,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH3,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH4,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH5,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH6,                                /* UINT32 */
    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_VSWR_STATE_CH7,                                /* UINT32 */

    ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_APP_END = ADI_ADRV904X_DFE_SVC_LOG_TRACE_EVENT_RANGE_APP_MAX
} adi_adrv904x_DfeSvcLogTraceEvent_e;

#endif  /* ADRV904X_DFE_SVC_LOG_TRACE_EVENTS_T_H_ */


