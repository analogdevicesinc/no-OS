/***************************************************************************//**
 *   @file   max42500.h
 *   @brief  Header file of MAX42500 Driver.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
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
#include "no_os_timer.h"
#include "no_os_irq.h"

/** X is set based on the pull configuration of the ADDR pin */
#define MAX42500_ADDR(x)            (0x28 + (x))
#define MAX42500_SILICON_ID         (0x30)
#define MAX42500_I2C_WR_FRAME_SIZE  (4)
#define MAX42500_I2C_RD_FRAME_SIZE  (5)

/* CONFIG1 bit masks */
#define MAX42500_CONFIG1_PECE_MASK  NO_OS_BIT(0)
#define MAX42500_CONFIG1_MBST_MASK  NO_OS_BIT(1)
#define MAX42500_CONFIG1_RR_MASK    NO_OS_BIT(2)

/* VMON bit masks */
#define MAX42500_VMON_IN_MASK(bit)  NO_OS_BIT(bit)
#define MAX42500_VMON_VMPD_MASK     NO_OS_BIT(7)

/* RSTMAP bit masks */
#define MAX42500_RSTMAP_IN_MASK(bit)    NO_OS_BIT(bit)
#define MAX42500_RSTMAP_PARM_MASK       NO_OS_BIT(7)

/* WDCDIV bit masks */
#define MAX42500_WDCDIV_SWW_MASK    NO_OS_BIT(6)
#define MAX42500_WDCDIV_WDIC_MASK   (0x3F)

/* WDCFG2 bit masks */
#define MAX42500_WDCFG2_WDEN_MASK   NO_OS_BIT(3)
#define MAX42500_WDCFG2_1UP_MASK    (0x7)

/* WDLOCK bit masks */
#define MAX42500_WDLOCK_LOCK_MASK   NO_OS_BIT(0)

/* RSTCTRL bit masks */
#define MAX42500_RSTCTRL_MR1_MASK   NO_OS_BIT(2)
#define MAX42500_RSTCTRL_RHLD_MASK  (0x3)

/* MAX42500 registers list */
enum max42500_registers {
	MAX42500_ID = 0,
	MAX42500_CONFIG1,
	MAX42500_CONFIG2,
	MAX42500_VMON,
	MAX42500_RSTMAP,
	MAX42500_STATOV,
	MAX42500_STATUV,
	MAX42500_STATOFF,
	MAX42500_VIN1,
	MAX42500_VIN2,
	MAX42500_VIN3,
	MAX42500_VIN4,
	MAX42500_VIN5,
	MAX42500_VINO6,
	MAX42500_VINU6,
	MAX42500_VINO7,
	MAX42500_VINU7,
	MAX42500_OVUV1,
	MAX42500_OVUV2,
	MAX42500_OVUV3,
	MAX42500_OVUV4,
	MAX42500_OVUV5,
	MAX42500_FPSSTAT1,
	MAX42500_FPSCFG1,
	MAX42500_UTIME1,
	MAX42500_UTIME2,
	MAX42500_UTIME3,
	MAX42500_UTIME4,
	MAX42500_UTIME5,
	MAX42500_UTIME6,
	MAX42500_UTIME7,
	MAX42500_DTIME1,
	MAX42500_DTIME2,
	MAX42500_DTIME3,
	MAX42500_DTIME4,
	MAX42500_DTIME5,
	MAX42500_DTIME6,
	MAX42500_DTIME7,
	MAX42500_WDSTAT,
	MAX42500_WDCDIV,
	MAX42500_WDCFG1,
	MAX42500_WDCFG2,
	MAX42500_WDKEY,
	MAX42500_WDLOCK,
	MAX42500_RSTCTRL,
	MAX42500_CID,
	MAX42500_REG_NO
};

/* Device register info */
struct max42500_reg_st {
	const enum max42500_registers addr;
	uint8_t value;
};

/* MAX42500 watchdog mode */
enum max42500_state {
	MAX42500_STATE_OFF = 0,
	MAX42500_STATE_SLEEP,
	MAX42500_STATE_ON,
	MAX42500_STATE_MAX
};

/* MAX42500 voltage monitor input */
enum max42500_vm_input {
	MAX42500_VM1 = 0,
	MAX42500_VM2,
	MAX42500_VM3,
	MAX42500_VM4,
	MAX42500_VM5,
	MAX42500_VM6,
	MAX42500_VM7,
	MAX42500_VM_MAX
};

/* MAX42500 watchdog mode */
enum max42500_wd_mode {
	MAX42500_WD_MODE_CH_RESP = 0,
	MAX42500_WD_MODE_SIMPLE,
	MAX42500_WD_MODE_MAX
};

/* MAX42500 watchdog mode */
enum max42500_wd_rhld {
	MAX42500_WD_RHOLD_0_MS = 0,
	MAX42500_WD_RHOLD_8_MS,
	MAX42500_WD_RHOLD_16_MS,
	MAX42500_WD_RHOLD_32_MS,
	MAX42500_WD_RHOLD_MAX
};

/**
 * @brief Initialization parameter for the device descriptor
 */
struct max42500_init_param {
	/* Device register settings */
	struct max42500_reg_st *regs;
	/* I2C */
	struct no_os_i2c_init_param comm_param;
	/* Timer */
	struct no_os_timer_init_param timer_param;
	/* IRQ controller */
	struct no_os_irq_init_param irq_param;
	/* EN0 pin GPIO */
	struct no_os_gpio_init_param en0_param;
	/* EN1 pin GPIO */
	struct no_os_gpio_init_param en1_param;
	/* ADDR pin GPIO */
	struct no_os_gpio_init_param addr_param;
	/* ADDR selection  */
	uint8_t addr_sel;
	/* Packet error checking enable */
	bool pece;
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
	bool wd_en;
};

/**
 * @brief max42500 device descriptor
 */
struct max42500_dev {
	/* Device register settings */
	struct max42500_reg_st *regs;
	/* I2C */
	struct no_os_i2c_desc *comm_desc;
	/* Timer */
	struct no_os_timer_desc *timer_desc;
	/* IRQ controller */
	struct no_os_irq_ctrl_desc *irq_desc;
	/* EN0 pin GPIO */
	struct no_os_gpio_desc *en0;
	/* EN1 pin GPIO */
	struct no_os_gpio_desc *en1;
	/* ADDR pin GPIO */
	struct no_os_gpio_desc *addr;
	/* ADDR selection  */
	uint8_t addr_sel;
	/* Packet error checking enable */
	bool pece;
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
	bool wd_en;
};

/** Enable voltage monitor inputs */
int max42500_set_vmon_enable(struct max42500_dev *desc, uint8_t vmon_en_mask);

/** Read a register value */
int max42500_reg_read(struct max42500_dev *desc, struct max42500_reg_st *);

/** Write a register value */
int max42500_reg_write(struct max42500_dev *desc, struct max42500_reg_st);

/** Wrap the write register function to give it a modern signature. */
int max42500_reg_write2(struct max42500_dev *desc,
			uint32_t reg,
			uint32_t writeval);

/** Initialize the device structure */
int max42500_init(struct max42500_dev **, struct max42500_init_param *);

/** Free the device descriptor */
int max42500_remove(struct max42500_dev *);

#endif // __MAX42500_H__
