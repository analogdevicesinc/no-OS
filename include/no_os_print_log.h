/***************************************************************************//**
 *   @file   no_os_print_log.h
 *   @brief  Print messages helpers.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _NO_OS_PRINT_LOG_H_
#define _NO_OS_PRINT_LOG_H_

#include <stdio.h>

#define NO_OS_LOG_EMERG	0x0
#define NO_OS_LOG_ALERT	0x1
#define NO_OS_LOG_CRIT	0x2
#define NO_OS_LOG_ERR		0x3
#define NO_OS_LOG_WARNING	0x4
#define NO_OS_LOG_NOTICE	0x5
#define NO_OS_LOG_INFO	0x6
#define NO_OS_LOG_DEBUG	0x7

#ifndef NO_OS_LOG_LEVEL
#define NO_OS_LOG_LEVEL NO_OS_LOG_INFO
#endif

#if defined(PRINT_TIME)
#define pr_time			{ struct no_os_time _t = no_os_get_time();	\
	printf("[%5d.%06d] ", _t.s, _t.us);					\
}
#else
#define pr_time			;
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_EMERG && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_emerg(fmt, args...) do {							\
	pr_time										\
	printf("EMERG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args);	\
} while (0)
#else
#define pr_emerg(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_ALERT && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_alert(fmt, args...) do {							\
	pr_time										\
	printf("ALERT: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args);	\
} while (0)
#else
#define pr_alert(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_CRIT && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_crit(fmt, args...) do { 						\
	pr_time									\
	printf("CRIT: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args);	\
} while (0)
#else
#define pr_crit(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_ERR && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_err(fmt, args...) do {						\
	pr_time									\
	printf("ERR: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args);	\
} while (0)
#else
#define pr_err(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_WARNING && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_warning(fmt, args...) do {		\
	pr_time					\
	printf("WARNING: " fmt, ##args);	\
} while (0)
#else
#define pr_warning(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_NOTICE && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_notice(fmt, args...) do {	\
	pr_time				\
	printf("NOTICE: " fmt, ##args);	\
} while (0)
#else
#define pr_notice(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL >= NO_OS_LOG_INFO && NO_OS_LOG_LEVEL <= NO_OS_LOG_DEBUG
#define pr_info(fmt, args...) do {	\
	pr_time 			\
	printf(fmt, ##args);		\
} while(0)
#else
#define pr_info(fmt, args...)
#endif

#if defined(NO_OS_LOG_LEVEL) && NO_OS_LOG_LEVEL == NO_OS_LOG_DEBUG
#define pr_debug(fmt, args...) do {	\
	pr_time				\
	printf("DEBUG: " fmt, ##args);	\
} while(0)
#else
#define pr_debug(fmt, args...)
#endif

#endif // _NO_OS_PRINT_LOG_H_
