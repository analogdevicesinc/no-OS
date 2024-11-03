/***************************************************************************//**
 *   @file   ad5293.c
 *   @brief  Source file for the AD5293 Driver
 *   @author Ming Zeng
********************************************************************************
The MIT License (MIT)

Copyright (c) 2024 Ming Zeng

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

/***************************** Include Files **********************************/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ad5293.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/***************************************************************************//**
 * @brief Reset ad5293 chip info
 *
 * Reset ad5293 chip info to default situation after hard/soft reset
 *
 * @param dev - The device structure.
 *
 * @return void
 *******************************************************************************/
static void ad5293_reset_chip_info(struct ad5293_dev* dev)
{
	uint8_t i;

	for (i = 0; i < dev->chip_num; i++) {
		(dev->chip + i)->Wiper_value = 512;
		(dev->chip + i)->cali = CALI_PERFORMANCE;
	}
}

/***************************************************************************//**
 * @brief Initialize the ad5293 device structure.
 *
 * Performs memory allocation of the device structure.
 *
 * @param device     - Pointer to location of device structure to write.
 * @param init_param - Pointer to configuration of the driver.
 *
 * @return ret - return code.
 *         Example: -ENOMEM - Memory allocation error.
 *                  -EINVAL - No GPIO instance.
 *                  -EINVAL - No valid chip number.
 *                  -EIO - SPI communication error.
 *                  -EIO - GPIO initialization error.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_init(struct ad5293_dev **device,
		    struct ad5293_init_param* init_param)
{
	struct ad5293_dev* dev = NULL;
	struct ad5293_chip_info* chp = NULL;
	int32_t ret ;

	if ((! device) || (! init_param))
		return - EINVAL;

	dev = (struct ad5293_dev*)no_os_malloc(sizeof(* dev));
	if (! dev)
		return - ENOMEM;

	// hardware reset pin initialization
	if (init_param->gpio_reset) {
		ret = no_os_gpio_get_optional(& dev->gpio_reset, init_param->gpio_reset);
		if (ret) {
			ret = - EIO;
			goto error;
		}

		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret) {
			ret = - EIO;
			goto error;
		}
	}

	// at least 1 chip
	if (init_param->chip_num < 1) {
		ret = - EINVAL;
		goto error;
	}

	dev->chip_num = init_param->chip_num;
	chp = (struct ad5293_chip_info*)no_os_malloc(dev->chip_num * sizeof(* chp));
	if (! chp) {
		ret = - ENOMEM;
		goto error;
	}

	dev->chip = chp;

	// reset all chips
	ret = ad5293_hard_reset(dev);
	if (ret)
		goto error;

	// spi initialization
	init_param->spi_init.mode = NO_OS_SPI_MODE_1; // CPOL=0, CPHA=1
	ret = no_os_spi_init(& dev->spi_desc, & init_param->spi_init);
	if (ret) {
		ret = - EIO;
		goto error;
	}

	*device = dev;

	return ret;

error:
	if (dev->gpio_reset)
		ret = no_os_gpio_remove(dev->gpio_reset);
	if (dev->spi_desc)
		ret = no_os_spi_remove(dev->gpio_reset);
	no_os_free(chp);
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free any resource used by the driver.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_remove(struct ad5293_dev* dev)
{
	int32_t ret ;

	if (dev->gpio_reset) {
		ret = no_os_gpio_remove(dev->gpio_reset);
		if (ret)
			return ret;
	}

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev->chip);
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reset ad5293 through Hardware Reset Pin.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_hard_reset(struct ad5293_dev* dev)
{
	int32_t ret ;

	if (! dev->gpio_reset)
		return - EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(1);  // >20ns pulse width

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(2); // t12: max 1.5ms reset time

	ad5293_reset_chip_info(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reset ad5293 through software reset command.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_soft_reset(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(RESET_FRAME >> 8);
		buf[2 * i + 1] = (uint8_t)(RESET_FRAME);
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	no_os_mdelay(2); // t12: max 1.5ms reset time

	ad5293_reset_chip_info(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Place SDO pin in high impedence mode.
 *
 * Minimize power dissipation when pin is used.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_sdo_setfloat(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(CMD_SDO_Z >> 8);
		buf[2 * i + 1] = (uint8_t)CMD_SDO_Z;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	memset(& buf, 0, sz);

	return no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
}

/***************************************************************************//**
 * @brief Place ad5293 in shutdown mode or recovery.
 *
 * In shut down mode, terminal A open, Wiper and terminal B connected.
 *
 * @param dev - The device structure.
 * @param sd - shutdown mode selection.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_shutdown(struct ad5293_dev* dev, enum shutdown_t sd)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(SHUTDOWN_FRAME(sd) >> 8);
		buf[2 * i + 1] = (uint8_t)SHUTDOWN_FRAME(sd);
	}

	return no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
}

/***************************************************************************//**
 * @brief Update ad5293 chip info struct calibration information.
 *
 * For calibration register write operation.
 *
 * @param dev - The device structure.
 * @param cali - calibration mode selection.
 * @param num - chip indication.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid num input.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_update_cali(struct ad5293_dev* dev, enum calibration_mode_t cali,
			   uint8_t num)
{

	if (num > (dev->chip_num - 1))
		return - EINVAL;

	(dev->chip + num)->cali = cali;

	return 0;
}

/***************************************************************************//**
 * @brief Update ad5293 chip info struct wiper value information.
 *
 * For wiper register write operation.
 *
 * @param dev - The device structure.
 * @param val - wiper value.
 * @param num - chip indication.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid num input.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_update_wiper(struct ad5293_dev* dev, uint16_t val, uint8_t num)
{
	if (num > (dev->chip_num - 1))
		return - EINVAL;

	(dev->chip + num)->Wiper_value = val;

	return 0;
}

/* internal function to lock/unlock write protection of ad5293*/
static int32_t ad5293_write_protect(struct ad5293_dev* dev,
				    enum protect_mode_t pm)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];
	enum protect_mode_t cali;

	for (i = 0; i < dev->chip_num; i++) {
		cali = (dev->chip + i)->cali;
		buf[2 * i] = (uint8_t)(AD5293_W_CTRL_FRAME(pm, cali) >> 8);
		buf[2 * i + 1] = (uint8_t)(AD5293_W_CTRL_FRAME(pm, cali));
	}

	return no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
}

/***************************************************************************//**
 * @brief Write ad5293 chip info struct calibration value through SPI interface.
 *
 * Note: multiple chips operation should in daisy-chain mode.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_write_cali(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];
	enum calibration_mode_t cali;

	for (i = 0; i < dev->chip_num; i++) {
		cali = (dev->chip + i)->cali;
		buf[2 * i] = (uint8_t)(AD5293_W_CTRL_FRAME(PROTECT_LOCK, cali) >> 8);
		buf[2 * i + 1] = (uint8_t)(AD5293_W_CTRL_FRAME(PROTECT_LOCK, cali));
	}

	return no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
}

/***************************************************************************//**
 * @brief Write ad5293 chip info struct wiper value through SPI interface.
 *
 * Note: multiple chips operation should in daisy-chain mode.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_write_wiper(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(AD5293_W_WIPER_FRAME((dev->chip + i)->Wiper_value) >> 8);
		buf[2 * i + 1] = (uint8_t)(AD5293_W_WIPER_FRAME((dev->chip + i)->Wiper_value));
	}

	ret = ad5293_write_protect(dev, PROTECT_UNLOCK);
	if (ret)
		return ret;

	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	no_os_udelay(3); // performance mode, t12 > 2.4us

	return ad5293_write_protect(dev, PROTECT_LOCK);

}

/***************************************************************************//**
 * @brief Read ad5293 device register calibration mode through SPI interface.
 *
 * read ad5293 calibration value and update it to chip info struct.
 * Note: multiple chips operation should in daisy-chain mode.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_read_reg_cali(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(AD5293_R_CTRL_FRAME >> 8);
		buf[2 * i + 1] = (uint8_t)(AD5293_R_CTRL_FRAME);
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	memset(& buf, 0, sz);
	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	for (i = 0; i < dev->chip_num; i++)
		(dev->chip + i)->cali = no_os_field_get(CALI_MSK, buf[2 * i + 1]);

	return ret;
}

/***************************************************************************//**
 * @brief Read ad5293 device register wiper value through SPI interface.
 *
 * read ad5293 wiper value and update it to chip info struct.
 * Note: multiple chips operation should in daisy-chain mode.
 *
 * @param dev - The device structure.
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid pointer input.
 *                  -ENOSYS - No valid operation.
 *                  0 - No errors encountered.
 *******************************************************************************/
int32_t ad5293_read_reg_wiper(struct ad5293_dev* dev)
{
	int32_t ret ;
	uint8_t i;
	uint16_t sz = 2 * dev->chip_num;
	uint8_t buf[sz];

	for (i = 0; i < dev->chip_num; i++) {
		buf[2 * i] = (uint8_t)(AD5293_R_WIPER_FRAME >> 8);
		buf[2 * i + 1] = (uint8_t)(AD5293_R_WIPER_FRAME);
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	memset(& buf, 0, sz);
	ret = no_os_spi_write_and_read(dev->spi_desc, & buf[0], sz);
	if (ret)
		return ret;

	for (i = 0; i < dev->chip_num; i++)
		(dev->chip + i)->Wiper_value = (uint16_t)buf[2 * i] << 8 | buf[2 * i + 1];

	return ret;
}

/***************************************************************************//**
 * @brief Get calibration of a certain chip.
 *
 * return chip info struct calibration value to caller.
 *
 * @param dev - The device structure.
 * @param num - The device number.( 0 to chip_num-1)
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid chip selected.
 *                  0 - No errors encountered.
 *******************************************************************************/
int8_t ad5293_get_cali(struct ad5293_dev* dev, uint8_t num)
{
	if (num > (dev->chip_num - 1))
		return - EINVAL;

	return (dev->chip + num)->cali;
}

/***************************************************************************//**
 * @brief Get wiper of a certain chip.
 *
 * return chip info struct wiper value to caller.
 *
 * @param dev - The device structure.
 * @param num - The device number.( 0 to chip_num-1)
 *
 * @return ret - return code.
 *         Example: -EINVAL - No valid chip selected.
 *                  0 - No errors encountered.
 *******************************************************************************/
int16_t ad5293_get_wiper(struct ad5293_dev* dev, uint8_t num)
{
	if (num > (dev->chip_num - 1))
		return - EINVAL;

	return (dev->chip + num)->Wiper_value;
}
