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
 * Charger configuration registers (all on the same slave address as the
 * TOP block). Only the registers touched by the MODE 6 bring-up sequence
 * are listed here.
 */
#define MAX77986_REG_CHG_CNFG_00	0x16
#define MAX77986_REG_CHG_CNFG_04	0x1A
#define MAX77986_REG_CHG_CNFG_06	0x1C
#define MAX77986_REG_CHG_CNFG_07	0x1D
#define MAX77986_REG_CHG_CNFG_08	0x1E
#define MAX77986_REG_CHG_CNFG_09	0x1F

/*
 * Register values for "I2C control, MODE 6 (high-voltage buck), adapter
 * powers SYS, no battery" with maximum current output.
 */
/* CHGPROT = 0b11 -> unlock charger registers. */
#define MAX77986_CHG_CNFG_06_UNLOCK	0x0C
/* SYS_TRACK_DIS = 1 (fixed SYS), MINSYS/CHG_CV_PRM left at reset. */
#define MAX77986_CHG_CNFG_04_VAL	0xE4
/* BCKSYS = 101 -> fixed buck SYS = 5.0 V (max). */
#define MAX77986_CHG_CNFG_08_VAL	0x50
/* CHGIN_ILIM = 5.5 A -> maximum adapter input current limit. */
#define MAX77986_CHG_CNFG_09_VAL	0x6D
/* DISIBS = 1 -> force BATT-to-SYS FET off (no battery). */
#define MAX77986_CHG_CNFG_07_VAL	0x20
/* BATRMV_MSK = 1, MODE = 0x4 (high-voltage buck). Enable buck last. */
#define MAX77986_CHG_CNFG_00_VAL	0x14

/**
 * @brief Initialize the MAX77986 over I2C in MODE 6 with maximum current.
 *
 * Brings up the adapter->SYS synchronous buck (no battery) regulated to a
 * fixed 5.0 V with the adapter input current limit set to its maximum
 * (5.5 A). The charger config registers are unlocked first, the voltage and
 * current limits configured, the BATT-to-SYS FET forced off, and MODE = 0x6
 * enabled last.
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

#endif /* __POWRMS_MAX77986_H__ */
