/***************************************************************************//**
 *   @file   libiio_example.c
 *   @brief  Basic example of the iio_demo project, served by libiio
 *   @author Radu Ciobanu (radu-rares.ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <noos_iiod.h>
#include "libiio_example.h"

/* The devices and settings it serves are in common_data.c. */
extern const struct noos_iiod_param iiod_param;

/**
 * @brief Serve the iio_demo devices with libiio's iiod, over the transport
 *        picked in CONFIG_LIBIIO_TRANSPORT. Does not return.
 * @return A negative error code if the server could not start.
 */
int libiio_example_main(void)
{
	return noos_iiod_run(&iiod_param);
}
