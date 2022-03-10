// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad917x_reg.h
 *
 * \brief AD917x SPI Register Definition Header File
 *
 * release 1.1.X
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */
#ifndef __AD917X_REGISTERS_H__
#define __AD917X_REGISTERS_H__

#include "api_def.h"

int32_t ad917x_register_write_tbl(ad917x_handle_t *h,
				  struct adi_reg_data *tbl, uint32_t count);

int32_t ad917x_register_read_block(ad917x_handle_t *h,
				   const uint16_t address, uint8_t *data, uint32_t count);

/*Device DEFINITION */
#define AD917X_JESD_NOF_LANES         8
#define AD917X_JESD_NOF_LINKS         2
#define AD917X_JESD_NOF_SYNCOUTB      2
#define AD9171_ID		      9171
#define AD9172_ID		      9172
#define AD9173_ID		      9173

/*REGISTER SUMMARY : (AD917X_REGMAP_V4)*/
#define AD917X_IF_CFG_A_REG           0x000
#define AD917X_IF_CFG_B_REG           0x001
#define AD917X_DEV_CFG_REG            0x002
#define AD917X_CHIP_TYPE_REG          0x003
#define AD917X_PROD_ID_LSB_REG        0x004
#define AD917X_PROD_ID_MSB_REG        0x005
#define AD917X_CHIP_GRADE_REG         0x006
#define AD917X_SPI_PAGEINDX_REG       0x008
#define AD917X_CHANNEL_PAGE_0         NO_OS_BIT(0)
#define AD917X_CHANNEL_PAGE_1         NO_OS_BIT(1)
#define AD917X_CHANNEL_PAGE_2         NO_OS_BIT(2)
#define AD917X_CHANNEL_PAGE_3         NO_OS_BIT(3)
#define AD917X_CHANNEL_PAGE_4         NO_OS_BIT(4)
#define AD917X_CHANNEL_PAGE_5         NO_OS_BIT(5)
#define AD917X_MAINDAC_PAGE_0         NO_OS_BIT(6)
#define AD917X_MAINDAC_PAGE_1         NO_OS_BIT(7)

#define AD917X_SYSREF_ROTATION_REG    0x03B
#define AD917X_SYNC_LOGIC_EN          NO_OS_BIT(7)
#define AD917X_SYNC_RSV_EN            NO_OS_BIT(6)
#define AD917X_PERIODIC_RST_EN        NO_OS_BIT(5)
#define AD917X_NCORST_AFTER_ROTATION  NO_OS_BIT(4)
#define AD917X_ROTATION_MODE(x)       (((x) & 0x3) << 0)



#define AD917X_SYSREF_CTRL_REG         0x084
#define AD917X_SYSREF_PD               NO_OS_BIT(0)
#define AD917X_SYSREF_DC_COUPLED       NO_OS_BIT(6)

#define AD917X_PLL_VCO_CTRL_REG        0x094
#define AD917X_PLL_VCO_DIV_EN(x)       (((x) & 0x3) << 0)

#define AD917X_PLL_BYPASS_REG         0x095
#define AD917X_PLL_BYPASS(x)          ((x) ? NO_OS_BIT(0) : 0)

#define AD917X_DLL_CTRL0_REG          0x0C1
#define AD917X_DLL_CFG                NO_OS_BIT(6) | NO_OS_BIT(3)
#define AD917X_DLL_HF                 NO_OS_BIT(5)
#define AD917X_DLL_RST                NO_OS_BIT(0)
#define AD917X_DLL_STATUS_REG         0x0C3
#define AD917X_DLL_LOCK               NO_OS_BIT(0)

#define AD917X_DIG_RESET_REG          0x100
#define AD917X_DIG_PATH_PDN(x)        ((x) ? NO_OS_BIT(0) : 0)

#define AD917X_JESD_MODE_REG          0x110
#define AD917X_JESD_MODE_INVALID      NO_OS_BIT(7)
#define AD917X_LINK_MODE              NO_OS_BIT(5)
#define AD917X_JESD_MODE(x)           (((x) & 0x1F) << 0)

#define AD917X_INTERP_MODE_REG        0x111
#define AD917X_CH_INTERP_MODE(x)      (((x) & 0xF) << 0)
#define AD917X_DP_INTERP_MODE(x)      (((x) & 0xF) << 4)

#define AD917X_DDSM_DATAPATH_CFG_REG  0x112
#define AD917X_DDSM_MODE(x)           (((x) & 0x3) << 4)
#define AD917X_DDSM_NCO_EN            NO_OS_BIT(3)
#define AD917X_DDSM_MODULUS_EN        NO_OS_BIT(2)
#define AD917X_DDSM_SEL_SIDEBAND      NO_OS_BIT(1)
#define AD917X_DDSM_EN_SYNC_ALL_CHNL_NCO_RESETS NO_OS_BIT(0)

#define AD917X_DDSM_FTW_UPDATE_REG    0x113
#define AD917X_DDSM_FTW_LOAD_SYSREF   NO_OS_BIT(2)
#define AD917X_DDSM_FTW_LOAD_ACK      NO_OS_BIT(1)
#define AD917X_DDSM_FTW_LOAD_REQ      NO_OS_BIT(0)

#define AD917X_DDSM_FTW0_REG          0x114
#define AD917X_DDSM_FTW1_REG          0x115
#define AD917X_DDSM_FTW2_REG          0x116
#define AD917X_DDSM_FTW3_REG          0x117
#define AD917X_DDSM_FTW4_REG          0x118
#define AD917X_DDSM_FTW5_REG          0x119

#define AD917X_DDSM_PHASE_OFFSET0_REG 0x11C
#define AD917X_DDSM_PHASE_OFFSET1_REG 0x11D

#define AD917X_DDSM_ACC_MODULUS0_REG  0x124
#define AD917X_DDSM_ACC_MODULUS1_REG  0x125
#define AD917X_DDSM_ACC_MODULUS2_REG  0x126
#define AD917X_DDSM_ACC_MODULUS3_REG  0x127
#define AD917X_DDSM_ACC_MODULUS4_REG  0x128
#define AD917X_DDSM_ACC_MODULUS5_REG  0x129

#define AD917X_DDSM_ACC_DELTA0_REG    0x12A
#define AD917X_DDSM_ACC_DELTA1_REG    0x12B
#define AD917X_DDSM_ACC_DELTA2_REG    0x12C
#define AD917X_DDSM_ACC_DELTA3_REG    0x12D
#define AD917X_DDSM_ACC_DELTA4_REG    0x12E
#define AD917X_DDSM_ACC_DELTA5_REG    0x12F

#define AD917X_DDSC_DATAPATH_CFG_REG  0x130
#define AD917X_DDSC_NCO_EN            NO_OS_BIT(6)
#define AD917X_DDSC_MODULUS_EN        NO_OS_BIT(2)
#define AD917X_DDSC_SEL_SIDEBAND      NO_OS_BIT(1)
#define AD917X_DDSC_TEST_TONE_EN      NO_OS_BIT(0)

#define AD917X_DDSC_FTW_UPDATE_REG    0x131
#define AD917X_DDSC_FTW_LOAD_SYSREF   NO_OS_BIT(2)
#define AD917X_DDSC_FTW_LOAD_ACK      NO_OS_BIT(1)
#define AD917X_DDSC_FTW_LOAD_REQ      NO_OS_BIT(0)

#define AD917X_DDSC_FTW0_REG          0x132
#define AD917X_DDSC_FTW1_REG          0x133
#define AD917X_DDSC_FTW2_REG          0x134
#define AD917X_DDSC_FTW3_REG          0x135
#define AD917X_DDSC_FTW4_REG          0x136
#define AD917X_DDSC_FTW5_REG          0x137

#define AD917X_DDSC_PHASE_OFFSET0_REG 0x138
#define AD917X_DDSC_PHASE_OFFSET1_REG 0x139

#define AD917X_DDSC_ACC_MODULUS0_REG  0x13A
#define AD917X_DDSC_ACC_MODULUS1_REG  0x13B
#define AD917X_DDSC_ACC_MODULUS2_REG  0x13C
#define AD917X_DDSC_ACC_MODULUS3_REG  0x13D
#define AD917X_DDSC_ACC_MODULUS4_REG  0x13E
#define AD917X_DDSC_ACC_MODULUS5_REG  0x13F

#define AD917X_DDSC_ACC_DELTA0_REG    0x140
#define AD917X_DDSC_ACC_DELTA1_REG    0x141
#define AD917X_DDSC_ACC_DELTA2_REG    0x142
#define AD917X_DDSC_ACC_DELTA3_REG    0x143
#define AD917X_DDSC_ACC_DELTA4_REG    0x144
#define AD917X_DDSC_ACC_DELTA5_REG    0x145

#define AD917X_X_FTW_UPDATE_REG(x)    ((x)==AD917X_DDSM?0x113:0x131)

#define AD917X_X_FTW0_REG(x)          ((x)==AD917X_DDSM?0x114:0x132)
#define AD917X_X_FTW1_REG(x)          ((x)==AD917X_DDSM?0x115:0x133)
#define AD917X_X_FTW2_REG(x)          ((x)==AD917X_DDSM?0x116:0x134)
#define AD917X_X_FTW3_REG(x)          ((x)==AD917X_DDSM?0x117:0x135)
#define AD917X_X_FTW4_REG(x)          ((x)==AD917X_DDSM?0x118:0x136)
#define AD917X_X_FTW5_REG(x)          ((x)==AD917X_DDSM?0x119:0x137)

#define AD917X_X_PHASE_OFFSET0_REG(x) ((x)==AD917X_DDSM?0x11C:0x138)
#define AD917X_X_PHASE_OFFSET1_REG(x) ((x)==AD917X_DDSM?0x11D:0x139)

#define AD917X_X_ACC_MODULUS0_REG(x)  ((x)==AD917X_DDSM?0x124:0x13A)
#define AD917X_X_ACC_MODULUS1_REG(x)  ((x)==AD917X_DDSM?0x125:0x13B)
#define AD917X_X_ACC_MODULUS2_REG(x)  ((x)==AD917X_DDSM?0x126:0x13C)
#define AD917X_X_ACC_MODULUS3_REG(x)  ((x)==AD917X_DDSM?0x127:0x13D)
#define AD917X_X_ACC_MODULUS4_REG(x)  ((x)==AD917X_DDSM?0x128:0x13E)
#define AD917X_X_ACC_MODULUS5_REG(x)  ((x)==AD917X_DDSM?0x129:0x13F)

#define AD917X_X_ACC_DELTA0_REG(x)    ((x)==AD917X_DDSM?0x12A:0x140)
#define AD917X_X_ACC_DELTA1_REG(x)    ((x)==AD917X_DDSM?0x12B:0x141)
#define AD917X_X_ACC_DELTA2_REG(x)    ((x)==AD917X_DDSM?0x12C:0x142)
#define AD917X_X_ACC_DELTA3_REG(x)    ((x)==AD917X_DDSM?0x12D:0x143)
#define AD917X_X_ACC_DELTA4_REG(x)    ((x)==AD917X_DDSM?0x12E:0x144)
#define AD917X_X_ACC_DELTA5_REG(x)    ((x)==AD917X_DDSM?0x12F:0x145)

#define AD917X_CHNL_GAIN0_REG          0x146
#define AD917X_CHNL_GAIN1_REG          0x147
#define CHANNEL_GAIN0(x)               (uint8_t)((x) & 0xFF)
#define CHANNEL_GAIN1(x)               (uint8_t)(((x) >> 8) & 0x0F)
#define CHANNEL_GAIN(g0, g1)           (uint16_t)(((uint16_t)((g1) << 8) | (g0)) & 0xFFF)

#define AD917X_DC_CAL_TONE0_REG          0x148
#define AD917X_DC_CAL_TONE1_REG          0x149

#define AD917X_DDSM_CAL_MODE_DEF_REG     0x1E6
#define AD917X_DDSM_EN_CAL_ACC           NO_OS_BIT(2)
#define AD917X_DDSM_EN_CAL_DC_INPUT      NO_OS_BIT(1)
#define AD917X_DDSM_EN_CAL_FREQ_TUNE     NO_OS_BIT(0)


#define AD917X_MASTER_PD_REG           0x200
#define AD917X_SERDES_PDN(x)           ((x) ? NO_OS_BIT(0) : 0)
#define AD917X_PHY_PD_REG              0x201
#define AD917X_PLL_EN_CTRL_REG         0x280
#define AD917X_SERDES_PLL_STARTUP      NO_OS_BIT(0)

#define AD917X_GEN_PD_REG              0x203
#define AD917X_SYNCOUTB_0_PD           NO_OS_BIT(1)
#define AD917X_SYNCOUTB_1_PD           NO_OS_BIT(0)
#define AD917X_SYNCOUTB_PD(x)          (((x) & 0x3) << 0)

#define AD917X_SYNCOUTB_CTRL_0_REG     0x253
#define AD917X_SYNCOUTB_CTRL_1_REG     0x254
#define AD917X_SYNCOUTB_MODE(x)        ((x) ? NO_OS_BIT(0) : 0)

#define AD917X_PLL_STATUS_REG          0x281

#define AD917X_JESD_RX_CTL_REG         0x300
#define AD917X_DUAL_MODE               NO_OS_BIT(3)
#define AD917X_LINK_PAGE(x)            ((x) ? NO_OS_BIT(2) : 0)
#define AD917X_LINK_EN(x)              (((x) & 0x3) << 0)
#define AD917X_LINK_0_EN               NO_OS_BIT(0)
#define AD917X_LINK_1_EN               NO_OS_BIT(1)

#define AD917X_JESD_LMFC_DELAY0_REG    0x304
#define AD917X_JESD_LMFC_DELAY1_REG    0x305
#define AD917X_JESD_LMFC_DELAY(x)      (((x) & 0x3F) << 0)
#define AD917X_JESD_LMFC_VAR0_REG      0x306
#define AD917X_JESD_LMFC_VAR1_REG      0x307
#define AD917X_JESD_LMFC_VAR(x)        (((x) & 0x3F) << 0)

#define AD917X_JESD_XBAR_LANE_REG      0x308
#define AD917X_JESD_XBAR_LANE_REG      0x308
#define AD917X_JESD_XBAR_LANE_REG      0x308

#define AD917X_JESD_XBAR_LANE_REG      0x308
#define AD917X_XBAR_LANE_EVEN(x)       (((x) & 0x7) << 0)
#define AD917X_XBAR_LANE_ODD(x)        (((x) & 0x7) << 3)

#define AD917X_JESD_INVERT_LANE_REG    0x334
#define AD917X_JESD_INVERT_LANE(x)     NO_OS_BIT(x)

#define AD917X_JESD_PARAM_REG_BASE     0x450
#define AD917X_JESD_PARAM_REG_LEN      0xB
#define AD917X_JESD_L_GET(x)           ((x[3] & 0xF) + 1)
#define AD917X_JESD_F_GET(x)           ((x[4]) + 1)
#define AD917X_JESD_K_GET(x)           ((x[5] & 0x1F) + 1)
#define AD917X_JESD_M_GET(x)           ((x[6]) + 1)
#define AD917X_JESD_N_GET(x)           ((x[7] & 0xF) + 1)
#define AD917X_JESD_NP_GET(x)          ((x[8] & 0xF) + 1)
#define AD917X_JESD_S_GET(x)           ((x[9] & 0xF) + 1)
#define AD917X_JESD_HD_GET(x)          ((x[10] & 0x80) >> 7)
#define AD917X_JESD_DID_GET(x)         (x[0])
#define AD917X_JESD_BID_GET(x)         (x[1])
#define AD917X_JESD_LID0_GET(x)        (x[2] & 0xF)
#define AD917X_JESD_V_GET(x)           ((x[9] & 0xF8) >> 5)
#define AD917X_JESD_L(x)               (((x) & 0xF) << 0)
#define AD917X_JESD_NP(x)              (((x) & 0xF) << 0)

#define AD917X_JESD_ILS_SCR_L_REG      0x453
#define AD917X_JESD_SCR                NO_OS_BIT(7)
#define AD917X_JESD_ILS_NP_REG         0x458
#define AD917X_JESD_JESDV              NO_OS_BIT(5)

#define AD917X_JESD_CODE_GRP_SYNC_REG   0x470
#define AD917X_JESD_FRAME_SYNC_REG      0x471
#define AD917X_JESD_GOOD_CHECKSUM_REG   0x472
#define AD917X_JESD_INIT_LANE_SYNC_REG  0x473

#define AD917X_JESD_CTRL0_REG           0x475
#define AD917X_JESD_QBD_SOFT_RST        NO_OS_BIT(3)



#define AD917X_NVM_LOADER_REG          0x705
#define AD917X_NVM_BLR_DONE            NO_OS_BIT(1)


#define AD917X_DACPLL_CTRLX_REG        0x790
#define AD917X_DACPLL_CTRLY_REG        0x791
#define AD917X_DACPLL_CTRL0_REG        0x792
#define AD917X_RESET_VCO_DIV           NO_OS_BIT(1)

#define AD917X_DACPLL_CTRL1_REG        0x793
#define AD917X_M_DIV(x)                (((x) & 0x3) << 0)

#define AD917X_DACPLL_CTRL7_REG        0x799
#define AD917X_L_DIV(x)                (((x) & 0x3) << 6)
#define AD917X_N_DIV(x)                (((x) & 0x3F) << 0)


#define AD917X_DACPLL_STATUS_REG       0x7B5
#define AD917X_DACPLL_LOCK             NO_OS_BIT(0)

#endif /*__AD917X_REG_H__*/
