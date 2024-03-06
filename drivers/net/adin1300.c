#include <stdio.h>
#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "mdio_bitbang.h"
#include "adin1300.h"

int adin1300_init(struct adin1300_desc **dev, struct adin1300_init_param *param)
{
	int ret;
	uint16_t val;
	struct adin1300_desc *d;

	if (!dev || !param)
		return -EINVAL;

	d = (struct adin1300_desc *) no_os_calloc(1, sizeof(*d));
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
		ret = adin1300_hard_reset(d);
	} else {
		ret = adin1300_soft_reset(d);
	}
	if (ret)
		goto free_reset;

	// MDIO sanity check after a reset
	ret = adin1300_read(d, ADIN1300_PHY_ID_1, &val);
	if (val != 0x283) {
		ret = -EFAULT;
		goto free_mdio;
	}
	ret = adin1300_read(d, ADIN1300_PHY_ID_2, &val);
	if (val != 0xbc30) {
		ret = -EFAULT;
		goto free_mdio;
	}

	ret = adin1300_config_rgmii(d, param->rgmii_config);
	if (ret)
		goto free_mdio;

	ret = adin1300_config_gp_clk(d, param->gp_clk, param->gp_clk_source);
	if (ret)
		goto free_mdio;

	ret = adin1300_config_clk25_ref(d, param->clk25_ref);
	if (ret)
		goto free_mdio;

	ret = adin1300_config_speed(d, param->speed_cap);
	if (ret)
		goto free_mdio;

	ret = adin1300_autoneg(d, param->autoneg);
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

int adin1300_soft_reset(struct adin1300_desc *dev)
{
	int ret;

	ret = adin1300_write(dev, ADIN1300_GE_SFT_RST, ADIN1300_GE_SFT_RST_MASK);
	if (ret < 0)
		return ret;

	no_os_mdelay(20);

	return 0;
}

int adin1300_hard_reset(struct adin1300_desc *dev)
{
	int ret;

	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;
	no_os_mdelay(1);
	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;
	no_os_mdelay(20);

	return 0;
}

int adin1300_remove(struct adin1300_desc *dev)
{
	no_os_mdio_remove(dev->mdio);
	no_os_gpio_remove(dev->reset_gpio);
	free(dev);
	return 0;
}

inline int adin1300_write(struct adin1300_desc *dev, uint32_t addr,
			  uint16_t val)
{
	return no_os_mdio_write(dev->mdio, addr, val);
}

inline int adin1300_read(struct adin1300_desc *dev, uint32_t addr,
			 uint16_t *val)
{
	return no_os_mdio_read(dev->mdio, addr, val);
}

int adin1300_write_bits(struct adin1300_desc *dev, uint32_t addr, uint16_t val,
			uint16_t bitmask)
{
	int ret;
	uint16_t rval;

	ret = no_os_mdio_read(dev->mdio, addr, &rval);
	if (ret)
		return ret;

	rval &= ~bitmask;
	rval |= (val & bitmask);

	return no_os_mdio_write(dev->mdio, addr, rval);
}

int adin1300_config_rgmii(struct adin1300_desc *dev,
			  struct adin1300_rgmii_config rgmii)
{
	uint16_t reg;

	reg = no_os_field_prep(ADIN1300_GE_RGMII_EN_MASK, 1) |
	      no_os_field_prep(ADIN1300_GE_RGMII_10_LOW_LTNCY_EN_MSK, 1) |
	      no_os_field_prep(ADIN1300_GE_RGMII_100_LOW_LTNCY_EN_MSK, 1);

	if (rgmii.tx_idelay_en)
		reg |= no_os_field_prep(ADIN1300_GE_RGMII_TX_ID_EN_MASK, 1) |
		       no_os_field_prep(ADIN1300_GE_RGMII_GTX_SEL_MASK, rgmii.tx_idelay);

	if (rgmii.rx_idelay_en)
		reg |= no_os_field_prep(ADIN1300_GE_RGMII_RX_ID_EN_MASK, 1) |
		       no_os_field_prep(ADIN1300_GE_RGMII_RX_SEL_MASK, rgmii.rx_idelay);

	return adin1300_write(dev, ADIN1300_GE_RGMII_CFG, reg);
}

int adin1300_config_gp_clk(struct adin1300_desc *dev, bool on,
			   enum adin1300_gp_clk_source source)
{
	int ret;
	uint16_t mask = ADIN1300_GE_CLK_RCVR_125_EN_MASK |
			ADIN1300_GE_CLK_FREE_125_EN_MASK |
			ADIN1300_GE_CLK_HRT_RCVR_EN_MASK |
			ADIN1300_GE_CLK_HRT_FREE_EN_MASK |
			ADIN1300_GE_CLK_25_EN_MASK;

	ret = adin1300_write_bits(dev, ADIN1300_GE_CLK_CFG, 0, mask);
	if (ret)
		return ret;
	if (on)
		return adin1300_write_bits(dev, ADIN1300_GE_CLK_CFG, NO_OS_BIT(source),
					   NO_OS_BIT(source));

	return 0;
}

int adin1300_config_clk25_ref(struct adin1300_desc *dev, bool on)
{
	return adin1300_write_bits(dev, ADIN1300_GE_CLK_CFG,
				   on ? ADIN1300_GE_REF_CLK_EN_MASK : 0, ADIN1300_GE_REF_CLK_EN_MASK);
}

int adin1300_config_speed(struct adin1300_desc *dev,
			  enum adin1300_speed speed_cap)
{
	int ret;
	uint16_t affected = ADIN1300_SELECTOR_ADV_MASK |
			    ADIN1300_HD_10_ADV_MASK |
			    ADIN1300_FD_10_ADV_MASK |
			    ADIN1300_HD_100_ADV_MASK |
			    ADIN1300_FD_100_ADV_MASK;
	uint16_t bits = no_os_field_prep(ADIN1300_SELECTOR_ADV_MASK, 1);


	bits |= no_os_field_prep(ADIN1300_HD_10_ADV_MASK, 1);
	if (speed_cap >= ADIN1300_10_BASE_T_FULLDUPLEX)
		bits |= no_os_field_prep(ADIN1300_FD_10_ADV_MASK, 1);
	if (speed_cap >= ADIN1300_100_BASE_TX_HALFDUPLEX)
		bits |= no_os_field_prep(ADIN1300_HD_100_ADV_MASK, 1);
	if (speed_cap >= ADIN1300_100_BASE_TX_FULLDUPLEX)
		bits |= no_os_field_prep(ADIN1300_FD_100_ADV_MASK, 1);
	ret = adin1300_write_bits(dev, ADIN1300_AUTONEG_ADV, bits, affected);
	if (ret)
		return ret;

	affected = ADIN1300_HD_1000_ADV_MASK | ADIN1300_FD_1000_ADV_MASK;
	bits = no_os_field_prep(ADIN1300_HD_1000_ADV_MASK,
				0); // 1000 half-duplex is not supported
	if (speed_cap >= ADIN1300_1000_BASE_T_FULLDUPLEX)
		bits |= no_os_field_prep(ADIN1300_FD_1000_ADV_MASK, 1);

	ret = adin1300_write_bits(dev, ADIN1300_MSTR_SLV_CONTROL, bits, affected);
	if (ret)
		return ret;

	affected = ADIN1300_DPLX_MODE_MASK | ADIN1300_SPEED_SEL_LSB_MASK |
		   ADIN1300_SPEED_SEL_MSB_MASK;
	if (speed_cap & 0x1)
		bits |= no_os_field_prep(ADIN1300_DPLX_MODE_MASK, 1);
	if (speed_cap & 0x2)
		bits |= no_os_field_prep(ADIN1300_SPEED_SEL_LSB_MASK, 1);
	if (speed_cap & 0x4)
		bits |= no_os_field_prep(ADIN1300_SPEED_SEL_MSB_MASK, 1);

	return adin1300_write_bits(dev, ADIN1300_MII_CONTROL, bits, affected);
}

int adin1300_autoneg(struct adin1300_desc *dev, bool on)
{
	uint16_t bits = ADIN1300_AUTONEG_EN_MASK | ADIN1300_RESTART_ANEG_MASK;

	return adin1300_write_bits(dev, ADIN1300_MII_CONTROL, on ? bits : 0, bits);
}

bool adin1300_link_is_up(struct adin1300_desc *dev)
{
	int ret;
	uint16_t val;

	ret = adin1300_read(dev, ADIN1300_PHY_STATUS_1, &val);
	if (!ret && no_os_field_get(ADIN1300_LINK_STAT_MASK, val))
		return true;

	return false;
}

enum adin1300_speed adin1300_resolved_speed(struct adin1300_desc *dev)
{
	uint16_t val;

	adin1300_read(dev, ADIN1300_PHY_STATUS_1, &val);

	return (enum adin1300_speed)no_os_field_get(ADIN1300_HCD_TECH_MASK, val);
}
