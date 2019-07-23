// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad9208_reg.h
 *
 * \brief AD9208 SPI Register Definition Header File
 *
 * Release 1.0.X
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */
#ifndef __AD9208_REGISTERS_H__
#define __AD9208_REGISTERS_H__

#include "api_def.h"

int ad9208_register_write_tbl(ad9208_handle_t *h,
			      struct adi_reg_data *tbl, uint32_t count);

int ad9208_register_read_block(ad9208_handle_t *h,
			       const uint16_t address, uint8_t *data,
			       uint32_t count);
int ad9208_adc_select_ch(ad9208_handle_t *h, uint8_t ch);
int ad9208_register_chip_transfer(ad9208_handle_t *h);
int ad9208_is_sync_spi_update_enabled(ad9208_handle_t *h, uint8_t *enabled);

/*Device DEFINITION */
#define AD9208_JESD_NOF_LANES         8
#define AD9208_JESD_NOF_LINKS         2
#define AD9208_JESD_NOF_SYNCOUTB      2
#define AD9208_ADC_NOF_CH             2
#define AD9208_ADC_CH_INDEX(x)          (BIT(x)>>1)
/*REGISTER SUMMARY : (AD9208_REGMAP_V4)*/
#define AD9208_IF_CFG_A_REG           0x000
#define AD9208_IF_CFG_B_REG           0x001
#define AD9208_DEV_CFG_REG            0x002
#define AD9208_PDN_MODE(x)            (((x)&0x3)<<0)

#define AD9208_CHIP_TYPE_REG          0x003
#define AD9208_PROD_ID_LSB_REG        0x004
#define AD9208_PROD_ID_MSB_REG        0x005
#define AD9208_CHIP_GRADE_REG         0x006
#define AD9208_CH_INDEX_REG           0x008
#define AD9208_CH_INDEX_SEL(x)        (((x)&0x3)<<0)
#define AD9208_CHANNEL_PAGE_0         BIT(0)
#define AD9208_CHANNEL_PAGE_1         BIT(1)
#define AD9208_CHANNEL_PAGE_2         BIT(2)
#define AD9208_CHANNEL_PAGE_3         BIT(3)
#define AD9208_CHANNEL_PAGE_4         BIT(4)
#define AD9208_CHANNEL_PAGE_5         BIT(5)
#define AD9208_MAINDAC_PAGE_0         BIT(6)
#define AD9208_MAINDAC_PAGE_1         BIT(7)

#define AD9208_CHIP_SPI_XFER_REG     0x000F
#define AD9208_CHIP_TRIGGER_SPI_XFER  BIT(0)

#define AD9208_CHIP_PIN_CTRL0_REG     0x03F
#define AD9208_CHIP_PDN_PIN_DISABLE   BIT(7)
#define AD9208_CHIP_PIN_CTRL1_REG     0x040
#define AD9208_CHIP_PDN_MODE(x)       (((x)&0xC0)<<6)
#define AD9208_CHIP_PIN_CTRL_MASK(x)  (0x07 << (3 * (x)))

#define AD9208_IP_CLK_CFG_REG         0x0108
#define AD9208_IP_CLK_DIV(x)          (((x)&0x3)<<0)
#define AD9208_IP_CLK_PHASE_ADJ_REG   0x0109
#define AD9208_IP_CLK_PHASE_ADJ(x)    (((x)&0xF)<<0)

#define AD9208_IP_CLK_STAT_REG         0x011B
#define AD9208_IP_CLK_DCS1_REG         0x011C
#define AD9208_IP_CLK_DCS2_REG         0x011E

#define AD9208_SYSREF_CTRL_0_REG       0x0120
#define AD9208_SYSREF_MODE_SEL(x)      (((x)&0x3)<<1)
#define AD9208_SYSREF_CTRL_1_REG       0x0121
#define AD9208_SYSREF_TRANSITION_SEL(x) (((x)&0x1)<<4)
#define AD9208_SYSREF_CLK_EDGE_SEL(x)   (((x)&0x1)<<3)
#define AD9208_SYSREF_NSHOT_IGNORE(x)  (((x)&0x3)<<0)
#define AD9208_SYSREF_CTRL_2_REG       0x0122
#define AD9208_SYSREF_WIN_NEG(x)       (((x)&0x3)<<2)
#define AD9208_SYSREF_WIN_POS(x)       (((x)&0x3)<<0)
#define AD9208_SYSREF_CTRL_3_REG       0x0123
#define AD9208_SYSREF_TS_DELAY(x)      (((x)&0x7F)<<0)

#define AD9208_SYSREF_STAT_0_REG       0x0128
#define AD9208_SYSREF_STAT_1_REG       0x0129
#define AD9208_SYSREF_STAT_2_REG       0x012A

#define AD9208_BUFF_CFG_P_REG         0x1A4C
#define AD9208_BUFF_CTRL_P(x)         (((x)&0x3F)<<0)
#define AD9208_BUFF_CFG_N_REG         0x1A4D
#define AD9208_BUFF_CTRL_N(x)         (((x)&0x3F)<<0)

#define AD9208_CHIP_SYNC_MODE_REG     0x01FF
#define AD9208_SYNC_TS_ENABLE	      BIT(0)

#define AD9208_ADC_MODE_REG           0x0200
#define AD9208_ADC_MODE(x)            (((x)&0x3)<<0)
#define AD9208_ADC_Q_IGNORE           BIT(5)

#define AD9208_ADC_DCM_REG            0x0201
#define AD9208_ADC_DCM_RATE(x)        (((x)&0xF)<<0)
#define AD9208_DCM_NONE               0x0
#define AD9208_DCM2_EN                BIT(0)
#define AD9208_DCM4_EN                BIT(1)
#define AD9208_DCM16_EN               BIT(2)
#define AD9208_DCM3_EN                BIT(3)

#define AD9208_FD_UT_LSB_REG           0x0247
#define AD9208_FD_UT_LSB(x)            (((x)&0xFF)<<0)
#define AD9208_FD_UT_MSB_REG           0x0248
#define AD9208_FD_UT_MSB(x)            (((x>>8)&0x1F)<<0)
#define AD9208_FD_LT_LSB_REG           0x0249
#define AD9208_FD_LT_LSB(x)            (((x)&0xFF)<<0)
#define AD9208_FD_LT_MSB_REG           0x024A
#define AD9208_FD_LT_MSB(x)            (((x>>8)&0x1F)<<0)

#define AD9208_FD_DWELL_LSB_REG        0x024B
#define AD9208_FD_DWELL_LSB(x)         (((x)&0xFF)<<0)
#define AD9208_FD_DWELL_MSB_REG        0x024C
#define AD9208_FD_DWELL_MSB(x)         (((x>>8)&0xFF)<<0)

#define AD9208_DDC_SYNC_CTRL_REG       0x0300
#define AD9208_DDC_UPDATE_MODE         BIT(7)
#define AD9208_NCO_SOFT_RESET          BIT(4)
#define AD9208_NCO_SYSREF_N_SHOT_MODE  BIT(1)
#define AD9208_NCO_SYSREF_SYNC_EN      BIT(0)

#define AD9208_DDCX_REG_OFFSET        0x20
#define AD9208_DDCX_CTRL0_REG         0x0310
#define AD9208_DDCX_MIXER_SEL         BIT(7)
#define AD9208_DDCX_GAIN_SEL          BIT(6)
#define AD9208_DDCX_NCO_IF_MODE(x)    (((x)&0x3)<<4)
#define AD9208_DDCX_COMPLEX_TO_REAL   BIT(3)
#define AD9208_DDCX_DCM_FILT_SEL_0(x)   (((x)&0x7)<<0)

#define AD9208_DDCX_DATA_SEL_REG      0x0311
#define AD9208_DDCX_DCM_FILT_SEL_1(x)  (((x)&0xF)<<4)
#define AD9208_DDCX_Q_IP_CHB_SEL      BIT(2)
#define AD9208_DDCX_I_IP_CHB_SEL      BIT(0)

#define AD9208_DDCX_FTW0_REG          0x0316
#define AD9208_DDCX_FTW1_REG          0x0317
#define AD9208_DDCX_FTW2_REG          0x0318
#define AD9208_DDCX_FTW3_REG          0x0319
#define AD9208_DDCX_FTW4_REG          0x031A
#define AD9208_DDCX_FTW5_REG          0x031B

#define AD9208_DDCX_PO0_REG          0x031D
#define AD9208_DDCX_PO1_REG          0x031E
#define AD9208_DDCX_PO2_REG          0x031F
#define AD9208_DDCX_PO3_REG          0x0320
#define AD9208_DDCX_PO4_REG          0x0321
#define AD9208_DDCX_PO5_REG          0x0322

#define AD9208_DDCX_FRAC_REG_OFFSET   0x10
#define AD9208_DDCX_MAW0_REG          0x0390
#define AD9208_DDCX_MAW1_REG          0x0391
#define AD9208_DDCX_MAW2_REG          0x0392
#define AD9208_DDCX_MAW3_REG          0x0393
#define AD9208_DDCX_MAW4_REG          0x0394
#define AD9208_DDCX_MAW5_REG          0x0395

#define AD9208_DDCX_MBW0_REG          0x0398
#define AD9208_DDCX_MBW1_REG          0x0399
#define AD9208_DDCX_MBW2_REG          0x039A
#define AD9208_DDCX_MBW3_REG          0x039B
#define AD9208_DDCX_MBW4_REG          0x039C
#define AD9208_DDCX_MBW5_REG          0x039D

#define AD9208_REG_TEST_MODE			0x0550

#define AD9208_TESTMODE_OFF			0x0
#define AD9208_TESTMODE_MIDSCALE_SHORT		0x1
#define AD9208_TESTMODE_POS_FULLSCALE		0x2
#define AD9208_TESTMODE_NEG_FULLSCALE		0x3
#define AD9208_TESTMODE_ALT_CHECKERBOARD	0x4
#define AD9208_TESTMODE_PN23_SEQ		0x5
#define AD9208_TESTMODE_PN9_SEQ			0x6
#define AD9208_TESTMODE_ONE_ZERO_TOGGLE		0x7
#define AD9208_TESTMODE_USER			0x8
#define AD9208_TESTMODE_RAMP			0xF

#define AD9208_REG_OUTPUT_MODE			0x0561
#define AD9208_OUTPUT_MODE_OFFSET_BINARY	0x0
#define AD9208_OUTPUT_MODE_TWOS_COMPLEMENT	0x1

#define AD9208_OP_MODE_CTRL_1_REG     0x0559
#define AD9208_OP_MODE_CTRL_2_REG     0x055A
#define AD9208_OP_CONV_CTRL_BIT_SEL(x) (((x)&0xF)>>0)
#define AD9208_OP_OVERANGE_CLR_REG   0x0562
#define AD9208_OP_OVERANGE_STAT_REG   0x0563

#define AD9208_JESD_LMFC_OFFSET_REG   0x0578
#define AD9208_JESD_LMFC_OFFSET(x)    (((x)&0x1F)>>0)

#define AD9208_JESD_SERDES_PLL_CFG_REG 0x056E
#define AD9208_JESD_SLR_CTRL(x)       (((x)&0xF)<<4)

#define AD9208_JESD_SERDES_PLL_REG    0x056F
#define AD9208_JESD_PLL_LOCK_STAT     BIT(7)

#define AD9208_JESD_LINK_CTRL1_REG    0x0571
#define AD9208_JESD_LINK_PDN          BIT(0)

#define AD9208_JESD_ID_CFG_REG_OFFSET 0x3
#define AD9208_JESD_DID_CFG_REG       0x0580
#define AD9208_JESD_BID_CFG_REG       0x0581
#define AD9208_JESD_BID(x)            (((x)&0xF)<<0)
#define AD9208_JESD_LID0_CFG_REG      0x0583
#define AD9208_JESD_LID0(x)           (((x)&0x1F)<<0)

#define AD9208_JESD_CFG_REG_OFFSET    0x8
#define AD9208_JESD_L_SCR_CFG_REG     0x058B
#define AD9208_JESD_SCR_EN            BIT(7)
#define AD9208_JESD_LANES(x)          (((x)&0x1F)<<0)

#define AD9208_JESD_F_CFG_REG         0x058C
#define AD9208_JESD_F(x)              (((x)&0xF)<<0)

#define AD9208_JESD_K_CFG_REG         0x058D
#define AD9208_JESD_K(x)              (((x)&0x1F)<<0)

#define AD9208_JESD_M_CFG_REG         0x058E
#define AD9208_JESD_M(x)              (((x)&0x7)<<0)

#define AD9208_JESD_CS_N_CFG_REG      0x058F
#define AD9208_JESD_CS(x)             (((x)&0x3)<<6)
#define AD9208_JESD_N(x)              (((x)&0x1F)<<0)

#define AD9208_JESD_SCV_NP_CFG_REG     0x0590
#define AD9208_JESD_SUBCLASS(x)        (((x)&0x7)<<5)
#define AD9208_JESD_NP(x)              (((x)&0x1F)<<0)
#define AD9208_JESD_S_CFG_REG          0x0591
#define AD9208_JESD_S(x)              (((x)&0x1F)<<0)

#define AD9208_JESD_HD_CF_CFG_REG      0x0592
#define AD9208_JESD_HD                 BIT(7)
#define AD9208_JESD_CF(x)              (((x)&0x1F)<<0)

#define AD9208_JESD_XBAR_CFG_REG_OFFSET 0x5
#define AD9208_JESD_XBAR_CFG_REG       0x05B2
#define AD9208_JESD_XBAR_LN_EVEN(x)    (((x)&0x7) << 0)
#define AD9208_JESD_XBAR_LN_ODD(x)     (((x)&0x7) << 4)

#define AD9208_DC_OFFSET_CAL_CTRL      0x0701
#define AD9208_DC_OFFSET_CAL_EN        BIT(7)

#define AD9208_VREF_CTRL_REG          0x18A6
#define AD9208_EXT_VREF_MODE          BIT(0)

#define AD9208_EXT_VCM_CTRL_REG      0x18E3
#define AD9208_EXT_VCM_BUFF          BIT(6)
#define AD9208_EXT_VCM_BUFF_CURR(x)  (((x)&0x3F) << 0)

#define AD9208_TEMP_DIODE_CTRL_REG    0x18E6
#define AD9208_CENTRAL_DIODE_1X_EN    BIT(0)
#define AD9208_CENTRAL_DIODE_20X_EN   BIT(1)

#define AD9208_ANALOG_CFG_REG         0x1908
#define AD9208_DC_COUPLE_EN(x)        (((x)&0x1)<<2)

#define AD9208_FULL_SCALE_CFG_REG     0x1910
#define AD9208_TRM_VREF(x)            (((x)&0xF)<<0)

#endif /*__AD9208_REG_H__*/
