/***************************************************************************//**
 *   @file   relay.h
 *   @brief  Relay interface file.
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
#ifndef __RELAY_H__
#define __RELAY_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

/******************************************************************************/
/************************* Functions Declarations******************************/
/******************************************************************************/
/*! Initialize relay */
int relay_init(struct no_os_gpio_desc **relay);

/*! Open relay cmmand */
int relay_open(struct no_os_gpio_desc *relay);

/*! Close relay command */
int relay_close(struct no_os_gpio_desc *relay);

/*! Relay command line state */
int relay_state(struct no_os_gpio_desc *relay, uint8_t *state);

/*! Get relay status */
uint8_t get_relay_status(void);

/*! Set relay status to CLOSED - 1 */
void set_relay_status(void);

/*! Set relay status to OPENED - 0 */
void reset_relay_status(void);

#endif /* __RELAY_H__ */
