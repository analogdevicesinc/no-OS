/*******************************************************************************
 *   @file   diag_max96793_config.h
 *   @brief  Header file for configuration needed by Diagnostic example 
 *           application for MAX96793 device.
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

#ifndef DIAG_MAX96793_CONFIG_H
#define DIAG_MAX96793_CONFIG_H

#include "diag_mngr.h"

extern void gmsl_uc_diag_call_back(struct gmsl_dev *dev, enum gmsl_dev_diag_sub_type fault_type, void* data, bool pal_err);

static struct gmsl_diag_type_cfg max96793_diag_type_devid = 
{
    .diag_sub_type = GMSL_DIAG_PART_DEV_ID,
    .interval_in_sec = START_UP,
    .call_back = &gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_devrev = 
{
    .diag_sub_type = GMSL_DIAG_PART_DEV_REV,
    .interval_in_sec = START_UP,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_partcfg = 
{
    .diag_sub_type = GMSL_DIAG_PART_CFG,
    .interval_in_sec = START_UP,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linklock = 
{
    .diag_sub_type = GMSL_DIAG_LINK_LOCK,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linkdecode = 
{
    .diag_sub_type = GMSL_DIAG_LINK_DECODE,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linkidle = 
{
    .diag_sub_type = GMSL_DIAG_LINK_IDLE,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};


static struct gmsl_diag_type_cfg max96793_diag_type_linkmaxtrans = 
{
    .diag_sub_type = GMSL_DIAG_LINK_MAX_RETRANS,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linefault = 
{
    .diag_sub_type = GMSL_DIAG_LINK_LINE_FAULT,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linkfec = 
{
    .diag_sub_type = GMSL_DIAG_LINK_FEC,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_linkeom = 
{
    .diag_sub_type = GMSL_DIAG_LINK_EOM,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_vidpclklost = 
{
    .diag_sub_type = GMSL_DIAG_VIDEO_PCLK_LOST,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_vidovrflow = 
{
    .diag_sub_type = GMSL_DIAG_VIDEO_OVERFLOW,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_viddrift = 
{
    .diag_sub_type = GMSL_DIAG_VIDEO_DRIFT,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_vidstreamid = 
{
    .diag_sub_type = GMSL_DIAG_VIDEO_STREAM_ID,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_mipi_rx_err = 
{
    .diag_sub_type = GMSL_DIAG_MIPI_RX,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_mem_ecc_2b_err = 
{
    .diag_sub_type = GMSL_DIAG_MEM_ECC_2BIT_ERROR,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_phy_low_power_err = 
{
    .diag_sub_type = GMSL_DIAG_PHY_LOW_POWER_ERR,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_mipi_status = 
{
    .diag_sub_type = GMSL_DIAG_MIPI_STATUS,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

static struct gmsl_diag_type_cfg max96793_diag_type_remote_error = 
{
    .diag_sub_type = GMSL_DIAG_REMOTE_ERROR_FLAG,
    .interval_in_sec = INTERVAL_5_SEC,
    .call_back = gmsl_uc_diag_call_back,
};

struct gmsl_diag_mngr_cfg max96793_diag_mngr_cfg0 = 
{
    .diag_type_cfg[GMSL_DIAG_PART_DEV_ID] = &max96793_diag_type_devid,
    .diag_type_cfg[GMSL_DIAG_PART_DEV_REV] = &max96793_diag_type_devrev,
    .diag_type_cfg[GMSL_DIAG_PART_CFG] = &max96793_diag_type_partcfg,
    .diag_type_cfg[GMSL_DIAG_LINK_LOCK] = &max96793_diag_type_linklock,
    .diag_type_cfg[GMSL_DIAG_LINK_DECODE] = &max96793_diag_type_linkdecode,
    .diag_type_cfg[GMSL_DIAG_LINK_IDLE] = &max96793_diag_type_linkidle,
    .diag_type_cfg[GMSL_DIAG_LINK_MAX_RETRANS] = &max96793_diag_type_linkmaxtrans,
    .diag_type_cfg[GMSL_DIAG_LINK_LINE_FAULT] = &max96793_diag_type_linefault,
    .diag_type_cfg[GMSL_DIAG_LINK_FEC] = &max96793_diag_type_linkfec,
    .diag_type_cfg[GMSL_DIAG_LINK_EOM] = &max96793_diag_type_linkeom,
    .diag_type_cfg[GMSL_DIAG_VIDEO_PCLK_LOST] = &max96793_diag_type_vidpclklost,
    .diag_type_cfg[GMSL_DIAG_VIDEO_OVERFLOW] = &max96793_diag_type_vidovrflow,
    .diag_type_cfg[GMSL_DIAG_VIDEO_DRIFT] = &max96793_diag_type_viddrift,
    .diag_type_cfg[GMSL_DIAG_VIDEO_STREAM_ID] = &max96793_diag_type_vidstreamid,
    .diag_type_cfg[GMSL_DIAG_MIPI_RX] = &max96793_diag_type_mipi_rx_err,
    .diag_type_cfg[GMSL_DIAG_MEM_ECC_2BIT_ERROR] = &max96793_diag_type_mem_ecc_2b_err,
    .diag_type_cfg[GMSL_DIAG_PHY_LOW_POWER_ERR] = &max96793_diag_type_phy_low_power_err,
    .diag_type_cfg[GMSL_DIAG_MIPI_STATUS] = &max96793_diag_type_mipi_status,
    .diag_type_cfg[GMSL_DIAG_REMOTE_ERROR_FLAG] = &max96793_diag_type_remote_error,
};

struct gmsl_diag_mngr_cfg max96793_diag_mngr_cfg1 = 
{
    .diag_type_cfg[GMSL_DIAG_PART_DEV_ID] = &max96793_diag_type_devid,
    .diag_type_cfg[GMSL_DIAG_PART_DEV_REV] = &max96793_diag_type_devrev,
    .diag_type_cfg[GMSL_DIAG_PART_CFG] = &max96793_diag_type_partcfg,
    .diag_type_cfg[GMSL_DIAG_LINK_LOCK] = &max96793_diag_type_linklock,
    .diag_type_cfg[GMSL_DIAG_LINK_DECODE] = &max96793_diag_type_linkdecode,
    .diag_type_cfg[GMSL_DIAG_LINK_IDLE] = &max96793_diag_type_linkidle,
    .diag_type_cfg[GMSL_DIAG_LINK_MAX_RETRANS] = &max96793_diag_type_linkmaxtrans,
    .diag_type_cfg[GMSL_DIAG_LINK_LINE_FAULT] = &max96793_diag_type_linefault,
    .diag_type_cfg[GMSL_DIAG_LINK_FEC] = &max96793_diag_type_linkfec,
    .diag_type_cfg[GMSL_DIAG_LINK_EOM] = &max96793_diag_type_linkeom,
    .diag_type_cfg[GMSL_DIAG_VIDEO_PCLK_LOST] = &max96793_diag_type_vidpclklost,
    .diag_type_cfg[GMSL_DIAG_VIDEO_OVERFLOW] = &max96793_diag_type_vidovrflow,
    .diag_type_cfg[GMSL_DIAG_VIDEO_DRIFT] = &max96793_diag_type_viddrift,
    .diag_type_cfg[GMSL_DIAG_VIDEO_STREAM_ID] = &max96793_diag_type_vidstreamid,
    .diag_type_cfg[GMSL_DIAG_MIPI_RX] = &max96793_diag_type_mipi_rx_err,
    .diag_type_cfg[GMSL_DIAG_MEM_ECC_2BIT_ERROR] = &max96793_diag_type_mem_ecc_2b_err,
    .diag_type_cfg[GMSL_DIAG_PHY_LOW_POWER_ERR] = &max96793_diag_type_phy_low_power_err,
    .diag_type_cfg[GMSL_DIAG_MIPI_STATUS] = &max96793_diag_type_mipi_status,
    .diag_type_cfg[GMSL_DIAG_REMOTE_ERROR_FLAG] = &max96793_diag_type_remote_error,
};

#endif /* DIAG_MAX96793_CONFIG_H */
