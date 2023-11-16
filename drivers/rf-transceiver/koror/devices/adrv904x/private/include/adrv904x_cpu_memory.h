/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_cpu_memory.h
 *
 * \brief   Contains CPU memory tables
 *
 * \details Contains CPU memory tables and a few configuration constants.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_CPU_MEMORY_H__
#define __ADRV904X_CPU_MEMORY_H__

/* CPU0/CPU1 program and data memory starting addresses */
#define ADRV904X_CPU0_PM_START    (0x01000000U)
#define ADRV904X_CPU0_DM_START    (0x20000000U)

#define ADRV904X_CPU1_PM_START    (0x02000000U)
#define ADRV904X_CPU1_DM_START    (0x21000000U)

/* CPU program and data memory physical sizes */
#define ADRV904X_CPU_PM_SIZE      (0x50000U)
#define ADRV904X_CPU_DM_SIZE      (0x40000U)

/* CPU program and data memory limits (virtual sizes) */
#define ADRV904X_CPU_PM_LIMIT     (0x50000U)
#define ADRV904X_CPU_DM_LIMIT     (0x40000U)

/* Ensure limits are within the physical memory limits */
#if ADRV904X_CPU_PM_LIMIT > ADRV904X_CPU_PM_SIZE
#error "ADRV904X_CPU_PM_LIMIT must be <= ADRV904X_CPU_PM_SIZE"
#endif
#if ADRV904X_CPU_DM_LIMIT > ADRV904X_CPU_DM_SIZE
#error "ADRV904X_CPU_DM_LIMIT must be <= ADRV904X_CPU_DM_SIZE"
#endif

/* CPU0/CPU1 memory bank setting for the l1mem0_shared_bank_div_ctrl
 * and l1mem1_shared_bank_div_ctrl registers.
 *
 * Possible values:
 * IRAM_288K_DRAM_288K: 0x00U
 * IRAM_224K_DRAM_352K: 0x01U
 * IRAM_256K_DRAM_320K: 0x02U
 * IRAM_320K_DRAM_256K: 0x03U
 * IRAM_352K_DRAM_224K: 0x04U
 */
#define ADRV904X_CPU_MEM_BANK_CTRL_REG_VAL       (0x03U)

/** Regular for the format of CPU memory MACRO definition:
 * 1. If the memory belongs both to CPU0 and CPU1, or ignore the CPU index, the MACRO should contain the 'PM' or 'DM'
 *    to point out which kind of memory it belongs to
 * 2. If the memory belongs to a specific CPU, the MACRO should contains the 'CPU_0' or 'CPU_1' to indicate
 *    which CPU it belongs to, and contain the 'PM' or 'DM' to indicate which kind of memory it belongs to
 */

/**
 * The following memory definitions are in the ADRV904X CPU 0 program memory range (before application):
 * ADRV904X_CPU0_PM_START to ADRV904X_CPU0_PM_START + (ADRV904X_CPU_PM_SIZE - ADRV904X_CPU_PM_LIMIT) - 1
 * -> (0x01000000 to 0x01004FFF)
 *
 * Any access of this M4 memory via shared memory must account for 64-bit alignment when dereferencing pointers
 * and when accessing 64-bit data, such as in a struct.  With a struct, 64-bit members must be 64-bit
 * aligned, and therefore not packed.
 *
 * Any pointers that are shared must be on a 64-bit boundary since they will be read with a 64-bit register.
 * After the pointer is read, it must be masked to only use the lower 32-bits.
 *
 * Example:
 *
 * \code
 * // runtime values from BBIC/Radio Processor
 * _adi_svc_ProfileRuntimeProfile_t *pRuntimeProfile = *((_adi_svc_ProfileRuntimeProfile_t**)_ADI_SVC_CMN_CPU_1_PM_RUNTIME_PROFILE_PTR);
 *
 * // the pointer is read as a 64-bit value, but the pointer is 32-bits
 * pRuntimeProfile = (_adi_svc_ProfileRuntimeProfile_t *)((uint64_t)pRuntimeProfile & 0xFFFFFFFFUL);
 * \endcode
 */

#define ADRV904X_CPU_0_PM_EXCEPTION_FLAG            (0x01000398U)       /*!< CPU 0 ARM exception status */

#define ADRV904X_PM_DEBUG_CONFIG_BUF                (0x010003A4U)       /*!< JTAG Enable flag */
#define ADRV904X_PM_DEVICE_REV_DATA                 (0x010003A8U)       /*!< Firmware version */

#define ADRV904X_CPU_0_PM_CHECKSUM                  (0x01000468U)       /*!< Run-time checksum (ADRV904X_PM_DEVICE_REV_DATA + size of RevData) */

#define ADRV904X_PM_SW_BKPT_TABLE_PTR               (0x0100048CU)       /*!< Breakpoint table pointer */
#define ADRV904X_PM_SW_BKPT_GLOBAL_HALT_MASK_PTR    (0x01000490U)       /*!< Global Halt Mask pointer */

#define ADRV904X_CPU_0_PM_TRACKING_CAL_TASK_INFO    (0x01000494U)       /*!< Tracking cal info */
#define ADRV904X_CPU_0_PM_TRACKING_CAL_TASK_DATA    (0x01000498U)       /*!< Tracking cal Data */
#define ADRV904X_CPU_0_PM_INIT_CAL_TASK_INFO        (0x0100049CU)       /*!< Init cal info/status */
#define ADRV904X_CPU_0_PM_ERROR_STATUS              (0x010004A0U)       /*!< Error status */

#define ADRV904X_CPU_0_PM_CONFIG_LOG                (0x010004A4U)       /*!< Configuration framework log */
#define ADRV904X_CPU_0_PM_INIT_CAL_DATA             (0x010004A8U)       /*!< Location of the initial cal data */
#define ADRV904X_CPU_0_PM_CMD_LOG                   (0x010004ACU)       /*!< Command log record */
#define ADRV904X_CPU_0_PM_TCB_TABLE                 (0x010004B0U)       /*!< TCB buff table */
#define ADRV904X_CPU_0_PM_CURR_TASK_HANDLE          (0x010004B4U)       /*!< Current task handle */
#define ADRV904X_CPU_0_PM_SYS_CLOCK                 (0x010004B8U)       /*!< System clock config */
#define ADRV904X_PM_HEALTH_STATUS_PTR               (0x010004BCU)       /*!< Health monitors status pointer */

/* Range from 0x010004C0U to 0x010004E8U are defined below */



#define ADRV904X_CPU_0_PM_EFUSE_SETTINGS_PTR          (0x010004ECU)     /*!< eFUSE system settings - CPU 0 */

#define ADRV904X_CPU_0_PM_RADIO_SEQ_SEQUENCE_SIZE     (0x010004F0U)
#define ADRV904X_CPU_0_PM_RADIO_SEQ_BUILD_TIME_CRC    (0x010004F4U)

#define ADRV904X_PM_CORE_SCRATCH_EXT_PTR              (0x01000504U)     /*!< Extended core scratch registers pointer */

/**
 * The following memory definitions are in the ADRV904X CPU 1 program memory range (before application):
 * ADRV904X_CPU1_PM_START to ADRV904X_CPU1_PM_START + (ADRV904X_CPU_PM_SIZE - ADRV904X_CPU_PM_LIMIT) - 1
 * -> (0x02000000 to 0x02004FFF)
 *
 * Any pointers that are shared must be on a 64-bit boundary since they will be read with a 64-bit register.
 * After the pointer is read, it must be masked to only use the lower 32-bits.
 * See example above.
 */

#define ADRV904X_CPU_1_PM_EXCEPTION_FLAG    (0x02000398U)               /*!< CPU 1 ARM exception status */

/* 0x020003A4U is available */

#define ADRV904X_CPU_1_PM_TRACKING_CAL_TASK_INFO    (0x020003A8U)       /*!< Tracking cal info */
#define ADRV904X_CPU_1_PM_TRACKING_CAL_TASK_DATA    (0x020003ACU)       /*!< Tracking cal Data */
#define ADRV904X_CPU_1_PM_INIT_CAL_TASK_INFO        (0x020003B0U)       /*!< Init cal info/status */
#define ADRV904X_CPU_1_PM_ERROR_STATUS              (0x020003B4U)       /*!< Error status */

#define ADRV904X_CPU_1_PM_CONFIG_LOG                (0x020003B8U)       /*!< Configuration framework log */
#define ADRV904X_CPU_1_PM_INIT_CAL_DATA             (0x020003BCU)       /*!< Location of the initial cal data */
#define ADRV904X_CPU_1_PM_TCB_TABLE                 (0x020003C0U)       /*!< TCB buff table */
#define ADRV904X_CPU_1_PM_CURR_TASK_HANDLE          (0x020003C4U)       /*!< Current task handle */
#define ADRV904X_CPU_1_PM_SYS_CLOCK                 (0x020003C8U)       /*!< System clock config */
#define ADRV904X_CPU_1_PM_CHECKSUM                  (0x020003CCU)       /*!< Run-time checksum of CPU 1 */
#define ADRV904X_CPU_1_PM_CMD_LOG                   (0x020003D0U)       /*!< Command log record */
#define ADRV904X_CPU_1_PM_RUNTIME_PROFILE_PTR       (0x020003D8U)       /*!< Runtime profile pointer - shared ptr: must be 64-bit aligned */
#define ADRV904X_PM_DEVICE_PROFILE_PTR              (0x020003E0U)       /*!< Device profile pointer - shared ptr: must be 64-bit aligned */

/**
 * CPU 0 and CPU 1 data memory definitions.
 */

/* mailbox buffers */
#define ADRV904X_CPU_0_MAILBOX_NUM_LINKS               (1)
#define ADRV904X_CPU_0_DM_MAILBOX_LINK_0_START_ADDR    (0x20000000U)
#define ADRV904X_CPU_0_MAILBOX_LINK_0_SIZE             (0x400U)


#define ADRV904X_CPU_0_DFE_MAILBOX_NUM_LINKS           (1)
#define ADRV904X_CPU_0_DFE_MAILBOX_LINK_0_START_ADDR   (0x20000400U)
#define ADRV904X_CPU_0_DFE_MAILBOX_LINK_0_SIZE         (0x20U)

/* Radio Sequencer Image Size and CRC */
#define ADRV904X_DM_RSEQ_IMAGE_SIZE                    (0x20000420U)
#define ADRV904X_DM_RSEQ_IMAGE_CRC                     (0x20000424U)

#define ADRV904X_CPU_1_MAILBOX_NUM_LINKS               (1)
#define ADRV904X_CPU_1_DM_MAILBOX_LINK_0_START_ADDR    (0x21000000U)
#define ADRV904X_CPU_1_MAILBOX_LINK_0_SIZE             (0x400U)


#define ADRV904X_CPU_1_DFE_MAILBOX_NUM_LINKS           (1)
#define ADRV904X_CPU_1_DFE_MAILBOX_LINK_0_START_ADDR   (0x21000400U)
#define ADRV904X_CPU_1_DFE_MAILBOX_LINK_0_SIZE         (0x20U)

/* __CHECKSUM_ADDR__ OFFSET IN temp.icf */
#define ADRV904X_CPU_0_PM_CHECKSUM_BUILDTIME    (0x0104FFFCU)
#define ADRV904X_CPU_1_PM_CHECKSUM_BUILDTIME    (0x0204FFFCU)
#endif /* __ADRV904X_CPU_MEMORY_H__ */



