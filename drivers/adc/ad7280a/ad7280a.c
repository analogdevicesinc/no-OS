/**************************************************************************//**
*   @file   ad7280a.c
*   @brief  Driver for the AD7280A Lithium Ion Battery Monitoring System
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad7280a.h"
#include "no_os_alloc.h"

/*****************************************************************************/
/************************ Functions Definitions ******************************/
/*****************************************************************************/

/******************************************************************************
 * @brief Initializes the communication with the device.
 *
 * @param device     - The device structure.
 *        init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
******************************************************************************/
int8_t ad7280a_init(struct ad7280a_dev **device,
		    struct ad7280a_init_param init_param)
{
	struct ad7280a_dev *dev;
	int8_t status;
	uint32_t value;

	dev = (struct ad7280a_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* GPIO */
	status = no_os_gpio_get(&dev->gpio_pd, &init_param.gpio_pd);
	status |= no_os_gpio_get(&dev->gpio_cnvst, &init_param.gpio_cnvst);
	status |= no_os_gpio_get(&dev->gpio_alert, &init_param.gpio_alert);

	AD7280A_PD_OUT;
	AD7280A_PD_HIGH;
	AD7280A_CNVST_OUT;
	AD7280A_CNVST_HIGH;
	AD7280A_ALERT_IN;

	/* Wait 250us */
	no_os_mdelay(250);

	status |= no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* Example 1 from the datasheet */
	/* Configure the Control LB register for all devices */
	value = ad7280a_crc_write((uint32_t) (AD7280A_CONTROL_LB << 21) |
				  ((AD7280A_CTRL_LB_MUST_SET |
				    AD7280A_CTRL_LB_LOCK_DEV_ADDR |
				    AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN) << 13) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Configure the Read Register for all devices */
	value = ad7280a_crc_write((uint32_t) (AD7280A_READ << 21) |
				  (AD7280A_CONTROL_LB << 15) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Read master address */
	value = ad7280a_transfer_32bits(dev,
					AD7280A_READ_TXVAL);
	//printf("Master address=0x%x\r\n",(value >> 27));
	/* Read slave address */
	value = ad7280a_transfer_32bits(dev,
					AD7280A_READ_TXVAL);
	//printf("Slave address=0x%x\r\n",(value >> 27));

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD7280A_Init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7280a_remove(struct ad7280a_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_pd);
	ret |= no_os_gpio_remove(dev->gpio_cnvst);
	ret |= no_os_gpio_remove(dev->gpio_alert);

	no_os_free(dev);

	return ret;
}

/******************************************************************************
* @brief Reads/transmits 32 data bits from/to AD7280A.
 *
 * @param dev           - The device structure.
 *        data          - Data to be transmitted (MOSI pin).
 *
 * @return receivedData - Received data (MISO pin).
******************************************************************************/
uint32_t ad7280a_transfer_32bits(struct ad7280a_dev *dev,
				 uint32_t data)
{

	uint8_t data_buf[4];
	uint32_t received_data;

	data_buf[0] = (data >> 24) & 0xff;
	data_buf[1] = (data >> 16) & 0xff;
	data_buf[2] = (data >> 8)  & 0xff;
	data_buf[3] = (data >> 0)  & 0xff;

	no_os_spi_write_and_read(dev->spi_desc,
				 (uint8_t*)data_buf,
				 4);

	received_data = ((uint32_t)data_buf[0] << 24) +
			((uint32_t)data_buf[1] << 16) +
			((uint32_t)data_buf[2] << 8)  +
			((uint32_t)data_buf[3] << 0);

	return received_data;
}

/******************************************************************************
 * @brief Computes the CRC value for a write transmission, and prepares the
 *        complete write codeword
 *
 * @param message : The message that is to be transmitted. The least significant
 *                  11 bits are discarded
 *
 * @return Complete codeword that can be transmitted to AD7280A
******************************************************************************/
uint32_t ad7280a_crc_write(uint32_t message)
{
	int32_t i;
	int32_t crc;
	int32_t xor1 = 0;
	int32_t xor2 = 0;
	int32_t xor3 = 0;
	int32_t xor4 = 0;
	int32_t xor5 = 0;
	uint32_t data_out = 0;
	int32_t crc0 = 0;
	int32_t crc1 = 0;
	int32_t crc2 = 0;
	int32_t crc3 = 0;
	int32_t crc4 = 0;
	int32_t crc5 = 0;
	int32_t crc6 = 0;
	int32_t crc7 = 0;

	message = message >> 11;
	for (i = NUMBITS_WRITE; i >= 0; i--) {
		xor5 = (crc4 ^ crc7) ? 1 : 0;
		xor4 = (crc2 ^ crc7) ? 1 : 0;
		xor3 = (crc1 ^ crc7) ? 1 : 0;
		xor2 = (crc0 ^ crc7) ? 1 : 0;
		xor1 = ((message & (1 << i)) ? 1 : 0) ^ (crc7 ? 1 : 0);
		crc7 = crc6;
		crc6 = crc5;
		crc5 = xor5;
		crc4 = crc3;
		crc3 = xor4;
		crc2 = xor3;
		crc1 = xor2;
		crc0 = xor1;
	}
	crc = (crc0 ? (1 << 0) : 0) | (crc1 ? (1 << 1) : 0) | (crc2 ? (1 << 2) : 0) |
	      (crc3 ? (1 << 3) : 0) | (crc4 ? (1 << 4) : 0) | (crc5 ? (1 << 5) : 0) |
	      (crc6 ? (1 << 6) : 0) | (crc7 ? (1 << 7) : 0);
	data_out = (message << 11) | (crc << 3) | 2;

	return data_out;
}

/******************************************************************************
 * @brief Checks the received message if the received CRC and computed CRC are
 *        the same.
 *
 * @param message : The received message
 *
 * @return 1 if the two CRC are identical
 *         0 otherwise
******************************************************************************/
int32_t ad7280a_crc_read(uint32_t message)
{
	int32_t i;
	uint32_t data_in = 0;
	int32_t crc_rec;
	int32_t crc_computed;
	int32_t xor1 = 0;
	int32_t xor2 = 0;
	int32_t xor3 = 0;
	int32_t xor4 = 0;
	int32_t xor5 = 0;
	int32_t crc0 = 0;
	int32_t crc1 = 0;
	int32_t crc2 = 0;
	int32_t crc3 = 0;
	int32_t crc4 = 0;
	int32_t crc5 = 0;
	int32_t crc6 = 0;
	int32_t crc7 = 0;

	crc_rec= (message >> 2) & 0xFF;
	data_in = message >> 10;
	for (i = NUMBITS_READ; i >= 0; i--) {
		xor5 = (crc4 ^ crc7) ? 1 : 0;
		xor4 = (crc2 ^ crc7) ? 1 : 0;
		xor3 = (crc1 ^ crc7) ? 1 : 0;
		xor2 = (crc0 ^ crc7) ? 1 : 0;
		xor1 = ((data_in & (1 << i)) ? 1 : 0) ^ (crc7 ? 1 : 0);
		crc7 = crc6;
		crc6 = crc5;
		crc5 = xor5;
		crc4 = crc3;
		crc3 = xor4;
		crc2 = xor3;
		crc1 = xor2;
		crc0 = xor1;
	}
	crc_computed = (crc0 ? (1 << 0) : 0) | (crc1 ? (1 << 1) : 0) |
		       (crc2 ? (1 << 2) : 0) | (crc3 ? (1 << 3) : 0) |
		       (crc4 ? (1 << 4) : 0) | (crc5 ? (1 << 5) : 0) |
		       (crc6 ? (1 << 6) : 0) | (crc7 ? (1 << 7) : 0);
	if (crc_rec == crc_computed) {
		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************
 * @brief Performs a read from all registers on 2 devices.
 *
 * @param dev - The device structure.
 *
 * @return 1.
******************************************************************************/
int8_t ad7280a_convert_read_all(struct ad7280a_dev *dev)
{
	uint8_t i;
	//long err = 1;
	uint32_t value;

	/* Configure Control HB register. Read all register, convert all registers,
	average 8 values for all devices */
	value = ad7280a_crc_write((uint32_t) (AD7280A_CONTROL_HB << 21) |
				  ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
				    AD7280A_CTRL_HB_CONV_INPUT_ALL |
				    AD7280A_CTRL_HB_CONV_AVG_8) << 13) |
				  (1 << 12));
	/* Configure the Read register for all devices */
	ad7280a_transfer_32bits(dev,
				value);
	value = ad7280a_crc_write((uint32_t) (AD7280A_READ << 21) |
				  (AD7280A_CELL_VOLTAGE_1 << 15) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Configure the CNVST register, allow single CNVST pulse */
	value = ad7280a_crc_write((uint32_t) (AD7280A_CNVST_N_CONTROL << 21) |
				  (2 << 13) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Wait 100us */
	no_os_mdelay(100);
	/* Toggle CNVST pin */
	AD7280A_CNVST_LOW;
	/* Wait 50us */
	no_os_mdelay(50);
	AD7280A_CNVST_HIGH;
	/* Wait 300us */
	no_os_mdelay(300);
	/* Read data from both devices */
	for(i = 0; i < 24; i++) {
		dev->read_data[i] = ad7280a_transfer_32bits(dev,
				    AD7280A_READ_TXVAL);
	}

	// Optionally check all received data to make sure CRC is correct
	/*for(i=0;i<24;i++)
	{
	    if ( crc_read(dev->readData[i]) == 0 )
	    {
	        err = 0;
	    }
	}*/

	/* Convert the received data to float values. */
	ad7280a_convert_data_all(dev);

	return (1);
}

/******************************************************************************
 * @brief Converts acquired data from all channels to float values.
 *
 * @param dev - The device structure.
 *
 * @return 1.
******************************************************************************/
int8_t ad7280a_convert_data_all(struct ad7280a_dev *dev)
{
	uint8_t i;

	for(i = 0; i < 6; i++) {
		dev->cell_voltage[i]     = 1 + ((dev->read_data[i]    >> 11) & 0xfff) *
					   0.0009765625;
		dev->aux_adc[i]          = ((dev->read_data[i+6]      >> 11) & 0xfff) *
					   0.001220703125;
		dev->cell_voltage[i + 6] = 1 + ((dev->read_data[i+12] >> 11) & 0xfff) *
					   0.0009765625;
		dev->aux_adc[i + 6]      = ((dev->read_data[i+18]     >> 11) & 0xfff) *
					   0.001220703125;
	}

	return (1);
}

/******************************************************************************
 * @brief Reads the register content of one selected register.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from the
 *                   selected device (only registers from Register Map with
 *                   address higher or equal to 0x0D).
 *
 * @return readContent - register content.
******************************************************************************/
int16_t ad7280a_read_register(struct ad7280a_dev *dev,
			      uint8_t dev_addr,
			      uint8_t read_reg)
{
	uint32_t value;
	int16_t read_content;
	//unsigned char readDevAddress;
	//unsigned char readRegAddress;
	//unsigned char readWriteAck;


	/* Example 6 from datasheet */
	/* Disable reading on all devices */
	value = ad7280a_crc_write((uint32_t) (AD7280A_CONTROL_HB << 21) |
				  (AD7280A_CTRL_HB_CONV_RES_READ_NO << 13) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Enable reading on the selected device */
	value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
				  (AD7280A_CONTROL_HB << 21) |
				  ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
				    AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13));
	ad7280a_transfer_32bits(dev,
				value);
	/* Wait 100us */
	no_os_mdelay(100);
	/* Configure the Read register */
	value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
				  (AD7280A_READ << 21) |
				  (read_reg << 15));
	ad7280a_transfer_32bits(dev,
				value);
	/* Perform the read */
	value = ad7280a_transfer_32bits(dev,
					AD7280A_READ_TXVAL);
	if (ad7280a_crc_read(value) == 1) {
		//readDevAddress = value >> 27;              // Extract the Device address
		//readRegAddress = (value >> 21 ) & 0x2F;    // Extract the Register address
		read_content    = (value >> 13 ) & 0xFF;    // Extract the Register data
		//readWriteAck   = (value & 0x200) ? 1 : 0;  // Extract the write acknowledge
	} else {
		read_content = -1;
	}

	return read_content;
}

/******************************************************************************
 * @brief Reads the conversion of one channel.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from
 *                   selected device (only Cell_Voltage_x or AUX_ADC_x).
 *
 * @return readConversion - ADC code or -1 if an error occured
******************************************************************************/
int16_t ad7280a_read_conversion(struct ad7280a_dev *dev,
				uint8_t dev_addr,
				uint8_t read_reg)
{
	uint32_t value;
	int16_t read_conversion;
	//unsigned char  readDevAddress;
	//unsigned char  readRegAddress;
	//unsigned char  readWriteAck;

	/* Example 4 from datasheet */
	/* Write the register address to Read Register */
	value = ad7280a_crc_write((uint32_t)(dev_addr << 31) |
				  (AD7280A_READ << 21) |
				  (read_reg << 15));
	ad7280a_transfer_32bits(dev,
				value);
	/* Disable reading on all devices */
	value = ad7280a_crc_write((uint32_t)(AD7280A_CONTROL_HB << 21) |
				  (AD7280A_CTRL_HB_CONV_RES_READ_NO << 13) |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
	/* Wait 100us */
	no_os_mdelay(100);
	/*  */
	value = ad7280a_crc_write((uint32_t)(dev_addr << 31) |
				  (AD7280A_CONTROL_HB << 21) |
				  ((AD7280A_CTRL_HB_CONV_RES_READ_ALL |
				    AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13));
	ad7280a_transfer_32bits(dev,
				value);
	/* Wait 100us */
	no_os_mdelay(100);
	/* Allow conversions to be initiated using CNVST pin on selected part */
	value=ad7280a_crc_write((uint32_t)(dev_addr << 31) |
				(AD7280A_CNVST_N_CONTROL << 21) |
				(2 << 13));
	/* Write the CNVST_N register, allow single CNVST pulse */
	ad7280a_transfer_32bits(dev,
				value);
	/* Initiate conversions through the falling edge of CNVST */
	AD7280A_CNVST_LOW;
	/* Allow sufficient time for all conversions to be completed */
	/* Wait 50us */
	no_os_mdelay(50);
	AD7280A_CNVST_HIGH;
	/* Wait 300us */
	no_os_mdelay(300);
	/* Perform the read */
	value = ad7280a_transfer_32bits(dev,
					AD7280A_READ_TXVAL);

	if (ad7280a_crc_read(value) == 1) {
		//readDevAddress = value >> 27;              // Extract the Device address
		//readRegAddress = (value >> 23 ) & 0x2F;    // Extract the Register address
		read_conversion = (value >> 11 ) & 0xFFF;   // Extract the Register data
		//readWriteAck   = (value & 0x200) ? 1 :0;   //Extract the write acknowledge
	} else {
		read_conversion = -1;
	}

	return read_conversion;
}

/******************************************************************************
 * @brief Converts the input data to a voltage value.
 *
 * @param  type - 0 = cell voltage input calculation.
 *              - 1 = auxiliary ADC input calculation.
 *         data - ADC code to be converted to volts.
 *
 * @return result - calculated voltage.
******************************************************************************/
float ad7280a_convert_data(uint8_t type,
			   uint16_t data)
{
	float result = 0;

	data = data & 0xfff;
	if (type == 0) {
		result = 1 + ((float)data * 0.0009765625);
	} else {
		if (type == 1) {
			result = (float)data * 0.001220703125;
		}
	}

	return result;
}

/******************************************************************************
 * @brief Writes the content of one selected register from the selected device.
 *
 * @param  dev     - The device structure.
 *         devAddr - the device address
 *                   Example: 0 - master.
 *                            1 - slave.
 *         readReg - the address of the register to be read from the
 *                   selected device (only registers from Register Map with
 *                   address higher or equal to 0x0D).
 *
 * @return none.
******************************************************************************/
void ad7280a_write_register(struct ad7280a_dev *dev,
			    uint8_t dev_addr,
			    uint8_t read_reg,
			    uint8_t reg_val)
{
	uint32_t value = 0;

	switch (read_reg) {
	case 0x0D:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CONTROL_HB << 21)      |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x0E:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CONTROL_LB << 21)      |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x0F:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31)  |
					  (AD7280A_CELL_OVERVOLTAGE << 21) |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x10:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31)   |
					  (AD7280A_CELL_UNDERVOLTAGE << 21) |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x11:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31)     |
					  (AD7280A_AUX_ADC_OVERVOLTAGE << 21) |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x12:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31)      |
					  (AD7280A_AUX_ADC_UNDERVOLTAGE << 21) |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x13:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_ALERT << 21)           |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x14:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CELL_BALANCE << 21)    |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x15:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB1_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x16:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB2_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x17:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB3_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x18:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB4_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x19:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB5_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x1A:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CB6_TIMER << 21)       |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x1B:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_PD_TIMER << 21)        |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x1C:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_READ << 21)            |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	case 0x1D:
		value = ad7280a_crc_write((uint32_t) (dev_addr << 31) |
					  (AD7280A_CNVST_N_CONTROL << 21) |
					  (reg_val << 13));
		ad7280a_transfer_32bits(dev,
					value);
		break;
	}
}

/******************************************************************************
 * @brief Performs the self test for two devices(one master and one slave).
 *
 * @param   dev           - The device structure.
 *          *selfTestRegA - the voltage corresponding to self test master
 *                          register.
 *          *selfTestRegB - the voltage corresponding to self test slave
 *                          register.
 *
 * @return none.
******************************************************************************/
void ad7280a_selftest_all(struct ad7280a_dev *dev,
			  float *self_test_reg_a,
			  float *self_test_reg_b)
{
	uint32_t value;

	/* Example 7 of AD7280 datasheet */
	value = ad7280a_crc_write((uint32_t) (AD7280A_CONTROL_HB << 21)    |
				  ((AD7280A_CTRL_HB_CONV_RES_READ_ALL           |
				    AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST) << 13) |
				  (1 << 12));
	/* Select self test conversion and read on all parts */
	ad7280a_transfer_32bits(dev,
				value);
	/* Wait 100us */
	no_os_mdelay(100);
	value = ad7280a_crc_write((uint32_t) (AD7280A_READ << 21) |
				  (AD7280A_SELF_TEST << 15)            |
				  (1 << 12));
	/* Write the read register with the address of the Self test register */
	ad7280a_transfer_32bits(dev,
				value);
	value = ad7280a_crc_write((uint32_t) (AD7280A_CNVST_N_CONTROL << 21) |
				  (2 << 13)                       |
				  (1 << 12));
	/* Write the CNVST_N register, allow single CNVST pulse */
	ad7280a_transfer_32bits(dev,
				value);
	AD7280A_CNVST_LOW;
	/* wait 100us */
	no_os_mdelay(100);
	AD7280A_CNVST_HIGH;
	/* wait 300us */
	no_os_mdelay(300);
	value = ad7280a_crc_write((uint32_t) (AD7280A_CNVST_N_CONTROL << 21) |
				  (1 << 13)                       |
				  (1 << 12));
	/* Update registers */
	ad7280a_transfer_32bits(dev,
				value);
	value = (ad7280a_transfer_32bits(dev,
					 AD7280A_READ_TXVAL) >> 11) & 0xfff;
	/* Read master self test register */
	*self_test_reg_a = (float)value * 0.001220703125;
	value = (ad7280a_transfer_32bits(dev,
					 AD7280A_READ_TXVAL) >> 11) & 0xfff;
	/* Read slave self test register */
	*self_test_reg_b = (float)value * 0.001220703125;
	value = ad7280a_crc_write((uint32_t) (AD7280A_CONTROL_HB << 21) |
				  ((AD7280A_CTRL_HB_CONV_RES_READ_ALL        |
				    AD7280A_CTRL_HB_CONV_INPUT_ALL) << 13)     |
				  (1 << 12));
	ad7280a_transfer_32bits(dev,
				value);
}

/******************************************************************************
 * @brief Reads the value of Alert Pin from the device.
 *
 * @param dev - The device structure.
 *
 * @return alertAD7280A - the value read of the pin
 *                        Example: 0 - the pin is low.
 *                                 1 - the pin is high.
******************************************************************************/
uint8_t ad7280a_alert_pin(struct ad7280a_dev *dev)
{
	uint8_t alert_ad7280a = 0;

	no_os_gpio_get_value(dev->gpio_alert,
			     &alert_ad7280a);

	return alert_ad7280a;
}
