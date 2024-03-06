#ifndef _ADIN1300_H_
#define _ADIN1300_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

#define ADIN1300_MII_CONTROL		0x0000
#define ADIN1300_LOOPBACK_MASK		NO_OS_BIT(14)
#define ADIN1300_SPEED_SEL_LSB_MASK	NO_OS_BIT(13)
#define ADIN1300_AUTONEG_EN_MASK	NO_OS_BIT(12)
#define ADIN1300_RESTART_ANEG_MASK	NO_OS_BIT(9)
#define ADIN1300_DPLX_MODE_MASK		NO_OS_BIT(8)
#define ADIN1300_SPEED_SEL_MSB_MASK	NO_OS_BIT(6)

#define ADIN1300_MII_STATUS		0x1
#define ADIN1300_AUTONEG_DONE_MASK	NO_OS_BIT(5)
#define ADIN1300_REM_FLT_LAT_MASK	NO_OS_BIT(4)
#define ADIN1300_LINK_STAT_LAT_MASK	NO_OS_BIT(2)
#define ADIN1300_JABBER_DET_LAT_MASK	NO_OS_BIT(1)

#define ADIN1300_PHY_ID_1		0x2
#define ADIN1300_PHY_ID_2		0x3

#define ADIN1300_AUTONEG_ADV		0x04
#define ADIN1300_FD_100_ADV_MASK	NO_OS_BIT(8)
#define ADIN1300_HD_100_ADV_MASK	NO_OS_BIT(7)
#define ADIN1300_FD_10_ADV_MASK		NO_OS_BIT(6)
#define ADIN1300_HD_10_ADV_MASK		NO_OS_BIT(5)
#define ADIN1300_SELECTOR_ADV_MASK	NO_OS_GENMASK(4, 0)

#define ADIN1300_MSTR_SLV_CONTROL	0x09
#define ADIN1300_FD_1000_ADV_MASK	NO_OS_BIT(9)
#define ADIN1300_HD_1000_ADV_MASK	NO_OS_BIT(8)

#define ADIN1300_PHY_CTRL_1		0x0012
#define ADIN1300_DIAG_CLK_EN_MASK	NO_OS_BIT(2)

#define ADIN1300_PHY_CTRL_STATUS_1	0x0013
#define ADIN1300_LB_ALL_DIG_SEL_MASK	NO_OS_BIT(12)

#define ADIN1300_RX_ERR_CNT		0x0014

#define ADIN1300_PHY_CTRL_2		0x0016
#define ADIN1300_DN_SPEED_TO_100_EN_MASK	NO_OS_BIT(11)
#define ADIN1300_DN_SPEED_TO_10_EN_MASK	NO_OS_BIT(10)
#define ADIN1300_CLK_CNTRL_MASK		NO_OS_GENMASK(3, 1)

#define ADIN1300_PHY_CTRL_3		0x0017
#define ADIN1300_LINK_EN_MASK		NO_OS_BIT(13)

#define ADIN1300_IRQ_MASK		0x0018
#define ADIN1300_IRQ_STATUS		0x0019
#define ADIN1300_LNK_STAT_CHNG_IRQ_MASK	NO_OS_BIT(2)
#define ADIN1300_SPEED_CHG_IRQ_MASK	NO_OS_BIT(1)
#define ADIN1300_HW_IRQ_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_PHY_STATUS_1		0x001a
#define ADIN1300_HCD_TECH_MASK		NO_OS_GENMASK(9, 7)
#define ADIN1300_LINK_STAT_MASK		NO_OS_BIT(6)

#define ADIN1300_PHY_STATUS_2		0x001f

#define ADIN1300_FC_EN			NO_OS_MDIO_C45_ADDR(0x1e, 0x9403)
#define ADIN1300_FC_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_FC_TX_SEL		NO_OS_MDIO_C45_ADDR(0x1e, 0x9407)
#define ADIN1300_FC_TX_SEL_MASK		NO_OS_BIT(0)

#define ADIN1300_FC_FRM_CNT_H		NO_OS_MDIO_C45_ADDR(0x1e, 0x940a)
#define ADIN1300_FC_FRM_CNT_L		NO_OS_MDIO_C45_ADDR(0x1e, 0x940b)
#define ADIN1300_FC_LEN_ERR_CNT		NO_OS_MDIO_C45_ADDR(0x1e, 0x940c)
#define ADIN1300_FC_ALGN_ERR_CNT	NO_OS_MDIO_C45_ADDR(0x1e, 0x940d)
#define ADIN1300_FC_SYMB_ERR_CNT	NO_OS_MDIO_C45_ADDR(0x1e, 0x940e)
#define ADIN1300_FC_OSZ_ERR_CNT		NO_OS_MDIO_C45_ADDR(0x1e, 0x940f)
#define ADIN1300_FC_USZ_ERR_CNT		NO_OS_MDIO_C45_ADDR(0x1e, 0x9410)

#define ADIN1300_FG_EN			NO_OS_MDIO_C45_ADDR(0x1e, 0x9415)
#define ADIN1300_FG_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_FG_CNTRL_RSTRT		NO_OS_MDIO_C45_ADDR(0x1e, 0x9416)
#define ADIN1300_FG_RSTRT_MASK		NO_OS_BIT(3)
#define ADIN1300_FG_CNTRL_MASK		NO_OS_GENMASK(2, 0)

#define ADIN1300_FG_FRM_LEN		NO_OS_MDIO_C45_ADDR(0x1e, 0x941a)

#define ADIN1300_FG_DONE		NO_OS_MDIO_C45_ADDR(0x1e, 0x941e)
#define ADIN1300_FG_DONE_MASK		NO_OS_BIT(0)

#define ADIN1300_GE_SFT_RST			NO_OS_MDIO_C45_ADDR(0x1e, 0xff0c)
#define ADIN1300_GE_SFT_RST_MASK		NO_OS_BIT(0)

#define ADIN1300_GE_CLK_CFG			NO_OS_MDIO_C45_ADDR(0x1e, 0xff1f)
#define ADIN1300_GE_CLK_RCVR_125_EN_MASK	NO_OS_BIT(5)
#define ADIN1300_GE_CLK_FREE_125_EN_MASK	NO_OS_BIT(4)
#define ADIN1300_GE_REF_CLK_EN_MASK		NO_OS_BIT(3)
#define ADIN1300_GE_CLK_HRT_RCVR_EN_MASK	NO_OS_BIT(2)
#define ADIN1300_GE_CLK_HRT_FREE_EN_MASK	NO_OS_BIT(1)
#define ADIN1300_GE_CLK_25_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_GE_RGMII_CFG			NO_OS_MDIO_C45_ADDR(0x1e, 0xff23)
#define ADIN1300_GE_RGMII_100_LOW_LTNCY_EN_MSK	NO_OS_BIT(10)
#define ADIN1300_GE_RGMII_10_LOW_LTNCY_EN_MSK	NO_OS_BIT(9)
#define ADIN1300_GE_RGMII_RX_SEL_MASK		NO_OS_GENMASK(8, 6)
#define ADIN1300_GE_RGMII_GTX_SEL_MASK		NO_OS_GENMASK(5, 3)
#define ADIN1300_GE_RGMII_RX_ID_EN_MASK		NO_OS_BIT(2)
#define ADIN1300_GE_RGMII_TX_ID_EN_MASK		NO_OS_BIT(1)
#define ADIN1300_GE_RGMII_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_GE_B10_REGEN_PRE		NO_OS_MDIO_C45_ADDR(0x1e, 0xff38)
#define ADIN1300_GE_B10_REGEN_PRE_MSK		NO_OS_BIT(0)

enum adin1300_speed {
	ADIN1300_10_BASE_T_HALFDUPLEX,
	ADIN1300_10_BASE_T_FULLDUPLEX,
	ADIN1300_100_BASE_TX_HALFDUPLEX,
	ADIN1300_100_BASE_TX_FULLDUPLEX,
	ADIN1300_1000_BASE_T_HALFDUPLEX,
	ADIN1300_1000_BASE_T_FULLDUPLEX
};

enum adin1300_gp_clk_source {
	ADIN1300_GP_CLK_REFERENCE,
	ADIN1300_GP_CLK_HEARTBEAT_FREE,
	ADIN1300_GP_CLK_HEARTBEAT_RECOVERED,
	ADIN1300_GP_CLK_FREE = 4,
	ADIN1300_GP_CLK_RECOVERED
};

enum adin1300_rgmii_idelay {
	ADIN1300_RGMII_2_00_NS,
	ADIN1300_RGMII_1_60_NS,
	ADIN1300_RGMII_1_80_NS,
	ADIN1300_RGMII_2_20_NS = 6,
	ADIN1300_RGMII_2_40_NS
};

struct adin1300_rgmii_config {
	bool tx_idelay_en;
	enum adin1300_rgmii_idelay tx_idelay;
	bool rx_idelay_en;
	enum adin1300_rgmii_idelay rx_idelay;

};

struct adin1300_init_param {
	struct no_os_gpio_init_param *reset_param;
	struct no_os_mdio_init_param mdio_param;
	struct adin1300_rgmii_config rgmii_config;
	bool gp_clk;
	enum adin1300_gp_clk_source gp_clk_source;
	bool clk25_ref;
	enum adin1300_speed speed_cap;
	bool autoneg;
};

struct adin1300_desc {
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_mdio_desc *mdio;
};

int adin1300_init(struct adin1300_desc **dev,
		  struct adin1300_init_param *param);
int adin1300_remove(struct adin1300_desc *dev);
int adin1300_write(struct adin1300_desc *dev, uint32_t addr, uint16_t val);
int adin1300_read(struct adin1300_desc *dev, uint32_t addr, uint16_t *val);
int adin1300_write_bits(struct adin1300_desc *dev, uint32_t addr, uint16_t val,
			uint16_t bitmask);
int adin1300_soft_reset(struct adin1300_desc *dev);
int adin1300_hard_reset(struct adin1300_desc *dev);
int adin1300_config_rgmii(struct adin1300_desc *dev,
			  struct adin1300_rgmii_config rgmii);
int adin1300_config_gp_clk(struct adin1300_desc *dev, bool on,
			   enum adin1300_gp_clk_source source);
int adin1300_config_clk25_ref(struct adin1300_desc *dev, bool on);
int adin1300_autoneg(struct adin1300_desc *dev, bool on);
int adin1300_config_speed(struct adin1300_desc *dev,
			  enum adin1300_speed speed_cap);
enum adin1300_speed adin1300_resolved_speed(struct adin1300_desc *dev);
bool adin1300_link_is_up(struct adin1300_desc *dev);



#endif