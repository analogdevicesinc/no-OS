/***************************************************************************//**
 *   @file   iio_trigger.h
 *   @brief  Header file for iio_trigger.
 *   @author RBolboac(ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef IIO_TRIGGER_H_
#define IIO_TRIGGER_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio.h"
#include "iio_types.h"
#include "no_os_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define TRIG_MAX_NAME_SIZE 20

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @struct iio_hw_trig
 * @brief IIO hardware trigger structure
 */
struct iio_hw_trig {
	/** IIO descriptor */
	struct iio_desc	*iio_desc;
	/** Interrupt descriptor to be linked with the trigger */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** Interrupt id to be linked with the trigger */
	uint32_t irq_id;
	/** Interrupt trigger level */
	enum no_os_irq_trig_level irq_trig_lvl;
	/** Device trigger name */
	char name[TRIG_MAX_NAME_SIZE + 1];
};

/**
 * @struct iio_hw_trig_cb_info
 * @brief Hardware trigger callback extra information structure
 */
struct iio_hw_trig_cb_info {
	/** Platform specific event that triggers the calling of the callback. */
	enum no_os_irq_event event;
	/** Interrupt source peripheral specifier. */
	enum no_os_irq_peripheral peripheral;
	/** This will be used to store HAL specific descriptors */
	void *handle;
};

/**
 * @struct iio_hw_trig_init_param
 * @brief IIO hardware trigger initialization structure
 */
struct iio_hw_trig_init_param {
	/** IIO descriptor */
	struct iio_desc *iio_desc;
	/** Interrupt descriptor to be linked with the trigger */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/** Interrupt id to be linked with the trigger */
	uint32_t irq_id;
	/** Interrupt trigger level */
	enum no_os_irq_trig_level irq_trig_lvl;
	/** Additional interrupt callback information */
	struct iio_hw_trig_cb_info cb_info;
	/** Device trigger name */
	const char *name;
};

/**
 * @struct iio_sw_trig
 * @brief IIO software trigger structure
 */
struct iio_sw_trig {
	/** IIO descriptor */
	struct iio_desc *iio_desc;
	/** Device trigger name */
	char name[TRIG_MAX_NAME_SIZE + 1];
};

/**
 * @struct iio_sw_trig_init_param
 * @brief IIO software trigger initialization structure
 */
struct iio_sw_trig_init_param {
	/** IIO descriptor */
	struct iio_desc *iio_desc;
	/** Device trigger name */
	const char *name;
};

#ifndef LINUX_PLATFORM
/** API to initialize a hardware trigger */
int iio_hw_trig_init(struct iio_hw_trig **iio_trig,
		     struct iio_hw_trig_init_param *init_param);
/** API to enable a hardware  trigger */
int iio_trig_enable(void *trig);
/** API to disable a hardware trigger */
int iio_trig_disable(void *trig);
/** API for hardware trigger handler */
void iio_hw_trig_handler(void *trig);
/** API to remove a hardware trigger */
int iio_hw_trig_remove(struct iio_hw_trig *trig);
#endif

/** API to initialize a software trigger */
int iio_sw_trig_init(struct iio_sw_trig **iio_trig,
		     struct iio_sw_trig_init_param *init_param);
/** API for software trigger handler */
int iio_sw_trig_handler(void *trig, char *buf, uint32_t len,
			const struct iio_ch_info *channel,
			intptr_t priv);
/** API to remove a software trigger */
int iio_trig_remove(struct iio_sw_trig *trig);

#endif /* IIO_TRIGGER_H_ */
