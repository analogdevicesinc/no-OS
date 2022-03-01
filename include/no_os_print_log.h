/***************************************************************************//**
 *   @file   print_log.h
 *   @brief  Print messages helpers.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef PRINT_LOG_H_
#define PRINT_LOG_H_

#include <stdio.h>

#define LOG_EMERG	0x0
#define LOG_ALERT	0x1
#define LOG_CRIT	0x2
#define LOG_ERR		0x3
#define LOG_WARNING	0x4
#define LOG_NOTICE	0x5
#define LOG_INFO	0x6
#define LOG_DEBUG	0x7

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_EMERG && LOG_LEVEL <= LOG_DEBUG
#define pr_emerg(fmt, args...) printf("EMERG: %s:%d:%s(): " \
fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define pr_emerg(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_ALERT && LOG_LEVEL <= LOG_DEBUG
#define pr_alert(fmt, args...) printf("ALERT: %s:%d:%s(): " \
fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define pr_alert(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_CRIT && LOG_LEVEL <= LOG_DEBUG
#define pr_crit(fmt, args...) printf("CRIT: %s:%d:%s(): " \
fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define pr_crit(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_ERR && LOG_LEVEL <= LOG_DEBUG
#define pr_err(fmt, args...) printf("ERR: %s:%d:%s(): " \
fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define pr_err(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_WARNING && LOG_LEVEL <= LOG_DEBUG
#define pr_warning(fmt, args...) printf("WARNING: " fmt, ##args)
#else
#define pr_warning(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_NOTICE && LOG_LEVEL <= LOG_DEBUG
#define pr_notice(fmt, args...) printf("NOTICE: " fmt, ##args)
#else
#define pr_notice(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_INFO && LOG_LEVEL <= LOG_DEBUG
#define pr_info(fmt, args...) printf(fmt, ##args)
#else
#define pr_info(fmt, args...)
#endif

#if defined(LOG_LEVEL) && LOG_LEVEL == LOG_DEBUG
#define pr_debug(fmt, args...) printf("DEBUG: " fmt, ##args)
#else
#define pr_debug(fmt, args...)
#endif

#endif /* PRINT_LOG_H_ */
