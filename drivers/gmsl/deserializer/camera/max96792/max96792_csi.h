/*****************************************************************************
*   @file   max96792_csi.h
*   @brief  Header file of CSI Driver Implementation of MAX96792 Deserializer.
*   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAX96792_CSI_H
#define MAX96792_CSI_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "gmsl_common.h"
#include "gmsl_cam_des.h"
#include "gmsl_dbg.h"
#include "gmsl_reg_access.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "max96792.h"

extern int max96792_csi_update_pipe_remaps(struct gmsl_dev *dev,
		struct gmsl_pipe_des_pipe_cfg *pipe);
extern int max96792_csi_select_links(struct gmsl_dev *dev, unsigned int mask);
extern int max96792_cc_enable_remote_control_chnl(struct gmsl_dev *dev,
		enum gmsl_link link, bool enable);
extern int max96792_csi_enable_mipi_out(struct gmsl_dev *dev, bool enable);
extern int max96792_csi_init_pipe(struct gmsl_dev *dev,
				  bool pipe_stream_autoselect, struct gmsl_pipe_des_pipe_cfg *pipe);
extern int max96792_csi_init_phy(struct gmsl_dev *dev,
				 struct gmsl_link_des_phy_cfg *phy);
extern int max96792_csi_init(struct gmsl_dev *dev);
extern int max96792_csi_set_cphy_config(struct gmsl_dev *dev,
					struct gmsl_link_des_phy_cfg *phy);
extern int max96792_csi_set_dphy_config(struct gmsl_dev *dev,
					struct gmsl_link_des_phy_cfg *phy);
extern int max96792_csi_set_rx_link_rate(struct gmsl_dev *dev,
		struct gmsl_dev_csi_des_init_param *init_param, unsigned int mask);
extern int max96792_mipi_tunnel_pipe_cntrl(struct gmsl_dev *dev,
		struct gmsl_pipe_des_pipe_cfg *pipe,
		enum gmsl_link_mipi_csi_controller dst_cntroller);
extern int max96792_pipe_set_tx_mode(struct gmsl_dev *dev,
				     struct gmsl_pipe_des_pipe_cfg *pipe);
extern int max96792_csi_copy1_phy_cfg(struct gmsl_dev *dev, bool copy_en,
				      enum gmsl_link_mipi_csi_controller *src_cont,
				      enum gmsl_link_mipi_csi_controller *dest_cont);
extern int max96792_csi_copy1_phy_cfg(struct gmsl_dev *dev, bool copy_en,
				      enum gmsl_link_mipi_csi_controller *src_cont,
				      enum gmsl_link_mipi_csi_controller *dest_cont);
extern int max96792_csi_update_link_config(struct gmsl_dev *dev,
		unsigned int mask);
#endif