/***************************************************************************/
/**
 *   @file   gmsl_dbg.h
 *   @brief  Header file of GMSL debug module.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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