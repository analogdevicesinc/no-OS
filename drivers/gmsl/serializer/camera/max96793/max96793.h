/*******************************************************************************
 *   @file   max96793.h
 *   @brief  Header file of MAX96793 Driver.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
#ifndef MAX96793_H
#define MAX96793_H

#include <stdbool.h>
#include <stdint.h>
#include "gmsl_common.h"

#define MAX96793_LMN_COUNT      (2u)     /* Number of LMN */
#define MAX96793_MIPI_PHY_COUNT (2u)     /* Number of PHY */

#define MAX96793_DEV_STATE_MEM_SIZE     (sizeof(struct max96793_state))

/**
 * @enum max96793_phy
 * @brief Enumeration for MAX96793 PHY.
 */
enum max96793_phy {
	MAX96793_PHY_1 = 0u,    /*!< Phy 1 */
	MAX96793_PHY_2 = 1u,    /*!< Phy 2 */
};

/**
 * @struct max96793_dev_capabilities
 * @brief Structure representing the capabilities of the MAX96793 device.
 */
struct max96793_dev_capabilities {
	uint8_t max_links_count;     /*!< Maximum number of Links in the device */
	uint8_t max_lmn_count;       /*!< Maximum number of Line Fault Monitors in the device */
	uint8_t max_phy_ctrlr;       /*!< Maximum number of Phy controllers in the device */
};

/**
 * @struct max96793_state
 * @brief Structure representing MAX96793 device state.
 */
struct max96793_state {
	/* To_do: state variable or members to be added */
	uint8_t dummy;     /*!< MAX96793 state */
};

int32_t max96793_init(struct gmsl_dev **device, struct no_os_i2c_desc *i2c_desc,
		      void *init_param);
int32_t max96793_remove(struct gmsl_dev *dev);

#endif
