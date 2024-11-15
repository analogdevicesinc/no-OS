/***************************************************************************//**
 *   @file   adum7701.h
 *   @brief  Header file for adum7701 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SRC_ADUM7701_H_
#define SRC_ADUM7701_H_

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct adum7701_dev {
	/* GPIO */
	gpio_desc *dec_ratio;
	gpio_desc *filter_reset;
};

struct adum7701_init_param {
	/* GPIO */
	gpio_init_param dec_ratio;
	gpio_init_param filter_reset;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t adum7701_init(struct adum7701_dev **device,
		      struct adum7701_init_param init_param);
int32_t adum7701_remove(struct adum7701_dev *dev);

#endif /* SRC_ADUM7701_H_ */
