/***************************************************************************//**
*   @file    ad7124_regs.h
*   @brief   AD7124-8 register initialization file for ad7124-8pmdz project.
********************************************************************************
* Copyright 2015-2019(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/


#ifndef AD7124_REGS_H_
#define AD7124_REGS_H_

#include "ad7124.h"

/*! Array holding the info for the ad7124 registers - address, initial value,
    size and access type. */
extern struct ad7124_st_reg ad7124_init_regs_default[AD7124_REG_NO];

#endif /* AD7124_REGS_H_ */
