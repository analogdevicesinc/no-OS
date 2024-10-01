/***************************************************************************//**
 *   @file   tmc7300.h
 *   @brief  Header file for the TMC7300 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#ifndef _TMC7300_H_
#define _TMC7300_H_

#include <stdint.h>
#include "no_os_uart.h"
#include "no_os_gpio.h"

#define TMC7300_BRIDGE_NUM			2
#define TMC7300_DUTY_MAX_VALUE			511
#define TMC7300_SENDDELAY_MAX_VALUE		15

#define TMC7300_GCONF_REG		0x00
#define TMC7300_IFCNT_REG		0x02
#define TMC7300_SLAVECONF_REG		0x03
#define TMC7300_IOIN_REG		0x03
#define TMC7300_CURRENT_LIMIT_REG	0x10
#define TMC7300_PWM_AB_REG		0x22
#define TMC7300_CHOPCONF_REG		0x6C
#define TMC7300_DRV_STATUS_REG		0x6F
#define TMC7300_PWMCONF_REG		0x70

#define TMC7300_PAR_MODE_MASK	NO_OS_BIT(2)
#define TMC7300_PWM_DIRECT_MASK NO_OS_BIT(0)
#define TMC7300_DRV_ENABLE_MASK NO_OS_BIT(0)
#define TMC7300_SLAVECONF_MASK 	NO_OS_GENMASK(11, 8)
#define TMC7300_IRUN_MASK 	NO_OS_GENMASK(12, 8)
#define TMC7300_PWM_B_MASK	NO_OS_GENMASK(24, 16)
#define TMC7300_PWM_FREQ_MASK	NO_OS_GENMASK(17, 16)
#define TMC7300_FREEWHEEL_MASK	NO_OS_GENMASK(21, 20)
#define TMC7300_BLANK_TIME_MASK	NO_OS_GENMASK(16, 15)

#define TMC7300_LI_MASK(bridge)	(bridge == 0) ? NO_OS_BIT(6) : \
					NO_OS_BIT(7)

#define TMC7300_PWM_MASK(bridge)	(bridge == 0) ? NO_OS_GENMASK(8, 0) : \
					NO_OS_GENMASK(24, 16)

enum tmc7300_bridge {
	TMC7300_BRIDGE_A,
	TMC7300_BRIDGE_B,
};

enum tmc7300_standstill_mode {
	TMC7300_FREEWHEELING = 1,
	TMC7300_BREAK_LS,
	TMC7300_BREAK_HS,
};

enum tmc7300_pwm_freq {
	TMC7300_PWM_FREQ_2_1024,
	TMC7300_PWM_FREQ_2_683,
	TMC7300_PWM_FREQ_2_512,
	TMC7300_PWM_FREQ_2_410,
};

enum tmc7300_blank_time {
	TMC7300_BLANK_TIME_16,
	TMC7300_BLANK_TIME_24,
	TMC7300_BLANK_TIME_32,
	TMC7300_BLANK_TIME_40,
};

enum tmc7300_motor_dir {
	TMC7300_DIR_CW,
	TMC7300_DIR_CCW,
};

struct _tmc7300_drv_status {
	uint8_t otpw : 1;
	uint8_t ot : 1;
	uint8_t s2ga : 1;
	uint8_t s2gb : 1;
	uint8_t s2vsa : 1;
	uint8_t s2vsb : 1;
	uint8_t lia : 1;
	uint8_t lib : 1;
	uint8_t t120: 1;
	uint8_t t150: 1;
	uint32_t reserved: 22;
};

union tmc7300_drv_status {
	struct _tmc7300_drv_status bits;
	uint32_t val;
};

struct _tmc7300_ioin {
	uint8_t en : 1;
	uint8_t nstdby : 1;
	uint8_t ad0 : 1;
	uint8_t ad1 : 1;
	uint8_t diag : 1;
	uint8_t uart_on : 1;
	uint8_t uart_input : 1;
	uint8_t mode_input : 1;
	uint8_t a2 : 1;
	uint8_t a1 : 1;
	uint8_t comp_a1a2 : 1;
	uint8_t comp_b1b2 : 1;
	uint16_t reserved : 12;
	uint8_t version : 8;
};

union tmc7300_ioin {
	struct _tmc7300_ioin bits;
	uint32_t val;
};

struct tmc7300_bridge_priv {
	uint8_t pwm_duty;
	enum tmc7300_motor_dir motor_dir;
};

/**
 * @struct tmc7300_descs
 * @brief Runtime state for the TMC7300 driver.
 */
struct tmc7300_desc {
	/** Device address configured by the AD0 and AD1 pins */
	uint32_t addr;
	/** Initialized UART descriptor. Has to have asynchronous_rx = 1 */
	struct no_os_uart_desc *comm_desc;
	/** Initialized GPIO descriptor for the EN (bridge driver enable) signal */
	struct no_os_gpio_desc *en_gpio;
	/** Initialized GPIO descriptor for the VIO signal */
	struct no_os_gpio_desc *vio_gpio;

	/**
	 * Whether the motor is actively driven or
	 * freewheeling/passive braking is applied
	 */
	bool motor_drive;
	/* Current limit setting */
	uint8_t irun;
	/** PWM settings for the output bridges */
	struct tmc7300_bridge_priv bridge_priv[TMC7300_BRIDGE_NUM];
};

/**
 * @struct tmc7300_init_param
 * @brief TMC7300 initialization structure.
 */
struct tmc7300_init_param {
	/** Device address configured by the AD0 and AD1 pins */
	uint32_t addr;
	/** Initialized UART descriptor. Has to have asynchronous_rx = 1 */
	struct no_os_uart_desc *comm_desc;
	/** Initialized GPIO descriptor for the EN (bridge driver enable) signal */
	struct no_os_gpio_desc *en_gpio;
	/** Initialized GPIO descriptor for the VIO signal */
	struct no_os_gpio_desc *vio_gpio;
	/** Enable single motor drive using both bridges */
	bool parallel_mode;
};

/*! Read the value of a register */
int tmc7300_reg_read(struct tmc7300_desc *, uint32_t, uint32_t *);

/*! Write the value of a register */
int tmc7300_reg_write(struct tmc7300_desc *, uint32_t, uint32_t);

/*! Update the value of a field specified using a bit mask */
int tmc7300_reg_update(struct tmc7300_desc *, uint32_t, uint32_t, uint32_t);

/*! Read the LI{A,B} field, indicating if the bridge output is current limited or not */
int tmc7300_get_load_indicator(struct tmc7300_desc *, enum tmc7300_bridge,
			       uint32_t *);

/*! Set the state of the bridge driver */
int tmc7300_drv_enable(struct tmc7300_desc *, bool);

/*! Set the value of the PWM duty cycle. The sign specifies the full bridge output direction */
int tmc7300_set_pwm_duty(struct tmc7300_desc *, enum tmc7300_bridge, int32_t);

/*! Set the delay in the device's UART access response */
int tmc7300_set_send_delay(struct tmc7300_desc *, uint8_t);

/*! Set the current limit field*/
int tmc7300_set_current_limit(struct tmc7300_desc *, uint32_t);

/*! Read the valid write access counter */
int tmc7300_get_ifcnt(struct tmc7300_desc *, uint8_t *);

/*! Read the DRV_STATUS register */
int tmc7300_get_drv_status(struct tmc7300_desc *, union tmc7300_drv_status *);

/*! Read the IOIN register */
int tmc7300_get_ioin(struct tmc7300_desc *, union tmc7300_ioin *);

/*! Set the standstill mode (freewheeling/braking). The PWM output will be stopped. */
int tmc7300_set_standstill_mode(struct tmc7300_desc *,
				enum tmc7300_standstill_mode);

/*! Set the PWM frequency */
int tmc7300_set_pwm_freq(struct tmc7300_desc *, enum tmc7300_pwm_freq);

/*! Read the PWM frequency */
int tmc7300_get_pwm_freq(struct tmc7300_desc *, enum tmc7300_pwm_freq *);

/*! Read the comparator's blank time. */
int tmc7300_get_blank_time(struct tmc7300_desc *, enum tmc7300_blank_time *);

/*! Set the comparator's blank time. */
int tmc7300_set_blank_time(struct tmc7300_desc *, enum tmc7300_blank_time);

/*! Set the PWM duty cycle in the 0 - 255 (0% - 100%) range and the output direction (CW/CCW) */
int tmc7300_set_pwm_duty_dir(struct tmc7300_desc *, enum tmc7300_bridge,
			     uint8_t, enum tmc7300_motor_dir);

/*! Configure the initial state of the TMC7300 */
int tmc7300_init(struct tmc7300_desc **, struct tmc7300_init_param *);

/*! Free the resources allocated by the tmc7300_init() function */
int tmc7300_remove(struct tmc7300_desc *);

#endif