/***************************************************************************//**
 *   @file   nmea_ubx_gnss.h
 *   @brief  Platform-specific implementation for NMEA/UBX GNSS devices
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef NMEA_UBX_GNSS_H_
#define NMEA_UBX_GNSS_H_

#include "no_os_gnss.h"
#include "nmea_ubx.h"

/**
 * @struct nmea_ubx_gnss_extra
 * @brief Platform-specific extra data for NMEA/UBX GNSS implementation
 */
struct nmea_ubx_gnss_extra {
	/** Underlying GNSS driver device pointer */
	struct gnss_dev *gnss_device;
};

/**
 * @struct nmea_ubx_gnss_init_param
 * @brief Platform-specific initialization parameters for NMEA/UBX GNSS
 */
struct nmea_ubx_gnss_init_param {
	/** Underlying GNSS driver initialization parameters */
	struct gnss_init_param gnss_init_param;
};

/**
 * @brief NMEA/UBX GNSS platform operations
 */
extern const struct no_os_gnss_platform_ops nmea_ubx_gnss_ops;

#endif /* NMEA_UBX_GNSS_H_ */
