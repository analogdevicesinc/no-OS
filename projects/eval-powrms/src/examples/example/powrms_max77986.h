/***************************************************************************//**
 *   @file   powrms_max77986.h
 *   @brief  MAX77985/MAX77986 charger setup for EVAL-POWRMS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __POWRMS_MAX77986_H__
#define __POWRMS_MAX77986_H__

#include "no_os_i2c.h"

/* MAX77985/MAX77986 7-bit slave address (TOP/CHARGER block). */
#define MAX77986_I2C_ADDR		0x6B

/*
 * Charger configuration / status registers (all on the same slave address as
 * the TOP block). Only the registers touched by the MODE 5 bring-up sequence
 * and the post-init status dump are listed here.
 */
#define MAX77986_REG_CHG_DETAILS_00	0x13
#define MAX77986_REG_CHG_DETAILS_01	0x14
#define MAX77986_REG_CHG_CNFG_00	0x16
#define MAX77986_REG_CHG_CNFG_01	0x17
#define MAX77986_REG_CHG_CNFG_02	0x18
#define MAX77986_REG_CHG_CNFG_03	0x19
#define MAX77986_REG_CHG_CNFG_04	0x1A
#define MAX77986_REG_CHG_CNFG_06	0x1C
#define MAX77986_REG_CHG_CNFG_07	0x1D
#define MAX77986_REG_CHG_CNFG_08	0x1E
#define MAX77986_REG_CHG_CNFG_09	0x1F

/* Field accessors for the two status registers. */
#define MAX77986_CHGIN_DTLS(d00)	(((d00) >> 5) & 0x3)
#define MAX77986_BAT_DTLS(d01)		(((d01) >> 4) & 0x7)
#define MAX77986_CHG_DTLS(d01)		((d01) & 0xF)

/*
 * Register values for "I2C control, MODE 5 (charger + high-voltage buck)":
 * the adapter powers SYS through the buck *and* charges the 1-cell Li+ pack
 * on BATT.
 */
/* CHGPROT = 0b11 -> unlock charger registers. */
#define MAX77986_CHG_CNFG_06_UNLOCK	0x0C
/*
 * TKEN = 1 (trickle charge on a deeply discharged cell), WDTEN = 0 (no
 * watchdog: nothing kicks it in this firmware and an expiry would stop the
 * charge), CHG_RSTRT = 0b01 (restart 150 mV below CHG_CV_PRM),
 * FCHGTIME = 0b101 (7 h safety timer).
 */
#define MAX77986_CHG_CNFG_01_VAL	0x95
/*
 * CHG_CC -> fast-charge current. 0x1E = 1.5 A, which fits inside the 5 V/3 A
 * budget of a plain USB-C port once the SYS load is accounted for. Raise it
 * (50 mA per code, 0x6E = 5.5 A max) only if the pack and the adapter allow.
 */
#define MAX77986_CHG_CNFG_02_VAL	0x1E
/* TO_TIME = 0b011 (30 min top-off), TO_ITH = 0b0010 (200 mA). */
#define MAX77986_CHG_CNFG_03_VAL	0x32
/*
 * SYS_TRACK_DIS = 0 -> SYS tracks the battery (required for normal power-path
 * operation with a pack fitted), MINSYS = 0b01 (3.5 V),
 * CHG_CV_PRM = 0x04 -> 4.20 V termination for a standard 1-cell Li+.
 */
#define MAX77986_CHG_CNFG_04_VAL	0x24
/* All zero: DISIBS = 0 so the BATT-to-SYS FET follows the power path. */
#define MAX77986_CHG_CNFG_07_VAL	0x00
/* BCKSYS = 101 -> fixed buck SYS = 5.0 V (max). */
#define MAX77986_CHG_CNFG_08_VAL	0x50
/*
 * CHGIN_ILIM = 0x3B -> 3.0 A adapter input current limit. The previous 5.5 A
 * (0x6D) exceeds what a USB-C port can source; AICL then folds the input back
 * and leaves nothing for the charger.
 */
#define MAX77986_CHG_CNFG_09_VAL	0x3B
/* BATRMV_MSK = 1, MODE = 0x5 (charger on + buck on). Enable last. */
#define MAX77986_CHG_CNFG_00_VAL	0x15

/**
 * @brief Initialize the MAX77986 over I2C in MODE 5 (charge + buck).
 *
 * Brings up the adapter->SYS synchronous buck and the 1-cell Li+ battery
 * charger. The charger config registers are unlocked first, then the charge
 * current / termination voltage / top-off and adapter input current limits
 * are programmed, and MODE = 0x5 is enabled last.
 *
 * @param desc - Will be set to the allocated I2C descriptor on success. The
 *               caller owns it and must release it with max77986_remove().
 * @param param - I2C bus initialization parameters (slave address 0x6B).
 *
 * @return 0 on success, negative error code on failure.
 */
int max77986_init(struct no_os_i2c_desc **desc,
		  const struct no_os_i2c_init_param *param);

/**
 * @brief Free the resources allocated by max77986_init().
 *
 * @param desc - I2C descriptor returned by max77986_init().
 *
 * @return 0 on success, negative error code on failure.
 */
int max77986_remove(struct no_os_i2c_desc *desc);

/**
 * @brief Read the raw CHG_DETAILS_00 / CHG_DETAILS_01 status registers.
 *
 * Decode the fields with MAX77986_CHGIN_DTLS(), MAX77986_BAT_DTLS() and
 * MAX77986_CHG_DTLS().
 *
 * @param desc       - I2C descriptor returned by max77986_init().
 * @param details_00 - Will be set to the CHG_DETAILS_00 content.
 * @param details_01 - Will be set to the CHG_DETAILS_01 content.
 *
 * @return 0 on success, negative error code on failure.
 */
int max77986_get_details(struct no_os_i2c_desc *desc, uint8_t *details_00,
			 uint8_t *details_01);

/**
 * @brief Log the charger input / battery / charge-state details.
 *
 * Reads CHG_DETAILS_00 and CHG_DETAILS_01 and prints the decoded CHGIN,
 * BAT_DTLS and CHG_DTLS fields. Useful to tell "charger disabled" apart from
 * "charging is suspended" (thermistor out of range, battery removed, timer
 * fault) when the pack does not take charge.
 *
 * @param desc - I2C descriptor returned by max77986_init().
 *
 * @return 0 on success, negative error code on failure.
 */
int max77986_log_status(struct no_os_i2c_desc *desc);

#endif /* __POWRMS_MAX77986_H__ */
