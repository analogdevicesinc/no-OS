#include <stdio.h>
#include "no_os_delay.h"
#include "no_os_mdio.h"
#include "mdio_bitbang.h"
#include "net.h"
#include "parameters.h"
#include "adin1300.h"
#include "iio_adin1300.h"
#include "max24287.h"
#include "iio_max24287.h"

#if (TARGET_NUM == 32650)
int net_init(struct adin1300_iio_desc **adin1300_iio, 
		struct max24287_iio_desc **max24287_iio,
		unsigned int speed)
{
	int ret;
	uint16_t spd;
	enum adin1300_speed phy_speed_cap;
	struct adin1300_desc *adin1300;
	struct max24287_desc *max24287;

	switch(speed) {
	case 10:
		phy_speed_cap = ADIN1300_10_BASE_T_FULLDUPLEX;
		break;
	case 100:
		phy_speed_cap = ADIN1300_100_BASE_TX_FULLDUPLEX;
		break;
	case 1000:
	default:
		phy_speed_cap = ADIN1300_1000_BASE_T_FULLDUPLEX;
		break;
	};

struct adin1300_init_param adin1300_ip = {
		.reset_param = &adin1300_reset_gpio_ip,
		.mdio_param = {
			.c45 = true,
			.addr = 0,
			.ops = &mdio_bitbang_ops,
			.extra = &(struct mdio_bitbang_init_param){
				.mdc = adin1300_mdc_gpio_ip,
				.mdio = adin1300_mdio_gpio_ip,
			},
		},
		.rgmii_config = {
			.tx_idelay_en = true,
			.rx_idelay_en = true
		},
		.gp_clk = true,
		.gp_clk_source = ADIN1300_GP_CLK_RECOVERED,
		.clk25_ref = true,
		.speed_cap = phy_speed_cap,
		.autoneg = true,
	};
	ret = adin1300_init(&adin1300, &adin1300_ip);
	if (ret)
		return ret;
	
	ret = adin1300_iio_init(adin1300_iio,
		&(struct adin1300_iio_init_param){.dev = adin1300});
	if (ret)
		return ret;

	// wait a bit for PHY to establish link
	int timeout = 5000;
	while(timeout) {
		timeout--;
		if (adin1300_link_is_up(adin1300))
			break;
		no_os_mdelay(1);
	}

	if (timeout)
	{
		spd = adin1300_resolved_speed(adin1300);
		const char *speeds[] = {
			"10BASE-T half duplex",
			"10BASE-T full duplex",
			"100BASE-TX half duplex",
			"100BASE-TX full duplex",
			"1000BASE-T half duplex",
			"1000BASE-T full duplex",
			"invalid",
		};
		printf("Resolved speed: %s (0x%x) in %d ms\n", speeds[spd], spd, 5000 - timeout);
	}
	else
		printf("PHY autonegotiation timeout\n");

	// SERDES
	struct max24287_init_param serdes_ip = {
		.reset_param = &max24287_reset_gpio_ip,
		.mdio_param = {
			.addr = 0x7,
			.ops = &mdio_bitbang_ops,
			.extra = &(struct mdio_bitbang_init_param){
				.mdc = max24287_mdc_gpio_ip,
				.mdio = max24287_mdio_gpio_ip,
			},
		},
		.parallel = MAX24287_RGMII,
		.parspeed = spd, // use the speed that the PHY negotiated on ethernet
		.serial = MAX24287_SGMII,
		.serspeed = MAX24287_1000_FULLDUPLEX,
	};
	ret = max24287_init(&max24287, &serdes_ip);
	if (ret)
		return ret;
	
	// select GPIO2 & GPIO3 functions
	// 0x25: GPIO2 125 MHz TX PLL, GPIO3 25MHz or 125MHz from receive clock recovery PLL
	max24287_write(max24287, MAX24287_GPIOCR1, 0x25);

	ret = max24287_iio_init(max24287_iio,
		&(struct max24287_iio_init_param){.dev = max24287});
	if (ret)
		return ret;

	return 0;
}

#endif
