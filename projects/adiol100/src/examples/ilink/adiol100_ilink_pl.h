/***************************************************************************//**
 *   @file   adiol100_ilink_pl.h
 *   @brief  i-link IO-Link stack port layer for the ADIOL100.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef ADIOL100_ILINK_PL_H
#define ADIOL100_ILINK_PL_H

#include <stdint.h>
#include <stdbool.h>
#include "osal.h"
#include "iolink_pl_hw_drv.h"
#include "adiol100.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

#define ADIOL100_NUM_CHANNELS 2

/* Port layer driver struct. The first member must be iolink_hw_drv_t so the
 * i-link stack can cast between the generic and private types. */
typedef struct iolink_adiol100_drv
{
   iolink_hw_drv_t drv;

   struct adiol100_dev *dev;
   os_mutex_t *exclusive;
   os_event_t *dl_event[ADIOL100_NUM_CHANNELS];
   uint32_t pl_flag;
   bool wurq_request[ADIOL100_NUM_CHANNELS];
   bool is_iolink[ADIOL100_NUM_CHANNELS];

   struct no_os_gpio_desc *irq_gpio_a_desc;
   struct no_os_gpio_desc *irq_gpio_b_desc;
   struct no_os_irq_ctrl_desc *irq_ctrl;
} iolink_adiol100_drv_t;

/* Port layer configuration. Passed to iolink_adiol100_init(). */
typedef struct iolink_adiol100_cfg
{
   /* ADIOL100 driver init parameters (SPI, chip address, clock). */
   struct adiol100_init_param *adiol100_ip;

   /* GPIO for IRQA pin (channel A interrupt, active-low). */
   struct no_os_gpio_init_param *irq_gpio_a;

   /* GPIO for IRQB pin (channel B interrupt, active-low). */
   struct no_os_gpio_init_param *irq_gpio_b;

   /* IRQ controller init parameters. */
   struct no_os_irq_init_param *irq_ip;
} iolink_adiol100_cfg_t;

/** Initialize the ADIOL100 port layer for the i-link stack. */
iolink_hw_drv_t *iolink_adiol100_init(const iolink_adiol100_cfg_t *cfg);

#endif /* ADIOL100_ILINK_PL_H */
