/***************************************************************************//**
 *   @file   rcd.h
 *   @brief  RCD interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifndef __RCD_H__
#define __RCD_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_gpio.h"
#include "no_os_irq.h"

/******************************************************************************/
/************************* Functions Declarations******************************/
/******************************************************************************/

/*! Get RCD flag value */
int get_rcd_flag_state(void);

/*! Reset RCD flag value */
void reset_rcd_flag_state(void);

/*! Initialize RCD */
int rcd_init(struct no_os_irq_ctrl_desc *desc,
	     struct no_os_gpio_desc **rcm_test, struct no_os_gpio_desc **rcddc,
	     struct no_os_gpio_desc **rcdac);

#endif /* __RCD_H__ */
