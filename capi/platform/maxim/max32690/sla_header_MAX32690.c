/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
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
 ******************************************************************************/

#define SWAP(x) \
    (((x) >> 24) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | (((x)&0x000000FF) << 24))

typedef enum {
    ROM_A1_VERSION = 0x01000000,
} enum_rom_version_t;

#ifdef __SLA_FWK__
#include <stdio.h>
#include <stdint.h>

extern unsigned int _start_SWAP;
extern unsigned int _SLA_Size_SWAP;

typedef enum {
    MagicH = 0x48495357,
    MagicL = 0x45444744,
} enum_magic_t;

typedef struct {
    enum_magic_t MagicHigh; //> SLA Header magic
    enum_magic_t MagicLow; //> SLA Header magic
} magic_t;

typedef struct {
    magic_t Magic;
    enum_rom_version_t RomVersion; //> ROM version
    unsigned int LoadAddr; //> Relocation address.
    unsigned int SLA_CodeSize; //> SLA code size in bytes
    unsigned int *JumpAddr; //> Rom code will jump at this address
    unsigned int ArgSize; //> Size of the Argument
    unsigned int AppVersionNumber; //> Version of this application
} flash_app_header_t;

__attribute__((section(".sb_sla_header"))) __attribute__((__used__))
const flash_app_header_t sb_header = {
    .Magic =
        {
            .MagicHigh = SWAP(MagicH),
            .MagicLow  = SWAP(MagicL),
        },

#if defined(MAX32690_A1)
    .RomVersion = SWAP(ROM_A1_VERSION),
#else
    .RomVersion = SWAP(ROM_A1_VERSION),
#endif
    .LoadAddr         = SWAP(0x10000000),
    .SLA_CodeSize     = (unsigned int)&_SLA_Size_SWAP, // Trick to get constant defined at link time
    .JumpAddr         = &_start_SWAP,
    .ArgSize          = 0,
    .AppVersionNumber = SWAP(0x01000000), // 0xAABBCCCC for version AA.BB.CCCC
};

#endif //__SLA_FWK__

#ifdef __SCPA_FWK__
/** Global declarations */

#include <string.h>

typedef enum {
    MagicH = 0xDEADBEEF,
    MagicL = 0xCAFEFADE,
} enum_magic_t;

typedef struct {
    enum_magic_t MagicHigh; //> SLA Header magic
    enum_magic_t MagicLow; //> SLA Header magic
} magic_t;

typedef int (*__scpa_write_t)(unsigned int dest, unsigned int length, unsigned char *p_src);
typedef int (*__scpa_erase_t)(unsigned int dest, unsigned int length);

/** Generic Plugin Operations */
typedef struct {
    __scpa_write_t write; //> Write to memory
    __scpa_write_t compare; //> Compare memory data
    __scpa_erase_t erase; //> Erase memory
} scpa_ops_t;

typedef struct {
    magic_t Magic;
    enum_rom_version_t RomVersion; //> ROM version
    unsigned int mem_base_addr; //> Base address of memory targetted by applet
    unsigned int mem_size; //> Size of this memory
    scpa_ops_t ops; //> Operations of the SCP Applet
} scpa_header_t;

int start_scpa_write(unsigned int dest, unsigned int length, unsigned char *p_src);
int start_scpa_compare(unsigned int dest, unsigned int length, unsigned char *p_src);
int start_scpa_erase(unsigned int dest, unsigned int length);

int __attribute__((weak)) scpa_write(unsigned int dest, unsigned int length, unsigned char *p_src);
int __attribute__((weak))
scpa_compare(unsigned int dest, unsigned int length, unsigned char *p_src);
int __attribute__((weak)) scpa_erase(unsigned int dest, unsigned int length);

extern unsigned int __bss_start__;
extern unsigned int __bss_end__;
extern unsigned int __bss_magic__;

#ifndef SCPA_MEM_BASE_ADDR
#define SCPA_MEM_BASE_ADDR 0xC0000000
#warning 'SCPA_MEM_BASE_ADDR not defined using default value 0xC0000000'
#endif

#ifndef SCPA_MEM_SIZE
#define SCPA_MEM_SIZE 1024
#warning 'SCPA_MEM_SIZE not defined using default value 1024'
#endif

unsigned int __attribute__((section(".scpa_init"))) Magic_bss = 0xABADCAFE;

__attribute__((section(".scpa_header"))) __attribute__((__used__))
const scpa_header_t scpa_header = {
    .Magic =
        {
            .MagicHigh = MagicH,
            .MagicLow  = MagicL,
        },
#ifdef MAX32690_A1
    .RomVersion = SWAP(ROM_A1_VERSION),
#elif defined(MAX32690_A2)
    .RomVersion = SWAP(ROM_A2_VERSION),
#else
#error "Please Select a chip ROM revision"
#endif
    .mem_base_addr = SCPA_MEM_BASE_ADDR,
    .mem_size      = SCPA_MEM_SIZE,
    .ops =
        {
            .write   = (__scpa_write_t)start_scpa_write,
            .compare = (__scpa_write_t)start_scpa_compare,
            .erase   = (__scpa_erase_t)start_scpa_erase,
        },
};

int __attribute__((section(".scpa_ops")))
start_scpa_write(unsigned int dest, unsigned int length, unsigned char *p_src)
{
    volatile unsigned int bss_size =
        (volatile unsigned int)&__bss_end__ - (volatile unsigned int)&__bss_start__;
    volatile unsigned char *p_bss = (volatile unsigned char *)&__bss_start__;
    volatile unsigned int *p_magic = (volatile unsigned int *)&__bss_magic__;

    // Automatic Code for bss init
    if (*p_magic == 0xABADCAFE) {
        memset((void *)p_bss, 0x00, bss_size);
        *p_magic = 0x0;
    }
    return scpa_write(dest, length, p_src);
}

int __attribute__((section(".scpa_ops")))
start_scpa_compare(unsigned int dest, unsigned int length, unsigned char *p_src)
{
    volatile unsigned int bss_size =
        (volatile unsigned int)&__bss_end__ - (volatile unsigned int)&__bss_start__;
    volatile unsigned char *p_bss = (volatile unsigned char *)&__bss_start__;
    volatile unsigned int *p_magic = (volatile unsigned int *)&__bss_magic__;

    // Automatic Code for bss init
    if (*p_magic == 0xABADCAFE) {
        memset((void *)p_bss, 0x00, bss_size);
        *p_magic = 0x0;
    }
    return scpa_compare(dest, length, p_src);
}

int __attribute__((section(".scpa_ops"))) start_scpa_erase(unsigned int dest, unsigned int length)
{
    volatile unsigned int bss_size =
        (volatile unsigned int)&__bss_end__ - (volatile unsigned int)&__bss_start__;
    volatile unsigned char *p_bss = (volatile unsigned char *)&__bss_start__;
    volatile unsigned int *p_magic = (volatile unsigned int *)&__bss_magic__;

    // Automatic Code for bss init
    if ((*p_magic == 0xABADCAFE)) {
        memset((void *)p_bss, 0x00, bss_size);
        *p_magic = 0x0;
    }
    return scpa_erase(dest, length);
}

int __attribute__((section(".scpa_ops")))
scpa_write(unsigned int dest, unsigned int length, unsigned char *p_src)
{
    (void)dest;
    (void)length;
    (void)p_src;
    return 0;
}

int __attribute__((section(".scpa_ops")))
scpa_compare(unsigned int dest, unsigned int length, unsigned char *p_src)
{
    (void)dest;
    (void)length;
    (void)p_src;
    return 0;
}

int __attribute__((section(".scpa_ops"))) scpa_erase(unsigned int dest, unsigned int length)
{
    (void)dest;
    (void)length;
    return 0;
}

void Reset_Handler(void) {}

#endif //__SCPA_FWK__
