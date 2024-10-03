/***************************************************************************//**
 *   @file   max42500.h
 *   @brief  Header file of MAX42500 Driver.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
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
#ifndef __MAX42500_H__
#define __MAX42500_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"

#define MAX42500_REG_ID                 0x00
#define MAX42500_REG_CONFIG1            0x01
#define MAX42500_REG_CONFIG2            0x02
#define MAX42500_REG_VMON               0x03
#define MAX42500_REG_RSTMAP             0x04
#define MAX42500_REG_STATOV             0x05
#define MAX42500_REG_STATUV             0x06
#define MAX42500_REG_STATOFF            0x07
#define MAX42500_REG_VIN1               0x08
#define MAX42500_REG_VIN2               0x09
#define MAX42500_REG_VIN3               0x0A
#define MAX42500_REG_VIN4               0x0B
#define MAX42500_REG_VIN5               0x0C
#define MAX42500_REG_VINO6              0x0D
#define MAX42500_REG_VINU6              0x0E
#define MAX42500_REG_VINO7              0x0F
#define MAX42500_REG_VINU7              0x10
#define MAX42500_REG_OVUV1              0x11
#define MAX42500_REG_OVUV2              0x12
#define MAX42500_REG_OVUV3              0x13
#define MAX42500_REG_OVUV4              0x14
#define MAX42500_REG_OVUV5              0x15
#define MAX42500_REG_FPSSTAT1           0x16
#define MAX42500_REG_FPSCFG1            0x17
#define MAX42500_REG_UTIME1             0x18
#define MAX42500_REG_UTIME2             0x19
#define MAX42500_REG_UTIME3             0x1A
#define MAX42500_REG_UTIME4             0x1B
#define MAX42500_REG_UTIME5             0x1C
#define MAX42500_REG_UTIME6             0x1D
#define MAX42500_REG_UTIME7             0x1E
#define MAX42500_REG_DTIME1             0x1F
#define MAX42500_REG_DTIME2             0x20
#define MAX42500_REG_DTIME3             0x21
#define MAX42500_REG_DTIME4             0x22
#define MAX42500_REG_DTIME5             0x23
#define MAX42500_REG_DTIME6             0x24
#define MAX42500_REG_DTIME7             0x25
#define MAX42500_REG_WDSTAT             0x26
#define MAX42500_REG_WDCDIV             0x27
#define MAX42500_REG_WDCFG1             0x28
#define MAX42500_REG_WDCFG2             0x29
#define MAX42500_REG_WDKEY              0x2A
#define MAX42500_REG_WDLOCK             0x2B
#define MAX42500_REG_RSTCTRL            0x2C
#define MAX42500_REG_CID                0x2D

/** X is set based on the pull configuration of the ADDR pin */
#define MAX42500_ADDR(x)                (0x28 + (x))
#define MAX42500_SILICON_ID             (0x30)
#define MAX42500_I2C_WR_FRAME_SIZE      (4)
#define MAX42500_I2C_RD_FRAME_SIZE      (5)

/** MAX42500 Nominal voltage computation */
#define MAX42500_VNOM_MAX_VM1_VM4       3.6875
#define MAX42500_VNOM_MAX_VM5           5.6
#define MAX42500_MIN_VNOM               0.5
#define MAX42500_VNOM_STEP_VM1_VM4      0.0125
#define MAX42500_VNOM_STEP_VM5          0.02

/** MAX42500 Undervoltage/Overvoltage maximum and minimum thresholds*/
#define MAX42500_MAX_THRESH_VM1_VM5     10
#define MAX42500_MIN_THRESH_VM1_VM5     2.5
#define MAX42500_MAX_THRESH_VM6_V7      1.775
#define MAX42500_MIN_THRESH_VM6_V7      0.5

/* MAX42500 device state */
enum max42500_state {
	MAX42500_STATE_OFF,
	MAX42500_STATE_SLEEP,
	MAX42500_STATE_ON,
	MAX42500_STATE_MAX
};

/* MAX42500 voltage monitor input */
enum max42500_vm_input {
	MAX42500_VM1,
	MAX42500_VM2,
	MAX42500_VM3,
	MAX42500_VM4,
	MAX42500_VM5,
	MAX42500_VM6,
	MAX42500_VM7,
	MAX42500_VM_MAX
};

/* MAX42500 comparator status */
enum max42500_comp_stat {
	MAX42500_COMP_STAT_OFF,
	MAX42500_COMP_STAT_UV,
	MAX42500_COMP_STAT_OV,
	MAX42500_COMP_STAT_MAX
};

/* MAX42500 watchdog mode */
enum max42500_wd_mode {
	MAX42500_WD_MODE_CH_RESP,
	MAX42500_WD_MODE_SIMPLE,
	MAX42500_WD_MODE_MAX
};

/* MAX42500 reset hold/active timeout time. */
enum max42500_wd_rhld {
	MAX42500_WD_RHOLD_0_MS,
	MAX42500_WD_RHOLD_8_MS,
	MAX42500_WD_RHOLD_16_MS,
	MAX42500_WD_RHOLD_32_MS,
	MAX42500_WD_RHOLD_MAX
};

/**
 * @brief Initialization parameter for the device descriptor
 */
struct max42500_init_param {
	/* I2C */
	struct no_os_i2c_init_param comm_param;
	/* EN0 pin GPIO */
	struct no_os_gpio_init_param en0_param;
	/* EN1 pin GPIO */
	struct no_os_gpio_init_param en1_param;
	/* ADDR pin GPIO */
	struct no_os_gpio_init_param addr_param;
	/* ADDR selection  */
	uint8_t addr_sel;
	/* Packet error checking enable */
	uint8_t pece;
	/* Enabled voltage monitor inputs */
	uint8_t vmon_en;
	/* Voltage monitor power down enable */
	uint8_t vmon_vmpd;
	/* Enabled voltage monitor reset mapping */
	uint8_t reset_map;
	/* Watchdog mode */
	enum max42500_wd_mode wd_mode;
	/* Watchdog clock div */
	uint8_t wd_cdiv;
	/* Watchdog close window */
	uint8_t wd_close;
	/* Watchdog open window */
	uint8_t wd_open;
	/* Watchdog first update window */
	uint8_t wd_1ud;
	/* Watchdog enable */
	uint8_t wd_en;
};

/**
 * @brief max42500 device descriptor
 */
struct max42500_dev {
	/* I2C */
	struct no_os_i2c_desc *comm_desc;
	/* EN0 pin GPIO */
	struct no_os_gpio_desc *en0;
	/* EN1 pin GPIO */
	struct no_os_gpio_desc *en1;
	/* ADDR pin GPIO */
	struct no_os_gpio_desc *addr;
	/* ADDR selection  */
	uint8_t addr_sel;
	/* Packet error checking enable */
	uint8_t pece;
	/* Enabled voltage monitor inputs */
	uint8_t vmon_en;
	/* Voltage monitor power down enable */
	uint8_t vmon_vmpd;
	/* Enabled voltage monitor reset mapping */
	uint8_t reset_map;
	/* Watchdog mode */
	enum max42500_wd_mode wd_mode;
	/* Watchdog clock div */
	uint8_t wd_cdiv;
	/* Watchdog close window */
	uint8_t wd_close;
	/* Watchdog open window */
	uint8_t wd_open;
	/* Watchdog first update window */
	uint8_t wd_1ud;
	/* Watchdog enable */
	uint8_t wd_en;
};

/** Set device state through EN0 and EN1 pins */
int max42500_set_state(struct max42500_dev *desc, enum max42500_state state);

/** Read a register value */
int max42500_reg_read(struct max42500_dev *desc,
		      uint8_t reg_addr,
		      uint8_t *reg_data);

/** Write a register value */
int max42500_reg_write(struct max42500_dev *desc,
		       uint8_t reg_addr,
		       uint8_t data);

/** Update a register's value based on a mask */
int max42500_reg_update(struct max42500_dev *desc,
			uint8_t reg_addr,
			uint8_t mask,
			uint8_t data);

/** Set nominal voltage for VM1 to VM5 */
int max42500_set_nominal_voltage(struct max42500_dev *desc,
				 enum max42500_vm_input vm_in,
				 float voltage);

/** Get the status of the voltage monitor input */
int max42500_get_comp_status(struct max42500_dev *desc,
			     enum max42500_vm_input vm_in,
			     enum max42500_comp_stat comp_stat,
			     uint8_t *status);

/** Set the overvoltage threshold of VM1 to VM5 */
int max42500_set_ov_thresh1(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh);

/** Set the overvoltage threshold of VM6 and VM7 */
int max42500_set_ov_thresh2(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh);

/** Set the undervoltage threshold of VM1 to VM5 */
int max42500_set_uv_thresh1(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh);

/** Set the undervoltage threshold of VM6 and VM7 */
int max42500_set_uv_thresh2(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh);

/** Get the power-up timestamp for a specified voltage monitor input */
int max42500_get_power_up_timestamp(struct max42500_dev *desc,
				    enum max42500_vm_input vm_in,
				    uint8_t *timestamp);

/** Get the power-down timestamp for a specified voltage monitor input */
int max42500_get_power_down_timestamp(struct max42500_dev *desc,
				      enum max42500_vm_input vm_in,
				      uint8_t *timestamp);

/** Enable/Disable watchdog */
int max42500_set_watchdog_enable(struct max42500_dev *desc, bool wd_enable);

/** Update the watchdog key based on the mode and current value */
int max42500_set_watchdog_key(struct max42500_dev *desc);

/** Set watchdog reset hold time */
int max42500_set_watchdog_rhld(struct max42500_dev *desc,
			       enum max42500_wd_rhld rhld);

/** Initialize the device structure */
int max42500_init(struct max42500_dev **, struct max42500_init_param *);

/** Free the device descriptor */
int max42500_remove(struct max42500_dev *);

#endif // __MAX42500_H__
