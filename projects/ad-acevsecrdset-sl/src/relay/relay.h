/***************************************************************************//**
 *   @file   relay.h
 *   @brief  Relay interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
