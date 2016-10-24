/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad5676.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * SPI/I2C command write to device.
 * @param dev - The device structure.
 * @param cmd - The command.
 * @param data - The data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_cmd_write(ad5676_dev *dev,
						 uint8_t cmd,
						 uint16_t data)
{
	uint8_t buf[3];
	int32_t ret;

	buf[0] = cmd;
	buf[1] = (data & 0xFF00) >> 8;
	buf[2] = (data & 0x00FF) >> 0;

	if(chip_info[dev->act_device].communication == SPI)
	{
		ret = spi_write_and_read(&dev->spi_dev, buf, 3);
	}
	else
	{		
		if((cmd == AD5676_CMD_SET_DCEN)|| 
		  ((cmd & 0xF0) == AD5676_CMD_READBACK))
		{
		   // Dasy chain and readback commands are not available for I2C devices
			printf("%s: Command %x is not available for the selected device.\n", 
				   __func__, cmd);
		   return FAILURE;
		}
		else
		{	
			ret = i2c_write_and_read(&dev->i2c_dev, buf, 3);
		}
	}

	return ret;
}

/**
 * SPI readback register from device.
 * @param dev - The device structure.
 * @param dac - The dac address.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_spi_readback_reg(ad5676_dev *dev,
								ad5676_dac dac,
								uint32_t *data)
{
	uint8_t buf[3] = {0, 0, 0};
	int32_t ret;

	if(chip_info[dev->act_device].communication == SPI)
	{
		ad5676_cmd_write(dev, AD5676_CMD_SET_READBACK(dac), 0x0000);
		ret = spi_write_and_read(&dev->spi_dev, buf, 3);

		*data = (buf[0] << 16) | ((buf[1] << 8)) | (buf[2] << 0);
	}
	else
	{
		printf("%s: This feature is not available in I2C communication mode.\n",
			  __func__);
		return FAILURE;
	}

	return ret;
}

/**
 * Internal reference and gain setup.
 * @param dev - The device structure.
 * @param vref - internal reference enabled.
 *				 Accepted values: AD5676_INT_REF_ON
 *								  AD5676_INT_REF_OFF
 * @param gain - amplifier gain setting
 *				 Accepted values: AD5676_AMP_GAIN_1
 *								  AD5676_AMP_GAIN_2
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_set_gain_voltage_ref(ad5676_dev *dev,
							   		ad5676_voltage_ref vref,
							   		ad5676_amp_gain gain)
{
	return ad5676_cmd_write(dev, AD5676_CMD_INT_REF_SETUP,
							AD5676_CFG_GAIN(gain) | AD5676_CFG_INT_REF(vref));
}

/**
 * Power down the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *					Accepted values: AD5676_PWR_PD0(x) | AD5676_PWR_PD1(x) |
 *									 ... AD5676_PWR_PD7(x) 
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_set_pwr_dac(ad5676_dev *dev,
						   uint16_t setting)
{
	return ad5676_cmd_write(dev, AD5676_CMD_WR_PWR_DAC,
								setting);
}

/**
 * Do a software reset.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_do_soft_reset(ad5676_dev *dev)
{
	return ad5676_cmd_write(dev, AD5676_CMD_SW_RESET,
								0x0000);
}

/**
 * Set the DAC register for the selected channel.
 * @param dev - The device structure.
 * @param dac - The selected channel.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_set_dac_reg(ad5676_dev *dev,
						   ad5676_dac dac,
						   uint16_t data)
{
	return ad5676_cmd_write(dev, AD5676_CMD_WR_DAC_REG(dac),
								data);	
}

/**
 * Initialize the device.
 * @param id - The supported device ids.
 *					Accepted values: ID_AD5676
 *									 ID_AD5676R
 *									 ID_AD5672R
 *									 ID_AD5675
 *									 ID_AD5675R 
 *									 ID_AD5671R
 *									 ID_AD5686
 *									 ID_AD5686R
 *									 ID_AD5685R
 *									 ID_AD5684
 *									 ID_AD5684R
 *									 ID_AD5696
 *									 ID_AD5696R
 *									 ID_AD5695R
 *									 ID_AD5694
 *									 ID_AD5694R
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 					   parameters.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad5676_setup(ad5676_supported_device_ids id,
					 ad5676_dev **device,
					 ad5676_init_param init_param)
{
	ad5676_dev *dev;
	int32_t ret;
	dev = (ad5676_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	/* SPI */
	dev->spi_dev.chip_select = init_param.spi_chip_select;
	dev->spi_dev.mode = init_param.spi_mode;
	dev->spi_dev.device_id = init_param.spi_device_id;
	dev->spi_dev.type = init_param.spi_type;
	ret = spi_init(&dev->spi_dev);
	
	/* I2C */
	dev->i2c_dev.i2c_dev_addr = init_param.i2c_dev_addr_selection;
	ret = i2c_init(&dev->i2c_dev);

	/* GPIO */
	dev->gpio_dev.device_id = init_param.gpio_device_id;
	dev->gpio_dev.type = init_param.gpio_type;
	ret |= gpio_init(&dev->gpio_dev);

	dev->gpio_reset = init_param.gpio_reset;
	ret |= gpio_set_direction(&dev->gpio_dev, dev->gpio_reset, GPIO_OUT);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_reset, GPIO_LOW);
	mdelay(10);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_reset, GPIO_HIGH);
	mdelay(10);
	
	dev->gpio_ldac = init_param.gpio_ldac;
	ret |= gpio_set_direction(&dev->gpio_dev, dev->gpio_ldac, GPIO_OUT);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_ldac, GPIO_HIGH);
	mdelay(10);
	ret |= gpio_set_value(&dev->gpio_dev, dev->gpio_ldac, GPIO_LOW);
	mdelay(10);

	/* Device Settings */
	dev->daisy_chain_en = init_param.daisy_chain_en;
	ret |= ad5676_set_gain_voltage_ref(dev, init_param.vref, init_param.gain);
	ret |= ad5676_set_pwr_dac(dev, init_param.pwr_dac_setting);

	dev->act_device = id; // actual device selected
	*device = dev;

	if (!ret)
		printf("%s successfully initialized\n", chip_info[id].dev_name);
	else
		printf("%s initialization error (%d)\n", chip_info[id].dev_name, ret);

	return ret;
}