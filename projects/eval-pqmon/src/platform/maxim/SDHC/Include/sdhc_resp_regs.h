/**
 * @file    SDHC_resp_regs.h
 * @brief   Registers, Bit Masks and Bit Positions for the response registers
 * from the card.
 */

/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef LIBRARIES_SDHC_INCLUDE_SDHC_RESP_REGS_H_
#define LIBRARIES_SDHC_INCLUDE_SDHC_RESP_REGS_H_

/* **** Includes **** */
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// @cond
/*
    If types are not defined elsewhere (CMSIS) define them here
*/
#ifndef __IO
#define __IO volatile
#endif
#ifndef __I
#define __I volatile const
#endif
#ifndef __O
#define __O volatile
#endif
#ifndef __R
#define __R volatile const
#endif
/// @endcond

/* **** Definitions **** */

/**
 * @ingroup     sdhc_resp
 * @defgroup    sdhc_response_registers Registers
 * @brief       Registers, Bit Masks and Bit Positions for the response
 * registers from the card.
 * @description Card Registers
 */

/**
 * @ingroup sdhc_response_registers
 * Structure type to access the ADC Registers.
 */
typedef struct {
	__IO unsigned int rsv0 : 2;        /**< <tt>\b 0x0000:<\tt> rsv */
	__IO unsigned int file_format : 2; /**< <tt>\b 0x0002:<\tt> File Format */
	__IO unsigned int
	temp_write_protect : 1; /**< <tt>\b 0x0003:<\tt> Temp Write Protect */
	__IO unsigned int perm_write_protect : 1; /**< <tt>\b 0x0004:<\tt>
                                                 Permenant Write Protect */
	__IO unsigned int copy : 1;               /**< <tt>\b 0x0005:<\tt> Copy */
	__IO unsigned int
	file_format_grp : 1;    /**< <tt>\b 0x0006:<\tt> File Format Group */
	__IO unsigned int rsv1 : 5; /**< <tt>\b 0x000B:<\tt> rsv */
	__IO unsigned int
	write_bl_partial : 1; /**< <tt>\b 0x000C:<\tt> Write Block Partial */
	__IO unsigned int
	write_bl_len : 4; /**< <tt>\b 0x0010:<\tt> Write Block Length */
	__IO unsigned int
	r2w_factor : 3; /**< <tt>\b 0x0013:<\tt> Read To Write Factor */
	__IO unsigned int rsv2 : 2;          /**< <tt>\b 0x0015:<\tt> rsv */
	__IO unsigned int wp_grp_enable : 1; /**< <tt>\b 0x0016:<\tt> Write Protect
                                            Group Enable */
	__IO unsigned int
	wp_grp_size : 7; /**< <tt>\b 0x001D:<\tt> Write Protect Group Size */
	__IO unsigned int
	sector_size_0 : 1; /**< <tt>\b 0x001E:<\tt> Sector Size 0 */
	__IO unsigned int
	sector_size_1 : 6; /**< <tt>\b 0x0025:<\tt> Sector Size 1 */
	__IO unsigned int
	erase_blk_en : 1;       /**< <tt>\b 0x0026:<\tt> Erase Block Length */
	__IO unsigned int rsv3 : 1; /**< <tt>\b 0x0027:<\tt> rsv */
	__IO unsigned int c_size : 22; /**< <tt>\b 0x003D:<\tt> Capicity Size */
	__IO unsigned int rsv4 : 2;    /**< <tt>\b 0x003F:<\tt> rsv */
	__IO unsigned int rsv5 : 4;    /**< <tt>\b 0x0044:<\tt> rsv */
	__IO unsigned int dsr_imp : 1; /**< <tt>\b 0x0045:<\tt> DSR_IMP */
	__IO unsigned int
	read_blk_misalign : 1; /**< <tt>\b 0x0046:<\tt> Read Block Misalign */
	__IO unsigned int write_blk_misalign : 1; /**< <tt>\b 0x0047:<\tt> Write
                                                 Block Misalign */
	__IO unsigned int
	read_bl_partial : 1; /**< <tt>\b 0x0048:<\tt> Read Block Partial */
	__IO unsigned int
	read_bl_len : 4;        /**< <tt>\b 0x004C:<\tt> Read Block Length */
	__IO unsigned int ccc : 12; /**< <tt>\b 0x0058:<\tt> CCC */
	__IO unsigned int tran_speed : 8; /**< <tt>\b 0x0064:<\tt> Tran Speed */
	__IO unsigned int nsac : 8;       /**< <tt>\b 0x006C:<\tt> NSAC */
	__IO unsigned int taac : 8;       /**< <tt>\b 0x0074:<\tt> TAAC */
	__IO unsigned int rsv6 : 6;       /**< <tt>\b 0x007A:<\tt> rsv */
	__IO unsigned int
	csd_structure : 2;      /**< <tt>\b 0x007C:<\tt> CSD Structure */
	__IO unsigned int rsv7 : 8; /**< <tt>\b 0x0084:<\tt> rsv */
} csd_structure;

typedef struct {
	union {
		__IO uint32_t array[4];
		__IO csd_structure csd;
	};
} mxc_sdhc_csd_regs_t;

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_SDHC_INCLUDE_SDHC_RESP_REGS_H_
