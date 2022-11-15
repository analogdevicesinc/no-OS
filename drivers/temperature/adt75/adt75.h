/***************************************************************************//**
 *   @file   adt75.h
 *   @brief  Header file of ADT75 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#ifndef _ADT75_H
#define _ADT75_H

#include <stdint.h>
#include "no_os_i2c.h"

/** x is set based on the value of A2, A1, A0 pins */
#define ADT75_ADDR(x)		(0x48 + (x))
#define ADT75_CONV_DELAY_MS     40
#define ADT75_FRAME_SIZE	3

#define ADT75_TEMP_VALUE_REG    0x0
#define ADT75_CONF_REG          0x1
#define ADT75_HYST_REG          0x2
#define ADT75_OS_REG            0x3
#define ADT75_ONE_SHOT_REG      0x4

#define ADT75_DATA_REG_MASK     	NO_OS_GENMASK(2, 0)
#define ADT75_CONFIG_REG_MASK   	NO_OS_GENMASK(7, 0)

#define ADT75_TEMP_MASK			NO_OS_GENMASK(15, 4)
#define ADT75_SIGN_BIT			11

/** Configuration register fields */
#define ADT75_SHUTDOWN_MASK		NO_OS_BIT(0)
#define ADT75_COMP_INT_MASK		NO_OS_BIT(1)
#define ADT75_ALERT_POL_MASK		NO_OS_BIT(2)
#define ADT75_FAULT_QUEUE_MASK		NO_OS_GENMASK(4, 3)
#define ADT75_ONESHOT_MASK		NO_OS_BIT(5)

#define ADT75_HYST_MASK			NO_OS_GENMASK(15, 4)
#define ADT75_OVER_TEMP_MASK		NO_OS_GENMASK(15, 4)

/** 1 LSB = 0.0625 C */
#define ADT75_TEMP_DIV			16

/**
 * @brief Initialization parameter for the device descriptor
 */
struct adt75_init_param {
	struct no_os_i2c_init_param comm_param;
};

/**
 * @brief ADT75 device descriptor
 */
struct adt75_desc {
	struct no_os_i2c_desc *comm_desc;
};

/** Get a single temperature sample and convert it to milli C */
int adt75_get_single_temp(struct adt75_desc *, int32_t *);

/** Read a register value */
int adt75_reg_read(struct adt75_desc *, uint32_t, uint16_t *);

/** Write a register values */
int adt75_reg_write(struct adt75_desc *, uint32_t, uint16_t);

/** Initialize the device structure */
int adt75_init(struct adt75_desc **, struct adt75_init_param *);

/** Free the device descriptor */
int adt75_remove(struct adt75_desc *);

#endif // _ADT75_H
