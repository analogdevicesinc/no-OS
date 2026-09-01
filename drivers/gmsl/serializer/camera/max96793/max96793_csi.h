/*******************************************************************************
 *   @file   max96793_csi.h
 *   @brief  Header file of CSI Driver Implementation of MAX96793 Serializer.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

