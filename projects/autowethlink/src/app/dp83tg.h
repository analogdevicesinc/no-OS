#ifndef _DP83TG_H_
#define _DP83TG_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

#define DP83TG_PHY_ID_1				0x2
#define DP83TG_PHY_ID_2				0x3

#define DP83TG_REGCR				0xd
#define DP83TG_ADDAR				0xe

#define DP83TG_MII_REG_1F			0x1f
#define DP83TG_SW_GLOBAL_RESET_MASK	NO_OS_BIT(15)
#define DP83TG_DIGITAL_RESET_MASK	NO_OS_BIT(14)

#define DP83TG_SGMII_CTRL_1			NO_OS_MDIO_C45_ADDR(0x1f, 0x608)
#define DP83TG_SGMII_CTRL_2			NO_OS_MDIO_C45_ADDR(0x1f, 0x60c)

struct dp83tg_init_param {
	struct no_os_gpio_init_param *reset;
	struct no_os_mdio_init_param mdio;
};

struct dp83tg_desc {
	struct no_os_gpio_desc *reset;
	struct no_os_mdio_desc *mdio;
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

#endif