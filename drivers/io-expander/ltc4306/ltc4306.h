/***************************************************************************//**
 *   @file   ltc4306.h
 *   @brief  Header file of ltc4306 driver.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __LTC4306_H__
#define __LTC4306_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "no_os_i2c.h"
#include "no_os_util.h"

/******************************************************************************/
/************************** LTC4306 Definitions *******************************/
/******************************************************************************/

/* Register address definitions */
#define LTC4306_CTRL_REG0				0x00
#define LTC4306_CTRL_REG1				0x01
#define LTC4306_CTRL_REG2           	0x02
#define LTC4306_CTRL_REG3            	0x03
#define LTC4306_OUT_OF_BOUNDS  	        0x04

/* Special Addresses */
#define LTC4306_MASS_WRITE_ADDR         0xBA
#define LTC4306_ALERT_RESPONSE_ADDR     0x19

/* Masks (Bits and Fields) */
/* REG0 */
#define LTC4306_DOWNSTREAM_CONNECT		NO_OS_BIT(7)
#define LTC4306_ALERT_LOGIC(x)			NO_OS_BIT(7 - (x))
#define LTC4306_FAILED_CONN				NO_OS_BIT(2)
#define LTC4306_LATCHED_TOUT			NO_OS_BIT(1)
#define LTC4306_REALTIME_TOUT			NO_OS_BIT(0)

/* REG1 */
#define LTC4306_UPSTREAM_EN				NO_OS_BIT(7)
#define LTC4306_DOWNSTREAM_EN			NO_OS_BIT(6)
#define LTC4306_OUT_DRV_STATE(x)		NO_OS_BIT(6 - (x))
#define LTC4306_GPIO_LOGIC(x)			NO_OS_BIT(2 - (x))

/* REG2 */
#define LTC4306_GPIO_MODE_MASK			NO_OS_GENMASK(7, 6)
#define LTC4306_GPIO_MODE_CONFIG(x)		NO_OS_BIT(8 - (x))
#define LTC4306_CONN_REQ				NO_OS_BIT(5)
#define LTC4306_OUT_MODE_MASK			NO_OS_GENMASK(4, 3)
#define LTC4306_OUT_MODE_CONFIG(x)		NO_OS_BIT(5 - (x))
#define LTC4306_MASS_WRITE				NO_OS_BIT(2)
#define LTC4306_TOUT					NO_OS_GENMASK(1, 0)

/* REG3 */
#define LTC4306_FET_STATE_MASK			NO_OS_GENMASK(7, 4)
#define LTC4306_FET_STATE(x)			NO_OS_BIT(8 - (x))
#define LTC4306_BUS_LOGIC_STATE_MASK	NO_OS_GENMASK(3, 0)
#define LTC4306_LOGIC_STATE(x)			NO_OS_BIT(4 - (x))

/* Other definitions */
#define LTC4306_MIN_CHANNEL_INDEX		1
#define LTC4306_CHANNEL_TWO				2
#define LTC4306_CHANNEL_THREE			3
#define LTC4306_MAX_CHANNEL_INDEX		4
#define LTC4306_GPIO_MIN				1
#define LTC4306_GPIO_MAX				2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ltc4306_addr_conn {
	LTC4306_LOW,
	LTC4306_HIGH,
	LTC4306_NO_CONN,
};

enum ltc4306_timeout_mode {
	LTC4306_DISABLED,
	LTC4306_30MS,
	LTC4306_15MS,
	LTC4306_7P5MS,
};

struct ltc4306_dev {
	struct no_os_i2c_desc	*i2c_desc;
	/* GPIO status indicators (input or ouptut) */
	bool is_input[LTC4306_GPIO_MAX];
	/* GPIO out mode indicators (push pull or open drain */
	bool is_push_pull[LTC4306_GPIO_MAX];
	/* FET status indicators (connected or not) */
	bool is_closed[LTC4306_MAX_CHANNEL_INDEX];
	/* Upstream accelerator indicator */
	bool upstream;
	/* Downstream accelerator indicator */
	bool downstream;
};

struct ltc4306_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Generate slave address depending on A0 and A1 logic levels */
int ltc4306_addr_gen (struct ltc4306_init_param *init_param,
		      enum ltc4306_addr_conn addr0, enum ltc4306_addr_conn addr1,
		      enum ltc4306_addr_conn addr2);

/* Initialize device */
int ltc4306_init(struct ltc4306_dev **device,
		 struct ltc4306_init_param init_param);

/* Write byte/s to selected starting register */
int ltc4306_write(struct ltc4306_dev *dev, uint8_t addr, uint8_t *write_data,
		  uint8_t bytes);

/* Read byte/s from selected starting register */
int ltc4306_read(struct ltc4306_dev *dev, uint8_t addr, uint8_t *read_data,
		 uint8_t bytes);

/* Update selected register using mask and desired value */
int ltc4306_reg_update(struct ltc4306_dev *dev, uint8_t addr, int update_mask,
		       int update_val);

/* Deallocate resources for device */
int ltc4306_remove(struct ltc4306_dev *dev);

/* Checks if any downstream bus is connected to upstream bus */
int ltc4306_downstream_check(struct ltc4306_dev *dev, bool *downstream_conn);

/* Reads the status of selected ALERT pin */
int ltc4306_read_alert_logic_state(struct ltc4306_dev *dev,
				   uint8_t alert_pin_number, bool *is_high);

/* Checks if any downstream connection attempt failed */
int ltc4306_read_failed_conn(struct ltc4306_dev *dev, bool *is_high);

/* Checks if any latched timeout occurs */
int ltc4306_get_latched_timeout(struct ltc4306_dev *dev, bool *timed_out);

/* Check status of stuck low timeout circuitry */
int ltc4306_get_realtime_timeout(struct ltc4306_dev *dev, bool *timed_out);

/* Sets status of upstream accelerator */
int ltc4306_set_upstream_accel(struct ltc4306_dev *dev, bool upstream_en);

/* Gets status of upstream accelerator */
int ltc4306_get_upstream_accel(struct ltc4306_dev *dev, bool *upstream_en);

/* Sets status of downstream accelerator */
int ltc4306_set_downstream_accel(struct ltc4306_dev *dev, bool downstream_en);

/* Gets status of downstream accelerator */
int ltc4306_get_downstream_accel(struct ltc4306_dev *dev, bool *downstream_en);

/* Sets GPIO Output Driver state */
int ltc4306_set_gpio_output_state(struct ltc4306_dev *dev, int gpio,
				  bool is_high);

/* Gets GPIO Output Driver state */
int ltc4306_get_gpio_output_state(struct ltc4306_dev *dev, int gpio,
				  bool *is_high);

/* Reads logic level of selected GPIO */
int ltc4306_read_gpio_logic_state(struct ltc4306_dev *dev, int gpio,
				  bool *is_high);

/* Sets the LTC4306's connection requirement bit field for downstream connect */
int ltc4306_set_conn_req(struct ltc4306_dev *dev, bool connect_regardless);

/* Gets the LTC4306's connection requirement bit field for downstream connect */
int ltc4306_get_conn_req(struct ltc4306_dev *dev, bool *connect_regardless);

/* Configure LTC4306's GPIO mode and output config (for output) */
int ltc4306_gpio_configure(struct ltc4306_dev *dev, bool gpio1,
			   bool gpio2, bool gpio1_is_pushpull, bool gpio2_is_pushpull);

/* Sets the mass write bit field */
int ltc4306_set_mass_write(struct ltc4306_dev *dev, bool mass_write_en);

/* Gets the mass write bit field */
int ltc4306_get_mass_write(struct ltc4306_dev *dev, bool *mass_write_en);

/* Sets the LTC4306's stuck low timeout mode */
int ltc4306_set_timeout_mode(struct ltc4306_dev *dev,
			     enum ltc4306_timeout_mode tout);

/* Gets the LTC4306's stuck low timeout mode */
int ltc4306_get_timeout_mode(struct ltc4306_dev *dev,
			     enum ltc4306_timeout_mode *tout);

/* Reads LTC4306's selected Bus Logic */
int ltc4306_read_bus_logic_state(struct ltc4306_dev *dev, uint8_t bus_number,
				 bool *is_high);

/* Sets the connection for selected LTC4306 downstream channel */
int ltc4306_set_downstream_channel(struct ltc4306_dev *dev,
				   uint8_t bus_num, bool connect);

/* Gets the connection for selected LTC4306 downstream channel */
int ltc4306_get_downstream_channel(struct ltc4306_dev *dev,
				   uint8_t bus_num, bool *connect);

#endif	/* __LTC4306_H__ */
