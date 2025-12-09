/***************************************************************************//**
 *   @file   nmea_ubx_gnss.h
 *   @brief  Platform-specific implementation for NMEA/UBX GNSS devices
 *   @author Radu Etz (radu.etz@analog.com)
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