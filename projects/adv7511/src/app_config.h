/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Config file of ADV7511 no-OS project.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
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

#ifndef CONFIG_H_
#define CONFIG_H_

// #define HAVE_VERBOSE_MESSAGES /* Recommended during development prints errors and warnings */
// #define DEBUG		 /* For Debug purposes only */

#define XILINX

//#define PLATFORM_AC701
//#define PLATFORM_KC705
//#define PLATFORM_VC707
//#define PLATFORM_ZC702
//#define PLATFORM_ZC706
//#define PLATFORM_ZED

#if !defined(PLATFORM_AC701) && !defined(PLATFORM_KC705) && \
	!defined(PLATFORM_VC707) && !defined(PLATFORM_ZC702) && \
	!defined(PLATFORM_ZC706) && !defined(PLATFORM_ZED)
#error "ERROR: Must define a platform in app_config.h"
#endif

#endif
