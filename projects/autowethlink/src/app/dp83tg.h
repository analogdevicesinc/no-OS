#ifndef _DP83TG_H_
#define _DP83TG_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

#define DP83TG_PHY_ID_1		0x2
#define DP83TG_PHY_ID_2		0x3

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

#endif