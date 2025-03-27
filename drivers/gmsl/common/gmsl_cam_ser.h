/***************************************************************************/
/**
 *   @file   gmsl_cam_ser.h
 *   @brief  Header file of gmsl camera serializer common interface
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef GMSL_CAM_SER_H
#define GMSL_CAM_SER_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define GMSL_CAM_SER_CSI2_MAX_MIPI_PHY_PER_PORT	                (2) 
#define GMSL_CAM_SER_CSI2_MAX_SENSOR_LANES	                    (4) 
#define GMSL_CAM_SER_CSI2_MAX_DATA_LANES	                    (4)
#define GMSL_CAM_SER_CSI2_MAX_DATA_LANES_PER_MIPI_PHY	        (2)  

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct gmsl_csi_ser_mipi_cfg
 * @brief Configuration structure for GMSL Camera Serializer MIPI CSI2 interface.
 */
struct gmsl_csi_ser_mipi_cfg {
	unsigned int lane[GMSL_CAM_SER_CSI2_MAX_DATA_LANES_PER_MIPI_PHY];         /*!< GMSL Camera Serializer MIPI CSI2 data lane to sensor lane mapping. \
	                                                                          This field is used to create Serializer Data lane to Sensor Lane Mapping. \
																              (PHY D0) Ser Data Lane 0 --> Sensor Lane 0/1/2/3 \
																              (PHY D1) Ser Data Lane 1 --> Sensor Lane 0/1/2/3 */

	bool lane_polarities[GMSL_CAM_SER_CSI2_MAX_DATA_LANES_PER_MIPI_PHY + 1];  /*!< GMSL Camera Serializer MIPI CSI2 lane polarities -> true/false. \
	                                                                          + 1 is to accomodate Clock Lane. */
};

/**
 * @struct gmsl_pipe_ser_pipe_cfg
 * @brief A structure that contains configuration parameters for a GMSL camera serializer pipe.
 */
struct gmsl_pipe_ser_pipe_cfg {
	unsigned int index;                 /*!< GMSL Camera Serializer pipe index */
	unsigned int phy_id;                /*!< GMSL Camera Serializer mipi phy id assocoated to the pipe */
	unsigned int stream_id;             /*!< GMSL Camera Serializer stream id */
	unsigned int *dts;                  /*!< GMSL Camera Serializer input data types */
	unsigned int num_dts;               /*!< GMSL Camera Serializer number of input data types */
	unsigned int vcs;                   /*!< GMSL Camera Serializer input virtual cahannel ids */
	unsigned int soft_bpp;              /*!< GMSL Camera Serializer soft bits-per-pixel */
	unsigned int bpp;                   /*!< GMSL Camera Serializer bits-per-pixel */
	bool dbl8;                          /*!< GMSL Camera Serializer double-8bit enable */
	bool dbl10;                         /*!< GMSL Camera Serializer double-10bit enable */
	bool dbl12;                         /*!< GMSL Camera Serializer double-12bit enable */
	bool enabled;                       /*!< GMSL Camera Serializer pipe enable */
	bool active;                        /*!< GMSL Camera Serializer set pipe active */
};

/**
 * @struct gmsl_link_ser_phy_cfg
 * @brief A structure that contains configuration parameters for a GMSL camera serializer MIPI PHY.
 */
struct gmsl_link_ser_phy_cfg {
	unsigned int id;                              /*!< GMSL Camera Serializer MIPI PHY Id (1 - 2)*/
	struct gmsl_csi_ser_mipi_cfg mipi;   		  /*!< GMSL Camera Serializer MIPI PHY configs */
	bool enabled;                                 /*!< GMSL Camera Serializer MIPI PHY enable/disable */
	bool is_master;                               /*!< GMSL Camera Serializer MIPI PHY flag to set PHY as master */
};

/**
 * @struct gmsl_csi_ser_port_cfg
 * @brief A structure that contains configuration parameters for a GMSL camera serializer MIPI port.
 */
struct gmsl_csi_ser_port_cfg {
	unsigned int index;                                                    /*!< GMSL Camera Serializer MIPI PORT index (0 = default) */
	unsigned int ctlr_idx;                                                 /*!< GMSL Camera Serializer MIPI PORT controller ID */
	unsigned int lane_count;                                               /*!< GMSL Camera Serializer MIPI PORT Lane Count */
	bool en_deskew;                                                        /*!< GMSL Camera Serializer MIPI PORT enable/disable Deskew */
	bool en_cphy;                                                          /*!< GMSL Camera Serializer MIPI PORT enable/disable CPHY */
	bool enabled;                                                          /*!< GMSL Camera Serializer MIPI PORT enable/disable */
	struct gmsl_link_ser_phy_cfg phy[GMSL_CAM_SER_CSI2_MAX_MIPI_PHY_PER_PORT];  /*!< GMSL Camera Serializer MIPI PHYs associated to this PORT */
};

/**
 * @struct gmsl_dev_csi_ser_init_param
 * @brief GMSL Camera Serializer driver initailization structure
 */
struct gmsl_dev_csi_ser_init_param {
	char *dev_name;											 /*!< GMSL Camera Serializer custom device name */
	uint8_t index;											 	 /*!< GMSL devices index */
	/** @todo: Implement memory allocation from application for drivers */
	void *dev_instance_mem;									 	 /*!< GMSL Camera Serializer device instance memory */
	void *dev_state_mem;										 /*!< GMSL Camera Serializer device state memory */
}; 

#endif /* GMSL_CAM_SER_H */
