/*******************************************************************************
 *   @file   gmsl_reg_access.h
 *   @brief  Header file of GMSL Register Access.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
#ifndef GMSL_REG_ACCESS_H
#define GMSL_REG_ACCESS_H

#include <stdint.h>
#include "no_os_i2c.h"

#define RETURN_ON_ERR(x) if(x != 0) return (x)

#define REG_WRITE_RETURN_ON_ERR(ret, i2c_desc, reg, writeval) {\
    ret = gmsl_reg_write(i2c_desc, reg, writeval); \
    if(ret)\
        return ret;\
}

#define REG_READ_RETURN_ON_ERR(ret, i2c_desc, reg, mask, readval) {\
    ret = gmsl_reg_read(i2c_desc, reg, mask, readval); \
    if(ret)\
        return ret;\
}

#define REG_UPDATE_RETURN_ON_ERR(ret, i2c_desc, reg, val, mask) {\
    ret = gmsl_reg_update(i2c_desc, reg, val, mask); \
    if(ret)\
        return ret;\
}

#define BREAK_ON_ERR(x)\
if(x != 0)\
{\
    break;\
}

#define REG_WRITE_BREAK_ON_ERR(ret, i2c_desc, reg, writeval)\
{\
    ret = gmsl_reg_write(i2c_desc, reg, writeval);\
    BREAK_ON_ERR(ret);\
}

#define REG_READ_BREAK_ON_ERR(ret, i2c_desc, reg, mask, readval)\
{\
    ret = gmsl_reg_read(i2c_desc, reg, mask, readval);\
    BREAK_ON_ERR(ret);\
}

#define REG_UPDATE_BREAK_ON_ERR(ret, i2c_desc, reg, val, mask)\
{\
    ret = gmsl_reg_update(i2c_desc, reg, val, mask);\
    BREAK_ON_ERR(ret);\
}

/** Read a device register through the I2C interface. */
int32_t gmsl_reg_read(struct no_os_i2c_desc *i2c_desc,
		      uint16_t reg, uint8_t mask, uint8_t *readval);

/** Write a device register through the I2C interface. */
int32_t gmsl_reg_write(struct no_os_i2c_desc *i2c_desc,
		       uint16_t reg,
		       uint8_t writeval);

/** Read-modify-write operation on a field of a register. */
int32_t gmsl_reg_update(struct no_os_i2c_desc *i2c_desc,
			uint16_t reg,
			uint8_t val,
			uint8_t mask);

#endif	/* GMSL_REG_ACCESS_H */
