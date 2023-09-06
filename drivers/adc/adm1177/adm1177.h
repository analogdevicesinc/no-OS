/**************************************************************************//**
 *   @file   adm1177.c
 *   @brief  Header file of ADM1177 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
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
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
******************************************************************************/
#ifndef __ADM1177_H__
#define __ADM1177_H__

#include <stdint.h>
#include <stdio.h>
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_units.h"

/* ADM1177 Slave Address */
#define	ADM1177_ADDRESS			0x5A

/* ADM1177 Write Command Byte Bits */
#define ADM1177_CMD_V_CONT      	NO_OS_BIT(0)
#define ADM1177_CMD_V_ONCE      	NO_OS_BIT(1)
#define ADM1177_CMD_I_CONT      	NO_OS_BIT(2)
#define ADM1177_CMD_I_ONCE      	NO_OS_BIT(3)
#define ADM1177_CMD_VRANGE      	NO_OS_BIT(4)
#define ADM1177_CMD_STATUS_RD   	NO_OS_BIT(6)

/* ADM11777 Write Extended Command Byte Register Addresses */
#define ADM1177_REG_ALERT_EN    	(NO_OS_BIT(7) | 0x1)
#define ADM1177_REG_ALERT_TH    	(NO_OS_BIT(7) | 0x2)
#define ADM1177_REG_CONTROL     	(NO_OS_BIT(7) | 0x3)

/* ALERT_EN Register Operations */
#define ADM1177_EN_ADC_OC1_MASK 	NO_OS_BIT(0)
#define ADM1177_EN_ADC_OC4_MASK 	NO_OS_BIT(1)
#define ADM1177_EN_HS_ALERT_MASK     	NO_OS_BIT(2)
#define ADM1177_EN_OFF_ALERT_MASK    	NO_OS_BIT(3)
#define ADM1177_CLEAR_MASK           	NO_OS_BIT(4)

/* CONTROL Register Operations */
#define ADM1177_SWOFF_MASK           	NO_OS_BIT(0)

/* Status Byte Operations */
#define ADM1177_ADC_OC_MASK		NO_OS_BIT(0)
#define ADM1177_ADC_ALERT_MASK		NO_OS_BIT(1)
#define ADM1177_HS_OC_MASK		NO_OS_BIT(2)
#define ADM1177_HS_ALERT_MASK		NO_OS_BIT(3)
#define ADM1177_OFF_STATUS_MASK		NO_OS_BIT(4)
#define ADM1177_OFF_ALERT_MASK		NO_OS_BIT(5)

/* Write Extended Command Mask */
#define ADM1177_EXT_CMD_MASK		NO_OS_BIT(7)

#define ADM1177_I_FULLSCALE		105840
#define ADM1177_RESOLUTION		4096
#define ADM1177_V_FULLSCALE_0		26350
#define ADM1177_V_FULLSCALE_1		6650
#define ADM1177_R_SENSE			25

enum adm1177_last_command {
	ADM1177_VOLTAGE_EN,
	ADM1177_CURRENT_EN,
	ADM1177_VOLTAGE_AND_CURRENT_EN,
	ADM1177_STATUS_EN,
};

enum adm1177_vrange {
	ADM1177_VRANGE_LOW,
	ADM1177_VRANGE_HIGH,
};

struct adm1177_dev {
	struct no_os_i2c_desc   *i2c_desc;
	enum adm1177_last_command last_command;
	enum adm1177_vrange vrange;
};

struct adm1177_init_param {
	struct no_os_i2c_init_param     i2c_init;
};

/* Initializes the ADM1177 */
int adm1177_init(struct adm1177_dev **device,
		 struct adm1177_init_param *init_param);

/* Removes the resources allocated by the ADM1177 */
int adm1177_remove(struct adm1177_dev *device);

/* Write the command byte to the slave */
int adm1177_write(struct adm1177_dev *device,
		  uint8_t cmd,
		  uint8_t reg_address);

/* Reads the status byte if requested */
int adm1177_read_status(struct adm1177_dev *device, uint8_t *status_byte);

/* Sets vrange of the device */
int adm1177_set_vrange(struct adm1177_dev *device, uint8_t vrange);

/* Gets conversion results for both Voltage and Current at the same time */
int adm1177_read_conv(struct adm1177_dev *device,
		      uint16_t* conv_voltage,
		      uint16_t* conv_current);

/* Converts the raw sample of the voltage from the ADC in uV */
int adm1177_to_microvolts(struct adm1177_dev *device,
			  uint16_t raw_sample,
			  uint64_t *voltage_uv);

/* Converts the raw sample of the current from the ADC in uA */
int adm1177_to_microampers(uint16_t raw_sample, uint64_t *current_ua);

#endif /*__ADM1177_H__*/
