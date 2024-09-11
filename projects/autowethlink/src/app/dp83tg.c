#include <stdint.h>
#include <errno.h>
#include "dp83tg.h"
#include "no_os_mdio.h"
#include "no_os_alloc.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"

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

	// initialize the MDIO
	ret = no_os_mdio_init(&d->mdio, &param->mdio);
	if (ret)
		goto free_reset;

	// reset
	if (param->reset) {
		ret = no_os_gpio_get(&d->reset, param->reset);
		if (ret)
			goto free_d;
		ret = dp83tg_hard_reset(d);
	} else {
		ret = dp83tg_soft_reset(d);
	}
	if (ret)
		goto free_reset;

	// MDIO sanity check after a reset
	ret = dp83tg_read(d, DP83TG_PHY_ID_1, &val);
	if (val != 0x2000) {
		ret = -EFAULT;
		goto free_mdio;
	}

	ret = dp83tg_read(d, DP83TG_PHY_ID_2, &val);
	if (val != 0xa284) {
		ret = -EFAULT;
		goto free_mdio;
	}
	
	ret = dp83tg_sgmii(d);
	if (ret)
		return ret;

	*dev = d;

	return 0;
free_mdio:
	no_os_mdio_remove(d->mdio);
free_reset:
	no_os_gpio_remove(d->reset);
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
	return dp83tg_reg_mask_is_nonzero(dev, DP83TG_BMSR, DP83TG_LINK_STATUS_MASK);
}

bool dp83tg_mii_link_is_up(struct dp83tg_desc *dev)
{
	return dp83tg_reg_mask_is_nonzero(dev, DP83TG_SGMII_STATUS, DP83TG_LINK_STATUS_1000BX_MASK);
}

bool dp83tg_mdi_link_is_up(struct dp83tg_desc *dev)
{
	return dp83tg_reg_mask_is_nonzero(dev, DP83TG_MII_REG_10, DP83TG_LINK_STATUS_BIT_MASK);

}