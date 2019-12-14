/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ad4110.h"
#include "error.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/***************************************************************************//**
 * Compute CRC8 checksum.
 *
 * @param data      - The data buffer.
 * @param data_size - The size of the data buffer.
 *
 * @return CRC8 checksum.
*******************************************************************************/
uint8_t ad4110_compute_crc8(uint8_t *data,
			    uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD4110_CRC8_POLY;
			} else
				crc <<= 1;
		}
		data++;
		data_size--;
	}
	return crc;
}

/***************************************************************************//**
 * Compute XOR checksum.
 *
 * @param data      - The data buffer.
 * @param data_size - The size of the data buffer.
 *
 * @return XOR checksum.
*******************************************************************************/
uint8_t ad4110_compute_xor(uint8_t *data,
			   uint8_t data_size)
{
	uint8_t crc = 0;
	uint8_t buf[3];

	uint8_t i;

	for (i = 0; i < data_size; i++) {
		buf[i] = *data;
		crc ^= buf[i];
		data++;
	}
	return crc;
}

/***************************************************************************//**
 * SPI internal register write to device using a mask.
 *
 * @param dev      - The device structure.
 * @param reg_map  - The register map.
 *		     Accepted values: A4110_ADC
 *				      A4110_AFE
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_spi_int_reg_write_msk(struct ad4110_dev *dev,
				     uint8_t reg_map,
				     uint8_t reg_addr,
				     uint32_t data,
				     uint16_t mask)
{
	int32_t ret;
	uint32_t reg_data;

	ret = ad4110_spi_int_reg_read(dev, reg_map, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	ret |= ad4110_spi_int_reg_write(dev, reg_map, reg_addr, reg_data);

	return ret;
}

/***************************************************************************//**
 * Set the mode of the ADC.
 *
 * @param dev       - The device structure.
 * @param mode      - The ADC mode
 *		      Accepted values: AD4110_CONTINOUS_CONV_MODE
 *				       AD4110_SINGLE_CONV_MODE
 *				       AD4110_STANDBY_MODE
 *				       AD4110_PW_DOWN_MODE
 *				       AD4110_SYS_OFFSET_CAL
 *				       AD4110_SYS_GAIN_CAL
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_set_adc_mode(struct ad4110_dev *dev, enum ad4110_adc_mode mode)
{
	int32_t ret;

	if(mode == AD4110_SYS_OFFSET_CAL)
		printf("Assuming that the applied analog input is the zero scale point. \n");
	else if(mode == AD4110_SYS_GAIN_CAL)
		printf("Assuming that the applied analog input is the full scale point. \n");

	ret = ad4110_spi_int_reg_write_msk(dev,
					   A4110_ADC,
					   AD4110_REG_ADC_MODE,
					   AD4110_ADC_MODE(mode),
					   AD4110_REG_ADC_MODE_MSK);

	return ret;
}

/***************************************************************************//**
 * Set the gain.
 *
 * @param dev  - The device structure.
 * @param gain - The gain value.
 * 		 Accepted values: AD4110_GAIN_0_2
 * 				  AD4110_GAIN_0_25
 * 				  AD4110_GAIN_0_3
 * 				  AD4110_GAIN_0_375
 * 				  AD4110_GAIN_0_5
 * 				  AD4110_GAIN_0_75
 * 				  AD4110_GAIN_1
 * 				  AD4110_GAIN_1_5
 * 				  AD4110_GAIN_2
 * 				  AD4110_GAIN_3
 * 				  AD4110_GAIN_4
 * 				  AD4110_GAIN_6
 * 				  AD4110_GAIN_8
 * 				  AD4110_GAIN_12
 * 				  AD4110_GAIN_16
 * 				  AD4110_GAIN_24
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_set_gain(struct ad4110_dev *dev, enum ad4110_gain gain)
{
	int32_t ret;

	ret = ad4110_spi_int_reg_write_msk(dev,
					   A4110_AFE,
					   AD4110_REG_PGA_RTD_CTRL,
					   AD4110_REG_PGA_RTD_CTRL_GAIN_CH(gain),
					   AD4110_REG_PGA_RTD_CTRL_GAIN_CH(0xF));

	return ret;
}

/***************************************************************************//**
 * Set the operation mode.
 *
 * @param dev  - The device structure.
 * @param mode - The operation mode type.
 * 		 Accepted values: AD4110_VOLTAGE_MODE
 * 				  AD4110_CURRENT_MODE
 * 				  AD4110_CURRENT_MODE_EXT_R_SEL
 * 				  AD4110_THERMOCOUPLE
 * 				  AD4110_FLD_POWER_MODE
 * 				  AD4110_RTD_2W_MODE
 * 				  AD4110_RTD_3W_MODE
 * 				  AD4110_RTD_4W_MODE
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_set_op_mode(struct ad4110_dev *dev, enum ad4110_op_mode mode)
{
	int32_t ret;

	switch(mode) {
	case AD4110_VOLTAGE_MODE:
		// clear IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_DISABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);
		break;
	case AD4110_CURRENT_MODE:
		// set IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_ENABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);

		break;

	case AD4110_CURRENT_MODE_EXT_R_SEL:
		// set IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_ENABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);

		// set EXT_R_SEL
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_AFE_CNTRL2,
						    AD4110_ENABLE,
						    AD4110_REG_AFE_CNTRL2_EXT_R_SEL_MSK);
		break;

	case AD4110_THERMOCOUPLE:
		// clear IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_DISABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);

		// enable VBIAS
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_AFE_CNTRL2,
						    AD4110_AFE_VBIAS(AD4110_AFE_VBIAS_ON),
						    AD4110_AFE_VBIAS(AD4110_AFE_VBIAS_OFF));
		break;

	case AD4110_FLD_POWER_MODE:
		// set IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_ENABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);

		// set EN_FLD_PWR
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_AFE_CNTRL2,
						    AD4110_ENABLE,
						    AD4110_REG_AFE_CNTRL2_EN_FLD_PWR_MSK);

		break;

	case AD4110_RTD_4W_MODE:
		// clear IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_DISABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);
		// clear RTD_3W4W bit
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_PGA_RTD_CTRL,
						    AD4110_DISABLE,
						    AD4110_REG_PGA_RTD_CTRL_23W_EN_MSK);
		break;

	case AD4110_RTD_3W_MODE:
		// clear IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_DISABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);
		// set RTD_3W4W bit
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_PGA_RTD_CTRL,
						    AD4110_ENABLE,
						    AD4110_REG_PGA_RTD_CTRL_23W_EN_MSK);
		break;

	case AD4110_RTD_2W_MODE:
		// clear IMODE bit
		ret = ad4110_spi_int_reg_write_msk(dev,
						   A4110_AFE,
						   AD4110_REG_AFE_CNTRL2,
						   AD4110_DISABLE,
						   AD4110_REG_AFE_CNTRL2_IMODE_MSK);
		// set RTD_3W4W bit
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_PGA_RTD_CTRL,
						    AD4110_ENABLE,
						    AD4110_REG_PGA_RTD_CTRL_23W_EN_MSK);
		// enable VBIAS
		ret |=
			ad4110_spi_int_reg_write_msk(dev,
						     A4110_AFE,
						     AD4110_REG_AFE_CNTRL2,
						     AD4110_AFE_VBIAS(AD4110_AFE_VBIAS_ON),
						     AD4110_AFE_VBIAS(AD4110_AFE_VBIAS_OFF));

		// enable the 100 �A pull-down current source on AIN(-)
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_AFE,
						    AD4110_REG_AFE_CNTRL2,
						    AD4110_REG_AFE_CNTRL2_AINN_DN100,
						    AD4110_REG_AFE_CNTRL2_AINN_DN100);
		break;

	default:
		break;
	}

	return ret;
}

/***************************************************************************//**
 * Do a SPI software reset.
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_spi_do_soft_reset(struct ad4110_dev *dev)
{
	int32_t ret;
	uint8_t buf [ 8 ] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	/* The AD4110 can be reset by writing a series of 64 1�s to the DIN
	 * input */
	ret = spi_write_and_read(dev->spi_dev, buf, 8);

	return ret;
}


/***************************************************************************//**
 * Get the data size of a specified register.
 *
 * @param dev      - The device structure.
 * @param reg_map  - The register map.
 *		     Accepted values: A4110_ADC
 *				      A4110_AFE
 * @param reg_addr - The register address.
 *
 * @return the data size in bytes
*******************************************************************************/
uint8_t ad4110_get_data_size(struct ad4110_dev *dev,
			     uint8_t reg_map,
			     uint8_t reg_addr)
{
	uint8_t data_size;
	data_size = 3; // default size: 1 cmd byte + 2 data bytes

	if(reg_map == A4110_ADC) {
		if(reg_addr == AD4110_REG_ADC_STATUS)
			data_size = 2;
		else if(reg_addr == AD4110_REG_DATA) {
			if(dev->data_length == AD4110_DATA_WL24)
				data_size++;

			if(dev->data_stat == AD4110_ENABLE)
				data_size++;
		} else if(reg_addr >= AD4110_ADC_OFFSET0)
			data_size = 4;
	}

	return data_size;
}

/***************************************************************************//**
 * SPI internal register write to device.
 *
 * @param dev      - The device structure.
 * @param reg_map  - The register map.
 *		     Accepted values: A4110_ADC
 *				      A4110_AFE
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_spi_int_reg_write(struct ad4110_dev *dev,
				 uint8_t reg_map,
				 uint8_t reg_addr,
				 uint32_t reg_data)
{
	uint8_t buf[5];

	uint8_t buf_size;
	uint8_t data_size = 3;
	int32_t ret;

	if(reg_addr >= AD4110_ADC_OFFSET0)
		data_size = 4;

	buf[0] = (reg_map << 7) | AD4110_CMD_WR_COM_REG(reg_addr);

	switch(data_size) {
	case 3:
		buf[1] = (reg_data & 0xFF00) >> 8;
		buf[2] = (reg_data & 0x00FF) >> 0;
		break;

	case 4:
		buf[1] = (reg_data & 0xFF0000) >> 16;
		buf[2] = (reg_data & 0x00FF00) >> 8;
		buf[3] = (reg_data & 0x0000FF) >> 0;
		break;

	default:
		break;
	}

	if(((dev->afe_crc_en != AD4110_AFE_CRC_DISABLE) &&
	    (reg_map == A4110_AFE))||
	    ((dev->adc_crc_en != AD4110_ADC_CRC_DISABLE) &&
	     (reg_map == A4110_ADC))) {
		buf_size = data_size + 1;
		buf[data_size] = ad4110_compute_crc8(&buf[0], data_size);
	} else
		buf_size = data_size;
	ret = spi_write_and_read(dev->spi_dev, buf, buf_size);

	return ret;
}

/***************************************************************************//**
 * SPI internal register read from device.
 *
 * @param dev      - The device structure.
 * @param reg_map  - The register map.
 *		     Accepted values: A4110_ADC
 *				      A4110_AFE
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_spi_int_reg_read(struct ad4110_dev *dev,
				uint8_t reg_map,
				uint8_t reg_addr,
				uint32_t *reg_data)
{
	uint8_t buf[6];

	uint8_t buf_size;
	uint8_t data_size;
	uint32_t data;
	uint8_t crc;
	int32_t ret;

	data_size = ad4110_get_data_size(dev, reg_map, reg_addr);

	buf[0] = (reg_map << 7) | AD4110_CMD_READ_COM_REG(reg_addr); // cmd byte
	buf[1] = 0xAA; // dummy data byte
	buf[2] = 0xAA; // dummy data byte
	buf[3] = 0xAA; // dummy data byte
	buf[4] = 0xAA; // dummy data byte
	buf[5] = 0x00; // dummy data byte

	if(((dev->afe_crc_en != AD4110_AFE_CRC_DISABLE) &&
	    (reg_map == A4110_AFE))||
	    ((dev->adc_crc_en != AD4110_ADC_CRC_DISABLE) &&
	     (reg_map == A4110_ADC)))
		buf_size = data_size + 1; // 1 byte for crc
	else
		buf_size = data_size;

	ret = spi_write_and_read(dev->spi_dev, buf, buf_size);
	switch (data_size) {
	case 3:
		data = (buf[1] << 8) | buf[2];
		break;
	case 4:
		data = (buf[1] << 16) | (buf[2] << 8) | buf[3];
		break;
	case 2:
		data = buf[1];
		break;
	case 5:
		// ADC Data conversion result of 24 bits + status reg append
		data = (buf[1] << 24) | (buf[2] << 16) | (buf[3] << 8) | buf[4];
		break;
	default:
		break;
	}

	*reg_data = data;

	if(((dev->afe_crc_en == AD4110_AFE_CRC) && (reg_map == A4110_AFE))||
	    ((dev->adc_crc_en == AD4110_ADC_CRC_CRC) &&
	     (reg_map == A4110_ADC))) {
		buf[0] = (reg_map << 7) | AD4110_CMD_READ_COM_REG(reg_addr);
		crc = ad4110_compute_crc8(&buf[0], data_size);
		if (crc != buf[buf_size - 1]) {
			printf("%s: CRC Error.\n", __func__);
			ret = FAILURE;
		}
	} else if ((dev->adc_crc_en == AD4110_ADC_XOR_CRC) &&
		   (reg_map == A4110_ADC)) {
		buf[0] = (reg_map << 7) | AD4110_CMD_READ_COM_REG(reg_addr);
		crc = ad4110_compute_xor(&buf[0], data_size);
		if (crc != buf[buf_size - 1]) {
			printf("%s: CRC Error.\n", __func__);
			ret = FAILURE;
		}
	}

	return ret;
}

/***************************************************************************//**
 * Initialize the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_setup(struct ad4110_dev **device,
		     struct ad4110_init_param init_param)
{
	struct ad4110_dev *dev;
	int32_t ret;
	dev = (struct ad4110_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = spi_init(&dev->spi_dev, &init_param.spi_init);

	/* GPIO */
	ret |= gpio_get(&dev->gpio_reset, &init_param.gpio_reset);

	ret |= gpio_direction_output(dev->gpio_reset, GPIO_LOW);
	mdelay(10);
	ret |= gpio_set_value(dev->gpio_reset, GPIO_HIGH);
	mdelay(10);

	/* Device Settings */
	ad4110_spi_do_soft_reset(dev);
	mdelay(10);

	if(init_param.data_stat == AD4110_ENABLE) {
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_ADC,
						    AD4110_REG_ADC_INTERFACE,
						    AD4110_DATA_STAT_EN,
						    AD4110_REG_ADC_INTERFACE_DS_MSK);
	}
	dev->data_stat = init_param.data_stat;

	if(init_param.data_length == AD4110_DATA_WL16) {
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_ADC,
						    AD4110_REG_ADC_INTERFACE,
						    AD4110_DATA_WL16,
						    AD4110_REG_ADC_INTERFACE_WL16_MSK);
	}
	dev->data_length = init_param.data_length;

	if(init_param.afe_crc_en != AD4110_AFE_CRC_DISABLE) {
		ret |= ad4110_spi_int_reg_write(dev,
						A4110_AFE,
						AD4110_REG_AFE_CNTRL1,
						AD4110_REG_AFE_CNTRL1_CRC_EN);
	}
	dev->afe_crc_en = init_param.afe_crc_en;

	if(init_param.adc_crc_en != AD4110_ADC_CRC_DISABLE) {
		ret |= ad4110_spi_int_reg_write_msk(dev,
						    A4110_ADC,
						    AD4110_REG_ADC_INTERFACE,
						    AD4110_ADC_CRC_EN(init_param.adc_crc_en),
						    AD4110_REG_ADC_INTERFACE_CRC_EN_MSK);
	}
	dev->adc_crc_en = init_param.adc_crc_en;

	ret |= ad4110_set_op_mode(dev, init_param.op_mode);
	dev->op_mode = init_param.op_mode;

	ret |= ad4110_set_gain(dev, init_param.gain);
	dev->gain = init_param.gain;

	*device = dev;

	if (!ret)
		printf("AD4110 successfully initialized\n");
	else
		printf("AD4110 initialization error (%d)\n", ret);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad4110_setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad4110_remove(struct ad4110_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_dev);

	ret |= gpio_remove(dev->gpio_reset);

	free(dev);

	return ret;
}
