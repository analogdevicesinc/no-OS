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
 * @brief Location of a single-bit field within a device register.
 */
struct adp20086_field_map {
	uint8_t reg;
	uint8_t mask;
};

/**
 * @brief MASK register bit of every interrupt source, indexed by
 *	  enum adp20086_interrupt_source.
 */
static const struct adp20086_field_map adp20086_int_maps[] = {
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
 * @brief STAT register bit of every status flag, indexed by
 *	  enum adp20086_status_field.
 */
static const struct adp20086_field_map adp20086_status_maps[] = {
	[ADP20086_STATUS_TLIM_SERV] = { ADP20086_REG_STAT1, ADP20086_STAT1_TLIM_SERV },
	[ADP20086_STATUS_ACC] = { ADP20086_REG_STAT1, ADP20086_STAT1_ACC },
	[ADP20086_STATUS_OVIN] = { ADP20086_REG_STAT1, ADP20086_STAT1_OVIN },
	[ADP20086_STATUS_UVIN] = { ADP20086_REG_STAT1, ADP20086_STAT1_UVIN },
	[ADP20086_STATUS_OVDD] = { ADP20086_REG_STAT1, ADP20086_STAT1_OVDD },
	[ADP20086_STATUS_UVDD] = { ADP20086_REG_STAT1, ADP20086_STAT1_UVDD },
	[ADP20086_STATUS_TSOUT1] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_TS(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_TSOUT2] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_TS(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_TSOUT3] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_TS(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_TSOUT4] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_TS(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_OCOUT1] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_OC(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_OCOUT2] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_OC(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_OCOUT3] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_OC(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_OCOUT4] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_OC(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_OVOUT1] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_OV(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_OVOUT2] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_OV(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_OVOUT3] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_OV(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_OVOUT4] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_OV(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_UVOUT1] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_UV(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_UVOUT2] = { ADP20086_REG_STAT2, ADP20086_CHAN_STATUS_UV(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_UVOUT3] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_UV(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_UVOUT4] = { ADP20086_REG_STAT3, ADP20086_CHAN_STATUS_UV(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_LDET1] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_LOAD_DET(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_LDET2] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_LOAD_DET(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_LDET3] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_LOAD_DET(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_LDET4] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_LOAD_DET(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_OPEN1] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_OPEN(ADP20086_CHANNEL1) },
	[ADP20086_STATUS_OPEN2] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_OPEN(ADP20086_CHANNEL2) },
	[ADP20086_STATUS_OPEN3] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_OPEN(ADP20086_CHANNEL3) },
	[ADP20086_STATUS_OPEN4] = { ADP20086_REG_STAT4, ADP20086_CHAN_STATUS_OPEN(ADP20086_CHANNEL4) },
	[ADP20086_STATUS_ADCFCAL] = { ADP20086_REG_STAT5, ADP20086_STAT5_ADC_FCAL },
	[ADP20086_STATUS_INTBSTS] = { ADP20086_REG_STAT5, ADP20086_STAT5_INTBSTS },
	[ADP20086_STATUS_BIST] = { ADP20086_REG_STAT5, ADP20086_STAT5_BIST },
	[ADP20086_STATUS_PARERR] = { ADP20086_REG_STAT5, ADP20086_STAT5_PARERR },
	[ADP20086_STATUS_ECC_COR] = { ADP20086_REG_STAT5, ADP20086_STAT5_ECC_COR },
	[ADP20086_STATUS_ECC_DET] = { ADP20086_REG_STAT5, ADP20086_STAT5_ECC_DET },
};

/**
 * @brief INTB assertion handler.
 *
 * Disables the GPIO interrupt and records the assertion. The fault itself is
 * read back from task context by adp20086_process_interrupt(), which re-arms
 * the interrupt; keeping the handler this short avoids running I2C traffic in
 * interrupt context.
 *
 * @param ctx - The ADP20086 device descriptor, passed as callback context.
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
 * @brief Read the raw code of a telemetry ADC register.
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Telemetry register address.
 * @param code - Pointer to store the 8-bit ADC code.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adp20086_read_adc_code(struct adp20086_dev *dev, uint8_t reg,
				  uint8_t *code)
{
	if (!dev || !code)
		return -EINVAL;

	return adp20086_read(dev, reg, code);
}

/**
 * @brief Read a telemetry ADC register and scale its code to micro-units.
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Telemetry register address.
 * @param lsb - Weight of one ADC count, in micro-units.
 * @param val_micro - Pointer to store the scaled value, in micro-units.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Initialize the ADP20086 device.
 *
 * Allocates the descriptor, brings up the I2C bus and the optional EN and INTB
 * GPIOs, reads the ID register to latch the PEC enable, part ID and revision,
 * then checks the power-on self-test result. EN is driven high by default so
 * that outputs OTP programmed to start enabled do not immediately assert INTB.
 * When an IRQ controller is supplied, the INTB callback is registered for
 * falling edges but left disabled; call adp20086_arm_interrupt() to enable it.
 *
 * @param dev - Pointer to store the allocated device descriptor.
 * @param adp20086_ip - The ADP20086 initialization parameter.
 * @return 0 in case of success, -ENODEV if the self-test reports a fault,
 *	   negative error code otherwise.
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
		// Default to HIGH to avoid triggering INTB due to channels OTP programmed to be enabled by default.
		ret = no_os_gpio_direction_output(device->en_gpio_desc, NO_OS_GPIO_HIGH);
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
 * @brief Free the resources allocated by adp20086_init().
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_remove(struct adp20086_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->irq_ctrl && dev->intb_gpio_desc) {
		ret = no_os_irq_disable(dev->irq_ctrl, dev->intb_gpio_desc->number);
		if (ret)
			return ret;

		ret = no_os_irq_unregister_callback(dev->irq_ctrl,
						    dev->intb_gpio_desc->number,
						    &dev->irq_cb);
		if (ret)
			return ret;
	}

	ret = no_os_gpio_remove(dev->intb_gpio_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->en_gpio_desc);
	if (ret)
		return ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Drive the EN pin of the device.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to drive EN high, false to drive it low.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_gpio_en(struct adp20086_dev *dev, bool enabled)
{
	if (!dev || !dev->en_gpio_desc)
		return -EINVAL;

	return no_os_gpio_set_value(dev->en_gpio_desc, enabled);
}

/**
 * @brief Read back the state of the EN pin.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the state; true when EN is high.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Read the state of the INTB pin.
 * @param dev - The ADP20086 device descriptor.
 * @param asserted - Pointer to store the state; true when INTB is low, which
 *		     is the asserted level.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Register the callback run when an INTB assertion is processed.
 *
 * The callback is invoked from adp20086_process_interrupt(), in task context,
 * with the status snapshot that was read after the assertion.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param cb - Callback to run, or NULL to remove the current one.
 * @param ctx - User context passed back to the callback.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_interrupt_callback(struct adp20086_dev *dev,
				    adp20086_int_cb_t cb,
				    void *ctx)
{
	if (!dev)
		return -EINVAL;

	dev->int_cb = cb;
	dev->int_cb_ctx = ctx;

	return 0;
}

/**
 * @brief Enable the INTB GPIO interrupt.
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_arm_interrupt(struct adp20086_dev *dev)
{
	int ret;

	if (!dev || !dev->irq_ctrl || !dev->intb_gpio_desc)
		return -EINVAL;

	ret = no_os_irq_enable(dev->irq_ctrl, dev->intb_gpio_desc->number);
	if (ret)
		return ret;

	dev->int_armed = true;

	return 0;
}

/**
 * @brief Disable the INTB GPIO interrupt.
 *
 * A disarm requested while an assertion is still pending stays in effect:
 * adp20086_process_interrupt() will not re-arm the interrupt afterwards.
 *
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_disarm_interrupt(struct adp20086_dev *dev)
{
	int ret;

	if (!dev || !dev->irq_ctrl || !dev->intb_gpio_desc)
		return -EINVAL;

	ret = no_os_irq_disable(dev->irq_ctrl, dev->intb_gpio_desc->number);
	if (ret)
		return ret;

	dev->int_armed = false;

	return 0;
}

/**
 * @brief Get whether the INTB interrupt is currently armed.
 * @param dev - The ADP20086 device descriptor.
 * @param armed - Pointer to store the armed state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_interrupt_armed(struct adp20086_dev *dev, bool *armed)
{
	if (!dev || !armed)
		return -EINVAL;

	*armed = dev->int_armed;

	return 0;
}

/**
 * @brief Service a pending INTB assertion.
 *
 * Does nothing when no assertion is pending. Otherwise reads a status
 * snapshot, hands it to the registered callback, and re-arms the interrupt
 * unless it was disarmed in the meantime. If INTB is still asserted after
 * re-arming - the fault has not cleared - the assertion is marked pending
 * again so the next call picks it up. Call this from the main loop.
 *
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_process_interrupt(struct adp20086_dev *dev)
{
	struct adp20086_status status;
	bool asserted;
	int ret, ret2;

	if (!dev)
		return -EINVAL;

	if (!dev->int_pending)
		return 0;

	dev->int_pending = false;

	ret = adp20086_get_status(dev, &status);
	if (ret)
		goto rearm;

	if (dev->int_cb)
		dev->int_cb(dev, &status, dev->int_cb_ctx);

rearm:
	/* A disarm requested while an assertion was pending stays in effect. */
	if (!dev->int_armed)
		return ret;

	ret2 = adp20086_arm_interrupt(dev);
	if (ret2)
		return ret2;

	/* In case INTB fault is still present. */
	if (!adp20086_get_intb(dev, &asserted) && asserted)
		dev->int_pending = true;

	return ret;
}

/**
 * @brief Read one device register.
 *
 * When PEC is enabled the trailing CRC byte is read back and verified against
 * the frame, which includes both I2C addresses and the register pointer.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Register address.
 * @param data - Pointer to store the register value.
 * @return 0 in case of success, -EIO on a PEC mismatch, negative error code
 *	   otherwise.
 */
int adp20086_read(struct adp20086_dev *dev, uint8_t reg, uint8_t *data)
{
	uint8_t vals[ADP20086_RD_FRAME_SIZE];
	int num_bytes = 1;
	uint8_t crc;
	int ret;

	if (!dev || !data)
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
 * @brief Write one device register.
 *
 * When PEC is enabled a CRC byte computed over the address, register and data
 * is appended to the frame.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Register address.
 * @param data - Value to write.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Read-modify-write the masked field of a register.
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Register address.
 * @param mask - Mask of the field to update.
 * @param data - Field value, shifted into place by the mask.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Read the masked field of a register.
 * @param dev - The ADP20086 device descriptor.
 * @param reg - Register address.
 * @param mask - Mask of the field to read.
 * @param data - Pointer to store the field value, shifted down by the mask.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Select the source routed to the legacy ADC registers.
 *
 * Only affects the MAX20087-compatible path read by
 * adp20086_read_legacy_adc(); the dedicated telemetry registers are unaffected.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param config - Mux source to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_mux_config(struct adp20086_dev *dev,
			    enum adp20086_legacy_adc_mux_config config)
{
	if (!dev || config > ADP20086_LEGACY_MUX_VIN_VDD)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_MUX,
					(uint8_t)config);
}

/**
 * @brief Enable conversions for the legacy ADC path.
 *
 * Without ENC set the legacy ADC registers are not refreshed and a read
 * returns whatever the last conversion left behind.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable conversions, false to disable them.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_enc_config(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_ENC,
					(uint8_t)enabled);
}

/**
 * @brief Set the clear-on-read behaviour of the STAT registers.
 *
 * With CLR set, reading a STAT register clears its latched bits.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to clear status bits on read, false to keep them.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_clr_config(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG, ADP20086_CONFIG_CLR,
					(uint8_t)enabled);
}

/**
 * @brief Get the source currently routed to the legacy ADC registers.
 * @param dev - The ADP20086 device descriptor.
 * @param config - Pointer to store the mux source.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Get whether conversions are enabled for the legacy ADC path.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the ENC bit state.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Get the clear-on-read behaviour of the STAT registers.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the CLR bit state.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Enable or disable one output switch.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param enabled - true to enable the output, false to disable it.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_channel(struct adp20086_dev *dev, enum adp20086_channel ch,
			 bool enabled)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_CONFIG,
					ADP20086_CONFIG_EN(ch), enabled);
}

/**
 * @brief Enable or disable all four output switches in one write.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable every output, false to disable them all.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_channels(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	if (enabled)
		return adp20086_update_register(dev, ADP20086_REG_CONFIG,
						ADP20086_CONFIG_EN_MASK, ADP20086_ALL_CH_ASSERTED);

	return adp20086_update_register(dev, ADP20086_REG_CONFIG,
					ADP20086_CONFIG_EN_MASK, false);

}

/**
 * @brief Get which output switches are enabled.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled_channels - Pointer to store the per-output enable state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_enabled_channels(struct adp20086_dev *dev,
				  struct adp20086_enabled_channels *enabled_channels)
{
	struct adp20086_enabled_channels en_ch;
	uint8_t data;
	int ret;

	if (!dev || !enabled_channels)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_CONFIG,
					  ADP20086_CONFIG_EN_MASK, &data);
	if (ret)
		return ret;

	en_ch.out1 = no_os_field_get(ADP20086_CONFIG_EN(ADP20086_CHANNEL1),
				     (uint32_t) data);
	en_ch.out2 = no_os_field_get(ADP20086_CONFIG_EN(ADP20086_CHANNEL2),
				     (uint32_t) data);
	en_ch.out3 = no_os_field_get(ADP20086_CONFIG_EN(ADP20086_CHANNEL3),
				     (uint32_t) data);
	en_ch.out4 = no_os_field_get(ADP20086_CONFIG_EN(ADP20086_CHANNEL4),
				     (uint32_t) data);

	*enabled_channels = en_ch;

	return 0;
}

/**
 * @brief Enable or disable load detection on one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param enabled - true to enable load detection, false to disable it.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_load_detection(struct adp20086_dev *dev,
				enum adp20086_channel ch, bool enabled)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
					ADP20086_LDET_EN(ch), enabled);
}

/**
 * @brief Enable or disable load detection on all four outputs in one write.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable load detection everywhere, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_load_detection(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	if (enabled)
		return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
						ADP20086_LDET_EN_MASK, ADP20086_ALL_CH_ASSERTED);

	return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
					ADP20086_LDET_EN_MASK, false);
}

/**
 * @brief Get which outputs have load detection enabled.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled_channels - Pointer to store the per-output enable state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_enabled_load_detection(struct adp20086_dev *dev,
					struct adp20086_enabled_channels *enabled_channels)
{
	struct adp20086_enabled_channels en_ch;
	uint8_t data;
	int ret;

	if (!dev || !enabled_channels)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_LOAD_INT_EN,
					  ADP20086_LDET_EN_MASK, &data);
	if (ret)
		return ret;

	en_ch.out1 = no_os_field_get(ADP20086_LDET_EN(ADP20086_CHANNEL1),
				     (uint32_t) data);
	en_ch.out2 = no_os_field_get(ADP20086_LDET_EN(ADP20086_CHANNEL2),
				     (uint32_t) data);
	en_ch.out3 = no_os_field_get(ADP20086_LDET_EN(ADP20086_CHANNEL3),
				     (uint32_t) data);
	en_ch.out4 = no_os_field_get(ADP20086_LDET_EN(ADP20086_CHANNEL4),
				     (uint32_t) data);

	*enabled_channels = en_ch;

	return 0;
}

/**
 * @brief Enable or disable open-load protection on one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param enabled - true to enable open-load protection, false to disable it.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_open_protection(struct adp20086_dev *dev,
				 enum adp20086_channel ch, bool enabled)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
					ADP20086_OPEN_PROT(ch), enabled);
}

/**
 * @brief Enable or disable open-load protection on all outputs in one write.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable the protection everywhere, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_open_protection(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	if (enabled)
		return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
						ADP20086_OPEN_PROT_MASK, ADP20086_ALL_CH_ASSERTED);

	return adp20086_update_register(dev, ADP20086_REG_LOAD_INT_EN,
					ADP20086_OPEN_PROT_MASK, false);
}

/**
 * @brief Get which outputs have open-load protection enabled.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled_channels - Pointer to store the per-output enable state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_enabled_open_protection(struct adp20086_dev *dev,
		struct adp20086_enabled_channels *enabled_channels)
{
	struct adp20086_enabled_channels en_ch;
	uint8_t data;
	int ret;

	if (!dev || !enabled_channels)
		return -EINVAL;

	ret = adp20086_get_register_field(dev, ADP20086_REG_LOAD_INT_EN,
					  ADP20086_OPEN_PROT_MASK, &data);
	if (ret)
		return ret;

	en_ch.out1 = no_os_field_get(ADP20086_OPEN_PROT(ADP20086_CHANNEL1),
				     (uint32_t) data);
	en_ch.out2 = no_os_field_get(ADP20086_OPEN_PROT(ADP20086_CHANNEL2),
				     (uint32_t) data);
	en_ch.out3 = no_os_field_get(ADP20086_OPEN_PROT(ADP20086_CHANNEL3),
				     (uint32_t) data);
	en_ch.out4 = no_os_field_get(ADP20086_OPEN_PROT(ADP20086_CHANNEL4),
				     (uint32_t) data);

	*enabled_channels = en_ch;

	return 0;
}

/**
 * @brief Get whether packet error checking is enabled on this part.
 *
 * PEC is an OTP setting, read back from the ID register at init.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param pec_enabled - Pointer to store the PEC enable state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_pece(struct adp20086_dev *dev, bool *pec_enabled)
{
	if (!dev || !pec_enabled)
		return -EINVAL;

	*pec_enabled = dev->pece;

	return 0;
}

/**
 * @brief Get the part ID read back at init.
 * @param dev - The ADP20086 device descriptor.
 * @param part_id - Pointer to store the part ID.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_part_id(struct adp20086_dev *dev, uint8_t *part_id)
{
	if (!dev || !part_id)
		return -EINVAL;

	*part_id = dev->part_id;

	return 0;
}

/**
 * @brief Get the silicon revision read back at init.
 * @param dev - The ADP20086 device descriptor.
 * @param revision - Pointer to store the revision.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_revision(struct adp20086_dev *dev, uint8_t *revision)
{
	if (!dev || !revision)
		return -EINVAL;

	*revision = dev->revision;

	return 0;
}

/**
 * @brief Get the state of the overvoltage comparator self-test.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the OVTST bit state.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Run or stop the overvoltage comparator self-test.
 *
 * Setting OVTST forces the inputs of all four output OV comparators high, so a
 * working comparator answers by latching its OVOUT status bit. The datasheet
 * requires every output to be disabled first, so enabling the test while any
 * output is on is rejected rather than producing a meaningless result.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to start the self-test, false to end it.
 * @return 0 in case of success, -EINVAL if any output is enabled, negative
 *	   error code otherwise.
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
 * @brief Get whether the telemetry IIR filter is enabled.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the filter enable state.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Enable or disable the telemetry IIR filter.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable the filter, false to disable it.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_iir_filter(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3, ADP20086_MASK3_IIR_EN,
					(uint8_t) enabled);
}

/**
 * @brief Get whether the output discharge resistors are enabled.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - Pointer to store the discharge enable state.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Enable or disable the output discharge resistors.
 * @param dev - The ADP20086 device descriptor.
 * @param enabled - true to enable the resistors, false to disable them.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_discharge_resistors(struct adp20086_dev *dev, bool enabled)
{
	if (!dev)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_MASK3,
					ADP20086_MASK3_DISCH_EN, (uint8_t) enabled);
}

/**
 * @brief Get the mask bit of a single interrupt source.
 * @param dev - The ADP20086 device descriptor.
 * @param src - Interrupt source to query.
 * @param masked - Pointer to store the mask bit; true means the source is
 *		   masked and will not assert INTB. The matching status bit
 *		   still latches.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Set the mask bit of a single interrupt source.
 * @param dev - The ADP20086 device descriptor.
 * @param src - Interrupt source to configure.
 * @param masked - true to mask the source so it does not assert INTB, false
 *		   to let it through. Masking never stops the status bit from
 *		   latching.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Get the reset value of the interrupt masks.
 *
 * Matches the device defaults: every source unmasked except ACC.
 *
 * @return The default mask set.
 */
struct adp20086_masks adp20086_get_default_mask(void)
{
	struct adp20086_masks mask = { 0 };

	mask.accm = true;

	return mask;
}

/**
 * @brief Read back every interrupt mask bit in one pass.
 * @param dev - The ADP20086 device descriptor.
 * @param mask - Pointer to store the mask set.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_configured_interrupt_masks(struct adp20086_dev *dev,
		struct adp20086_masks *mask)
{
	uint8_t reg_mask, reg_mask2, reg_mask3;
	int ret;

	if (!dev || !mask)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_MASK, &reg_mask);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_MASK2, &reg_mask2);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_MASK3, &reg_mask3);
	if (ret)
		return ret;

	mask->accm = no_os_field_get(ADP20086_MASK_ACCM, reg_mask);
	mask->tsm = no_os_field_get(ADP20086_MASK_TSM, reg_mask);
	mask->vddm = no_os_field_get(ADP20086_MASK_VDD, reg_mask);
	mask->vinm = no_os_field_get(ADP20086_MASK_VIN, reg_mask);
	mask->ocm = no_os_field_get(ADP20086_MASK_OC, reg_mask);
	mask->ovm = no_os_field_get(ADP20086_MASK_OV, reg_mask);
	mask->uvm = no_os_field_get(ADP20086_MASK_UV, reg_mask);

	/* MASK2: UV1M..UV4M in bits 4..7, OPNM1..OPNM4 in bits 0..3. */
	for (int ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		mask->uvoutm[ch] = no_os_field_get(ADP20086_MASK2_UVM(ch), reg_mask2);
		mask->opnoutm[ch] = no_os_field_get(ADP20086_MASK2_OPNM(ch), reg_mask2);
	}

	mask->fcalm = no_os_field_get(ADP20086_MASK3_FCALM, reg_mask3);
	mask->bistm = no_os_field_get(ADP20086_MASK3_BISTM, reg_mask3);
	mask->parerrm = no_os_field_get(ADP20086_MASK3_PARERRM, reg_mask3);
	mask->ecc_corm = no_os_field_get(ADP20086_MASK3_ECC_CORM, reg_mask3);
	mask->ecc_detm = no_os_field_get(ADP20086_MASK3_ECC_DETM, reg_mask3);

	return 0;
}

/**
 * @brief Write every interrupt mask bit in one pass.
 *
 * MASK and MASK3 are read-modify-written so the IIR and discharge bits they
 * share are preserved; MASK2 holds only mask bits and is written outright.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param mask - Mask set to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_configure_interrupt_masks(struct adp20086_dev *dev,
				       struct adp20086_masks mask)
{
	uint8_t reg_mask, reg_mask2, reg_mask3;
	int ret;
	int ch;

	if (!dev)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_MASK, &reg_mask);
	if (ret)
		return ret;

	reg_mask &= ~(ADP20086_MASK_INTMASKS);
	reg_mask |= (mask.accm ? ADP20086_MASK_ACCM : 0) |
		    (mask.tsm ? ADP20086_MASK_TSM : 0) |
		    (mask.vddm ? ADP20086_MASK_VDD : 0) |
		    (mask.vinm ? ADP20086_MASK_VIN : 0) |
		    (mask.ocm ? ADP20086_MASK_OC : 0) |
		    (mask.ovm ? ADP20086_MASK_OV : 0) |
		    (mask.uvm ? ADP20086_MASK_UV : 0);

	ret = adp20086_write(dev, ADP20086_REG_MASK, reg_mask);
	if (ret)
		return ret;

	/* MASK2: UV1M..UV4M in bits 4..7, OPNM1..OPNM4 in bits 0..3. */
	reg_mask2 = 0;
	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		if (mask.uvoutm[ch])
			reg_mask2 |= ADP20086_MASK2_UVM(ch);
		if (mask.opnoutm[ch])
			reg_mask2 |= ADP20086_MASK2_OPNM(ch);
	}

	ret = adp20086_write(dev, ADP20086_REG_MASK2, reg_mask2);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_MASK3, &reg_mask3);
	if (ret)
		return ret;

	reg_mask3 &= ~(ADP20086_MASK3_INTMASKS);
	reg_mask3 |= (mask.fcalm ? ADP20086_MASK3_FCALM : 0) |
		     (mask.bistm ? ADP20086_MASK3_BISTM : 0) |
		     (mask.parerrm ? ADP20086_MASK3_PARERRM : 0) |
		     (mask.ecc_corm ? ADP20086_MASK3_ECC_CORM : 0) |
		     (mask.ecc_detm ? ADP20086_MASK3_ECC_DETM : 0);

	return adp20086_write(dev, ADP20086_REG_MASK3, reg_mask3);
}

/**
 * @brief Restore the interrupt masks to their reset values.
 *
 * Only the mask bits are touched; the IIR and discharge bits sharing MASK and
 * MASK3 keep their current values.
 *
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_reset_masks(struct adp20086_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/** Only reset INTB masks, not IIR and DISCH bits. */
	ret = adp20086_update_register(dev, ADP20086_REG_MASK,
				       ADP20086_MASK_INTMASKS,
				       ADP20086_REG_MASK_DEFAULT);
	if (ret)
		return ret;

	ret = adp20086_write(dev, ADP20086_REG_MASK2, ADP20086_REG_MASK2_DEFAULT);
	if (ret)
		return ret;

	/** Only reset INTB masks, not IIR and DISCH bits. */
	ret = adp20086_update_register(dev, ADP20086_REG_MASK3,
				       ADP20086_MASK3_INTMASKS,
				       ADP20086_REG_MASK3_DEFAULT);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read a snapshot of all five STAT registers.
 *
 * Note that this clears the latched bits when CLR is set; see
 * adp20086_set_clr_config().
 *
 * @param dev - The ADP20086 device descriptor.
 * @param status - Pointer to store the decoded status.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_status(struct adp20086_dev *dev,
			struct adp20086_status *status)
{
	uint8_t stat1, stat2, stat3, stat4, stat5, stat;
	int ret;
	int ch;

	if (!dev || !status)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_STAT1, &stat1);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT2, &stat2);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT3, &stat3);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT4, &stat4);
	if (ret)
		return ret;

	ret = adp20086_read(dev, ADP20086_REG_STAT5, &stat5);
	if (ret)
		return ret;

	status->tlim_serv = no_os_field_get(ADP20086_STAT1_TLIM_SERV, stat1);
	status->acc = no_os_field_get(ADP20086_STAT1_ACC, stat1);
	status->ovin = no_os_field_get(ADP20086_STAT1_OVIN, stat1);
	status->uvin = no_os_field_get(ADP20086_STAT1_UVIN, stat1);
	status->ovdd = no_os_field_get(ADP20086_STAT1_OVDD, stat1);
	status->uvdd = no_os_field_get(ADP20086_STAT1_UVDD, stat1);

	for (ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		stat = (ch < ADP20086_CHANNEL3) ? stat2 : stat3;

		status->thermal_shutdown[ch] = no_os_field_get(ADP20086_CHAN_STATUS_TS(ch),
					       stat);
		status->overcurrent[ch] = no_os_field_get(ADP20086_CHAN_STATUS_OC(ch), stat);
		status->ovout[ch] = no_os_field_get(ADP20086_CHAN_STATUS_OV(ch), stat);
		status->uvout[ch] = no_os_field_get(ADP20086_CHAN_STATUS_UV(ch), stat);
		status->load_detected[ch] = no_os_field_get(ADP20086_CHAN_STATUS_LOAD_DET(ch),
					    stat4);
		status->open_load[ch] = no_os_field_get(ADP20086_CHAN_STATUS_OPEN(ch), stat4);
	}

	status->adc_fcal = no_os_field_get(ADP20086_STAT5_ADC_FCAL, stat5);
	status->intbsts = no_os_field_get(ADP20086_STAT5_INTBSTS, stat5);
	status->bist = no_os_field_get(ADP20086_STAT5_BIST, stat5);
	status->parerr = no_os_field_get(ADP20086_STAT5_PARERR, stat5);
	status->ecc_cor = no_os_field_get(ADP20086_STAT5_ECC_COR, stat5);
	status->ecc_det = no_os_field_get(ADP20086_STAT5_ECC_DET, stat5);

	return 0;
}

/**
 * @brief Read a single status flag.
 *
 * Only the one STAT register holding the flag is read, which keeps the other
 * latched bits intact when CLR is set.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param field - Status flag to read.
 * @param asserted - Pointer to store the flag state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_status_field(struct adp20086_dev *dev,
			      enum adp20086_status_field field, bool *asserted)
{
	uint8_t reg_data;
	int ret;

	if (!dev || !asserted || field < 0
	    || field >= NO_OS_ARRAY_SIZE(adp20086_status_maps))
		return -EINVAL;

	struct adp20086_field_map map = adp20086_status_maps[field];

	ret = adp20086_read(dev, map.reg, &reg_data);
	if (ret)
		return ret;

	*asserted =  no_os_field_get(map.mask, reg_data);

	return 0;
}

/**
 * @brief Clear every latched status bit.
 *
 * Sets CLR if it is not already set, reads all STAT registers to clear them,
 * then restores the original CLR setting.
 *
 * @param dev - The ADP20086 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_clear_status(struct adp20086_dev *dev)
{
	bool clr;
	int ret, ret2 = 0;
	struct adp20086_status status;

	if (!dev)
		return -EINVAL;

	ret = adp20086_get_clr_config(dev, &clr);
	if (ret)
		return ret;

	if (!clr) {
		ret = adp20086_set_clr_config(dev, true);
		if (ret)
			return ret;
	}

	ret = adp20086_get_status(dev, &status);

	if (!clr)
		ret2 = adp20086_set_clr_config(dev, false);

	if (ret)
		return ret;

	if (ret2)
		return ret2;

	return 0;
}

/**
 * @brief Check the power-on self-test and memory integrity flags.
 *
 * Fails on an ADC calibration, BIST, parity or uncorrectable ECC fault. A
 * corrected ECC error is not treated as a failure.
 *
 * @param dev - The ADP20086 device descriptor.
 * @return 0 if the device passed, -ENODEV if a fault is latched, negative
 *	   error code otherwise.
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
 * @brief Read one of the legacy MAX20087-compatible ADC registers.
 *
 * The quantity behind a register depends on the mux setting, and under the
 * VIN/VDD setting it is not even uniform across the four, so only the raw
 * code is returned. Set ENC first or the registers are not refreshed. Use the
 * dedicated telemetry reads for a calibrated value.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param adc - Legacy ADC register to read.
 * @param data - Pointer to store the raw code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_legacy_adc(struct adp20086_dev *dev,
			     enum adp20086_legacy_adc adc, uint8_t *data)
{
	if (!dev || !data || adc < ADP20086_ADC1_LEGACY || adc > ADP20086_ADC4_LEGACY)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_ADC_LEGACY(adc), data);
}

/**
 * @brief Set the current limit of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param ilim - Current limit step to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_ilim(struct adp20086_dev *dev, enum adp20086_channel ch,
		      enum adp20086_ilim_available ilim)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4
	    || ilim < ADP20086_ILIM_52MA || ilim > ADP20086_ILIM_832MA)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_ILIM(ch),
					ADP20086_ILIM_MASK(ch), ilim);
}

/**
 * @brief Set the same current limit on all four outputs.
 * @param dev - The ADP20086 device descriptor.
 * @param ilim - Current limit step to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_ilim(struct adp20086_dev *dev,
			  enum adp20086_ilim_available ilim)
{
	int ret;

	if (!dev || ilim < ADP20086_ILIM_52MA || ilim > ADP20086_ILIM_832MA)
		return -EINVAL;

	for (int ch = ADP20086_CHANNEL1; ch <= ADP20086_CHANNEL4; ch++) {
		ret = adp20086_update_register(dev, ADP20086_REG_ILIM(ch),
					       ADP20086_ILIM_MASK(ch),
					       ilim);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Get the raw current limit code of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param code - Pointer to store the ILIM field value.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Get the current limit of one output, in microamps.
 *
 * The ILIM field is zero-indexed: code 0 already means one full step.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param ilim_ua - Pointer to store the current limit, in microamps.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_ilim(struct adp20086_dev *dev, enum adp20086_channel ch,
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
 * @brief Get the raw load detection threshold code of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param code - Pointer to store the threshold code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_ldet_threshold_code(struct adp20086_dev *dev,
				     enum adp20086_channel ch, uint8_t *code)
{
	if (!dev || !code || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_LDET_SET(ch), code);
}

/**
 * @brief Get the load detection threshold of one output, in microamps.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param threshold_ua - Pointer to store the threshold, in microamps.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_ldet_threshold(struct adp20086_dev *dev,
				enum adp20086_channel ch, uint32_t *threshold_ua)
{
	uint8_t code;
	int ret;

	if (!dev || !threshold_ua || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_LDET_SET(ch), &code);
	if (ret)
		return ret;

	*threshold_ua = code * ADP20086_LDET_LSB_UA;

	return 0;
}

/**
 * @brief Set the load detection threshold of one output.
 *
 * The request is truncated to the next lower representable step.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param threshold_ua - Threshold in microamps, up to ADP20086_MAX_LDET_UA.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_ldet_threshold(struct adp20086_dev *dev,
				enum adp20086_channel ch, uint32_t threshold_ua)
{
	uint8_t code;

	if (!dev || threshold_ua > ADP20086_MAX_LDET_UA || ch < ADP20086_CHANNEL1
	    || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	code = threshold_ua / ADP20086_LDET_LSB_UA;

	return adp20086_write(dev, ADP20086_REG_LDET_SET(ch), code);
}

/**
 * @brief Get the raw input overvoltage threshold code.
 * @param dev - The ADP20086 device descriptor.
 * @param code - Pointer to store the threshold code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_ovin_code(struct adp20086_dev *dev, uint8_t *code)
{
	if (!dev || !code)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_OVIN_SET, code);
}

/**
 * @brief Get the input overvoltage threshold, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param ovin_uv - Pointer to store the threshold, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_ovin(struct adp20086_dev *dev, uint32_t *ovin_uv)
{
	uint8_t code;
	int ret;

	if (!dev || !ovin_uv)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_OVIN_SET, &code);
	if (ret)
		return ret;

	*ovin_uv = code * ADP20086_VIN_LSB_UV;

	return 0;
}

/**
 * @brief Set the input overvoltage threshold.
 *
 * Truncated to the next lower step, so the device trips no later than the
 * requested voltage.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param ovin_uv - Threshold in microvolts, up to ADP20086_MAX_VIN_UV.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_ovin(struct adp20086_dev *dev, uint32_t ovin_uv)
{
	uint8_t code;

	if (!dev || ovin_uv > ADP20086_MAX_VIN_UV)
		return -EINVAL;

	code = ovin_uv / ADP20086_VIN_LSB_UV;

	return adp20086_write(dev, ADP20086_REG_OVIN_SET, code);
}

/**
 * @brief Get the raw input undervoltage threshold code.
 * @param dev - The ADP20086 device descriptor.
 * @param code - Pointer to store the threshold code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_uvin_code(struct adp20086_dev *dev, uint8_t *code)
{
	if (!dev || !code)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_UVIN_SET, code);
}

/**
 * @brief Get the input undervoltage threshold, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param uvin_uv - Pointer to store the threshold, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_uvin(struct adp20086_dev *dev, uint32_t *uvin_uv)
{
	uint8_t code;
	int ret;

	if (!dev || !uvin_uv)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_UVIN_SET, &code);
	if (ret)
		return ret;

	*uvin_uv = code * ADP20086_VIN_LSB_UV;

	return 0;
}

/**
 * @brief Set the input undervoltage threshold.
 *
 * Rounded up to the next step, so the device trips no later than the
 * requested voltage.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param uvin_uv - Threshold in microvolts, up to ADP20086_MAX_VIN_UV.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_uvin(struct adp20086_dev *dev, uint32_t uvin_uv)
{
	uint8_t code;

	if (!dev || uvin_uv > ADP20086_MAX_VIN_UV)
		return -EINVAL;

	code = NO_OS_DIV_ROUND_UP(uvin_uv, ADP20086_VIN_LSB_UV);

	return adp20086_write(dev, ADP20086_REG_UVIN_SET, code);
}

/**
 * @brief Get the raw open-load current threshold code of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param code - Pointer to store the threshold code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_iopen_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (!dev || !code || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_IOPEN_SET(ch), code);
}

/**
 * @brief Get the open-load current threshold of one output, in microamps.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param iopen_ua - Pointer to store the threshold, in microamps.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_iopen(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *iopen_ua)
{
	uint8_t code;
	int ret;

	if (!dev || !iopen_ua || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_IOPEN_SET(ch), &code);
	if (ret)
		return ret;

	*iopen_ua = code * ADP20086_IOPEN_LSB_UA;

	return 0;
}

/**
 * @brief Set the open-load current threshold of one output.
 *
 * Rounded up to the next representable step.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param iopen_ua - Threshold in microamps, up to ADP20086_MAX_IOPEN_UA.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_iopen(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t iopen_ua)
{
	uint8_t code;

	if (!dev || iopen_ua > ADP20086_MAX_IOPEN_UA || ch < ADP20086_CHANNEL1
	    || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	code = NO_OS_DIV_ROUND_UP(iopen_ua, ADP20086_IOPEN_LSB_UA);

	return adp20086_write(dev, ADP20086_REG_IOPEN_SET(ch), code);
}

/**
 * @brief Get the raw output undervoltage threshold code of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param code - Pointer to store the threshold code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_uvout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (!dev || !code || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read(dev, ADP20086_REG_UVOUT_SET(ch), code);
}

/**
 * @brief Get the output undervoltage threshold of one output, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param uvout_uv - Pointer to store the threshold, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_uvout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *uvout_uv)
{
	uint8_t code;
	int ret;

	if (!dev || !uvout_uv || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	ret = adp20086_read(dev, ADP20086_REG_UVOUT_SET(ch), &code);
	if (ret)
		return ret;

	*uvout_uv = code * ADP20086_VOUT_LSB_UV;

	return 0;
}

/**
 * @brief Set the output undervoltage threshold of one output.
 *
 * Rounded up to the next representable step.
 *
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param uvout_uv - Threshold in microvolts, up to ADP20086_MAX_VOUT_UV.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_uvout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t uvout_uv)
{
	uint8_t code;

	if (!dev || uvout_uv > ADP20086_MAX_VOUT_UV || ch < ADP20086_CHANNEL1
	    || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	code = NO_OS_DIV_ROUND_UP(uvout_uv, ADP20086_VOUT_LSB_UV);

	return adp20086_write(dev, ADP20086_REG_UVOUT_SET(ch), code);
}

/**
 * @brief Get the soft-start ramp of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param ssu - Pointer to store the ramp setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_get_soft_startup(struct adp20086_dev *dev,
			      enum adp20086_channel ch,
			      enum adp20086_soft_startup *ssu)
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
 * @brief Set the soft-start ramp of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param ssu - Ramp setting to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_soft_startup(struct adp20086_dev *dev,
			      enum adp20086_channel ch,
			      enum adp20086_soft_startup ssu)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4
	    || ssu < ADP20086_SSU_0R5MS || ssu > ADP20086_SSU_4MS)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_START_SLEW,
					ADP20086_START_SLEW_MASK(ch), ssu);
}

/**
 * @brief Set the same soft-start ramp on all four outputs in one write.
 * @param dev - The ADP20086 device descriptor.
 * @param ssu - Ramp setting to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_soft_startup(struct adp20086_dev *dev,
				  enum adp20086_soft_startup ssu)
{
	uint8_t data = 0;

	if (!dev || ssu < ADP20086_SSU_0R5MS || ssu > ADP20086_SSU_4MS)
		return -EINVAL;

	for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++) {
		data |= no_os_field_prep(ADP20086_START_SLEW_MASK(c), ssu);
	}
	return adp20086_write(dev, ADP20086_REG_START_SLEW, data);
}

/**
 * @brief Get the soft-shutdown ramp of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to query.
 * @param ssd - Pointer to store the ramp setting.
 * @return 0 in case of success, negative error code otherwise.
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
 * @brief Set the soft-shutdown ramp of one output.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to configure.
 * @param ssd - Ramp setting to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_soft_shutdown(struct adp20086_dev *dev,
			       enum adp20086_channel ch, enum adp20086_soft_shutdown ssd)
{
	if (!dev || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4
	    || ssd < ADP20086_SSD_0R25MS || ssd > ADP20086_SSD_2MS)
		return -EINVAL;

	return adp20086_update_register(dev, ADP20086_REG_SHDN_SLEW,
					ADP20086_SHDN_SLEW_MASK(ch), ssd);
}

/**
 * @brief Set the same soft-shutdown ramp on all four outputs in one write.
 * @param dev - The ADP20086 device descriptor.
 * @param ssd - Ramp setting to select.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_set_all_soft_shutdown(struct adp20086_dev *dev,
				   enum adp20086_soft_shutdown ssd)
{
	uint8_t data = 0;

	if (!dev || ssd < ADP20086_SSD_0R25MS || ssd > ADP20086_SSD_2MS)
		return -EINVAL;

	for (int c = ADP20086_CHANNEL1; c <= ADP20086_CHANNEL4; c++) {
		data |= no_os_field_prep(ADP20086_SHDN_SLEW_MASK(c), ssd);
	}

	return adp20086_write(dev, ADP20086_REG_SHDN_SLEW, data);
}

/**
 * @brief Read the raw input voltage ADC code.
 * @param dev - The ADP20086 device descriptor.
 * @param code - Pointer to store the ADC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vin_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VIN_READ, code);
}

/**
 * @brief Read the input voltage, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param vin_uv - Pointer to store the input voltage, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vin(struct adp20086_dev *dev, uint32_t *vin_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VIN_READ,
					    ADP20086_VIN_LSB_UV, vin_uv);
}

/**
 * @brief Read the raw output voltage ADC code of one channel.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to read.
 * @param code - Pointer to store the ADC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_code(dev, ADP20086_REG_VOUT_READ(ch), code);
}

/**
 * @brief Read the output voltage of one channel, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to read.
 * @param vout_uv - Pointer to store the output voltage, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *vout_uv)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_VOUT_READ(ch),
					    ADP20086_VOUT_LSB_UV, vout_uv);
}

/**
 * @brief Read the raw output current ADC code of one channel.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to read.
 * @param code - Pointer to store the ADC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_iout_code(struct adp20086_dev *dev, enum adp20086_channel ch,
			    uint8_t *code)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_code(dev, ADP20086_REG_IOUT_READ(ch), code);
}

/**
 * @brief Read the output current of one channel, in microamps.
 * @param dev - The ADP20086 device descriptor.
 * @param ch - Output channel to read.
 * @param iout_ua - Pointer to store the output current, in microamps.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_iout(struct adp20086_dev *dev, enum adp20086_channel ch,
		       uint32_t *iout_ua)
{
	if (ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
		return -EINVAL;

	return adp20086_read_adc_microunits(dev, ADP20086_REG_IOUT_READ(ch),
					    ADP20086_IOUT_LSB_UA, iout_ua);
}

/**
 * @brief Read the raw VDD supply ADC code.
 * @param dev - The ADP20086 device descriptor.
 * @param code - Pointer to store the ADC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vdd_code(struct adp20086_dev *dev, uint8_t *code)
{
	return adp20086_read_adc_code(dev, ADP20086_REG_VDD_READ, code);
}

/**
 * @brief Read the VDD supply voltage, in microvolts.
 * @param dev - The ADP20086 device descriptor.
 * @param vdd_uv - Pointer to store the supply voltage, in microvolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int adp20086_read_vdd(struct adp20086_dev *dev, uint32_t *vdd_uv)
{
	return adp20086_read_adc_microunits(dev, ADP20086_REG_VDD_READ,
					    ADP20086_VDD_LSB_UV, vdd_uv);
}
