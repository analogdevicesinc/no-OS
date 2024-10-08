/*!
 * @brief     SPI Register Definition Header File, automatically generated by
 *            yoda2h v1.1.2 at 1/2/2020 9:13:37 AM.
 *
 * @copyright copyright(c) 2018 - Analog Devices Inc.All Rights Reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_BF
 * @{
 */
#ifndef __ADI_AD9083_BF_JTX_IP_H__
#define __ADI_AD9083_BF_JTX_IP_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"

/*============= D E F I N E S ==============*/
#define REG_JTX_CORE_SAMPLE_XBAR_ADDR(n) (0x00000200 + 1 * n)
#define BF_JTX_CONV_SEL_INFO(n) (0x00000200 + 1 * n), 0x00000700
#define BF_JTX_CONV_DISABLE_INFO(n) (0x00000200 + 1 * n), 0x00000107

#define REG_JTX_CORE_CONFIG_ADDR 0x00000260
#define BF_JTX_LINK_TYPE_INFO 0x00000260, 0x00000200
#define BF_JTX_CHKSUM_DISABLE_INFO 0x00000260, 0x00000102
#define BF_JTX_CHKSUM_LSB_ALG_INFO 0x00000260, 0x00000103
#define BF_JTX_SYSREF_FOR_STARTUP_INFO 0x00000260, 0x00000106
#define BF_JTX_SYSREF_FOR_RELINK_INFO 0x00000260, 0x00000107

#define REG_JTX_CORE_LANE_XBAR_ADDR(n) (0x00000261 + 1 * n)
#define BF_JTX_LANE_SEL_INFO(n) (0x00000261 + 1 * n), 0x00000500
#define BF_JTX_LANE_INV_INFO(n) (0x00000261 + 1 * n), 0x00000105
#define BF_JTX_FORCE_LANE_PD_INFO(n) (0x00000261 + 1 * n), 0x00000106
#define BF_JTX_LANE_PD_INFO(n) (0x00000261 + 1 * n), 0x00000107

#define REG_JTX_CORE_TEST_CONFIG_ADDR 0x00000271
#define BF_JTX_TEST_GEN_MODE_INFO 0x00000271, 0x00000400
#define BF_JTX_TEST_GEN_SEL_INFO 0x00000271, 0x00000204
#define BF_JTX_TEST_MIRROR_INFO 0x00000271, 0x00000106
#define BF_JTX_TEST_USER_GO_INFO 0x00000271, 0x00000107

#define REG_JTX_TEST_USER_DATA0_ADDR 0x00000272

#define REG_JTX_CORE_SYNC_N_SEL_ADDR 0x0000027B
#define BF_JTX_SYNC_N_SEL_INFO 0x0000027B, 0x00000300

#define REG_JTX_CORE_13_ADDR 0x0000027C
#define BF_JTX_LINK_EN_INFO 0x0000027C, 0x00000100

#define REG_JTX_TPL_CONFIG0_ADDR 0x0000027D
#define BF_JTX_TPL_ADAPTIVE_LATENCY_INFO 0x0000027D, 0x00000100
#define BF_JTX_TPL_TEST_ENABLE_INFO 0x0000027D, 0x00000101
#define BF_JTX_TPL_CONV_ASYNCHRONOUS_INFO 0x0000027D, 0x00000102
#define BF_JTX_NS_CFG_INFO 0x0000027D, 0x00000503

#define REG_JTX_TPL_CONFIG1_ADDR 0x0000027E
#define BF_JTX_TPL_CFG_INVALID_INFO 0x0000027E, 0x00000100
#define BF_JTX_TPL_SYSREF_RCVD_INFO 0x0000027E, 0x00000101
#define BF_JTX_TPL_SYSREF_PHASE_ERR_INFO 0x0000027E, 0x00000102
#define BF_JTX_TPL_PHASE_ESTABLISHED_INFO 0x0000027E, 0x00000103
#define BF_JTX_TPL_SYSREF_MASK_INFO 0x0000027E, 0x00000105
#define BF_JTX_TPL_SYSREF_CLR_PHASE_ERR_INFO 0x0000027E, 0x00000106
#define BF_JTX_TPL_SYSREF_IGNORE_WHEN_LINKED_INFO 0x0000027E, 0x00000107

#define REG_JTX_TPL_LATENCY_ADJUST_ADDR 0x0000027F
#define BF_JTX_TPL_LATENCY_ADJUST_INFO 0x0000027F, 0x00000800

#define REG_JTX_TPL_PHASE_ADJUST0_ADDR 0x00000280
#define BF_JTX_TPL_PHASE_ADJUST_INFO 0x00000280, 0x00001000

#define REG_JTX_TPL_TEST_NUM_FRAMES0_ADDR 0x00000282
#define BF_JTX_TPL_TEST_NUM_FRAMES_M1_INFO 0x00000282, 0x00001000

#define REG_JTX_TPL_SYSREF_N_SHOT_ADDR 0x00000284
#define BF_JTX_TPL_SYSREF_N_SHOT_COUNT_INFO 0x00000284, 0x00000400
#define BF_JTX_TPL_SYSREF_N_SHOT_ENABLE_INFO 0x00000284, 0x00000104

#define REG_JTX_TPL_BUF_FRAMES_ADDR 0x00000285
#define BF_JTX_TPL_BUF_FRAMES_INFO 0x00000285, 0x00000800

#define REG_JTX_L0_DID_ADDR 0x00000286
#define BF_JTX_DID_CFG_INFO 0x00000286, 0x00000800

#define REG_JTX_L0_ADJCNT_BID_ADDR 0x00000287
#define BF_JTX_BID_CFG_INFO 0x00000287, 0x00000400
#define BF_JTX_ADJCNT_CFG_INFO 0x00000287, 0x00000404

#define REG_JTX_L0_ADJDIR_PHADJ_LID_ADDR 0x00000288
#define BF_JTX_PHADJ_CFG_INFO 0x00000288, 0x00000105
#define BF_JTX_ADJDIR_CFG_INFO 0x00000288, 0x00000106

#define REG_JTX_L0_SCR_L_ADDR 0x00000289
#define BF_JTX_L_CFG_INFO 0x00000289, 0x00000500
#define BF_JTX_SCR_CFG_INFO 0x00000289, 0x00000107

#define REG_JTX_L0_F_ADDR 0x0000028A
#define BF_JTX_F_CFG_INFO 0x0000028A, 0x00000800

#define REG_JTX_L0_K_ADDR 0x0000028B
#define BF_JTX_K_CFG_INFO 0x0000028B, 0x00000800

#define REG_JTX_L0_M_ADDR 0x0000028C
#define BF_JTX_M_CFG_INFO 0x0000028C, 0x00000800

#define REG_JTX_L0_CS_N_ADDR 0x0000028D
#define BF_JTX_N_CFG_INFO 0x0000028D, 0x00000500
#define BF_JTX_CS_CFG_INFO 0x0000028D, 0x00000206

#define REG_JTX_L0_SUBCLASSV_NP_ADDR 0x0000028E
#define BF_JTX_NP_CFG_INFO 0x0000028E, 0x00000500
#define BF_JTX_SUBCLASSV_CFG_INFO 0x0000028E, 0x00000305

#define REG_JTX_L0_JESDV_S_ADDR 0x0000028F
#define BF_JTX_S_CFG_INFO 0x0000028F, 0x00000500
#define BF_JTX_JESDV_CFG_INFO 0x0000028F, 0x00000305

#define REG_JTX_L0_HD_ADDR 0x00000290
#define BF_JTX_HD_CFG_INFO 0x00000290, 0x00000107

#define REG_JTX_L0_CHKSUM_ADDR(n) (0x00000293 + 1 * n)
#define BF_JTX_CHKSUM_CFG_INFO(n) (0x00000293 + 1 * n), 0x00000800

#define REG_JTX_L0_LID_ADDR(n) (0x00000297 + 1 * n)
#define BF_JTX_LID_CFG_INFO(n) (0x00000297 + 1 * n), 0x00000500

#define REG_JTX_DL_204B_CONFIG0_ADDR 0x000002A3
#define BF_JTX_DL_204B_BYP_ACG_CFG_INFO 0x000002A3, 0x00000100
#define BF_JTX_DL_204B_BYP_8B10B_CFG_INFO 0x000002A3, 0x00000101
#define BF_JTX_DL_204B_ILAS_TEST_EN_CFG_INFO 0x000002A3, 0x00000102
#define BF_JTX_DL_204B_BYP_ILAS_CFG_INFO 0x000002A3, 0x00000103
#define BF_JTX_DL_204B_ILAS_DELAY_CFG_INFO 0x000002A3, 0x00000404

#define REG_JTX_DL_204B_CONFIG1_ADDR 0x000002A4
#define BF_JTX_DL_204B_10B_MIRROR_INFO 0x000002A4, 0x00000100
#define BF_JTX_DL_204B_DEL_SCR_CFG_INFO 0x000002A4, 0x00000101
#define BF_JTX_DL_204B_LSYNC_EN_CFG_INFO 0x000002A4, 0x00000102

#define REG_JTX_DL_204B_CONFIG2_ADDR 0x000002A5
#define BF_JTX_DL_204B_RJSPAT_EN_CFG_INFO 0x000002A5, 0x00000100
#define BF_JTX_DL_204B_RJSPAT_SEL_CFG_INFO 0x000002A5, 0x00000201
#define BF_JTX_DL_204B_TPL_TEST_EN_CFG_INFO 0x000002A5, 0x00000104
#define BF_JTX_DL_204B_TESTMODE_IGNORE_SYNCN_CFG_INFO 0x000002A5, 0x00000105

#define REG_JTX_DL_204B_KF_ILAS_ADDR 0x000002A6
#define BF_JTX_DL_204B_KF_ILAS_CFG_INFO 0x000002A6, 0x00000800

#define REG_JTX_DL_204B_STATE_ADDR 0x000002A7
#define BF_JTX_DL_204B_STATE_INFO 0x000002A7, 0x00000400

#define REG_JTX_DL_204B_SYNC_N_ADDR 0x000002A8
#define BF_JTX_DL_204B_SYNC_N_FORCE_VAL_INFO 0x000002A8, 0x00000100
#define BF_JTX_DL_204B_SYNC_N_FORCE_EN_INFO 0x000002A8, 0x00000101
#define BF_JTX_DL_204B_SYNC_N_INFO 0x000002A8, 0x00000107

#define REG_JTX_DL_204B_CLEAR_SYNC_NE_COUNT_ADDR 0x000002A9
#define BF_JTX_DL_204B_CLEAR_SYNC_NE_COUNT_INFO 0x000002A9, 0x00000100

#define REG_JTX_DL_204B_SYNC_NE_COUNT_ADDR 0x000002AA
#define BF_JTX_DL_204B_SYNC_NE_COUNT_INFO 0x000002AA, 0x00000800

#define REG_JTX_DL_204B_LANE_CONFIG_ADDR(n) (0x000002AB + 1 * n)
#define BF_JTX_DL_204B_L_EN_CFG_INFO(n) (0x000002AB + 1 * n), 0x00000100
#define BF_JTX_DL_204B_PHY_DATA_SEL_CFG_INFO(n) (0x000002AB + 1 * n), 0x00000102
#define BF_JTX_DL_204B_SCR_DATA_SEL_CFG_INFO(n) (0x000002AB + 1 * n), 0x00000103
#define BF_JTX_DL_204B_SCR_IN_CTRL_CFG_INFO(n) (0x000002AB + 1 * n), 0x00000104

#define REG_JTX_PHY_IFX_LANE_CONFIG_ADDR(n) (0x000002C9 + 1 * n)
#define BF_JTX_BR_LOG2_RATIO_INFO(n) (0x000002C9 + 1 * n), 0x00000400
#define BF_JTX_LANE_FIFO_WR_ENTRIES_INFO(n) (0x000002C9 + 1 * n), 0x00000404

#endif /* __ADI_AD9083_BF_JTX_IP_H__ */
       /*! @} */