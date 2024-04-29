/*******************************************************************************
*   @file   ltc2992.h
*   @brief  Header file of the LTC2992 Driver
*   @authors Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __LTC2992_H__
#define __LTC2992_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"

#define LTC2992_REG_CTRLA			0x00
#define LTC2992_REG_CTRLB			0x01
#define LTC2992_REG_ALERT1			0x02
#define LTC2992_REG_FAULT1			0x03
#define LTC2992_REG_NADC			0x04
#define LTC2992_REG_POWER1			0x05
#define LTC2992_REG_POWER1_MAX			0x08
#define LTC2992_REG_POWER1_MIN			0x0B
#define LTC2992_REG_POWER1_MAX_THRESH		0x0E
#define LTC2992_REG_POWER1_MIN_THRESH		0x11
#define LTC2992_REG_DSENSE1			0x14
#define LTC2992_REG_DSENSE1_MAX			0x16
#define LTC2992_REG_DSENSE1_MIN			0x18
#define LTC2992_REG_DSENSE1_MAX_THRESH		0x1A
#define LTC2992_REG_DSENSE1_MIN_THRESH		0x1C
#define LTC2992_REG_SENSE1			0x1E
#define LTC2992_REG_SENSE1_MAX			0x20
#define LTC2992_REG_SENSE1_MIN			0x22
#define LTC2992_REG_SENSE1_MAX_THRESH		0x24
#define LTC2992_REG_SENSE1_MIN_THRESH		0x26
#define LTC2992_REG_G1				0x28
#define LTC2992_REG_G1_MAX			0x2A
#define LTC2992_REG_G1_MIN			0x2C
#define LTC2992_REG_G1_MAX_THRESH		0x2E
#define LTC2992_REG_G1_MIN_THRESH		0x30
#define LTC2992_REG_ALERT2			0x34
#define LTC2992_REG_FAULT2			0x35
#define LTC2992_REG_G2				0x5A
#define LTC2992_REG_G2_MAX			0x5C
#define LTC2992_REG_G2_MIN			0x5E
#define LTC2992_REG_G2_MAX_THRESH		0x60
#define LTC2992_REG_G2_MIN_THRESH		0x62
#define LTC2992_REG_G3				0x64
#define LTC2992_REG_G3_MAX			0x66
#define LTC2992_REG_G3_MIN			0x68
#define LTC2992_REG_G3_MAX_THRESH		0x6A
#define LTC2992_REG_G3_MIN_THRESH		0x6C
#define LTC2992_REG_G4				0x6E
#define LTC2992_REG_G4_MAX			0x70
#define LTC2992_REG_G4_MIN			0x72
#define LTC2992_REG_G4_MAX_THRESH		0x74
#define LTC2992_REG_G4_MIN_THRESH		0x76
#define LTC2992_REG_ISUM			0x78
#define LTC2992_REG_ISUM_MAX			0x7A
#define LTC2992_REG_ISUM_MIN			0x7C
#define LTC2992_REG_ISUM_MAX_THRESH		0x7E
#define LTC2992_REG_ISUM_MIN_THRESH		0x80
#define LTC2992_REG_PSUM			0x82
#define LTC2992_REG_PSUM_MAX			0x85
#define LTC2992_REG_PSUM_MIN			0x88
#define LTC2992_REG_PSUM_MAX_THRESH		0x8B
#define LTC2992_REG_PSUM_MIN_THRESH		0x8E
#define LTC2992_REG_ALERT3			0x91
#define LTC2992_REG_FAULT3			0x92
#define LTC2992_REG_GPIO_STATUS			0x95
#define LTC2992_REG_GPIO_IO_CTRL		0x96
#define LTC2992_REG_GPIO4_CTRL			0x97
#define LTC2992_REG_MFR_SPECIAL_ID		0xE7

#define LTC2992_REG_POWER(x)			(LTC2992_REG_POWER1 + ((x) * 0x32))
#define LTC2992_REG_POWER_MAX(x)		(LTC2992_REG_POWER1_MAX + ((x) * 0x32))
#define LTC2992_REG_POWER_MIN(x)		(LTC2992_REG_POWER1_MIN + ((x) * 0x32))
#define LTC2992_REG_POWER_MAX_THRESH(x)		(LTC2992_REG_POWER1_MAX_THRESH + ((x) * 0x32))
#define LTC2992_REG_POWER_MIN_THRESH(x)		(LTC2992_REG_POWER1_MIN_THRESH + ((x) * 0x32))
#define LTC2992_REG_DSENSE(x)			(LTC2992_REG_DSENSE1 + ((x) * 0x32))
#define LTC2992_REG_DSENSE_MAX(x)		(LTC2992_REG_DSENSE1_MAX + ((x) * 0x32))
#define LTC2992_REG_DSENSE_MIN(x)		(LTC2992_REG_DSENSE1_MIN + ((x) * 0x32))
#define LTC2992_REG_DSENSE_MAX_THRESH(x)	(LTC2992_REG_DSENSE1_MAX_THRESH + ((x) * 0x32))
#define LTC2992_REG_DSENSE_MIN_THRESH(x)	(LTC2992_REG_DSENSE1_MIN_THRESH + ((x) * 0x32))
#define LTC2992_REG_SENSE(x)			(LTC2992_REG_SENSE1 + ((x) * 0x32))
#define LTC2992_REG_SENSE_MAX(x)		(LTC2992_REG_SENSE1_MAX + ((x) * 0x32))
#define LTC2992_REG_SENSE_MIN(x)		(LTC2992_REG_SENSE1_MIN + ((x) * 0x32))
#define LTC2992_REG_SENSE_MAX_THRESH(x)		(LTC2992_REG_SENSE1_MAX_THRESH + ((x) * 0x32))
#define LTC2992_REG_SENSE_MIN_THRESH(x)		(LTC2992_REG_SENSE1_MIN_THRESH + ((x) * 0x32))
#define LTC2992_REG_POWER_FAULT(x)		(LTC2992_REG_FAULT1 + ((x) * 0x32))
#define LTC2992_REG_SENSE_FAULT(x)		(LTC2992_REG_FAULT1 + ((x) * 0x32))
#define LTC2992_REG_DSENSE_FAULT(x)		(LTC2992_REG_FAULT1 + ((x) * 0x32))

#define LTC2992_OFFSET_CAL_MSK			NO_OS_BIT(7)
#define LTC2992_MEASURE_MODE_MSK		NO_OS_GENMASK(6,5)
#define LTC2992_VSEL_CON_MSK			NO_OS_GENMASK(4,3)
#define LTC2992_VSEL_SNAP_MSK			NO_OS_GENMASK(2,0)
#define LTC2992_POWER_ALERT_MSK			NO_OS_GENMASK(7,6)
#define LTC2992_DSENSE_ALERT_MSK		NO_OS_GENMASK(5,4)
#define LTC2992_SENSE_ALERT_MSK			NO_OS_GENMASK(3,2)
#define LTC2992_ISUM_ALERT_MSK			NO_OS_GENMASK(3,2)
#define LTC2992_PSUM_ALERT_MSK			NO_OS_GENMASK(1,0)
#define LTC2992_POWER_FAULT_MSK			NO_OS_GENMASK(7,6)
#define LTC2992_DSENSE_FAULT_MSK		NO_OS_GENMASK(5,4)
#define LTC2992_SENSE_FAULT_MSK			NO_OS_GENMASK(3,2)
#define LTC2992_ISUM_FAULT_MSK			NO_OS_GENMASK(3,2)
#define LTC2992_PSUM_FAULT_MSK			NO_OS_GENMASK(1,0)
#define LTC2992_12B_SENSE_VAL_MSK		NO_OS_GENMASK(15,4)
#define LTC2992_12B_POWER_VAL_MSK		NO_OS_GENMASK(23,0)
#define LTC2992_8B_SENSE_VAL_MSK		NO_OS_GENMASK(15,8)
#define LTC2992_8B_POWER_VAL_MSK		NO_OS_GENMASK(23,8)

#define LTC2992_G1_FAULT_ALERT_MSK		0x03
#define LTC2992_G2_FAULT_ALERT_MSK		0x03
#define LTC2992_G3_FAULT_ALERT_MSK		0xC0
#define LTC2992_G4_FAULT_ALERT_MSK		0x30

#define LTC2992_ALERT_CLR_BIT			NO_OS_BIT(7)
#define LTC2992_READ_CTRL_BIT			NO_OS_BIT(5)
#define LTC2992_STUCK_BUS_TIMEOUT_BIT		NO_OS_BIT(4)
#define LTC2992_PEAK_RST_BIT			NO_OS_BIT(3)
#define LTC2992_RESET_BIT			NO_OS_BIT(0)
#define LTC2992_G4_ALERT_PIN_BIT		NO_OS_BIT(7)
#define LTC2992_G1_CTRL_BIT			NO_OS_BIT(7)
#define LTC2992_G2_CTRL_BIT			NO_OS_BIT(6)
#define LTC2992_G3_CTRL_BIT			NO_OS_BIT(0)
#define LTC2992_G4_CTRL_BIT			NO_OS_BIT(6)

#define LTC2992_MFR_ID_VALUE			0x62
#define LTC2992_RESOLUTION_POS			7

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ltc2992_adr_state {
	LTC2992_ADR_HIGH,
	LTC2992_ADR_LOW,
	LTC2992_ADR_NC,
};

enum ltc2992_gpio {
	LTC2992_GPIO1,
	LTC2992_GPIO2,
	LTC2992_GPIO3,
	LTC2992_GPIO4,
};

enum ltc2992_gpio_output {
	LTC2992_GPIO_OUTPUT_HI_Z,
	LTC2992_GPIO_OUTPUT_PULL_LOW,
};

enum ltc2992_sense {
	LTC2992_SENSE1,
	LTC2992_SENSE2,
};

enum ltc2992_channel {
	LTC2992_VOLTAGE,
	LTC2992_VSHUNT,
	LTC2992_CURRENT,
	LTC2992_POWER,
	LTC2992_ISUM,
	LTC2992_PSUM,
};

enum ltc2992_calibration {
	LTC2992_CALIBRATE_ALWAYS,
	LTC2992_CALIBRATE_ON_DEMAND,
};

enum ltc2992_measure_mode {
	LTC2992_MODE_CONTINUOUS,
	LTC2992_MODE_SNAPSHOT,
	LTC2992_MODE_SINGLE_CYCLE,
	LTC2992_MODE_SHUTDOWN,
};

enum ltc2992_continuous_vsel {
	LTC2992_CONT_VSEL_ALL,
	LTC2992_CONT_VSEL_SENSE_ONLY,
	LTC2992_CONT_VSEL_GPIO1_GPIO2,
	LTC2992_CONT_VSEL_GPIOS_ONLY,
};

enum ltc2992_snapshot_vsel {
	LTC2992_SNAP_VSEL_SENSE1,
	LTC2992_SNAP_VSEL_SENSE2,
	LTC2992_SNAP_VSEL_GPIO1,
	LTC2992_SNAP_VSEL_GPIO2,
	LTC2992_SNAP_VSEL_GPIO3,
	LTC2992_SNAP_VSEL_GPIO4,
	LTC2992_SNAP_VSEL_SENSE1_SENSE2,
	LTC2992_SNAP_VSEL_GPIO1_GPIO2,
};

enum ltc2992_resolution {
	LTC2992_RESOLUTION_12_BIT,
	LTC2992_RESOLUTION_8_BIT,
};

enum ltc2992_alert_value {
	LTC2992_ALERT_DISABLE_ALL,
	LTC2992_ALERT_ENABLE_UV,
	LTC2992_ALERT_ENABLE_OV,
	LTC2992_ALERT_ENABLE_ALL,
};

enum ltc2992_fault_status {
	LTC2992_FAULT_NONE,
	LTC2992_FAULT_UNDERVALUE,
	LTC2992_FAULT_OVERVALUE,
	LTC2992_FAULT_ALL,
};

struct ltc2992_regmap {
	uint8_t data;
	uint8_t max;
	uint8_t min;
	uint8_t max_thresh;
	uint8_t min_thresh;
	uint8_t fault;
	uint8_t alert;
	uint8_t fault_alert_mask;
	uint8_t ctrl;
	uint8_t ctrl_mask;
};

struct ltc2992_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *alert_gpio_desc;
	uint32_t shunt_resistor;
	uint8_t resolution;
};

struct ltc2992_init_param {
	struct no_os_i2c_init_param i2c_init;
	struct no_os_gpio_init_param alert_gpio_init;
	enum ltc2992_resolution resolution;
	enum ltc2992_calibration calibration;
	enum ltc2992_measure_mode measure_mode;
	enum ltc2992_continuous_vsel continuous_vsel;
	enum ltc2992_snapshot_vsel snapshot_vsel;
	uint32_t shunt_resistor;
	bool alert_clear;
	bool fault_clear_on_read;
	bool stuck_bus_timer_wakeup_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Generate device I2C address based on ADR pin states */
int ltc2992_addr_gen(struct ltc2992_init_param *init_param,
		     enum ltc2992_adr_state a0,
		     enum ltc2992_adr_state a1);

/* Initialize communication peripheral for the device */
int ltc2992_init(struct ltc2992_dev **device,
		 struct ltc2992_init_param *init_param);

/* Free resources allocated to the device */
int ltc2992_remove(struct ltc2992_dev *dev);

/* Read raw data from device register */
int ltc2992_read_reg(struct ltc2992_dev *dev, uint8_t addr,
		     uint32_t *data, uint8_t num_bytes);

/* Write raw data to device register */
int ltc2992_write_reg(struct ltc2992_dev *dev, uint8_t addr,
		      uint32_t value, uint8_t num_bytes);

/* Update register value in the device */
int ltc2992_update_reg(struct ltc2992_dev *dev, uint8_t addr,
		       uint8_t mask, uint8_t val);

/* Read bus voltage in a sense channel */
int ltc2992_get_voltage(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			uint32_t *voltage_data);

/* Read shunt voltage in a sense channel */
int ltc2992_get_vshunt(struct ltc2992_dev *dev,
		       enum ltc2992_sense sense,
		       uint32_t *vshunt_data);

/* Read bus current along a sense channel */
int ltc2992_get_current(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			uint32_t *current_data);

/* Read bus power output in a sense channel */
int ltc2992_get_power(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      uint32_t *power_data);

/* Read the shunt voltage sum or power sum of both sense channels */
int ltc2992_get_sum(struct ltc2992_dev *dev,
		    enum ltc2992_channel chan,
		    uint32_t *sum_data);

/* Read sense data at a SENSE channel */
int ltc2992_get_sense_data(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t *data);

/* Read maximum sense data at a SENSE channel */
int ltc2992_get_max_data(struct ltc2992_dev *dev,
			 enum ltc2992_sense sense,
			 enum ltc2992_channel chan,
			 uint32_t *data);

/* Read minimum sense data at a SENSE channel */
int ltc2992_get_min_data(struct ltc2992_dev *dev,
			 enum ltc2992_sense sense,
			 enum ltc2992_channel chan,
			 uint32_t *data);

/* Set upper thresh level for alert at a SENSE channel */
int ltc2992_set_max_thresh(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t thresh_value);
/* Set lower thresh level for alert at a SENSE channel */
int ltc2992_set_min_thresh(struct ltc2992_dev *dev,
			   enum ltc2992_sense sense,
			   enum ltc2992_channel chan,
			   uint32_t thresh_value);

/* Enable alert */
int ltc2992_set_alert(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      enum ltc2992_channel chan,
		      enum ltc2992_alert_value alert);

/* Get fault status */
int ltc2992_get_fault(struct ltc2992_dev *dev,
		      enum ltc2992_sense sense,
		      enum ltc2992_channel chan,
		      enum ltc2992_fault_status *fault);

/* Clear fault status */
int ltc2992_clear_fault(struct ltc2992_dev *dev,
			enum ltc2992_sense sense,
			enum ltc2992_channel chan);

/* Get data from a GPIO data converter */
int ltc2992_get_gpio_data(struct ltc2992_dev *dev,
			  enum ltc2992_gpio gpio,
			  uint32_t *gpio_data);

/* Get maximum data from a GPIO data converter */
int ltc2992_get_gpio_max_data(struct ltc2992_dev *dev,
			      enum ltc2992_gpio gpio,
			      uint32_t *gpio_data);

/* Get minimum data from a GPIO data converter */
int ltc2992_get_gpio_min_data(struct ltc2992_dev *dev,
			      enum ltc2992_gpio gpio,
			      uint32_t *gpio_data);

/* Set upper threshold level in a GPIO */
int ltc2992_set_gpio_max_thresh(struct ltc2992_dev *dev,
				enum ltc2992_gpio gpio,
				uint32_t thresh_value);

/* Set lower threshold level in a GPIO */
int ltc2992_set_gpio_min_thresh(struct ltc2992_dev *dev,
				enum ltc2992_gpio gpio,
				uint32_t thresh_value);

/* Enable alert when GPIO level reaches beyond threshold limit */
int ltc2992_set_gpio_alert(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   enum ltc2992_alert_value alert_value);

/* Get GPIO fault status */
int ltc2992_get_gpio_fault(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   enum ltc2992_fault_status *fault);

/* Clear GPIO fault status */
int ltc2992_clear_gpio_fault(struct ltc2992_dev *dev,
			     enum ltc2992_gpio gpio);

/* Reset alert pin (GPI04) status */
int ltc2992_reset_alert_pin(struct ltc2992_dev *dev);

/* Get GPIO state */
int ltc2992_get_gpio_state(struct ltc2992_dev *dev,
			   enum ltc2992_gpio gpio,
			   uint8_t *state);

/* Set GPIO output */
int ltc2992_set_gpio_output(struct ltc2992_dev *dev,
			    enum ltc2992_gpio gpio,
			    enum ltc2992_gpio_output output);
#endif /* __LTC2992_H__ */
