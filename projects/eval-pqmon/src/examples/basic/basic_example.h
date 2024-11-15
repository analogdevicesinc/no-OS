/*******************************************************************************
 *   @file   basic_example.h
 *   @brief  PQM firmware header file, called by main
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PQM_FW_H__
#define __PQM_FW_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "common_data.h"
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief Initialize pqm descriptor
 *
 * @param desc Object descriptor
 * @param param Initializing paramters
 * @return Return 0 on success, different from 0 otherwise
 */
int32_t pqm_init(struct pqm_desc **desc, struct pqm_init_para *param);

/**
 * @brief Remove pqm descriptor
 *
 * @param desc Frees up memory space allocated for descriptor
 * @return Return 0 on success, different from 0 otherwise
 */
int32_t pqm_remove(struct pqm_desc *desc);

/**
 * @brief active pqm channels
 * @param dev - descriptor for the pqm
 * @param mask - active channels mask
 * @return 0 in case of success, -1 otherwise.
 */
int32_t update_pqm_channels(void *dev, uint32_t mask);

/**
 * @brief close all channels
 * @param dev - physical instance of an pqm device
 * @return 0 in case of success.
 */
int32_t close_pqm_channels(void *dev);

/**
 * @brief PQM example main execution
 *
 * @return If working correctly, will execute continuously function iio_app_run
 * and will not return.
 */
int basic_pqm_firmware();

#endif /* __PQM_FW_H__ */
