/*******************************************************************************
 *   @file   no_os_config.h
 *   @brief  Default configurations for No-OS.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef _NO_OS_CONFIG_H_
#define _NO_OS_CONFIG_H_

#ifndef CONFIG_PLATFORM_IRQ_ACTIONS
#define CONFIG_PLATFORM_IRQ_ACTIONS 1
#endif

#ifndef CONFIG_DYNAMIC_ALLOC
#define CONFIG_DYNAMIC_ALLOC 0
#endif

#ifndef CONFIG_IIO_INST
#define CONFIG_IIO_INST 1
#endif

#ifndef CONFIG_IIO_DEVS
#define CONFIG_IIO_DEVS 1
#endif

#ifndef CONFIG_IIO_TRIGS
#define CONFIG_IIO_TRIGS 1
#endif

#ifndef CONFIG_IIOD_CONN_BUFF_SIZE
#define CONFIG_IIOD_CONN_BUFF_SIZE 4096
#endif

#ifndef CONFIG_IIOD_CONN
#define CONFIG_IIOD_CONN 1
#endif

#ifndef CONFIG_NET_DEVS
#define CONFIG_NET_DEVS 1
#endif

#ifndef CONFIG_NET_IFACE
#define CONFIG_NET_IFACE 1
#endif

#ifndef CONFIG_SPI_NUM_DEVS
#define CONFIG_SPI_NUM_DEVS 5
#endif

#endif