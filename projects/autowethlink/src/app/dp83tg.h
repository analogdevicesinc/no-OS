#ifndef _DP83TG_H_
#define _DP83TG_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

#define DP83TG_PHY_ID_1					0x2
#define DP83TG_PHY_ID_2					0x3

#define DP83TG_REGCR					0xd
#define DP83TG_ADDAR					0xe

#define DP83TG_MII_REG_10				0x10
#define DP83TG_LINK_STATUS_BIT_MASK		NO_OS_BIT(0)

#define DP83TG_MII_REG_11				0x11
#define DP83TG_INT_POLARITY_MASK		NO_OS_BIT(3)
#define DP83TG_INT_EN_MASK				NO_OS_BIT(1)

#define DP83TG_INTERRUPT_STATUS_1		0x12
#define DP83TG_ENERGY_DETECT_MASK		NO_OS_BIT(6)
#define DP83TG_LINK_STATUS_CHANGED_MASK	NO_OS_BIT(5)
#define DP83TG_TRAINING_DONE_MASK		NO_OS_BIT(2)

#define DP83TG_MII_REG_1F				0x1f
#define DP83TG_SW_GLOBAL_RESET_MASK		NO_OS_BIT(15)
#define DP83TG_DIGITAL_RESET_MASK		NO_OS_BIT(14)

#define DP83TG_BMSR						NO_OS_MDIO_C45_ADDR(0x1f, 0x141)
#define DP83TG_LINK_STATUS_MASK			NO_OS_BIT(2)

#define DP83TG_LSR						NO_OS_MDIO_C45_ADDR(0x1f, 0x180)
#define DP83TG_LINK_UP_MASK				NO_OS_BIT(15)

#define DP83TG_SGMII_CTRL_1				NO_OS_MDIO_C45_ADDR(0x1f, 0x608)
#define DP83TG_CFG_SGMII_EN_MASK		NO_OS_BIT(9)
#define DP83TG_SGMII_AUTONEG_TIMER_MASK	NO_OS_GENMASK(2, 1)
#define DP83TG_MR_AN_ENABLE_MASK		NO_OS_BIT(0)

#define DP83TG_SGMII_STATUS				NO_OS_MDIO_C45_ADDR(0x1f, 0x60a)
#define DP83TG_SGMII_PAGE_RECEIVED_MASK	NO_OS_BIT(12)
#define DP83TG_LINK_STATUS_1000BX_MASK	NO_OS_BIT(11)
#define DP83TG_MR_AN_COMPLETE_MASK		NO_OS_BIT(10)

#define DP83TG_SGMII_CTRL_2				NO_OS_MDIO_C45_ADDR(0x1f, 0x60c)
#define DP83TG_MR_RESTART_AN_MASK		NO_OS_BIT(6)

#define DP83TG_PMA_PMD_CONTROL			NO_OS_MDIO_C45_ADDR(0x1, 0x1834)
#define DP83TG_CFG_MASTER_SLAVE_MASK	NO_OS_BIT(14)

struct dp83tg_init_param {
	struct no_os_gpio_init_param *reset;
	struct no_os_mdio_init_param mdio;
	struct no_os_gpio_init_param *link;
};

struct dp83tg_desc {
	struct no_os_gpio_desc *reset;
	struct no_os_mdio_desc *mdio;
	struct no_os_gpio_desc *link;
};

int dp83tg_init(struct dp83tg_desc **dev,
		  struct dp83tg_init_param *param);
int dp83tg_remove(struct dp83tg_desc *dev);

int dp83tg_write(struct dp83tg_desc *dev, uint32_t addr,
			  uint16_t val);
int dp83tg_read(struct dp83tg_desc *dev, uint32_t addr,
			 uint16_t *val);
int dp83tg_write_bits(struct dp83tg_desc *dev, uint8_t addr, uint16_t val,
			uint16_t bitmask);

int dp83tg_soft_reset(struct dp83tg_desc *dev);
int dp83tg_hard_reset(struct dp83tg_desc *dev);

int dp83tg_sgmii(struct dp83tg_desc *dev);
bool dp83tg_link_is_up(struct dp83tg_desc *dev);
bool dp83tg_mii_link_is_up(struct dp83tg_desc *dev);
bool dp83tg_mdi_link_is_up(struct dp83tg_desc *dev);

#endif