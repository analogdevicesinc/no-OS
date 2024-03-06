#include <stdio.h>
#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "mdio_bitbang.h"
#include "max24287.h"

int max24287_init(struct max24287_desc **dev, struct max24287_init_param *param)
{
	int ret;
	uint16_t val;
	struct max24287_desc *d;

	if (!dev || !param)
		return -EINVAL;

	d = (struct max24287_desc *) no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	// initialize the MDIO
	ret = no_os_mdio_init(&d->mdio, &param->mdio_param);
	if (ret)
		goto free_reset;

	// reset
	if (param->reset_param) {
		ret = no_os_gpio_get(&d->reset_gpio, param->reset_param);
		if (ret)
			goto free_d;
		ret = max24287_hard_reset(d);
	} else {
		ret = max24287_soft_reset(d);
	}
	if (ret)
		goto free_reset;

	// MDIO sanity check after a reset
	ret = max24287_read(d, MAX24287_ID, &val);
	if (val != 0x1ee0 && val != 0x1edf) {
		ret = -EFAULT;
		goto free_mdio;
	}

	if (val == 0x1ee0) {
		// Rev B sequence
		max24287_write(d, MAX24287_PTPCR1,
			       MAX24287_PTPCR1_W_MASK | MAX24287_RX_PWDN_MASK);
		no_os_mdelay(1);
		max24287_write(d, MAX24287_PTPCR1, MAX24287_PTPCR1_W_MASK);
		max24287_write(d, MAX24287_BMCR, MAX24287_DP_RST_MASK);
	}

	max24287_read(d, MAX24287_IR, &val); // clean the IR statuses
	max24287_read(d, MAX24287_BMSR, &val); // clean the BMSR statuses

	ret = max24287_config_parallel(d, param->parallel, param->parspeed);
	if (ret)
		goto free_mdio;

	ret = max24287_config_serial(d, param->serial, param->serspeed);
	if (ret)
		goto free_mdio;

	ret = max24287_config_link(d, true);
	if (ret)
		goto free_mdio;
	*dev = d;

	return 0;
free_mdio:
	no_os_mdio_remove(d->mdio);
free_reset:
	no_os_gpio_remove(d->reset_gpio);
free_d:
	free(d);
	return ret;
}

int max24287_soft_reset(struct max24287_desc *dev)
{
	int ret;

	ret = max24287_write_bits(dev, MAX24287_GPIOCR1, MAX24287_RST_MASK,
				  MAX24287_RST_MASK);
	if (ret < 0)
		return ret;

	no_os_mdelay(1);

	return 0;
}

int max24287_hard_reset(struct max24287_desc *dev)
{
	int ret;

	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
	no_os_udelay(100);
	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1);

	return 0;
}

int max24287_write(struct max24287_desc *dev, uint8_t addr, uint16_t val)
{
	int ret;
	uint8_t page = MAX24287_PAGE(addr);
	uint8_t reg = MAX24287_ADDR(addr);

	if (page)
		ret = no_os_mdio_write(dev->mdio, MAX24287_PAGESEL, 0x10 | page);

	ret = no_os_mdio_write(dev->mdio, reg, val);

	if (page)
		ret = no_os_mdio_write(dev->mdio, MAX24287_PAGESEL, 0x10);

	return ret;
}

int max24287_read(struct max24287_desc *dev, uint8_t addr, uint16_t *val)
{
	int ret;
	uint8_t page = MAX24287_PAGE(addr);
	uint8_t reg = MAX24287_ADDR(addr);

	if (page)
		ret = no_os_mdio_write(dev->mdio, MAX24287_PAGESEL, 0x10 | page);

	ret = no_os_mdio_read(dev->mdio, reg, val);

	if (page)
		ret = no_os_mdio_write(dev->mdio, MAX24287_PAGESEL, 0x10);

	return ret;
}

int max24287_write_bits(struct max24287_desc *dev, uint8_t addr, uint16_t val,
			uint16_t bitmask)
{
	int ret;
	uint16_t rval;

	ret = max24287_read(dev, addr, &rval);
	if (ret)
		return ret;

	rval &= ~bitmask;
	rval |= (val & bitmask);

	return max24287_write(dev, addr, rval);
}

int max24287_config_parallel(struct max24287_desc *dev,
			     enum max24287_parallel par, enum max24287_speed speed)
{
	uint16_t val;
	uint16_t spd = speed >> 1;

	switch (par) {
	case MAX24287_RGMII:
		if (speed > MAX24287_1000_FULLDUPLEX || speed == MAX24287_1000_HALFDUPLEX)
			return -ENOTSUP;

		val = no_os_field_prep(MAX24287_GMIICR_W_MASK, 1);
		val |= no_os_field_prep(MAX24287_GMIICR_SPD_MASK, spd);
		val |= no_os_field_prep(MAX24287_GMIICR_DDR_MASK, 1);

		return max24287_write(dev, MAX24287_GMIICR, val);
	default:
		return -ENOTSUP;
	};
}

int max24287_get_config_parallel(struct max24287_desc *dev,
				 enum max24287_parallel *par, enum max24287_speed *speed)
{
	int ret;
	uint16_t val;

	ret = max24287_read(dev, MAX24287_GMIICR, &val);
	if (ret)
		return ret;
	if (par)
		*par = MAX24287_RGMII;
	if (speed)
		*speed = (no_os_field_get(MAX24287_GMIICR_SPD_MASK, val) << 1) | 0x1;

	return 0;
}

int max24287_config_serial(struct max24287_desc *dev, enum max24287_serial ser,
			   enum max24287_speed speed)
{
	int ret;
	uint16_t val;
	uint16_t fullduplex = speed & 0x1;
	uint16_t spd = speed >> 1;

	switch (ser) {
	case MAX24287_SGMII:
		if (speed > MAX24287_1000_FULLDUPLEX)
			return -ENOTSUP;

		ret = max24287_write_bits(dev, MAX24287_PCSCR, 0, MAX24287_BASEX_MASK);
		if (ret)
			return ret;

		val = no_os_field_prep(MAX24287_AN_ADV_W_MASK, 1);
		val |= no_os_field_prep(MAX24287_DPLX_MASK, fullduplex);
		val |= no_os_field_prep(MAX24287_SPD_MASK, spd);
		return max24287_write(dev, MAX24287_AN_ADV, val);
	default:
		return -ENOTSUP;
	};
}

int max24287_get_config_serial(struct max24287_desc *dev,
			       enum max24287_serial *ser, enum max24287_speed *speed)
{
	int ret;
	uint16_t val;

	if (ser) {
		ret = max24287_read(dev, MAX24287_PCSCR, &val);
		if (ret)
			return ret;
		*ser = no_os_field_get(MAX24287_BASEX_MASK, val);
	}
	if (speed) {
		ret = max24287_read(dev, MAX24287_AN_ADV, &val);
		if (ret)
			return ret;
		*speed = (no_os_field_get(MAX24287_SPD_MASK, val) << 1) |
			 no_os_field_get(MAX24287_DPLX_MASK, val);
	}

	return 0;
}

int max24287_config_link(struct max24287_desc *dev, bool up)
{
	uint16_t val = no_os_field_prep(MAX24287_AN_ADV_W_MASK, 1) |
		       no_os_field_prep(MAX24287_LK_MASK, up);
	return max24287_write_bits(dev, MAX24287_AN_ADV, val,
				   MAX24287_AN_ADV_W_MASK | MAX24287_LK_MASK);

}

bool max24287_link_is_up(struct max24287_desc *dev)
{
	int ret;
	uint16_t val;

	ret = max24287_read(dev, MAX24287_BMSR, &val);
	ret = max24287_read(dev, MAX24287_BMSR, &val);
	if (!ret && no_os_field_get(MAX24287_LINK_ST_MASK, val))
		return true;

	return false;
}

void max24287_regmap(struct max24287_desc *dev)
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
		max24287_read(dev, addrs[i], &val);
		printf("%u.%u %s 0x%x\n", MAX24287_PAGE(addrs[i]), MAX24287_ADDR(addrs[i]),
		       names[i], val);
	}
}
