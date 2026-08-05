/***************************************************************************//**
 *   @file   interrupt.h
 *   @brief  Header file of interrupt configuration for eval-ublox-gnss project.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "nmea_ubx.h"

/* Interrupt initialization */
int interrupt_init(struct gnss_dev *gnss_dev);

/* Interrupt removal */
int interrupt_remove(struct gnss_dev *gnss_dev);

/* Get interrupt flag value */
int get_irq_flag_state(void);

/* Interrupt flag state */
void reset_irq_flag_state(void);

#endif /* __INTERRUPT_H__ */
