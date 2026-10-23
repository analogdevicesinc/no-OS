/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <xil_cache.h>

#ifdef CONFIG_ADAR300X_BASIC_EXAMPLE
#include "basic_example.h"
#elif defined(CONFIG_ADAR300X_IIO_EXAMPLE)
#include "iio_example.h"
#endif

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

#ifdef CONFIG_ADAR300X_BASIC_EXAMPLE
	return basic_example_main();
#elif defined(CONFIG_ADAR300X_IIO_EXAMPLE)
	return iio_example_main();
#endif
}
