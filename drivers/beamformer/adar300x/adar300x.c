/***************************************************************************//**
 *   @file   adar300x.c
 *   @brief  Implementation of ADAR3000/ADAR3001 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <errno.h>
#include <stddef.h>

#include "adar300x.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/*
 * product_id is recorded but never validated: the documented values are not
 * consistent across the family. CHIPTYPE is the reliable identity check.
 * An ADAR3000 reads PRODUCT_ID_L 0x01 / PRODUCT_ID_H 0xC0 and SPI_REV 0x01;
 * the other three entries are unverified against silicon.
 */
static const struct adar300x_chip_info adar300x_chip_info[] = {
	[ADAR3000] = { .name = "ADAR3000", .product_id = 0x01, .num_beams = 4, .num_elements = 4 },
	[ADAR3001] = { .name = "ADAR3001", .product_id = 0x01, .num_beams = 4, .num_elements = 4 },
	[ADAR3002] = { .name = "ADAR3002", .product_id = 0x01, .num_beams = 4, .num_elements = 4 },
	[ADAR3003] = { .name = "ADAR3003", .product_id = 0x00, .num_beams = 4, .num_elements = 4 },
};

/**
 * @brief Writes data to ADAR300X over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data value to write.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adar300x_reg_write(struct adar300x_dev *dev, uint16_t reg_addr,
		       uint8_t data)
{
	uint8_t buff[ADAR300X_BUFF_SIZE_BYTES];
	uint16_t cmd;

	if (!dev)
		return -EINVAL;

	cmd = ADAR300X_DEV_ADDR(dev->dev_addr) |
	      (reg_addr & ADAR300X_REG_ADDR_MSK);

	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(data);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = data;
	}

	return no_os_spi_write_and_read(dev->spi_desc, buff,
					ADAR300X_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ADAR300X over SPI.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adar300x_reg_read(struct adar300x_dev *dev, uint16_t reg_addr,
		      uint8_t *data)
{
	uint8_t buff[ADAR300X_BUFF_SIZE_BYTES];
	uint16_t cmd;
	int ret;

	if (!dev || !data)
		return -EINVAL;

	cmd = ADAR300X_SPI_READ_CMD | ADAR300X_DEV_ADDR(dev->dev_addr) |
	      (reg_addr & ADAR300X_REG_ADDR_MSK);

	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(cmd & 0xFF);
		buff[1] = no_os_bit_swap_constant_8(cmd >> 8);
		buff[2] = no_os_bit_swap_constant_8(ADAR300X_SPI_DUMMY_DATA);
	} else {
		buff[0] = cmd >> 8;
		buff[1] = cmd & 0xFF;
		buff[2] = ADAR300X_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff,
				       ADAR300X_BUFF_SIZE_BYTES);
	if (ret)
		return ret;

	if (dev->spi_desc->bit_order)
		*data = no_os_bit_swap_constant_8(buff[2]);
	else
		*data = buff[2];

	return 0;
}

/**
 * @brief Updates the value of an ADAR300X register.
 * @param dev	   - The device structure.
 * @param reg_addr - The register address.
 * @param mask	   - Bits to be updated.
 * @param data	   - Update value for the mask.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adar300x_reg_update(struct adar300x_dev *dev, uint16_t reg_addr,
			uint8_t mask, uint8_t data)
{
	uint8_t tmp, orig;
	int ret;

	ret = adar300x_reg_read(dev, reg_addr, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= data & mask;

	if (tmp != orig)
		return adar300x_reg_write(dev, reg_addr, tmp);

	return 0;
}

/**
 * @brief Selects the address page used by the beamstate RAM windows.
 * @param dev  - The device structure.
 * @param page - The address page.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_page(struct adar300x_dev *dev, uint8_t page)
{
	int ret;

	if (!dev)
		return -EINVAL;

	page &= ADAR300X_ADDRESS_PAGE_MSK;

	if (dev->cur_page == page)
		return 0;

	ret = adar300x_reg_update(dev, ADAR300X_REG_ADDRESS_PAGE,
				  ADAR300X_ADDRESS_PAGE_MSK, page);
	if (ret)
		return ret;

	dev->cur_page = page;

	return 0;
}

/**
 * @brief Writes a register on a given address page.
 * @param dev	   - The device structure.
 * @param page	   - The address page.
 * @param reg_addr - The register address.
 * @param data	   - Data value to write.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adar300x_page_reg_write(struct adar300x_dev *dev, uint8_t page,
			    uint16_t reg_addr, uint8_t data)
{
	int ret;

	ret = adar300x_set_page(dev, page);
	if (ret)
		return ret;

	return adar300x_reg_write(dev, reg_addr, data);
}

/**
 * @brief Reads a register from a given address page.
 * @param dev	   - The device structure.
 * @param page	   - The address page.
 * @param reg_addr - The register address.
 * @param data	   - Data read from the device.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
int adar300x_page_reg_read(struct adar300x_dev *dev, uint8_t page,
			   uint16_t reg_addr, uint8_t *data)
{
	int ret;

	ret = adar300x_set_page(dev, page);
	if (ret)
		return ret;

	return adar300x_reg_read(dev, reg_addr, data);
}

/**
 * @brief Resolves a beamstate bank, beam and element to a register address.
 * @param dev	    - The device structure.
 * @param beamstate - Which beamstate bank to address.
 * @param beam	    - Beam index.
 * @param element   - Element index within the beam.
 * @param param	    - Delay or attenuation.
 * @param reg_addr  - Resolved register address.
 * @return	    - 0 in case of success or negative error code otherwise.
 */
static int adar300x_element_addr(struct adar300x_dev *dev,
				 enum adar300x_beamstate beamstate,
				 uint8_t beam, uint8_t element,
				 enum adar300x_element_param param,
				 uint16_t *reg_addr)
{
	uint16_t offset;

	if (!dev || !reg_addr)
		return -EINVAL;

	if (beam >= dev->chip_info->num_beams ||
	    element >= dev->chip_info->num_elements)
		return -EINVAL;

	offset = beam * ADAR300X_VALUES_PER_BEAM + element * 2;
	if (param == ADAR300X_ATTENUATION)
		offset++;

	switch (beamstate) {
	case ADAR300X_BEAMSTATE_DIRECT:
		*reg_addr = ADAR300X_REG_DIRECT_CTRL(offset);
		return 0;
	case ADAR300X_BEAMSTATE_RESET:
		*reg_addr = ADAR300X_REG_RESET_BEAMSTATE(offset);
		return 0;
	case ADAR300X_BEAMSTATE_MUTE:
		*reg_addr = ADAR300X_REG_MUTE_BEAMSTATE(offset);
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Writes one element value in one of the beamstate banks.
 *
 * In direct mode the written value is applied on the next update; in instant
 * direct mode it takes effect immediately.
 *
 * @param dev	    - The device structure.
 * @param beamstate - Which beamstate bank to write.
 * @param beam	    - Beam index.
 * @param element   - Element index within the beam.
 * @param param	    - Delay or attenuation.
 * @param val	    - Value to write, 0 to ADAR300X_RAW_MAX.
 * @return	    - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_element(struct adar300x_dev *dev,
			 enum adar300x_beamstate beamstate, uint8_t beam,
			 uint8_t element, enum adar300x_element_param param,
			 uint8_t val)
{
	uint16_t reg_addr;
	int ret;

	if (val > ADAR300X_RAW_MAX)
		return -EINVAL;

	ret = adar300x_element_addr(dev, beamstate, beam, element, param,
				    &reg_addr);
	if (ret)
		return ret;

	return adar300x_page_reg_write(dev, ADAR300X_PAGE_CONFIG, reg_addr,
				       val);
}

/**
 * @brief Reads one element value back from one of the beamstate banks.
 * @param dev	    - The device structure.
 * @param beamstate - Which beamstate bank to read.
 * @param beam	    - Beam index.
 * @param element   - Element index within the beam.
 * @param param	    - Delay or attenuation.
 * @param val	    - Value read from the device.
 * @return	    - 0 in case of success or negative error code otherwise.
 */
int adar300x_get_element(struct adar300x_dev *dev,
			 enum adar300x_beamstate beamstate, uint8_t beam,
			 uint8_t element, enum adar300x_element_param param,
			 uint8_t *val)
{
	uint16_t reg_addr;
	int ret;

	if (!val)
		return -EINVAL;

	ret = adar300x_element_addr(dev, beamstate, beam, element, param,
				    &reg_addr);
	if (ret)
		return ret;

	ret = adar300x_page_reg_read(dev, ADAR300X_PAGE_CONFIG, reg_addr, val);
	if (ret)
		return ret;

	*val &= ADAR300X_RAW_MSK;

	return 0;
}

/**
 * @brief Writes an amplifier bias and enable pair.
 *
 * Both live in the same byte, so they are written together rather than as two
 * read modify write cycles.
 *
 * @param dev	   - The device structure.
 * @param reg_addr - Amplifier register address.
 * @param bias	   - Bias setting, 0 to ADAR300X_AMP_BIAS_MAX.
 * @param enable   - True to enable the amplifier in this state.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
static int adar300x_write_amp(struct adar300x_dev *dev, uint16_t reg_addr,
			      uint8_t bias, bool enable)
{
	uint8_t val;

	if (bias > ADAR300X_AMP_BIAS_MAX)
		return -EINVAL;

	val = no_os_field_prep(ADAR300X_AMP_BIAS_MSK, bias);
	if (enable)
		val |= ADAR300X_AMP_EN_MSK;

	return adar300x_page_reg_write(dev, ADAR300X_PAGE_CONFIG, reg_addr,
				       val);
}

/**
 * @brief Reads an amplifier bias and enable pair back.
 * @param dev	   - The device structure.
 * @param reg_addr - Amplifier register address.
 * @param bias	   - Bias setting read from the device, may be NULL.
 * @param enable   - Enable state read from the device, may be NULL.
 * @return	   - 0 in case of success or negative error code otherwise.
 */
static int adar300x_read_amp(struct adar300x_dev *dev, uint16_t reg_addr,
			     uint8_t *bias, bool *enable)
{
	uint8_t val;
	int ret;

	ret = adar300x_page_reg_read(dev, ADAR300X_PAGE_CONFIG, reg_addr, &val);
	if (ret)
		return ret;

	if (bias)
		*bias = no_os_field_get(ADAR300X_AMP_BIAS_MSK, val);
	if (enable)
		*enable = !!(val & ADAR300X_AMP_EN_MSK);

	return 0;
}

/**
 * @brief Sets the bias and enable of one beam amplifier in one state.
 * @param dev	 - The device structure.
 * @param state	 - Which beam amplifier state to write.
 * @param beam	 - Beam index.
 * @param bias	 - Bias setting, 0 to ADAR300X_AMP_BIAS_MAX.
 * @param enable - True to enable the amplifier in this state.
 * @return	 - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_beam_amp(struct adar300x_dev *dev,
			  enum adar300x_beam_amp_state state, uint8_t beam,
			  uint8_t bias, bool enable)
{
	if (!dev)
		return -EINVAL;

	if (beam >= dev->chip_info->num_beams ||
	    state > ADAR300X_BEAM_AMP_SLEEP)
		return -EINVAL;

	return adar300x_write_amp(dev, ADAR300X_REG_BEAM_AMP(state, beam),
				  bias, enable);
}

/**
 * @brief Reads back the bias and enable of one beam amplifier.
 * @param dev	 - The device structure.
 * @param state	 - Which beam amplifier state to read.
 * @param beam	 - Beam index.
 * @param bias	 - Bias setting read from the device, may be NULL.
 * @param enable - Enable state read from the device, may be NULL.
 * @return	 - 0 in case of success or negative error code otherwise.
 */
int adar300x_get_beam_amp(struct adar300x_dev *dev,
			  enum adar300x_beam_amp_state state, uint8_t beam,
			  uint8_t *bias, bool *enable)
{
	if (!dev)
		return -EINVAL;

	if (beam >= dev->chip_info->num_beams ||
	    state > ADAR300X_BEAM_AMP_SLEEP)
		return -EINVAL;

	return adar300x_read_amp(dev, ADAR300X_REG_BEAM_AMP(state, beam),
				 bias, enable);
}

/**
 * @brief Sets the bias and enable of one element amplifier in one state.
 * @param dev	  - The device structure.
 * @param state	  - Which element amplifier state to write.
 * @param element - Element index.
 * @param bias	  - Bias setting, 0 to ADAR300X_AMP_BIAS_MAX.
 * @param enable  - True to enable the amplifier in this state.
 * @return	  - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_element_amp(struct adar300x_dev *dev,
			     enum adar300x_element_amp_state state,
			     uint8_t element, uint8_t bias, bool enable)
{
	if (!dev)
		return -EINVAL;

	if (element >= dev->chip_info->num_elements ||
	    state > ADAR300X_ELEMENT_AMP_SLEEP)
		return -EINVAL;

	return adar300x_write_amp(dev,
				  ADAR300X_REG_ELEMENT_AMP(state, element),
				  bias, enable);
}

/**
 * @brief Reads back the bias and enable of one element amplifier.
 * @param dev	  - The device structure.
 * @param state	  - Which element amplifier state to read.
 * @param element - Element index.
 * @param bias	  - Bias setting read from the device, may be NULL.
 * @param enable  - Enable state read from the device, may be NULL.
 * @return	  - 0 in case of success or negative error code otherwise.
 */
int adar300x_get_element_amp(struct adar300x_dev *dev,
			     enum adar300x_element_amp_state state,
			     uint8_t element, uint8_t *bias, bool *enable)
{
	if (!dev)
		return -EINVAL;

	if (element >= dev->chip_info->num_elements ||
	    state > ADAR300X_ELEMENT_AMP_SLEEP)
		return -EINVAL;

	return adar300x_read_amp(dev,
				 ADAR300X_REG_ELEMENT_AMP(state, element),
				 bias, enable);
}

/**
 * @brief Selects where a beam takes its next beamstate from.
 * @param dev  - The device structure.
 * @param beam - Beam index.
 * @param mode - Beamstate source.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_beam_mode(struct adar300x_dev *dev, uint8_t beam,
			   enum adar300x_beam_mode mode)
{
	if (!dev)
		return -EINVAL;

	if (beam >= dev->chip_info->num_beams ||
	    mode > ADAR300X_BEAM_MODE_INST_DIRECT)
		return -EINVAL;

	return adar300x_reg_update(dev, ADAR300X_REG_BEAMSTATE_MODE,
				   ADAR300X_BEAM_MODE_MSK(beam),
				   mode << (beam * 2));
}

/**
 * @brief Reads back the mode of a beam.
 * @param dev  - The device structure.
 * @param beam - Beam index.
 * @param mode - Beamstate source read from the device.
 * @return     - 0 in case of success or negative error code otherwise.
 */
int adar300x_get_beam_mode(struct adar300x_dev *dev, uint8_t beam,
			   enum adar300x_beam_mode *mode)
{
	uint8_t val;
	int ret;

	if (!dev || !mode)
		return -EINVAL;

	if (beam >= dev->chip_info->num_beams)
		return -EINVAL;

	ret = adar300x_reg_read(dev, ADAR300X_REG_BEAMSTATE_MODE, &val);
	if (ret)
		return ret;

	*mode = (val & ADAR300X_BEAM_MODE_MSK(beam)) >> (beam * 2);

	return 0;
}

/**
 * @brief Strobes an update on every beam set in the mask.
 *
 * Only has an effect when the update source is SPI rather than the pins, see
 * adar300x_set_update_source_spi().
 *
 * @param dev	    - The device structure.
 * @param beam_mask - Bit per beam, bit 0 is beam 0.
 * @return	    - 0 in case of success or negative error code otherwise.
 */
int adar300x_update(struct adar300x_dev *dev, uint8_t beam_mask)
{
	if (!dev)
		return -EINVAL;

	if (beam_mask >= NO_OS_BIT(dev->chip_info->num_beams))
		return -EINVAL;

	return adar300x_reg_write(dev, ADAR300X_REG_BEAMWISE_UPDATE,
				  beam_mask);
}

/**
 * @brief Chooses whether update, mute and reset come from the pins or SPI.
 * @param dev - The device structure.
 * @param spi - True to drive the commands over SPI, false to use the pins.
 * @return    - 0 in case of success or negative error code otherwise.
 */
int adar300x_set_update_source_spi(struct adar300x_dev *dev, bool spi)
{
	if (!dev)
		return -EINVAL;

	return adar300x_reg_update(dev, ADAR300X_REG_PIN_OR_SPI_CTL,
				   ADAR300X_UPDATE_SPI_CTL,
				   spi ? ADAR300X_UPDATE_SPI_CTL : 0);
}

/**
 * @brief Reads back the update source selection.
 * @param dev - The device structure.
 * @param spi - True when the commands are driven over SPI.
 * @return    - 0 in case of success or negative error code otherwise.
 */
int adar300x_get_update_source_spi(struct adar300x_dev *dev, bool *spi)
{
	uint8_t val;
	int ret;

	if (!dev || !spi)
		return -EINVAL;

	ret = adar300x_reg_read(dev, ADAR300X_REG_PIN_OR_SPI_CTL, &val);
	if (ret)
		return ret;

	*spi = !!(val & ADAR300X_UPDATE_SPI_CTL);

	return 0;
}

/**
 * @brief Reads the on-chip ADC.
 *
 * The ADC requires a manual reset pulse before each conversion, and clocking
 * is provided by dummy SPI transactions after the start bit is set. The
 * conversion complete flag is polled before reading the result.
 *
 * @param dev   - The device structure.
 * @param input - Analog input mux selection.
 * @param value - ADC conversion result.
 * @return	- 0 in case of success or negative error code otherwise.
 */
int adar300x_adc_read(struct adar300x_dev *dev, enum adar300x_adc_input input,
		      uint8_t *value)
{
	uint8_t ctrl2;
	int ret, i;
	int timeout_us = 50000;
	int poll_us = 100;

	if (!dev || !value)
		return -EINVAL;

	if (input > ADAR300X_ADC_TEMPERATURE)
		return -EINVAL;

	/* Reset the ADC */
	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL,
				  ADAR300X_ADC_RESET_MSK, ADAR300X_ADC_RESET_MSK);
	if (ret)
		return ret;

	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL,
				  ADAR300X_ADC_RESET_MSK, 0);
	if (ret)
		return ret;

	/* Enable and select the input */
	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL,
				  ADAR300X_ADC_CLK_EN_MSK | ADAR300X_ADC_EN_MSK |
				  ADAR300X_ADC_MUX_SEL_MSK,
				  ADAR300X_ADC_CLK_EN_MSK | ADAR300X_ADC_EN_MSK |
				  input);
	if (ret)
		return ret;

	/* Start conversion: clear then set the start bit */
	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL2,
				  ADAR300X_ADC_START_MSK, 0);
	if (ret)
		return ret;
	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL2,
				  ADAR300X_ADC_START_MSK, 0);
	if (ret)
		return ret;
	ret = adar300x_reg_update(dev, ADAR300X_REG_ADC_CONTROL2,
				  ADAR300X_ADC_START_MSK, ADAR300X_ADC_START_MSK);
	if (ret)
		return ret;

	/*
	 * The SAR runs off SCLK, so the conversion only advances while the bus
	 * is active. Seven dummy transactions are not enough on their own: at
	 * 24 clocks each they supply 168 of the 784 the data sheet requires.
	 * The status reads below make up the rest, which is why the conversion
	 * must be polled rather than waited out with a delay.
	 */
	for (i = 0; i < ADAR300X_ADC_NUM_CLOCKS; i++) {
		ret = adar300x_reg_write(dev, ADAR300X_REG_SCRATCHPAD,
					 ADAR300X_SCRATCHPAD_PATTERN_2);
		if (ret)
			return ret;
	}

	/* Poll for conversion complete */
	for (i = 0; i < timeout_us / poll_us; i++) {
		ret = adar300x_reg_read(dev, ADAR300X_REG_ADC_CONTROL2, &ctrl2);
		if (ret)
			return ret;
		if (ctrl2 & ADAR300X_ADC_END_CONV_MSK)
			break;
		no_os_udelay(poll_us);
	}

	if (!(ctrl2 & ADAR300X_ADC_END_CONV_MSK))
		return -ETIMEDOUT;

	return adar300x_reg_read(dev, ADAR300X_REG_ADC_DATA_OUT, value);
}

/**
 * @brief Pulses the RSTB pin. No-op when no reset GPIO is wired.
 * @param dev - The device structure.
 * @return    - 0 in case of success or negative error code otherwise.
 */
int adar300x_hard_reset(struct adar300x_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (!dev->gpio_rstb)
		return 0;

	ret = no_os_gpio_set_value(dev->gpio_rstb, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(1);

	ret = no_os_gpio_set_value(dev->gpio_rstb, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1);

	dev->cur_page = ADAR300X_PAGE_INVALID;

	return 0;
}

/**
 * @brief Issues the soft reset sequence.
 *
 * Chip 0 uses the bus wide reset in ADAR300X_REG_SPI_CONFIG, which the
 * datasheet documents for power up. Chips 1 to 15 ignore that register and
 * must use the individual reset in ADAR300X_REG_CHIP_RESET instead, so the
 * register is chosen from dev_addr. The bus wide reset also resets every other
 * chip sharing the bus, which matters when bringing up several devices.
 *
 * The reset is issued several times back to back, as the datasheet recommends
 * when RSTB could not be held low across the supply ramp. The SPI_CONFIG
 * readback is logged but not enforced; the scratchpad test in adar300x_init()
 * is the authoritative check that the bus works.
 *
 * @param dev - The device structure.
 * @return    - 0 in case of success or negative error code otherwise.
 */
int adar300x_soft_reset(struct adar300x_dev *dev)
{
	uint8_t val;
	int ret;
	int i;

	if (!dev)
		return -EINVAL;

	for (i = 0; i < ADAR300X_SOFT_RESET_COUNT; i++) {
		if (dev->dev_addr == 0)
			ret = adar300x_reg_write(dev, ADAR300X_REG_SPI_CONFIG,
						 ADAR300X_SOFT_RESET_CMD);
		else
			ret = adar300x_reg_write(dev, ADAR300X_REG_CHIP_RESET,
						 no_os_field_prep(ADAR300X_CHIP_RESET_MSK,
							 ADAR300X_CHIP_RESET_VAL));
		if (ret)
			return ret;

		no_os_udelay(100);
	}

	dev->cur_page = ADAR300X_PAGE_INVALID;

	ret = adar300x_reg_read(dev, ADAR300X_REG_SPI_CONFIG, &val);
	if (ret)
		return ret;

	pr_info("SPI_CONFIG after soft reset: 0x%02X\n", val);

	return 0;
}

/**
 * @brief Writes then reads back a scratchpad pattern.
 * @param dev	  - The device structure.
 * @param pattern - Pattern to write and expect.
 * @return	  - 0 in case of success or negative error code otherwise.
 */
static int adar300x_check_scratchpad(struct adar300x_dev *dev, uint8_t pattern)
{
	uint8_t val;
	int ret;

	ret = adar300x_reg_write(dev, ADAR300X_REG_SCRATCHPAD, pattern);
	if (ret)
		return ret;

	ret = adar300x_reg_read(dev, ADAR300X_REG_SCRATCHPAD, &val);
	if (ret)
		return ret;

	if (val != pattern) {
		pr_err("SCRATCHPAD 0x%02X: read back 0x%02X\n", pattern, val);
		return -ENODEV;
	}

	pr_info("SCRATCHPAD 0x%02X: OK\n", pattern);

	return 0;
}

/**
 * @brief Requests an optional GPIO and drives it low.
 * @param desc	- Descriptor to populate, set to NULL when param is NULL.
 * @param param - Initialization parameters, may be NULL.
 * @return	- 0 in case of success or negative error code otherwise.
 */
static int adar300x_gpio_setup(struct no_os_gpio_desc **desc,
			       struct no_os_gpio_init_param *param)
{
	int ret;

	ret = no_os_gpio_get_optional(desc, param);
	if (ret)
		return ret;

	if (!*desc)
		return 0;

	return no_os_gpio_direction_output(*desc, NO_OS_GPIO_LOW);
}

/**
 * @brief Initializes the ADAR300X.
 * @param dev	     - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return	     - 0 in case of success or negative error code otherwise.
 */
int adar300x_init(struct adar300x_dev **dev,
		  struct adar300x_init_param *init_param)
{
	struct adar300x_dev *device;
	uint8_t chiptype;
	uint8_t pid_l, pid_h, rev;
	int ret;

	if (!dev || !init_param)
		return -EINVAL;

	if (init_param->type >= NO_OS_ARRAY_SIZE(adar300x_chip_info))
		return -EINVAL;

	if (init_param->dev_addr > 0xF)
		return -EINVAL;

	device = no_os_calloc(1, sizeof(*device));
	if (!device)
		return -ENOMEM;

	device->type = init_param->type;
	device->chip_info = &adar300x_chip_info[init_param->type];
	device->dev_addr = init_param->dev_addr;
	device->cur_page = ADAR300X_PAGE_INVALID;

	/*
	 * Every control pin parks low. That is the required bring-up state:
	 * the eval board SPI mux selects the hardware SPI bus when its select
	 * input is low, and the beam qualifiers must be idle. RSTB is released
	 * by the hard reset below.
	 */
	ret = adar300x_gpio_setup(&device->gpio_mux_sel,
				  init_param->gpio_mux_sel);
	if (ret)
		goto error_gpio;

	ret = adar300x_gpio_setup(&device->gpio_update, init_param->gpio_update);
	if (ret)
		goto error_gpio;

	ret = adar300x_gpio_setup(&device->gpio_reset, init_param->gpio_reset);
	if (ret)
		goto error_gpio;

	ret = adar300x_gpio_setup(&device->gpio_mute, init_param->gpio_mute);
	if (ret)
		goto error_gpio;

	ret = adar300x_gpio_setup(&device->gpio_rstb, init_param->gpio_rstb);
	if (ret)
		goto error_gpio;

	ret = adar300x_hard_reset(device);
	if (ret)
		goto error_gpio;

	if (init_param->spi_desc_shared) {
		device->spi_desc = init_param->spi_desc_shared;
		device->spi_desc_is_owned = false;
	} else {
		ret = no_os_spi_init(&device->spi_desc, &init_param->spi_init);
		if (ret)
			goto error_gpio;

		device->spi_desc_is_owned = true;
	}

	ret = adar300x_soft_reset(device);
	if (ret)
		goto error_spi;

	ret = adar300x_check_scratchpad(device, ADAR300X_SCRATCHPAD_PATTERN_1);
	if (ret)
		goto error_spi;

	ret = adar300x_check_scratchpad(device, ADAR300X_SCRATCHPAD_PATTERN_2);
	if (ret)
		goto error_spi;

	ret = adar300x_reg_read(device, ADAR300X_REG_CHIPTYPE, &chiptype);
	if (ret)
		goto error_spi;

	if (chiptype != ADAR300X_CHIPTYPE_VAL) {
		pr_err("CHIPTYPE: read 0x%02X, expected 0x%02X\n", chiptype,
		       ADAR300X_CHIPTYPE_VAL);
		ret = -ENODEV;
		goto error_spi;
	}

	pr_info("CHIPTYPE: 0x%02X\n", chiptype);

	ret = adar300x_reg_read(device, ADAR300X_REG_PRODUCT_ID_L, &pid_l);
	if (ret)
		goto error_spi;

	ret = adar300x_reg_read(device, ADAR300X_REG_PRODUCT_ID_H, &pid_h);
	if (ret)
		goto error_spi;

	ret = adar300x_reg_read(device, ADAR300X_REG_SPI_REV, &rev);
	if (ret)
		goto error_spi;

	pr_info("PRODUCT_ID: 0x%02X 0x%02X   SPI_REV: 0x%02X\n", pid_l, pid_h,
		rev);

	pr_info("adar300x_init: OK (%s, dev_addr %u)\n",
		device->chip_info->name, device->dev_addr);

	*dev = device;

	return 0;

error_spi:
	if (device->spi_desc_is_owned)
		no_os_spi_remove(device->spi_desc);
error_gpio:
	no_os_gpio_remove(device->gpio_rstb);
	no_os_gpio_remove(device->gpio_mute);
	no_os_gpio_remove(device->gpio_reset);
	no_os_gpio_remove(device->gpio_update);
	no_os_gpio_remove(device->gpio_mux_sel);
	no_os_free(device);

	return ret;
}

/**
 * @brief Frees the resources allocated by adar300x_init().
 * @param dev - The device structure.
 * @return    - 0 in case of success or negative error code otherwise.
 */
int adar300x_remove(struct adar300x_dev *dev)
{
	if (!dev)
		return -EINVAL;

	if (dev->spi_desc_is_owned)
		no_os_spi_remove(dev->spi_desc);

	no_os_gpio_remove(dev->gpio_rstb);
	no_os_gpio_remove(dev->gpio_mute);
	no_os_gpio_remove(dev->gpio_reset);
	no_os_gpio_remove(dev->gpio_update);
	no_os_gpio_remove(dev->gpio_mux_sel);

	no_os_free(dev);

	return 0;
}
