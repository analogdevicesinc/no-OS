/*******************************************************************************
 *   @file   max96792_max96793_pixel_mode_6G_linkA_linkB_example.c
 *   @brief  Configuration example application for MAX96792 gmsl part
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

#include <signal.h>
#include "max96792_max96793_example.h"
#include "diag_max96793_config.h"
#include "diag_max96792_config.h"

#define NUM_OF_LINKS (2)
#define NUM_OF_PIPES (2)
#define NUM_OF_PHYS  (4)
#define APP_NUM_OF_GMSL_DEVICES     (3u)

#define PAY_LOAD_SIZE               (2000u)

/* Handles for each GMSL device */
struct gmsl_dev *max96792_dev, *max96793_dev0, *max96793_dev1;
struct no_os_i2c_desc *i2c_desc;

/**
 * @brief Exit handler for the GMSL UC application.
 *
 * This function is called when an exit signal is detected. It performs cleanup
 * by removing all initialized devices and I2C descriptors, and then exits the application.
 *
 * @param signal The signal number that triggered the exit.
 */
static void gmsl_uc_exit_main(int signal)
{
	GMSL_LOG_INFO("Exit signal detected");

	/* Remove all initialized devices  */
	(void)max96792_remove(max96792_dev);
	(void)max96793_remove(max96793_dev0);
	(void)max96793_remove(max96793_dev1);
	(void)no_os_i2c_remove(i2c_desc);

	GMSL_LOG_INFO("Exiting Application!!!");

	exit(0);
}

/**
 * @brief Callback function for GMSL diagnostics.
 *
 * This function is called when a diagnostic event occurs.
 *
 * @param dev        Pointer to the GMSL device structure.
 * @param fault_type Enumeration indicating the type of diagnostic fault.
 * @param data       Pointer to additional data related to the diagnostic event.
 * @param pal_err    Flag indicating if the diagnostic check was failed due to PAL error.
 */
void gmsl_uc_diag_call_back(struct gmsl_dev *dev,
			    enum gmsl_dev_diag_sub_type fault_type, void* data, bool pal_err)
{
	GMSL_DBG_REQUIRE(dev != NULL, "Device handle is NULL");

	if (pal_err) {
		GMSL_LOG_ERROR("PAL Error Detected: %d", fault_type);
	} else {
		switch (fault_type) {
		case GMSL_DIAG_PART_DEV_ID:
			GMSL_LOG_INFO("Diagnostics Event reported for Device ID Check");
			break;
		case GMSL_DIAG_PART_DEV_REV:
			GMSL_LOG_INFO("Diagnostics Event reported for Device Revision Check");
			break;
		case GMSL_DIAG_PART_CFG:
			GMSL_LOG_INFO("Diagnostics Event reported for Part Configuration Error");
			break;
		case GMSL_DIAG_LINK_LOCK:
			GMSL_LOG_INFO("Diagnostics Event reported for Link Lock Error");
			break;
		case GMSL_DIAG_LINK_DECODE:
			GMSL_LOG_INFO("Diagnostics Event reported for Link Decode Error");
			break;
		case GMSL_DIAG_LINK_IDLE:
			GMSL_LOG_INFO("Diagnostics Event reported for Link Idle Error");
			break;
		case GMSL_DIAG_LINK_MAX_RETRANS:
			GMSL_LOG_INFO("Diagnostics Event reported for Link Maximum Retransmission Error");
			break;
		case GMSL_DIAG_LINK_LINE_FAULT:
			GMSL_LOG_INFO("Diagnostics Event reported for Line Fault Error");
			break;
		case GMSL_DIAG_LINK_FEC:
			GMSL_LOG_INFO("Diagnostics Event reported for FEC Error");
			break;
		case GMSL_DIAG_LINK_EOM:
			GMSL_LOG_INFO("Diagnostics Event reported for Eye Opening Monitoring Error");
			break;
		case GMSL_DIAG_VIDEO_PCLK_LOST:
			GMSL_LOG_INFO("Diagnostics Event reported for Video Pixel Clock Lost Error");
			break;
		case GMSL_DIAG_VIDEO_OVERFLOW:
			GMSL_LOG_INFO("Diagnostics Event reported for Video Overflow Error");
			break;
		case GMSL_DIAG_VIDEO_DRIFT:
			GMSL_LOG_INFO("Diagnostics Event reported for Video Drift Error");
			break;
		case GMSL_DIAG_VIDEO_STREAM_ID:
			GMSL_LOG_INFO("Diagnostics Event reported for Stream ID Error");
			break;
		case GMSL_DIAG_VIDEO_PARAMETER:
			GMSL_LOG_INFO("Diagnostics Event reported for Video Parameters Mismatch Error");
			break;
		case GMSL_DIAG_VIDEO_PPS:
			GMSL_LOG_INFO("Diagnostics Event reported for Video PPS Error");
			break;
		case GMSL_DIAG_HDCP:
			GMSL_LOG_INFO("Diagnostics Event reported for HDCP Error");
			break;
		case GMSL_DIAG_VIDEO_CRC_ERROR_STATUS:
			GMSL_LOG_INFO("Diagnostics Event reported for Video CRC Error");
			break;
		case GMSL_DIAG_MIPI_RX:
			GMSL_LOG_INFO("Diagnostics Event reported for MIPI RX Error");
			break;
		case GMSL_DIAG_MEM_ECC_2BIT_ERROR:
			GMSL_LOG_INFO("Diagnostics Event reported for Memory ECC 2bit Error");
			break;
		case GMSL_DIAG_LMO_STATUS:
			GMSL_LOG_INFO("Diagnostics Event reported for Line Memory Overflow (LMO) Error");
			break;
		case GMSL_DIAG_REMOTE_ERROR_FLAG:
			GMSL_LOG_INFO("Diagnostics Event reported for Remote Device Error");
			break;
		case GMSL_DIAG_VID_BLK_LENGTH_ERROR:
			GMSL_LOG_INFO("Diagnostics Event reported for Video Block Length Error");
			break;
		case GMSL_DIAG_PHY_LOW_POWER_ERR:
			GMSL_LOG_INFO("Diagnostics Event reported for PHY Low Power Error");
			break;
		case GMSL_DIAG_MIPI_STATUS:
			GMSL_LOG_INFO("Diagnostics Event reported for MIPI Status Error");
			break;
		default:
			GMSL_LOG_INFO("Diagnostics Event reported triggered for Unexpected Error");
			break;
		}
	}
}
/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int gmsl_uc_max96792_max96793_ex()
{
	int ret;
	uint8_t indx;
	bool error_i2c_remove = false;
	bool error_max96793_remove_dev0 = false;
	bool error_max96793_remove_dev1 = false;
	/* Pointer to I2C Descriptor for one GMSL device */
	struct no_os_i2c_desc  max96792_i2c_desc0, max96793_i2c_desc0,
		       max96793_i2c_desc1;

	/* PHY configuration for deserializer */
	struct gmsl_link_des_phy_cfg oDesPhyCfg[NUM_OF_PHYS];

	/* Pipe Configuration for deserializer */
	struct gmsl_pipe_des_pipe_cfg oDesPipeCfg[2];

	/* Port configuration for serializer */
	struct gmsl_csi_ser_port_cfg port_cfg;
	/* Pipe configuration for serializer */
	struct gmsl_pipe_ser_pipe_cfg oSerPipeCfg;

	signal(SIGINT, gmsl_uc_exit_main);

	/*Configure MAX96793 Serializer*/

	/* Configure MAX96793 Serializer Port */
	port_cfg.index =
		1;                                   /* Always port B selected */
	port_cfg.ctlr_idx =
		1;                                /* Only one controller-1 is connected to PHY's */
	port_cfg.lane_count = 4;                              /* 4 lanes */
	port_cfg.en_deskew = false;                           /* Deskew disabled */
	port_cfg.en_cphy = false;                             /* CPHY disabled */
	port_cfg.enabled = true;                              /* Port enabled */

	port_cfg.phy[GMSL_LINK_MIPI_CSI_CTRLR_0].id = 1;            /* PHY 1 */
	port_cfg.phy[0].enabled = true;                        /* PHY 1 enabled */
	port_cfg.phy[0].is_master = false;                     /* PHY 1 is master */
	port_cfg.phy[0].mipi.lane[0] = 2U;
	port_cfg.phy[0].mipi.lane[1] = 3U;
	port_cfg.phy[0].mipi.lane_polarities[0] = false;
	port_cfg.phy[0].mipi.lane_polarities[1] = false;
	port_cfg.phy[0].mipi.lane_polarities[2] = false;

	port_cfg.phy[GMSL_LINK_MIPI_CSI_CTRLR_1].id = 2;            /* PHY 2 */
	port_cfg.phy[1].enabled = true;                        /* PHY 2 enabled */
	port_cfg.phy[1].is_master = true;                      /* PHY 2 is not master */
	port_cfg.phy[1].mipi.lane[0] = 0U;
	port_cfg.phy[1].mipi.lane[1] = 1U;
	port_cfg.phy[1].mipi.lane_polarities[0] = false;
	port_cfg.phy[1].mipi.lane_polarities[1] = false;
	port_cfg.phy[1].mipi.lane_polarities[2] = false;

	/* Pipe configuration for serializer */
	oSerPipeCfg.index = 0;                                 /* Pipe 0 */
	oSerPipeCfg.phy_id = 2;                                /* PHY 2 */
	oSerPipeCfg.stream_id = 0;                             /* Stream 0 */
	oSerPipeCfg.dts = NULL;
	oSerPipeCfg.num_dts = 1;
	oSerPipeCfg.soft_bpp = 24;
	oSerPipeCfg.bpp = 12;                                  /* Bits per pixel */
	oSerPipeCfg.dbl8 = false;                              /* Double 8 bit mode */
	oSerPipeCfg.dbl10 = false;                             /* Double 10 bit mode */
	oSerPipeCfg.dbl12 = true;                              /* Double 12 bit mode */
	oSerPipeCfg.enabled = true;                            /* Pipe enabled */
	oSerPipeCfg.active = true;                             /* Pipe active */

	/*Configure MAX96792 Deserializer*/

	/* PHY configuration for Deserializer */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].index =
		0;                                    /* PHY index */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].link_frequency = 750000000;
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi_clk =
		1500;                              /* 1500 Mbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].phy_rate =
		GMSL_LINK_DES_RX_PHY_RATE_6G;     /* 12 Gbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].alt_mem_map8 =
		false;                         /* Alternate memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].alt_mem_map10 =
		false;                        /* Alternate memory map 10 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].alt_mem_map12 =
		true;                         /* Alternate memory map 12 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].alt2_mem_map8 =
		false;                        /* Alternate 2 memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.num_data_lanes =
		4;                      /* 4 data lanes */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.data_lanes[0] =
		MAX96792_DATA_LANE_2;    /* Data lane 0 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.data_lanes[1] =
		MAX96792_DATA_LANE_3;    /* Data lane 1 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.clock_lane =
		0;                          /* Clock lane */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.lane_polarities[0] =
		false;              /* Lane 0 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.lane_polarities[1] =
		false;              /* Lane 1 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.lane_polarities[2] =
		false;              /* Lane 2 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].mipi.lane_polarities[3] =
		false;              /* Lane 3 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].enabled =
		true;                               /* PHY enabled */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_0].cphy_enabled =
		false;                         /* CPHY disabled */

	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].index = 1; /* PHY index */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].link_frequency = 750000000;
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi_clk =
		1500;                               /* 1500 Mbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].phy_rate =
		GMSL_LINK_DES_RX_PHY_RATE_6G;      /* 6 Gbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].alt_mem_map8 =
		false;                          /* Alternate memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].alt_mem_map10 =
		false;                         /* Alternate memory map 10 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].alt_mem_map12 =
		false;                         /* Alternate memory map 12 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].alt2_mem_map8 =
		false;                         /* Alternate 2 memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.num_data_lanes =
		4;                       /* 4 data lanes */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.data_lanes[0] =
		MAX96792_DATA_LANE_0;     /* Data lane 0 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.data_lanes[1] =
		MAX96792_DATA_LANE_1;     /* Data lane 1 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.clock_lane =
		0;                           /* Clock lane */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.lane_polarities[0] =
		false;               /* Lane 0 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.lane_polarities[1] =
		false;               /* Lane 1 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.lane_polarities[2] =
		false;               /* Lane 2 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].mipi.lane_polarities[3] =
		false;               /* Lane 3 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].enabled =
		true;                                /* PHY enabled */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_1].cphy_enabled =
		false;                          /* CPHY disabled */

	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].index =
		2;                                      /* PHY index */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].link_frequency = 750000000;
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi_clk =
		1500;                                /* 1500 Mbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].phy_rate =
		GMSL_LINK_DES_RX_PHY_RATE_6G;       /* 6 Gbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].alt_mem_map8 =
		true;                            /* Alternate memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].alt_mem_map10 =
		false;                          /* Alternate memory map 10 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].alt_mem_map12 =
		true;                           /* Alternate memory map 12 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].alt2_mem_map8 =
		false;                          /* Alternate 2 memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.num_data_lanes =
		4;                        /* 4 data lanes */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.data_lanes[0] =
		MAX96792_DATA_LANE_0;      /* Data lane 0 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.data_lanes[1] =
		MAX96792_DATA_LANE_1;      /* Data lane 1 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.clock_lane =
		0;                            /* Clock lane */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.lane_polarities[0] =
		false;                /* Lane 0 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.lane_polarities[1] =
		false;                /* Lane 1 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.lane_polarities[2] =
		false;                /* Lane 2 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].mipi.lane_polarities[3] =
		false;                /* Lane 3 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].enabled =
		false;                                 /* PHY enabled */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_2].cphy_enabled =
		false;                           /* CPHY disabled */

	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].index =
		3;                                       /* PHY index */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].link_frequency = 750000000;
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi_clk =
		1500;                                 /* 1500 Mbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].phy_rate =
		GMSL_LINK_DES_RX_PHY_RATE_6G;        /* 6 Gbps */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].alt_mem_map8 =
		false;                            /* Alternate memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].alt_mem_map10 =
		false;                           /* Alternate memory map 10 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].alt_mem_map12 =
		false;                           /* Alternate memory map 12 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].alt2_mem_map8 =
		false;                           /* Alternate 2 memory map 8 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.num_data_lanes =
		MAX96792_MAX_DATA_LANES;   /* 4 data lanes */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.data_lanes[0] =
		MAX96792_DATA_LANE_2;       /* Data lane 0 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.data_lanes[1] =
		MAX96792_DATA_LANE_3;       /* Data lane 1 */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.clock_lane =
		0;                             /* Clock lane */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.lane_polarities[0] =
		false;                 /* Lane 0 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.lane_polarities[1] =
		false;                 /* Lane 1 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.lane_polarities[2] =
		false;                 /* Lane 2 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].mipi.lane_polarities[3] =
		false;                 /* Lane 3 polarity */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].enabled =
		false;                                  /* PHY enabled */
	oDesPhyCfg[GMSL_LINK_MIPI_CSI_PHY_3].cphy_enabled =
		false;                            /* CPHY disabled */

	/* Pipe Configuration for Deserializer */

	oDesPipeCfg[0].index = GMSL_PIPE_Y;                               /* Pipe 1 */
	oDesPipeCfg[0].stream_id = MAX96792_CSI_STREAM_ID_0;              /* Stream 0 */
	oDesPipeCfg[0].link_id = GMSL_LINK_A;
	oDesPipeCfg[0].num_remaps =
		3;                                    /* Number of remaps */
	oDesPipeCfg[0].remaps[0].from_dt =
		MAX96792_DATA_TYPE_RAW12;      /* From data type */
	oDesPipeCfg[0].remaps[0].from_vc =
		0;                             /* From virtual channel */
	oDesPipeCfg[0].remaps[0].to_dt =
		MAX96792_DATA_TYPE_RAW12;        /* To data type */
	oDesPipeCfg[0].remaps[0].to_vc =
		0;                               /* To virtual channel */
	oDesPipeCfg[0].remaps[0].phy = GMSL_LINK_MIPI_CSI_CTRLR_1;             /* PHY */
	oDesPipeCfg[0].remaps[1].from_dt =
		MAX96792_DATA_TYPE_DEFAULT;    /* From data type */
	oDesPipeCfg[0].remaps[1].from_vc =
		0;                             /* From virtual channel */
	oDesPipeCfg[0].remaps[1].to_dt =
		MAX96792_DATA_TYPE_DEFAULT;      /* To data type */
	oDesPipeCfg[0].remaps[1].to_vc =
		0;                               /* To virtual channel */
	oDesPipeCfg[0].remaps[1].phy = GMSL_LINK_MIPI_CSI_CTRLR_1;             /* PHY */
	oDesPipeCfg[0].remaps[2].from_dt =
		1;                             /* From data type */
	oDesPipeCfg[0].remaps[2].from_vc =
		0;                             /* From virtual channel */
	oDesPipeCfg[0].remaps[2].to_dt =
		1;                               /* To data type */
	oDesPipeCfg[0].remaps[2].to_vc =
		0;                               /* To virtual channel */
	oDesPipeCfg[0].remaps[2].phy = GMSL_LINK_MIPI_CSI_CTRLR_1;             /* PHY */
	oDesPipeCfg[0].input_pipe =
		GMSL_PIPE_Y;                          /* Input pipe */
	oDesPipeCfg[0].ser_cphy_mode =
		false;                             /* CPHY mode */
	oDesPipeCfg[0].dbl8 =
		false;                                      /* Double 8 bit mode */
	oDesPipeCfg[0].dbl10 =
		false;                                     /* Double 10 bit mode */
	oDesPipeCfg[0].dbl12 =
		false;                                     /* Double 12 bit mode */
	oDesPipeCfg[0].dbl8mode =
		false;                                  /* Double 8 mode */
	oDesPipeCfg[0].dbl10mode =
		false;                                 /* Double 10 mode */
	oDesPipeCfg[0].enabled =
		true;                                     /* Pipe enabled */

	oDesPipeCfg[1].index = GMSL_PIPE_Z;                                /* Pipe 2*/
	oDesPipeCfg[1].stream_id =
		MAX96792_CSI_STREAM_ID_0;               /* Stream 0 */
	oDesPipeCfg[1].link_id = GMSL_LINK_B;
	oDesPipeCfg[1].num_remaps =
		3;                                     /* Number of remaps */
	oDesPipeCfg[1].remaps[0].from_dt =
		MAX96792_DATA_TYPE_RAW12;       /* From data type */
	oDesPipeCfg[1].remaps[0].from_vc =
		0;                              /* From virtual channel */
	oDesPipeCfg[1].remaps[0].to_dt =
		MAX96792_DATA_TYPE_RAW12;         /* To data type */
	oDesPipeCfg[1].remaps[0].to_vc =
		1;                                /* To virtual channel */
	oDesPipeCfg[1].remaps[0].phy =
		GMSL_LINK_MIPI_CSI_CTRLR_1;              /* PHY */
	oDesPipeCfg[1].remaps[1].from_dt =
		MAX96792_DATA_TYPE_DEFAULT;     /* From data type */
	oDesPipeCfg[1].remaps[1].from_vc =
		0;                              /* From virtual channel */
	oDesPipeCfg[1].remaps[1].to_dt =
		MAX96792_DATA_TYPE_DEFAULT;       /* To data type */
	oDesPipeCfg[1].remaps[1].to_vc =
		1;                                /* To virtual channel */
	oDesPipeCfg[1].remaps[1].phy =
		GMSL_LINK_MIPI_CSI_CTRLR_1;              /* PHY */
	oDesPipeCfg[1].remaps[2].from_dt =
		1;                              /* From data type */
	oDesPipeCfg[1].remaps[2].from_vc =
		0;                              /* From virtual channel */
	oDesPipeCfg[1].remaps[2].to_dt =
		1;                                /* To data type */
	oDesPipeCfg[1].remaps[2].to_vc =
		1;                                /* To virtual channel */
	oDesPipeCfg[1].remaps[2].phy =
		GMSL_LINK_MIPI_CSI_CTRLR_1;              /* PHY */
	oDesPipeCfg[1].input_pipe =
		GMSL_PIPE_Z;                           /* Input pipe */
	oDesPipeCfg[1].ser_cphy_mode =
		false;                              /* CPHY mode */
	oDesPipeCfg[1].dbl8 =
		false;                                       /* Double 8 bit mode */
	oDesPipeCfg[1].dbl10 =
		false;                                      /* Double 10 bit mode */
	oDesPipeCfg[1].dbl12 =
		false;                                      /* Double 12 bit mode */
	oDesPipeCfg[1].dbl8mode =
		false;                                   /* Double 8 mode */
	oDesPipeCfg[1].dbl10mode =
		false;                                  /* Double 10 mode */
	oDesPipeCfg[1].enabled =
		true;                                     /* Pipe enabled */
	GMSL_LOG_INFO("***************************CONFIG & DIAG EXAMPLE****************************");

	/* I2C Initialization */
	ret = no_os_i2c_init(&i2c_desc, &gmsl_i2c2_ip);
	if (ret < 0)
		return -ENODEV;

	/* MAX96792 Deserializer Configuration Parameters */
	max96792_i2c_desc0 = *i2c_desc;
	max96792_i2c_desc0.slave_address = (0x98 >> 1); /* MAX96792 I2C Address */

	struct gmsl_dev_csi_des_init_param max96792_init_param = {
		.dev_name = "Camera MAX96792 Deserializer",
		.index = 0,
		.tunnel_mode = false,
		.dev_instance_mem = NULL,   /** @todo: Implement memory allocation from application for drivers */
		.dev_state_mem = NULL,      /** @todo: Implement memory allocation from application for drivers */
		.link_rx_rate[GMSL_LINK_A] = GMSL_LINK_DES_RX_PHY_RATE_6G,
		.link_rx_rate[GMSL_LINK_B] = GMSL_LINK_DES_RX_PHY_RATE_6G,
	};

	/* MAX96793 Serializer 1 Configuration Parameters*/
	max96793_i2c_desc0 = *i2c_desc;
	max96793_i2c_desc0.slave_address = (0x80 >>
					    1);  /* MAX96793 I2C Power-up Address */

	struct gmsl_dev_csi_ser_init_param max96793_dev0_init_param = {
		.index = 1,
		.dev_name = "Camera #1",
		.dev_instance_mem = NULL,
		.dev_state_mem = NULL
	};

	/* MAX96793 Serializer 2 Configuration Parameters */
	max96793_i2c_desc1 = *i2c_desc;
	max96793_i2c_desc1.slave_address = (0x80 >>
					    1);  /* MAX96793 I2C Power-up Address */

	struct gmsl_dev_csi_ser_init_param max96793_dev1_init_param = {
		.index = 2,
		.dev_name = "Camera #2",
		.dev_instance_mem = NULL,
		.dev_state_mem = NULL
	};

	do {
		/* Initialize max96792 */
		ret = max96792_init(&max96792_dev, &max96792_i2c_desc0,
				    (void *)&max96792_init_param);
		if (ret < 0) {
			error_i2c_remove = true;
			break;
		}
		/* Assigning device for diagnostics */
		max96792_diag_mngr_cfg.dev = max96792_dev;

		/* Configure Deserializer  */
		ret = max96792_csi_enable_mipi_out(max96792_dev, false);
		ret = max96792_csi_init(max96792_dev);
		ret = max96792_csi_update_link_config(max96792_dev, GMSL_BOTH_LINKS_CFG);
		ret = max96792_csi_select_links(max96792_dev, GMSL_BOTH_LINKS_CFG);
		ret = max96792_csi_set_rx_link_rate(max96792_dev, &max96792_init_param,
						    GMSL_BOTH_LINKS_CFG);

		/* PHYs initialization */
		for (indx = GMSL_LINK_MIPI_CSI_PHY_0 ; indx <= GMSL_LINK_MIPI_CSI_PHY_3;
		     indx++) {
			ret = max96792_csi_init_phy(max96792_dev, &oDesPhyCfg[indx]);
		}

		/* Pipes  initialization */
		for (indx = 0; indx < NUM_OF_PIPES; indx++) {
			ret = max96792_pipe_set_tx_mode(max96792_dev, &oDesPipeCfg[indx]);
			ret = max96792_mipi_tunnel_pipe_cntrl(max96792_dev, &oDesPipeCfg[indx],
							      GMSL_LINK_MIPI_CSI_CTRLR_0 + indx);
			ret = max96792_csi_init_pipe(max96792_dev, false, &oDesPipeCfg[indx]);
			ret = max96792_csi_update_pipe_remaps(max96792_dev, &oDesPipeCfg[indx]);
		}

		ret = max96792_csi_enable_mipi_out(max96792_dev, true);

		/* Initialize Serializer 1 */
		ret = max96793_init(&max96793_dev0, &max96793_i2c_desc0,
				    (void *)&max96793_dev0_init_param);
		if (ret < 0) {
			error_max96793_remove_dev0 = true;
			break;
		}

		/* Initialize Serializer 2 */
		ret = max96793_init(&max96793_dev1, &max96793_i2c_desc1,
				    (void *)&max96793_dev1_init_param);
		if (ret < 0) {
			error_max96793_remove_dev1 = true;
			break;
		}

		/* Disable Link A Remote Control Channel */
		ret = max96792_cc_enable_remote_control_chnl(max96792_dev, GMSL_LINK_A, false);
		no_os_mdelay(10);
		/* Change Serializer 2 Device address */
		ret = max96793_csi_change_ser_address(max96793_dev1, 0x82);
		/* Update I2C address for Serializer 2 I2C handle */
		max96793_i2c_desc1.slave_address = (0x82 >> 1);
		/* Enable Link A Remote Control Channel */
		ret = max96792_cc_enable_remote_control_chnl(max96792_dev, GMSL_LINK_A, true);
		no_os_mdelay(10);

		/* Assigning device for diagnostics */
		max96793_diag_mngr_cfg0.dev = max96793_dev0;
		/* Configure Serializer  1 */
		ret = max96793_csi_set_pipe_enable(max96793_dev0, false);
		ret = max96793_csi_init(max96793_dev0, false);
		ret = max96793_csi_init_port(max96793_dev0, &port_cfg);
		ret = max96793_csi_init_pipe(max96793_dev0, &oSerPipeCfg);
		GMSL_LOG_INFO("MAX96793 SER-1 Configured");

		/* Assigning device for diagnostics */
		max96793_diag_mngr_cfg1.dev = max96793_dev1;
		/* Configure Serializer  2 */
		ret = max96793_csi_set_pipe_enable(max96793_dev1, false);
		ret = max96793_csi_init(max96793_dev1, false);
		ret = max96793_csi_init_port(max96793_dev1, &port_cfg);
		ret = max96793_csi_init_pipe(max96793_dev1, &oSerPipeCfg);
		GMSL_LOG_INFO("MAX96793 SER-2 Configured");

		/* Configure GPIO0 on Serializers for enabling Cameras */
		ret = max96793_gpio_cam_cfg(max96793_dev0);
		ret = max96793_gpio_cam_cfg(max96793_dev1);

		/* Update I2C address in the I2C handle with Camera Device Address */
		i2c_desc->slave_address = (0x6c >> 1);
		/* COnfigure Camera */
		ovx3a_cam_init(i2c_desc);
		GMSL_LOG_INFO("OVX3A Camera Configured");

		struct gmsl_diag_mngr_cfg *diag_mngr_config[APP_NUM_OF_GMSL_DEVICES] = {
			[0] = &max96792_diag_mngr_cfg,
			[1] = &max96793_diag_mngr_cfg0,
			[2] = &max96793_diag_mngr_cfg1,
		};

		uint8_t oPayLoadBuffer[PAY_LOAD_SIZE];

		(void)gmsl_diag_mngr_init((struct gmsl_diag_mngr_cfg **)diag_mngr_config,
					  APP_NUM_OF_GMSL_DEVICES, (void *)&oPayLoadBuffer);

		GMSL_LOG_INFO("Diagnostics Manager Initialized for Deserializer and Serializers");

		while (1) {
			(void)gmsl_diag_mngr_proc();
			no_os_mdelay(10);
		}

	} while (false);

	/* Remove Devices */
	ret = max96792_remove(max96792_dev);
	if (error_max96793_remove_dev1)
		ret = max96793_remove(max96793_dev1);

	if (error_max96793_remove_dev0 || error_max96793_remove_dev1)
		ret = max96793_remove(max96793_dev0);

	if (error_max96793_remove_dev0 || error_max96793_remove_dev1
	    || error_i2c_remove) {
		ret = no_os_i2c_remove(i2c_desc);
		if (ret < 0) {
			GMSL_LOG_ERROR("i2c remove failed");
		} else {
			/*Do Nothing*/
		}
	}
	if (!(error_max96793_remove_dev0 && error_max96793_remove_dev1
	      && error_i2c_remove)) {
		ret = max96793_remove(max96793_dev1);
		ret = max96793_remove(max96793_dev0);
		ret = no_os_i2c_remove(i2c_desc);
		if (ret < 0) {
			GMSL_LOG_ERROR("i2c remove failed");
		} else {
			/*Do Nothing*/
		}
	}
	return ret;
}