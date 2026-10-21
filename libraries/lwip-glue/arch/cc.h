/* SPDX-License-Identifier: Apache-2.0 */
/*
 * @file cc.h
 *
 ******************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc. (now owned by
 * Analog Devices, Inc.)
 * Copyright (C) 2023 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
// #ifndef _MXC_CC_H_
// #define _MXC_CC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

typedef uint8_t     u8_t;
typedef int8_t      s8_t;
typedef uint16_t    u16_t;
typedef int16_t     s16_t;
typedef uint32_t    u32_t;
typedef int32_t     s32_t;

typedef uintptr_t   mem_ptr_t;

#define LWIP_ERR_T  int

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT  	__attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* Platform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do {                		\
        printf x;                   						\
    } while (0)

#define LWIP_PLATFORM_ASSERT(x) do {                		\
        printf("Assert \"%s\" failed at line %d in %s\n",   \
                x, __LINE__, __FILE__);             		\
        while (1);                        					\
    } while (0)

// #endif /* _MXC_CC_H_ */
