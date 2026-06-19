/***************************************************************************//**
*   @file   adin1320.c
*   @brief  Source file of the ADIN1320 driver.
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "adin1320.h"

/**
 * @brief Initialize the Device
 * @param dev - The device structure
 * @param param - Initialization parameter containing information about the
 * 		 device to be initialized.
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_init(struct adin1320_desc **dev,
		  struct adin1320_init_param *param)
{
	int ret;
	uint16_t val;
	uint16_t model_rev;
	struct adin1320_desc *d;

	if (!dev || !param)
		return -EINVAL;

	d = (struct adin1320_desc *) no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	/* Initialize MDIO */
	ret = no_os_mdio_init(&d->mdio, &param->mdio_param);
	if (ret)
		goto free_d;

	/* Perform reset */
	if (!param->reset_param) {
		ret = adin1320_soft_reset(d, ADIN1320_RESET_GE_SUBSYS_PIN);
		if (ret)
			goto free_mdio;
	} else {
		ret = no_os_gpio_get(&d->reset_gpio, param->reset_param);
		if (ret)
			goto free_mdio;

		ret = adin1320_hard_reset(d);
		if (ret)
			goto free_reset;
	}

	/* Check MDIO sanity through PHY ID check */
	ret = adin1320_read(d, ADIN1320_GE_PHY_ID_1, &val);
	if (ret)
		goto free_reset;

	/* Expected PHY_ID_1 register value */
	if (val != ADIN1320_GE_PHY_ID_1_RESET) {
		ret = -EFAULT;
		goto free_reset;
	}

	ret = adin1320_read(d, ADIN1320_GE_PHY_ID_2, &val);
	if (ret)
		goto free_reset;

	model_rev = no_os_field_get(ADIN1320_GE_MODEL_NUM_MASK, val);
	if (model_rev != ADIN1320_PHY_ID) {
		ret = -EFAULT;
		goto free_reset;
	}

	ret = adin1320_write_bits(d, ADIN1320_IRQ_MASK,
				  ADIN1320_HW_IRQ_EN_MASK |
				  ADIN1320_LNK_STAT_CHNG_IRQ_EN_MASK,
				  ADIN1320_HW_IRQ_EN_MASK |
				  ADIN1320_LNK_STAT_CHNG_IRQ_EN_MASK);
	if (ret)
		goto free_reset;

	*dev = d;

	return 0;
free_reset:
	if (d->reset_gpio)
		no_os_gpio_remove(d->reset_gpio);
free_mdio:
	no_os_mdio_remove(d->mdio);
free_d:
	no_os_free(d);
	return ret;
}

/**
 * @brief Perform Device Hard Reset
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_hard_reset(struct adin1320_desc *dev)
{
	int ret;

	if (!dev || !dev->reset_gpio)
		return -EINVAL;

	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* Minimum reset pulse length is 1 microsecond */
	no_os_mdelay(1);
	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Minimum 5ms required after reset deassertion before device can be used */
	no_os_mdelay(20);

	return 0;
}

/**
 * @brief Remove Initialization of the Device
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_remove(struct adin1320_desc *dev)
{
	int ret_mdio = 0;
	int ret_gpio = 0;

	if (!dev)
		return -EINVAL;

	if (dev->mdio)
		ret_mdio = no_os_mdio_remove(dev->mdio);

	if (dev->reset_gpio)
		ret_gpio = no_os_gpio_remove(dev->reset_gpio);

	no_os_free(dev);

	return ret_mdio ? ret_mdio : ret_gpio;
}

/**
 * @brief MDIO Write with Clause22 or Clause45
 * @param dev - The device structure
 * @param addr - Register Address
 * @param val - Value to write in the register
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_write(struct adin1320_desc *dev, uint32_t addr,
		   uint16_t val)
{
	if (!dev || !dev->mdio)
		return -EINVAL;

	return no_os_mdio_write(dev->mdio, addr, val);
}

/**
 * @brief MDIO Read with Clause22 or Clause45
 * @param dev - The device structure
 * @param addr - Register Address
 * @param val - Read value from the register
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_read(struct adin1320_desc *dev, uint32_t addr,
		  uint16_t *val)
{
	if (!dev || !dev->mdio)
		return -EINVAL;

	return no_os_mdio_read(dev->mdio, addr, val);
}

/**
 * @brief MDIO Write with Clause22 or Clause45 on a Specific Bit
 * @param dev - The device structure
 * @param addr - Register Address
 * @param val - Masked value to write on the bitmask
 * @param bitmask - Mask for the bits to be written on
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_write_bits(struct adin1320_desc *dev, uint32_t addr, uint16_t val,
			uint16_t bitmask)
{
	int ret;
	uint16_t rval;

	if (!dev || !dev->mdio)
		return -EINVAL;

	ret = no_os_mdio_read(dev->mdio, addr, &rval);
	if (ret)
		return ret;

	rval &= ~bitmask;
	rval |= (val & bitmask);

	return no_os_mdio_write(dev->mdio, addr, rval);
}

/**
 * @brief Configure RGMII Tx and Rx Delays
 * @param dev - The device structure
 * @param rgmii - Struct container for RGMII configurations
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_config_rgmii(struct adin1320_desc *dev,
			  struct adin1320_rgmii_config rgmii)
{
	uint16_t bitmask = ADIN1320_GE_RGMII_TX_ID_EN_MASK |
			   ADIN1320_GE_RGMII_RX_ID_EN_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if (!dev)
		return -EINVAL;

	if (rgmii.tx_idelay_en != ADIN1320_RGMII_IDELAY_ENABLE &&
	    rgmii.tx_idelay_en != ADIN1320_RGMII_IDELAY_DISABLE)
		return -EINVAL;

	if (rgmii.rx_idelay_en != ADIN1320_RGMII_IDELAY_ENABLE &&
	    rgmii.rx_idelay_en != ADIN1320_RGMII_IDELAY_DISABLE)
		return -EINVAL;

	if (rgmii.tx_idelay_en == ADIN1320_RGMII_IDELAY_ENABLE) {
		switch (rgmii.tx_idelay) {
		case ADIN1320_RGMII_2_00_NS:
		case ADIN1320_RGMII_1_60_NS:
		case ADIN1320_RGMII_1_80_NS:
		case ADIN1320_RGMII_2_20_NS:
		case ADIN1320_RGMII_2_40_NS:
			bitmask |= ADIN1320_GE_RGMII_GTX_SEL_MASK;
			write_bits |= ADIN1320_GE_RGMII_TX_ID_EN_MASK |
				      no_os_field_prep(ADIN1320_GE_RGMII_GTX_SEL_MASK,
						       rgmii.tx_idelay);
			break;
		default:
			return -EINVAL;
		}
	}

	if (rgmii.rx_idelay_en == ADIN1320_RGMII_IDELAY_ENABLE) {
		switch (rgmii.rx_idelay) {
		case ADIN1320_RGMII_2_00_NS:
		case ADIN1320_RGMII_1_60_NS:
		case ADIN1320_RGMII_1_80_NS:
		case ADIN1320_RGMII_2_20_NS:
		case ADIN1320_RGMII_2_40_NS:
			bitmask |= ADIN1320_GE_RGMII_RX_SEL_MASK;
			write_bits |= ADIN1320_GE_RGMII_RX_ID_EN_MASK |
				      no_os_field_prep(ADIN1320_GE_RGMII_RX_SEL_MASK,
						       rgmii.rx_idelay);
			break;
		default:
			return -EINVAL;
		}
	}

	return adin1320_write_bits(dev, ADIN1320_GE_RGMII_CFG, write_bits,
				   bitmask);
}

/**
 * @brief Configure GP Clock
 * @param dev - The device structure
 * @param source - GP Clock to Select
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_config_gp_clk(struct adin1320_desc *dev,
			   enum adin1320_gp_clk_source source)
{
	int ret;
	uint16_t mask = ADIN1320_GE_CLK_RCVR_125_EN_MASK |
			ADIN1320_GE_CLK_FREE_125_EN_MASK |
			ADIN1320_GE_CLK_HRT_RCVR_EN_MASK |
			ADIN1320_GE_CLK_HRT_FREE_EN_MASK |
			ADIN1320_GE_CLK_25_EN_MASK |
			ADIN1320_SD_RX_CLK_125_EN_MASK |
			ADIN1320_SD_TX_CLK_125_EN_MASK |
			ADIN1320_SD_RX_CLK_HRT_EN_MASK |
			ADIN1320_SD_TX_CLK_HRT_EN_MASK;

	if ((source <= ADIN1320_GP_CLK_MIN) || (source >= ADIN1320_GP_CLK_MAX))
		return -EINVAL;

	if (NO_OS_BIT(source) & ~(mask | NO_OS_BIT(ADIN1320_GP_CLK_DISABLE)))
		return -EINVAL;

	ret = adin1320_write_bits(dev, ADIN1320_GE_CLK_CFG,
				  ADIN1320_BITMASK_RESET, mask);
	if (ret)
		return ret;

	if (source != ADIN1320_GP_CLK_DISABLE)
		return adin1320_write_bits(dev, ADIN1320_GE_CLK_CFG,
					   NO_OS_BIT(source),
					   NO_OS_BIT(source));

	return 0;
}

/**
 * @brief Enable/disable GE Clock 25MHz Reference
 * @param dev - The device structure
 * @param enable - Enable/disable 25MHz Ref Clock
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_config_clk25_ref(struct adin1320_desc *dev,
			      enum adin1320_clk25_ref_enable enable)
{
	if ((enable != ADIN1320_CLK25_REF_DISABLE) &&
	    (enable != ADIN1320_CLK25_REF_ENABLE))
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_GE_CLK_CFG,
				   enable ? ADIN1320_GE_REF_CLK_EN_MASK :
				   ADIN1320_BITMASK_RESET,
				   ADIN1320_GE_REF_CLK_EN_MASK);
}

static int adin1320_autoneg(struct adin1320_desc *dev,
			    enum adin1320_autoneg_enable enable,
			    uint32_t reg_add_enable,
			    uint16_t bitm_enable,
			    uint32_t reg_add_restart,
			    uint16_t bitm_restart)
{
	int ret;

	if ((enable != ADIN1320_AUTONEG_DISABLE) &&
	    (enable != ADIN1320_AUTONEG_ENABLE))
		return -EINVAL;

	ret = adin1320_write_bits(dev, reg_add_enable,
				  enable ? bitm_enable :
				  ADIN1320_BITMASK_RESET,
				  bitm_enable);
	if (ret)
		return ret;

	return adin1320_write_bits(dev, reg_add_restart,
				   enable ? bitm_restart :
				   ADIN1320_BITMASK_RESET,
				   bitm_restart);
}

/**
 * @brief Enable/disable and Reset Autonegotiation for Copper Media
 * @param dev - The device structure
 * @param enable - Enable and reset autonegotiation, or disable autonegotiation
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_autoneg(struct adin1320_desc *dev,
			enum adin1320_autoneg_enable enable)
{
	return adin1320_autoneg(dev, enable, ADIN1320_MII_CONTROL,
				ADIN1320_AUTONEG_EN_MASK, ADIN1320_MII_CONTROL,
				ADIN1320_RESTART_ANEG_MASK);
}

/**
 * @brief Enable/disable and Reset Autonegotiation for SerDes Media
 * @param dev - The device structure
 * @param enable - Enable and reset autonegotiation, or disable autonegotiation
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_autoneg(struct adin1320_desc *dev,
			enum adin1320_autoneg_enable enable)
{
	return adin1320_autoneg(dev, enable, ADIN1320_GE_SD_CFG,
				ADIN1320_SD_AUTONEG_EN_CFG_MASK,
				ADIN1320_SD_CONTROL,
				ADIN1320_SD_RESTART_ANEG_MASK);
}

static int adin1320_resolved_speed(struct adin1320_desc *dev, uint32_t reg_add,
				   uint16_t bitmask, uint16_t *speed_val)
{
	int ret;
	uint16_t data;

	ret = adin1320_read(dev, reg_add, &data);
	if (ret)
		return ret;

	*speed_val = NO_OS_BIT(no_os_field_get(bitmask, data));

	return 0;
}

/**
 * @brief Get Resolved Speed for Copper Media
 * @param dev - The device structure
 * @param resolved_speed - Resolved speed read from HCD_TECH
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_resolved_speed(struct adin1320_desc *dev,
			       enum adin1320_cu_speed *resolved_speed)
{
	int ret;
	uint16_t speed_val;

	if (!resolved_speed)
		return -EINVAL;

	ret = adin1320_resolved_speed(dev, ADIN1320_PHY_STATUS_1,
				      ADIN1320_HCD_TECH_MASK, &speed_val);
	if (ret)
		return ret;

	*resolved_speed = (enum adin1320_cu_speed)speed_val;

	return 0;
}

/**
 * @brief Get Resolved Speed for SerDes Media
 * @param dev - The device structure
 * @param resolved_speed - Resolved speed read from SD_LINK_TYPE_CFG
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_resolved_speed(struct adin1320_desc *dev,
			       enum adin1320_sd_speed *resolved_speed)
{
	int ret;
	uint16_t speed_val;

	if (!resolved_speed)
		return -EINVAL;

	ret = adin1320_resolved_speed(dev, ADIN1320_GE_SD_CFG,
				      ADIN1320_SD_LINK_TYPE_CFG_MASK,
				      &speed_val);
	if (ret)
		return ret;

	*resolved_speed = (enum adin1320_sd_speed)speed_val;

	return 0;
}

/**
 * @brief Select and Perform Device Reset
 * @param dev - The device structure
 * @param reset_cfg - Type of soft reset to perform
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_soft_reset(struct adin1320_desc *dev,
			enum adin1320_soft_reset_option reset_cfg)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch (reset_cfg) {
	case ADIN1320_RESET_GE_SUBSYS:
		ret = adin1320_write_bits(dev,
					  ADIN1320_GE_SFT_RST_CFG_EN,
					  ADIN1320_BITMASK_RESET,
					  ADIN1320_GE_SFT_RST_CFG_EN_MASK);
		if (ret)
			return ret;

		ret = adin1320_write_bits(dev, ADIN1320_GE_SFT_RST,
					  ADIN1320_GE_SFT_RST_MASK,
					  ADIN1320_GE_SFT_RST_MASK);
		break;

	case ADIN1320_RESET_GE_SUBSYS_PIN:
		ret = adin1320_write_bits(dev,
					  ADIN1320_GE_SFT_RST_CFG_EN,
					  ADIN1320_GE_SFT_RST_CFG_EN_MASK,
					  ADIN1320_GE_SFT_RST_CFG_EN_MASK);
		if (ret)
			return ret;

		ret = adin1320_write_bits(dev, ADIN1320_GE_SFT_RST,
					  ADIN1320_GE_SFT_RST_MASK,
					  ADIN1320_GE_SFT_RST_MASK);
		break;

	default:
		return -EINVAL;
	}

	return ret;
}

static int adin1320_read_irq_status(struct adin1320_desc *dev,
				    bool *val_irq_pending,
				    uint16_t *val_irq_status, uint32_t reg_add)
{
	int ret;
	uint16_t data;

	if (!val_irq_pending || !val_irq_status)
		return -EINVAL;

	ret = adin1320_read(dev, reg_add, &data);
	if (ret)
		return ret;

	*val_irq_status = data;
	*val_irq_pending = (data != 0);

	return 0;
}

/**
 * @brief Read interrupt status for copper media
 * @param dev - The device structure
 * @param val_irq_pending - Set to true if any IRQ is pending, false otherwise
 * @param val_irq_status - The complete IRQ status register value showing
 * 			   status of all IRQs
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_read_irq_status(struct adin1320_desc *dev,
				bool *val_irq_pending,
				uint16_t *val_irq_status)
{
	return adin1320_read_irq_status(dev, val_irq_pending, val_irq_status,
					ADIN1320_IRQ_STATUS);
}

/**
 * @brief Read interrupt status for serdes media
 * @param dev - The device structure
 * @param val_irq_pending - Set to true if any IRQ is pending, false otherwise
 * @param val_irq_status - The complete IRQ status register value showing
 * 			   status of all IRQs
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_read_irq_status(struct adin1320_desc *dev,
				bool *val_irq_pending,
				uint16_t *val_irq_status)
{
	return adin1320_read_irq_status(dev, val_irq_pending, val_irq_status,
					ADIN1320_SD_IRQ_STATUS);
}

/**
 * @brief Enter/exit Software Powerdown
 * @param dev - The device structure
 * @param val - Enter/exit Software Powerdown
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_set_software_powerdown(struct adin1320_desc *dev,
				    enum adin1320_software_powerdown val)
{
	if ((val != ADIN1320_SOFTWARE_POWERUP) &&
	    (val != ADIN1320_SOFTWARE_POWERDOWN))
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_MII_CONTROL,
				   val ? ADIN1320_SFT_PD_MASK :
				   ADIN1320_BITMASK_RESET,
				   ADIN1320_SFT_PD_MASK);
}

/**
 * @brief Get Software Powerdown Status
 * @param dev - The device structure
 * @param val - Software Powerdown Status
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_software_powerdown(struct adin1320_desc *dev,
				    enum adin1320_software_powerdown *val)
{
	int ret;
	uint16_t data;
	uint16_t bit_value;

	if (!val)
		return -EINVAL;

	ret = adin1320_read(dev, ADIN1320_MII_CONTROL, &data);
	if (ret)
		return ret;

	bit_value = no_os_field_get(ADIN1320_SFT_PD_MASK, data);
	*val = (enum adin1320_software_powerdown)bit_value;

	return 0;
}

/**
 * @brief Get Device ID
 * @param dev - The device structure
 * @param device_id - Combined device ID (upper 16 bits: PHY ID 1, lower 16 bits: PHY ID 2)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_device_id(struct adin1320_desc *dev, uint32_t *device_id)
{
	int ret;
	uint16_t val_id1, val_id2;

	if (!device_id)
		return -EINVAL;

	ret = adin1320_read(dev, ADIN1320_GE_PHY_ID_1, &val_id1);
	if (ret)
		return ret;

	ret = adin1320_read(dev, ADIN1320_GE_PHY_ID_2, &val_id2);
	if (ret)
		return ret;

	*device_id = no_os_field_prep(NO_OS_GENMASK(ADIN1320_DEVICE_ID1_HIGH_BIT,
				      ADIN1320_DEVICE_ID1_LOW_BIT),
				      val_id1) |
		     no_os_field_prep(NO_OS_GENMASK(ADIN1320_DEVICE_ID2_HIGH_BIT,
				      ADIN1320_DEVICE_ID2_LOW_BIT),
				      val_id2);

	return 0;
}

/**
 * @brief Allows the RMII FIFO to be Reset
 * @note This function places the device in software powerdown.
 *       The caller is responsible for calling
 *       adin1320_set_software_powerdown(dev, ADIN1320_SOFTWARE_POWERUP)
 *       to resume normal operation after the FIFO reset.
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_reset_rmii_fifo(struct adin1320_desc *dev)
{
	int ret;

	/* Software Powerdown */
	ret = adin1320_set_software_powerdown(dev,
					      ADIN1320_SOFTWARE_POWERDOWN);
	if (ret)
		return ret;

	return adin1320_write_bits(dev, ADIN1320_GE_RMII_CFG,
				   ADIN1320_GE_RMII_FIFO_RST_MASK,
				   ADIN1320_GE_RMII_FIFO_RST_MASK);
}

/**
 * @brief MAC Interface Configuration
 * @note This function places the device in software powerdown.
 *       The caller is responsible for calling
 *       adin1320_set_software_powerdown(dev, ADIN1320_SOFTWARE_POWERUP)
 *       to resume normal operation after the interface is configured.
 * @param dev - The device structure
 * @param val - Select MAC Interface
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_select_mac_interface(struct adin1320_desc *dev,
				  enum adin1320_mac_interface val)
{
	int ret;
	uint16_t rgmii_bits = ADIN1320_GE_RGMII_EN_MASK |
			      ADIN1320_GE_RGMII_TX_ID_EN_MASK;
	uint16_t rmii_bits = ADIN1320_GE_RMII_EN_MASK;
	uint16_t sgmii_bits = ADIN1320_SD_SGMII_EN_MASK;

	switch (val) {
	case ADIN1320_RGMII_MAC_INTERFACE:
	case ADIN1320_RMII_MAC_INTERFACE:
	case ADIN1320_MII_MAC_INTERFACE:
	case ADIN1320_SGMII_MAC_INTERFACE:
		break;
	default:
		return -EINVAL;
	}

	/* Software Powerdown */
	ret = adin1320_set_software_powerdown(dev,
					      ADIN1320_SOFTWARE_POWERDOWN);
	if (ret)
		return ret;

	/* Disable all MAC interfaces first */
	ret = adin1320_write_bits(dev, ADIN1320_GE_RGMII_CFG,
				  ADIN1320_BITMASK_RESET, rgmii_bits);
	if (ret)
		return ret;

	ret = adin1320_write_bits(dev, ADIN1320_GE_RMII_CFG,
				  ADIN1320_BITMASK_RESET, rmii_bits);
	if (ret)
		return ret;

	ret = adin1320_write_bits(dev, ADIN1320_GE_SD_CFG,
				  ADIN1320_BITMASK_RESET, sgmii_bits);
	if (ret)
		return ret;

	/* Enable the selected interface */
	switch (val) {
	case ADIN1320_RGMII_MAC_INTERFACE:
		return adin1320_write_bits(dev, ADIN1320_GE_RGMII_CFG,
					   rgmii_bits, rgmii_bits);
	case ADIN1320_RMII_MAC_INTERFACE:
		return adin1320_write_bits(dev, ADIN1320_GE_RMII_CFG,
					   rmii_bits, rmii_bits);
	case ADIN1320_SGMII_MAC_INTERFACE:
		return adin1320_write_bits(dev, ADIN1320_GE_SD_CFG,
					   sgmii_bits, sgmii_bits);
	case ADIN1320_MII_MAC_INTERFACE:
		/* MII: all interfaces disabled (default) */
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Enables/Disables the Link
 * @param dev - The device structure
 * @param val - Enable/disable link
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_link_cfg(struct adin1320_desc *dev,
			 enum adin1320_link_cfg_enable val)
{
	if ((val != ADIN1320_LINK_CFG_DISABLE) &&
	    (val != ADIN1320_LINK_CFG_ENABLE))
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_PHY_CTRL_3,
				   val ? ADIN1320_LINK_EN_MASK :
				   ADIN1320_BITMASK_RESET,
				   ADIN1320_LINK_EN_MASK);
}

static int adin1320_get_mii_status(struct adin1320_desc *dev,
				   struct adin1320_mii_status *mii_status,
				   uint32_t reg_add)
{
	int ret;
	uint16_t data;
	uint16_t bitm_link_stat, bitm_autoneg_done, bitm_jabber_det, bitm_rem_flt;

	if (!mii_status)
		return -EINVAL;

	if ((mii_status->link_stat <= ADIN1320_LINK_STAT_MIN) ||
	    (mii_status->link_stat >= ADIN1320_LINK_STAT_MAX))
		return -EINVAL;

	if ((mii_status->lat_link_stat <= ADIN1320_LAT_LINK_STAT_MIN) ||
	    (mii_status->lat_link_stat >= ADIN1320_LAT_LINK_STAT_MAX))
		return -EINVAL;

	if ((mii_status->autoneg_stat <= ADIN1320_AUTONEG_STAT_MIN) ||
	    (mii_status->autoneg_stat >= ADIN1320_AUTONEG_STAT_MAX))
		return -EINVAL;

	if ((mii_status->lat_jabber_det <= ADIN1320_LAT_JABBER_DET_MIN) ||
	    (mii_status->lat_jabber_det >= ADIN1320_LAT_JABBER_DET_MAX))
		return -EINVAL;

	if ((mii_status->lat_rem_flt <= ADIN1320_LAT_REM_FLT_MIN) ||
	    (mii_status->lat_rem_flt >= ADIN1320_LAT_REM_FLT_MAX))
		return -EINVAL;

	/* Select bit masks based on register address */
	if (reg_add == ADIN1320_MII_STATUS) {
		bitm_link_stat = ADIN1320_LINK_STAT_LAT_MASK;
		bitm_autoneg_done = ADIN1320_AUTONEG_DONE_MASK;
		bitm_jabber_det = ADIN1320_JABBER_DET_LAT_MASK;
		bitm_rem_flt = ADIN1320_REM_FLT_LAT_MASK;
	} else if (reg_add == ADIN1320_SD_STATUS) {
		bitm_link_stat = ADIN1320_SD_LINK_STAT_OK_LL_MASK;
		bitm_autoneg_done = ADIN1320_SD_AN_DONE_MASK;
		bitm_jabber_det = ADIN1320_SD_JABBER_DET_LH_MASK;
		bitm_rem_flt = ADIN1320_SD_REM_FLT_LH_MASK;
	} else {
		return -EINVAL;
	}

	ret = adin1320_read(dev, reg_add, &data);
	if (ret)
		return ret;

	if (mii_status->lat_rem_flt) {
		if (no_os_field_get(bitm_rem_flt, data)) {
			mii_status->lat_rem_flt = ADIN1320_LAT_REM_FLT_DETECTED;
		} else {
			mii_status->lat_rem_flt = ADIN1320_LAT_REM_FLT_NONE_DETECTED;
		}
	}

	if (mii_status->lat_jabber_det) {
		if (no_os_field_get(bitm_jabber_det, data)) {
			mii_status->lat_jabber_det = ADIN1320_LAT_JABBER_DET_DETECTED;
		} else {
			mii_status->lat_jabber_det = ADIN1320_LAT_JABBER_DET_NONE_DETECTED;
		}
	}

	if (mii_status->autoneg_stat) {
		if (no_os_field_get(bitm_autoneg_done, data)) {
			mii_status->autoneg_stat = ADIN1320_AUTONEG_STAT_DONE;
		} else {
			mii_status->autoneg_stat = ADIN1320_AUTONEG_STAT_NOT_DONE;
		}
	}

	if (mii_status->lat_link_stat) {
		if (no_os_field_get(bitm_link_stat, data)) {
			mii_status->lat_link_stat = ADIN1320_LAT_LINK_STAT_NO_LINK_DROP;
		} else {
			mii_status->lat_link_stat = ADIN1320_LAT_LINK_STAT_LINK_DROPPED;
		}
	}

	if (mii_status->link_stat) {
		ret = adin1320_read(dev, reg_add, &data);
		if (ret)
			return ret;

		if (no_os_field_get(bitm_link_stat, data)) {
			mii_status->link_stat = ADIN1320_LINK_STAT_LINKUP;
		} else {
			mii_status->link_stat = ADIN1320_LINK_STAT_LINKDOWN;
		}
	}

	return 0;
}

/**
 * @brief Get Different Status from Copper Media MII Status Register
 * @param dev - The device structure
 * @param mii_status - Initialized struct container for MII status to be filled
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_get_mii_status(struct adin1320_desc *dev,
			       struct adin1320_mii_status *mii_status)
{
	return adin1320_get_mii_status(dev, mii_status, ADIN1320_MII_STATUS);
}

/**
 * @brief Get Different Status from SerDes Media MII Status Register
 * @param dev - The device structure
 * @param mii_status - Initialized struct container for MII status to be filled
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_get_mii_status(struct adin1320_desc *dev,
			       struct adin1320_mii_status *mii_status)
{
	return adin1320_get_mii_status(dev, mii_status, ADIN1320_SD_STATUS);
}

/**
 * @brief Configure Autonegotiation Advertisement Speeds for Copper Media
 * @param dev - The device structure
 * @param autoneg_adv_speeds - Autoneg Adv speeds to enable
 * 		 (masked with adin1320_cu_speed)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_autoneg_adv_cfg(struct adin1320_desc *dev,
				uint16_t autoneg_adv_speeds)
{
	int ret;
	uint16_t bitmask = ADIN1320_HD_10_ADV_MASK | ADIN1320_FD_10_ADV_MASK |
			   ADIN1320_HD_100_ADV_MASK | ADIN1320_FD_100_ADV_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if (autoneg_adv_speeds >= ADIN1320_CU_SPEED_MAX)
		return -EINVAL;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_10BASE_T_HD)
		write_bits |= ADIN1320_HD_10_ADV_MASK;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_10BASE_T_FD)
		write_bits |= ADIN1320_FD_10_ADV_MASK;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_100BASE_TX_HD)
		write_bits |= ADIN1320_HD_100_ADV_MASK;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_100BASE_TX_FD)
		write_bits |= ADIN1320_FD_100_ADV_MASK;

	ret = adin1320_write_bits(dev, ADIN1320_AUTONEG_ADV, write_bits,
				  bitmask);
	if (ret)
		return ret;

	bitmask = ADIN1320_HD_1000_ADV_MASK | ADIN1320_FD_1000_ADV_MASK;
	write_bits = ADIN1320_BITMASK_RESET;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_1000BASE_T_HD)
		write_bits |= ADIN1320_HD_1000_ADV_MASK;

	if (autoneg_adv_speeds & ADIN1320_CU_SPEED_1000BASE_T_FD)
		write_bits |= ADIN1320_FD_1000_ADV_MASK;

	return adin1320_write_bits(dev, ADIN1320_MSTR_SLV_CONTROL, write_bits,
				   bitmask);
}

/**
 * @brief Configure Autonegotiation Advertisement Speeds for SerDes Media
 * @param dev - The device structure
 * @param autoneg_adv_speeds - Autoneg Adv speeds to enable
 * 		 (masked with adin1320_fi_speed)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_autoneg_adv_cfg(struct adin1320_desc *dev,
				uint16_t autoneg_adv_speeds)
{
	uint16_t bitmask = ADIN1320_SD_HD_1000ADV_MASK |
			   ADIN1320_SD_FD_1000ADV_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if (autoneg_adv_speeds & ~(ADIN1320_SD_SPEED_1000BASE_X_HD |
				   ADIN1320_SD_SPEED_1000BASE_X_FD))
		return -EINVAL;

	if (autoneg_adv_speeds & ADIN1320_SD_SPEED_1000BASE_X_HD)
		write_bits |= ADIN1320_SD_HD_1000ADV_MASK;

	if (autoneg_adv_speeds & ADIN1320_SD_SPEED_1000BASE_X_FD)
		write_bits |= ADIN1320_SD_FD_1000ADV_MASK;

	return adin1320_write_bits(dev, ADIN1320_SD_AUTONEG_ADV, write_bits,
				   bitmask);
}

/**
 * @brief Get Enabled Autonegotiation Advertised Speeds for Copper Media
 * @param dev - The device structure
 * @param autoneg_adv_speeds - Enabled Autoneg Adv speeds
 * 		 (masked with adin1320_cu_speed)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_get_autoneg_adv(struct adin1320_desc *dev,
				uint16_t *autoneg_adv_speeds)
{
	int ret;
	uint16_t speed_data;

	if (!autoneg_adv_speeds)
		return -EINVAL;

	*autoneg_adv_speeds = 0;

	ret = adin1320_read(dev, ADIN1320_AUTONEG_ADV, &speed_data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_HD_10_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_10BASE_T_HD;

	if (no_os_field_get(ADIN1320_FD_10_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_10BASE_T_FD;

	if (no_os_field_get(ADIN1320_HD_100_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_100BASE_TX_HD;

	if (no_os_field_get(ADIN1320_FD_100_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_100BASE_TX_FD;

	ret = adin1320_read(dev, ADIN1320_MSTR_SLV_CONTROL, &speed_data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_HD_1000_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_1000BASE_T_HD;

	if (no_os_field_get(ADIN1320_FD_1000_ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_CU_SPEED_1000BASE_T_FD;

	return 0;
}

/**
 * @brief Get Enabled Autonegotiation Advertised Speeds for SerDes Media
 * @param dev - The device structure
 * @param autoneg_adv_speeds - Enabled Autoneg Adv speeds
 * 		 (masked with adin1320_fi_speed)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_get_autoneg_adv(struct adin1320_desc *dev,
				uint16_t *autoneg_adv_speeds)
{
	int ret;
	uint16_t speed_data;

	if (!autoneg_adv_speeds)
		return -EINVAL;

	*autoneg_adv_speeds = 0;

	ret = adin1320_read(dev, ADIN1320_SD_AUTONEG_ADV, &speed_data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_SD_HD_1000ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_SD_SPEED_1000BASE_X_HD;

	if (no_os_field_get(ADIN1320_SD_FD_1000ADV_MASK, speed_data))
		*autoneg_adv_speeds |= ADIN1320_SD_SPEED_1000BASE_X_FD;

	return 0;
}

/**
 * @brief Auto MDIX Config
 * @param dev - The device structure
 * @param val - MDIX configuration
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_auto_mdix_cfg(struct adin1320_desc *dev,
			   enum adin1320_auto_mdix val)
{
	uint16_t bitmask = ADIN1320_MAN_MDIX_MASK | ADIN1320_AUTO_MDI_EN_MASK;
	uint16_t write_bits;

	switch (val) {
	case ADIN1320_MANUAL_MDI:
		write_bits = ADIN1320_BITMASK_RESET;
		break;

	case ADIN1320_MANUAL_MDIX:
		write_bits = ADIN1320_MAN_MDIX_MASK;
		break;

	case ADIN1320_AUTO_MDIX_PREFER_MDIX:
		write_bits = ADIN1320_MAN_MDIX_MASK |
			     ADIN1320_AUTO_MDI_EN_MASK;
		break;

	case ADIN1320_AUTO_MDIX_PREFER_MDI:
		write_bits = ADIN1320_AUTO_MDI_EN_MASK;
		break;

	default:
		return -EINVAL;
	}

	return adin1320_write_bits(dev, ADIN1320_PHY_CTRL_1, write_bits,
				   bitmask);
}

/**
 * @brief Energy Detect Power Down mode Config
 * @param dev - The device structure
 * @param val - Energy power down mode
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_energy_detect_pwd_cfg(struct adin1320_desc *dev,
				   enum adin1320_energy_detect_pwd val)
{
	uint16_t bitmask;
	uint16_t write_bits;

	switch (val) {
	case ADIN1320_NRG_PD_EN:
		bitmask = ADIN1320_NRG_PD_EN_MASK;
		write_bits = ADIN1320_NRG_PD_EN_MASK;
		break;

	case ADIN1320_NRG_PD_DIS:
		bitmask = ADIN1320_NRG_PD_EN_MASK;
		write_bits = ADIN1320_BITMASK_RESET;
		break;

	case ADIN1320_NRG_PD_TX_DIS:
		bitmask = ADIN1320_NRG_PD_EN_MASK |
			  ADIN1320_NRG_PD_TX_EN_MASK;
		write_bits = ADIN1320_NRG_PD_EN_MASK;
		break;

	case ADIN1320_NRG_PD_TX_EN:
		bitmask = ADIN1320_NRG_PD_EN_MASK |
			  ADIN1320_NRG_PD_TX_EN_MASK;
		write_bits = ADIN1320_NRG_PD_EN_MASK |
			     ADIN1320_NRG_PD_TX_EN_MASK;
		break;

	default:
		return -EINVAL;
	}

	return adin1320_write_bits(dev, ADIN1320_PHY_CTRL_STATUS_2, write_bits,
				   bitmask);
}

/**
 * @brief Get Energy Detect Power Down mode Status
 * @param dev - The device structure
 * @param val - Energy Detect Status
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_energy_detect_pwd_stat(struct adin1320_desc *dev,
					enum adin1320_edpd_stat *val)
{
	int ret;
	uint16_t data;
	uint16_t bit_value;

	if (!val)
		return -EINVAL;

	ret = adin1320_read(dev, ADIN1320_PHY_CTRL_STATUS_2, &data);
	if (ret)
		return ret;

	bit_value = no_os_field_get(ADIN1320_PHY_IN_NRG_PD_MASK, data);
	*val = (enum adin1320_edpd_stat)bit_value;

	return 0;
}

/**
 * @brief Configure Downspeed
 * @param dev - The device structure
 * @param downspeeds - Downspeeds to enable (masked with adin1320_downspeed)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_downspeed_cfg(struct adin1320_desc *dev, uint8_t downspeeds)
{
	int ret;
	uint16_t bitmask = ADIN1320_DN_SPEED_TO_10_EN_MASK |
			   ADIN1320_DN_SPEED_TO_100_EN_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if (downspeeds >= ADIN1320_DOWNSPEED_CFG_MAX)
		return -EINVAL;

	ret = adin1320_cu_autoneg(dev, ADIN1320_AUTONEG_ENABLE);
	if (ret)
		return ret;

	if (downspeeds & ADIN1320_DOWNSPEED_TO_10)
		write_bits |= ADIN1320_DN_SPEED_TO_10_EN_MASK;

	if (downspeeds & ADIN1320_DOWNSPEED_TO_100)
		write_bits |= ADIN1320_DN_SPEED_TO_100_EN_MASK;

	return adin1320_write_bits(dev, ADIN1320_PHY_CTRL_2, write_bits,
				   bitmask);
}

/**
 * @brief Downspeed - Number of retries
 * @param dev - The device structure
 * @param val - Number of retries
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_set_downspeed_retries(struct adin1320_desc *dev, uint16_t val)
{
	uint16_t write_bits;

	if (val >= ADIN1320_SET_DOWNSPEED_RETRIES_MAX)
		return -EINVAL;

	write_bits = no_os_field_prep(ADIN1320_NUM_SPEED_RETRY_MASK, val);

	return adin1320_write_bits(dev, ADIN1320_PHY_CTRL_3, write_bits,
				   ADIN1320_NUM_SPEED_RETRY_MASK);
}

/**
 * @brief Set Force Speed for Copper Media
 * @param dev - The device structure
 * @param val - Forced speed to select
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_forced_speed(struct adin1320_desc *dev,
			     enum adin1320_cu_speed val)
{
	uint16_t bitmask = ADIN1320_SPEED_SEL_MSB_MASK |
			   ADIN1320_SPEED_SEL_LSB_MASK |
			   ADIN1320_DPLX_MODE_MASK;
	uint16_t write_bits;

	switch (val) {
	case ADIN1320_CU_SPEED_10BASE_T_HD:
		write_bits = ADIN1320_BITMASK_RESET;
		break;

	case ADIN1320_CU_SPEED_10BASE_T_FD:
		write_bits = ADIN1320_DPLX_MODE_MASK;
		break;

	case ADIN1320_CU_SPEED_100BASE_TX_HD:
		write_bits = ADIN1320_SPEED_SEL_LSB_MASK;
		break;

	case ADIN1320_CU_SPEED_100BASE_TX_FD:
		write_bits = ADIN1320_SPEED_SEL_LSB_MASK |
			     ADIN1320_DPLX_MODE_MASK;
		break;

	/*
	 * Auto-negotiation is always used for a 1000BASE-T link, as required
	 * by the IEEE standards. Use adin1320_cu_autoneg_adv_cfg()
	 * and adin1320_cu_autoneg() instead.
	 */

	default:
		return -EINVAL;
	}

	return adin1320_write_bits(dev, ADIN1320_MII_CONTROL, write_bits,
				   bitmask);
}

/**
 * @brief Set Force Speed for SerDes Media
 * @note This function places the device in software powerdown.
 *       The caller is responsible for calling
 *       adin1320_set_software_powerdown(dev, ADIN1320_SOFTWARE_POWERUP)
 *       to resume normal operation after the speed is configured.
 * @param dev - The device structure
 * @param forced_speed - Forced speed to select
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_forced_speed(struct adin1320_desc *dev,
			     enum adin1320_sd_speed forced_speed)
{
	int ret;
	uint16_t write_bits;
	uint16_t bitmask = ADIN1320_SD_LINK_TYPE_CFG_MASK;

	switch (forced_speed) {
	case ADIN1320_SD_SPEED_100BASE_FX_HD:
	case ADIN1320_SD_SPEED_100BASE_FX_FD:
	case ADIN1320_SD_SPEED_1000BASE_KX:
	case ADIN1320_SD_SPEED_1000BASE_X_HD:
	case ADIN1320_SD_SPEED_1000BASE_X_FD:
	case ADIN1320_SD_SPEED_1000BASE_X_HDFD:
		break;
	default:
		return -EINVAL;
	}

	write_bits = no_os_field_prep(ADIN1320_SD_LINK_TYPE_CFG_MASK,
				      no_os_log_base_2(forced_speed));

	ret = adin1320_set_software_powerdown(dev,
					      ADIN1320_SOFTWARE_POWERDOWN);
	if (ret)
		return ret;

	return adin1320_write_bits(dev, ADIN1320_GE_SD_CFG, write_bits,
				   bitmask);
}

/**
 * @brief Set Energy Efficient Ethernet speeds
 * @param dev - The device structure
 * @param eee_speeds - EEE speeds to enable (masked with adin1320_eee_speeds)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_set_eee(struct adin1320_desc *dev, uint8_t eee_speeds)
{
	uint16_t bitmask = ADIN1320_EEE_100_ADV_MASK |
			   ADIN1320_EEE_1000_ADV_MASK |
			   ADIN1320_EEE_1000_KX_ADV_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if (eee_speeds >= ADIN1320_EEE_MAX)
		return -EINVAL;

	if (eee_speeds & ADIN1320_EEE_100_ADV)
		write_bits |= ADIN1320_EEE_100_ADV_MASK;

	if (eee_speeds & ADIN1320_EEE_1000_ADV)
		write_bits |= ADIN1320_EEE_1000_ADV_MASK;

	if (eee_speeds & ADIN1320_EEE_1000_KX_ADV)
		write_bits |= ADIN1320_EEE_1000_KX_ADV_MASK;

	return adin1320_write_bits(dev, ADIN1320_EEE_ADV, write_bits, bitmask);
}

/**
 * @brief Get Energy Efficient Ethernet Status
 * @param dev - The device structure
 * @param eee_speeds - Enabled EEE speeds (masked with adin1320_eee_speeds)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_eee(struct adin1320_desc *dev, uint8_t *eee_speeds)
{
	int ret;
	uint16_t speed_data;

	if (!eee_speeds)
		return -EINVAL;

	*eee_speeds = 0;

	ret = adin1320_read(dev, ADIN1320_EEE_ADV, &speed_data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_EEE_100_ADV_MASK, speed_data))
		*eee_speeds |= ADIN1320_EEE_100_ADV;

	if (no_os_field_get(ADIN1320_EEE_1000_ADV_MASK, speed_data))
		*eee_speeds |= ADIN1320_EEE_1000_ADV;

	if (no_os_field_get(ADIN1320_EEE_1000_KX_ADV_MASK, speed_data))
		*eee_speeds |= ADIN1320_EEE_1000_KX_ADV;

	return 0;
}

/**
 * @brief Get Master Slave Status
 * @param dev - The device structure
 * @param val - Master slave status
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_master_slave_status(struct adin1320_desc *dev,
				     enum adin1320_master_slave_status *val)
{
	int ret;
	uint16_t data;

	if (!val)
		return -EINVAL;

	ret = adin1320_read(dev, ADIN1320_MSTR_SLV_STATUS, &data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_MSTR_SLV_FLT_MASK, data))
		*val = ADIN1320_MAST_SLAVE_FAULT_DETECT;
	else if (no_os_field_get(ADIN1320_MSTR_RSLVD_MASK, data))
		*val = ADIN1320_RESOLVED_TO_MASTER;
	else
		*val = ADIN1320_RESOLVED_TO_SLAVE;

	return 0;
}

/**
 * @brief Configure PHY as Master or Slave for 1 Gb speed only
 * @param dev - The device structure
 * @param val - Config as Master or Slave
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_master_slave_config(struct adin1320_desc *dev,
				 enum adin1320_adv_master_slave_cfg val)
{
	uint16_t bitmask = ADIN1320_MAN_MSTR_SLV_EN_ADV_MASK |
			   ADIN1320_MAN_MSTR_ADV_MASK;
	uint16_t write_bits;

	switch (val) {
	case ADIN1320_MAN_ADV_MASTER:
		write_bits = ADIN1320_MAN_MSTR_SLV_EN_ADV_MASK |
			     ADIN1320_MAN_MSTR_ADV_MASK;
		break;

	case ADIN1320_MAN_ADV_SLAVE:
		write_bits = ADIN1320_MAN_MSTR_SLV_EN_ADV_MASK;
		break;

	case ADIN1320_MAN_MSTR_SLV_DIS:
		write_bits = ADIN1320_BITMASK_RESET;
		break;

	default:
		return -EINVAL;
	}

	return adin1320_write_bits(dev, ADIN1320_MSTR_SLV_CONTROL, write_bits,
				   bitmask);
}

/**
 * @brief Configure the interrupt mask register for copper media
 * @param dev - The device structure
 * @param callback_events - Bitmask of interrupt events to enable
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_config_interrupt(struct adin1320_desc *dev,
				 uint32_t callback_events)
{
	uint16_t bitmask = ADIN1320_HW_IRQ_EN_MASK |
			   ADIN1320_SPEED_CHG_IRQ_EN_MASK |
			   ADIN1320_LNK_STAT_CHNG_IRQ_EN_MASK |
			   ADIN1320_RX_STAT_CHNG_IRQ_EN_MASK |
			   ADIN1320_FIFO_OU_IRQ_EN_MASK |
			   ADIN1320_IDLE_ERR_CNT_IRQ_EN_MASK |
			   ADIN1320_PAGE_RX_IRQ_EN_MASK |
			   ADIN1320_FC_FG_IRQ_EN_MASK |
			   ADIN1320_AN_STAT_CHNG_IRQ_EN_MASK |
			   ADIN1320_MDIO_SYNC_IRQ_EN_MASK |
			   ADIN1320_CBL_DIAG_IRQ_EN_MASK;

	if (callback_events & ~bitmask)
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_IRQ_MASK, callback_events,
				   bitmask);
}

/**
 * @brief Configure the interrupt mask register for serdes media
 * @param dev - The device structure
 * @param callback_events - Bitmask of interrupt events to enable
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_config_interrupt(struct adin1320_desc *dev,
				 uint32_t callback_events)
{
	uint16_t bitmask = ADIN1320_SD_MDIO_ERR_IRQ_EN_MASK |
			   ADIN1320_SD_LINK_STAT_CHG_IRQ_EN_MASK |
			   ADIN1320_SD_FIFO_OU_IRQ_EN_MASK |
			   ADIN1320_SD_PAGE_RX_IRQ_EN_MASK |
			   ADIN1320_SD_FC_FG_IRQ_EN_MASK |
			   ADIN1320_SD_AN_STAT_CHG_IRQ_EN_MASK |
			   ADIN1320_SD_TX_PLL_LCK_LOST_IRQ_EN_MASK |
			   ADIN1320_SD_RX_PLL_LCK_LOST_IRQ_EN_MASK |
			   ADIN1320_SD_OS_RX_PLL_LCK_LOST_IRQ_EN_MASK;

	if (callback_events & ~bitmask)
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_SD_IRQ_MASK, callback_events,
				   bitmask);
}

/**
 * @brief Configure general LED settings
 * @param dev - The device structure
 * @param enable_led_output - Enable/Disable LED output
 * @param pulse_stretch - Pulse stretch duration selection
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_led_cfg(struct adin1320_desc *dev,
		     enum adin1320_led_output_enable enable_led_output,
		     enum adin1320_led_pul_str_dur_select pulse_stretch)
{
	uint16_t bitmask = ADIN1320_LED_OE_N_MASK |
			   ADIN1320_LED_PUL_STR_EN_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;

	if ((enable_led_output != ADIN1320_LED_OUTPUT_ENABLE) &&
	    (enable_led_output != ADIN1320_LED_OUTPUT_DISABLE))
		return -EINVAL;

	if ((pulse_stretch <= ADIN1320_LED_PUL_STR_DUR_SELECT_MIN) ||
	    (pulse_stretch >= ADIN1320_LED_PUL_STR_DUR_SELECT_MAX))
		return -EINVAL;

	if (pulse_stretch != ADIN1320_LED_PUL_STR_DISABLE) {
		bitmask |= ADIN1320_LED_PUL_STR_DUR_SEL_MASK;
		write_bits |= ADIN1320_LED_PUL_STR_EN_MASK |
			      no_os_field_prep(ADIN1320_LED_PUL_STR_DUR_SEL_MASK,
					       pulse_stretch);
	}

	/* LED_OE_N has inverted logic: 0 = output enabled, 1 = output disabled */
	write_bits |= no_os_field_prep(ADIN1320_LED_OE_N_MASK,
				       !enable_led_output);

	return adin1320_write_bits(dev, ADIN1320_LED_CTRL_1, write_bits,
				   bitmask);
}

/**
 * @brief Configure LED settings for Copper Media
 * @param dev - The device structure
 * @param led_sel - LED to configure
 * @param led_pat - LED pattern selection
 * @param led_pause - Pause duration between patterns
 * @param led_cfg - LED configuration
 * @param led_inv_state - LED invert state
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_cu_led_ctrl(struct adin1320_desc *dev,
			 enum adin1320_led_sel led_sel, uint8_t led_pat,
			 uint8_t led_pause, enum adin1320_cu_led_cfg led_cfg,
			 enum adin1320_led_inv_state led_inv_state)
{
	int ret;
	uint16_t bitmask_cfg;
	uint16_t bitmask_ext_cfg;
	uint16_t bitmask_inv;
	uint16_t bitmask;
	uint16_t bit_value;
	uint16_t write_bits;

	if ((led_sel <= ADIN1320_LED_SEL_MIN) ||
	    (led_sel >= ADIN1320_LED_SEL_MAX))
		return -EINVAL;

	if (led_pause >= ADIN1320_CU_LED_CTRL_LED_PAUSE_MAX)
		return -EINVAL;

	if ((led_cfg <= ADIN1320_CU_LED_CFG_MIN) ||
	    (led_cfg >= ADIN1320_CU_LED_CFG_MAX))
		return -EINVAL;

	if ((led_inv_state != ADIN1320_LED_INV_STATE_ACTIVE_HIGH) &&
	    (led_inv_state != ADIN1320_LED_INV_STATE_ACTIVE_LOW))
		return -EINVAL;

	switch (led_sel) {
	case ADIN1320_LED_SEL_0:
		bitmask_cfg = ADIN1320_LED_0_CFG_MASK;
		bitmask_ext_cfg = ADIN1320_LED_0_EXT_CFG_EN_MASK;
		bitmask_inv = ADIN1320_LED_A_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_1:
		bitmask_cfg = ADIN1320_LED_1_CFG_MASK;
		bitmask_ext_cfg = ADIN1320_LED_1_EXT_CFG_EN_MASK;
		bitmask_inv = ADIN1320_LED_B_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_2:
		bitmask_cfg = ADIN1320_LED_2_CFG_MASK;
		bitmask_ext_cfg = ADIN1320_LED_2_EXT_CFG_EN_MASK;
		bitmask_inv = ADIN1320_LED_C_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_3:
		bitmask_cfg = ADIN1320_LED_3_CFG_MASK;
		bitmask_ext_cfg = ADIN1320_LED_3_EXT_CFG_EN_MASK;
		bitmask_inv = ADIN1320_LED_D_INV_EN_MASK;
		break;

	default:
		return -EINVAL;
	}

	bitmask = ADIN1320_LED_PAT_PAUSE_DUR_MASK;
	bit_value = no_os_field_get(ADIN1320_LED_EXT_CFG_EN_VAL, led_cfg);
	write_bits = no_os_field_prep(bitmask_ext_cfg, bit_value) |
		     no_os_field_prep(bitmask, led_pause);
	ret = adin1320_write_bits(dev, ADIN1320_LED_CTRL_1, write_bits,
				  bitmask_ext_cfg | bitmask);
	if (ret)
		return ret;

	bit_value = no_os_field_get(ADIN1320_LED_CFG_VAL, led_cfg);
	write_bits = no_os_field_prep(bitmask_cfg, bit_value);
	ret = adin1320_write_bits(dev, ADIN1320_LED_CTRL_2, write_bits,
				  bitmask_cfg);
	if (ret)
		return ret;

	bitmask = ADIN1320_LED_PAT_SEL_MASK | ADIN1320_LED_PAT_MASK;
	write_bits = no_os_field_prep(ADIN1320_LED_PAT_SEL_MASK, led_sel) |
		     no_os_field_prep(ADIN1320_LED_PAT_MASK, led_pat);
	ret = adin1320_write_bits(dev, ADIN1320_LED_CTRL_3, write_bits,
				  bitmask);
	if (ret)
		return ret;

	write_bits = no_os_field_prep(bitmask_inv, led_inv_state);

	return adin1320_write_bits(dev, ADIN1320_GE_PHY_LED_CFG, write_bits,
				   bitmask_inv);
}

/**
 * @brief Configure LED settings for SerDes Media
 * @param dev - The device structure
 * @param led_sel - LED to configure
 * @param led_cfg - LED configuration
 * @param led_inv_state - LED invert state
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_sd_led_ctrl(struct adin1320_desc *dev,
			 enum adin1320_led_sel led_sel,
			 enum adin1320_sd_led_cfg led_cfg,
			 enum adin1320_led_inv_state led_inv_state)
{
	int ret;
	uint32_t write_reg;
	uint16_t bitmask_cfg;
	uint16_t bitmask_inv;
	uint16_t write_bits;

	if ((led_sel <= ADIN1320_LED_SEL_MIN) ||
	    (led_sel >= ADIN1320_LED_SEL_MAX))
		return -EINVAL;

	if ((led_cfg <= ADIN1320_SD_LED_CFG_MIN) ||
	    (led_cfg >= ADIN1320_SD_LED_CFG_MAX))
		return -EINVAL;

	if ((led_inv_state != ADIN1320_LED_INV_STATE_ACTIVE_HIGH) &&
	    (led_inv_state != ADIN1320_LED_INV_STATE_ACTIVE_LOW))
		return -EINVAL;

	switch (led_sel) {
	case ADIN1320_LED_SEL_0:
		write_reg = ADIN1320_SD_FIB_LED_CFG_0;
		bitmask_cfg = ADIN1320_SD_FIB_LED_CFG_0_MASK;
		bitmask_inv = ADIN1320_LED_A_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_1:
		write_reg = ADIN1320_SD_FIB_LED_CFG_1;
		bitmask_cfg = ADIN1320_SD_FIB_LED_CFG_1_MASK;
		bitmask_inv = ADIN1320_LED_B_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_2:
		write_reg = ADIN1320_SD_FIB_LED_CFG_2;
		bitmask_cfg = ADIN1320_SD_FIB_LED_CFG_2_MASK;
		bitmask_inv = ADIN1320_LED_C_INV_EN_MASK;
		break;

	case ADIN1320_LED_SEL_3:
		write_reg = ADIN1320_SD_FIB_LED_CFG_3;
		bitmask_cfg = ADIN1320_SD_FIB_LED_CFG_3_MASK;
		bitmask_inv = ADIN1320_LED_D_INV_EN_MASK;
		break;

	default:
		return -EINVAL;
	}

	write_bits = no_os_field_prep(bitmask_cfg, led_cfg);
	ret = adin1320_write_bits(dev, write_reg, write_bits, bitmask_cfg);
	if (ret)
		return ret;

	write_bits = no_os_field_prep(bitmask_inv, led_inv_state);

	return adin1320_write_bits(dev, ADIN1320_GE_PHY_LED_CFG, write_bits,
				   bitmask_inv);
}

/**
 * @brief Configure Magic Packet Address Matchings
 * @param dev - The device structure
 * @param enabled_matches - Magic packet address matchings to enable
 * 		 (masked with adin1320_mgc_match_en)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_mgc_match_cfg(struct adin1320_desc *dev, uint16_t enabled_matches)
{
	uint16_t bitmask_match = ADIN1320_GE_MGC_0_DA_BC_AD_EN_MASK |
				 ADIN1320_GE_MGC_0_DA_MC_AD_EN_MASK |
				 ADIN1320_GE_MGC_0_DA_UC_AD_EN_MASK |
				 ADIN1320_GE_MGC_0_DA_STA_0_AD_EN_MASK;
	uint16_t write_bits = ADIN1320_BITMASK_RESET;
	uint16_t bitmask = bitmask_match | ADIN1320_GE_MGC_0_EN_MASK;

	if (enabled_matches >= ADIN1320_MGC_MATCH_EN_MAX)
		return -EINVAL;

	write_bits |= no_os_field_prep(bitmask_match, enabled_matches);

	if (enabled_matches)
		write_bits |= ADIN1320_GE_MGC_0_EN_MASK;

	return adin1320_write_bits(dev, ADIN1320_GE_MGC_0_CNTRL, write_bits,
				   bitmask);
}

/**
 * @brief Configure Magic Packet Checks
 * @param dev - The device structure
 * @param enabled_checks - Magic packet checks to enable
 * 		 (masked with adin1320_mgc_chk_en)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_mgc_check_cfg(struct adin1320_desc *dev, uint16_t enabled_checks)
{
	uint16_t write_bits = ADIN1320_BITMASK_RESET;
	uint16_t bitmask = ADIN1320_GE_MGC_0_KEY_CHK_EN_MASK |
			   ADIN1320_GE_MGC_0_LEN_CHK_EN_MASK |
			   ADIN1320_GE_MGC_0_CRC_CHK_EN_MASK;

	if (enabled_checks >= ADIN1320_MGC_CHK_EN_MAX)
		return -EINVAL;

	write_bits |= no_os_field_prep(bitmask, enabled_checks);

	return adin1320_write_bits(dev, ADIN1320_GE_MGC_0_CNTRL, write_bits,
				   bitmask);
}

/**
 * @brief Configure Magic Packet SecureOn Key
 * @param dev - The device structure
 * @param key_byte - Select 6 or 4 byte key
 * @param key - Key bytes (array of 6 bytes, only first 4 used for 4-byte key)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_mgc_key_cfg(struct adin1320_desc *dev,
			 enum adin1320_mgc_key_byte key_byte,
			 uint8_t key[6])
{
	int ret;
	uint16_t write_key;
	uint16_t write_4byte_en;

	if ((key_byte != ADIN1320_MGC_KEY_BYTE_4) &&
	    (key_byte != ADIN1320_MGC_KEY_BYTE_6))
		return -EINVAL;

	/* Set SecureOn key byte 0-1 */
	write_key = no_os_field_prep(ADIN1320_GE_MGC_0_KEY_0_MASK, key[0]) |
		    no_os_field_prep(ADIN1320_GE_MGC_0_KEY_1_MASK, key[1]);
	ret = adin1320_write(dev, ADIN1320_GE_MGC_0_KEY_01, write_key);
	if (ret)
		return ret;

	/* Set SecureOn key byte 2-3 */
	write_key = no_os_field_prep(ADIN1320_GE_MGC_0_KEY_2_MASK, key[2]) |
		    no_os_field_prep(ADIN1320_GE_MGC_0_KEY_3_MASK, key[3]);
	ret = adin1320_write(dev, ADIN1320_GE_MGC_0_KEY_23, write_key);
	if (ret)
		return ret;

	if (key_byte == ADIN1320_MGC_KEY_BYTE_6) {
		/* Set SecureOn key byte 4-5 */
		write_key = no_os_field_prep(ADIN1320_GE_MGC_0_KEY_4_MASK,
					     key[4]) |
			    no_os_field_prep(ADIN1320_GE_MGC_0_KEY_5_MASK,
					     key[5]);
		ret = adin1320_write(dev, ADIN1320_GE_MGC_0_KEY_45, write_key);
		if (ret)
			return ret;

		write_4byte_en = ADIN1320_BITMASK_RESET;
	} else {
		write_4byte_en = ADIN1320_GE_MGC_0_KEY_4BY_EN_MASK;
	}

	return adin1320_write_bits(dev, ADIN1320_GE_MGC_0_CNTRL, write_4byte_en,
				   ADIN1320_GE_MGC_0_KEY_4BY_EN_MASK);
}

/**
 * @brief Configure Wake-on-LAN Station Address
 * @param dev - The device structure
 * @param mac_address - Station MAC address (6 bytes)
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_wol_address(struct adin1320_desc *dev,
			 uint8_t mac_address[6])
{
	int ret;
	uint16_t write_val;

	/* Set station MAC address byte 0-1 */
	write_val = no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_0_MASK, mac_address[0]) |
		    no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_1_MASK, mac_address[1]);
	ret = adin1320_write(dev, ADIN1320_GE_WOL_STA_0_AD_01, write_val);
	if (ret)
		return ret;

	/* Set station MAC address byte 2-3 */
	write_val = no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_2_MASK, mac_address[2]) |
		    no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_3_MASK, mac_address[3]);
	ret = adin1320_write(dev, ADIN1320_GE_WOL_STA_0_AD_23, write_val);
	if (ret)
		return ret;

	/* Set station MAC address byte 4-5 */
	write_val = no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_4_MASK, mac_address[4]) |
		    no_os_field_prep(ADIN1320_GE_WOL_STA_0_AD_5_MASK, mac_address[5]);
	return adin1320_write(dev, ADIN1320_GE_WOL_STA_0_AD_45, write_val);
}

/**
 * @brief Configure Wake-on-LAN System Settings
 * @param dev - The device structure
 * @param enable_fi - Enable/disable wake-on-LAN detection from serdes interface
 * @param active_signal - Set active signal polarity
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_wol_sys_cfg(struct adin1320_desc *dev,
			 enum adin1320_wol_fi_en enable_fi,
			 enum adin1320_wol_inv active_signal)
{
	uint16_t write_bits = ADIN1320_BITMASK_RESET;
	uint16_t bitmask = ADIN1320_GE_WOL_WAKE_INV_EN_MASK |
			   ADIN1320_GE_WOL_2_PHY_SD_SEL_MASK;

	if ((enable_fi != ADIN1320_WOL_FI_DIS) &&
	    (enable_fi != ADIN1320_WOL_FI_EN))
		return -EINVAL;

	if ((active_signal != ADIN1320_WOL_INV_ACTIVE_HIGH) &&
	    (active_signal != ADIN1320_WOL_INV_ACTIVE_LOW))
		return -EINVAL;

	write_bits = no_os_field_prep(ADIN1320_GE_WOL_WAKE_INV_EN_MASK,
				      active_signal) |
		     no_os_field_prep(ADIN1320_GE_WOL_2_PHY_SD_SEL_MASK,
				      enable_fi);

	return adin1320_write_bits(dev, ADIN1320_GE_WOL_SYS_CNTRL, write_bits,
				   bitmask);
}

/**
 * @brief Configure Wake-on-LAN Signal Settings
 * @param dev - The device structure
 * @param signal_type - Select the type of signal for wake-up
 * @param pulse_length - Pulse length duration selection
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_wol_sig_cfg(struct adin1320_desc *dev,
			 enum adin1320_wol_sig signal_type,
			 uint8_t pulse_length)
{
	uint16_t write_bits = ADIN1320_BITMASK_RESET;
	uint16_t bitmask = ADIN1320_GE_WOL_PUL_LENM_1_MASK |
			   ADIN1320_GE_WOL_PUL_EN_MASK;

	if ((signal_type != ADIN1320_WOL_SIG_LEVEL) &&
	    (signal_type != ADIN1320_WOL_SIG_PULSE))
		return -EINVAL;

	if (pulse_length >= ADIN1320_WOL_SIG_CFG_PUL_LEN_MAX)
		return -EINVAL;

	write_bits = no_os_field_prep(ADIN1320_GE_WOL_PUL_EN_MASK,
				      signal_type) |
		     no_os_field_prep(ADIN1320_GE_WOL_PUL_LENM_1_MASK,
				      pulse_length);

	return adin1320_write_bits(dev, ADIN1320_GE_WOL_SIG_CNTRL, write_bits,
				   bitmask);
}

/**
 * @brief Configure Wake-on-LAN Wake-up Events
 * @param dev - The device structure
 * @param wake_on_key - Enable/disable wake on Magic Packet key error
 * @param wake_on_link_change - Enable/disable wake on link status change
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_wol_wake_cfg(struct adin1320_desc *dev,
			  enum adin1320_wol_wake_key_enable wake_on_key,
			  enum adin1320_wol_wake_link_enable wake_on_link_change)
{
	uint16_t write_bits = ADIN1320_BITMASK_RESET;
	uint16_t bitmask = ADIN1320_GE_WOL_KEY_ERR_WAKE_EN_MASK |
			   ADIN1320_GE_WOL_LS_CHG_WAKE_EN_MASK;

	if ((wake_on_key != ADIN1320_WOL_WAKE_KEY_DISABLE) &&
	    (wake_on_key != ADIN1320_WOL_WAKE_KEY_ENABLE))
		return -EINVAL;

	if ((wake_on_link_change != ADIN1320_WOL_WAKE_LINK_DISABLE) &&
	    (wake_on_link_change != ADIN1320_WOL_WAKE_LINK_ENABLE))
		return -EINVAL;

	write_bits = no_os_field_prep(ADIN1320_GE_WOL_KEY_ERR_WAKE_EN_MASK,
				      wake_on_key) |
		     no_os_field_prep(ADIN1320_GE_WOL_LS_CHG_WAKE_EN_MASK,
				      wake_on_link_change);

	return adin1320_write_bits(dev, ADIN1320_GE_WOL_WAKE_CNTRL, write_bits,
				   bitmask);
}

/**
 * @brief Enable/Disable Wake-on-LAN
 * @param dev - The device structure
 * @param enable - Enable/disable Wake-on-LAN
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_wol_en_cfg(struct adin1320_desc *dev,
			enum adin1320_wol_enable enable)
{
	if ((enable != ADIN1320_WOL_DISABLE) &&
	    (enable != ADIN1320_WOL_ENABLE))
		return -EINVAL;

	return adin1320_write_bits(dev, ADIN1320_GE_WOL_EN,
				   enable ? ADIN1320_GE_WOL_EN_MASK :
				   ADIN1320_BITMASK_RESET,
				   ADIN1320_GE_WOL_EN_MASK);
}

/**
 * @brief Get Auto Media Selected as Active Media
 * @param dev - The device structure
 * @param media - Active media selected
 * @return 0 in case of success, negative code otherwise
 */
int adin1320_get_active_media(struct adin1320_desc *dev,
			      enum adin1320_active_media *media)
{
	int ret;
	uint16_t data;

	if (!media)
		return -EINVAL;

	ret = adin1320_read(dev, ADIN1320_GE_MSEL_AUTO_STAT, &data);
	if (ret)
		return ret;

	if (no_os_field_get(ADIN1320_GE_MSEL_AUTO_SD_MASK, data))
		*media = ADIN1320_ACTIVE_MEDIA_SD;
	else if (no_os_field_get(ADIN1320_GE_MSEL_AUTO_GE_MASK, data))
		*media = ADIN1320_ACTIVE_MEDIA_CU;
	else
		*media = ADIN1320_ACTIVE_MEDIA_NULL;

	return 0;
}
