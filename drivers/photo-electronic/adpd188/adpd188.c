/***************************************************************************//**
 *   @file   adpd188.c
 *   @brief  ADPD188 driver implementation.
 *   @author Andrei Drimbarean (Andrei.Drimbarean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include "adpd188.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the ADPD188 driver.
 * @param device - The ADPD188 descriptor.
 * @param init_param - The structure that contains the ADPD188 initialization
 *                     parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_init(struct adpd188_dev **device,
		     struct adpd188_init_param *init_param)
{
	int32_t ret;
	struct adpd188_dev *dev;
	uint16_t reg_data;

	dev = (struct adpd188_dev *)no_os_calloc(1, sizeof (*dev));
	if(!dev)
		return -1;
	dev->device = init_param->device;
	if (dev->device == ADPD1080)
		dev->phy_opt = ADPD188_I2C;
	else if (dev->device == ADPD1081)
		dev->phy_opt = ADPD188_SPI;
	else
		dev->phy_opt = init_param->phy_opt;

	if(dev->phy_opt == ADPD188_SPI)
		ret = no_os_spi_init((struct no_os_spi_desc **)&dev->phy_desc,
				     (const struct no_os_spi_init_param *)&init_param->phy_init);
	else if(dev->phy_opt == ADPD188_I2C)
		ret = no_os_i2c_init((struct no_os_i2c_desc **)&dev->phy_desc,
				     (const struct no_os_i2c_init_param *)&init_param->phy_init);
	else
		ret = -1;
	if(ret != 0)
		goto error_dev;

	ret = adpd188_reg_read(dev, ADPD188_REG_DEVID, &reg_data);
	if(ret != 0)
		goto error_phy;
	if((dev->device == ADPD188BI) && (reg_data != ADPD188_DEVICE_ID))
		goto error_phy;
	else if(((dev->device == ADPD1080) || (dev->device == ADPD1081)) &&
		(reg_data != ADPD108X_DEVICE_ID))
		goto error_phy;

	ret = adpd188_sw_reset(dev);
	if(ret != 0)
		goto error_phy;

	ret = no_os_gpio_get(&dev->gpio0, &init_param->gpio0_init);
	if(ret != 0)
		goto error_phy;
	ret = no_os_gpio_get(&dev->gpio1, &init_param->gpio1_init);
	if(ret != 0)
		goto error_gpio0;

	ret = no_os_gpio_direction_input(dev->gpio0);
	if(ret != 0)
		goto error_gpio1;
	ret = no_os_gpio_direction_input(dev->gpio1);
	if(ret != 0)
		goto error_gpio1;

	*device = dev;

	return 0;

error_gpio1:
	no_os_gpio_remove(dev->gpio1);
error_gpio0:
	no_os_gpio_remove(dev->gpio0);
error_phy:
	if(dev->phy_opt == ADPD188_SPI)
		no_os_spi_remove(dev->phy_desc);
	else if(dev->phy_opt == ADPD188_I2C)
		no_os_i2c_remove(dev->phy_desc);
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * @brief Free resources allocated by adpd188_init().
 * @param dev - The ADPD188 descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_remove(struct adpd188_dev *dev)
{
	int32_t ret;

	if(dev->phy_opt == ADPD188_SPI)
		ret = no_os_spi_remove(dev->phy_desc);
	else if(dev->phy_opt == ADPD188_I2C)
		ret = no_os_i2c_remove(dev->phy_desc);
	else
		ret = -1;
	if(ret != 0)
		return -1;

	ret = no_os_gpio_remove(dev->gpio0);
	if(ret != 0)
		return -1;
	ret = no_os_gpio_remove(dev->gpio1);
	if(ret != 0)
		return -1;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Read one 16 bit register of the ADPD188.
 * @param dev - The ADPD188 descriptor.
 * @param reg_addr - The register address.
 * @param reg_val - Value of the read register.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_reg_read(struct adpd188_dev *dev, uint8_t reg_addr,
			 uint16_t *reg_val)
{
	int32_t ret;
	uint8_t buff[] = {0, 0, 0};

	if(dev->phy_opt == ADPD188_SPI) {
		buff[0] = (reg_addr << 1) & 0xFE;
		ret = no_os_spi_write_and_read(dev->phy_desc, buff, 3);
	} else if(dev->phy_opt == ADPD188_I2C) {
		ret = no_os_i2c_write(dev->phy_desc, &reg_addr, 1, 0);
		if(ret != 0)
			return -1;
		/**
		 *  Store read values starting with the second place in the buffer to
		 *  have the value in the same spaces as in the SPI case. This way the
		 *  register value can be compiled only once outside the if clause.
		 */
		ret = no_os_i2c_read(dev->phy_desc, (buff + 1), 2, 1);
	} else {
		ret = -1;
	}
	if(ret != 0)
		return -1;

	*reg_val = (buff[1] << 8) & 0xFF00;
	*reg_val |= buff[2];

	return 0;
}

/**
 * @brief Write one 16 bit register of the ADPD188.
 * @param dev - The ADPD188 descriptor.
 * @param reg_addr - The register address.
 * @param reg_val - The new value of the register.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_reg_write(struct adpd188_dev *dev, uint8_t reg_addr,
			  uint16_t reg_val)
{
	int32_t ret;
	uint8_t buff[] = {0, 0, 0};

	buff[1] = (reg_val & 0xFF00) >> 8;
	buff[2] = reg_val & 0x00FF;

	if(dev->phy_opt == ADPD188_SPI) {
		buff[0] = (reg_addr << 1) | 1;
		ret = no_os_spi_write_and_read(dev->phy_desc, buff, 3);
	} else if(dev->phy_opt == ADPD188_I2C) {
		buff[0] = reg_addr;
		ret = no_os_i2c_write(dev->phy_desc, buff, 3, 1);
	} else {
		ret = -1;
	}
	if(ret != 0)
		return -1;

	return 0;
}

/**
 * @brief Get the mode of operation of the ADPD188.
 * @param dev - The ADPD188 descriptor.
 * @param mode - The cur mode of operation. Values can be:
 *                   - ADPD188_STANDBY;
 *                   - ADPD188_PROGRAM;
 *                   - ADPD188_NORMAL.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_mode_get(struct adpd188_dev *dev, enum adpd188_mode *mode)
{
	int32_t ret;
	uint16_t data;

	ret = adpd188_reg_read(dev, ADPD188_REG_MODE, &data);
	if(ret != 0)
		return -1;

	*mode = data & ADPD188_MODE_MODE_MASK;

	return 0;
}

/**
 * @brief Set the mode of operation of the ADPD188.
 * @param dev - The ADPD188 descriptor.
 * @param new_mode - The new mode of operation. Values can be:
 *                   - ADPD188_STANDBY;
 *                   - ADPD188_PROGRAM;
 *                   - ADPD188_NORMAL.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_mode_set(struct adpd188_dev *dev, enum adpd188_mode new_mode)
{
	uint16_t data;

	data = new_mode & ADPD188_MODE_MODE_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_MODE, data);
}

/**
 * @brief Get the number of bytes currently present in FIFO.
 * @param dev - The ADPD188 descriptor.
 * @param bytes_no - Number of bytes in the FIFO.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_fifo_status_get(struct adpd188_dev *dev, uint8_t *bytes_no)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_STATUS, &reg_data);
	if(ret != 0)
		return -1;

	*bytes_no = (reg_data & ADPD188_STATUS_FIFO_SAMPLES_MASK) >>
		    ADPD188_STATUS_FIFO_SAMPLES_POS;

	return 0;
}

/**
 * @brief Empty the FIFO.
 * @param dev - The ADPD188 descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_fifo_clear(struct adpd188_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_STATUS, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= 0x8000;
	/* Write 0 to the interrupt flags to not clear them unintentionally. */
	reg_data &= ~(ADPD188_STATUS_SLOTA_INT_MASK |
		      ADPD188_STATUS_SLOTB_INT_MASK);

	return adpd188_reg_write(dev, ADPD188_REG_STATUS, reg_data);
}

/**
 * @brief Set the number of 16 bit words that need to be in the FIFO to trigger
 *        an interrupt.
 * @param dev - The ADPD188 descriptor.
 * @param word_no - Number of words that trigger an interrupt.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_fifo_thresh_set(struct adpd188_dev *dev, uint8_t word_no)
{
	int32_t ret;
	uint16_t reg_data;

	if(word_no > ADPD188_FIFO_THRESH_MAX_THRESHOLD)
		return -1;

	ret = adpd188_reg_read(dev, ADPD188_REG_FIFO_THRESH, &reg_data);
	if(ret != 0)
		return -1;

	reg_data &= ~ADPD188_FIFO_THRESH_FIFO_THRESH_MASK;
	reg_data |= (word_no << ADPD188_FIFO_THRESH_FIFO_THRESH_POS) &
		    ADPD188_FIFO_THRESH_FIFO_THRESH_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_FIFO_THRESH, reg_data);
}

/**
 * @brief Get the slot and FIFO interrupt flags.
 * @param dev - The ADPD188 descriptor.
 * @param flags - The ORed value of the flags. If no interrupt has triggered the
 *                value will be zero. Otherwise it can be a logical OR between:
 *                - ADPD188_SLOTA_INT;
 *                - ADPD188_SLOTB_INT;
 *                - ADPD188_FIFO_INT.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_interrupt_get(struct adpd188_dev *dev, uint8_t *flags)
{
	int32_t ret;
	uint16_t reg_data;

	*flags = 0;

	ret = adpd188_reg_read(dev, ADPD188_REG_STATUS, &reg_data);
	if(ret != 0)
		return -1;
	if(reg_data & ADPD188_STATUS_SLOTA_INT_MASK)
		*flags |= ADPD188_SLOTA_INT;
	if(reg_data & ADPD188_STATUS_SLOTB_INT_MASK)
		*flags |= ADPD188_SLOTB_INT;

	return 0;
}

/**
 * @brief Clear the slot and FIFO interrupt flags.
 * @param dev - The ADPD188 descriptor.
 * @param flags - The ORed value of the flags that need to be cleared. If no
 *                interrupt needs to be cleared the value will be zero.
 *                Otherwise do a logical OR between the flags that need to be
 *                cleared:
 *                - ADPD188_SLOTA_INT;
 *                - ADPD188_SLOTB_INT;
 *                - ADPD188_FIFO_INT.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_interrupt_clear(struct adpd188_dev *dev, uint8_t flags)
{
	int32_t ret;
	uint16_t reg_data;

	if(!flags)
		return 0;

	ret = adpd188_reg_read(dev, ADPD188_REG_STATUS, &reg_data);
	if(ret != 0)
		return -1;

	/*
	 * If an interrupt is not to be cleared, but happens to be asserted write 0
	 * to that spot to not clear it unintentionally.
	 */
	if(flags & ADPD188_SLOTA_INT)
		reg_data |= ADPD188_STATUS_SLOTA_INT_MASK;
	else
		reg_data &= ~ADPD188_STATUS_SLOTA_INT_MASK;
	if(flags & ADPD188_SLOTB_INT)
		reg_data |= ADPD188_STATUS_SLOTB_INT_MASK;
	else
		reg_data &= ~ADPD188_STATUS_SLOTB_INT_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_STATUS, reg_data);
}

/**
 * @brief Enable the slot and FIFO interrupt flags.
 * @param dev - The ADPD188 descriptor.
 * @param flags - The ORed value of the flags that need to be enabled. If no
 *                interrupt needs to be enabled the value will be zero.
 *                Otherwise do a logical OR between the flags that need to be
 *                enabled:
 *                - ADPD188_SLOTA_INT;
 *                - ADPD188_SLOTB_INT;
 *                - ADPD188_FIFO_INT.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_interrupt_en(struct adpd188_dev *dev, uint8_t flags)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_INT_MASK, &reg_data);
	if(ret != 0)
		return -1;
	if(flags & ADPD188_SLOTA_INT)
		reg_data &= ~ADPD188_INT_MASK_SLOTA_INT_MASK_MASK;
	if(flags & ADPD188_SLOTB_INT)
		reg_data &= ~ADPD188_INT_MASK_SLOTB_INT_MASK_MASK;
	if(flags & ADPD188_FIFO_INT)
		reg_data &= ~ADPD188_INT_MASK_FIFO_INT_MASK_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_INT_MASK, reg_data);
}

/**
 * @brief Setup drive and polarity of the GPIOs. Also enable GPIO if necessary.
 * @param dev - The ADPD188 descriptor.
 * @param config - Configuration structure of the GPIO.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_gpio_setup(struct adpd188_dev *dev,
			   struct adpd188_gpio_config config)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_GPIO_DRV, &reg_data);
	if(ret != 0)
		return -1;
	if(config.gpio_id == 0) {
		if(config.gpio_pol)
			reg_data |= ADPD188_GPIO_DRV_GPIO0_POL_MASK;
		else
			reg_data &= ~ADPD188_GPIO_DRV_GPIO0_POL_MASK;
		if(config.gpio_drv)
			reg_data |= ADPD188_GPIO_DRV_GPIO0_DRV_MASK;
		else
			reg_data &= ~ADPD188_GPIO_DRV_GPIO0_DRV_MASK;
		if(config.gpio_en)
			reg_data |= ADPD188_GPIO_DRV_GPIO0_ENA_MASK;
		else
			reg_data &= ~ADPD188_GPIO_DRV_GPIO0_ENA_MASK;
	} else if(config.gpio_id == 1) {
		if(config.gpio_pol)
			reg_data |= ADPD188_GPIO_DRV_GPIO1_POL_MASK;
		else
			reg_data &= ~ADPD188_GPIO_DRV_GPIO1_POL_MASK;
		if(config.gpio_drv)
			reg_data |= ADPD188_GPIO_DRV_GPIO1_DRV_MASK;
		else
			reg_data &= ~ADPD188_GPIO_DRV_GPIO1_DRV_MASK;
	}

	return adpd188_reg_write(dev, ADPD188_REG_GPIO_DRV, reg_data);
}

/**
 * @brief Setup the GPIO source.
 * @param dev - The ADPD188 descriptor.
 * @param gpio_id - ID of the GPIO (0 or 1).
 * @param config - ID of the source of the GPIO.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_gpio_alt_setup(struct adpd188_dev *dev, uint8_t gpio_id,
			       enum adpd188_gpio_alt_config config)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_GPIO_CTRL, &reg_data);
	if(ret != 0)
		return -1;

	if(gpio_id == 0) {
		reg_data &= ~ADPD188_GPIO_CTRL_GPIO0_ALT_CFG_MASK;
		reg_data |= (config << ADPD188_GPIO_CTRL_GPIO0_ALT_CFG_POS) &
			    ADPD188_GPIO_CTRL_GPIO0_ALT_CFG_MASK;
	} else if(gpio_id == 1) {
		reg_data &= ~ADPD188_GPIO_CTRL_GPIO1_ALT_CFG_MASK;
		reg_data |= (config << ADPD188_GPIO_CTRL_GPIO1_ALT_CFG_POS) &
			    ADPD188_GPIO_CTRL_GPIO1_ALT_CFG_MASK;
	} else {
		return -1;
	}

	return adpd188_reg_write(dev, ADPD188_REG_GPIO_CTRL, reg_data);
}

/**
 * @brief Do software reset of the device.
 * @param dev - The ADPD188 descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_sw_reset(struct adpd188_dev *dev)
{
	return adpd188_reg_write(dev, ADPD188_REG_SW_RESET, 0x1);
}

/**
 * @brief Do internal 32MHz clock calibration. This calibration requires the
 *        32kHz clock to be calibrated first. The 32kHz calibration needs an
 *        external reference.
 * @param dev - The ADPD188 descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_clk32mhz_cal(struct adpd188_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;
	float clk_error;

	ret = adpd188_reg_read(dev, ADPD188_REG_DATA_ACCESS_CTL, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_DATA_ACCESS_CTL, reg_data);
	if(ret != 0)
		return -1;

	ret = adpd188_reg_read(dev, ADPD188_REG_CLK32M_CAL_EN, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= ADPD188_CLK32M_CAL_EN_CLK32M_CAL_EN_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_CLK32M_CAL_EN, reg_data);
	if(ret != 0)
		return -1;

	no_os_mdelay(1);
	ret = adpd188_reg_read(dev, ADPD188_REG_CLK_RATIO, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ADPD188_CLK_RATIO_CLK_RATIO_MASK;

	clk_error = 32000000.0 * (1.0 - (float)reg_data/2000.0);
	if (dev->device == ADPD188BI)
		reg_data = clk_error / 109000;
	else
		reg_data = clk_error / 112000;
	reg_data &= ADPD188_CLK32M_ADJUST_CLK32M_ADJUST_MASK;

	ret = adpd188_reg_write(dev, ADPD188_REG_CLK32M_ADJUST, reg_data);
	if(ret != 0)
		return -1;

	ret = adpd188_reg_read(dev, ADPD188_REG_CLK32M_CAL_EN, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_CLK32M_CAL_EN_CLK32M_CAL_EN_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_CLK32M_CAL_EN, reg_data);
	if(ret != 0)
		return -1;

	ret = adpd188_reg_read(dev, ADPD188_REG_DATA_ACCESS_CTL, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_DATA_ACCESS_CTL, reg_data);
}

/**
 * @brief Enable slot and setup its FIFO interaction.
 * @param dev - The ADPD188 descriptor.
 * @param config - Configuration structure for the slot.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_slot_setup(struct adpd188_dev *dev,
			   struct adpd188_slot_config config)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_SLOT_EN, &reg_data);
	if(ret != 0)
		return -1;

	if(config.slot_id == ADPD188_SLOTA) {
		reg_data &= ~ADPD188_SLOT_EN_SLOTA_EN_MASK;
		reg_data |= (config.slot_en << ADPD188_SLOT_EN_SLOTA_EN_POS) &
			    ADPD188_SLOT_EN_SLOTA_EN_MASK;
		reg_data &= ~ADPD188_SLOT_EN_SLOTA_FIFO_MODE_MASK;
		reg_data |= (config.sot_fifo_mode <<
			     ADPD188_SLOT_EN_SLOTA_FIFO_MODE_POS) &
			    ADPD188_SLOT_EN_SLOTA_FIFO_MODE_MASK;
	} else if(config.slot_id == ADPD188_SLOTB) {
		reg_data &= ~ADPD188_SLOT_EN_SLOTB_EN_MASK;
		reg_data |= (config.slot_en << ADPD188_SLOT_EN_SLOTB_EN_POS) &
			    ADPD188_SLOT_EN_SLOTB_EN_MASK;
		reg_data &= ~ADPD188_SLOT_EN_SLOTB_FIFO_MODE_MASK;
		reg_data |= (config.sot_fifo_mode <<
			     ADPD188_SLOT_EN_SLOTB_FIFO_MODE_POS) &
			    ADPD188_SLOT_EN_SLOTB_FIFO_MODE_MASK;
	}

	return adpd188_reg_write(dev, ADPD188_REG_SLOT_EN, reg_data);
}

/**
 * @brief Set sample frequency of the ADC.
 * @param dev - The ADPD188 descriptor.
 * @param freq_hz - Desired ADC sample frequency.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_adc_fsample_set(struct adpd188_dev *dev, uint16_t freq_hz)
{
	uint16_t reg_data;

	if(freq_hz > 2000)
		return -1;

	reg_data = 32000 / (freq_hz * 4);

	return adpd188_reg_write(dev, ADPD188_REG_FSAMPLE, reg_data);
}

/**
 * @brief Get sample frequency of the ADC.
 * @param dev - The ADPD188 descriptor.
 * @param freq_hz - ADC sample frequency.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_adc_fsample_get(struct adpd188_dev *dev, uint16_t *freq_hz)
{
	int32_t ret;
	uint16_t reg_data;

	ret = adpd188_reg_read(dev, ADPD188_REG_FSAMPLE, &reg_data);
	if(ret != 0)
		return -1;
	*freq_hz = 32000 / (reg_data * 4);

	return 0;
}

/**
 * @brief Do initial configuration of the device to use as a smoke detector. The
 *        configuration is described in the datasheet.
 * @param dev - The ADPD188 descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t adpd188_smoke_detect_setup(struct adpd188_dev *dev)
{
	int32_t ret;
	uint16_t reg_data;
	struct adpd188_slot_config slota_conf, slotb_conf;

	ret = adpd188_reg_read(dev, ADPD188_REG_SLOT_EN, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= ADPD188_SLOT_EN_RDOUT_MODE_MASK |
		    ADPD188_SLOT_EN_FIFO_OVRN_PREVENT_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOT_EN, reg_data);
	if(ret != 0)
		return -1;
	slota_conf.slot_en = true;
	slota_conf.slot_id = ADPD188_SLOTA;
	slota_conf.sot_fifo_mode = ADPD188_32BIT_SUM;
	ret = adpd188_slot_setup(dev, slota_conf);
	if(ret != 0)
		return -1;
	slotb_conf.slot_en = true;
	slotb_conf.slot_id = ADPD188_SLOTB;
	slotb_conf.sot_fifo_mode = ADPD188_32BIT_SUM;
	ret = adpd188_slot_setup(dev, slotb_conf);
	if(ret != 0)
		return -1;

	ret = adpd188_adc_fsample_set(dev, 16.0);
	if(ret != 0)
		return -1;

	ret = adpd188_reg_read(dev, ADPD188_REG_PD_LED_SELECT, &reg_data);
	if(ret != 0)
		return -1;
	/* Blue LED in slot A */
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_MASK;
	/* IR LED in slot A */
	reg_data |= (3 << ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_MASK;
	/* Combine PDs for both slots */
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK;
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_PD_LED_SELECT, reg_data);
	if(ret != 0)
		return -1;

	/* No decimation for any slot */
	ret = adpd188_reg_write(dev, ADPD188_REG_NUM_AVG, 0);
	if(ret != 0)
		return -1;

	/* Slot A chop mode is inverted, non-inverted, non-inverted, inverted */
	ret = adpd188_reg_read(dev, ADPD188_REG_INT_SEQ_A, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_A_INTEG_ORDER_A_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_INT_SEQ_A, reg_data);
	if(ret != 0)
		return -1;

	/* No ADC offset on channel 1, Slot A */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_CH1_OFFSET, 0);
	if(ret != 0)
		return -1;
	/* Unused channel 2, slot A */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_CH2_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;
	/* Unused channel 3, slot A */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_CH3_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;
	/* Unused channel 4, slot A */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_CH4_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;

	/* Slot B chop mode is inverted, non-inverted, non-inverted, inverted */
	ret = adpd188_reg_read(dev, ADPD188_REG_INT_SEQ_B, &reg_data);
	if(ret != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_B_INTEG_ORDER_B_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_INT_SEQ_B, reg_data);
	if(ret != 0)
		return -1;

	/* No ADC offset on channel 1, Slot B */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_CH1_OFFSET, 0);
	if(ret != 0)
		return -1;
	/* Unused channel 2, slot B */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_CH2_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;
	/* Unused channel 3, slot B */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_CH3_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;
	/* Unused channel 4, slot B */
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_CH4_OFFSET, 0x3FFF);
	if(ret != 0)
		return -1;

	/* Set IR LED 3 power */
	ret = adpd188_reg_read(dev, ADPD188_REG_ILED3_COARSE, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_ILED3_COARSE_ILED3_COARSE_MASK;
	reg_data |= (0x9 << ADPD188_ILED3_COARSE_ILED3_COARSE_POS) &
		    ADPD188_ILED3_COARSE_ILED3_COARSE_MASK;
	reg_data &= ~ADPD188_ILED3_COARSE_ILED3_SLEW_MASK;
	reg_data |= (0x3 << ADPD188_ILED3_COARSE_ILED3_SLEW_POS) &
		    ADPD188_ILED3_COARSE_ILED3_SLEW_MASK;
	reg_data |= ADPD188_ILED3_COARSE_ILED3_SCALE_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_ILED3_COARSE, reg_data);
	if(ret != 0)
		return -1;

	/* Set blue LED 1 power */
	ret = adpd188_reg_read(dev, ADPD188_REG_ILED1_COARSE, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_ILED1_COARSE_ILED1_COARSE_MASK;
	reg_data |= (0x6 << ADPD188_ILED1_COARSE_ILED1_COARSE_POS) &
		    ADPD188_ILED1_COARSE_ILED1_COARSE_MASK;
	reg_data &= ~ADPD188_ILED1_COARSE_ILED1_SLEW_MASK;
	reg_data |= (0x3 << ADPD188_ILED1_COARSE_ILED1_SLEW_POS) &
		    ADPD188_ILED1_COARSE_ILED1_SLEW_MASK;
	reg_data |= ADPD188_ILED1_COARSE_ILED1_SCALE_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_ILED1_COARSE, reg_data);
	if(ret != 0)
		return -1;

	/* Slot A 4 LED pulses with 15us period */
	ret = adpd188_reg_read(dev, ADPD188_REG_SLOTA_NUMPULSES, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_NUMPULSES, reg_data);
	if(ret != 0)
		return -1;

	/* Slot B 4 LED pulses with 15us period */
	ret = adpd188_reg_read(dev, ADPD188_REG_SLOTB_NUMPULSES, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_NUMPULSES, reg_data);
	if(ret != 0)
		return -1;

	/* Slot A integrator window */
	ret = adpd188_reg_read(dev, ADPD188_REG_SLOTA_AFE_WINDOW, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTA_AFE_WINDOW, reg_data);
	if(ret != 0)
		return -1;

	/* Slot B integrator window */
	ret = adpd188_reg_read(dev, ADPD188_REG_SLOTB_AFE_WINDOW, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_SLOTB_AFE_WINDOW, reg_data);
	if(ret != 0)
		return -1;

	/* Power down channels 2, 3 and 4 */
	ret = adpd188_reg_read(dev, ADPD188_REG_AFE_PWR_CFG1, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_MASK;
	reg_data |= (0x1C << ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_POS) &
		    ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_MASK;
	ret = adpd188_reg_write(dev, ADPD188_REG_AFE_PWR_CFG1, reg_data);
	if(ret != 0)
		return -1;

	/* Math for chop mode is inverted, non-inverted, non-inverted, inverted */
	ret = adpd188_reg_read(dev, ADPD188_REG_MATH, &reg_data);
	if(ret != 0)
		return -1;
	reg_data &= ~ADPD188_MATH_FLT_MATH34_B_MASK;
	reg_data |= (0x01 << ADPD188_MATH_FLT_MATH34_B_POS) &
		    ADPD188_MATH_FLT_MATH34_B_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH34_A_MASK;
	reg_data |= (0x01 << ADPD188_MATH_FLT_MATH34_A_POS) &
		    ADPD188_MATH_FLT_MATH34_A_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH12_B_MASK;
	reg_data |= (0x02 << ADPD188_MATH_FLT_MATH12_B_POS) &
		    ADPD188_MATH_FLT_MATH12_B_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH12_A_MASK;
	reg_data |= (0x02 << ADPD188_MATH_FLT_MATH12_A_POS) &
		    ADPD188_MATH_FLT_MATH12_A_MASK;

	return adpd188_reg_write(dev, ADPD188_REG_MATH, reg_data);
}
