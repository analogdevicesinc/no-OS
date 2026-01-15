/***************************************************************************//**
 *   @file   lm75.c
 *   @brief  Source file of LM75 Driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "lm75.h"

static const struct lm75_reg_permissions {
	bool readable;
	bool writable;
	uint16_t reset_value;
} lm75_reg_perms[] = {
	{ true, false, 0, }, /* 00 temperature */
	{ true, true,  0, }, /* 01 configuration */
	{ true, true,  LM75_HYST_POR_VALUE, }, /* 02 hysteresis */
	{ true, true,  LM75_OS_POR_VALUE, },   /* 03 overtemp */
};

/**
 * @brief Determine if LM75 register is readable.
 *
 * @param[in] addr LM75 register.
 *
 * @return true if LM75 register is readable, false otherwise.
 */
static inline bool lm75_reg_is_readable(const uint8_t addr)
{
	if (addr > lm75_os_reg)
		return false;

	return lm75_reg_perms[addr].readable;
}

/**
 * @brief Determine if LM75 register can be written to.
 *
 * @param[in] addr LM75 register.
 *
 * @return true if LM75 register is writeable, false otherwise.
 */
static inline bool lm75_reg_is_writable(const uint8_t addr)
{
	if (addr > lm75_os_reg)
		return false;

	return lm75_reg_perms[addr].writable;
}

/**
 * @brief Return the POR value of a given register defined by addr parameter.
 *
 * @param[in] addr LM75 register.
 *
 * @return POR value of register.
 */
static uint16_t lm75_reg_reset_value(const uint8_t addr)
{
	if (addr > lm75_os_reg)
		return LM75_ADDR_MASK;
	return lm75_reg_perms[addr].reset_value & LM75_SENSOR_DATA_MASK;
}

/**
 * @brief Perform the proper IIC sequence to properly read a 8-bit value
 *        from any LM75 register.
 *
 * @param[in] dev lm75 device descriptor.
 * @param[in] addr LM75 register to write to.
 * @param[in] val 8-bit value read from addr.
 *
 * @return 0 on success, negative value if otherwise.
 * @return -ENODEV when dev is NULL
 * @return -EINVAL if val is NULL
 * @return -ERANGE if register addr falls beyond the acceptable values.
 * @return -EPERM when reading is not supported for this register.
 */
static int lm75_reg_read8(struct lm75_dev *dev, uint8_t addr, uint8_t *val)
{
	int err;
	uint8_t frame[LM75_FRAME_SIZE];

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;
	if (addr > lm75_os_reg)
		return -ERANGE;

	if (!lm75_reg_is_readable(addr))
		return -EPERM;

	frame[0] = addr;
	err = no_os_i2c_write(dev->i2c, frame, 1, 0);
	if (err)
		return err;

	err = no_os_i2c_read(dev->i2c, frame, 1, 1);
	if (err)
		return err;

	*val = frame[0];

	return 0;
}

/**
 * @brief Perform the proper IIC sequence to properly write a 8-bit value
 *        from any LM75 register.
 *
 * @param[in] dev lm75 device descriptor.
 * @param[in] addr LM75 register to write to.
 * @param[in] val 8-bit value to write to addr.
 *
 * @return 0 on success, negative value if otherwise.
 */
static int lm75_reg_write8(struct lm75_dev *dev, uint8_t addr, uint8_t val)
{
	uint8_t frame[LM75_FRAME_SIZE];

	if (!dev)
		return -ENODEV;
	if (addr > lm75_os_reg)
		return -ERANGE;

	if (!lm75_reg_is_writable(addr))
		return -EPERM;

	frame[0] = addr;
	frame[1] = val;
	return no_os_i2c_write(dev->i2c, frame, 2, 1);
}

/**
 * @brief Perform the proper IIC sequence to properly read a 16-bit value
 *        from any LM75 register.
 *
 * @param[in] dev lm75 device descriptor.
 * @param[in] addr LM75 register to write to.
 * @param[in] val 16-bit value read from addr.
 *
 * @return 0 on success, negative value if otherwise.
 */
static int lm75_reg_read16(struct lm75_dev *dev, uint8_t addr, uint16_t *val)
{
	int err;
	uint8_t frame[LM75_FRAME_SIZE];

	if (!dev)
		return -ENODEV;
	if (!val)
		return -EINVAL;
	if (addr > lm75_os_reg)
		return -ERANGE;

	if (!lm75_reg_is_readable(addr))
		return -EPERM;

	frame[0] = addr;
	err = no_os_i2c_write(dev->i2c, frame, 1, 0);
	if (err)
		return err;

	err = no_os_i2c_read(dev->i2c, frame, 2, 1);
	if (err)
		return err;

	*val = no_os_get_unaligned_be16(frame);

	return 0;
}

/**
 * @brief Perform the proper IIC sequence to properly write a 16 bit value
 *        to any LM75 register.
 *
 * @param[in] dev lm75 device descriptor.
 * @param[in] addr LM75 register to write to.
 * @param[in] val 16-bit value to write to addr.
 *
 * @return 0 on success, negative value if otherwise.
 */
static int lm75_reg_write16(struct lm75_dev *dev, uint8_t addr, uint16_t val)
{
	uint8_t frame[LM75_FRAME_SIZE];

	if (!dev)
		return -ENODEV;
	if (addr > lm75_os_reg)
		return -ERANGE;

	if (!lm75_reg_is_writable(addr))
		return -EPERM;

	frame[0] = addr;
	frame[1] = no_os_field_get(0xFF00, val);
	frame[2] = no_os_field_get(0x00FF, val);
	return no_os_i2c_write(dev->i2c, frame, 3, 1);
}

/**
 * @brief Sanity check IIC address meant for addressing LM75.
 *
 * LM75 uses 7-bit IIC addressing with the top 4 bits hardcoded while the three
 * lower bits are user defined. We check if the hardcoded address bits are
 * correct.
 *
 * @param[in] addr address value used to target this LM75 device.
 *
 * @return true if address is valid LM75 address, false if otherwise.
 */
static bool lm75_addr_ok(const uint8_t addr)
{
	uint8_t lm75_hw_addr = LM75_HW_ADDR;
	return (addr & lm75_hw_addr) == lm75_hw_addr;
}

/**
 * @brief Create new LM75 descriptor and reserve i2c and memory resources
 *        used by the device driver.
 *
 * @param[in] lm75 lm75 initialization parameter structure.
 * @param[in] ip lm75 initialization parameter structure.
 *
 * @return 0 on success, negative value on error.
 */
static int new_lm75(struct lm75_dev **lm75pp,
		    struct no_os_i2c_init_param *ip)
{
	struct lm75_dev *lm75;
	struct no_os_i2c_desc *iic;
	uint16_t hyst;
	int err;

	if (!lm75pp)
		return -EINVAL;

	if (!lm75_addr_ok(ip->slave_address))
		return -EINVAL;

	lm75 = no_os_malloc(sizeof(*lm75));
	if (!lm75)
		return -ENOMEM;

	err = no_os_i2c_init(&iic, ip);
	if (err)
		goto err_iic_failed;

	lm75->i2c = iic;
	*lm75pp = lm75;

	/* we check here if link is up with LM75 */
	err = lm75_read_temperature(lm75, lm75_hysteresis_temperature, &hyst);
	if (err)
		goto err_iic_remove;
	if (hyst != lm75_reg_reset_value(lm75_hyst_reg)) {
		err = -ENODEV;
		goto err_iic_remove;
	}

	lm75->signature = LM75_DEVICE_SIGNATURE;

	return 0;

err_iic_remove:
	no_os_i2c_remove(iic);
err_iic_failed:
	no_os_free(lm75);
	return err;
}

/**
 * @brief release resources used by LM75 driver like i2c and memory.
 *
 * @param[in] lm75 lm75 initialization parameter structure.
 *
 * @return 0 on success, negative value on error.
 */
static int delete_lm75(struct lm75_dev *lm75)
{
	int err = 0;

	if (!lm75)
		return -EINVAL;

	if (lm75->i2c)
		err = no_os_i2c_remove(lm75->i2c);

	no_os_free(lm75);

	return err;
}

/**
 * @brief Initialize LM75 device driver.
 *
 * Creates an LM75 device driver descriptor and puts the LM75 device into
 * comparator mode initially.
 *
 * @param[out] devpp the newly create LM75 device descriptor.
 * @param[in] ip lm75 initialization parameter structure.
 *
 * @return 0 on success, negative value on error.
 */
int lm75_init(struct lm75_dev **devpp,
	      struct lm75_init_param *ip)
{
	struct no_os_i2c_init_param *iic_ip;
	struct lm75_dev *ldev;
	int err;

	if (!devpp)
		return -EINVAL;

	if (!ip)
		return -EINVAL;

	iic_ip = ip->i2c_ip;
	if (!iic_ip) {
		return -EINVAL;
	}

	err = new_lm75(&ldev, iic_ip);
	if (err)
		return err;

	/* just make sure lm75 is fully operational */
	err = lm75_shutdown(ldev, false);
	if (err)
		goto err_delete_lm75;

	/* LM75 driver initializes to comparator mode */
	err = lm75_write_mode(ldev, lm75_comparator_mode);
	if (err)
		goto err_shutdown_lm75;

	/* set the overtemp signal polarity for comparator mode */
	err = lm75_write_os_polarity(ldev, ip->os_polarity);
	if (err)
		goto err_shutdown_lm75;

	/* set the fault queue */
	err = lm75_write_fault_queue(ldev, ip->fault_count);
	if (err)
		goto err_shutdown_lm75;

	*devpp = ldev;

	return 0;

err_shutdown_lm75:
	err = lm75_shutdown(ldev, true);
	if (err)
		return err;

err_delete_lm75:
	return delete_lm75(ldev);
}

/**
 * @brief Uninitialize LM75 device driver.
 *
 * Put LM75 into shutdown (low power mode) and release resources back to
 * system.
 *
 * @param[in] dev the LM75 device.
 *
 * @return 0 on success, negative value on fail.
 */
int lm75_remove(struct lm75_dev *dev)
{
	int err;

	if (!dev)
		return -EINVAL;

	err = lm75_shutdown(dev, true);
	if (err)
		return err;

	return delete_lm75(dev);
}

/**
 * @brief Read temperature data.
 *
 * Used to read die, hysteresis and over temperature.
 *
 * @param[in] dev the LM75 device.
 * @param[in] whence the type of temperature to update, can be
 *            lm75_die_temperature, lm75_hysteresis_temperature or
 *            lm75_over_temperature.
 * @param[out] val raw sensor temperature data defined by whence parameter.
 *
 * @return 0 on success, negative value on error.
 */
int lm75_read_temperature(struct lm75_dev *dev,
			  enum lm75_temperature_type whence,
			  uint16_t *val)
{
	int err;
	enum lm75_register reg;
	uint16_t temp;

	switch (whence) {
	case lm75_die_temperature:
		reg = lm75_temperature_reg;
		break;
	case lm75_hysteresis_temperature:
		reg = lm75_hyst_reg;
		break;
	case lm75_over_temperature:
		reg = lm75_os_reg;
		break;
	default:
		return -EINVAL;
	}

	err = lm75_reg_read16(dev, reg, &temp);
	if (err)
		return err;

	*val = temp & 0xFF80;

	return 0;
}

/**
 * @brief Write temperature data.
 *
 * Used to update hysteresis and over temperature. Die temperature is
 * read-only.
 *
 * @param[in] dev the LM75 device.
 * @param[in] whence the type of temperature to update, can be either
 *            lm75_hysteresis_temperature or lm75_over_temperature only.
 * @param[in] val the raw sensor data to write.
 *
 * @return 0 on success, negative value on error.
 */
int lm75_write_temperature(struct lm75_dev *dev,
			   enum lm75_temperature_type whence,
			   uint16_t val)
{
	enum lm75_register reg;

	switch (whence) {
	case lm75_die_temperature:
		reg = lm75_temperature_reg;
		break;
	case lm75_hysteresis_temperature:
		reg = lm75_hyst_reg;
		break;
	case lm75_over_temperature:
		reg = lm75_os_reg;
		break;
	default:
		return -EINVAL;
	}

	return lm75_reg_write16(dev, reg, val);
}

/**
 * @brief Convert a raw sensor data to milliCelsius value.
 *
 * @param[in] raw raw sensor data to convert to MilliCelsius.
 *
 * @return converted MilliCelsius value from raw sensor data.
 */
int lm75_raw_to_millicelsius(const uint16_t raw_temp)
{
	int tmp;

	tmp = no_os_field_get(LM75_TEMP_MASK, raw_temp);
	tmp = no_os_sign_extend16(tmp, 8);
	tmp = tmp * 1000 / 2;

	return tmp;
}

/**
 * @brief Convert a milliCelsius value to raw sensor data that can be written
 *        to an LM75 device.
 *
 * Used primarily to update hyst and over temperature.
 *
 * @param[in] millicelsius milliCelsius value to convert to raw sensor data.
 *
 * @return raw converted sensor value from millicelsius.
 */
uint16_t lm75_millicelsius_to_raw(const int millicelsius)
{
	int tmp;
	uint16_t raw_temp;

	tmp = millicelsius * 2 / 1000;
	raw_temp = (uint16_t)no_os_field_prep(LM75_TEMP_MASK, tmp);

	return raw_temp;
}

/**
 * @brief Read a field in LM75 configuration register defined by field_mask.
 *
 * @param[in] dev The LM75 device structure.
 * @param[in] field_mask a bitmask of the field of read.
 * @param[out] config value of the field.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int lm75_read_one_configuration_field(struct lm75_dev *dev,
		uint8_t field_mask, uint8_t *config)
{
	int err;
	uint8_t cfg;

	if (!dev)
		return -ENODEV;

	if (!config)
		return -EINVAL;

	err = lm75_reg_read8(dev, lm75_configuration_reg, &cfg);
	if (err)
		return err;

	*config = no_os_field_get(field_mask, cfg);

	return 0;
}

/**
 * @brief Write a field in LM75 configuration register defined by field_mask.
 *
 * @param[in] dev The LM75 device structure.
 * @param[in] field_mask a bitmask of the field of read.
 * @param[out] config value to write to the field.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
static int lm75_write_one_configuration_field(struct lm75_dev *dev,
		uint8_t field_mask, uint8_t field_val)
{
	int err;
	uint8_t config;

	if (!dev)
		return -ENODEV;

	err = lm75_reg_read8(dev, lm75_configuration_reg, &config);
	if (err)
		return err;

	config &= ~field_mask;
	config |= no_os_field_prep(field_mask, field_val);

	return lm75_reg_write8(dev, lm75_configuration_reg, config);
}

/**
 * @brief Read if LM75 is in shutdown or normal operation.
 *
 * @param[in] dev The LM75 device structure.
 * @param[out] flag true shuts down the LM75. false puts the LM75 into normal
 *             operation.
 *
 * @return 0 in case of success
 * @return -ENODEV if dev is NULL
 * @return -EINVAL if flag is NULL
 */
int lm75_is_shutdown(struct lm75_dev *dev, bool *flag)
{
	int err;
	uint8_t val;

	if (!dev)
		return -ENODEV;

	if (!flag)
		return -EINVAL;

	err = lm75_read_one_configuration_field(dev, LM75_CONFIG_SHUTDOWN, &val);
	if (err)
		return err;
	*flag = !!val;

	return 0;
}

/**
 * @brief Put the LM75 into normal or shutdown operation
 *
 * @param[in] dev The LM75 device structure.
 * @param[in] flag true shuts down the LM75. false puts the LM75 into normal
 *             operation.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_shutdown(struct lm75_dev *dev, bool flag)
{
	return lm75_write_one_configuration_field(dev, LM75_CONFIG_SHUTDOWN, flag);
}

/**
 * @brief Read the event mode of LM75, whether interrupt based or comparator.
 * @param[in] dev The LM75 device structure.
 * @param[out] mode buffer where mode will be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_read_mode(struct lm75_dev *dev, enum lm75_mode *mode)
{
	return lm75_read_one_configuration_field(dev, LM75_CONFIG_INT_EN, mode);
}

/**
 * @brief Change the event mode of LM75, whether interrupt based or comparator.
 * @param[in] dev The LM75 device structure.
 * @param[in] mode either lm75_comparator_mode or lm75_interrupt_mode
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_write_mode(struct lm75_dev *dev, enum lm75_mode mode)
{
	return lm75_write_one_configuration_field(dev, LM75_CONFIG_INT_EN, mode);
}

/**
 * @brief Read the over temperature line polarity setting.
 *
 * @param dev[in] The LM75 device structure.
 * @param pol[out] buffer where fault queue number will be written.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_read_os_polarity(struct lm75_dev *dev, enum lm75_os_polarity *pol)
{
	return lm75_read_one_configuration_field(dev, LM75_CONFIG_OS_POLARITY, pol);
}

/**
 * @brief Write the polarity of the over temperature line.
 *
 * @param[in] dev The LM75 device structure.
 * @param[out] pol lm75_os_active_low or lm75_os_active_high.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_write_os_polarity(struct lm75_dev *dev, enum lm75_os_polarity pol)
{
	return lm75_write_one_configuration_field(dev, LM75_CONFIG_OS_POLARITY, pol);
}

/**
 * @brief Read fault count.
 *
 * @param[in] dev The LM75 device structure.
 * @param[out] fq buffer where fault queue number will be written.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_read_fault_queue(struct lm75_dev *dev, enum lm75_fault_queue *fq)
{
	return lm75_read_one_configuration_field(dev, LM75_CONFIG_FAULT_QUEUE, fq);
}

/**
 * @brief Write fault count.
 *
 * @param[in] dev The LM75 device structure.
 * @param[in] fq fault queue number.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int lm75_write_fault_queue(struct lm75_dev *dev, enum lm75_fault_queue fq)
{
	return lm75_write_one_configuration_field(dev, LM75_CONFIG_FAULT_QUEUE, fq);
}
