/*******************************************************************************
 *   @file   max96793_csi.h
 *   @brief  Header file of CSI Driver Implementation of MAX96793 Serializer.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef MAX96793_CSI_H
#define MAX96793_CSI_H

#include "gmsl_common.h"
#include "gmsl_cam_ser.h"
#include "no_os_delay.h"
int32_t max96793_csi_change_ser_address(struct gmsl_dev *dev,
					unsigned int new_addr);

int32_t max96793_csi_set_pipe_enable(struct gmsl_dev *dev, bool enable);

int32_t max96793_csi_config_sensor_to_ser_lane_map(struct gmsl_dev *dev,
		struct gmsl_link_ser_phy_cfg *phy);

int32_t max96793_csi_config_sensor_lane_pol(struct gmsl_dev *dev,
		struct gmsl_link_ser_phy_cfg *phy);

int32_t max96793_csi_init_port(struct gmsl_dev *dev,
			       struct gmsl_csi_ser_port_cfg *port);

int32_t max96793_csi_init(struct gmsl_dev *dev, bool tunnel_mode);

int32_t max96793_csi_init_pipe(struct gmsl_dev *dev,
			       struct gmsl_pipe_ser_pipe_cfg *pipe);

extern int max96793_gpio_cam_cfg(struct gmsl_dev *dev);
#endif

