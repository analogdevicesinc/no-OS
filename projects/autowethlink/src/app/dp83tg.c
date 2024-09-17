#include <stdint.h>
#include <errno.h>
#include "dp83tg.h"
#include "no_os_mdio.h"
#include "no_os_alloc.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"

static const uint16_t dp83tg_master_init[25][2] = {
         {0x408, 0x580},
         {0x409, 0x2a},
         {0x8a1, 0xbff},
         {0x802, 0x422},
         {0x864, 0x1fd0},
         {0x865, 0x2},
         {0x8a3, 0x24e9},
         {0x800, 0x2090},
         {0x840, 0x4120},
         {0x841, 0x6151},
         {0x8a0, 0x01E7},
         {0x879, 0xe4ce},
         {0x89f, 0x1},
         {0x844, 0x3f10},
         {0x843, 0x327a},
         {0x842, 0x6652},
         {0x8a8, 0xe080},
         {0x8a9, 0x3f0},
         {0x88d, 0x3fa0},
         {0x899, 0x3fa0},
         {0x50b, 0x7e7c},
         {0x56a, 0x5f41},
         {0x56b, 0xffb4},
         {0x56c, 0xffb4},
         {0x573, 0x1},
};

static const uint16_t dp83tg_slave_init[25][2] = {
         {0x408, 0x580},
         {0x409, 0x2a},
         {0x8a1, 0xbff},
         {0x802, 0x422},
         {0x864, 0x1fd0},
         {0x865, 0x2},
         {0x853, 0x632},
         {0x824, 0x15e0},
         {0x86a, 0x106},
         {0x894, 0x5057},
         {0x85d, 0x6405},
         {0x892, 0x1b0},
         {0x852, 0x327a},
         {0x851, 0x6652},
         {0x877, 0x55},
         {0x80b, 0x16},
         {0x8a8, 0xe080},
         {0x8a9, 0x3f0},
         {0x88d, 0x3fa0},
         {0x899, 0x3fa0},
         {0x1f, 0x4000},
         {0x56a, 0x5f41},
         {0x56b, 0xffb4},
         {0x56c, 0xffb4},
         {0x573, 0x1},
};

int dp83tg_config(struct dp83tg_desc *d, bool master)
{
	int ret;
	uint16_t val;
	int i;

	if (d->reset)
		ret = dp83tg_hard_reset(d);
	else
		ret = dp83tg_soft_reset(d);
	if (ret)
		return ret;

	// MDIO sanity check after a reset
	ret = dp83tg_read(d, DP83TG_PHY_ID_1, &val);
	if (val != 0x2000)
		ret = -EFAULT;

	ret = dp83tg_read(d, DP83TG_PHY_ID_2, &val);
	if (val != 0xa284)
		return -EFAULT;
	
	ret = dp83tg_sgmii(d);
	if (ret)
		return ret;

	/* Start up procedure */
	ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x573), 0x101);
	if (ret)
		return ret;

	if (master) {
		ret = dp83tg_write(d, DP83TG_BMSR, 0x940);
		if (ret)
			return ret;

		ret = dp83tg_write(d, DP83TG_BMSR, 0x140);
		if (ret)
			return ret;

		ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x834), 0xc001);
		if (ret)
			return ret;

		for (i = 0; i < 25; i++) {
			ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, dp83tg_master_init[i][0]), dp83tg_master_init[i][1]);
			if (ret)
				return ret;
		}
	}
	else {
		ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x834), 0x8001);
		if (ret)
			return ret;

		for (i = 0; i < 25; i++) {
			ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, dp83tg_slave_init[i][0]), dp83tg_slave_init[i][1]);
			if (ret)
				return ret;
		}
	}

	ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x18c), 0x1);
	if (ret)
		return ret;

	ret = dp83tg_write(d, DP83TG_MII_REG_1F, DP83TG_DIGITAL_RESET_MASK);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x573), 0x1);
	if (ret)
		return ret;

	ret = dp83tg_write(d, NO_OS_MDIO_C45_ADDR(0x1f, 0x56A), 0x5f41);
	if (ret)
		return ret;

	return 0;
}

int dp83tg_init(struct dp83tg_desc **dev, struct dp83tg_init_param *param)
{
	int ret;
	uint16_t val;
	struct dp83tg_desc *d;

	if (!dev || !param)
		return -EINVAL;

	d = (struct dp83tg_desc *) no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_mdio_init(&d->mdio, &param->mdio);
	if (ret)
		goto free_d;

	if (param->reset) {
		ret = no_os_gpio_get(&d->reset, param->reset);
		if (ret)
			goto free_mdio;
	}

	ret = dp83tg_config(d, param->master);
	if (ret)
		goto free_reset;

	*dev = d;

	return 0;
free_reset:
	no_os_gpio_remove(d->reset);
free_mdio:
	no_os_mdio_remove(d->mdio);
free_d:
	free(d);
	return ret;
}

int dp83tg_remove(struct dp83tg_desc *dev)
{
	no_os_mdio_remove(dev->mdio);
	no_os_gpio_remove(dev->reset);
	free(dev);
	return 0;
}

int dp83tg_soft_reset(struct dp83tg_desc *dev)
{
	int ret;

	ret = dp83tg_write_bits(dev, DP83TG_MII_REG_1F, DP83TG_SW_GLOBAL_RESET_MASK,
				  DP83TG_SW_GLOBAL_RESET_MASK);
	if (ret < 0)
		return ret;

	no_os_mdelay(1);

	return 0;
}

int dp83tg_hard_reset(struct dp83tg_desc *dev)
{
	int ret;

	ret = no_os_gpio_direction_output(dev->reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
	no_os_udelay(5);
	ret = no_os_gpio_direction_output(dev->reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1);

	return 0;
}

int dp83tg_write(struct dp83tg_desc *dev, uint32_t addr,
			  uint16_t val)
{
	int ret;
	uint8_t devad = no_os_field_get(NO_OS_MDIO_C45_DEVADDR_MASK, addr);
	uint16_t regad = no_os_field_get(NO_OS_MDIO_DATA_MASK, addr);

	if (devad) {
		ret = no_os_mdio_write(dev->mdio, DP83TG_REGCR, devad);
		if (ret)
			return ret;
		ret = no_os_mdio_write(dev->mdio, DP83TG_ADDAR, regad);
		if (ret)
			return ret;
		ret = no_os_mdio_write(dev->mdio, DP83TG_REGCR, devad | 0x4000);
		if (ret)
			return ret;
		addr = DP83TG_ADDAR;
	}
	return no_os_mdio_write(dev->mdio, addr, val);
}

int dp83tg_read(struct dp83tg_desc *dev, uint32_t addr,
			 uint16_t *val)
{
	int ret;
	uint8_t devad = no_os_field_get(NO_OS_MDIO_C45_DEVADDR_MASK, addr);
	uint16_t regad = no_os_field_get(NO_OS_MDIO_DATA_MASK, addr);

	if (devad) {
		ret = no_os_mdio_write(dev->mdio, DP83TG_REGCR, devad);
		if (ret)
			return ret;
		ret = no_os_mdio_write(dev->mdio, DP83TG_ADDAR, regad);
		if (ret)
			return ret;
		ret = no_os_mdio_write(dev->mdio, DP83TG_REGCR, devad | 0x4000);
		if (ret)
			return ret;
		addr = DP83TG_ADDAR;
	}
	return no_os_mdio_read(dev->mdio, addr, val);
}

int dp83tg_write_bits(struct dp83tg_desc *dev, uint8_t addr, uint16_t val,
			uint16_t bitmask)
{
	int ret;
	uint16_t rval;

	ret = dp83tg_read(dev, addr, &rval);
	if (ret)
		return ret;

	rval &= ~bitmask;
	rval |= (val & bitmask);

	return dp83tg_write(dev, addr, rval);
}

int dp83tg_sgmii(struct dp83tg_desc *dev)
{
	return dp83tg_write_bits(dev, DP83TG_SGMII_CTRL_1, DP83TG_CFG_SGMII_EN_MASK, DP83TG_CFG_SGMII_EN_MASK);
}

static bool dp83tg_reg_mask_is_nonzero(struct dp83tg_desc *dev, uint32_t addr, uint16_t mask)
{
	int ret;
	uint16_t val;

	ret = dp83tg_read(dev, addr, &val);
	if (ret)
		return false;
	
	return (bool)no_os_field_get(mask, val);
}

bool dp83tg_link_is_up(struct dp83tg_desc *dev)
{
/* TODO: remove all this stuff 
	if (dev->link)
	{
		int ret;
		uint8_t link;

		ret = no_os_gpio_get_value(dev->link, &link);
		if (ret)
			return false;
		
		return (bool)link;
	}
	else
		return dp83tg_reg_mask_is_nonzero(dev, DP83TG_BMSR, DP83TG_LINK_STATUS_MASK);
*/
	return dp83tg_mii_link_is_up(dev) && dp83tg_mdi_link_is_up(dev);
}

bool dp83tg_mii_link_is_up(struct dp83tg_desc *dev)
{
	return dp83tg_reg_mask_is_nonzero(dev, DP83TG_SGMII_STATUS, DP83TG_LINK_STATUS_1000BX_MASK);
}

bool dp83tg_mdi_link_is_up(struct dp83tg_desc *dev)
{
	return dp83tg_reg_mask_is_nonzero(dev, DP83TG_LSR, DP83TG_LINK_UP_MASK);

}