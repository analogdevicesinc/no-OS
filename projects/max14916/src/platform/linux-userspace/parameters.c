/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of linux-userspace platform parameters for max14916.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

#ifdef CONFIG_FTD2XX
struct ftd2xx_spi_init max14916_spi_extra = {
	.channel_config_pin =
	((1 << 6) |		/* BIT7  - BIT0:  Initial direction of the pins */
	 (1 << 6) << 8 |	/* BIT15 - BIT8:  Initial values of the pins    */
	 (1 << 6) << 16 |	/* BIT23 - BIT16: Final direction of the pins   */
	 (1 << 6) << 24),	/* BIT31 - BIT24: Final values of the pins      */
};
#endif
