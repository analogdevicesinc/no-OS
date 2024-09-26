/*******************************************************************************
 *   @file   basic_example.h
 *   @brief  PQM firmware header file, called by main
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
