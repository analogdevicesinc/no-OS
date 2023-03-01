/***************************************************************************//**
 *   @file   ADP5589.c
 *   @brief  Implementation of ADP5589 Driver.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#include "adp5589.h"			// ADP5589 definitions.
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param register_value   - Data value to write.
 *
 * @return None.
*******************************************************************************/
void adp5589_set_register_value(struct adp5589_dev *dev,
				uint8_t register_address,
				uint8_t register_value)
{
	static uint8_t write_data[2] = {0, 0};

	write_data[0] = register_address;
	write_data[1] = register_value;
	no_os_i2c_write(dev->i2c_desc, write_data, 2, 1);
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 *
 * @return register_value  - Value of the register.
*******************************************************************************/
uint8_t adp5589_get_register_value(struct adp5589_dev *dev,
				   uint8_t register_address)
{
	static uint8_t read_data[2]   = {0, 0};
	static uint8_t register_value = 0;

	read_data[0] = register_address;
	no_os_i2c_write(dev->i2c_desc, read_data, 1, 0);
	no_os_i2c_read(dev->i2c_desc, read_data, 1, 1);
	register_value = read_data[0];

	return register_value;
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the ADP5589
 *		  part is present.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or
 *                                ADP5589 part is not present.
 *                            0 - I2C peripheral is initialized and ADP5589
 *                                part is present.
*******************************************************************************/
int8_t adp5589_init(struct adp5589_dev **device,
		    struct adp5589_init_param init_param)
{
	struct adp5589_dev *dev;
	static uint8_t status;

	dev = (struct adp5589_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	if((adp5589_get_register_value(dev,ADP5589_ADR_ID) & ADP5589_ID_MAN_ID) !=
	    ADP5589_ID) {
		status = -1;
	}
	/* Enable internal oscillator and set clock frequency to 500 kHz. */
	adp5589_set_register_value(dev,
				   ADP5589_ADR_GENERAL_CFG_B,
				   ADP5589_GENERAL_CFG_B_OSC_EN|
				   ADP5589_GENERAL_CFG_B_CORE_FREQ(3));

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adp5589_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adp5589_remove(struct adp5589_dev *dev)
{
	int32_t ret;

	ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the PWM generator in continuous mode.
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void adp5589_init_pwm(struct adp5589_dev *dev)
{
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PWM_CFG,
				   ADP5589_PWM_CFG_PWM_EN);
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PIN_CONFIG_D,
				   ADP5589_PIN_CONFIG_D_R3_EXTEND(2));
}

/***************************************************************************//**
 * @brief Initializes keyboard decoder.
 *
 * @param dev        - The device structure.
 * @param pmod_port  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *			         PMOD_IOXP_J2 - J2 connector.
 *
 * @return None.
*******************************************************************************/
void adp5589_init_key(struct adp5589_dev *dev,
		      uint8_t pmod_port)
{
	if(!pmod_port) {
		/* Config row 0,1,2,3 */
		adp5589_set_register_value(dev,
					   ADP5589_ADR_PIN_CONFIG_A,
					   0x0F);
		/* Config column 0,1,2,3 */
		adp5589_set_register_value(dev,
					   ADP5589_ADR_PIN_CONFIG_B,
					   0x0F);
	} else {
		/* Config row 4,5,6,7 */
		adp5589_set_register_value(dev,
					   ADP5589_ADR_PIN_CONFIG_A,
					   0xF0);
		/* Config column 4,5,6,7 */
		adp5589_set_register_value(dev,
					   ADP5589_ADR_PIN_CONFIG_B,
					   0xF0);
	}
}

/***************************************************************************//**
 * @brief Sets the PWM On and Off times.
 *
 * @param dev          - The device structure.
 * @param pwm_off_time - The amount of time in uS for which the PWM pulse is LOW.
 * @param pwm_on_time  - The amount of time in uS for which the PWM pulse is HIGH.
 *
 * @return None.
*******************************************************************************/
void adp5589_set_pwm(struct adp5589_dev *dev,
		     uint16_t pwm_off_time,
		     uint16_t pwm_on_time)
{
	uint8_t data = 0;

	data = (uint8_t)(pwm_off_time >> 8);
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PWM_OFFT_HIGH,
				   data);
	data = (uint8_t)(pwm_off_time & 0xFF);
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PWM_OFFT_LOW,
				   data);
	data = (uint8_t)(pwm_on_time & 0xFF);
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PWM_ONT_LOW,
				   data);
	data = (uint8_t)(pwm_on_time >> 8);
	adp5589_set_register_value(dev,
				   ADP5589_ADR_PWM_ONT_HIGH,
				   data);
}
/***************************************************************************//**
 * @brief Sets the direction of the pins.
 *
 * @param dev  - The device structure.
 * @param dev  - The device structure.
 * @param reg  - The address of the direction register to be written.
 * @param val  - The data to be written to the direction register.
 *
 * @return None.
*******************************************************************************/
void adp5589_gpio_direction(struct adp5589_dev *dev,
			    uint8_t reg,
			    uint8_t val)
{
	adp5589_set_register_value(dev,
				   reg,
				   val);
}
/***************************************************************************//**
 * @brief Reads the state of the pins.
 *
 * @param dev  - The device structure.
 * @param reg  - The address of the status register to be read.
 *
 * @return val - The value of the register.
*******************************************************************************/
uint8_t adp5589_get_pin_state(struct adp5589_dev *dev,
			      uint8_t reg)
{
	uint8_t val = 0;

	val = adp5589_get_register_value(dev,
					 reg);

	return val;
}

/***************************************************************************//**
 * @brief Sets the state of the pins.
 *
 * @param dev    - The device structure.
 * @param reg    - The address of the status register to be read.
 * @param state  - The value to be written to the pins.
 *
 * @return none
*******************************************************************************/
void adp5589_set_pin_state(struct adp5589_dev *dev,
			   uint8_t reg,
			   uint8_t state)
{
	adp5589_set_register_value(dev,
				   reg,
				   state);
}

/***************************************************************************//**
 * @brief Decodes the key on the Pmod-KYPD.
 *
 * @param reg  	     - The value of the FIFO register
 * @param pmod_port  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *                               PMOD_IOXP_J2 - J2 connector.
 * @param event_type - Pressed or released.
 *                     Example: ADP5589_EVENT_KEY_RELEASED
 *                              ADP5589_EVENT_KEY_PRESSED
 *
 * @return key       - Actual Key on the Pmod-KYPD.
*******************************************************************************/
uint8_t adp5589_key_decode(uint8_t reg,
			   uint8_t event_type,
			   uint8_t pmod_port)
{
	uint8_t key = 0;

	reg -= 0x30 * pmod_port;
	if(event_type == ADP5589_EVENT_KEY_PRESSED) {
		reg -= 0x80;
	}
	switch(reg) {
	case 0x25:
		key = '1';
		break;
	case 0x24:
		key = '4';
		break;
	case 0x23:
		key = '7';
		break;
	case 0x1A:
		key = '2';
		break;
	case 0x19:
		key = '5';
		break;
	case 0x18:
		key = '8';
		break;
	case 0x0F:
		key = '3';
		break;
	case 0x0E:
		key = '6';
		break;
	case 0x0D:
		key = '9';
		break;
	case 0x04:
		key = 'A';
		break;
	case 0x22:
		key = '0';
		break;
	case 0x17:
		key = 'F';
		break;
	case 0x0C:
		key = 'E';
		break;
	case 0x01:
		key = 'D';
		break;
	case 0x02:
		key = 'C';
		break;
	case 0x03:
		key = 'B';
		break;
	default:
		key = 'x';
		break;
	}

	return key;
}

/***************************************************************************//**
 * @brief Locks the ADP5589 and requests Password for unlock.
 *
 * @param dev        - The device structure.
 * @param first_event - First event occured.
 * @param second_event - Second event occured.
 * @param pmod_port  - J1 or J2 connector of PmodIOXP
 *			Example: PMOD_IOXP_J1 - J1 connector.
 *				 PMOD_IOXP_J2 - J2 connector.
 *
 * @return key       - Actual Key on the Pmod-KYPD.
*******************************************************************************/
void adp5589_key_lock(struct adp5589_dev *dev,
		      uint8_t first_event,
		      uint8_t second_event,
		      uint8_t pmod_port)
{
	uint8_t data = 0;

	adp5589_set_register_value(dev,
				   ADP5589_ADR_UNLOCK1,
				   ADP5589_UNLOCK1_UNLOCK1_STATE | (first_event + 0x30 * pmod_port));
	adp5589_set_register_value(dev,
				   ADP5589_ADR_UNLOCK2,
				   ADP5589_UNLOCK2_UNLOCK2_STATE | (second_event + 0x30 * pmod_port));
	adp5589_set_register_value(dev,
				   ADP5589_ADR_LOCK_CFG, ADP5589_LOCK_CFG_LOCK_EN);
	do {
		data = adp5589_get_register_value(dev,
						  ADP5589_ADR_STATUS);
	} while((data & ADP5589_STATUS_LOCK_STAT) != 0);
}
