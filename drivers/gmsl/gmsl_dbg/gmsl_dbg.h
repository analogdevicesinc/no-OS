/*******************************************************************************
 *   @file   gmsl_dbg.h
 *   @brief  Header file of GMSL debug module.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef DBG_H
#define DBG_H

#include <stdbool.h> /*!< Boolean definitions */
#include <stdint.h>  /*!< Integer types */
#include <string.h>  /*!< String functions */
#include <stdio.h>   /*!< I/O functions */

/* Max size of individual log message */
#define MAX_LOG_MSG_SIZE       (2200u)

#define GMSL_LOG_LEVEL_ERR		0x0
#define GMSL_LOG_LEVEL_WARNING	0x1
#define GMSL_LOG_LEVEL_INFO	    0x2
#define GMSL_LOG_LEVEL_DEBUG    0x3

#ifdef _DEBUG

/* To be implemented by application*/
extern uint8_t gmsl_uc_log_msg_buffer[MAX_LOG_MSG_SIZE];
extern uint32_t gmsl_uc_log_msg_buf_length;
extern void gmsl_uc_log_error(uint8_t *tx_data, uint32_t size);
extern void gmsl_uc_log_debug(uint8_t *tx_data, uint32_t size);
extern void gmsl_uc_log_info(uint8_t *tx_data, uint32_t size);
extern void gmsl_uc_log_warning(uint8_t *tx_data, uint32_t size);
extern void gmsl_uc_set_fatal_error(void);

#if (defined(GMSL_LOG_LEVEL) && (GMSL_LOG_LEVEL >= GMSL_LOG_LEVEL_ERR) && (GMSL_LOG_LEVEL <= GMSL_LOG_LEVEL_DEBUG))
#define GMSL_LOG_ERROR(msg, ...) do {\
    /* Create the log string */\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, (char *)(msg), ##__VA_ARGS__); \
    /* Send the data over UART */\
    gmsl_uc_log_error(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
} while (false)
#else
#define GMSL_LOG_ERROR(msg, ...)
#endif

#if (defined(GMSL_LOG_LEVEL) && (GMSL_LOG_LEVEL >= GMSL_LOG_LEVEL_WARNING) && (GMSL_LOG_LEVEL <= GMSL_LOG_LEVEL_DEBUG))
#define GMSL_LOG_WARNING(msg, ...) do {\
    /* Create the log string */\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, (char *)(msg), ##__VA_ARGS__); \
    /* Send the data over UART */\
    gmsl_uc_log_warning(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
} while (false)
#else
#define GMSL_LOG_WARNING(msg, ...)
#endif

#if (defined(GMSL_LOG_LEVEL) && (GMSL_LOG_LEVEL >= GMSL_LOG_LEVEL_INFO) && (GMSL_LOG_LEVEL <= GMSL_LOG_LEVEL_DEBUG))
#define GMSL_LOG_INFO(msg, ...) do {\
    /* Create the log string */\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, (char *)(msg), ##__VA_ARGS__); \
    /* Send the data over UART */\
    gmsl_uc_log_info(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
} while (false)
#else
#define GMSL_LOG_INFO(msg, ...)
#endif

#if (defined(GMSL_LOG_LEVEL) && (GMSL_LOG_LEVEL >= GMSL_LOG_LEVEL_DEBUG) && (GMSL_LOG_LEVEL <= GMSL_LOG_LEVEL_DEBUG))
#define GMSL_LOG_DEBUG(msg, ...) do {\
    /* Create the log string */\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, (char *)(msg), ##__VA_ARGS__); \
    /* Send the data over UART */\
    gmsl_uc_log_debug(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
} while (false)
#else
#define GMSL_LOG_DEBUG(msg, ...)
#endif

#if (defined(GMSL_LOG_LEVEL) && (GMSL_LOG_LEVEL >= GMSL_LOG_LEVEL_INFO) && (GMSL_LOG_LEVEL <= GMSL_LOG_LEVEL_DEBUG))
#define GMSL_LOG_DIAG(current_diag_err, dev_name_str, dev_addr, msg, ...) do {\
    if(true == current_diag_err)\
    {\
        gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, "[%s-0x%x]"msg, dev_name_str, dev_addr, ##__VA_ARGS__); \
        gmsl_uc_log_error(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
    }\
    else \
    {\
        gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, "[%s-0x%x]"msg, dev_name_str, dev_addr, ##__VA_ARGS__); \
        gmsl_uc_log_info(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
    }\
} while (false)
#else
#define GMSL_LOG_DIAG(current_diag_err, dev_name_str, dev_addr, msg, ...) do {\
    if(true == current_diag_err)\
    {\
        gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, "[%s-0x%x]"msg, dev_name_str, dev_addr, ##__VA_ARGS__); \
        gmsl_uc_log_error(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length); \
    }\
} while (false)
#endif

#define GMSL_LOG_DEV_ERROR(dev_name_str, dev_addr, msg, ...)\
{\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, "[%s-0x%x]"msg, dev_name_str, dev_addr, ##__VA_ARGS__);\
    gmsl_uc_log_error(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length);\
}

#define GMSL_LOG_DEV_INFO(dev_name_str, dev_addr, msg, ...)\
{\
    gmsl_uc_log_msg_buf_length = (uint32_t)snprintf((char*)&gmsl_uc_log_msg_buffer[0u], MAX_LOG_MSG_SIZE, "[%s-0x%x]"msg, dev_name_str, dev_addr, ##__VA_ARGS__);\
    gmsl_uc_log_info(gmsl_uc_log_msg_buffer, gmsl_uc_log_msg_buf_length);\
}

#define GMSL_DBG_ERROR() {\
    GMSL_LOG_ERROR("Fatal error!");\
    gmsl_uc_set_fatal_error();\
}

#define GMSL_DBG_REQUIRE(cond, errMsg) {\
    if(!(cond)) {\
        GMSL_LOG_ERROR("Precondition error: " errMsg);\
        gmsl_uc_set_fatal_error();\
    }\
}

/*=========== Macros to en/dis log printing =============*/

#else
#define GMSL_LOG_ERROR(msg, ...)
#define GMSL_LOG_WARNING(msg, ...)
#define GMSL_LOG_INFO(msg, ...)
#define GMSL_LOG_DEBUG(msg, ...)

#define GMSL_LOG_DIAG(current_diag_err, dev_name_str, dev_addr, msg, ...)
#define GMSL_LOG_DEV_ERROR(dev_name_str, dev_addr, msg, ...)
#define GMSL_LOG_DEV_INFO(dev_name_str, dev_addr, msg, ...)
#define GMSL_DBG_REQUIRE(cond, errMsg)
#define GMSL_DBG_ERROR()

#endif

#endif
