/***************************************************************************//**
*   @file   adin1320.h
*   @brief  Header file of the ADIN1320 driver.
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __ADIN1320_H__
#define __ADIN1320_H__

#include <stdbool.h>
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

/** ADIN1320 Register Map */

#define ADIN1320_MII_CONTROL		0x0000
#define ADIN1320_SFT_RST_MASK		NO_OS_BIT(15)
#define ADIN1320_LOOPBACK_MASK		NO_OS_BIT(14)
#define ADIN1320_SPEED_SEL_LSB_MASK	NO_OS_BIT(13)
#define ADIN1320_AUTONEG_EN_MASK	NO_OS_BIT(12)
#define ADIN1320_SFT_PD_MASK		NO_OS_BIT(11)
#define ADIN1320_RESTART_ANEG_MASK	NO_OS_BIT(9)
#define ADIN1320_DPLX_MODE_MASK		NO_OS_BIT(8)
#define ADIN1320_SPEED_SEL_MSB_MASK	NO_OS_BIT(6)

#define ADIN1320_MII_STATUS		0x0001
#define ADIN1320_AUTONEG_DONE_MASK	NO_OS_BIT(5)
#define ADIN1320_REM_FLT_LAT_MASK	NO_OS_BIT(4)
#define ADIN1320_LINK_STAT_LAT_MASK	NO_OS_BIT(2)
#define ADIN1320_JABBER_DET_LAT_MASK	NO_OS_BIT(1)

#define ADIN1320_PHY_ID_1		0x0002

#define ADIN1320_PHY_ID_2		0x0003
#define ADIN1320_MODEL_NUM_MASK		NO_OS_GENMASK(9, 4)
#define ADIN1320_REV_NUM_MASK		NO_OS_GENMASK(3, 0)

#define ADIN1320_AUTONEG_ADV		0x0004
#define ADIN1320_FD_100_ADV_MASK	NO_OS_BIT(8)
#define ADIN1320_HD_100_ADV_MASK	NO_OS_BIT(7)
#define ADIN1320_FD_10_ADV_MASK		NO_OS_BIT(6)
#define ADIN1320_HD_10_ADV_MASK		NO_OS_BIT(5)
#define ADIN1320_SELECTOR_ADV_MASK	NO_OS_GENMASK(4, 0)

#define ADIN1320_LP_ABILITY		0x0005
#define ADIN1320_AUTONEG_EXP		0x0006
#define ADIN1320_TX_NEXT_PAGE		0x0007
#define ADIN1320_LP_RX_NEXT_PAGE	0x0008

#define ADIN1320_MSTR_SLV_CONTROL	0x0009
#define ADIN1320_FD_1000_ADV_MASK	NO_OS_BIT(9)
#define ADIN1320_HD_1000_ADV_MASK	NO_OS_BIT(8)
#define ADIN1320_MAN_MSTR_ADV_MASK	NO_OS_BIT(11)
#define ADIN1320_MAN_MSTR_SLV_EN_ADV_MASK	NO_OS_BIT(12)

#define ADIN1320_MSTR_SLV_STATUS	0x000a
#define ADIN1320_MSTR_RSLVD_MASK	NO_OS_BIT(14)
#define ADIN1320_MSTR_SLV_FLT_MASK	NO_OS_BIT(15)

#define ADIN1320_EXT_STATUS		0x000f
#define ADIN1320_EXT_REG_PTR		0x0010
#define ADIN1320_EXT_REG_DATA		0x0011

#define ADIN1320_PHY_CTRL_1		0x0012
#define ADIN1320_AUTO_MDI_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_MAN_MDIX_MASK		NO_OS_BIT(9)
#define ADIN1320_DIAG_CLK_EN_MASK	NO_OS_BIT(2)

#define ADIN1320_PHY_CTRL_STATUS_1	0x0013
#define ADIN1320_LB_ALL_DIG_SEL_MASK	NO_OS_BIT(12)

#define ADIN1320_PHY_CTRL_STATUS_2	0x0015
#define ADIN1320_PHY_IN_NRG_PD_MASK	NO_OS_BIT(1)
#define ADIN1320_NRG_PD_TX_EN_MASK	NO_OS_BIT(2)
#define ADIN1320_NRG_PD_EN_MASK		NO_OS_BIT(3)

#define ADIN1320_PHY_CTRL_2		0x0016
#define ADIN1320_DN_SPEED_TO_100_EN_MASK	NO_OS_BIT(11)
#define ADIN1320_DN_SPEED_TO_10_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_CLK_CNTRL_MASK		NO_OS_GENMASK(3, 1)

#define ADIN1320_PHY_CTRL_3		0x0017
#define ADIN1320_NUM_SPEED_RETRY_MASK	NO_OS_GENMASK(12, 10)
#define ADIN1320_LINK_EN_MASK		NO_OS_BIT(13)

#define ADIN1320_IRQ_MASK		0x0018
#define ADIN1320_CBL_DIAG_IRQ_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_MDIO_SYNC_IRQ_EN_MASK	NO_OS_BIT(9)
#define ADIN1320_AN_STAT_CHNG_IRQ_EN_MASK	NO_OS_BIT(8)
#define ADIN1320_FC_FG_IRQ_EN_MASK	NO_OS_BIT(7)
#define ADIN1320_PAGE_RX_IRQ_EN_MASK	NO_OS_BIT(6)
#define ADIN1320_IDLE_ERR_CNT_IRQ_EN_MASK	NO_OS_BIT(5)
#define ADIN1320_FIFO_OU_IRQ_EN_MASK	NO_OS_BIT(4)
#define ADIN1320_RX_STAT_CHNG_IRQ_EN_MASK	NO_OS_BIT(3)
#define ADIN1320_LNK_STAT_CHNG_IRQ_EN_MASK	NO_OS_BIT(2)
#define ADIN1320_SPEED_CHG_IRQ_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_HW_IRQ_EN_MASK		NO_OS_BIT(0)

#define ADIN1320_IRQ_STATUS		0x0019
#define ADIN1320_CBL_DIAG_IRQ_STAT_MASK		NO_OS_BIT(10)
#define ADIN1320_MDIO_SYNC_IRQ_STAT_MASK	NO_OS_BIT(9)
#define ADIN1320_AN_STAT_CHNG_IRQ_STAT_MASK	NO_OS_BIT(8)
#define ADIN1320_FC_FG_IRQ_STAT_MASK		NO_OS_BIT(7)
#define ADIN1320_PAGE_RX_IRQ_STAT_MASK		NO_OS_BIT(6)
#define ADIN1320_IDLE_ERR_CNT_IRQ_STAT_MASK	NO_OS_BIT(5)
#define ADIN1320_FIFO_OU_IRQ_STAT_MASK		NO_OS_BIT(4)
#define ADIN1320_RX_STAT_CHNG_IRQ_STAT_MASK	NO_OS_BIT(3)
#define ADIN1320_LNK_STAT_CHNG_IRQ_STAT_MASK	NO_OS_BIT(2)
#define ADIN1320_SPEED_CHNG_IRQ_STAT_MASK	NO_OS_BIT(1)
#define ADIN1320_IRQ_PENDING_MASK	NO_OS_BIT(0)

#define ADIN1320_PHY_STATUS_1		0x001a
#define ADIN1320_HCD_TECH_MASK		NO_OS_GENMASK(9, 7)
#define ADIN1320_LINK_STAT_MASK		NO_OS_BIT(6)

#define ADIN1320_LED_CTRL_1		0x001b
#define ADIN1320_LED_3_EXT_CFG_EN_MASK	NO_OS_BIT(13)
#define ADIN1320_LED_2_EXT_CFG_EN_MASK	NO_OS_BIT(12)
#define ADIN1320_LED_1_EXT_CFG_EN_MASK	NO_OS_BIT(11)
#define ADIN1320_LED_0_EXT_CFG_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_LED_PAT_PAUSE_DUR_MASK	NO_OS_GENMASK(7, 4)
#define ADIN1320_LED_PUL_STR_DUR_SEL_MASK	NO_OS_GENMASK(3, 2)
#define ADIN1320_LED_OE_N_MASK		NO_OS_BIT(1)
#define ADIN1320_LED_PUL_STR_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_LED_CTRL_2		0x001c
#define ADIN1320_LED_0_CFG_MASK		NO_OS_GENMASK(3, 0)
#define ADIN1320_LED_1_CFG_MASK		NO_OS_GENMASK(7, 4)
#define ADIN1320_LED_2_CFG_MASK		NO_OS_GENMASK(11, 8)
#define ADIN1320_LED_3_CFG_MASK		NO_OS_GENMASK(15, 12)

#define ADIN1320_LED_CTRL_3		0x001d
#define ADIN1320_LED_PAT_SEL_MASK	NO_OS_GENMASK(15, 14)
#define ADIN1320_LED_PAT_TICK_DUR_MASK	NO_OS_GENMASK(13, 8)
#define ADIN1320_LED_PAT_MASK		NO_OS_GENMASK(7, 0)

#define ADIN1320_PHY_STATUS_2		0x001f

#define ADIN1320_EEE_ADV		NO_OS_MDIO_C45_ADDR(0x1e, 0x8001)
#define ADIN1320_EEE_1000_KX_ADV_MASK	NO_OS_BIT(4)
#define ADIN1320_EEE_1000_ADV_MASK	NO_OS_BIT(2)
#define ADIN1320_EEE_100_ADV_MASK	NO_OS_BIT(1)

#define ADIN1320_SD_CONTROL		NO_OS_MDIO_C45_ADDR(0x1e, 0xfc00)
#define ADIN1320_SD_RESTART_ANEG_MASK	NO_OS_BIT(9)

#define ADIN1320_SD_STATUS		NO_OS_MDIO_C45_ADDR(0x1e, 0xfc01)
#define ADIN1320_SD_AN_DONE_MASK	NO_OS_BIT(5)
#define ADIN1320_SD_REM_FLT_LH_MASK	NO_OS_BIT(4)
#define ADIN1320_SD_LINK_STAT_OK_LL_MASK	NO_OS_BIT(2)
#define ADIN1320_SD_JABBER_DET_LH_MASK	NO_OS_BIT(1)

#define ADIN1320_SD_AUTONEG_ADV		NO_OS_MDIO_C45_ADDR(0x1e, 0xfc04)
#define ADIN1320_SD_HD_1000ADV_MASK	NO_OS_BIT(6)
#define ADIN1320_SD_FD_1000ADV_MASK	NO_OS_BIT(5)

#define ADIN1320_SD_IRQ_MASK		NO_OS_MDIO_C45_ADDR(0x1e, 0xfc18)
#define ADIN1320_SD_OS_RX_PLL_LCK_LOST_IRQ_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_SD_RX_PLL_LCK_LOST_IRQ_EN_MASK		NO_OS_BIT(9)
#define ADIN1320_SD_TX_PLL_LCK_LOST_IRQ_EN_MASK		NO_OS_BIT(8)
#define ADIN1320_SD_AN_STAT_CHG_IRQ_EN_MASK		NO_OS_BIT(7)
#define ADIN1320_SD_FC_FG_IRQ_EN_MASK	NO_OS_BIT(6)
#define ADIN1320_SD_PAGE_RX_IRQ_EN_MASK	NO_OS_BIT(5)
#define ADIN1320_SD_FIFO_OU_IRQ_EN_MASK	NO_OS_BIT(4)
#define ADIN1320_SD_LINK_STAT_CHG_IRQ_EN_MASK		NO_OS_BIT(1)
#define ADIN1320_SD_MDIO_ERR_IRQ_EN_MASK		NO_OS_BIT(0)

#define ADIN1320_SD_IRQ_STATUS		NO_OS_MDIO_C45_ADDR(0x1e, 0xfc19)
#define ADIN1320_SD_OS_RX_PLL_LCK_LOST_IRQ_LH_MASK	NO_OS_BIT(10)
#define ADIN1320_SD_RX_PLL_LCK_LOST_IRQ_LH_MASK		NO_OS_BIT(9)
#define ADIN1320_SD_TX_PLL_LCK_LOST_IRQ_LH_MASK	NO_OS_BIT(8)
#define ADIN1320_SD_AN_CHG_IRQ_LH_MASK	NO_OS_BIT(7)
#define ADIN1320_SD_FC_FG_IRQ_LH_MASK	NO_OS_BIT(6)
#define ADIN1320_SD_PAGE_RX_IRQ_LH_MASK	NO_OS_BIT(5)
#define ADIN1320_SD_FIFO_OU_IRQ_LH_MASK	NO_OS_BIT(4)
#define ADIN1320_SD_LINK_STAT_CHG_LH_MASK		NO_OS_BIT(1)
#define ADIN1320_SD_MDIO_ERR_LH_MASK	NO_OS_BIT(0)

#define ADIN1320_SD_FIB_LED_CFG_0	NO_OS_MDIO_C45_ADDR(0x1e, 0xfcaa)
#define ADIN1320_SD_FIB_LED_CFG_0_MASK	NO_OS_GENMASK(3, 0)

#define ADIN1320_SD_FIB_LED_CFG_1	NO_OS_MDIO_C45_ADDR(0x1e, 0xfcab)
#define ADIN1320_SD_FIB_LED_CFG_1_MASK	NO_OS_GENMASK(3, 0)

#define ADIN1320_SD_FIB_LED_CFG_2	NO_OS_MDIO_C45_ADDR(0x1e, 0xfcac)
#define ADIN1320_SD_FIB_LED_CFG_2_MASK	NO_OS_GENMASK(3, 0)

#define ADIN1320_SD_FIB_LED_CFG_3	NO_OS_MDIO_C45_ADDR(0x1e, 0xfcad)
#define ADIN1320_SD_FIB_LED_CFG_3_MASK	NO_OS_GENMASK(3, 0)

#define ADIN1320_GE_PHY_ID_1		NO_OS_MDIO_C45_ADDR(0x1e, 0xff00)
#define ADIN1320_GE_PHY_ID_1_RESET	0x0283

#define ADIN1320_GE_PHY_ID_2		NO_OS_MDIO_C45_ADDR(0x1e, 0xff01)
#define ADIN1320_GE_MODEL_NUM_MASK	NO_OS_GENMASK(9, 4)
#define ADIN1320_GE_REV_NUM_MASK	NO_OS_GENMASK(3, 0)

#define ADIN1320_GE_SFT_RST		NO_OS_MDIO_C45_ADDR(0x1e, 0xff0c)
#define ADIN1320_GE_SFT_RST_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_SFT_RST_CFG_EN	NO_OS_MDIO_C45_ADDR(0x1e, 0xff0d)
#define ADIN1320_GE_SFT_RST_CFG_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_IRQ_EN		NO_OS_MDIO_C45_ADDR(0x1e, 0xff1d)
#define ADIN1320_GE_WOL_WAKE_IRQ_EN_MASK	NO_OS_BIT(7)

#define ADIN1320_GE_IRQ_LAT		NO_OS_MDIO_C45_ADDR(0x1e, 0xff1e)
#define ADIN1320_GE_WOL_WAKE_IRQ_LH_MASK	NO_OS_BIT(7)

#define ADIN1320_GE_CLK_CFG		NO_OS_MDIO_C45_ADDR(0x1e, 0xff1f)
#define ADIN1320_SD_RX_CLK_125_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_SD_TX_CLK_125_EN_MASK	NO_OS_BIT(9)
#define ADIN1320_SD_RX_CLK_HRT_EN_MASK	NO_OS_BIT(8)
#define ADIN1320_SD_TX_CLK_HRT_EN_MASK	NO_OS_BIT(7)
#define ADIN1320_GE_CLK_RCVR_125_EN_MASK	NO_OS_BIT(5)
#define ADIN1320_GE_CLK_FREE_125_EN_MASK	NO_OS_BIT(4)
#define ADIN1320_GE_REF_CLK_EN_MASK	NO_OS_BIT(3)
#define ADIN1320_GE_CLK_HRT_RCVR_EN_MASK	NO_OS_BIT(2)
#define ADIN1320_GE_CLK_HRT_FREE_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_CLK_25_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_RGMII_CFG		NO_OS_MDIO_C45_ADDR(0x1e, 0xff23)
#define ADIN1320_GE_RGMII_CFG_RESERVED_MASK	NO_OS_GENMASK(15, 11)
#define ADIN1320_GE_RGMII_100_LOW_LTNCY_EN_MASK	NO_OS_BIT(10)
#define ADIN1320_GE_RGMII_10_LOW_LTNCY_EN_MASK	NO_OS_BIT(9)
#define ADIN1320_GE_RGMII_RX_SEL_MASK	NO_OS_GENMASK(8, 6)
#define ADIN1320_GE_RGMII_GTX_SEL_MASK	NO_OS_GENMASK(5, 3)
#define ADIN1320_GE_RGMII_RX_ID_EN_MASK	NO_OS_BIT(2)
#define ADIN1320_GE_RGMII_TX_ID_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_RGMII_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_RMII_CFG		NO_OS_MDIO_C45_ADDR(0x1e, 0xff24)
#define ADIN1320_GE_RMII_EN_MASK	NO_OS_BIT(0)
#define ADIN1320_GE_RMII_FIFO_RST_MASK	NO_OS_BIT(7)

#define ADIN1320_GE_PHY_LED_CFG		NO_OS_MDIO_C45_ADDR(0x1e, 0xff29)
#define ADIN1320_LED_D_INV_EN_MASK	NO_OS_BIT(6)
#define ADIN1320_LED_C_INV_EN_MASK	NO_OS_BIT(5)
#define ADIN1320_LED_B_INV_EN_MASK	NO_OS_BIT(4)
#define ADIN1320_LED_A_INV_EN_MASK	NO_OS_BIT(3)

#define ADIN1320_GE_B10_REGEN_PRE	NO_OS_MDIO_C45_ADDR(0x1e, 0xff38)
#define ADIN1320_GE_B10_REGEN_PRE_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_SD_CFG		NO_OS_MDIO_C45_ADDR(0x1e, 0xff53)
#define ADIN1320_SD_LINK_TYPE_CFG_MASK	NO_OS_GENMASK(7, 5)
#define ADIN1320_SD_AUTONEG_EN_CFG_MASK	NO_OS_BIT(2)
#define ADIN1320_SD_SGMII_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_MSEL_AUTO_STAT	NO_OS_MDIO_C45_ADDR(0x1e, 0xff57)
#define ADIN1320_GE_MSEL_AUTO_SD_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_MSEL_AUTO_GE_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_WOL_SYS_CNTRL	NO_OS_MDIO_C45_ADDR(0x1e, 0xff78)
#define ADIN1320_GE_WOL_WAKE_INV_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_WOL_2_PHY_SD_SEL_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_WOL_EN		NO_OS_MDIO_C45_ADDR(0x1e, 0xff79)
#define ADIN1320_GE_WOL_EN_MASK		NO_OS_BIT(0)

#define ADIN1320_GE_WOL_WAKE_CNTRL	NO_OS_MDIO_C45_ADDR(0x1e, 0xff7a)
#define ADIN1320_GE_WOL_KEY_ERR_WAKE_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_WOL_LS_CHG_WAKE_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_WOL_SIG_CNTRL	NO_OS_MDIO_C45_ADDR(0x1e, 0xff7b)
#define ADIN1320_GE_WOL_PUL_LENM_1_MASK	NO_OS_GENMASK(11, 8)
#define ADIN1320_GE_WOL_PUL_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_WOL_STA_0_AD_01	NO_OS_MDIO_C45_ADDR(0x1e, 0xff7c)
#define ADIN1320_GE_WOL_STA_0_AD_1_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_WOL_STA_0_AD_0_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_WOL_STA_0_AD_23	NO_OS_MDIO_C45_ADDR(0x1e, 0xff7d)
#define ADIN1320_GE_WOL_STA_0_AD_3_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_WOL_STA_0_AD_2_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_WOL_STA_0_AD_45	NO_OS_MDIO_C45_ADDR(0x1e, 0xff7e)
#define ADIN1320_GE_WOL_STA_0_AD_5_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_WOL_STA_0_AD_4_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_MGC_0_CNTRL		NO_OS_MDIO_C45_ADDR(0x1e, 0xff80)
#define ADIN1320_GE_MGC_0_KEY_4BY_EN_MASK	NO_OS_BIT(8)
#define ADIN1320_GE_MGC_0_KEY_CHK_EN_MASK	NO_OS_BIT(7)
#define ADIN1320_GE_MGC_0_LEN_CHK_EN_MASK	NO_OS_BIT(6)
#define ADIN1320_GE_MGC_0_CRC_CHK_EN_MASK	NO_OS_BIT(5)
#define ADIN1320_GE_MGC_0_DA_BC_AD_EN_MASK	NO_OS_BIT(4)
#define ADIN1320_GE_MGC_0_DA_MC_AD_EN_MASK	NO_OS_BIT(3)
#define ADIN1320_GE_MGC_0_DA_UC_AD_EN_MASK	NO_OS_BIT(2)
#define ADIN1320_GE_MGC_0_DA_STA_0_AD_EN_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_MGC_0_EN_MASK	NO_OS_BIT(0)

#define ADIN1320_GE_MGC_0_KEY_01	NO_OS_MDIO_C45_ADDR(0x1e, 0xff81)
#define ADIN1320_GE_MGC_0_KEY_1_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_MGC_0_KEY_0_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_MGC_0_KEY_23	NO_OS_MDIO_C45_ADDR(0x1e, 0xff82)
#define ADIN1320_GE_MGC_0_KEY_3_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_MGC_0_KEY_2_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_MGC_0_KEY_45	NO_OS_MDIO_C45_ADDR(0x1e, 0xff83)
#define ADIN1320_GE_MGC_0_KEY_5_MASK	NO_OS_GENMASK(15, 8)
#define ADIN1320_GE_MGC_0_KEY_4_MASK	NO_OS_GENMASK(7, 0)

#define ADIN1320_GE_WOL_STAT		NO_OS_MDIO_C45_ADDR(0x1e, 0xff85)
#define ADIN1320_GE_MGC_0_KEY_ERR_MASK	NO_OS_BIT(2)
#define ADIN1320_GE_MGC_0_FRM_MATCH_MASK	NO_OS_BIT(1)
#define ADIN1320_GE_WOL_LS_CHG_MASK	NO_OS_BIT(0)

/** LED config value extraction: Bit 4 maps to LED_x_EXT_CFG_EN,
 *  Bits [3:0] map to LED_x_CFG */

#define ADIN1320_LED_EXT_CFG_EN_VAL	NO_OS_BIT(4)
#define ADIN1320_LED_CFG_VAL		NO_OS_GENMASK(3, 0)

/** Bitrange Max Values */

#define ADIN1320_DOWNSPEED_CFG_MAX		0x04
#define ADIN1320_SET_DOWNSPEED_RETRIES_MAX	0x08
#define ADIN1320_CU_LED_CTRL_LED_PAUSE_MAX	0x10
#define ADIN1320_WOL_SIG_CFG_PUL_LEN_MAX	0x10

/** Driver Macros */

#define ADIN1320_MII_STATUS_STRUCT_INIT		{0, 0, 0, 0, 0}
#define ADIN1320_STATION_ADDRESS_MAX_BYTE	6
#define ADIN1320_BITMASK_RESET			0x0000
#define ADIN1320_DISABLE			0
#define ADIN1320_ENABLE				1

/** Device ID field positions for combining PHY_ID_1 and PHY_ID_2 into 32-bit ID */
#define ADIN1320_DEVICE_ID1_HIGH_BIT	31
#define ADIN1320_DEVICE_ID1_LOW_BIT	16
#define ADIN1320_DEVICE_ID2_HIGH_BIT	15
#define ADIN1320_DEVICE_ID2_LOW_BIT	0

/** ADIN1320 PHY ID */
#define ADIN1320_PHY_ID		0x6

enum adin1320_gp_clk_source {
	ADIN1320_GP_CLK_MIN = -1,
	ADIN1320_GP_CLK_REFERENCE,
	ADIN1320_GP_CLK_HEARTBEAT_FREE,
	ADIN1320_GP_CLK_HEARTBEAT_RECOVERED,
	ADIN1320_GP_CLK_FREE = 4,
	ADIN1320_GP_CLK_RECOVERED,
	ADIN1320_GP_CLK_TX_HEARTBEAT = 7,
	ADIN1320_GP_CLK_RX_HEARTBEAT,
	ADIN1320_GP_CLK_TX_FREE,
	ADIN1320_GP_CLK_RX_RECOVERED,
	ADIN1320_GP_CLK_DISABLE,
	ADIN1320_GP_CLK_MAX
};

enum adin1320_clk25_ref_enable {
	ADIN1320_CLK25_REF_DISABLE,
	ADIN1320_CLK25_REF_ENABLE,
};

enum adin1320_rgmii_idelay_enable {
	ADIN1320_RGMII_IDELAY_DISABLE = 0,
	ADIN1320_RGMII_IDELAY_ENABLE
};

enum adin1320_rgmii_idelay {
	ADIN1320_RGMII_2_00_NS,
	ADIN1320_RGMII_1_60_NS,
	ADIN1320_RGMII_1_80_NS,
	/* Values 3-5 are reserved */
	ADIN1320_RGMII_2_20_NS = 6,
	ADIN1320_RGMII_2_40_NS
};

enum adin1320_soft_reset_option {
	ADIN1320_RESET_GE_SUBSYS = 0,
	ADIN1320_RESET_GE_SUBSYS_PIN,
};

enum adin1320_software_powerdown {
	ADIN1320_SOFTWARE_POWERUP = 0,
	ADIN1320_SOFTWARE_POWERDOWN
};

enum adin1320_mac_interface {
	ADIN1320_RGMII_MAC_INTERFACE = 0,
	ADIN1320_RMII_MAC_INTERFACE,
	ADIN1320_MII_MAC_INTERFACE,
	ADIN1320_SGMII_MAC_INTERFACE
};

enum adin1320_autoneg_enable {
	ADIN1320_AUTONEG_DISABLE = 0,
	ADIN1320_AUTONEG_ENABLE
};

enum adin1320_link_stat {
	ADIN1320_LINK_STAT_MIN = -1,
	ADIN1320_LINK_STAT_NO_CHECK, /* for don't check or didn't checked */
	ADIN1320_LINK_STAT_LINKDOWN,
	ADIN1320_LINK_STAT_LINKUP,
	ADIN1320_LINK_STAT_MAX
};

enum adin1320_autoneg_stat {
	ADIN1320_AUTONEG_STAT_MIN = -1,
	ADIN1320_AUTONEG_STAT_NO_CHECK, /* for don't check or didn't checked */
	ADIN1320_AUTONEG_STAT_NOT_DONE,
	ADIN1320_AUTONEG_STAT_DONE,
	ADIN1320_AUTONEG_STAT_MAX
};

enum adin1320_lat_jabber_det {
	ADIN1320_LAT_JABBER_DET_MIN = -1,
	ADIN1320_LAT_JABBER_DET_NO_CHECK, /* for don't check or didn't checked */
	ADIN1320_LAT_JABBER_DET_NONE_DETECTED,
	ADIN1320_LAT_JABBER_DET_DETECTED,
	ADIN1320_LAT_JABBER_DET_MAX
};

enum adin1320_lat_link_stat {
	ADIN1320_LAT_LINK_STAT_MIN = -1,
	ADIN1320_LAT_LINK_STAT_NO_CHECK, /* for don't check or didn't checked */
	ADIN1320_LAT_LINK_STAT_NO_LINK_DROP,
	ADIN1320_LAT_LINK_STAT_LINK_DROPPED,
	ADIN1320_LAT_LINK_STAT_MAX
};

enum adin1320_lat_rem_flt {
	ADIN1320_LAT_REM_FLT_MIN = -1,
	ADIN1320_LAT_REM_FLT_NO_CHECK, /* for don't check or didn't checked */
	ADIN1320_LAT_REM_FLT_NONE_DETECTED,
	ADIN1320_LAT_REM_FLT_DETECTED,
	ADIN1320_LAT_REM_FLT_MAX
};

enum adin1320_link_cfg_enable {
	ADIN1320_LINK_CFG_DISABLE = ADIN1320_DISABLE,
	ADIN1320_LINK_CFG_ENABLE = ADIN1320_ENABLE
};

enum adin1320_cu_speed {
	ADIN1320_CU_SPEED_10BASE_T_HD		= NO_OS_BIT(0),
	ADIN1320_CU_SPEED_10BASE_T_FD		= NO_OS_BIT(1),
	ADIN1320_CU_SPEED_100BASE_TX_HD		= NO_OS_BIT(2),
	ADIN1320_CU_SPEED_100BASE_TX_FD		= NO_OS_BIT(3),
	ADIN1320_CU_SPEED_1000BASE_T_HD		= NO_OS_BIT(4),
	ADIN1320_CU_SPEED_1000BASE_T_FD		= NO_OS_BIT(5),
	ADIN1320_CU_SPEED_MAX			= NO_OS_BIT(6)
};

enum adin1320_sd_speed {
	ADIN1320_SD_SPEED_NO_BASE_X		= NO_OS_BIT(0),
	ADIN1320_SD_SPEED_100BASE_FX_HD		= NO_OS_BIT(2),
	ADIN1320_SD_SPEED_100BASE_FX_FD		= NO_OS_BIT(3),
	ADIN1320_SD_SPEED_1000BASE_KX		= NO_OS_BIT(4),
	ADIN1320_SD_SPEED_1000BASE_X_FD		= NO_OS_BIT(5),
	ADIN1320_SD_SPEED_1000BASE_X_HD		= NO_OS_BIT(6),
	ADIN1320_SD_SPEED_1000BASE_X_HDFD	= NO_OS_BIT(7)
};

enum adin1320_auto_mdix {
	ADIN1320_MANUAL_MDI = 0,
	ADIN1320_MANUAL_MDIX,
	ADIN1320_AUTO_MDIX_PREFER_MDIX,
	ADIN1320_AUTO_MDIX_PREFER_MDI
};

enum adin1320_energy_detect_pwd {
	ADIN1320_NRG_PD_DIS = 0,
	ADIN1320_NRG_PD_EN,
	ADIN1320_NRG_PD_TX_EN,
	ADIN1320_NRG_PD_TX_DIS
};

enum adin1320_edpd_stat {
	ADIN1320_NOT_IN_NRG_PD_MODE = 0,
	ADIN1320_IN_NRG_PD_MODE
};

enum adin1320_downspeed {
	ADIN1320_DOWNSPEED_TO_10	= NO_OS_BIT(0),
	ADIN1320_DOWNSPEED_TO_100	= NO_OS_BIT(1)
};

enum adin1320_eee_speeds {
	ADIN1320_EEE_100_ADV		= NO_OS_BIT(0),
	ADIN1320_EEE_1000_ADV		= NO_OS_BIT(1),
	ADIN1320_EEE_1000_KX_ADV	= NO_OS_BIT(2),
	ADIN1320_EEE_MAX		= NO_OS_BIT(3)
};

enum adin1320_master_slave_status {
	ADIN1320_RESOLVED_TO_SLAVE = 0,
	ADIN1320_RESOLVED_TO_MASTER,
	ADIN1320_MAST_SLAVE_FAULT_DETECT
};

enum adin1320_adv_master_slave_cfg {
	ADIN1320_MAN_ADV_MASTER = 0,
	ADIN1320_MAN_ADV_SLAVE,
	ADIN1320_MAN_MSTR_SLV_DIS
};

enum adin1320_wol_enable {
	ADIN1320_WOL_DISABLE = ADIN1320_DISABLE,
	ADIN1320_WOL_ENABLE = ADIN1320_ENABLE,
};

enum adin1320_wol_fi_en {
	ADIN1320_WOL_FI_DIS = ADIN1320_DISABLE,
	ADIN1320_WOL_FI_EN = ADIN1320_ENABLE
};

enum adin1320_wol_inv {
	ADIN1320_WOL_INV_ACTIVE_HIGH = 0,
	ADIN1320_WOL_INV_ACTIVE_LOW
};

enum adin1320_wol_sig {
	ADIN1320_WOL_SIG_LEVEL = 0,
	ADIN1320_WOL_SIG_PULSE
};

enum adin1320_wol_wake_key_enable {
	ADIN1320_WOL_WAKE_KEY_DISABLE = ADIN1320_DISABLE,
	ADIN1320_WOL_WAKE_KEY_ENABLE = ADIN1320_ENABLE
};

enum adin1320_wol_wake_link_enable {
	ADIN1320_WOL_WAKE_LINK_DISABLE = ADIN1320_DISABLE,
	ADIN1320_WOL_WAKE_LINK_ENABLE = ADIN1320_ENABLE
};

enum adin1320_mgc_chk_en {
	ADIN1320_MGC_CHK_EN_CRC = NO_OS_BIT(0),
	ADIN1320_MGC_CHK_EN_LEN = NO_OS_BIT(1),
	ADIN1320_MGC_CHK_EN_KEY = NO_OS_BIT(2),
	ADIN1320_MGC_CHK_EN_MAX = NO_OS_BIT(3)
};

enum adin1320_mgc_match_en {
	ADIN1320_MGC_MATCH_EN_ST = NO_OS_BIT(0),
	ADIN1320_MGC_MATCH_EN_UC = NO_OS_BIT(1),
	ADIN1320_MGC_MATCH_EN_MC = NO_OS_BIT(2),
	ADIN1320_MGC_MATCH_EN_BC = NO_OS_BIT(3),
	ADIN1320_MGC_MATCH_EN_MAX = NO_OS_BIT(4)
};

enum adin1320_mgc_key_byte {
	ADIN1320_MGC_KEY_BYTE_6 = 0,
	ADIN1320_MGC_KEY_BYTE_4
};

enum adin1320_led_output_enable {
	ADIN1320_LED_OUTPUT_DISABLE = ADIN1320_DISABLE,
	ADIN1320_LED_OUTPUT_ENABLE = ADIN1320_ENABLE
};

enum adin1320_led_pul_str_dur_select {
	ADIN1320_LED_PUL_STR_DUR_SELECT_MIN = -1,
	ADIN1320_LED_PUL_STR_DUR_SELECT_32,
	ADIN1320_LED_PUL_STR_DUR_SELECT_64,
	ADIN1320_LED_PUL_STR_DUR_SELECT_102,
	ADIN1320_LED_PUL_STR_DUR_SELECT_PROG,
	ADIN1320_LED_PUL_STR_DISABLE,
	ADIN1320_LED_PUL_STR_DUR_SELECT_MAX
};

enum adin1320_led_sel {
	ADIN1320_LED_SEL_MIN = -1,
	ADIN1320_LED_SEL_0,
	ADIN1320_LED_SEL_1,
	ADIN1320_LED_SEL_2,
	ADIN1320_LED_SEL_3,
	ADIN1320_LED_SEL_MAX
};

enum adin1320_cu_led_cfg {
	ADIN1320_CU_LED_CFG_MIN = -1,
	/** On if 1000BASE-T link */
	ADIN1320_CU_LED_CFG_ON_1000,
	/** On if 100BASE-TX link */
	ADIN1320_CU_LED_CFG_ON_100,
	/** On if 10BASE-T link */
	ADIN1320_CU_LED_CFG_ON_10,
	/** On if 1000BASE-T link, blink if 100BASE-TX */
	ADIN1320_CU_LED_CFG_ON_1000_BLNK_100,
	/** On if link up */
	ADIN1320_CU_LED_CFG_ON_LINK,
	/** On if transmitting */
	ADIN1320_CU_LED_CFG_ON_TX,
	/** On if receiving */
	ADIN1320_CU_LED_CFG_ON_RX,
	/** On if activity (transmitting or receiving) */
	ADIN1320_CU_LED_CFG_ON_TX_RX,
	/** On if full duplex link */
	ADIN1320_CU_LED_CFG_ON_LINK_FD,
	/** On if collision */
	ADIN1320_CU_LED_CFG_ON_COL,
	/** On if link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_LINK_BLNK_TX_RX,
	/** On if link, blink if receiving */
	ADIN1320_CU_LED_CFG_ON_LINK_BLNK_RX,
	/** On if full duplex link, blink on collision */
	ADIN1320_CU_LED_CFG_ON_LINK_FD_BLNK_COL,
	/** Blink */
	ADIN1320_CU_LED_CFG_BLNK,
	/** ON */
	ADIN1320_CU_LED_CFG_ON,
	/** OFF */
	ADIN1320_CU_LED_CFG_OFF,
	/** On if 10BASE-T or 100BASE-TX link */
	ADIN1320_CU_LED_CFG_ON_10_100,
	/** On if 100BASE-TX or 1000BASE-T link */
	ADIN1320_CU_LED_CFG_ON_100_1000,
	/** On if 10BASE-T link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_10_BLNK_TX_RX,
	/** On if 100BASE-TX link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_100_BLNK_TX_RX,
	/** On if 1000BASE-T link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_1000_BLNK_TX_RX,
	/** On if 10BASE-T or 100BASE-TX link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_10_100_BLNK_TX_RX,
	/** On if 100BASE-TX or 1000BASE-T link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_100_1000_BLNK_TX_RX,
	/** On if 10BASE-T or 1000BASE-T link */
	ADIN1320_CU_LED_CFG_ON_10_1000,
	/** On if 10BASE-T or 1000BASE-T link, blink on activity */
	ADIN1320_CU_LED_CFG_ON_10_1000_BLNK_TX_RX,
	/** Blink on activity */
	ADIN1320_CU_LED_CFG_BLNK_TX_RX,
	/** Blink if transmitting */
	ADIN1320_CU_LED_CFG_BLNK_TX,
	/** On if 1000BASE-T link, blink if 10BASE-T link */
	ADIN1320_CU_LED_CFG_ON_1000_BLNK_10,
	/** On if 100BASE-TX link, blink if 1000BASE-T link */
	ADIN1320_CU_LED_CFG_ON_100_BLNK_1000,
	/** On if 100BASE-TX link, blink if 10BASE-T link */
	ADIN1320_CU_LED_CFG_ON_100_BLNK_10,
	/** On if 10BASE-T link, blink if 1000BASE-T link */
	ADIN1320_CU_LED_CFG_ON_10_BLNK_1000,
	/** On if 10BASE-T link, blink if 100BASE-TX link */
	ADIN1320_CU_LED_CFG_ON_10_BLNK_100,
	ADIN1320_CU_LED_CFG_MAX
};

enum adin1320_sd_led_cfg {
	ADIN1320_SD_LED_CFG_MIN = -1,
	/** On if 1000BASE-X link */
	ADIN1320_SD_LED_CFG_ON_1000,
	/** On if 100BASE-FX link */
	ADIN1320_SD_LED_CFG_ON_100,
	/** Link Up */
	ADIN1320_SD_LED_CFG_LINK,
	/** Blink on Transmitting */
	ADIN1320_SD_LED_CFG_BLNK_TX,
	/** Blink on Receiving */
	ADIN1320_SD_LED_CFG_BLNK_RX,
	/** Blink on Activity */
	ADIN1320_SD_LED_CFG_BLNK_TX_RX,
	/** Link / Blink on Transmitting */
	ADIN1320_SD_LED_CFG_LINK_BLNK_TX,
	/** Link / Blink on Receiving */
	ADIN1320_SD_LED_CFG_LINK_BLNK_RX,
	/** Link / Blink on Activity */
	ADIN1320_SD_LED_CFG_LINK_BLNK_TX_RX,
	/** Blink */
	ADIN1320_SD_LED_CFG_BLNK,
	/** ON */
	ADIN1320_SD_LED_CFG_ON,
	/** OFF */
	ADIN1320_SD_LED_CFG_OFF,
	ADIN1320_SD_LED_CFG_MAX
};

enum adin1320_led_inv_state {
	ADIN1320_LED_INV_STATE_ACTIVE_HIGH = 0,
	ADIN1320_LED_INV_STATE_ACTIVE_LOW
};

enum adin1320_active_media {
	ADIN1320_ACTIVE_MEDIA_NULL = 0,
	ADIN1320_ACTIVE_MEDIA_CU,
	ADIN1320_ACTIVE_MEDIA_SD
};

struct adin1320_rgmii_config {
	enum adin1320_rgmii_idelay_enable tx_idelay_en;
	enum adin1320_rgmii_idelay tx_idelay;
	enum adin1320_rgmii_idelay_enable rx_idelay_en;
	enum adin1320_rgmii_idelay rx_idelay;
};

struct adin1320_mii_status {
	enum adin1320_link_stat link_stat;
	enum adin1320_autoneg_stat autoneg_stat;
	enum adin1320_lat_jabber_det lat_jabber_det;
	enum adin1320_lat_link_stat lat_link_stat;
	enum adin1320_lat_rem_flt lat_rem_flt;
};

struct adin1320_init_param {
	struct no_os_gpio_init_param *reset_param;
	struct no_os_mdio_init_param mdio_param;
};

struct adin1320_desc {
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_mdio_desc *mdio;
};

int adin1320_init(struct adin1320_desc **dev,
		  struct adin1320_init_param *param);
int adin1320_hard_reset(struct adin1320_desc *dev);
int adin1320_remove(struct adin1320_desc *dev);
int adin1320_write(struct adin1320_desc *dev, uint32_t addr, uint16_t val);
int adin1320_read(struct adin1320_desc *dev, uint32_t addr, uint16_t *val);
int adin1320_write_bits(struct adin1320_desc *dev, uint32_t addr, uint16_t val,
			uint16_t bitmask);
int adin1320_config_rgmii(struct adin1320_desc *dev,
			  struct adin1320_rgmii_config rgmii);
int adin1320_config_gp_clk(struct adin1320_desc *dev,
			   enum adin1320_gp_clk_source source);
int adin1320_config_clk25_ref(struct adin1320_desc *dev,
			      enum adin1320_clk25_ref_enable enable);
int adin1320_cu_autoneg(struct adin1320_desc *dev,
			enum adin1320_autoneg_enable enable);
int adin1320_sd_autoneg(struct adin1320_desc *dev,
			enum adin1320_autoneg_enable enable);
int adin1320_cu_resolved_speed(struct adin1320_desc *dev,
			       enum adin1320_cu_speed *resolved_speed);
int adin1320_sd_resolved_speed(struct adin1320_desc *dev,
			       enum adin1320_sd_speed *resolved_speed);
int adin1320_soft_reset(struct adin1320_desc *dev,
			enum adin1320_soft_reset_option reset_cfg);
int adin1320_cu_read_irq_status(struct adin1320_desc *dev,
				bool *val_irq_pending,
				uint16_t *val_irq_status);
int adin1320_sd_read_irq_status(struct adin1320_desc *dev,
				bool *val_irq_pending,
				uint16_t *val_irq_status);
int adin1320_set_software_powerdown(struct adin1320_desc *dev,
				    enum adin1320_software_powerdown val);
int adin1320_get_software_powerdown(struct adin1320_desc *dev,
				    enum adin1320_software_powerdown *val);
int adin1320_get_device_id(struct adin1320_desc *dev, uint32_t *device_id);
int adin1320_reset_rmii_fifo(struct adin1320_desc *dev);
int adin1320_select_mac_interface(struct adin1320_desc *dev,
				  enum adin1320_mac_interface val);
int adin1320_cu_link_cfg(struct adin1320_desc *dev,
			 enum adin1320_link_cfg_enable val);
int adin1320_cu_get_mii_status(struct adin1320_desc *dev,
			       struct adin1320_mii_status *mii_status);
int adin1320_sd_get_mii_status(struct adin1320_desc *dev,
			       struct adin1320_mii_status *mii_status);
int adin1320_cu_autoneg_adv_cfg(struct adin1320_desc *dev,
				uint16_t autoneg_adv_speeds);
int adin1320_sd_autoneg_adv_cfg(struct adin1320_desc *dev,
				uint16_t autoneg_adv_speeds);
int adin1320_cu_get_autoneg_adv(struct adin1320_desc *dev,
				uint16_t *autoneg_adv_speeds);
int adin1320_sd_get_autoneg_adv(struct adin1320_desc *dev,
				uint16_t *autoneg_adv_speeds);
int adin1320_auto_mdix_cfg(struct adin1320_desc *dev,
			   enum adin1320_auto_mdix val);
int adin1320_energy_detect_pwd_cfg(struct adin1320_desc *dev,
				   enum adin1320_energy_detect_pwd val);
int adin1320_get_energy_detect_pwd_stat(struct adin1320_desc *dev,
					enum adin1320_edpd_stat *val);
int adin1320_downspeed_cfg(struct adin1320_desc *dev,
			   uint8_t downspeeds);
int adin1320_set_downspeed_retries(struct adin1320_desc *dev, uint16_t val);
int adin1320_cu_forced_speed(struct adin1320_desc *dev,
			     enum adin1320_cu_speed val);
int adin1320_sd_forced_speed(struct adin1320_desc *dev,
			     enum adin1320_sd_speed forced_speed);
int adin1320_set_eee(struct adin1320_desc *dev, uint8_t eee_speeds);
int adin1320_get_eee(struct adin1320_desc *dev, uint8_t *eee_speeds);
int adin1320_get_master_slave_status(struct adin1320_desc *dev,
				     enum adin1320_master_slave_status *val);
int adin1320_master_slave_config(struct adin1320_desc *dev,
				 enum adin1320_adv_master_slave_cfg val);
int adin1320_cu_config_interrupt(struct adin1320_desc *dev,
				 uint32_t callback_events);
int adin1320_sd_config_interrupt(struct adin1320_desc *dev,
				 uint32_t callback_events);
int adin1320_led_cfg(struct adin1320_desc *dev,
		     enum adin1320_led_output_enable enable_led_output,
		     enum adin1320_led_pul_str_dur_select pulse_stretch);
int adin1320_cu_led_ctrl(struct adin1320_desc *dev,
			 enum adin1320_led_sel led_sel, uint8_t led_pat,
			 uint8_t led_pause, enum adin1320_cu_led_cfg led_cfg,
			 enum adin1320_led_inv_state led_inv_state);
int adin1320_sd_led_ctrl(struct adin1320_desc *dev,
			 enum adin1320_led_sel led_sel,
			 enum adin1320_sd_led_cfg led_cfg,
			 enum adin1320_led_inv_state led_inv_state);
int adin1320_mgc_match_cfg(struct adin1320_desc *dev,
			   uint16_t enabled_matches);
int adin1320_mgc_check_cfg(struct adin1320_desc *dev,
			   uint16_t enabled_checks);
int adin1320_mgc_key_cfg(struct adin1320_desc *dev,
			 enum adin1320_mgc_key_byte key_byte,
			 uint8_t key[6]);
int adin1320_wol_address(struct adin1320_desc *dev,
			 uint8_t mac_address[6]);
int adin1320_wol_sys_cfg(struct adin1320_desc *dev,
			 enum adin1320_wol_fi_en enable_fi,
			 enum adin1320_wol_inv active_signal);
int adin1320_wol_sig_cfg(struct adin1320_desc *dev,
			 enum adin1320_wol_sig signal_type,
			 uint8_t pulse_length);
int adin1320_wol_wake_cfg(struct adin1320_desc *dev,
			  enum adin1320_wol_wake_key_enable wake_on_key,
			  enum adin1320_wol_wake_link_enable wake_on_link_change);
int adin1320_wol_en_cfg(struct adin1320_desc *dev,
			enum adin1320_wol_enable enable);
int adin1320_get_active_media(struct adin1320_desc *dev,
			      enum adin1320_active_media *media);

#endif /* __ADIN1320_H__ */
