/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_cpu_types.h
*
* \brief Contains ADRV903X data types for on board cpus feature
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_CPU_TYPES_H_
#define _ADI_ADRV903X_CPU_TYPES_H_

#include "adi_adrv903x_utilities_types.h"
#include "adi_adrv903x_version_types.h"
#include "adi_adrv903x_cpu_fw_rev_info_types.h"
#include "adi_library_types.h"


#define ADI_ADRV903X_CPU_0_BINARY_IMAGE_FILE_SIZE_BYTES (ADRV903X_CPU_PM_SIZE) 
#define ADI_ADRV903X_CPU_1_BINARY_IMAGE_FILE_SIZE_BYTES (ADRV903X_CPU_PM_SIZE)

#define ADI_ADRV903X_CPU_0_BINARY_IMAGE_FILE_SIZE_MASK  (ADRV903X_CPU_PM_SIZE - 1u)

#define ADI_ADRV903X_CPU_0_PROFILE_BINARY_IMAGE_FILE_SIZE_BYTES ADRV903X_DEVICE_PROFILE_SIZE_BYTES
#define ADI_ADRV903X_LINK_ID_MAX 1U

/**
 * \brief Enumerated list of CPU Type
 */
typedef enum adi_adrv903x_CpuType
{
    ADI_ADRV903X_CPU_TYPE_UNKNOWN = -1,  /*!< Unknown CPU */
    ADI_ADRV903X_CPU_TYPE_0,             /*!< CPU 0 */
    ADI_ADRV903X_CPU_TYPE_1,             /*!< CPU 1 */
    ADI_ADRV903X_CPU_TYPE_MAX_RADIO,     /*!< Number of radio CPUs */
    ADI_ADRV903X_CPU_TYPE_MAX,           /*!< total CPUs, radio and non-radio */
} adi_adrv903x_CpuType_e;

#ifndef CLIENT_IGNORE
/**
 * \brief Structure to hold memory map of a cpu
 */
typedef struct adi_adrv903x_CpuAddr
{
    uint8_t enabled; /*!< Cpu enabled status */
    
    /* subsystem registers */
    uint16_t ctlAddr; /*!< control register */
    uint32_t bootAddr; /*!< boot address */
    uint32_t stackPtrAddr; /*!< stack pointer */
    uint32_t memBankCtrlAddr; /*!< memory bank control register */
    
    /* Mailbox registers */
    uint16_t cmdAddr; /*!< command. b7 = command_busy. b5:0 command */
    uint16_t extCmdAddr; /*!< command payload. 4 bytes */
    uint16_t cmdStatusAddr; /*!< command status. 16 bytes */
    
    /* Program section */
    uint32_t progStartAddr; /*!< start address of program memory section */
    uint32_t progEndAddr; /*!< end address of program memory section */
    uint32_t versionAddr; /*!< 48-bytes set aside for FW Rev Info address */
    uint32_t structChecksumAddr; /*!< checksum structure address */
    uint32_t debugPointersAddr; /*!< Debug Pointers */
    uint32_t buildChecksumAddr; /*!< FW build time checksum address */
    
    /* Data section */
    uint32_t dataStartAddr; /*!< start address of data memory section */
    uint32_t dataEndAddr; /*!< end address of data memory section */
    uint32_t cfrPulseAddr; /*!<  */
    uint32_t mailboxSetAddr; /*!< Mailbox SET command buffer */
    uint32_t mailboxGetAddr; /*!< Mailbox GET command buffer */
    uint32_t mailboxRunInitAddr; /*!< Mailbox RUN_INIT command buffer */
    uint32_t mailboxFhmAddr; /*!< Mailbox Frequency Hopping Mode */
    uint16_t curTransactionId[ADI_ADRV903X_LINK_ID_MAX];
} adi_adrv903x_CpuAddr_t;

/**
 * \brief CPU data structure
 */
typedef struct adi_adrv903x_Cpu
{
    adi_adrv903x_Version_t fwVersion; /*!< FW version */
    adi_adrv903x_Version_t devProfileVersion; /*!< Device profile version */
    uint32_t devProfileAddr; /*!< Device Profile structure address */
    uint32_t devDebugConfigAddr; /*!< Device Debug Config structure address */
    uint8_t  devDebugIarEnable;  /*!< Device Debug IAR Enable flag */
    adi_adrv903x_CpuAddr_t cpuAddr[ADI_ADRV903X_MAX_NUM_CPUS]; /*!< CPU address structures */
    adi_adrv903x_CpuMemDumpBinaryInfo_t cpuMemDumpBinaryInfo;  /*!< CPU Memory Dump Binary Info structures */
} adi_adrv903x_Cpu_t;
#endif

/**
 * \brief Data structure to FW version information
 */
typedef struct adi_adrv903x_CpuFwVersion
{
    adi_adrv903x_Version_t     commVer; /*!< Common version for API and FW */
    adi_adrv903x_CpuFwBuildType_e  cpuFwBuildType; /*!< BuildType for FW */
} adi_adrv903x_CpuFwVersion_t;

#endif /* _ADI_ADRV903X_CPU_TYPES_H_ */
