#include <stdio.h>
#include "no_os_delay.h"
#include "no_os_mdio.h"
#include "mdio_bitbang.h"
#include "net.h"
#include "parameters.h"

#if (TARGET_NUM == 32650)

#define ADIN1300_MII_CONTROL		0x0000
#define ADIN1300_LOOPBACK_MASK		NO_OS_BIT(14)

#define ADIN1300_PHY_CTRL_1		0x0012
#define ADIN1300_DIAG_CLK_EN_MASK	NO_OS_BIT(2)

#define ADIN1300_PHY_CTRL_STATUS_1	0x0013
#define ADIN1300_LB_ALL_DIG_SEL_MASK	NO_OS_BIT(12)

#define ADIN1300_RX_ERR_CNT		0x0014

#define ADIN1300_PHY_STATUS_1		0x001a
#define ADIN1300_HCD_TECH_MASK		NO_OS_GENMASK(9, 7)
#define ADIN1300_LINK_STAT		NO_OS_BIT(6)

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

#define ADIN1300_GE_CLK_CFG			NO_OS_MDIO_C45_ADDR(0x1e, 0xff1f)
#define ADIN1300_GE_CLK_RCVR_125_EN_MASK	NO_OS_BIT(5)
#define ADIN1300_GE_CLK_FREE_125_EN_MASK	NO_OS_BIT(4)
#define ADIN1300_GE_REF_CLK_EN_MASK		NO_OS_BIT(3)
#define ADIN1300_GE_CLK_HRT_RCVR_EN_MASK	NO_OS_BIT(2)
#define ADIN1300_GE_CLK_HRT_FREE_EN_MASK	NO_OS_BIT(1)
#define ADIN1300_GE_CLK_25_EN_MASK		NO_OS_BIT(0)

#define ADIN1300_GE_RGMII_CFG			NO_OS_MDIO_C45_ADDR(0x1e, 0xff23)
#define ADIN1300_GE_RGMII_EN_MASK		NO_OS_BIT(0)
#define ADIn1300_GE_RGMII_RX_SEL_MASK		NO_OS_GENMASK(8, 6)
#define ADIn1300_GE_RGMII_GTX_SEL_MASK		NO_OS_GENMASK(5, 3)
#define ADIN1300_GE_RGMII_TX_ID_EN_MASK		NO_OS_BIT(1)
#define ADIN1300_GE_RGMII_RX_ID_EN_MASK		NO_OS_BIT(2)

// ------------ MAX24287 ---------------------
#define MAX24287_REG(page, addr)	(((page) << 5) | ((addr) & 0x1f))
#define MAX24287_PAGE(addr)		((addr) >> 5)
#define MAX24287_ADDR(addr)		((addr) & 0x1f)

#define MAX24287_BMCR		0
#define MAX24287_DP_RST_MASK	NO_OS_BIT(15)
#define MAX24287_AN_EN_MASK	NO_OS_BIT(12)
#define MAX24287_AN_START_MASK	NO_OS_BIT(9)

#define MAX24287_BMSR		1
#define MAX24287_ID1		2
#define MAX24287_ID2		3

#define MAX24287_AN_ADV		4
#define MAX24287_AN_ADV_W_MASK	NO_OS_BIT(0)
#define MAX24287_LK_MASK	NO_OS_BIT(15)
#define MAX24287_DPLX_MASK	NO_OS_BIT(12)
#define MAX24287_SPD_MASK	NO_OS_GENMASK(11, 10)

#define MAX24287_AN_RX		5

#define MAX24287_AN_EXP		6
#define MAX24287_EXT_STAT	15
#define MAX24287_JIT_DIAG	16

#define MAX24287_PCSCR		17
#define MAX24287_WD_DIS_MASK	NO_OS_BIT(6)
#define MAX24287_EN_CDET_MASK	NO_OS_BIT(0)

#define MAX24287_GMIICR			18
#define MAX24287_GMIICR_W_MASK		NO_OS_BIT(7)
#define MAX24287_GMIICR_SPD_MASK	NO_OS_GENMASK(15, 14)
#define MAX24287_GMIICR_DTE_DCE_MASK	NO_OS_BIT(12)
#define MAX24287_GMIICR_DDR_MASK	NO_OS_BIT(11)
#define MAX24287_GMIICR_TXCLK_EN_MASK	NO_OS_BIT(10)

#define MAX24287_CR		19
#define MAX24287_IR		20
#define MAX24287_ID		MAX24287_REG(1, 16)
#define MAX24287_GPIOCR1	MAX24287_REG(1, 17)
#define MAX24287_GPIOCR2	MAX24287_REG(1, 18)
#define MAX24287_GPIOSR		MAX24287_REG(1, 19)

#define MAX24287_PTPCR1		MAX24287_REG(2, 16)
#define MAX24287_PTPCR1_W_MASK	NO_OS_BIT(14)	
#define MAX24287_PLL_PWDN_MASK	NO_OS_BIT(5)
#define MAX24287_TX_PWDN_MASK	NO_OS_BIT(3)
#define MAX24287_RX_PWDN_MASK	NO_OS_BIT(2)

#define MAX24287_PAGESEL	31
#define MAX24287_PAGE_MASK	NO_OS_GENMASK(1, 0)

int max24287_write(struct no_os_mdio_desc *desc, uint8_t addr, uint16_t val)
{
	int ret;
	uint8_t page = MAX24287_PAGE(addr);
	uint8_t reg = MAX24287_ADDR(addr);

	if (page)
		ret = no_os_mdio_write(desc, MAX24287_PAGESEL, 0x10 | page);

	ret = no_os_mdio_write(desc, reg, val);

	if (page)
		ret = no_os_mdio_write(desc, MAX24287_PAGESEL, 0x10);
	
	return ret;
}

int max24287_read(struct no_os_mdio_desc *desc, uint8_t addr, uint16_t *val)
{
	int ret;
	uint8_t page = MAX24287_PAGE(addr);
	uint8_t reg = MAX24287_ADDR(addr);

	if (page)
		ret = no_os_mdio_write(desc, MAX24287_PAGESEL, 0x10 | page);

	ret = no_os_mdio_read(desc, reg, val);

	if (page)
		ret = no_os_mdio_write(desc, MAX24287_PAGESEL, 0x10);
	
	return ret;
}

void max24287_regmap(struct no_os_mdio_desc *max24287)
{
	unsigned int i;
	uint16_t val;
	uint8_t addrs[] = {
		MAX24287_BMCR,
		MAX24287_BMSR,
		MAX24287_ID1,
		MAX24287_ID2,
		MAX24287_AN_ADV,
		MAX24287_AN_RX,
		MAX24287_EXT_STAT,
		MAX24287_JIT_DIAG,
		MAX24287_PCSCR,
		MAX24287_GMIICR,
		MAX24287_CR,
		MAX24287_IR,
		MAX24287_ID,
		MAX24287_GPIOCR1,
		MAX24287_GPIOCR2,
		MAX24287_GPIOSR,
		MAX24287_PTPCR1
	};
	char *names[] = {
		"BMCR",
		"BMSR",
		"ID1",
		"ID2",
		"AN_ADV",
		"AN_RX",
		"EXT_STAT",
		"JIT_DIAG",
		"PCSCR",
		"GMIICR",
		"CR",
		"IR",
		"ID",
		"GPIOCR1",
		"GPIOCR2",
		"GPIOSR",
		"PTPCR1"
	};

	printf("----- MAX24287 -----\n");
	for (i = 0; i < NO_OS_ARRAY_SIZE(addrs); i++) {
		max24287_read(max24287, addrs[i], &val);
		printf("%u.%u %s 0x%x\n", MAX24287_PAGE(addrs[i]), MAX24287_ADDR(addrs[i]), names[i], val);
	}
}

int adin1300_regmap(struct no_os_mdio_desc *phy)
{
	uint32_t r;
	uint16_t val;
	printf("----- ADIN1300 -----\n");
	for (r = 0; r < 32; r++) {
		no_os_mdio_read(phy, r, &val);
		printf("%.2u 0x%.4x\n", r, val);
	}

	return 0;
}

int net_init(bool hbtx)
{
	int ret;
	uint16_t val;
	struct no_os_mdio_desc *phy, *serdes;
	struct no_os_gpio_desc *phy_reset, *serdes_reset;

	no_os_gpio_get(&phy_reset, &adin1300_reset_gpio_ip);
	no_os_gpio_direction_output(phy_reset, NO_OS_GPIO_LOW);
	no_os_mdelay(1);
	no_os_gpio_direction_output(phy_reset, NO_OS_GPIO_HIGH);
	no_os_mdelay(5);

	no_os_gpio_get(&serdes_reset, &max24287_reset_gpio_ip);
	no_os_gpio_direction_output(serdes_reset, NO_OS_GPIO_LOW);
	no_os_udelay(100);
	no_os_gpio_direction_output(serdes_reset, NO_OS_GPIO_HIGH);

	struct mdio_bitbang_init_param phy_eip = {
		.mdc = adin1300_mdc_gpio_ip,
		.mdio = adin1300_mdio_gpio_ip,
	};

	struct no_os_mdio_init_param phy_ip = {
		.c45 = true,
		.addr = 0,
		.ops = &mdio_bitbang_ops,
		.extra = &phy_eip,
	};
	ret = no_os_mdio_init(&phy, &phy_ip);
	if (ret)
		return ret;

	struct mdio_bitbang_init_param serdes_eip = {
		.mdc = max24287_mdc_gpio_ip,
		.mdio = max24287_mdio_gpio_ip,
	};

	struct no_os_mdio_init_param serdes_ip = {
		.addr = 0x7,
		.ops = &mdio_bitbang_ops,
		.extra = &serdes_eip,
	};
	ret = no_os_mdio_init(&serdes, &serdes_ip);
	if (ret)
		return ret;
	
	// -------------- SERDES --------------------
	// RevB sequence
	max24287_write(serdes, MAX24287_PTPCR1, MAX24287_PTPCR1_W_MASK | MAX24287_RX_PWDN_MASK);
	no_os_mdelay(1);
	max24287_write(serdes, MAX24287_PTPCR1, MAX24287_PTPCR1_W_MASK);
	max24287_write(serdes, MAX24287_BMCR, MAX24287_DP_RST_MASK);

	// disable autoneg watchdog and keep comma detection and code group alignment enabled
	//max24287_write(serdes, MAX24287_PCSCR, MAX24287_WD_DIS_MASK | MAX24287_EN_CDET_MASK);

	// GMIICR
	val = no_os_field_prep(MAX24287_GMIICR_W_MASK, 1); // write 1
	val |= no_os_field_prep(MAX24287_GMIICR_SPD_MASK, 2); // 0 - RGMII-10, 1 - RGMII-100, 2 - RGMII-1000 or 3 - RTBI
	val |= no_os_field_prep(MAX24287_GMIICR_DDR_MASK, 1);
	max24287_write(serdes, MAX24287_GMIICR, val);

	// select GPIO2 & GPIO3 functions
	max24287_write(serdes, MAX24287_GPIOCR1, 0x25); // 0x25: GPIO2 125 MHz TX PLL, GPIO3 25MHz or 125MHz from receive clock recovery PLL

	max24287_read(serdes, MAX24287_IR, &val); // clean the IR statuses
	max24287_read(serdes, MAX24287_BMSR, &val); // clean the BMSR statuses

	// auto negotiation
	//if (!hbtx)
	{
		val = no_os_field_prep(MAX24287_AN_ADV_W_MASK, 1);
		val |= no_os_field_prep(MAX24287_LK_MASK, 1);
		val |= no_os_field_prep(MAX24287_DPLX_MASK, 1);
		val |= no_os_field_prep(MAX24287_SPD_MASK, 2); // 0 - 10Mbps, 1 - 100 Mbps, 2 - 1000 Mbps
		max24287_write(serdes, MAX24287_AN_ADV, val);
		//max24287_write(serdes, MAX24287_BMCR, MAX24287_AN_EN_MASK | MAX24287_AN_START_MASK);
	}
	// else
	// {
	// 	max24287_write(serdes, MAX24287_AN_ADV, 0x0001);
	// }

	
	no_os_mdelay(1000);

	max24287_regmap(serdes);

	// -------------- PHY --------------------
	// back-to-back delay
	no_os_mdio_read(phy, ADIN1300_GE_RGMII_CFG, &val);
	val &= ~(ADIN1300_GE_RGMII_TX_ID_EN_MASK | ADIN1300_GE_RGMII_RX_ID_EN_MASK); 
		//| ADIn1300_GE_RGMII_RX_SEL_MASK | ADIn1300_GE_RGMII_GTX_SEL_MASK);
	val |= no_os_field_prep(ADIN1300_GE_RGMII_RX_ID_EN_MASK, 1);
	val |= no_os_field_prep(ADIN1300_GE_RGMII_TX_ID_EN_MASK, 1);
	// val |= no_os_field_prep(ADIn1300_GE_RGMII_RX_SEL_MASK, 0x7);
	// val |= no_os_field_prep(ADIn1300_GE_RGMII_GTX_SEL_MASK, 0x7);
	
	no_os_mdio_write(phy, ADIN1300_GE_RGMII_CFG, val);
	no_os_mdio_read(phy, ADIN1300_GE_RGMII_CFG, &val);
	printf("GE_RGMII_CFG 0x%x\n", val);

	// select GP_CLK and CLK25_REF
	no_os_mdio_write(phy, ADIN1300_GE_CLK_CFG,
		ADIN1300_GE_REF_CLK_EN_MASK | ADIN1300_GE_CLK_RCVR_125_EN_MASK);

	// wait a bit for PHY to establish link
	int timeout;
	while(timeout < 5000) {
		timeout++;
		no_os_mdio_read(phy, ADIN1300_PHY_STATUS_1, &val);
		if (val & ADIN1300_LINK_STAT)
			break;
		no_os_mdelay(1);
	}
	adin1300_regmap(phy);

	// figure out resolved speed
	no_os_mdio_read(phy, ADIN1300_PHY_STATUS_1, &val);
	val = no_os_field_get(ADIN1300_HCD_TECH_MASK, val);
	const char *speeds[] = {
		"10BASE-T half duplex",
		"10BASE-T full duplex",
		"100BASE-TX half duplex",
		"100BASE-TX full duplex",
		"1000BASE-T half duplex",
		"1000BASE-T full duplex",
		"reserved",
		"reserved"
	};
	printf("Resolved speed: %s (0x%x) in %d ms\n", speeds[val], val, timeout);

	return 0;
}

#endif
