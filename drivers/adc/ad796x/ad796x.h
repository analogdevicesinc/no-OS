/***************************************************************************//**
 *   @file   ad796x.h
 *   @brief  Header file of AD796X Driver.
 *   @author Axel Haslam (ahaslam@baylibre.com)
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
#ifndef __AD796X_H__
#define __AD796X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "axi_dmac.h"
#include "clk_axi_clkgen.h"
#include "axi_adc_core.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
enum ad796x_mode {
	AD796X_MODE0_POWER_DOWN,
	AD796X_MODE1_EXT_REF_5P0,
	AD796X_MODE2_INT_REF_4P0,
	AD796X_MODE3_EXT_REF_4P0,
	AD796X_MODE4_SNOOZE,
	AD796X_MODE5_TEST,
	AD796X_MODE6_INVALID,
	AD796X_MODE7_EXT_REF_5P0_9MHZ,
	AD796X_MODE8_INT_REF_4P0_9MHZ,
	AD796X_MODE9_EXT_REF_4P0_9MHZ,
	AD796X_MODE10_SNOOZE2,
};

#define AD796X_NUM_GPIOS	4

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct ad796x_init_param
 * @brief AD796X Device initialization parameters.
 */
struct ad796x_init_param {
	struct axi_adc_init		*ad796x_core_ip;
	struct axi_clkgen_init		*clkgen_ip;
	struct axi_dmac_init		*dmac_ip;
	struct no_os_pwm_init_param	*axi_pwm_0_ip;
	struct no_os_pwm_init_param	*axi_pwm_1_ip;
	struct no_os_gpio_init_param	*gpio_ip[AD796X_NUM_GPIOS];
	enum ad796x_mode mode;
};

/**
 * @struct ad796x_dev
 * @brief AD796X Device structure.
 */
struct ad796x_dev {
	struct axi_adc			*ad796x_core;
	struct axi_clkgen		*clkgen;
	struct axi_dmac			*axi_dmac;
	struct no_os_pwm_desc		*axi_pwm_0;
	struct no_os_pwm_desc		*axi_pwm_1;
	struct no_os_gpio_desc		*gpio_en[AD796X_NUM_GPIOS];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the device. */
int ad796x_init(struct ad796x_dev **device,
		struct ad796x_init_param *init_param);

/* Remove the device and release resources. */
int ad796x_remove(struct ad796x_dev *dev);

/* Read samples from the device */
int32_t ad796x_read_data(struct ad796x_dev *dev, uint32_t *buf,
			 uint16_t samples);

#endif /* __AD796X_H__ */
