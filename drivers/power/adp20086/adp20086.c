/***************************************************************************//**
 *   @file   adp20086.c
 *   @brief  Source file for the ADP20086 driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "adp20086.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_crc8.h"

NO_OS_DECLARE_CRC8_TABLE(adp20086_crc8_table);

/**
 * @brief Interrupt source -> (mask register, bit).
 *
 * Indexed by enum adp20086_interrupt_source.
*/
struct adp20086_int_map {
	/** Mask register holding the bit. */
	uint8_t reg;
	/** Bit mask of the source within that register. */
	uint8_t mask;
};

static const struct adp20086_int_map adp20086_int_maps[] = {
	[ADP20086_INT_ACCM]	= { ADP20086_REG_MASK, ADP20086_MASK_ACCM },
	[ADP20086_INT_TSM] = { ADP20086_REG_MASK, ADP20086_MASK_TSM },
	[ADP20086_INT_VDDM] = { ADP20086_REG_MASK, ADP20086_MASK_VDD },
	[ADP20086_INT_VINM] = { ADP20086_REG_MASK, ADP20086_MASK_VIN },
	[ADP20086_INT_OCM] = { ADP20086_REG_MASK, ADP20086_MASK_OC },
	[ADP20086_INT_OVM] = { ADP20086_REG_MASK, ADP20086_MASK_OV },
	[ADP20086_INT_UVM] = { ADP20086_REG_MASK, ADP20086_MASK_UV },
	[ADP20086_INT_UV4M]	= { ADP20086_REG_MASK2, ADP20086_MASK2_UV4M },
	[ADP20086_INT_UV3M]	= { ADP20086_REG_MASK2, ADP20086_MASK2_UV3M },
	[ADP20086_INT_UV2M]	= { ADP20086_REG_MASK2, ADP20086_MASK2_UV2M },
	[ADP20086_INT_UV1M]	= { ADP20086_REG_MASK2, ADP20086_MASK2_UV1M },
	[ADP20086_INT_OPNM4] = { ADP20086_REG_MASK2, ADP20086_MASK2_OPNM4 },
	[ADP20086_INT_OPNM3] = { ADP20086_REG_MASK2, ADP20086_MASK2_OPNM3 },
	[ADP20086_INT_OPNM2] = { ADP20086_REG_MASK2, ADP20086_MASK2_OPNM2 },
	[ADP20086_INT_OPNM1] = { ADP20086_REG_MASK2, ADP20086_MASK2_OPNM1 },
	[ADP20086_INT_FCALM] = { ADP20086_REG_MASK3, ADP20086_MASK3_FCALM },
	[ADP20086_INT_BISTM] = { ADP20086_REG_MASK3, ADP20086_MASK3_BISTM },
	[ADP20086_INT_PARERRM] = { ADP20086_REG_MASK3, ADP20086_MASK3_PARERRM },
	[ADP20086_INT_ECC_CORM]	= { ADP20086_REG_MASK3, ADP20086_MASK3_ECC_CORM },
	[ADP20086_INT_ECC_DETM]	= { ADP20086_REG_MASK3, ADP20086_MASK3_ECC_DETM },
};

/**
 * @brief Per-channel enable feature -> (register, offset of the 4-bit field).
 *
 * Indexed by the enable half of enum adp20086_parameters.
*/
struct adp20086_chan_feature_map {
	/** Register holding the enable field. */
	uint8_t reg;
	/** Bit offset of channel 1 within that register. */
	uint8_t shift;
};

static const struct adp20086_chan_feature_map adp20086_chan_feature_maps[] = {
	[ADP20086_PARAM_CHANNELS] = { ADP20086_REG_CONFIG, 0 },
	[ADP20086_PARAM_LOAD_DETECTION]	= { ADP20086_REG_LOAD_INT_EN, 4 },
	[ADP20086_PARAM_OPEN_PROTECTION] = { ADP20086_REG_LOAD_INT_EN, 0 },
};

/**
 * @brief Threshold parameter -> register layout and scaling.
 *
 * Indexed by the threshold half of enum adp20086_parameters.
*/
struct adp20086_param_map {
	/** Register of channel 1, or the single register when global. */
	uint8_t reg_base;
	/** Weight of one code, in micro-units. */
	uint32_t lsb;
	/** Largest settable value, in micro-units. */
	uint32_t max;
	/** True when the parameter is device-wide rather than per channel. */
	bool global;
};

static const struct adp20086_param_map adp20086_param_maps[] = {
	[ADP20086_PARAM_LDET]	= {
		ADP20086_REG_LDET_SET(ADP20086_CHANNEL1),
		ADP20086_LDET_LSB_UA,
		ADP20086_MAX_LDET_UA,
		false,
	},
	[ADP20086_PARAM_OVIN]	= {
		ADP20086_REG_OVIN_SET,
		ADP20086_VIN_LSB_UV,
		ADP20086_MAX_VIN_UV,
		true,
	},
	[ADP20086_PARAM_UVIN]	= {
		ADP20086_REG_UVIN_SET,
		ADP20086_VIN_LSB_UV,
		ADP20086_MAX_VIN_UV,
		true,
	},
	[ADP20086_PARAM_IOPEN]	= {
		ADP20086_REG_IOPEN_SET(ADP20086_CHANNEL1),
		ADP20086_IOPEN_LSB_UA,
		ADP20086_MAX_IOPEN_UA,
		false,
	},
	[ADP20086_PARAM_UVOUT]	= {
		ADP20086_REG_UVOUT_SET(ADP20086_CHANNEL1),
		ADP20086_VOUT_LSB_UV,
		ADP20086_MAX_VOUT_UV,
		false,
	},
};

/* internal helpers */
static int adp20086_resolve_chan_feature(enum adp20086_channel ch,
		enum adp20086_parameters param, uint8_t *reg, uint8_t *mask);
static const struct adp20086_param_map *adp20086_resolve_param(
	enum adp20086_parameters param);
static int adp20086_resolve_param_reg(enum adp20086_channel ch,
				      const struct adp20086_param_map *map, uint8_t *reg);
static int adp20086_enable_channel_param(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param);
static int adp20086_disable_channel_param(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param);
static int adp20086_get_channel_param_status(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *status);
static int adp20086_read_adc_code(struct adp20086_dev *dev, uint8_t reg,
				  uint8_t *code);
static int adp20086_read_adc_microunits(struct adp20086_dev *dev, uint8_t reg,
					uint32_t lsb, uint32_t *val_micro);
static int adp20086_set_param(struct adp20086_dev *dev,
			      enum adp20086_channel ch, enum adp20086_parameters param, uint32_t value);
static int adp20086_get_param_code(struct adp20086_dev *dev,
				   enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *code);
static int adp20086_get_param_microunits(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param,
		uint32_t *value_micro);

/**
 * @brief INTB falling-edge handler
 *
 * Runs in interrupt context, so it does no bus traffic: it masks the pin and
 * records that a fault is pending. The work is done later, in thread context,
 * by adp20086_process_interrupt().
 * @param ctx - Opaque context, expected to be a struct adp20086_dev pointer
*/
static void adp20086_intb_handler(void *ctx)
{
	struct adp20086_dev *dev = ctx;

	if (!dev)
		return;

	no_os_irq_disable(dev->irq_ctrl, dev->intb_gpio_desc->number);
	dev->int_pending = true;
}

/**
 * @brief Initialize the ADP20086 device descriptor
 *
 * Brings up the I2C bus, the optional EN pin and the optional INTB pin,
 * latches PECE, part ID and revision from the ID register, and fails with
 * -ENODEV if the device reports a self-test fault.
 *
 * When both intb_gpio_ip and irq_ctrl are supplied the INTB interrupt is
 * registered but left disabled; the application arms it with
 * adp20086_int_enable() once it has registered a callback.
 * @param dev - Pointer to the ADP20086 device descriptor to allocate
 * @param adp20086_ip - ADP20086 initialization parameter
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_init(struct adp20086_dev **dev,
		  struct adp20086_init_param *adp20086_ip)
{
	struct adp20086_dev *device;
	uint8_t id;
	int ret;

	if (!dev || !adp20086_ip)
		return -EINVAL;

	no_os_crc8_populate_msb(adp20086_crc8_table, ADP20086_CRC8_POLY);

	device = (struct adp20086_dev *)no_os_calloc(sizeof(*device), 1);
	if (!device)
		return -ENOMEM;

	ret = no_os_i2c_init(&device->i2c_desc, adp20086_ip->i2c_ip);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get_optional(&device->en_gpio_desc, adp20086_ip->en_gpio_ip);
	if (ret)
		goto error_i2c;

	if (device->en_gpio_desc) {
		ret = no_os_gpio_direction_output(device->en_gpio_desc, NO_OS_GPIO_LOW);
		if (ret)
			goto error_en;
	}

	ret = no_os_gpio_get_optional(&device->intb_gpio_desc,
				      adp20086_ip->intb_gpio_ip);
	if (ret)
		goto error_en;

	if (device->intb_gpio_desc) {
		ret = no_os_gpio_direction_input(device->intb_gpio_desc);
		if (ret)
			goto error_intb;

		if (adp20086_ip->irq_ctrl) {
			struct no_os_callback_desc irq_cb = {
				.callback = adp20086_intb_handler,
				.ctx = device,
				.event = NO_OS_EVT_GPIO,
				.peripheral = NO_OS_GPIO_IRQ,
			};

			ret = no_os_irq_register_callback(adp20086_ip->irq_ctrl,
							  device->intb_gpio_desc->number,
							  &irq_cb);
			if (ret)
				goto error_intb;

			device->irq_cb = irq_cb;
			device->irq_ctrl = adp20086_ip->irq_ctrl;

			ret = no_os_irq_trigger_level_set(device->irq_ctrl,
							  device->intb_gpio_desc->number,
							  NO_OS_IRQ_EDGE_FALLING);
			if (ret)
				goto error_irq;

			ret = no_os_irq_disable(device->irq_ctrl,
						device->intb_gpio_desc->number);
			if (ret)
				goto error_irq;
		}
	}

	ret = adp20086_read(device, ADP20086_REG_ID, &id);
	if (ret)
		goto error_irq;

	device->pece = no_os_field_get(ADP20086_ID_PECE, id);
	device->part_id = no_os_field_get(ADP20086_ID_ID, id);
	device->revision = no_os_field_get(ADP20086_ID_REV, id);

	ret = adp20086_check_self_test(device);
	if (ret)
		goto error_irq;

	*dev = device;

	return 0;

error_irq:
	if (device->irq_ctrl)
		no_os_irq_unregister_callback(device->irq_ctrl,
					      device->intb_gpio_desc->number,
					      &device->irq_cb);
error_intb:
	no_os_gpio_remove(device->intb_gpio_desc);
error_en:
	no_os_gpio_remove(device->en_gpio_desc);
error_i2c:
	no_os_i2c_remove(device->i2c_desc);
error_dev:
	no_os_free(device);

	return ret;
}

/**
 * @brief Free the resources allocated by adp20086_init()
 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_remove(struct adp20086_dev *dev)
{
	int ret, err = 0;

	if (!dev)
		return -EINVAL;

	if (dev->irq_ctrl && dev->intb_gpio_desc) {
		ret = no_os_irq_disable(dev->irq_ctrl, dev->intb_gpio_desc->number);
		if (ret && !err)
		    err = ret;

		ret = no_os_irq_unregister_callback(dev->irq_ctrl,
					      dev->intb_gpio_desc->number,
					      &dev->irq_cb);
		if (ret && !err)
		    err = ret;
	}

	ret = no_os_gpio_remove(dev->intb_gpio_desc);
	if (ret && !err)
	    err = ret;

	ret = no_os_gpio_remove(dev->en_gpio_desc);
	if (ret && !err)
	    err = ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret && !err)
		return ret;

	no_os_free(dev);

	return err;
}

/**
 * @brief Drive the ADP20086 EN pin
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to drive EN high, false to drive it low
 * @return 0 in case of success, -EINVAL if EN is not driven by a GPIO,
 *	   negative error code otherwise
*/
int adp20086_set_gpio_en(struct adp20086_dev *dev, bool enabled)
{
	if (!dev || !dev->en_gpio_desc)
		return -EINVAL;

	return no_os_gpio_set_value(dev->en_gpio_desc, enabled);
}

/**
 * @brief Read back the state of the ADP20086 EN pin
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the EN pin state
 * @return 0 in case of success, -EINVAL if EN is not driven by a GPIO,
 *	   negative error code otherwise
*/
int adp20086_get_gpio_en(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t value;
	int ret;

	if (!dev || !dev->en_gpio_desc || !enabled)
		return -EINVAL;

	ret = no_os_gpio_get_value(dev->en_gpio_desc, &value);
	if (ret)
		return ret;

	*enabled = (value == NO_OS_GPIO_HIGH);

	return 0;
}

/**
 * @brief Read back the state of the ADP20086 INTB pin
 *
 * INTB is an open-drain, active-low output, so a pin reading of 0 means a
 * fault is being signalled.
 * @param dev - ADP20086 device descriptor
 * @param asserted - Pointer to store the INTB state; true when a fault is
 *	   being signalled
 * @return 0 in case of success, -EINVAL if INTB is not wired, negative error
 *	   code otherwise
*/
int adp20086_get_intb(struct adp20086_dev *dev, bool *asserted)
{
	uint8_t value;
	int ret;

	if (!dev || !dev->intb_gpio_desc || !asserted)
		return -EINVAL;

	ret = no_os_gpio_get_value(dev->intb_gpio_desc, &value);
	if (ret)
		return ret;

	*asserted = (value == NO_OS_GPIO_LOW);

	return 0;
}

/**
 * @brief Register the callback invoked by adp20086_process_interrupt()
 * @param dev - ADP20086 device descriptor
 * @param cb - Callback to invoke on a fault, or NULL to deregister
 * @param ctx - Opaque context passed back to the callback
 * @return 0 in case of success, -EINVAL otherwise
*/
int adp20086_set_int_callback(struct adp20086_dev *dev, adp20086_int_cb_t cb,
			      void *ctx)
{
	if (!dev)
		return -EINVAL;

	dev->int_cb = cb;
	dev->int_cb_ctx = ctx;

	return 0;
}

/**
 * @brief Arm the INTB interrupt
 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, -EINVAL if INTB has no interrupt controller,
 *	   negative error code otherwise
*/
int adp20086_enable_intb(struct adp20086_dev *dev)
{
	if (!dev || !dev->irq_ctrl || !dev->intb_gpio_desc)
		return -EINVAL;

	return no_os_irq_enable(dev->irq_ctrl, dev->intb_gpio_desc->number);
}

/**
 * @brief Disarm the INTB interrupt
 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, -EINVAL if INTB has no interrupt controller,
 *	   negative error code otherwise
*/
int adp20086_disable_intb(struct adp20086_dev *dev)
{
	if (!dev || !dev->irq_ctrl || !dev->intb_gpio_desc)
		return -EINVAL;

	return no_os_irq_disable(dev->irq_ctrl, dev->intb_gpio_desc->number);
}

/**
 * @brief Service a pending INTB assertion
 *
 * This is the bottom half of the INTB interrupt and must be called from the
 * application main loop. It decodes the fault status, hands it to the
 * registered callback and re-arms the interrupt. It is a no-op when no
 * interrupt is pending, so it is cheap to call unconditionally.
 *
 * Reading the status registers also clears the latched faults, because
 * CONFIG.CLR ("clear faults on read") is set out of reset.
 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_process_interrupt(struct adp20086_dev *dev)
{
	struct adp20086_device_status dev_status;
	struct adp20086_channel_status ch_status;
	bool asserted;
	int ret;

	if (!dev)
		return -EINVAL;

	if (!dev->int_pending)
		return 0;

	/*
	 * Safe to clear up front: the handler masked the pin, so it cannot run
	 * again until this function re-enables it below.
	 */
	dev->int_pending = false;

	ret = adp20086_get_device_status(dev, &dev_status);
	if (ret)
		goto rearm;

	ret = adp20086_get_channel_status(dev, &ch_status);
	if (ret)
		goto rearm;

	if (dev->int_cb)
		dev->int_cb(dev, &dev_status, &ch_status, dev->int_cb_ctx);

rearm:
	adp20086_enable_intb(dev);

	/*
	 * INTB is level-held, so a fault latched between the status read and
	 * the re-enable above leaves the line low without producing a fresh
	 * falling edge. Sample the pin once and requeue if it is still
	 * asserted, otherwise that fault would go unreported.
	 */
	if (!adp20086_get_intb(dev, &asserted) && asserted)
		dev->int_pending = true;

	return ret;
}

/**
 * @brief Read data from ADP20086
 *
 * When PEC is enabled the device appends a CRC-8 byte, which is verified
 * against the address/register/address/data frame before the data is returned.
 * @param dev - ADP20086 device descriptor
 * @param reg - 8-bit ADP20086 register address
 * @param data - Buffer with received data
 * @return 0 in case of success, -EIO on PEC mismatch, negative error code
 *	   otherwise
*/
int adp20086_read(struct adp20086_dev *dev, uint8_t reg, uint8_t *data)
{
	uint8_t vals[ADP20086_RD_FRAME_SIZE];
	int num_bytes = 1;
	uint8_t crc;
	int ret;

	if (!dev)
		return -EINVAL;

	vals[0] = dev->i2c_desc->slave_address << 1;
	vals[1] = reg;
	vals[2] = (dev->i2c_desc->slave_address << 1) | 0x1;


	ret = no_os_i2c_write(dev->i2c_desc, &vals[1], num_bytes, 0);
	if (ret)
		return ret;

	// If PEC enabled, change num_bytes (1-byte data, 1-byte PEC)
	if (dev->pece)
		num_bytes = 2;

	ret = no_os_i2c_read(dev->i2c_desc, &vals[3], num_bytes, 1);
	if (ret)
		return ret;

	if (dev->pece) {
		crc = no_os_crc8(adp20086_crc8_table, vals, ADP20086_RD_FRAME_SIZE - 1, 0);
		if (vals[4] != crc)
			return -EIO;
	}

	*data = vals[3];

	return 0;
}

/**
 * @brief Write data to ADP20086
 *
 * When PEC is enabled a CRC-8 byte over the address/register/data frame is
 * appended to the transfer.
 * @param dev - ADP20086 device descriptor
 * @param reg - 8-bit ADP20086 register address
 * @param data - Data byte value to write to the ADP20086
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_write(struct adp20086_dev *dev, uint8_t reg, uint8_t data)
{
	uint8_t vals[ADP20086_WR_FRAME_SIZE];
	int num_bytes = 2;

	if (!dev)
		return -EINVAL;

	vals[0] = dev->i2c_desc->slave_address << 1;
	vals[1] = reg;
	vals[2] = data;

	// If PEC enabled, change num_bytes (1-byte register, 1-byte data, 1-byte PEC)
	if (dev->pece) {
		num_bytes = 3;
		vals[3] = no_os_crc8(adp20086_crc8_table, vals, ADP20086_WR_FRAME_SIZE - 1, 0);
	}

	return no_os_i2c_write(dev->i2c_desc, &vals[1], num_bytes, 1);
}

/**
 * @brief Update specific register bits of ADP20086 through a specified mask
 * @param dev - ADP20086 device descriptor
 * @param reg - 8-bit ADP20086 register address
 * @param mask - 8-bit register mask for target bit changes
 * @param data - Field value to write, unshifted
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_update_register(struct adp20086_dev *dev, uint8_t reg,
			     uint8_t mask, uint8_t data)
{
	int ret;
	uint8_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, reg, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= no_os_field_prep(mask, data);

	return adp20086_write(dev, reg, reg_data);
}

/**
 * @brief Read an ADP20086 register and extract the masked field
 * @param dev - ADP20086 device descriptor
 * @param reg - 8-bit ADP20086 register address
 * @param mask - 8-bit register mask of the target field
 * @param data - Pointer to store the extracted field value, unshifted
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_register_field(struct adp20086_dev *dev, uint8_t reg,
				uint8_t mask, uint8_t *data)
{
	int ret;
	uint8_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, reg, &reg_data);
	if (ret)
		return ret;

	*data = no_os_field_get(mask, reg_data);

	return 0;
}

/**
 * @brief Select the source routed to the legacy ADC registers
 * @param dev - ADP20086 device descriptor
 * @param config - Mux selection
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_configure_mux(struct adp20086_dev *dev,
			   enum adp20086_legacy_adc_mux_config config)
{
	if (!dev || config < ADP20086_LEGACY_MUX_IOUT
	    || config > ADP20086_LEGACY_MUX_VIN_VDD)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_MUX,
					(uint8_t)config);
}

/**
 * @brief Enable/disable the ADC conversions (CONFIG.ENC)
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to enable conversions
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_configure_enc(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_ENC,
					(uint8_t)enabled);
}

/**
 * @brief Enable/disable clear-on-read of the status registers (CONFIG.CLR)
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to clear the latched status bits on read
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_configure_clr(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_CLR,
					(uint8_t)enabled);
}

/**
 * @brief Read back the legacy ADC mux selection
 * @param dev - ADP20086 device descriptor
 * @param config - Pointer to store the mux selection
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_mux_config(struct adp20086_dev *dev,
			    enum adp20086_legacy_adc_mux_config *config)
{
	uint8_t val;
	int ret;

	if (!dev || !config)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_MUX,
					  &val);
	if (ret)
		return ret;

	*config = val;

	return 0;
}

/**
 * @brief Read back the CONFIG.ENC bit
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the conversion enable state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_enc_config(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_ENC,
					  &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

/**
 * @brief Read back the CONFIG.CLR bit
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the clear-on-read state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_clr_config(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_CLR,
					  &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

/**
 * @brief Enable one output channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to enable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_enable_channel(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_CHANNELS);
}

/**
 * @brief Disable one output channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to disable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_disable_channel(struct adp20086_dev *dev, enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_CHANNELS);
}

/**
 * @brief Read back the channel enable bit, or the whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query, or ADP20086_ALL_CHANNELS for the whole field
 * @param status - Pointer to store the enable state; a 4-bit mask with
 *		   LSB = channel 1 when querying all channels
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_enabled_channels(struct adp20086_dev *dev,
				  enum adp20086_channel ch, uint8_t *status)
{
	return adp20086_get_channel_param_status(dev, ch, ADP20086_PARAM_CHANNELS,
			status);
}

/**
 * @brief Enable load detection on one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to enable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_enable_load_detection(struct adp20086_dev *dev,
				   enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_LOAD_DETECTION);
}

/**
 * @brief Disable load detection on one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to disable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_disable_load_detection(struct adp20086_dev *dev,
				    enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_LOAD_DETECTION);
}

/**
 * @brief Read back the load-detection enable bit, or the whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query, or ADP20086_ALL_CHANNELS for the whole field
 * @param status - Pointer to store the enable state; a 4-bit mask with
 *		   LSB = channel 1 when querying all channels
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_enabled_load_detection(struct adp20086_dev *dev,
					enum adp20086_channel ch, uint8_t *status)
{
	return adp20086_get_channel_param_status(dev, ch, ADP20086_PARAM_LOAD_DETECTION,
			status);
}

/**
 * @brief Enable open-load protection on one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to enable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_enable_open_protection(struct adp20086_dev *dev,
				    enum adp20086_channel ch)
{
	return adp20086_enable_channel_param(dev, ch, ADP20086_PARAM_OPEN_PROTECTION);
}

/**
 * @brief Disable open-load protection on one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to disable, or ADP20086_ALL_CHANNELS
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_disable_open_protection(struct adp20086_dev *dev,
				     enum adp20086_channel ch)
{
	return adp20086_disable_channel_param(dev, ch, ADP20086_PARAM_OPEN_PROTECTION);
}

/**
 * @brief Read back the open-protection enable bit, or the whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query, or ADP20086_ALL_CHANNELS for the whole field
 * @param status - Pointer to store the enable state; a 4-bit mask with
 *		   LSB = channel 1 when querying all channels
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_enabled_open_protection(struct adp20086_dev *dev,
		enum adp20086_channel ch, uint8_t *status)
{
	return adp20086_get_channel_param_status(dev, ch,
			ADP20086_PARAM_OPEN_PROTECTION, status);
}

/**
 * @brief Resolve a per-channel feature enable to its register and bit mask
 *
 * ch may be ADP20086_ALL_CHANNELS, which selects the feature's whole 4-bit
 * field.
 * @param ch - Channel to resolve, or ADP20086_ALL_CHANNELS
 * @param param - Per-channel enable feature to resolve
 * @param reg - Pointer to store the register address
 * @param mask - Pointer to store the bit mask within that register
 * @return 0 in case of success, -EINVAL on an out-of-range channel or feature
*/
static int adp20086_resolve_chan_feature(enum adp20086_channel ch,
		enum adp20086_parameters param, uint8_t *reg, uint8_t *mask)
{
	const struct adp20086_chan_feature_map *map;

	if (ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	if (param < ADP20086_PARAM_CHANNELS ||
	    param >= NO_OS_ARRAY_SIZE(adp20086_chan_feature_maps))
		return -EINVAL;

	map = &adp20086_chan_feature_maps[param];
	*reg = map->reg;
	*mask = (ch == ADP20086_ALL_CHANNELS)
		? NO_OS_GENMASK(map->shift + 3, map->shift)
		: NO_OS_BIT(map->shift + ch);

	return 0;
}

/**
 * @brief Get the PEC enable state latched from ID.PECE at init
 * @param dev - ADP20086 device descriptor
 * @param pec_enabled - Pointer to store the PEC enable state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_pece(struct adp20086_dev *dev, bool *pec_enabled)
{
	if (!dev || !pec_enabled)
		return -EINVAL;

	*pec_enabled = dev->pece;

	return 0;
}

/**
 * @brief Get the part ID read back at init
 * @param dev - ADP20086 device descriptor
 * @param part_id - Pointer to store the part ID
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_part_id(struct adp20086_dev *dev, uint8_t *part_id)
{
	if (!dev || !part_id)
		return -EINVAL;

	*part_id = dev->part_id;

	return 0;
}

/**
 * @brief Get the silicon revision read back at init
 * @param dev - ADP20086 device descriptor
 * @param revision - Pointer to store the revision
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_revision(struct adp20086_dev *dev, uint8_t *revision)
{
	if (!dev || !revision)
		return -EINVAL;

	*revision = dev->revision;

	return 0;
}

/**
 * @brief Mask or unmask a single interrupt source
 * @param dev - ADP20086 device descriptor
 * @param src - Interrupt source to configure
 * @param masked - True to mask the source, false to let it assert INTB
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_interrupt_mask(struct adp20086_dev *dev,
				enum adp20086_interrupt_source src, bool masked)
{
	if (!dev || src < 0 || src >= NO_OS_ARRAY_SIZE(adp20086_int_maps))
		return -EINVAL;

	return adp20086_update_register(dev, adp20086_int_maps[src].reg,
					adp20086_int_maps[src].mask, (uint8_t) masked);
}

/**
 * @brief Restore mask registers to their datasheet defaults

 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_reset_masks(struct adp20086_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_write(dev, ADP20086_REG_MASK, ADP20086_REG_MASK_DEFAULT);
	if (ret)
		return ret;

	ret = adp20086_write(dev, ADP20086_REG_MASK2, ADP20086_REG_MASK2_DEFAULT);
	if (ret)
		return ret;

	ret = adp20086_write(dev, ADP20086_REG_MASK3, ADP20086_REG_MASK3_DEFAULT);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Enable/disable the overvoltage self-test
 *
 * The self-test may only be armed while every output channel is disabled, so
 * enabling it with any channel on is rejected.
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to arm the overvoltage self-test
 * @return 0 in case of success, -EINVAL if a channel is still enabled,
 *	   negative error code otherwise
*/
int adp20086_set_ovtst(struct adp20086_dev *dev, bool enabled)
{
	int ret;
	uint8_t channels_enabled;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_CONFIG, &channels_enabled);
	if (ret)
		return ret;

	channels_enabled = no_os_field_get(ADP20086_CONFIG_EN_MASK, channels_enabled);

	if (channels_enabled && enabled)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK, ADP20086_MASK_OVTST,
					(uint8_t) enabled);
}

/**
 * @brief Enable/disable the ADC IIR filter
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to enable the filter
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_iir_filter(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3, ADP20086_MASK3_IIR_EN,
					(uint8_t) enabled);
}

/**
 * @brief Enable/disable the output discharge resistors
 * @param dev - ADP20086 device descriptor
 * @param enabled - True to connect the discharge resistors
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_discharge_resistors(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3,
					ADP20086_MASK3_DISCH_EN, (uint8_t) enabled);
}

/**
 * @brief Read back the mask bit of a single interrupt source
 * @param dev - ADP20086 device descriptor
 * @param src - Interrupt source to query
 * @param masked - Pointer to store the mask state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_interrupt_mask(struct adp20086_dev *dev,
				enum adp20086_interrupt_source src, bool *masked)
{
	uint8_t val;
	int ret;

	if (!dev || !masked || src < 0 ||
	    src >= NO_OS_ARRAY_SIZE(adp20086_int_maps))
		return -EINVAL;

	ret = adp20086_get_register_field(dev, adp20086_int_maps[src].reg,
					  adp20086_int_maps[src].mask, &val);
	if (ret)
		return ret;

	*masked = val;

	return 0;
}

/**
 * @brief Read back the overvoltage self-test enable bit
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the self-test enable state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ovtst(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK, ADP20086_MASK_OVTST,
					  &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

/**
 * @brief Read back the ADC IIR filter enable bit
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the filter enable state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_iir_filter(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK3,
					  ADP20086_MASK3_IIR_EN, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

/**
 * @brief Read back the output discharge resistor enable bit
 * @param dev - ADP20086 device descriptor
 * @param enabled - Pointer to store the discharge resistor enable state
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_discharge_resistors(struct adp20086_dev *dev, bool *enabled)
{
	uint8_t val;
	int ret;

	if (!dev || !enabled)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_MASK3,
					  ADP20086_MASK3_DISCH_EN, &val);
	if (ret)
		return ret;

	*enabled = val;

	return 0;
}

/**
 * @brief Read the per-channel fault status of all four channels
 *
 * Collapses STAT2 (CH1/CH2), STAT3 (CH3/CH4) and STAT4 into one 4-bit mask per
 * fault type, with LSB = channel 1.
 * @param dev - ADP20086 device descriptor
 * @param status - Pointer to store the decoded per-channel status
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_channel_status(struct adp20086_dev *dev,
				struct adp20086_channel_status *status)
{
	uint8_t ts = 0, oc = 0, ov = 0, uv = 0;
	uint8_t stat2, stat3, stat4, stat;
	int ret;
	int ch;

	if (!dev || !status)
		return -EINVAL;

	/*
	 * Read all four channels' status in one pass: STAT2 (CH1/CH2),
	 * STAT3 (CH3/CH4) and STAT4 (load/open for all four).
	 */
	ret = adp20086_read(dev, ADP20086_REG_STAT2, &stat2);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT3, &stat3);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT4, &stat4);
	if (ret)
		return ret;

	/* Build a 4-bit mask per fault type, LSB = channel 1. */
	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		stat = (ch < ADP20086_CHANNEL3) ? stat2 : stat3;

		ts |= !!no_os_field_get(ADP20086_CHAN_STATUS_TS(ch), stat) << ch;
		oc |= !!no_os_field_get(ADP20086_CHAN_STATUS_OC(ch), stat) << ch;
		ov |= !!no_os_field_get(ADP20086_CHAN_STATUS_OV(ch), stat) << ch;
		uv |= !!no_os_field_get(ADP20086_CHAN_STATUS_UV(ch), stat) << ch;
	}

	status->thermal_shutdown = ts;
	status->overcurrent = oc;
	status->overvoltage = ov;
	status->undervoltage = uv;
	/* STAT4 already holds all four channels' bits, LSB = channel 1. */
	status->load_detected = no_os_field_get(NO_OS_GENMASK(7, 4), stat4);
	status->open_load = no_os_field_get(NO_OS_GENMASK(3, 0), stat4);

	return 0;
}

/**
 * @brief Test whether a channel is flagged in one of the status masks
 *
 * Saves callers from knowing that struct adp20086_channel_status holds 4-bit
 * masks with the LSB = channel 1.
 * @param mask - Any mask field of struct adp20086_channel_status
 * @param ch - Channel to test, or ADP20086_ALL_CHANNELS to test for any
 * @return true if the channel is flagged, false otherwise
*/
bool adp20086_channel_asserted(uint8_t mask, enum adp20086_channel ch)
{
	if (ch == ADP20086_ALL_CHANNELS)
		return !!(mask & NO_OS_GENMASK(ADP20086_CHANNEL4,
					       ADP20086_CHANNEL1));

	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return false;

	return !!(mask & NO_OS_BIT(ch));
}

/**
 * @brief Read the device-wide status from STAT1 and STAT5
 * @param dev - ADP20086 device descriptor
 * @param status - Pointer to store the decoded device status
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_device_status(struct adp20086_dev *dev,
			       struct adp20086_device_status *status)
{
	struct adp20086_device_status st;
	uint8_t stat1, stat5;
	int ret;

	if (!dev || !status)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_STAT1, &stat1);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT5, &stat5);
	if (ret)
		return ret;

	st.tlim_serv = no_os_field_get(ADP20086_STAT1_TLIM_SERV, stat1);
	st.acc = no_os_field_get(ADP20086_STAT1_ACC, stat1);
	st.ovin = no_os_field_get(ADP20086_STAT1_OVIN, stat1);
	st.uvin = no_os_field_get(ADP20086_STAT1_UVIN, stat1);
	st.ovdd = no_os_field_get(ADP20086_STAT1_OVDD, stat1);
	st.uvdd = no_os_field_get(ADP20086_STAT1_UVDD, stat1);
	st.adc_fcal = no_os_field_get(ADP20086_STAT5_ADC_FCAL, stat5);
	st.intbsts = no_os_field_get(ADP20086_STAT5_INTBSTS, stat5);
	st.bist = no_os_field_get(ADP20086_STAT5_BIST, stat5);
	st.parerr = no_os_field_get(ADP20086_STAT5_PARERR, stat5);
	st.ecc_cor = no_os_field_get(ADP20086_STAT5_ECC_COR, stat5);
	st.ecc_det = no_os_field_get(ADP20086_STAT5_ECC_DET, stat5);

	*status = st;

	return 0;
}

/**
 * @brief Flush the latched device status registers
 *
 * CONFIG.CLR is temporarily set if needed so the read actually clears the
 * latches, then restored, leaving the configuration unchanged.
 * @param dev - ADP20086 device descriptor
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_clear_device_status(struct adp20086_dev *dev)
{
	bool clr;
	uint8_t dummy;
	int ret;

	if (!dev)
		return -EINVAL;

	/*
	 * The STAT registers are clear-on-read, but only while CONFIG.CLR is
	 * set. Make sure it is enabled for the flush, then restore the original
	 * setting so this call has no lasting side effect on the configuration.
	 */
	ret = adp20086_get_clr_config(dev, &clr);
	if (ret)
		return ret;

	if (!clr) {
		ret = adp20086_configure_clr(dev, true);
		if (ret)
			return ret;
	}

	/* Reading the device status registers clears the latched faults. */
	ret = adp20086_read(dev, ADP20086_REG_STAT1, &dummy);
	if (ret)
		goto restore_clr;

	ret = adp20086_read(dev, ADP20086_REG_STAT5, &dummy);

restore_clr:
	if (!clr) {
		int ret2 = adp20086_configure_clr(dev, false);
		if (!ret)
			ret = ret2;
	}

	return ret;
}

/**
 * @brief Check STAT5 for a fatal self-test or memory integrity fault
 *
 * A corrected ECC error (ECC_COR) is not treated as a failure.
 * @param dev - ADP20086 device descriptor
 * @return 0 if the device passed, -ENODEV on a fatal fault, negative error
 *	   code otherwise
*/
int adp20086_check_self_test(struct adp20086_dev *dev)
{
	uint8_t stat5;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_STAT5, &stat5);
	if (ret)
		return ret;

	if (stat5 & ADP20086_SELFTEST_FAULT_MASK)
		return -ENODEV;

	return 0;
}

/**
 * @brief Read one legacy MAX20087-compatible ADC register
 *
 * The quantity reported depends on the mux selection made with
 * adp20086_configure_mux().
 * @param dev - ADP20086 device descriptor
 * @param adc - Legacy ADC register to read
 * @param data - Pointer to store the raw ADC code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_legacy_adc(struct adp20086_dev *dev,
			     enum adp20086_legacy_adc adc, uint8_t *data)
{
	if (!dev || !data || adc < ADP20086_ADC1_LEGACY || adc > ADP20086_ADC4_LEGACY)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_ADC_LEGACY(adc), data);
}

/**
 * @brief Set the current limit of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param ilim - Current limit code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_ilim(struct adp20086_dev *dev, enum adp20086_channel ch,
		      enum adp20086_ilim_available ilim)
{
	int ret;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4
	    || ilim < ADP20086_ILIM_52MA || ilim > ADP20086_ILIM_832MA)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS) {
		for (int c = 0; c <= ADP20086_CHANNEL4; c++) {
			ret = adp20086_update_register(dev, ADP20086_REG_ILIM(c), ADP20086_ILIM_MASK(c),
						       ilim);
			if (ret)
				return ret;
		}

		return 0;
	}

	return adp20086_update_register(dev, ADP20086_REG_ILIM(ch),
					ADP20086_ILIM_MASK(ch), ilim);
}

/**
 * @brief Read back the raw current limit code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param code - Pointer to store the current limit code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ilim_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			   uint8_t *code)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !code)
		return -EINVAL;

	return adp20086_get_register_field(dev, ADP20086_REG_ILIM(ch),
					   ADP20086_ILIM_MASK(ch), code);
}

/**
 * @brief Read back the current limit of a channel, in microamps
 *
 * Code 0 corresponds to one LSB, so the limit is (code + 1) steps.
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param ilim_ua - Pointer to store the current limit, in microamps
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ilim_ua(struct adp20086_dev *dev, enum adp20086_channel ch,
			 uint32_t *ilim_ua)
{
	int ret;
	uint8_t code;

	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !ilim_ua)
		return -EINVAL;

	ret = adp20086_get_ilim_code(dev, ch, &code);
	if (ret)
		return ret;

	/* ILIM is 0-indexed, 52ma @ code 0b0000 */
	*ilim_ua = (code + 1) * ADP20086_ILIM_LSB_UA;

	return 0;
}

/**
 * @brief Set the load-detect threshold of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param threshold_ua - Threshold in microamps, truncated to the nearest code
 *			 below; must be within one LSB and ADP20086_MAX_LDET_UA
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_ldet_threshold(struct adp20086_dev *dev,
				enum adp20086_channel ch, uint32_t threshold_ua)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_LDET, threshold_ua);
}

/**
 * @brief Read back the raw load-detect threshold code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ldet_threshold_code(struct adp20086_dev *dev,
				     enum adp20086_channel ch, uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_LDET, code);
}

/**
 * @brief Read back the load-detect threshold of a channel, in microamps
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param threshold_ua - Pointer to store the threshold, in microamps
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ldet_threshold_ua(struct adp20086_dev *dev,
				   enum adp20086_channel ch, uint32_t *threshold_ua)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_LDET,
					     threshold_ua);
}

/**
 * @brief Set the input overvoltage threshold
 * @param dev - ADP20086 device descriptor
 * @param ovin_uv - Threshold in microvolts, truncated to the nearest code
 *		    below; must be within one LSB and ADP20086_MAX_VIN_UV
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_ovin(struct adp20086_dev *dev, uint32_t ovin_uv)
{
	return adp20086_set_param(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_OVIN,
				  ovin_uv);
}

/**
 * @brief Read back the raw input overvoltage threshold code
 * @param dev - ADP20086 device descriptor
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ovin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_get_param_code(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_OVIN,
				       code);
}

/**
 * @brief Read back the input overvoltage threshold, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param ovin_uv - Pointer to store the threshold, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_ovin_uv(struct adp20086_dev *dev, uint32_t *ovin_uv)
{
	return adp20086_get_param_microunits(dev, ADP20086_ALL_CHANNELS,
					     ADP20086_PARAM_OVIN, ovin_uv);
}

/**
 * @brief Set the input undervoltage threshold
 * @param dev - ADP20086 device descriptor
 * @param uvin_uv - Threshold in microvolts, truncated to the nearest code
 *		    below; must be within one LSB and ADP20086_MAX_VIN_UV
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_uvin(struct adp20086_dev *dev, uint32_t uvin_uv)
{
	return adp20086_set_param(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_UVIN,
				  uvin_uv);
}

/**
 * @brief Read back the raw input undervoltage threshold code
 * @param dev - ADP20086 device descriptor
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_uvin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_get_param_code(dev, ADP20086_ALL_CHANNELS, ADP20086_PARAM_UVIN,
				       code);
}

/**
 * @brief Read back the input undervoltage threshold, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param uvin_uv - Pointer to store the threshold, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_uvin_uv(struct adp20086_dev *dev, uint32_t *uvin_uv)
{
	return adp20086_get_param_microunits(dev, ADP20086_ALL_CHANNELS,
					     ADP20086_PARAM_UVIN, uvin_uv);
}

/**
 * @brief Set the open-load current threshold of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param iopen_ua - Threshold in microamps, truncated to the nearest code
 *		     below; must be within one LSB and ADP20086_MAX_IOPEN_UA
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_iopen(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t iopen_ua)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_IOPEN, iopen_ua);
}

/**
 * @brief Read back the raw open-load current threshold code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_iopen_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_IOPEN, code);
}

/**
 * @brief Read back the open-load current threshold of a channel, in microamps
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param iopen_ua - Pointer to store the threshold, in microamps
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_iopen_ua(struct adp20086_dev *dev, enum adp20086_channel ch,
			  uint32_t *iopen_ua)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_IOPEN, iopen_ua);
}

/**
 * @brief Set the output undervoltage threshold of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param uvout_uv - Threshold in microvolts, truncated to the nearest code
 *		     below; must be within one LSB and ADP20086_MAX_VOUT_UV
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_uvout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t uvout_uv)
{
	return adp20086_set_param(dev, ch, ADP20086_PARAM_UVOUT, uvout_uv);
}

/**
 * @brief Read back the raw output undervoltage threshold code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_uvout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	return adp20086_get_param_code(dev, ch, ADP20086_PARAM_UVOUT, code);
}

/**
 * @brief Read back the output undervoltage threshold of a channel, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query
 * @param uvout_uv - Pointer to store the threshold, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_uvout_uv(struct adp20086_dev *dev, enum adp20086_channel ch,
			  uint32_t *uvout_uv)
{
	return adp20086_get_param_microunits(dev, ch, ADP20086_PARAM_UVOUT, uvout_uv);
}

/**
 * @brief Set the soft-start ramp of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param ssu - Soft-start ramp time
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch,
			    enum adp20086_soft_start ssu)
{
	uint8_t data = 0;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4
	    || ssu < ADP20086_SSU_0R5MS || ssu > ADP20086_SSU_4MS)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS) {
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++) {
			data |= no_os_field_prep(ADP20086_START_SLEW_MASK(c), ssu);
		}
		return adp20086_write(dev, ADP20086_REG_START_SLEW, data);
	}

	return adp20086_update_register(dev, ADP20086_REG_START_SLEW,
					ADP20086_START_SLEW_MASK(ch), ssu);
}

/**
 * @brief Read back the soft-start ramp of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query; ADP20086_ALL_CHANNELS is not accepted
 * @param ssu - Pointer to store the soft-start ramp time
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_soft_start(struct adp20086_dev *dev, enum adp20086_channel ch,
			    enum adp20086_soft_start *ssu)
{
	uint8_t val;
	int ret;

	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !ssu)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_START_SLEW,
					  ADP20086_START_SLEW_MASK(ch), &val);
	if (ret)
		return ret;

	*ssu = val;

	return 0;
}


/**
 * @brief Set the soft-shutdown ramp of one channel, or all of them
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param ssd - Soft-shutdown ramp time
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_set_soft_shutdown(struct adp20086_dev *dev,
			       enum adp20086_channel ch, enum adp20086_soft_shutdown ssd)
{
	uint8_t data = 0;

	if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4
	    || ssd < ADP20086_SSD_0R25MS || ssd > ADP20086_SSD_2MS)
		return -EINVAL;

	if (ch == ADP20086_ALL_CHANNELS) {
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++) {
			data |= no_os_field_prep(ADP20086_SHDN_SLEW_MASK(c), ssd);
		}
		return adp20086_write(dev, ADP20086_REG_SHDN_SLEW, data);
	}

	return adp20086_update_register(dev, ADP20086_REG_SHDN_SLEW,
					ADP20086_SHDN_SLEW_MASK(ch), ssd);
}

/**
 * @brief Read back the soft-shutdown ramp of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query; ADP20086_ALL_CHANNELS is not accepted
 * @param ssd - Pointer to store the soft-shutdown ramp time
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_get_soft_shutdown(struct adp20086_dev *dev,
			       enum adp20086_channel ch, enum adp20086_soft_shutdown *ssd)
{
	uint8_t val;
	int ret;

	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4 || !ssd)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_SHDN_SLEW,
					  ADP20086_SHDN_SLEW_MASK(ch), &val);
	if (ret)
		return ret;

	*ssd = val;

	return 0;
}

/**
 * @brief Read the raw input voltage ADC code
 * @param dev - ADP20086 device descriptor
 * @param code - Pointer to store the ADC code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VIN_READ, code);
}

/**
 * @brief Read the input voltage, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param vin_uv - Pointer to store the input voltage, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vin_uv(struct adp20086_dev *dev, uint32_t *vin_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VIN_READ,
					    ADP20086_VIN_LSB_UV, vin_uv);
}

/**
 * @brief Read the raw output voltage ADC code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to read
 * @param code - Pointer to store the ADC code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_code(dev, ADP20086_REG_VOUT_READ(ch), code);
}

/**
 * @brief Read the output voltage of a channel, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to read
 * @param vout_uv - Pointer to store the output voltage, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vout_uv(struct adp20086_dev *dev, enum adp20086_channel ch,
			  uint32_t *vout_uv)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_VOUT_READ(ch),
					    ADP20086_VOUT_LSB_UV, vout_uv);
}

/**
 * @brief Read the raw output current ADC code of a channel
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to read
 * @param code - Pointer to store the ADC code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_iout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_code(dev, ADP20086_REG_IOUT_READ(ch), code);
}

/**
 * @brief Read the output current of a channel, in microamps
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to read
 * @param iout_ua - Pointer to store the output current, in microamps
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_iout_ua(struct adp20086_dev *dev, enum adp20086_channel ch,
			  uint32_t *iout_ua)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_IOUT_READ(ch),
					    ADP20086_IOUT_LSB_UA, iout_ua);
}

/**
 * @brief Read the raw VDD supply ADC code
 * @param dev - ADP20086 device descriptor
 * @param code - Pointer to store the ADC code
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vdd_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VDD_READ, code);
}

/**
 * @brief Read the VDD supply voltage, in microvolts
 * @param dev - ADP20086 device descriptor
 * @param vdd_uv - Pointer to store the supply voltage, in microvolts
 * @return 0 in case of success, negative error code otherwise
*/
int adp20086_read_vdd_uv(struct adp20086_dev *dev, uint32_t *vdd_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VDD_READ,
					    ADP20086_VDD_LSB_UV, vdd_uv);
}

/**
 * @brief Read back a per-channel feature enable bit, or its whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query, or ADP20086_ALL_CHANNELS for the whole field
 * @param param - Per-channel enable feature to query
 * @param status - Pointer to store the enable state
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_get_channel_param_status(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *status)
{
	uint8_t reg, mask;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_resolve_chan_feature(ch, param, &reg, &mask);
	if (ret)
		return ret;

	return adp20086_get_register_field(dev, reg, mask, status);
}

/**
 * @brief Set a per-channel feature enable bit, or its whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to enable, or ADP20086_ALL_CHANNELS
 * @param param - Per-channel enable feature to set
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_enable_channel_param(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param)
{
	uint8_t reg, mask, enabled;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_resolve_chan_feature(ch, param, &reg, &mask);
	if (ret)
		return ret;

	enabled = (ch == ADP20086_ALL_CHANNELS) ? ADP20086_ALL_CH_ASSERTED
		  : ADP20086_SINGLE_CH_ASSERTED;

	return adp20086_update_register(dev, reg, mask, enabled);
}

/**
 * @brief Clear a per-channel feature enable bit, or its whole 4-bit field
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to disable, or ADP20086_ALL_CHANNELS
 * @param param - Per-channel enable feature to clear
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_disable_channel_param(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param)
{
	uint8_t reg, mask;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adp20086_resolve_chan_feature(ch, param, &reg, &mask);
	if (ret)
		return ret;

	return adp20086_update_register(dev, reg, mask, false);
}

/**
 * @brief Read a raw ADC code from a telemetry register
 * @param dev - ADP20086 device descriptor
 * @param reg - Telemetry register address
 * @param code - Pointer to store the ADC code
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_read_adc_code(struct adp20086_dev *dev, uint8_t reg,
				  uint8_t *code)
{
	if (!dev || !code)
		return -EINVAL;

	return adp20086_read(dev, reg, code);
}

/**
 * @brief Read a telemetry register and scale the code to micro-units
 * @param dev - ADP20086 device descriptor
 * @param reg - Telemetry register address
 * @param lsb - Weight of one code, in micro-units
 * @param val_micro - Pointer to store the scaled value, in micro-units
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_read_adc_microunits(struct adp20086_dev *dev, uint8_t reg,
					uint32_t lsb, uint32_t *val_micro)
{
	uint8_t code;
	int ret;

	if (!dev || !val_micro)
		return -EINVAL;

	ret = adp20086_read(dev, reg, &code);
	if (ret)
		return ret;

	*val_micro = code * lsb;

	return 0;
}

/**
 * @brief Set a threshold parameter from a value in micro-units
 *
 * The value is truncated down to the nearest code. Global parameters ignore
 * ch; per-channel ones accept ADP20086_ALL_CHANNELS to write every output.
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to configure, or ADP20086_ALL_CHANNELS
 * @param param - Threshold parameter to set
 * @param value - Threshold in micro-units; must be within one LSB and the
 *		  parameter's maximum
 * @return 0 in case of success, -EINVAL if the value is out of range,
 *	   negative error code otherwise
*/
static int adp20086_set_param(struct adp20086_dev *dev,
			      enum adp20086_channel ch, enum adp20086_parameters param, uint32_t value)
{
	const struct adp20086_param_map *map;
	uint8_t code;
	int ret;

	map = adp20086_resolve_param(param);
	if (!map)
		return -EINVAL;

	if (!dev || value > map->max)
		return -EINVAL;

	code = value / map->lsb;

	if (map->global)
		return adp20086_write(dev, map->reg_base, code);

	if (ch == ADP20086_ALL_CHANNELS) {
		for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++) {
			ret = adp20086_write(dev, map->reg_base + c, code);
			if (ret)
				return ret;
		}

		return 0;
	}

	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_write(dev, map->reg_base + ch, code);
}

/**
 * @brief Look up the register layout of a threshold parameter
 * @param param - Threshold parameter to resolve
 * @return Pointer to the parameter map, or NULL if param is not a threshold
*/
static const struct adp20086_param_map *adp20086_resolve_param(
	enum adp20086_parameters param)
{
	if (param < ADP20086_PARAM_LDET || param > ADP20086_PARAM_UVOUT)
		return NULL;

	return &adp20086_param_maps[param];
}

/**
 * @brief Resolve the register holding a threshold parameter for a channel
 * @param ch - Channel to resolve; ignored for global parameters
 * @param map - Parameter map returned by adp20086_resolve_param()
 * @param reg - Pointer to store the register address
 * @return 0 in case of success, -EINVAL on an out-of-range channel
*/
static int adp20086_resolve_param_reg(enum adp20086_channel ch,
				      const struct adp20086_param_map *map, uint8_t *reg)
{
	if (map->global) {
		*reg = map->reg_base;

		return 0;
	}

	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	*reg = map->reg_base + ch;

	return 0;
}

/**
 * @brief Read back the raw code of a threshold parameter
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query; ignored for global parameters
 * @param param - Threshold parameter to query
 * @param code - Pointer to store the threshold code
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_get_param_code(struct adp20086_dev *dev,
				   enum adp20086_channel ch, enum adp20086_parameters param, uint8_t *code)
{
	const struct adp20086_param_map *map;
	uint8_t reg;
	int ret;

	if (!dev || !code)
		return -EINVAL;

	map = adp20086_resolve_param(param);
	if (!map)
		return -EINVAL;

	ret = adp20086_resolve_param_reg(ch, map, &reg);
	if (ret)
		return ret;

	return adp20086_read(dev, reg, code);
}

/**
 * @brief Read back a threshold parameter scaled to micro-units
 * @param dev - ADP20086 device descriptor
 * @param ch - Channel to query; ignored for global parameters
 * @param param - Threshold parameter to query
 * @param value_micro - Pointer to store the threshold, in micro-units
 * @return 0 in case of success, negative error code otherwise
*/
static int adp20086_get_param_microunits(struct adp20086_dev *dev,
		enum adp20086_channel ch, enum adp20086_parameters param, uint32_t *value_micro)
{
	const struct adp20086_param_map *map;
	uint8_t reg, code;
	int ret;

	if (!dev || !value_micro)
		return -EINVAL;

	map = adp20086_resolve_param(param);
	if (!map)
		return -EINVAL;

	ret = adp20086_resolve_param_reg(ch, map, &reg);
	if (ret)
		return ret;

	ret = adp20086_read(dev, reg, &code);
	if (ret)
		return ret;

	*value_micro = code * map->lsb;

	return 0;
}
