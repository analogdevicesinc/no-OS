/** 
 * \file adrv903x_bf_core_types.h Automatically generated file with generator ver 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADRV903X_BF_CORE_TYPES_H_
#define _ADRV903X_BF_CORE_TYPES_H_

typedef enum adrv903x_BfCoreChanAddr
{
    ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS    =    0x47000000
} adrv903x_BfCoreChanAddr_e;

/** 
 * \brief Enumeration for l1mem0SharedBankDiv
 */

typedef enum adrv903x_Bf_Core_L1mem0SharedBankDiv
{
    ADRV903X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM000          =    0,  /*!< IRAM_288K  DRAM_288K    */
    ADRV903X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM001          =    1,  /*!< IRAM_224K  DRAM_352K    */
    ADRV903X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM002          =    2,  /*!< IRAM_256K  DRAM_320K    */
    ADRV903X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM003          =    3,  /*!< IRAM_320K  DRAM_256K    */
    ADRV903X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM004          =    4   /*!< IRAM_352K  DRAM_224K    */
} adrv903x_Bf_Core_L1mem0SharedBankDiv_e;

/** 
 * \brief Enumeration for l1mem1SharedBankDiv
 */

typedef enum adrv903x_Bf_Core_L1mem1SharedBankDiv
{
    ADRV903X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM000          =    0,  /*!< IRAM_288K  DRAM_288K    */
    ADRV903X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM001          =    1,  /*!< IRAM_224K  DRAM_352K    */
    ADRV903X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM002          =    2,  /*!< IRAM_256K  DRAM_320K    */
    ADRV903X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM003          =    3,  /*!< IRAM_320K  DRAM_256K    */
    ADRV903X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM004          =    4   /*!< IRAM_352K  DRAM_224K    */
} adrv903x_Bf_Core_L1mem1SharedBankDiv_e;

#endif // _ADRV903X_BF_CORE_TYPES_H_

