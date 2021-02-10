/***************************************************************************//**
 *   @file   adi_platform.h
 *   @brief  adrv9002 HAL types.
 *   @author Darius Berghe (darius.berghe@analog.com)
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

#ifndef __ADI_PLATFORM_TYPES_H__
#define __ADI_PLATFORM_TYPES_H__

#ifndef ADI_COMPILED_LOGLEVEL
/**
 * Log level events to be compiled into the device driver. Less severe logging calls will not be included, reducing
 * code size
 */
#define ADI_COMPILED_LOGLEVEL ADI_LOGLEVEL_WARN
#endif // !ADI_COMPILED_LOGLEVEL


/**
 * Log everything in exhaustive detail. Used only for development
 */
#define ADI_LOGLEVEL_TRACE (0)

/**
 * Log diagnostic information
 */
#define ADI_LOGLEVEL_DEBUG (1)

/**
 * Log state changes in the application
 */
#define ADI_LOGLEVEL_INFO (2)

/**
 * Log bad, but recoverable events
 */
#define ADI_LOGLEVEL_WARN (3)

/**
 * Log events that cannot be recovered from
 */
#define ADI_LOGLEVEL_ERROR (4)

/**
 * Log events that are likely to be fatal
 */
#define ADI_LOGLEVEL_FATAL (5)

/**
 * Disable all logging
 */
#define ADI_LOGLEVEL_NONE (255)

#endif /* __ADI_PLATFORM_TYPES_H__*/


