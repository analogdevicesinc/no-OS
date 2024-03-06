#ifndef _MAX24287_H_
#define _MAX24287_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_mdio.h"

#define MAX24287_REG(page, addr)	(((page) << 5) | ((addr) & 0x1f))
#define MAX24287_PAGE(addr)		((addr) >> 5)
#define MAX24287_ADDR(addr)		((addr) & 0x1f)

#define MAX24287_BMCR		0
#define MAX24287_DP_RST_MASK	NO_OS_BIT(15)
#define MAX24287_AN_EN_MASK	NO_OS_BIT(12)
#define MAX24287_AN_START_MASK	NO_OS_BIT(9)

#define MAX24287_BMSR		1
#define MAX24287_LINK_ST_MASK	NO_OS_BIT(2)

#define MAX24287_ID1		2
#define MAX24287_ID2		3

#define MAX24287_AN_ADV		4
#define MAX24287_AN_ADV_W_MASK	NO_OS_BIT(0)
#define MAX24287_LK_MASK	NO_OS_BIT(15)
#define MAX24287_DPLX_MASK	NO_OS_BIT(12)
#define MAX24287_SPD_MASK	NO_OS_GENMASK(11, 10)

#define MAX24287_AN_RX		5

#define MAX24287_AN_EXP		6
#define MAX24287_AN_RX_PAGE	NO_OS_BIT(1)

#define MAX24287_EXT_STAT	15
#define MAX24287_JIT_DIAG	16

#define MAX24287_PCSCR		17
#define MAX24287_WD_DIS_MASK	NO_OS_BIT(6)
#define MAX24287_BASEX_MASK	NO_OS_BIT(4)
#define MAX24287_EN_CDET_MASK	NO_OS_BIT(0)

#define MAX24287_GMIICR			18
#define MAX24287_GMIICR_W_MASK		NO_OS_BIT(7)
#define MAX24287_GMIICR_SPD_MASK	NO_OS_GENMASK(15, 14)
#define MAX24287_GMIICR_DTE_DCE_MASK	NO_OS_BIT(12)
#define MAX24287_GMIICR_DDR_MASK	NO_OS_BIT(11)
#define MAX24287_GMIICR_TXCLK_EN_MASK	NO_OS_BIT(10)

#define MAX24287_CR		19

#define MAX24287_IR		20
#define MAX24287_PAGE_IE	NO_OS_BIT(10)

#define MAX24287_ID		MAX24287_REG(1, 16)

#define MAX24287_GPIOCR1	MAX24287_REG(1, 17)
#define MAX24287_RST_MASK	NO_OS_BIT(15)
#define MAX24287_GPO1_SEL	NO_OS_GENMASK(14, 12)
#define MAX24287_GPO2_SEL	NO_OS_GENMASK(11, 9)
#define MAX24287_GPIO1_SEL	NO_OS_GENMASK(8, 6)
#define MAX24287_GPIO2_SEL	NO_OS_GENMASK(5, 3)
#define MAX24287_GPIO3_SEL	NO_OS_GENMASK(2, 0)

#define MAX24287_GPIOCR2	MAX24287_REG(1, 18)
#define MAX24287_GPIOSR		MAX24287_REG(1, 19)

#define MAX24287_PTPCR1		MAX24287_REG(2, 16)
#define MAX24287_PTPCR1_W_MASK	NO_OS_BIT(14)
#define MAX24287_PLL_PWDN_MASK	NO_OS_BIT(5)
#define MAX24287_TX_PWDN_MASK	NO_OS_BIT(3)
#define MAX24287_RX_PWDN_MASK	NO_OS_BIT(2)

#define MAX24287_PAGESEL	31
#define MAX24287_PAGE_MASK	NO_OS_GENMASK(1, 0)

enum max24287_parallel {
	MAX24287_TBI,
	MAX24287_RTBI,
	MAX24287_GMII,
	MAX24287_RGMII,
	MAX24287_MII
};

enum max24287_serial {
	MAX24287_SGMII,
	MAX24287_1000BASEX
};

enum max24287_speed {
	MAX24287_10_HALFDUPLEX,
	MAX24287_10_FULLDUPLEX,
	MAX24287_100_HALFDUPLEX,
	MAX24287_100_FULLDUPLEX,
	MAX24287_1000_HALFDUPLEX,
	MAX24287_1000_FULLDUPLEX,
	MAX24287_1250_HALFDUPLEX,
	MAX24287_1250_FULLDUPLEX
};

struct max24287_init_param {
	struct no_os_gpio_init_param *reset_param;
	struct no_os_mdio_init_param mdio_param;
	enum max24287_parallel parallel;
	enum max24287_speed parspeed;
	enum max24287_serial serial;
	enum max24287_speed serspeed;
};

struct max24287_desc {
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_mdio_desc *mdio;
};

int max24287_init(struct max24287_desc **dev,
		  struct max24287_init_param *param);
int max24287_remove(struct max24287_desc *dev);
int max24287_write(struct max24287_desc *dev, uint8_t addr, uint16_t val);
int max24287_read(struct max24287_desc *dev, uint8_t addr, uint16_t *val);
int max24287_write_bits(struct max24287_desc *dev, uint8_t addr, uint16_t val,
			uint16_t bitmask);
int max24287_hard_reset(struct max24287_desc *dev);
int max24287_soft_reset(struct max24287_desc *dev);
int max24287_config_parallel(struct max24287_desc *dev,
			     enum max24287_parallel par, enum max24287_speed speed);
int max24287_get_config_parallel(struct max24287_desc *dev,
				 enum max24287_parallel *par, enum max24287_speed *speed);
int max24287_config_serial(struct max24287_desc *dev, enum max24287_serial ser,
			   enum max24287_speed speed);
int max24287_get_config_serial(struct max24287_desc *dev,
			       enum max24287_serial *ser, enum max24287_speed *speed);
int max24287_config_link(struct max24287_desc *dev, bool up);
bool max24287_link_is_up(struct max24287_desc *dev);
void max24287_regmap(struct max24287_desc *dev);

#endif