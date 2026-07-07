/***************************************************************************//**
 *   @file   ad9081k.h
 *   @brief  AD9081K (K) no-OS dummy driver header.
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef AD9081K_H_
#define AD9081K_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

struct jesd204_dev;

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* JESD204 link IDs — must match the topology */
#define AD9081K_DEFRAMER0_LINK_TX	0
#define AD9081K_FRAMER0_LINK_RX		2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct ad9081k_dev
 * @brief  AD9081K dummy device structure.
 */
struct ad9081k_dev {
	struct jesd204_dev	*jdev;
	bool			is_initialized;
};

/**
 * @struct ad9081k_init_param
 * @brief  AD9081K dummy initialization parameters.
 */
struct ad9081k_init_param {
	/* placeholder for future parameters */
	uint8_t reserved;
};

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

int ad9081k_init(struct ad9081k_dev **device,
		 const struct ad9081k_init_param *init_param);

int ad9081k_remove(struct ad9081k_dev *dev);

#endif /* AD9081K_H_ */
