/** 
 * \file adrv904x_bf_core_types.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_CORE_TYPES_H_
#define _ADRV904X_BF_CORE_TYPES_H_

typedef enum adrv904x_BfCoreChanAddr
{
    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS    =    0x47000000
} adrv904x_BfCoreChanAddr_e;

/** 
 * \brief Enumeration for l1mem0SharedBankDiv
 */

typedef enum adrv904x_Bf_Core_L1mem0SharedBankDiv
{
    ADRV904X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM000          =    0,  /*!< IRAM_288K  DRAM_288K    */
    ADRV904X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM001          =    1,  /*!< IRAM_224K  DRAM_352K    */
    ADRV904X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM002          =    2,  /*!< IRAM_256K  DRAM_320K    */
    ADRV904X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM003          =    3,  /*!< IRAM_320K  DRAM_256K    */
    ADRV904X_BF_CORE_L1MEM0_SHARED_BANK_DIV_ENUM004          =    4   /*!< IRAM_352K  DRAM_224K    */
} adrv904x_Bf_Core_L1mem0SharedBankDiv_e;

/** 
 * \brief Enumeration for l1mem1SharedBankDiv
 */

typedef enum adrv904x_Bf_Core_L1mem1SharedBankDiv
{
    ADRV904X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM000          =    0,  /*!< IRAM_288K  DRAM_288K    */
    ADRV904X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM001          =    1,  /*!< IRAM_224K  DRAM_352K    */
    ADRV904X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM002          =    2,  /*!< IRAM_256K  DRAM_320K    */
    ADRV904X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM003          =    3,  /*!< IRAM_320K  DRAM_256K    */
    ADRV904X_BF_CORE_L1MEM1_SHARED_BANK_DIV_ENUM004          =    4   /*!< IRAM_352K  DRAM_224K    */
} adrv904x_Bf_Core_L1mem1SharedBankDiv_e;

#endif // _ADRV904X_BF_CORE_TYPES_H_

