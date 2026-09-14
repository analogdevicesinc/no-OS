
/*!
 * \brief     SPI Register Definition Header File, includes customised bitfield definitions derived from auto-generated headers
 *
 * \copyright copyright(c) 2018 - Analog Devices Inc.All Rights Reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup APOLLO_BF
 * @{
 */
#ifndef __ADI_APOLLO_BF_CUSTOM_H__
#define __ADI_APOLLO_BF_CUSTOM_H__

/*=========== I N C L U D E S  ============*/

#include "adi_apollo_bf_jrx_wrapper.h"
#include "adi_apollo_bf_jtx_dformat.h"
#include "adi_apollo_bf_txrx_cfir_top.h"
#include "adi_apollo_bf_txrx_fine_nco.h"
#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_apollo_profile_config.h"

/*============= D E F I N E S ==============*/

#define BF_JRX_CORE_PHASE_ADJUST_INFO_LSB(inst, n)          REG_JRX_CORE_PHASE_ADJUST0_ADDR(inst, n), 0x00000800
#define BF_JRX_CORE_PHASE_ADJUST_INFO_MSB(inst, n)          REG_JRX_CORE_PHASE_ADJUST1_ADDR(inst, n), 0x00000800

#define BF_LINK_TOTAL_INTERP_INFO_LSB(inst, n)              REG_GENERAL_JRX_CTRL_5_ADDR(inst, n), 0x00000800
#define BF_LINK_TOTAL_INTERP_INFO_MSB(inst, n)              REG_GENERAL_JRX_CTRL_6_ADDR(inst, n), 0x00000800

#define BF_LINK_DUC_INTERP_INFO_LSB(inst, n)                REG_GENERAL_JRX_CTRL_8_ADDR(inst, n), 0x00000800
#define BF_LINK_DUC_INTERP_INFO_MSB(inst, n)                REG_GENERAL_JRX_CTRL_9_ADDR(inst, n), 0x00000800

#define BF_INVALID_EN_0_INFO(inst)                          REG_INVALID_EN_ADDR(inst), 0x00000100
#define BF_INVALID_EN_1_INFO(inst)                          REG_INVALID_EN_ADDR(inst), 0x00000101

#define BF_COEFF_PROFILE_SEL0_INFO(inst)                    REG_CFIR_PROFILE_ADDR(inst), 0x00000100
#define BF_COEFF_PROFILE_SEL1_INFO(inst)                    REG_CFIR_PROFILE_ADDR(inst), 0x00000101
#define BF_COEFF_PROFILE_SEL2_INFO(inst)                    REG_CFIR_PROFILE_ADDR(inst), 0x00000102
#define BF_COEFF_PROFILE_SEL3_INFO(inst)                    REG_CFIR_PROFILE_ADDR(inst), 0x00000103

#define BF_AUTOFLIP_INCDIR_FTW_TXRX_FINE_NCO_INFO(inst)     REG_AUTOFLIP_INCDIR_ADDR(inst), 0x00000100
#define BF_AUTOFLIP_INCDIR_PHOFST_TXRX_FINE_NCO_INFO(inst)  REG_AUTOFLIP_INCDIR_ADDR(inst), 0x00000102

#define BF_AUTO_INC_DECB_FTW_TXRX_FINE_NCO_INFO(inst)       REG_AUTO_INC_DECB_ADDR(inst), 0x00000100
#define BF_AUTO_INC_DECB_PHOFST_TXRX_FINE_NCO_INFO(inst)    REG_AUTO_INC_DECB_ADDR(inst), 0x00000102
#define BF_HOP_PHASE_INC_INFO(inst)                         REG_HOP_PHASE_INC0_ADDR(inst), 0x00002000
#define BF_HOP_PHASE_OFFSET_INFO(inst)                      REG_HOP_PHASE_OFFSET0_ADDR(inst), 0x00001000

#define MEM_CODE_MEMORY_B_0                                 (0x02000000U)  //arm_mem.yda
#define MEM_CODE_MEMORY_A_6                                 (0x01030000U)  //arm_mem.yda
#define MEM_SYS_MEMORY_B_0                                  (0x21000000U)  //arm_mem.yda
#define MEM_SYS_MEMORY_B_MAX                                (0x21050000U + 0x8000U)  //arm_mem.yda
#define BF_CPU_0_PRIMARY                                    BF_SCRATCH_REG_INFO(16) //FW startup code
#define BF_CPU_1_PRIMARY                                    BF_SCRATCH_REG_INFO(17) //FW startup code
#define BF_EC_RAM_LOCK_INFO                                 REG_EC_ANA_BYTE3_ADDR, 0x00000106
#define BF_EC_RAM_LOCK_MASK                                 (0x40)
#define BF_CONFIG_TRANSFER_DONE                             REG_ALT_BOOT_ADDR(0), 0x00000100
#define BF_WAITING_FOR_CONFIG_TRANSFER_STATUS               REG_ALT_BOOT_ADDR(0), 0x00000101
#define BF_BOOT_STALL_STATUS                                REG_ALT_BOOT_ADDR(0), 0x00000102
#define BF_BOOT_DONE_STATUS                                 REG_ALT_BOOT_ADDR(0), 0x00000107
#define BF_BOOT_ERROR                                       REG_ALT_BOOT_ADDR(1), 0x00000800
#define BF_BOOT_STATUS                                      REG_ALT_BOOT_ADDR(2), 0x00000800
#define REG_RAM_BOOT_ERROR_PTR                              (0x020003B4U)
#define BF_RAM_BOOT_CORE0_STATUS                            (0x47000200U), 0x00000800
#define BF_RAM_BOOT_CORE1_STATUS                            (0x47000201U), 0x00000800

#define BF_DEVICE_PROFILE_CRC_CHECK_STATUS                  BF_SCRATCH_REG_INFO(6)

#define BF_NVMB_TE_BYPASS_INFO                              (0x4C0023E8U), 0x00000106U
#define BF_TE_BOOT_READY_SET                                REG_ALT_BOOT_ADDR(23), 0x00000800
#define REG_SECURE_BOOT_STAGE0                              (0x4C006342U)
#define REG_SECURE_BOOT_STAGE1                              (0x4C006343U)

// Enhancements to adi_apollo_bf_apollo_profile_config.h
#define BF_PC_RX_FNCO_SLICE_SELECT_INFO(inst)                (0x00000100 + inst)
#define BF_PC_TX_FNCO_SLICE_SELECT_INFO(inst)                (0x00000110 + inst)
#define BF_PC_RX_CNCO_SLICE_SELECT_INFO(inst)                (0x00000100 + inst)
#define BF_PC_TX_CNCO_SLICE_SELECT_INFO(inst)                (0x00000108 + inst)
#define BF_PC_RX_CFIR_SLICE_SELECT_INFO(inst)                (0x00000100 + inst)
#define BF_PC_TX_CFIR_SLICE_SELECT_INFO(inst)                (0x00000110 + inst)
#define BF_PC_RX_PFILT_SLICE_SELECT_INFO(inst)               (0x00000100 + inst)
#define BF_PC_TX_PFILT_SLICE_SELECT_INFO(inst)               (0x00000102 + inst)
#define BF_PC_RX_DR_SLICE_SELECT_INFO(inst)                  (0x00000100 + inst)
#define BF_PC_TX_DR_SLICE_SELECT_INFO(inst)                  (0x00000108 + inst)
#define BF_PC_BMEM_HOP_SLICE_SELECT_INFO(inst)               (0x00000100 + inst)

#endif /* __ADI_APOLLO_BF_CUSTOM_H__ */
/*! @} */
