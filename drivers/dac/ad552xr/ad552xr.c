/***************************************************************************//**
 *   @file   ad552xr.c
 *   @brief  Implementation of AD552XR Driver.
 *   @author Naga Himanshu Indraganti (naga.indraganti@analog.com)
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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ad552xr.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

static const uint16_t ad552xr_supported_product_ids[AD552XR_NUM_TYPES][1] = {
	[ID_AD5529R] = {
		AD5529R_PID_16BIT_16CH_WLCSP
	}
};

/**
 * @brief Assert Multi Drop Address GPIO lines.
 * @param dev - The device structure.
 * @return 0 in case of success, negative value otherwise.
 */
static int ad552xr_assert_dev_addr_gpios(struct ad552xr_dev *dev)
{
	int ret;
	enum no_os_gpio_values md_addr0_val;
	enum no_os_gpio_values md_addr1_val;

	if (!dev->gpio_md_addr[0] || !dev->gpio_md_addr[1])
		return 0;

	/* Select the MDADDR lines by extracting the 2 least significant bits from device address value */
	md_addr0_val = (dev->dev_addr & 1) ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW;
	md_addr1_val = ((dev->dev_addr >> 1) & 1) ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW;

	/* Assert the MDADDR lines */
	ret = no_os_gpio_set_value(dev->gpio_md_addr[0], md_addr0_val);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_md_addr[1], md_addr1_val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get device information.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad552xr_get_device_info(struct ad552xr_dev *dev)
{
	int ret;
	uint16_t val;
	uint16_t product_id;
	uint8_t i;
	bool found = false;

	/* Read Product ID */
	ret = ad552xr_spi_reg_read(dev, AD552XR_REG_PRODUCT_ID_L, &product_id);
	if (ret)
		return -EINVAL;

	ret = ad552xr_spi_reg_read(dev, AD552XR_REG_PRODUCT_ID_H, &val);
	if (ret)
		return -EINVAL;

	product_id |= (val << 8);

	/* Map product id for device supported resolution and number of channels */
	for (i = 0;
	     i < NO_OS_ARRAY_SIZE(ad552xr_supported_product_ids[dev->dev_info.type]); i++) {
		if (product_id == ad552xr_supported_product_ids[dev->dev_info.type][i])
			found = true;
	}

	if (!found)
		return -EINVAL;

	switch (product_id) {
	case AD5529R_PID_16BIT_16CH_WLCSP:
		dev->dev_info.resolution = AD552XR_RESOLUTION_16_BIT;
		dev->dev_info.num_channels = AD552XR_NUM_CHANNELS_16;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Perform a write operation followed by a read
 *        operation from the scratch pad register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad552xr_check_scratch_pad(struct ad552xr_dev *dev)
{
	int ret;
	uint16_t val;

	if (!dev)
		return -EINVAL;

	ret = ad552xr_spi_reg_write(dev, AD552XR_REG_SCRATCH_PAD,
				    AD552XR_SCRATCH_PAD_TEST_VAL);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = ad552xr_spi_reg_read(dev, AD552XR_REG_SCRATCH_PAD, &val);
	if (ret)
		return ret;

	if (val != AD552XR_SCRATCH_PAD_TEST_VAL) {
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Deallocate memory for the GPIOs assigned.
 * @param dev - Device driver handler.
 * @return 0 in case of success, negative value otherwise.
 */
static int ad552xr_remove_gpio(struct ad552xr_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Reset_B GPIO */
	ret = no_os_gpio_remove(dev->gpio_resetb);
	if (ret)
		return ret;

	/* Clear_B GPIO */
	ret = no_os_gpio_remove(dev->gpio_clearb);
	if (ret)
		return ret;

	/* Alarm_B GPIO */
	ret = no_os_gpio_remove(dev->gpio_alarmb);
	if (ret)
		return ret;

	/* MD Addr GPIO Lines */
	for (uint8_t i = 0; i < AD552XR_NUM_MD_ADDR_LINES; i++) {
		ret = no_os_gpio_remove(dev->gpio_md_addr[i]);
		if (ret)
			return ret;
	}

	/* LDAC TGP pins */
	for (uint8_t i = 0; i < AD552XR_NUM_LDAC_TGP_PINS; i++) {
		ret = no_os_gpio_remove(dev->gpio_ldac_tgp[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Device configuration reset.
 * @param dev - The device structure.
 * @param sw_reset - If true, soft reset, otherwise, hard reset of device.
 * @return 0 in case of success, negative value otherwise.
 */
static int ad552xr_cfg_reset(struct ad552xr_dev *dev, bool sw_reset)
{
	/* For SW reset, address ascension mode is retained. */
	if (!sw_reset)
		dev->spi_cfg.addr_asc = false;

	memset(&dev->spi_cfg, 0, sizeof(dev->spi_cfg));

	return 0;
}

/**
 * @brief Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_spi_reg_read(struct ad552xr_dev *dev,
			 uint32_t reg_addr,
			 uint16_t *reg_data)
{
	int ret;
	uint32_t addr;
	uint8_t reg_len;
	uint8_t cnt = 0;
	uint8_t dindx = 0;
	uint8_t buf[5] = {0};

	if (!dev || !reg_data)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD552XR_LEN(reg_addr);

	/* If not address ascension, start accessing from next address */
	if (!dev->spi_cfg.addr_asc)
		addr = AD552XR_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD552XR_ADDR(reg_addr);

	/* Update the transfer buffer with read/write bit field and address */
	buf[cnt++] = AD552XR_READ_BIT | (addr >> 8);
	buf[cnt++] = addr;

	/* Save the data phase start index */
	dindx = cnt;

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	/* Select the device address */
	ret = ad552xr_assert_dev_addr_gpios(dev);
	if (ret)
		return ret;

	/* Transfer the data */
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
	if (ret)
		return ret;

	/* Store the data based on data length */
	if (reg_len == 1)
		*reg_data = buf[dindx];
	else
		*reg_data = no_os_get_unaligned_be16(&buf[dindx]);

	return 0;
}

/**
 * @brief Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_spi_reg_write(struct ad552xr_dev *dev,
			  uint32_t reg_addr,
			  uint16_t reg_data)
{
	int ret;
	uint32_t addr;
	uint8_t reg_len;
	uint8_t cnt = 0;
	uint8_t buf[5] = {0};

	if (!dev)
		return -EINVAL;

	/* Get the register length */
	reg_len = AD552XR_LEN(reg_addr);

	/* If not address ascension, start accessing from next address */
	if (!dev->spi_cfg.addr_asc)
		addr = AD552XR_ADDR(reg_addr) + reg_len - 1;
	else
		addr = AD552XR_ADDR(reg_addr);

	/* Update the transfer buffer with address */
	buf[cnt++] = (addr >> 8);
	buf[cnt++] = addr;

	/* Swap the contents of data */
	no_os_memswap64(&reg_data, reg_len, reg_len);
	memcpy(&buf[cnt], &reg_data, reg_len);

	/* Add the register data size to calculate an actual length of SPI frame */
	cnt += reg_len;

	/* Select the device address */
	ret = ad552xr_assert_dev_addr_gpios(dev);
	if (ret)
		return ret;

	/* Transfer the data */
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, cnt);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_spi_write_mask(struct ad552xr_dev *dev,
			   uint32_t reg_addr,
			   uint32_t mask,
			   uint16_t data)
{
	int ret;
	uint8_t reg_len;
	uint16_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = ad552xr_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	if (dev->spi_cfg.addr_asc) {
		reg_len = AD552XR_LEN(reg_addr);
		no_os_memswap64(&mask, reg_len, reg_len);
	}

	reg_data = (reg_data & ~mask) | no_os_field_prep(mask, data);

	return ad552xr_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * Set reference configuration.
 * @param dev - The device structure.
 * @param ref_sel - The reference to be selected.
 * 			Accepted values: AD552XR_INTERNAL_VREF_4P096V (default)
 * 					 AD552XR_EXTERNAL_VREF
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_reference(struct ad552xr_dev *dev,
			  enum ad552xr_vref_select ref_sel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_REF_SEL,
				     AD552XR_VREF_SEL_MASK,
				     ref_sel);
	if (ret)
		return ret;

	dev->vref = ref_sel;

	return 0;
}

/**
 * Set output range for a channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param range_sel - Output range to be selected.
 * 				Accepted values: AD552XR_UNIPOLAR_5V (default)
 * 						 AD552XR_UNIPOLAR_10V
 * 						 AD552XR_UNIPOLAR_20V
 * 						 AD552XR_UNIPOLAR_40V
 * 						 AD552XR_BIPOLAR_5V
 * 						 AD552XR_BIPOLAR_10V
 * 						 AD552XR_BIPOLAR_15V
 * 						 AD552XR_BIPOLAR_20V
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_ch_output_range(struct ad552xr_dev *dev,
				uint8_t ch,
				enum ad552xr_output_range range_sel)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_OUT_RANGE_CH(ch),
				     AD552XR_OUT_RANGE_CHn_MASK,
				     range_sel);
	if (ret)
		return ret;

	dev->range[ch] = range_sel;

	return 0;
}

/**
 * Set the channel output enable.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param en - Enable/disable for the channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_channel_output_en(struct ad552xr_dev *dev, uint8_t ch, bool en)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_OUT_EN,
				     AD552XR_CHANNEL_SEL(ch),
				     (uint8_t) en);
	if (ret)
		return ret;

	dev->ldac_cfg.output_en_mask &= ~AD552XR_CHANNEL_SEL(ch);
	dev->ldac_cfg.output_en_mask |= (en << ch);

	return 0;
}

/**
 * Set the function generator enable configuration.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param en - Enable/disable for the channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_func_en(struct ad552xr_dev *dev, uint8_t ch, bool en)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_FUNC_EN,
				     AD552XR_CHANNEL_SEL(ch),
				     (uint8_t) en);
	if (ret)
		return ret;

	dev->ldac_cfg.func_en_mask &= ~AD552XR_CHANNEL_SEL(ch);
	dev->ldac_cfg.func_en_mask |= (en << ch);

	return 0;
}

/**
 * Set function generator mode for a channel.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param func_mode_sel - Function generator mode to be selected.
 * 				Accepted values: AD552XR_FUNCTION_MODE_TOGGLE (default)
 * 						 AD552XR_FUNCTION_MODE_DITHER
 * 						 AD552XR_FUNCTION_MODE_TRIANGULAR
 * 						 AD552XR_FUNCTION_MODE_SAWTOOTH
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_func_mode_select(struct ad552xr_dev *dev,
			     uint8_t ch,
			     enum ad552xr_function_mode func_mode_sel)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_FUNC_MODE_SEL_CH(ch),
				     AD552XR_FUNC_MODE_SEL_CHn,
				     func_mode_sel);
	if (ret)
		return ret;

	dev->ldac_cfg.func_mode[ch] = func_mode_sel;

	return 0;
}

/**
 * Set period for dither function generator.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param period - Dither period to be selected.
 * 				Accepted values: SAMPLES_128 (default)
 * 						 SAMPLES_64
 * 						 SAMPLES_32
 * 						 SAMPLES_16
 * 						 SAMPLES_8
 * 						 SAMPLES_4
 * 						 SAMPLES_2
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_dither_period(struct ad552xr_dev *dev,
			      uint8_t ch,
			      enum ad552xr_dither_period period)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_reg_write(dev,
				    AD552XR_REG_FUNC_DITHER_PERIOD_CH(ch),
				    period);
	if (ret)
		return ret;

	dev->ldac_cfg.dither_period[ch] = period;

	return 0;
}

/**
 * Set phase for dither function generator.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param phase - Dither phase to be selected.
 * 				Accepted values: DEGREES_0 (default)
 * 						 DEGREES_90
 * 						 DEGREES_180
 * 						 DEGREES_270
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_dither_phase(struct ad552xr_dev *dev,
			     uint8_t ch,
			     enum ad552xr_dither_phase phase)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_reg_write(dev,
				    AD552XR_REG_FUNC_DITHER_PHASE_CH(ch),
				    phase);
	if (ret)
		return ret;

	dev->ldac_cfg.dither_phase[ch] = phase;

	return 0;
}

/**
 * Set ramp step size.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param step_size - Step size for ramp waveform.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_ramp_step_size(struct ad552xr_dev *dev, uint8_t ch,
			       uint8_t step_size)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_reg_write(dev,
				    AD552XR_REG_FUNC_RAMP_STEP_CH(ch),
				    step_size);
	if (ret)
		return ret;

	dev->ldac_cfg.ramp_step[ch] = step_size;

	return 0;
}

/**
 * Set the hardware and software ldac configuration.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param ldac_mode_sel - LDAC mode to be selected.
 * 				Accepted values: AD552XR_HW_LDAC (default)
 * 						 AD552XR_SW_LDAC
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_hw_sw_ldac(struct ad552xr_dev *dev,
			   uint8_t ch,
			   enum ad552xr_hw_sw_ldac ldac_mode_sel)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_LDAC_HW_SW,
				     AD552XR_CHANNEL_SEL(ch),
				     ldac_mode_sel);
	if (ret)
		return ret;

	dev->ldac_cfg.ldac_hw_sw_mask &= ~AD552XR_CHANNEL_SEL(ch);
	dev->ldac_cfg.ldac_hw_sw_mask |= (ldac_mode_sel << ch);

	return 0;
}

/**
 * Set sync/async ldac configuration.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param is_sync - To set to synchronous LDAC.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_sync_async_ldac(struct ad552xr_dev *dev,
				uint8_t ch,
				bool is_sync)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_LDAC_SYNC_ASYNC,
				     AD552XR_CHANNEL_SEL(ch),
				     is_sync);
	if (ret)
		return ret;

	dev->ldac_cfg.ldac_sync_async_mask &= ~AD552XR_CHANNEL_SEL(ch);
	dev->ldac_cfg.ldac_sync_async_mask |= (is_sync << ch);

	return 0;
}

/**
 * @brief Set LDAC/TGP edge trigger.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param trig_edge - Trigger edge select.
 * 				Accepted values: RISING_EDGE_TRIG (default)
 * 						 FALLING_EDGE_TRIG
 * 						 ANY_EDGE_TRIG
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_hw_edge_trigger(struct ad552xr_dev *dev,
				uint8_t ch,
				enum ad552xr_ldac_edge_trig trig_edge)
{
	int ret;

	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_LDAC_HW_SRC_CH(ch),
				     AD552XR_LDAC_HW_EDGE_SEL_CH_MASK,
				     trig_edge);
	if (ret)
		return ret;

	dev->ldac_cfg.edge_trig[ch] = trig_edge;

	return 0;
}

/**
 * @brief Set LDAC/TGP source pin.
 * @param dev - The device structure.
 * @param ch - Channel selected.
 * @param ldac_hw_sel - HW LDAC Toggle pin select.
 * 				Accepted values: AD552XR_LDAC_TGP_0 (default)
 * 						 AD552XR_LDAC_TGP_1
 * 						 AD552XR_LDAC_TGP_2
 * 						 AD552XR_LDAC_TGP_3
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_hw_ldac_toggle_pin(struct ad552xr_dev *dev,
				   uint8_t ch,
				   enum ad552xr_ldac_toggle_sel ldac_hw_sel)
{
	int ret;

	if (!dev ||
	    !dev->gpio_ldac_tgp[ldac_hw_sel] ||
	    ch >= dev->dev_info.num_channels)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_LDAC_HW_SRC_CH(ch),
				     AD552XR_LDAC_HW_SEL_CH_MASK,
				     ldac_hw_sel);
	if (ret)
		return ret;

	dev->ldac_cfg.toggle_sel[ch] = ldac_hw_sel;

	return 0;
}

/**
 * Set dac value on DAC input register A.
 * @param dev - The device structure.
 * @param ch - selected channel.
 * @param dac_value - value that will be set in DAC input register A.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_dac_a_value(struct ad552xr_dev *dev,
			    uint8_t ch,
			    uint16_t dac_value)
{
	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	return ad552xr_spi_reg_write(dev,
				     AD552XR_REG_DAC_INPUT_A_CH(ch),
				     dac_value);
}

/**
 * Set dac value on DAC input register B.
 * @param dev - The device structure.
 * @param ch - selected channel.
 * @param dac_value - value that will be set in DAC input register B.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_dac_b_value(struct ad552xr_dev *dev,
			    uint8_t ch,
			    uint16_t dac_value)
{
	if (!dev || ch >= dev->dev_info.num_channels)
		return -EINVAL;

	return ad552xr_spi_reg_write(dev,
				     AD552XR_REG_FUNC_DAC_INPUT_B_CH(ch),
				     dac_value);
}

/**
 * @brief Trigger the LDAC using hardware.
 * @param dev - The device structure.
 * @param ldac_hw_sel - HW LDAC Toggle pin select.
 * 				Accepted values: AD552XR_LDAC_TGP_0 (default)
 * 						 AD552XR_LDAC_TGP_1
 * 						 AD552XR_LDAC_TGP_2
 * 						 AD552XR_LDAC_TGP_3
 * @param delay_us - Pulse width of the HW toggle gpio trigger in microseconds.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_hw_ldac_trigger(struct ad552xr_dev *dev,
			    enum ad552xr_ldac_toggle_sel ldac_hw_sel,
			    uint32_t delay_us)
{
	int ret;

	if (!dev || !dev->gpio_ldac_tgp[ldac_hw_sel])
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_ldac_tgp[ldac_hw_sel], NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(delay_us >> 1);

	ret = no_os_gpio_set_value(dev->gpio_ldac_tgp[ldac_hw_sel], NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Provide delay to achieve 50% duty cycle */
	no_os_udelay(delay_us >> 1);

	return 0;
}

/**
 * @brief Trigger the LDAC using software.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_sw_ldac_trigger(struct ad552xr_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad552xr_spi_reg_write(dev,
				     AD552XR_REG_SW_LDAC,
				     dev->ldac_cfg.ldac_hw_sw_mask);
}

/**
 * Set the multiplexer output select register.
 * @param dev - The device structure.
 * @param mux_output_sel - signal to be monitored on the mux_out pin.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_mux_out_select(struct ad552xr_dev *dev,
			       enum ad552xr_mux_out_select mux_output_sel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (mux_output_sel != AD552XR_MUX_OUT_AGND) {
		ret = ad552xr_spi_write_mask(dev, AD552XR_REG_MUX_OUT_SEL,
					     AD552XR_MUX_PARAM_SEL_MASK,
					     mux_output_sel - 1);
		if (ret)
			return ret;
	}

	ret = ad552xr_spi_write_mask(dev, AD552XR_REG_MUX_OUT_SEL,
				     AD552XR_MUX_OUT_EN,
				     (mux_output_sel != AD552XR_MUX_OUT_AGND));
	if (ret)
		return ret;

	dev->mux_out_sel = mux_output_sel;

	return 0;
}

/**
 * @brief Set device spi settings.
 * @param dev - The device structure.
 * @param spi_settings - The structure that contains the device spi parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad552xr_set_device_spi(struct ad552xr_dev *dev,
			   struct ad552xr_device_spi_settings *spi_settings)
{
	int ret;

	if (!dev || !spi_settings)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_INTERFACE_CONFIG_A,
				     AD552XR_INT_CONFIG_A_ADDR_ASC_MASK,
				     spi_settings->addr_asc);
	if (ret)
		return ret;

	dev->spi_cfg.addr_asc = spi_settings->addr_asc;

	return 0;
}

/**
 * @brief Perform soft reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_sw_reset(struct ad552xr_dev *dev)
{
	int ret;
	uint16_t val;

	if (!dev)
		return -EINVAL;

	ret = ad552xr_spi_write_mask(dev,
				     AD552XR_REG_INTERFACE_CONFIG_A,
				     AD552XR_INT_CONFIG_A_SW_RESET_MASK,
				     AD552XR_INT_CONFIG_A_SW_RESET_MASK);
	if (ret)
		return ret;

	ret = ad552xr_spi_reg_read(dev, AD552XR_REG_INTERFACE_CONFIG_A, &val);
	if (ret)
		return ret;

	ret = ad552xr_spi_reg_read(dev, AD552XR_REG_DEVICE_CONFIG, &val);
	if (ret)
		return ret;

	/* Default interface configuration after reset */
	return ad552xr_cfg_reset(dev, true);
}

/**
 * @brief Perform hard reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_hw_reset(struct ad552xr_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_resetb, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(dev->gpio_resetb, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	/* Default interface configuration after reset */
	return ad552xr_cfg_reset(dev, false);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_init(struct ad552xr_dev **device,
		 struct ad552xr_init_param *init_param)
{
	int ret;
	struct ad552xr_dev *dev;

	/* Initial checks */
	if (!device)
		return -ENODEV;

	if (!init_param)
		return -EINVAL;

	if (init_param->dev_addr > 3 || init_param->type >= AD552XR_NUM_TYPES)
		return -EINVAL;

	dev = (struct ad552xr_dev*)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init_prm);
	if (ret)
		goto err_dev;

	/* Reset GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_resetb, init_param->gpio_resetb);
	if (ret)
		goto err_spi;

	/* If Reset GPIO init params passed perform hw reset else sw reset */
	if (dev->gpio_resetb) {
		ret = no_os_gpio_direction_output(dev->gpio_resetb, NO_OS_GPIO_HIGH);
		if (ret)
			goto err_gpio;

		ret = ad552xr_hw_reset(dev);
		if (ret) {
			goto err_gpio;
		}
	} else {
		ret = ad552xr_sw_reset(dev);
		if (ret) {
			goto err_gpio;
		}

		/* Emulate hard reset by writing default value on Interface Configuration A register in addition to SW reset */
		ret = ad552xr_spi_reg_write(dev, AD552XR_REG_INTERFACE_CONFIG_A,
					    AD552XR_INTF_CFG_A_DEFAULT);
		if (ret)
			goto err_gpio;
	}

	/* Clear GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_clearb, init_param->gpio_clearb);
	if (ret)
		goto err_gpio;

	if (dev->gpio_clearb) {
		ret = no_os_gpio_direction_output(dev->gpio_clearb, NO_OS_GPIO_LOW);
		if (ret)
			goto err_gpio;

		no_os_mdelay(1);

		ret = no_os_gpio_set_value(dev->gpio_clearb, NO_OS_GPIO_HIGH);
		if (ret)
			goto err_gpio;
	}

	/* Alarm GPIO */
	ret = no_os_gpio_get_optional(&dev->gpio_alarmb, init_param->gpio_alarmb);
	if (ret)
		goto err_gpio;

	if (dev->gpio_alarmb) {
		ret = no_os_gpio_direction_input(dev->gpio_alarmb);
		if (ret)
			goto err_gpio;
	}

	/* MD Addr GPIO Lines */
	for (uint8_t i = 0; i < AD552XR_NUM_MD_ADDR_LINES; i++) {
		ret = no_os_gpio_get_optional(&dev->gpio_md_addr[i],
					      init_param->gpio_md_addr[i]);
		if (ret)
			goto err_gpio;
	}

	/* LDAC TGP pins */
	for (uint8_t i = 0; i < AD552XR_NUM_LDAC_TGP_PINS; i++) {
		ret = no_os_gpio_get_optional(&dev->gpio_ldac_tgp[i],
					      init_param->gpio_ldac_tgp[i]);
		if (ret)
			goto err_gpio;
	}

	/* Store the device information */
	dev->dev_info.type = init_param->type;
	dev->dev_addr = init_param->dev_addr;

	/* Select the device address */
	ret = ad552xr_assert_dev_addr_gpios(dev);
	if (ret)
		goto err_gpio;

	/* Get Device info */
	ret = ad552xr_get_device_info(dev);
	if (ret)
		goto err_gpio;

	/* Perform scratch pad register check */
	ret = ad552xr_check_scratch_pad(dev);
	if (ret)
		goto err_gpio;

	*device = dev;

	return 0;

err_gpio:
	ad552xr_remove_gpio(dev);
err_spi:
	no_os_spi_remove(dev->spi_desc);
err_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by ad552xr_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative value otherwise.
 */
int ad552xr_remove(struct ad552xr_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* Remove SPI descriptor */
	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	/* Remove GPIO descriptors */
	ret = ad552xr_remove_gpio(dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
